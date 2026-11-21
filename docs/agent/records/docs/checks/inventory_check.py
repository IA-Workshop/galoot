"""Check the order of the concept inventory.

Reads ../concept-inventory.md (or the path given) and fails if an ID is
repeated, if a row rests on an ID that no row has, or if a row rests on a
row that comes after it. Rows are the table lines whose first cell is an ID:
P and Q for prerequisites, K for the tutorial, X for the other pages. The
file's order is the teaching order, so a pass means the order has no cycle
and no forward reference. It prints the counts and the public names the
tutorial brings in.

Run: python3 -I inventory_check.py [path]
"""

import pathlib
import re
import sys

ID = re.compile(r"^[PQKX]\d{2,3}$")


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
    for e in errors:
        print("ERROR " + e)
    print("order holds" if not errors else f"{len(errors)} errors")
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
