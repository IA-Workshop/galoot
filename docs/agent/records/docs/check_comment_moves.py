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
  are not themselves deleted;
- every checked precondition is accounted for: each line of the Requires
  clause of a declaration that holds a GALOOT_ASSERT call, or of a clause
  that names GALOOT_ASSERT, is in the README's table of preconditions kept in
  full, whose rows must be keep, or in its table of checked conditions that
  are not undefined behavior; an assertion whose declaration has no Requires
  clause is in a third table, which names where its condition is stated.
  The README says how the declaration is found and what this can miss;
- every place in KEPT_IN_FULL, below, is a comment line whose row is keep,
  and the README's table of preconditions kept in full names exactly those
  places.

It then prints the full-line comment lines by class against each of: the
header, the kind, the position, the four position classes of plan 0048's
survey (prologue, /// outside detail, /// inside detail, and // elsewhere), and
what the row waits for. The README's tables reproduce these. Last it prints
the boundary report, which never fails: each pair of neighboring rows with a
different class or waits_for where a sentence runs from one into the other.
--boundaries FILE writes the report to FILE as well.
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
# What a row may wait for. The README's table "What a row waits for" says
# what each means; a condition that is met or ruled leaves this list.
WAITS = {"none", "B9", "D1", "D3", "the S11 amendment",
         "D3 and the S11 amendment"}

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


def code_text(line):
    """The code of a line, with comments cut and literal contents blanked."""
    out, i, n = [], 0, len(line)
    while i < n:
        c = line[i]
        if c in "\"'":
            j = i + 1
            while j < n and line[j] != c:
                j += 2 if line[j] == "\\" else 1
            out.append(c + " " * max(0, j - i - 1) + c)
            i = j + 1
        elif line.startswith("/*", i):
            end = line.find("*/", i + 2)
            out.append(" ")
            i = n if end < 0 else end + 2
        elif line.startswith("//", i):
            break
        else:
            out.append(c)
            i += 1
    return "".join(out)


def scope_kind(head):
    """namespace, class (struct, class, union, enum) or other, from a brace's head."""
    if re.search(r"\bnamespace\b", head):
        return "namespace"
    keys = list(re.finditer(r"\b(struct|class|union|enum)\b", head))
    if keys and "(" not in head[keys[-1].end():]:
        return "class"
    return "other"


def body_openers(text):
    """For each line, the line of the opening brace of the outermost scope
    around its start that is neither a namespace nor a class: the body of the
    function, or the initializer of the variable, that holds the line."""
    stack, head, opener = [], "", {}
    in_macro = False
    for number, line in enumerate(text.split("\n"), 1):
        bodies = [at for kind, at in stack if kind == "other"]
        opener[number] = bodies[0] if bodies else None
        s = line.strip()
        if in_macro or s.startswith("#"):
            in_macro = line.rstrip().endswith("\\")
            continue
        for c in code_text(line):
            if c == "{":
                inside = any(kind == "other" for kind, _ in stack)
                stack.append(("other" if inside else scope_kind(head), number))
                head = ""
            elif c == "}":
                stack.pop()
                head = ""
            elif c == ";":
                head = ""
            else:
                head += c
        head += " "
    if stack:
        raise ValueError(f"unbalanced braces, open at lines {[a for _, a in stack]}")
    return opener


def comment_text(line):
    return line.strip().lstrip("/").strip()


def is_code(line):
    s = line.strip()
    return bool(s) and not s.startswith(("//", "#"))


def doc_block(lines, opener):
    """The full-line comment lines right above the declaration whose body
    opens at line `opener`, found by walking up over the declaration's code
    lines until a line that is blank, a comment, a preprocessor line, or code
    that ends the statement before it. A NOLINT line between two lines of
    the declaration, as before a parameter list, is part of the declaration."""
    def head(line):
        return is_code(line) and not code_text(line).rstrip().endswith((";", "{", "}"))

    i = opener
    while i > 1:
        if head(lines[i - 2]):
            i -= 1
        elif (i > 2 and comment_text(lines[i - 2]).startswith("NOLINT")
              and head(lines[i - 3])):
            i -= 1
        else:
            break
    block = []
    j = i - 1
    while j >= 1 and lines[j - 1].strip().startswith("//"):
        block.insert(0, j)
        j -= 1
    return i, block


LABEL = re.compile(r"^[A-Z][a-z]+( [a-z]+)?( of \w+)?:")


def requires_paragraphs(lines, block):
    """The runs of lines in `block` that state a Requires clause: from a line
    whose text starts with Requires to the line before the next labeled
    clause (Guarantees:, Cost: and the like), an empty comment line, or the
    end of the block."""
    found, current = [], None
    for l in block:
        t = comment_text(lines[l - 1])
        if t.startswith("Requires"):
            if current:
                found.append(current)
            current = [l]
        elif current is not None:
            if not t or LABEL.match(t):
                found.append(current)
                current = None
            else:
                current.append(l)
    if current:
        found.append(current)
    return found


def comment_blocks(lines):
    """Every run of full-line comment lines, as lists of line numbers."""
    blocks, run = [], []
    for number, line in enumerate(lines, 1):
        if line.strip().startswith("//"):
            run.append(number)
        elif run:
            blocks.append(run)
            run = []
    if run:
        blocks.append(run)
    return blocks


def readme_places(readme, heading, headers, column=0):
    """The header lines a README table names in one column, as a list of
    (header, lines, table line), or None when there is no such heading.

    The table is the first one after the heading. A place is written
    `name.hpp:a-b` or, for the same header as the place before it, `:a-b`."""
    text = readme.split("\n")
    try:
        start = next(i for i, l in enumerate(text)
                     if l.lstrip("#").strip() == heading and l.startswith("#"))
    except StopIteration:
        return None
    places, header, seen_table = [], None, False
    for line in text[start + 1:]:
        if line.startswith("#"):
            break
        if not line.startswith("|"):
            if seen_table:
                break
            continue
        seen_table = True
        cells = [c.strip() for c in line.strip().strip("|").split("|")]
        if set(cells[0]) <= set("- ") or "`" not in cells[0]:
            continue  # the separator or the heading row
        for name, spec in re.findall(r"`(?:([\w./]+\.hpp))?:(\d+(?:-\d+)?)`",
                                     cells[column] if column < len(cells) else ""):
            if name:
                matches = [h for h in headers if h.endswith("/" + name)]
                header = matches[0] if len(matches) == 1 else name
            places.append((header, parse_lines(spec), line))
    return places


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


UB_HEADING = "Preconditions kept in full"
OTHER_HEADING = "Checked conditions that are not undefined behavior"
ORPHAN_HEADING = "Assertions with no Requires clause above them"

# The preconditions that are undefined behavior when broken in an unchecked
# build, each kept in full on its declaration. The README's first table names
# exactly these places and says why each is undefined; the list is here as
# well so that a change to one of them is a change to this check, and not only
# to the README that a comment unit edits. Several have no assertion behind
# them, so nothing else would notice their loss.
KEPT_IN_FULL = tuple(
    (f"include/galoot/{h}", spec) for h, spec in (
        ("core/assert.hpp", "30-32"),
        ("core/basis_change.hpp", "258-259"),
        ("core/basis_change.hpp", "753-757"),
        ("core/bits.hpp", "14-15"),
        ("core/bits.hpp", "45-47"),
        ("core/bits.hpp", "58-60"),
        ("core/bits.hpp", "80-81"),
        ("core/bits.hpp", "115-116"),
        ("core/bits.hpp", "130-131"),
        ("core/bits.hpp", "143-144"),
        ("core/bits.hpp", "167-168"),
        ("core/blade_product.hpp", "49-57"),
        ("core/blade_product.hpp", "82-83"),
        ("core/blade_product.hpp", "117-118"),
        ("core/blade_product.hpp", "139-140"),
        ("core/blade_product.hpp", "268-275"),
        ("core/linear.hpp", "392-396"),
        ("core/linear.hpp", "407-411"),
        ("core/metric.hpp", "126"),
        ("core/mv.hpp", "1281-1283"),
        ("core/plan.hpp", "142-143"),
        ("core/plan.hpp", "160"),
        ("core/plan.hpp", "185-187"),
        ("core/plan.hpp", "255-259"),
        ("core/plan.hpp", "303-304"),
        ("core/plan.hpp", "664-665"),
        ("core/plan.hpp", "698-699"),
        ("presets/rga3/motions.hpp", "48-61"),
    ))


def precondition_sites(texts):
    """The Requires clauses that state a checked precondition, by header.

    A clause is taken when the declaration whose body or initializer holds a
    GALOOT_ASSERT call has it in the comment right above, or when the clause
    itself names GALOOT_ASSERT (a precondition checked in a callee). Returns
    the clauses, as {(header, lines): [why]}, and the assertions whose
    declaration has no Requires clause above it."""
    sites, orphans = collections.defaultdict(list), []
    for h, text in texts.items():
        lines = text.split("\n")
        opener = body_openers(text)
        for number, line in enumerate(lines, 1):
            if line.strip().startswith("#") or "GALOOT_ASSERT(" not in code_text(line):
                continue
            if opener[number] is None:
                orphans.append((h, number, None))
                continue
            start, block = doc_block(lines, opener[number])
            paragraphs = requires_paragraphs(lines, block)
            if not paragraphs:
                orphans.append((h, number, start))
            for p in paragraphs:
                sites[(h, tuple(p))].append(
                    f"GALOOT_ASSERT at {number} in the declaration at {start}")
        for block in comment_blocks(lines):
            for p in requires_paragraphs(lines, block):
                if any("GALOOT_ASSERT" in lines[l - 1] for l in p):
                    sites[(h, tuple(p))].append("the clause names GALOOT_ASSERT")
    return sites, orphans


def spans(numbers):
    """Line numbers written as ranges: 3-5,9."""
    out, numbers = [], sorted(numbers)
    for n in numbers:
        if out and out[-1][1] == n - 1:
            out[-1][1] = n
        else:
            out.append([n, n])
    return ",".join(f"{a}-{b}" if a != b else f"{a}" for a, b in out)


def check_preconditions(readme_path, headers, texts, by_line):
    """Each checked precondition, and each one the README lists, is kept in full.

    Every line of a Requires clause that precondition_sites finds must be
    named by one of two README tables: the preconditions kept in full
    (undefined behavior when broken in an unchecked build), whose rows must
    all be keep, or the checked conditions that are not undefined behavior,
    which the ruling leaves to the ordinary classes. An assertion whose
    declaration has no Requires clause must be in a third table, which says
    where its condition is stated; those places must be in one of the first
    two."""
    errors = []
    readme = open(readme_path).read()
    names = {"kept in full": UB_HEADING, "not undefined behavior": OTHER_HEADING}
    lists = {}
    for name, heading in names.items():
        lists[name] = readme_places(readme, heading, headers)
        if lists[name] is None:
            errors.append(f"README: no table under the heading {heading!r}")
            lists[name] = []
    orphan_lines = readme_places(readme, ORPHAN_HEADING, headers) or []
    orphan_homes = readme_places(readme, ORPHAN_HEADING, headers, column=1) or []

    listed = {}
    for name, places in lists.items():
        for h, numbers, _ in places:
            if h not in texts:
                errors.append(f"README, {name}: {h} is not a header")
                continue
            for l in numbers:
                row = by_line.get((h, l))
                if row is None:
                    errors.append(f"README, {name}: {h}:{l} is not a full-line comment")
                    continue
                if listed.setdefault((h, l), name) != name:
                    errors.append(f"README: {h}:{l} is in both lists")
                if name == "kept in full" and row["class"] != "keep":
                    errors.append(f"row {row['number']}: {h}:{l} is a precondition kept "
                                  f"in full in the README, but the row is {row['class']}")

    kept = {(h, l) for h, spec in KEPT_IN_FULL for l in parse_lines(spec)}
    demoted = collections.defaultdict(list)
    for h, l in sorted(kept):
        row = by_line.get((h, l))
        if row is None:
            errors.append(f"KEPT_IN_FULL: {h}:{l} is not a full-line comment")
        elif row["class"] != "keep":
            demoted[row["number"]].append(l)
    for number, numbers in sorted(demoted.items()):
        row = next(r for r in by_line.values() if r["number"] == number)
        errors.append(f"row {number}: {row['header']}:{spans(numbers)} is a precondition "
                      f"kept in full (KEPT_IN_FULL), but the row is {row['class']}")
    in_readme = {(h, l) for h, numbers, _ in lists["kept in full"] for l in numbers}
    for difference, says in ((kept - in_readme, "in KEPT_IN_FULL but not in the "
                              "README's table"),
                             (in_readme - kept, "in the README's table but not in "
                              "KEPT_IN_FULL")):
        for h in sorted({h for h, _ in difference}):
            errors.append(f"README, kept in full: {h}:"
                          f"{spans(l for g, l in difference if g == h)} is {says}")

    sites, orphans = precondition_sites(texts)
    named = {(h, l) for h, numbers, _ in orphan_lines for l in numbers}
    found = set()
    for h, number, start in orphans:
        found.add((h, number))
        if (h, number) not in named:
            where = (f"the declaration at {start} has no Requires clause above it"
                     if start else "it is in no function body or initializer")
            errors.append(f"{h}:{number}: GALOOT_ASSERT, but {where}, and the "
                          f"README's table {ORPHAN_HEADING!r} does not name it")
    for h, l in sorted(named - found):
        errors.append(f"README, {ORPHAN_HEADING!r}: {h}:{l} is not such an assertion")
    for h, numbers, _ in orphan_homes:
        for l in numbers:
            if (h, l) not in by_line:
                errors.append(f"README, {ORPHAN_HEADING!r}: {h}:{l} is not a "
                              "full-line comment")
        sites[(h, tuple(numbers))].append("the README names it for an assertion")

    covered = set()
    for (h, paragraph), why in sorted(sites.items()):
        covered.update((h, l) for l in paragraph)
        missing = [l for l in paragraph if (h, l) not in listed and (h, l) in by_line]
        if missing:
            rows = sorted({f"{by_line[(h, l)]['number']} ({by_line[(h, l)]['class']})"
                           for l in missing})
            errors.append(f"{h}:{spans(missing)}: a checked precondition "
                          f"({'; '.join(why)}) in neither README list; "
                          f"rows {', '.join(rows)}")
    for h, numbers, _ in lists["not undefined behavior"]:
        if not any((h, l) in covered for l in numbers):
            errors.append(f"README, not undefined behavior: {h}:{spans(numbers)} "
                          "states no checked condition the checker found")
    return errors


def boundary_report(texts, records):
    """Adjacent rows with a different class or waits_for that share a sentence:
    the last line of the first does not end one, so the sentence open there
    ends in the next row. Marker and example rows hold no prose and are left
    out."""
    found = []
    rows = sorted((r for r in records if r["position"] != "trailing"
                   and r.get("line_list")),
                  key=lambda r: (r["header"], r["line_list"][0]))
    for a, b in zip(rows, rows[1:]):
        if (a["header"] != b["header"] or a["line_list"][-1] + 1 != b["line_list"][0]
                or (a["class"], a["waits_for"]) == (b["class"], b["waits_for"])
                or {"marker", "example"} & {a["kind"], b["kind"]}):
            continue
        lines = texts[a["header"]].split("\n")
        last = comment_text(lines[a["line_list"][-1] - 1])
        first = comment_text(lines[b["line_list"][0] - 1])
        if not last or not first or last.rstrip(")\"'`*").endswith((".", "?", "!")):
            continue
        found.append({
            "header": a["header"], "line": a["line_list"][-1],
            "rows": f"{a['number']},{b['number']}",
            "first_row": f"{a['class']}, {a['waits_for']}",
            "second_row": f"{b['class']}, {b['waits_for']}",
            "last_line": last, "next_line": first})
    return found


BOUNDARY_COLUMNS = ("header", "line", "rows", "first_row", "second_row",
                    "last_line", "next_line")


def write_boundaries(found, out):
    out.write("\t".join(BOUNDARY_COLUMNS) + "\n")
    for f in found:
        out.write("\t".join(str(f[c]) for c in BOUNDARY_COLUMNS) + "\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--revision", default="01dce7f")
    parser.add_argument("--readme", default="docs/agent/records/docs/README.md")
    parser.add_argument("--boundaries", metavar="TSV",
                        help="also write the boundary report to this file")
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
        if r["waits_for"] not in WAITS:
            errors.append(f"row {number}: waits_for {r['waits_for']!r}")
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

    errors.extend(check_preconditions(args.readme, headers, texts, by_line))
    boundaries = boundary_report(texts, records)
    if args.boundaries:
        with open(args.boundaries, "w") as out:
            write_boundaries(boundaries, out)

    def print_boundaries():
        print(f"\nSentences that cross a row boundary (a report, not a check): "
              f"{len(boundaries)}")
        write_boundaries(boundaries, sys.stdout)

    if errors:
        print("\n".join(errors))
        print_boundaries()
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
    print_boundaries()


if __name__ == "__main__":
    main()
