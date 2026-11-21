"""Check that comment-moves.tsv covers every comment line of every header once.

Usage, from the repository root:
    python3 -I docs/agent/records/docs/check_comment_moves.py [--revision REV] [TABLE]

The headers are read from git at the revision (default 01dce7f, the revision
the table names), not from the working tree, so the check means the same thing
after the headers change.

A comment line is either a full-line comment (its first non-space characters
are //, the rule of the line census) or a code line that carries a trailing
// comment outside any string or character literal. Rows whose position is
"trailing" cover the second kind; every other row covers only the first.
The /*name*/ markers on unnamed parameters are code, not comments.

Checks, each reported with the row's line number in the table:
- every comment line is covered by exactly one row, and no row covers a line
  that is not a comment line of its sort;
- the position column matches the census rule (prologue before the first
  #include, detail inside a detail namespace, public elsewhere);
- kind, class and waits_for take known values;
- a move or summary row names a target page in the documentation layout and a
  heading;
- a delete row names the place that already says it, unless its kind is
  narration or marker; a guide page named so must exist at the revision with
  that heading, and a header range named so must hold comment lines whose rows
  are not themselves deleted.

It then prints the full-line comment lines by class against each of: the
header, the kind, the position, the four position classes of plan 0048's
survey (prologue, /// outside detail, /// inside detail, and // elsewhere), and
what the row waits for. The README's tables reproduce these.
"""
import argparse
import collections
import re
import subprocess
import sys

KINDS = {"file prologue", "public contract", "internal contract", "reason",
         "equation or convention", "narration", "example", "marker", "pointer"}
CLASSES = ("keep", "summary", "move", "delete")
POSITIONS = {"prologue", "public", "detail", "trailing"}

# The page layout of the user documentation planned for docs/guide/. The file
# names inside each directory are proposals until the frame publishes them.
LAYOUT_DIRS = ("docs/guide/tutorial/", "docs/guide/exercises/")
LAYOUT_PAGES = {
    "docs/guide/README.md", "docs/guide/limits.md", "docs/guide/conventions.md",
    "docs/guide/diagnostics.md", "docs/guide/verification.md",
    "docs/guide/glossary.md",
    *(f"docs/guide/howto/{p}.md" for p in (
        "first-things", "build-and-configure", "algebras-and-types", "scalars",
        "rigid-geometry", "port-from-other-conventions")),
    *(f"docs/guide/concepts/{p}.md" for p in (
        "blades-and-grades", "metric-and-signature", "products",
        "complements-and-duals", "versors-and-transforms",
        "layouts-and-result-types", "exactness-and-rounding",
        "compile-time-cost", "how-generation-works")),
    *(f"docs/guide/reference/{p}.md" for p in (
        "index", "algebras", "layouts", "multivectors", "products",
        "involutions-complements-duals", "transform-and-compose",
        "change-of-basis", "printing", "macros", "blade-helpers", "rga3")),
}


def git(*args):
    return subprocess.run(["git", *args], capture_output=True, text=True,
                          check=True).stdout


def trailing_comment(line):
    """Whether a code line holds // outside literals and /* */ markers."""
    i, n = 0, len(line)
    while i < n:
        c = line[i]
        if c in "\"'":
            i += 1
            while i < n and line[i] != c:
                i += 2 if line[i] == "\\" else 1
        elif line.startswith("/*", i):
            end = line.find("*/", i + 2)
            i = n if end < 0 else end + 1
        elif line.startswith("//", i):
            return True
        i += 1
    return False


def census(text):
    """Full-line comment lines, trailing-comment lines, and positions (1-based)."""
    full, trailing, position = set(), set(), {}
    seen_include = in_detail = False
    for number, line in enumerate(text.split("\n"), 1):
        s = line.strip()
        seen_include = seen_include or s.startswith("#include")
        if re.match(r"namespace (galoot::core::)?detail\b", s):
            in_detail = True
        if s.startswith(("} // namespace detail",
                         "} // namespace galoot::core::detail")):
            in_detail = False
        position[number] = ("prologue" if not seen_include
                            else "detail" if in_detail else "public")
        if s.startswith("//"):
            full.add(number)
        elif trailing_comment(line):
            trailing.add(number)
    return full, trailing, position


def parse_lines(spec):
    lines = []
    for part in spec.split(","):
        first, _, last = part.partition("-")
        lines.extend(range(int(first), int(last or first) + 1))
    return lines


def headings(text):
    found = set()
    in_code = False
    for line in text.split("\n"):
        if line.startswith("```"):
            in_code = not in_code
        elif not in_code and line.startswith("#"):
            found.add(line.lstrip("#").strip())
    return found


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--revision", default="01dce7f")
    parser.add_argument("table", nargs="?",
                        default="docs/agent/records/docs/comment-moves.tsv")
    args = parser.parse_args()
    rev = args.revision

    headers = [p for p in git("ls-tree", "-r", "--name-only", rev,
                              "include/galoot").split("\n")
               if p.endswith(".hpp")]
    texts = {h: git("show", f"{rev}:{h}") for h in headers}
    counts = {h: census(texts[h]) for h in headers}

    rows = list(enumerate(open(args.table).read().rstrip("\n").split("\n"), 1))
    columns = rows[0][1].split("\t")
    expected = ["header", "lines", "position", "kind", "class", "descriptor",
                "target", "heading", "waits_for", "note"]
    errors = []
    if columns != expected:
        sys.exit(f"columns are {columns}, expected {expected}")

    owner = {}  # (header, line, sort) -> table row number
    records = []
    for number, raw in rows[1:]:
        fields = raw.split("\t")
        if len(fields) != len(expected):
            errors.append(f"row {number}: {len(fields)} fields")
            continue
        r = dict(zip(expected, fields))
        r["number"] = number
        records.append(r)
        h = r["header"]
        if h not in counts:
            errors.append(f"row {number}: {h} is not a header at {rev}")
            continue
        full, trailing, position = counts[h]
        sort = "trailing" if r["position"] == "trailing" else "full"
        r["line_list"] = parse_lines(r["lines"])
        for line in r["line_list"]:
            if line not in (trailing if sort == "trailing" else full):
                errors.append(f"row {number}: {h}:{line} is not a "
                              f"{'trailing' if sort == 'trailing' else 'full-line'} comment")
            key = (h, line, sort)
            if key in owner:
                errors.append(f"row {number}: {h}:{line} is also in row {owner[key]}")
            owner[key] = number
            if sort == "full" and position.get(line) != r["position"]:
                errors.append(f"row {number}: {h}:{line} is {position.get(line)}, "
                              f"not {r['position']}")
        if r["position"] not in POSITIONS:
            errors.append(f"row {number}: position {r['position']}")
        if r["kind"] not in KINDS:
            errors.append(f"row {number}: kind {r['kind']}")
        if r["class"] not in CLASSES:
            errors.append(f"row {number}: class {r['class']}")
        if not r["descriptor"]:
            errors.append(f"row {number}: no descriptor")
        if not r["waits_for"]:
            errors.append(f"row {number}: no waits_for")
        if r["class"] in ("move", "summary"):
            page = r["target"]
            if r["class"] == "summary" and page.startswith("include/"):
                pass  # the rest already lives in another comment; checked below
            elif not (page in LAYOUT_PAGES or page.startswith(LAYOUT_DIRS)):
                errors.append(f"row {number}: target {page!r} is not in the layout")
            if not r["heading"] and not page.startswith("include/"):
                errors.append(f"row {number}: no heading")

    for h, (full, trailing, _) in counts.items():
        for sort, lines in (("full", full), ("trailing", trailing)):
            missing = sorted(l for l in lines if (h, l, sort) not in owner)
            if missing:
                errors.append(f"{h}: {sort} comment lines with no row: {missing}")

    by_line = {(r["header"], l): r for r in records if r["position"] != "trailing"
               for l in r.get("line_list", [])}
    for r in records:
        if r["class"] == "summary" and r["target"].startswith("include/"):
            pass
        elif r["class"] != "delete" or r["kind"] in ("narration", "marker"):
            continue
        place = r["target"]
        if not place:
            errors.append(f"row {r['number']}: a {r['class']} of a {r['kind']} names no place")
        elif place.startswith("include/"):
            h, _, spec = place.partition(":")
            for line in parse_lines(spec) if spec else []:
                home = by_line.get((h, line))
                if home is None:
                    errors.append(f"row {r['number']}: {h}:{line} is not a comment line")
                elif home["class"] == "delete":
                    errors.append(f"row {r['number']}: its home {h}:{line} is deleted too "
                                  f"(row {home['number']})")
        else:
            try:
                page = git("show", f"{rev}:{place}")
            except subprocess.CalledProcessError:
                errors.append(f"row {r['number']}: {place} does not exist at {rev}")
                continue
            if r["heading"] and r["heading"] not in headings(page):
                errors.append(f"row {r['number']}: {place} has no heading {r['heading']!r}")

    if errors:
        print("\n".join(errors))
        sys.exit(f"{len(errors)} problems")

    lines_of = collections.Counter()
    for r in records:
        n = len(r["line_list"])
        sort = "trailing" if r["position"] == "trailing" else "full"
        lines_of[("header", r["header"], sort, r["class"])] += n
        lines_of[("kind", r["kind"], sort, r["class"])] += n
        lines_of[("position", r["position"], sort, r["class"])] += n
        lines_of[("waits_for", r["waits_for"], sort, r["class"])] += n
        if sort == "full":
            text = texts[r["header"]].split("\n")
            for line in r["line_list"]:
                doc = text[line - 1].strip().startswith("///")
                group = ("prologue" if r["position"] == "prologue"
                         else f"/// {r['position']}" if doc else "//")
                lines_of[("plan class", group, sort, r["class"])] += 1

    def table(axis, keys):
        print(f"\n{axis}\tkeep\tsummary\tmove\tdelete\tall")
        totals = collections.Counter()
        for k in keys:
            values = [lines_of[(axis, k, s, c)] for c in CLASSES
                      for s in ("full",)]
            totals.update(dict(zip(CLASSES, values)))
            print("\t".join([k, *map(str, values), str(sum(values))]))
        print("\t".join(["all", *(str(totals[c]) for c in CLASSES),
                         str(sum(totals.values()))]))

    full_total = sum(len(c[0]) for c in counts.values())
    trailing_total = sum(len(c[1]) for c in counts.values())
    print(f"revision {rev}: {len(headers)} headers, {len(records)} rows, "
          f"{full_total} full-line comment lines and {trailing_total} trailing "
          "comments, each covered once")
    print("Full-line comment lines:")
    table("header", headers)
    table("kind", sorted(KINDS))
    table("position", ["prologue", "public", "detail"])
    table("plan class", ["prologue", "/// public", "/// detail", "//"])
    table("waits_for", sorted({r["waits_for"] for r in records
                               if r["position"] != "trailing"}))
    trailing_by_class = collections.Counter()
    for r in records:
        if r["position"] == "trailing":
            trailing_by_class[r["class"]] += len(r["line_list"])
    print("\nTrailing comments: " + ", ".join(
        f"{c} {trailing_by_class[c]}" for c in CLASSES))


if __name__ == "__main__":
    main()
