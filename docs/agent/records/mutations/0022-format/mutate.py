#!/usr/bin/env -S python3 -I
"""Mutation runs for the multivector formatter (format.hpp) and the
catch-alls of WU15 part 3 (mv.hpp, linear.hpp, blade_set.hpp).

  mutate.py <preset> [mutant...]

Applies one mutant at a time to its header, builds the test programs of its
group with ninja's -k 0 so that each is tried, runs the CTest labels of its
group (programs and compile-fail tests), and restores the header. Every
build and test goes through tools/timed with the selection mutant:<name>.

A mutant is killed when a test fails: a program that does not compile
because of an error in the test's own code (a static_assert of the test, or
a call it makes that has no overload), or a compile-fail or run-time test
that fails. An error that comes from a warning made an error does not count;
the record lists each compile error's first line so that this can be read
off.

Writes out/<preset>.tsv: mutant, build exit, the programs that failed to
build with the first error line of each, and the failed tests.

Usage, from the root of a worktree that no other build uses, with the job
pool exported. The groups of part 3 are added when its code exists.
"""

import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True).stdout.strip())
CORE = ROOT / "include/galoot/core"
OUT = pathlib.Path(__file__).resolve().parent / "out"

GROUPS = {
    "format": (["core_format_test", "core_format_outside_core_test"],
               r"^core\.format$"),
}

F = "format.hpp"
CANONICAL = "      append(name, canonical_name(blade.mask).text());\n"
CANONICAL_REVERSED = (
    "      {\n"
    "        const blade_name canonical = canonical_name(blade.mask);\n"
    "        const std::string_view t = canonical.text();\n"
    "        append(name, t.substr(0, 1));\n"
    "        for (std::size_t k = t.size(); k > 1; --k) {\n"
    "          append(name, t.substr(k - 1, 1));\n"
    "        }\n"
    "      }\n")
LOOP = "    for (std::size_t i = 0; i < Blades.size(); ++i) {\n"
CANONICAL_ORDER = (
    "    constexpr auto order = [] {\n"
    "      std::array<std::size_t, Blades.size()> o{};\n"
    "      for (std::size_t k = 0; k < o.size(); ++k) {\n"
    "        o[k] = k;\n"
    "      }\n"
    "      for (std::size_t a = 0; a < o.size(); ++a) {\n"
    "        for (std::size_t b = a + 1; b < o.size(); ++b) {\n"
    "          if (galoot::core::canonical_less(Blades.blades[o[b]].mask,\n"
    "                                           Blades.blades[o[a]].mask)) {\n"
    "            const std::size_t s = o[a];\n"
    "            o[a] = o[b];\n"
    "            o[b] = s;\n"
    "          }\n"
    "        }\n"
    "      }\n"
    "      return o;\n"
    "    }();\n"
    "    for (std::size_t j = 0; j < Blades.size(); ++j) {\n"
    "      const std::size_t i = order[j];\n")
MESSAGE_A = ('"format: the scalar type has no std::formatter; "\n'
             '                           "give it a std::formatter, as double has"')
MESSAGE_B = ('"format: names such as e123 have one digit per "\n'
             '                           "index, so a multivector prints by name only for "\n'
             '                           "n up to 9; format its coefficients instead"')
MESSAGE_C = ('"format: an entry of the algebra tag\'s names is "\n'
             '                           "not a blade of the algebra; write 1, or e and "\n'
             '                           "distinct indices from 1 to n, as in e41"')
MESSAGE_D = ('"format: the algebra tag\'s names name one blade "\n'
             '                           "twice, perhaps in both orientations; name each "\n'
             '                           "blade once"')
SWAP = "\x00"

# name: (header, group, [(old, new, occurrences or None for exactly one)])
MUTANTS = {
    # Names.
    "minus_dropped": (F, "format", [('        append(name, "-");\n', "", None)]),
    "tag_name_for_either_orientation": (F, "format", [
        ("parse_name(text, n).blade == blade",
         "parse_name(text, n).blade.mask == blade.mask", None)]),
    "tag_names_ignored": (F, "format", [
        ("if constexpr (declares_names<Alg>) {",
         "if constexpr (false && declares_names<Alg>) {", [1])]),
    "canonical_index_order_reversed": (F, "format", [
        (CANONICAL, CANONICAL_REVERSED, None)]),
    "wide_algebra_printed": (F, "format", [
        ("n > galoot::core::max_compact_generators", "n > 10", None)]),
    # Form.
    "zeros_skipped": (F, "format", [
        ("      for (const char c : std::string_view{i == 0 ? \"\" : \", \"}) {",
         "      if (x.coefficients[i] == T{}) {\n        continue;\n      }\n"
         "      for (const char c : std::string_view{i == 0 ? \"\" : \", \"}) {",
         None)]),
    "canonical_order_for_layout_order": (F, "format", [
        (LOOP, CANONICAL_ORDER, None)]),
    "specification_not_forwarded": (F, "format", [
        ("out = scalar.format(x.coefficients[i], context);",
         "out = std::formatter<T, char>{}.format(x.coefficients[i], context);",
         None)]),
    "colon_changed": (F, "format", [("*out++ = ':';", "*out++ = '=';", None)]),
    "comma_changed": (F, "format", [('i == 0 ? "" : ", "', 'i == 0 ? "" : "; "',
                                     None)]),
    "empty_layout_writes_nothing": (F, "format", [
        ("    auto out = context.out();\n    *out++ = '{';",
         "    auto out = context.out();\n    if (Blades.size() == 0) {\n"
         "      return out;\n    }\n    *out++ = '{';", None)]),
    # Rejections.
    "scalar_rejection_removed": (F, "format", [
        ("static_assert(false,", "static_cast<void>(", [0])]),
    "wide_rejection_removed": (F, "format", [
        ("static_assert(false,", "static_cast<void>(", [1])]),
    "not_a_blade_rejection_removed": (F, "format", [
        ("static_assert(false,", "static_cast<void>(", [2])]),
    "named_twice_rejection_removed": (F, "format", [
        ("static_assert(false,", "static_cast<void>(", [3])]),
    "scalar_and_wide_messages_swapped": (F, "format", [
        (MESSAGE_A, SWAP, None), (MESSAGE_B, MESSAGE_A, None),
        (SWAP, MESSAGE_B, None)]),
    "names_messages_swapped": (F, "format", [
        (MESSAGE_C, SWAP, None), (MESSAGE_D, MESSAGE_C, None),
        (SWAP, MESSAGE_D, None)]),
    "names_checked_as_none": (F, "format", [
        ("    case NameError::no_names:\n      return TagNamesFault::none;\n"
         "    case NameError::repeated_blade:\n      return TagNamesFault::named_twice;\n"
         "    default:\n      return TagNamesFault::not_a_blade;\n",
         "    default:\n      return TagNamesFault::none;\n", None)]),
}


def mutate(text, old, new, occurrences):
    parts = text.split(old)
    if occurrences is None:
        if len(parts) != 2:
            sys.exit(f"anchor occurs {len(parts) - 1} times: {old!r}")
        return new.join(parts)
    if max(occurrences) >= len(parts) - 1:
        sys.exit(f"anchor occurs {len(parts) - 1} times: {old!r}")
    out = parts[0]
    for k, part in enumerate(parts[1:]):
        out += (new if k in occurrences else old) + part
    return out


def timed(step, preset, selection, command):
    return subprocess.run(
        [str(ROOT / "tools/timed"), step, preset, selection, "--", *command],
        cwd=ROOT, capture_output=True, text=True)


def failed_builds(output):
    """The object files that failed and the first error line of each."""
    failures = []
    lines = output.splitlines()
    for k, line in enumerate(lines):
        if line.startswith("FAILED: "):
            source = re.search(r"(\w+)_test\.cpp\.o", line)
            first = next((rest.strip() for rest in lines[k + 1:]
                          if "error:" in rest), "-")
            first = first.replace(f"{ROOT}/", "")[:400]
            failures.append(f"{source.group(1) if source else line}: {first}")
    return failures


def run(preset, name):
    header, group, edits = MUTANTS[name]
    targets, labels = GROUPS[group]
    path = CORE / header
    original = path.read_text()
    text = original
    for old, new, occurrences in edits:
        text = mutate(text, old, new, occurrences)
    if text == original:
        sys.exit(f"{name} changes nothing")
    path.write_text(text)
    try:
        build = timed("build", preset, f"mutant:{name}",
                      ["ninja", "-C", f"build/{preset}", "-k", "0", *targets])
        tests = timed("test", preset, f"mutant:{name}",
                      ["ctest", "--test-dir", f"build/{preset}", "-L", labels])
    finally:
        path.write_text(original)
    failed = re.findall(r"^\s*\d+ - (\S+) \(", tests.stdout, re.MULTILINE)
    return [name, str(build.returncode),
            " | ".join(failed_builds(build.stdout)) or "-",
            " ".join(failed) or "-"]


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    preset = sys.argv[1]
    names = sys.argv[2:] or list(MUTANTS)
    OUT.mkdir(exist_ok=True)
    record = OUT / f"{preset}.tsv"
    # A run of some mutants replaces their rows and keeps the others.
    rows = {}
    if record.exists():
        for line in record.read_text(encoding="utf-8").splitlines()[1:]:
            rows[line.split("\t")[0]] = line.split("\t")
    for name in names:
        rows[name] = run(preset, name)
        print("\t".join(rows[name]), flush=True)
    header = ["mutant", "build_exit", "build_failures", "failed_tests"]
    ordered = [rows[name] for name in MUTANTS if name in rows]
    with open(record, "w", encoding="utf-8") as f:
        f.write("".join("\t".join(r) + "\n" for r in [header, *ordered]))

    # Rebuild the unmutated programs, so that the tree matches the headers.
    targets = sorted({t for n in names for t in GROUPS[MUTANTS[n][1]][0]})
    rebuilt = timed("build", preset, "mutant:restore",
                    ["ninja", "-C", f"build/{preset}", *targets])
    if rebuilt.returncode != 0:
        sys.exit("the restored headers do not build")


if __name__ == "__main__":
    main()
