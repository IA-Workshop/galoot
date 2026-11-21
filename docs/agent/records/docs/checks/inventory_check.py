"""Check the order of the concept inventory.

Reads ../concept-inventory.md (or the path given) and fails if an ID is
repeated, if a row rests on an ID that no row has, or if a row rests on a
row that comes after it. Rows are the table lines whose first cell is an ID:
P and Q for prerequisites, K for the tutorial, X for the other pages. The
file's order is the teaching order, so a pass means the order has no cycle
and no forward reference. It prints the counts and the public names the
tutorial brings in.

It also fails if a tutorial row's text uses a term before the row that
defines it. The term a row defines is its headword: in a row of kind term or
symbol, the words before the first colon of the concept, without a leading
article, a symbol, or ", first form" or ", refined", and split at "and"
("Bulk and weight" defines both). The first row with a headword defines it,
so a refined row does not. A use is the headword, or its plural, as a whole
word in the concept of a K row above the defining one, outside backticks. A
headword that a prerequisite row also uses has its school meaning there
(line, plane) and is not checked. Only headwords are seen: a term named
later in a cell, such as "grade" after "Blade: ...;", is not, and the script
lists the term rows it could not read a headword from.

Run: python3 -I inventory_check.py [path]
"""

import pathlib
import re
import sys

ID = re.compile(r"^[PQKX]\d{2,3}$")


HEADWORD = re.compile(r"^(?:(?:A|An|The) )?([^:;]+?)(?:, (?:first form|refined))?:")


def prose(text):
    """The text of a cell without its code spans."""
    return re.sub(r"`[^`]*`", "", text)


def headwords(cells):
    """The terms a row of kind term or symbol defines, from its headword."""
    if cells[2] not in ("term", "symbol"):
        return None
    match = HEADWORD.match(prose(cells[1]).strip())
    if not match:
        return None
    words = [w for w in match.group(1).split() if any(ch.isalpha() for ch in w)]
    if not words or len(words) > 4:
        return None
    return [t.strip() for t in re.split(r"\band\b", " ".join(words)) if t.strip()]


def term_order(rows, errors):
    """Flag a K row whose text uses a headword above the row defining it."""
    defined = {}
    unread = []
    for index, (_, cells) in enumerate(rows):
        if cells[0][0] not in "KX":
            continue
        terms = headwords(cells)
        if terms is None:
            if cells[2] == "term":
                unread.append(cells[0])
            continue
        for term in terms:
            defined.setdefault(term.lower(), (index, cells[0]))
    school = []
    for term, (index, ident) in defined.items():
        use = re.compile(r"\b" + re.escape(term) + r"(?:s|es)?\b", re.IGNORECASE)
        if any(c[0][0] == "P" and use.search(prose(c[1])) for _, c in rows):
            school.append(term)
            continue
        for number, cells in rows[:index]:
            if cells[0][0] == "K" and use.search(prose(cells[1])):
                errors.append(f"line {number}: {cells[0]} uses '{term}' before "
                              f"{ident}, which defines it")
    return defined, unread, sorted(school)


def main(argv):
    here = pathlib.Path(__file__).resolve().parent
    path = pathlib.Path(argv[1]) if len(argv) > 1 else here.parent / "concept-inventory.md"
    rows = []
    for number, line in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        if not line.startswith("|"):
            continue
        cells = [c.strip() for c in line.strip().strip("|").split("|")]
        if len(cells) == 5 and ID.match(cells[0]):
            rows.append((number, cells))

    errors = []
    position = {}
    for index, (number, cells) in enumerate(rows):
        if cells[0] in position:
            errors.append(f"line {number}: {cells[0]} is repeated")
        position[cells[0]] = index

    names = []
    for index, (number, cells) in enumerate(rows):
        ident, _, _, rests, last = cells
        deps = [d.strip() for d in rests.split(",") if d.strip()]
        for dep in deps:
            if not ID.match(dep):
                errors.append(f"line {number}: {ident} rests on '{dep}', not an ID")
            elif dep not in position:
                errors.append(f"line {number}: {ident} rests on {dep}, which has no row")
            elif position[dep] >= index:
                errors.append(f"line {number}: {ident} rests on {dep}, which comes later")
            elif ident[0] in "PQ" and dep[0] not in "PQ":
                errors.append(f"line {number}: prerequisite {ident} rests on {dep}")
            elif ident[0] == "K" and dep[0] == "X":
                errors.append(f"line {number}: {ident} rests on {dep}, outside the tutorial")
            elif ident[0] == "K" and dep[0] == "K" and dep[1:-2] > ident[1:-2]:
                errors.append(f"line {number}: {ident} rests on a later chapter's {dep}")
        if ident[0] == "K" and not deps:
            errors.append(f"line {number}: {ident} rests on nothing")
        if ident[0] == "K":
            names += re.findall(r"`([^`]+)`", last)

    terms, unread, school = term_order(rows, errors)

    kinds = {k: sum(1 for _, c in rows if c[0][0] == k) for k in "PQKX"}
    chapters = sorted({c[0][1:-2] for _, c in rows if c[0][0] == "K"}, key=int)
    print(f"rows: {kinds['P'] + kinds['Q']} prerequisites, {kinds['K']} tutorial "
          f"concepts in {len(chapters)} chapters, {kinds['X']} outside the tutorial")
    for ch in chapters:
        count = sum(1 for _, c in rows if c[0][0] == "K" and c[0][1:-2] == ch)
        print(f"  chapter {ch}: {count}")
    unique = sorted(set(names))
    print(f"public names the tutorial brings in: {len(unique)}")
    print("  " + ", ".join(unique))
    if len(unique) != len(names):
        errors.append("a public name is brought in by two rows")
    print(f"headwords checked for use before definition: {len(terms)}")
    print("  taken as school meanings, from a prerequisite row: " + ", ".join(school))
    print("  term rows with no headword, not checked: " + ", ".join(unread))
    for e in errors:
        print("ERROR " + e)
    print("order holds" if not errors else f"{len(errors)} errors")
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
