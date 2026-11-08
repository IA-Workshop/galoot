"""Map every message of the core to the tests that require it.

Usage:
  ctest --test-dir build/<preset> --show-only=json-v1 > <preset>.json
  python3 -I scan.py <core include dir> <name>=<tests.json>... \
      [--check <catalogue.md>]
  python3 -I scan.py --self-test

A message is the literal of a `static_assert`, the message argument of a
`GALOOT_ASSERT`, or the text of an `#error`, in the headers of the core
include directory. Adjacent string literals are joined, as the compiler
joins them.

The tests are read from CTest's JSON listing of a configured build tree, one
listing per compiler, rather than from test/CMakeLists.txt: the listing holds
each test's command with every variable, loop, and compiler branch already
resolved. A test requires a literal when it runs test/expect_output.cmake and
the literal is one that script requires (not the ONLY pattern, not a FORBID
or FORBID_FIRST literal); a test marked WILL_FAIL requires nothing, since it
passes when a literal is missing. expect_output.cmake receives a `;` as the
character 1, so that character is read as `;`. The prefix that the run-time
handler prints before a GALOOT_ASSERT message is removed.

A literal pins a message when one holds the other as a substring and that is
true of no other message, so that the test fails if that message is lost.
When several messages match, a literal that holds one or more of them whole
pins the longest of those: "dot: the operands ..." is also a part of
"antidot: the operands ...", and a test that requires the first exactly pins
it. A literal that is only a part of several messages pins none.

Output: one Markdown row per message with its places and the tests that pin
it in each listing, then the messages a listing does not pin. With --check,
every test name written in backquotes in the catalogue must be a test of
every listing, and the names that are not are printed. Exit status 1 if
--check finds a missing name or --self-test fails, else 0; an unpinned
message is reported, not an error, because some messages are internal
invariants with a stated reason.
"""

import json
import os
import re
import sys

ASSERT_PREFIX = "galoot: assertion failed: "
STRING = re.compile(r'"((?:[^"\\\n]|\\.)*)"')
CALL = re.compile(r"\b(static_assert|GALOOT_ASSERT)\s*\(")
ERROR_DIRECTIVE = re.compile(r'^[ \t]*#[ \t]*error[ \t]+"((?:[^"\\]|\\.)*)"',
                             re.MULTILINE)
TEST_NAME = re.compile(r"`((?:core|rga3|tests|tools|support)\.[A-Za-z0-9_]+)`")


def unescape(text):
    return text.replace('\\"', '"').replace("\\\\", "\\")


def strip_comments(code):
    """Blank // and /* */ comments, keeping string literals and offsets."""
    out = []
    i = 0
    while i < len(code):
        if code.startswith("//", i):
            end = code.find("\n", i)
            end = len(code) if end == -1 else end
            out.append(" " * (end - i))
            i = end
        elif code.startswith("/*", i):
            end = code.find("*/", i) + 2
            out.append(re.sub(r"[^\n]", " ", code[i:end]))
            i = end
        elif code[i] == '"':
            match = STRING.match(code, i)
            end = match.end() if match else i + 1
            out.append(code[i:end])
            i = end
        elif code[i] == "'":
            end = code.find("'", i + 2 if code[i + 1] == "\\" else i + 1) + 1
            out.append(code[i:end])
            i = end
        else:
            out.append(code[i])
            i += 1
    return "".join(out)


def closing(code, start):
    """The index just past the ) that closes the ( before start."""
    depth = 1
    i = start
    while depth:
        if code[i] == '"':
            i = STRING.match(code, i).end()
            continue
        depth += {"(": 1, ")": -1}.get(code[i], 0)
        i += 1
    return i


def message_of(arguments):
    """The joined literals at the end of an argument list, or None."""
    matches = list(STRING.finditer(arguments))
    if not matches:
        return None
    run = [matches[-1]]
    for match in reversed(matches[:-1]):
        if arguments[match.end():run[0].start()].strip():
            break
        run.insert(0, match)
    if arguments[run[-1].end():].strip(" \t\n)"):
        return None
    return "".join(unescape(m.group(1)) for m in run)


def read_messages(include_dir):
    """{message: [place, ...]}, and the places of assertions without one."""
    messages = {}
    silent = []
    for name in sorted(os.listdir(include_dir)):
        if not name.endswith(".hpp"):
            continue
        path = os.path.join(include_dir, name)
        with open(path, encoding="utf-8") as f:
            code = f.read()
        bare = strip_comments(code)
        for match in CALL.finditer(bare):
            line = bare.count("\n", 0, match.start()) + 1
            text_start = bare.rfind("\n", 0, match.start()) + 1
            if bare[text_start:match.start()].lstrip().startswith("#define"):
                continue
            end = closing(bare, match.end())
            message = message_of(bare[match.end():end - 1])
            place = f"{name}:{line}"
            if message is None:
                silent.append(place)
            else:
                messages.setdefault(message, []).append(place)
        for match in ERROR_DIRECTIVE.finditer(bare):
            line = bare.count("\n", 0, match.start()) + 1
            messages.setdefault(unescape(match.group(1)), []).append(
                f"{name}:{line}")
    return messages, silent


def required_literals(test):
    """The literals a test requires of its output, or [] if none."""
    command = test.get("command") or []
    properties = {p["name"]: p["value"] for p in test.get("properties", [])}
    if properties.get("WILL_FAIL"):
        return []
    if len(command) < 5 or not command[2].endswith("expect_output.cmake"):
        return []
    literals = []
    mode = "require"
    i = 5  # cmake -P <script> -- <result>
    while i < len(command) and command[i] != "--":
        argument = command[i]
        if argument == "ONLY":
            i += 2
            continue
        if argument in ("FORBID", "FORBID_FIRST"):
            mode = "forbid"
        elif mode == "require":
            literals.append(argument.replace("\x01", ";"))
        i += 1
    return literals


def pins(literal, messages):
    """The one message the literal pins, or None."""
    if literal.startswith(ASSERT_PREFIX):
        literal = literal[len(ASSERT_PREFIX):]
    if not literal:
        return None
    found = [m for m in messages if literal in m or m in literal]
    if len(found) == 1:
        return found[0]
    # "dot: ..." is also inside "antidot: ...": a literal that equals one
    # message, or holds it whole, pins the longest such message.
    whole = [m for m in found if m in literal]
    return max(whole, key=len) if whole else None


def read_tests(path):
    with open(path, encoding="utf-8") as f:
        return json.load(f)["tests"]


def scan(include_dir, listings):
    """{message: {listing: sorted test names}}, places, and silent places."""
    messages, silent = read_messages(include_dir)
    pinned = {m: {name: set() for name, _ in listings} for m in messages}
    for name, tests in listings:
        for test in tests:
            for literal in required_literals(test):
                message = pins(literal, messages)
                if message is not None:
                    pinned[message][name].add(test["name"])
    return messages, pinned, silent


def report(messages, pinned, silent, names):
    lines = ["| Message | Places | " + " | ".join(names) + " |",
             "| --- | --- | " + " | ".join("---" for _ in names) + " |"]
    for message in sorted(messages, key=lambda m: messages[m][0]):
        cells = []
        for name in names:
            tests = sorted(pinned[message][name])
            cells.append(", ".join(f"`{t}`" for t in tests) or "none")
        text = message.replace("|", "\\|")
        lines.append(f"| {text} | {', '.join(messages[message])} | "
                     + " | ".join(cells) + " |")
    for name in names:
        missing = [m for m in messages if not pinned[m][name]]
        lines.append("")
        lines.append(f"Not pinned in {name}: {len(missing)} of "
                     f"{len(messages)} messages")
        for message in sorted(missing, key=lambda m: messages[m][0]):
            lines.append(f"- {', '.join(messages[message])}: {message}")
    lines.append("")
    lines.append(f"Assertions without a message: {len(silent)}")
    lines.append("- " + ", ".join(silent))
    return "\n".join(lines)


def check(catalogue, listings):
    with open(catalogue, encoding="utf-8") as f:
        written = sorted(set(TEST_NAME.findall(f.read())))
    absent = []
    for name, tests in listings:
        known = {t["name"] for t in tests}
        absent += [f"{name}: {t}" for t in written if t not in known]
    return written, absent


def self_test():
    """Runs the scan on the fixtures beside this script."""
    here = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                        "fixtures")
    listings = [("fixture", read_tests(os.path.join(here, "tests.json")))]
    messages, pinned, silent = scan(os.path.join(here, "include"), listings)
    got = {m: sorted(pinned[m]["fixture"]) for m in messages}
    expected = {
        "first: required in the second place": ["t.second_place"],
        "second: holds; a semicolon": ["t.semicolon"],
        "third: only forbidden": [],
        "fourth: only in a test that is meant to fail": [],
        "fifth: run time": ["t.run_time"],
        "sixth: joined from two literals": ["t.joined"],
        "seventh: shares a prefix": [],
        "seventh: shares a prefix too": [],
        "the directive": ["t.directive"],
        "eighth: cut short at run time": ["t.run_time_prefix"],
        "dot: nested": ["t.nested"],
        "antidot: nested": ["t.nested_long"],
    }
    failures = []
    for message in sorted(set(expected) | set(got)):
        if got.get(message) != expected.get(message):
            failures.append(f"{message}: expected {expected.get(message)}, "
                            f"got {got.get(message)}")
    if silent != ["fixture.hpp:16"]:
        failures.append(f"assertions without a message: {silent}")
    for failure in failures:
        print(f"self-test: {failure}")
    return 1 if failures else 0


def main(argv):
    if argv == ["--self-test"]:
        return self_test()
    catalogue = None
    if "--check" in argv:
        at = argv.index("--check")
        catalogue = argv[at + 1]
        argv = argv[:at] + argv[at + 2:]
    if len(argv) < 2 or not all("=" in a for a in argv[1:]):
        print(__doc__.split("\n\n")[1], file=sys.stderr)
        return 2
    listings = []
    for argument in argv[1:]:
        name, path = argument.split("=", 1)
        listings.append((name, read_tests(path)))
    messages, pinned, silent = scan(argv[0], listings)
    print(report(messages, pinned, silent, [n for n, _ in listings]))
    if catalogue is None:
        return 0
    written, absent = check(catalogue, listings)
    print("")
    print(f"Test names in {os.path.basename(catalogue)}: {len(written)}, "
          f"absent from a listing: {len(absent)}")
    for line in absent:
        print(f"- {line}")
    return 1 if absent else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
