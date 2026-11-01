"""Check that test cases state their references, that tests using golden
values state the facts those values pin, and that source files state reasons
rather than cite project documents; and trace a specification's items to the
tests that pin them.

Usage:
  python3 -I tools/test_references.py markers <path>...
  python3 -I tools/test_references.py golden <golden.hpp> <path>...
  python3 -I tools/test_references.py trace <table> <specification>
      <golden.hpp> <path>...
  python3 -I tools/test_references.py citations [--exclude <path>]...
      <path>...

For the first three commands, a <path> that is a directory is searched for
files named *_test.cpp, which are the only files the build compiles into test
programs, and for `golden` and `trace` also for headers (*.hpp, *.h), which
test programs include; a <path> that is a file is read as given. Each command
prints one line per finding, as <file>:<line>: <finding>, and exits 1 if there
is any, else 0.

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
sources, never a project document, a numbered rule of one, or a project
record such as a decision, plan, or review: the reader is told the reason, not
where it was written down. The forms rejected are the patterns of
tools/citation_patterns.txt, which `citations` also uses.

Golden values
-------------
`golden` reads the namespaces of <golden.hpp>. The comment above each one
ends with a line `// Fact: <fact>`, the fact its values pin. A test case that
uses a namespace states its fact in its marker. So does a static_assert
outside any test case, which carries a marker of the same form directly above
it. A use is `golden::<namespace>`, or a namespace alias of one, in code (not
in comments or strings). `using` declarations and directives that name golden
values are rejected, because they hide which test uses which values, and so is
a use anywhere else, such as in a helper function or a helper header: pass
the values to the helper from the test case instead. A fact is matched
ignoring case and runs of whitespace, with the wedge sign read as ^ and the
minus sign as -.

Citations
---------
`citations` reads every file under each <path>, other than the excluded
files and directories, and reports each match of the patterns in
tools/citation_patterns.txt, which lists them with a comment for each: the
names of the project's governance documents and their rule numbers, numbered
project records and the directory that holds them, units of work, review
findings, and the names of agent models and agents. The
patterns are matched across line breaks, with each line's indentation and
comment leader removed, so a citation split over two comment lines is
found. Code, comments, test names, messages, build files, and scripts are
written for a reader who has none of those records, so each states its
reason in place and cites only published sources. Exclude only files that
hold such text on purpose, such as the rejecting fixtures of this command.

Traceability
------------
`trace` reads <table>, a Markdown table with the columns Item, Fact, and
Implemented (yes or no): a specification item, a fact of it, and whether the
library implements it. An item may have several rows, one per fact; a fact
may not contain `|`. It checks that the table's items and the items of
<specification> (lines that start with the item number, such as `3.1 `) are
the same, that every line that starts a row has exactly three cells, and that
every fact of <golden.hpp> is in the table. A test case or marked
static_assert pins a row when its marker states the row's fact. The command
prints each item with what pins any of its rows, and fails if a row marked
implemented has no test that states its fact.
"""

import bisect
import os
import re
import sys

TEST_CASE = re.compile(r"^[ \t]*TEST_CASE\w*\s*\(", re.MULTILINE)
STATIC_ASSERT = re.compile(r"\bstatic_assert\s*\(")
STRING = re.compile(r'"((?:[^"\\]|\\.)*)"')
KINDS = ("proof", "identity", "independent value", "comparison", "contract",
         "regression guard")
SOURCE = re.compile(rf"^({'|'.join(KINDS)}): \S")
# Text that points to a project document, rule, or record instead of stating
# a reason, one regular expression per line of the pattern file; earlier
# lines win where two match at one place.
PATTERNS = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                        "citation_patterns.txt")


def read_patterns(path):
    with open(path, encoding="utf-8") as f:
        lines = [line.strip() for line in f]
    return re.compile("|".join(f"(?:{line})" for line in lines
                               if line and not line.startswith("#")))


POINTER = read_patterns(PATTERNS)
# Indentation and a comment leader at the start of a line: //, ///, #, /*,
# or the * that continues a block comment.
LINE_LEADER = re.compile(r"^\s*(?:///?|#+|/\*+|\*+/?)?\s*")
FACT_LINE = re.compile(r"^// Fact:(.*)$")
NAMESPACE = re.compile(r"^namespace (\w+) \{")
GOLDEN_USE = re.compile(r"(?<![\w:])(?:::)?golden::(\w+)")
ALIAS = re.compile(r"\bnamespace\s+(\w+)\s*=\s*(?:::)?golden(?:::(\w+))?\s*;")
USING = re.compile(r"\busing\s+(?:namespace\s+)?(?:::)?golden\b")
SPEC_ITEM = re.compile(r"^(\d+\.\d+) ")
ITEM = re.compile(r"\d+\.\d+")


def normalize(text):
    text = text.replace("\N{LOGICAL AND}", "^").replace("\N{MINUS SIGN}", "-")
    return " ".join(text.lower().split())


def code_only(text):
    """The text with every comment and every string or character literal
    replaced by spaces, keeping newlines, so offsets and lines still match."""
    out = list(text)
    i, n = 0, len(text)

    def blank(start, end):
        for k in range(start, end):
            if out[k] != "\n":
                out[k] = " "

    while i < n:
        if text.startswith("//", i):
            end = text.find("\n", i)
            end = n if end < 0 else end
        elif text.startswith("/*", i):
            end = text.find("*/", i + 2)
            end = n if end < 0 else end + 2
        elif text.startswith('R"', i) and (i == 0 or not
                                            text[i - 1].isalnum()):
            open_paren = text.find("(", i)
            delimiter = text[i + 2:open_paren]
            end = text.find(f"){delimiter}\"", open_paren)
            end = n if end < 0 else end + len(delimiter) + 2
        elif text[i] in "\"'" and not (text[i] == "'" and i > 0 and
                                       text[i - 1].isalnum()):
            # A ' after a digit or letter is a digit separator, as in 1'000.
            quote, end = text[i], i + 1
            while end < n and text[end] != quote:
                end += 2 if text[end] == "\\" else 1
            end += 1
        else:
            i += 1
            continue
        blank(i, end)
        i = end
    return "".join(out)


def matching(code, open_at, opener, closer):
    """The offset just past the bracket that closes code[open_at]."""
    depth = 0
    for k in range(open_at, len(code)):
        if code[k] == opener:
            depth += 1
        elif code[k] == closer:
            depth -= 1
            if depth == 0:
                return k + 1
    return len(code)


class Check:
    """A TEST_CASE, or a static_assert outside test cases: where it is, its
    name (empty for a static_assert), its marker, and its code."""

    def __init__(self, kind, path, line, name, marker_line, marker, code):
        self.kind = kind
        self.path = path
        self.line = line
        self.name = name
        self.marker_line = marker_line  # None when there is no marker
        self.marker = marker
        self.code = code

    def where(self):
        return f"{self.path}:{self.line}"

    def marker_where(self):
        return f"{self.path}:{self.marker_line}"


def marker_above(lines, index):
    """The marker in the comment block directly above lines[index], as
    (line number, text), or (None, None)."""
    top = index
    while top > 0 and lines[top - 1].lstrip().startswith("//"):
        top -= 1
    block = [line.lstrip()[2:].strip() for line in lines[top:index]]
    if not block or not block[0].startswith("Reference:"):
        return None, None
    text = " ".join([block[0][len("Reference:"):]] + block[1:])
    return top + 1, " ".join(text.split())


class Source:
    """One test file, split into its test cases, its static_asserts outside
    test cases, and the rest of its code."""

    def __init__(self, path):
        self.path = path
        with open(path, encoding="utf-8") as f:
            self.text = f.read()
        self.code = code_only(self.text)
        self.lines = self.text.splitlines()
        self.test_cases = []
        self.static_asserts = []
        covered = []

        for match in TEST_CASE.finditer(self.code):
            start = match.start()
            args_end = matching(self.code, match.end() - 1, "(", ")")
            body = self.code.find("{", args_end)
            end = len(self.code) if body < 0 else \
                matching(self.code, body, "{", "}")
            name = "".join(STRING.findall(self.text[start:args_end]))
            self.test_cases.append(self.check("TEST_CASE", start, end, name))
            covered.append((start, end))

        for match in STATIC_ASSERT.finditer(self.code):
            start = match.start()
            if any(a <= start < b for a, b in covered):
                continue
            end = matching(self.code, match.end() - 1, "(", ")")
            self.static_asserts.append(
                self.check("static_assert", start, end, ""))
            covered.append((start, end))

        rest = list(self.code)
        for a, b in covered:
            rest[a:b] = [" "] * (b - a)
        self.rest = "".join(rest)

    def line_of(self, offset):
        return self.code.count("\n", 0, offset) + 1

    def check(self, kind, start, end, name):
        line = self.line_of(start)
        marker_line, marker = marker_above(self.lines, line - 1)
        return Check(kind, self.path, line, name, marker_line, marker,
                     self.code[start:end])


def read_sources(paths, suffixes=("_test.cpp",)):
    """The sources at paths: each file given, and in each directory given,
    every file whose name ends with one of the suffixes."""
    files = []
    for path in paths:
        if os.path.isdir(path):
            for directory, _, names in os.walk(path):
                files += [os.path.join(directory, n) for n in names
                          if n.endswith(suffixes)]
        else:
            files.append(path)
    return [Source(f) for f in sorted(files)]


def marker_findings(check):
    if check.marker is None:
        return [f"{check.where()}: {check.kind} has no reference marker: a "
                "comment "
                "block starting '// Reference:' directly above it"]
    findings = []
    at = check.marker_where()
    listed = [s.strip() for s in check.marker.split(";") if s.strip()]
    kinds = []
    for source in listed:
        match = SOURCE.match(source)
        if match:
            kinds.append(match.group(1))
        else:
            findings.append(f"{at}: reference source is not '<kind>: <text>' "
                            f"with a kind of {', '.join(KINDS)}: '{source}'")
    if not listed:
        findings.append(f"{at}: reference marker states no source")
    if "regression guard" in kinds and len(listed) > 1:
        findings.append(f"{at}: a regression guard is the only source of its "
                        "reference")
    if "regression" in check.name.lower() and "regression guard" not in kinds:
        findings.append(f"{check.where()}: test case named as a regression "
                        "test is not labeled 'regression guard:'")
    pointer = POINTER.search(check.marker)
    if pointer:
        findings.append(f"{at}: reference points to a document or a record "
                        f"instead of stating the reason: '{pointer.group(0)}'")
    return findings


def citation_files(paths, excluded):
    """Every file at paths, directories searched recursively, except the
    excluded files and directories and Python's __pycache__ directories."""
    excluded = [os.path.realpath(p) for p in excluded]

    def skipped(path):
        real = os.path.realpath(path)
        return any(real == e or real.startswith(e + os.sep) for e in excluded)

    files = []
    for path in paths:
        if skipped(path):
            continue
        if not os.path.isdir(path):
            files.append(path)
            continue
        for directory, subdirectories, names in os.walk(path):
            subdirectories[:] = sorted(
                d for d in subdirectories if d != "__pycache__"
                and not skipped(os.path.join(directory, d)))
            files += [os.path.join(directory, n) for n in sorted(names)
                      if not skipped(os.path.join(directory, n))]
    return files


def citation_findings(path):
    """Each citation in the file. The lines are searched with their
    indentation and comment leaders removed and joined by spaces, so that a
    citation broken across two comment lines is found too."""
    try:
        with open(path, encoding="utf-8") as f:
            lines = f.read().splitlines()
    except UnicodeDecodeError:
        return [f"{path}: not UTF-8 text, so it cannot be checked"]
    starts, parts, offset = [], [], 0
    for line in lines:
        text = LINE_LEADER.sub("", line, count=1)
        starts.append(offset)
        parts.append(text)
        offset += len(text) + 1
    return [f"{path}:{bisect.bisect_right(starts, m.start())}: cites a "
            "project document or record instead of stating the reason: "
            f"'{' '.join(m.group(0).split())}'"
            for m in POINTER.finditer(" ".join(parts))]


def read_golden(path):
    """The fact of each namespace of the golden header, and the findings."""
    with open(path, encoding="utf-8") as f:
        lines = f.read().splitlines()
    facts, findings = {}, []
    for number, line in enumerate(lines, 1):
        match = NAMESPACE.match(line)
        if not match or match.group(1) == "golden":
            continue
        name = match.group(1)
        fact_line = FACT_LINE.match(lines[number - 2]) if number > 1 else None
        fact = fact_line.group(1).strip() if fact_line else ""
        if name in facts:
            findings.append(f"{path}:{number}: namespace {name} is defined "
                            "twice")
        elif not fact:
            findings.append(f"{path}:{number}: namespace {name} states no "
                            "fact: its comment ends with no '// Fact:' line")
        else:
            facts[name] = fact
    if not facts:
        findings.append(f"{path}: no namespace with a fact")
    return facts, findings


def golden_uses(code, aliases, facts):
    """Each golden namespace with a fact that the code uses, directly or
    through one of the aliases, with the offset of its first use."""
    used = {}
    for m in GOLDEN_USE.finditer(code):
        used.setdefault(m.group(1), m.start())
    for alias, target in aliases.items():
        pattern = rf"(?<![\w:]){alias}::" + ("" if target else r"(\w+)")
        for m in re.finditer(pattern, code):
            used.setdefault(target or m.group(1), m.start())
    return {name: at for name, at in used.items() if name in facts}


def golden_findings(golden_path, sources):
    facts, findings = read_golden(golden_path)
    for source in sources:
        aliases = {}
        for match in ALIAS.finditer(source.code):
            aliases[match.group(1)] = match.group(2)  # None for golden itself

        for match in USING.finditer(source.code):
            findings.append(f"{source.path}:{source.line_of(match.start())}: "
                            "'using' hides which test uses golden values; "
                            "name them as golden::<namespace> or through a "
                            "namespace alias")

        for check in source.test_cases + source.static_asserts:
            used = golden_uses(check.code, aliases, facts)
            if not used:
                continue
            if check.kind == "static_assert":
                findings += marker_findings(check)
            marker = normalize(check.marker or "")
            for name in sorted(used):
                if normalize(facts[name]) not in marker:
                    findings.append(f"{check.where()}: uses golden values of "
                                    f"{name} but its reference does not state "
                                    f"'{facts[name]}'")

        rest = ALIAS.sub(lambda m: " " * len(m.group(0)), source.rest)
        for name, at in sorted(golden_uses(rest, aliases, facts).items()):
            findings.append(f"{source.path}:{source.line_of(at)}: "
                            f"golden values of {name} are used outside a test "
                            "case or a static_assert; pass them in from the "
                            "test case")
    return findings


def read_table(path):
    """The rows of the table, as (line, item, fact, implemented), and a
    finding for each line that starts a row but is not one of three cells."""
    rows, findings = [], []
    with open(path, encoding="utf-8") as f:
        for number, line in enumerate(f.read().splitlines(), 1):
            text = line.strip()
            if not text.startswith("|"):
                continue
            cells = [c.strip() for c in text[1:].split("|")]
            if not ITEM.fullmatch(cells[0]):
                continue  # the header and the separator line
            if not text.endswith("|") or len(cells) != 4:
                findings.append(f"{path}:{number}: row of item {cells[0]} is "
                                "not three cells between | signs; a fact may "
                                "not contain |")
                continue
            rows.append((number, *cells[:3]))
    return rows, findings


def trace(table, specification, golden_path, sources):
    with open(specification, encoding="utf-8") as f:
        items = [m.group(1) for line in f if (m := SPEC_ITEM.match(line))]
    facts, findings = read_golden(golden_path)
    rows, row_findings = read_table(table)
    findings += row_findings
    if not rows:
        findings.append(f"{table}: no rows of the form "
                        "| item | fact | implemented |")

    table_facts = set()
    for number, item, fact, implemented in rows:
        at = f"{table}:{number}"
        if item not in items:
            findings.append(f"{at}: {specification} has no item {item}")
        if not fact:
            findings.append(f"{at}: item {item} has an empty fact")
        if implemented not in ("yes", "no"):
            findings.append(f"{at}: Implemented is '{implemented}', not yes "
                            "or no")
        table_facts.add(normalize(fact))
    table_items = {item for _, item, _, _ in rows}
    for item in items:
        if item not in table_items:
            findings.append(f"{table}: item {item} of {specification} has no "
                            "row")
    for name, fact in facts.items():
        if normalize(fact) not in table_facts:
            findings.append(f"{golden_path}: the fact of namespace {name}, "
                            f"'{fact}', is not in {table}")

    # Each row is pinned by the checks that state its own fact, so a row
    # marked implemented is not satisfied by a test of another row's fact.
    checks = [c for s in sources for c in s.test_cases + s.static_asserts
              if c.marker]
    pinned_by = {item: [] for item in items}
    for number, item, fact, implemented in rows:
        if item not in pinned_by or not fact:
            continue
        stating = [c.where() for c in checks
                   if normalize(fact) in normalize(c.marker)]
        pinned_by[item] += [w for w in stating if w not in pinned_by[item]]
        if implemented == "yes" and not stating:
            findings.append(f"{table}:{number}: item {item} is implemented "
                            "and no test states its fact")
    return findings, pinned_by


def citations(argv):
    paths, excluded = [], []
    args = iter(argv)
    for arg in args:
        if arg == "--exclude":
            excluded.append(next(args, ""))
        else:
            paths.append(arg)
    files = citation_files(paths, excluded)
    if not files:
        print(f"no files found under {' '.join(paths)}")
        return 1
    findings = [f for path in files for f in citation_findings(path)]
    if not findings:
        print(f"{len(files)} files, none citing a project document or record")
    for finding in findings:
        print(finding)
    return 1 if findings else 0


def main(argv):
    commands = {"markers": 2, "golden": 3, "trace": 5, "citations": 2}
    if not argv or argv[0] not in commands or len(argv) < commands[argv[0]]:
        sys.exit("usage: test_references.py markers <path>...\n"
                 "       test_references.py golden <golden.hpp> <path>...\n"
                 "       test_references.py trace <table> <specification> "
                 "<golden.hpp> <path>...\n"
                 "       test_references.py citations [--exclude <path>]... "
                 "<path>...")
    command = argv[0]
    if command == "citations":
        return citations(argv[1:])
    paths = argv[commands[command] - 1:]
    suffixes = ("_test.cpp",) if command == "markers" else \
        ("_test.cpp", ".hpp", ".h")
    sources = read_sources(paths, suffixes)
    if not any(s.test_cases for s in sources):
        print(f"no test cases found under {' '.join(paths)}")
        return 1

    if command == "markers":
        cases = [c for s in sources for c in s.test_cases]
        findings = [f for c in cases for f in marker_findings(c)]
        if not findings:
            print(f"{len(cases)} test cases, each with a reference marker")
    elif command == "golden":
        findings = golden_findings(argv[1], sources)
        if not findings:
            print("every use of golden values states its fact")
    else:
        findings, pinned_by = trace(*argv[1:4], sources)
        for item, where in pinned_by.items():
            if where:
                print(f"{item}: {', '.join(where)}")
        untested = [item for item, where in pinned_by.items() if not where]
        print(f"items with no test: {', '.join(untested) or 'none'}")
    for finding in findings:
        print(finding)
    return 1 if findings else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
