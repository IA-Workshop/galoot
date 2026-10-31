"""Check that every test case states its reference, and trace conventions to
the test cases that pin them.

Usage:
  python3 -I tools/test_references.py markers <path>...
  python3 -I tools/test_references.py trace <table> <conventions> <golden.hpp>
      <path>...

A <path> that is a directory is searched for files named *_test.cpp, which are
the only files the build compiles into test programs; a <path> that is a file
is read as given. Each command prints one line per finding, as
<file>:<line>: <finding>, and exits 1 if there is any, else 0.

The reference marker
--------------------
Every TEST_CASE is preceded, with no blank line between, by a comment block
that starts with `// Reference:`. Lines after the first continue it. The text
after `Reference:` is a list of sources separated by `;`, each of the form
`<kind>: <text>`, where the kind says what sort of evidence the test is and
the text states the mathematical fact, equation, or documented behavior it
rests on, in words a reader understands without other documents:

  proof              a derivation, or an exhaustive check over a finite set
  identity           an identity the structure satisfies for all inputs
  independent value  an exact value computed apart from the library, such as
                     the golden values in test/reference/golden.hpp
  comparison         a black-box comparison with another implementation
  contract           a behavior stated where it is declared, for tests of
                     the build and the tools
  regression guard   the test restates current output and is no evidence

A regression guard is the only source of its marker, and a test case whose
name contains "regression" carries one. The text cites only published
sources, never a numbered rule or a project record such as a decision, plan,
or review: the reader is told the reason, not where it was written down.

Traceability
------------
`trace` reads <table>, a Markdown table with the columns Item, Fact, and
Golden values: a conventions item, a fact of it in the words a test's
marker states it, and the namespaces of golden.hpp that hold values of that
fact. It checks that every item in the table exists in <conventions>, that
the namespaces in the table are exactly those of <golden.hpp>, and that a
test case that uses a golden namespace (`<namespace>::` between its TEST_CASE
line and the next) states that namespace's fact in its marker. It then prints
each item with the test cases whose markers state one of its facts, and the
items of <conventions> that no test case pins. A use of golden values in a
helper outside the test case is not seen.
"""

import os
import re
import sys

TEST_CASE = re.compile(r"^\s*TEST_CASE\w*\s*\(")
STRING = re.compile(r'"((?:[^"\\]|\\.)*)"')
KINDS = ("proof", "identity", "independent value", "comparison", "contract",
         "regression guard")
SOURCE = re.compile(rf"^({'|'.join(KINDS)}): \S")
# A pointer to a project record or a numbered rule instead of a reason.
POINTER = re.compile(
    r"(?i:\b(?:decisions?|plans?|research notes?|notes?|reviews?|lessons?)"
    r" \d{4}\b|\bspikes? [A-F]\b|\bworkbench\b|docs/agent\b"
    r"|\b(?:Claude|Opus|Sonnet|Haiku|Fable)\b)"
    r"|\bCONVENTIONS\b|\b[CWSPV]\d{1,2}\b|\bDoD-\d+\b")
CONVENTIONS_ITEM = re.compile(r"^(\d+\.\d+) ")
TABLE_ROW = re.compile(r"^\|\s*(\d+\.\d+)\s*\|([^|]*)\|([^|]*)\|\s*$")
GOLDEN_NAMESPACE = re.compile(r"^namespace (\w+) \{")


class TestCase:
    """A TEST_CASE: where it is, its name, its marker, and its body up to the
    next TEST_CASE."""

    def __init__(self, path, line, name, marker_line, marker, body):
        self.path = path
        self.line = line
        self.name = name
        self.marker_line = marker_line  # None when there is no marker
        self.marker = marker
        self.body = body

    def where(self):
        return f"{self.path}:{self.line}"

    def marker_where(self):
        return f"{self.path}:{self.marker_line}"


def normalize(text):
    return " ".join(text.split())


def test_files(paths):
    files = []
    for path in paths:
        if os.path.isdir(path):
            for directory, _, names in os.walk(path):
                files += [os.path.join(directory, n) for n in names
                          if n.endswith("_test.cpp")]
        else:
            files.append(path)
    return sorted(files)


def read_test_cases(path):
    with open(path, encoding="utf-8") as f:
        lines = f.read().splitlines()
    starts = [i for i, line in enumerate(lines) if TEST_CASE.match(line)]
    cases = []
    for k, i in enumerate(starts):
        end = starts[k + 1] if k + 1 < len(starts) else len(lines)

        # The name is the string literals up to the line that opens the body.
        head = []
        for line in lines[i:end]:
            head.append(line)
            if "{" in line:
                break
        name = "".join(STRING.findall(" ".join(head)))

        top = i
        while top > 0 and lines[top - 1].lstrip().startswith("//"):
            top -= 1
        block = [line.lstrip()[2:].strip() for line in lines[top:i]]
        marker_line, marker = None, None
        if block and block[0].startswith("Reference:"):
            marker_line = top + 1
            marker = normalize(" ".join([block[0][len("Reference:"):]] +
                                        block[1:]))
        cases.append(TestCase(path, i + 1, name, marker_line, marker,
                              "\n".join(lines[i:end])))
    return cases


def sources(marker):
    return [s.strip() for s in marker.split(";") if s.strip()]


def check_markers(cases):
    findings = []
    for case in cases:
        if case.marker is None:
            findings.append(f"{case.where()}: TEST_CASE has no reference "
                            "marker: a comment block starting "
                            "'// Reference:' directly above it")
            continue
        at = case.marker_where()
        listed = sources(case.marker)
        kinds = []
        for source in listed:
            match = SOURCE.match(source)
            if match:
                kinds.append(match.group(1))
            else:
                findings.append(f"{at}: reference source is not "
                                f"'<kind>: <text>' with a kind of "
                                f"{', '.join(KINDS)}: '{source}'")
        if not listed:
            findings.append(f"{at}: reference marker states no source")
        if "regression guard" in kinds and len(listed) > 1:
            findings.append(f"{at}: a regression guard is the only source of "
                            "its reference")
        if ("regression" in case.name.lower() and
                "regression guard" not in kinds):
            findings.append(f"{case.where()}: test case named as a regression "
                            "test is not labeled 'regression guard:'")
        pointer = POINTER.search(case.marker)
        if pointer:
            findings.append(f"{at}: reference points to a record or a "
                            f"numbered rule instead of stating the reason: "
                            f"'{pointer.group(0)}'")
    return findings


def read_lines(path):
    with open(path, encoding="utf-8") as f:
        return f.read().splitlines()


def read_table(path):
    """Rows of (line, item, fact, namespaces) from the Markdown table."""
    rows = []
    for number, line in enumerate(read_lines(path), 1):
        match = TABLE_ROW.match(line)
        if match:
            item, fact, golden = match.groups()
            namespaces = re.findall(r"`(\w+)`", golden)
            rows.append((number, item, normalize(fact), namespaces))
    return rows


def trace(table_path, conventions_path, golden_path, cases):
    findings = []
    items = [m.group(1) for line in read_lines(conventions_path)
             if (m := CONVENTIONS_ITEM.match(line))]
    golden = [m.group(1) for line in read_lines(golden_path)
              if (m := GOLDEN_NAMESPACE.match(line)) and m.group(1) != "golden"]
    rows = read_table(table_path)
    if not rows:
        findings.append(f"{table_path}: no rows of the form "
                        "| item | fact | golden values |")

    fact_of = {}
    for number, item, fact, namespaces in rows:
        at = f"{table_path}:{number}"
        if item not in items:
            findings.append(f"{at}: {conventions_path} has no item {item}")
        if not fact:
            findings.append(f"{at}: item {item} has an empty fact")
        for namespace in namespaces:
            if namespace in fact_of:
                findings.append(f"{at}: golden namespace {namespace} is "
                                "listed twice")
            elif namespace not in golden:
                findings.append(f"{at}: {golden_path} has no namespace "
                                f"{namespace}")
            fact_of[namespace] = fact
    for namespace in golden:
        if namespace not in fact_of:
            findings.append(f"{golden_path}: namespace {namespace} is not in "
                            f"{table_path}")

    for case in cases:
        marker = case.marker or ""
        for namespace, fact in sorted(fact_of.items()):
            if (re.search(rf"\b{namespace}::", case.body) and
                    fact not in marker):
                findings.append(f"{case.where()}: uses golden values of "
                                f"{namespace} but its reference does not "
                                f"state '{fact}'")

    pinned_by = {item: [] for item in items}
    for _, item, fact, _ in rows:
        if item not in pinned_by or not fact:
            continue
        for case in cases:
            if (case.marker and fact in case.marker and
                    case.where() not in pinned_by[item]):
                pinned_by[item].append(case.where())
    return findings, pinned_by


def main(argv):
    if len(argv) >= 2 and argv[0] == "markers":
        command, paths = "markers", argv[1:]
    elif len(argv) >= 5 and argv[0] == "trace":
        command, paths = "trace", argv[4:]
    else:
        sys.exit("usage: test_references.py markers <path>...\n"
                 "       test_references.py trace <table> <conventions> "
                 "<golden.hpp> <path>...")

    cases = [c for f in test_files(paths) for c in read_test_cases(f)]
    if not cases:
        print(f"no test cases found under {' '.join(paths)}")
        return 1

    if command == "markers":
        findings = check_markers(cases)
        for finding in findings:
            print(finding)
        if not findings:
            print(f"{len(cases)} test cases, each with a reference marker")
        return 1 if findings else 0

    findings, pinned_by = trace(*argv[1:4], cases)
    for finding in findings:
        print(finding)
    for item, where in pinned_by.items():
        if where:
            print(f"{item}: {', '.join(where)}")
    untested = [item for item, where in pinned_by.items() if not where]
    print(f"items with no test: {', '.join(untested) or 'none'}")
    return 1 if findings else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
