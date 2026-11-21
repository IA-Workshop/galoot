"""Checks candidates.tsv against the headers and sums the code at each unit's sites.

Usage, from the repository root:
    python3 -I docs/agent/records/simplification/entities.py > /tmp/entities.tsv
    python3 -I docs/agent/records/simplification/check_candidates.py /tmp/entities.tsv

For each row whose entity is one plain name, the row's line range must be the
range entities.py reports for an entity of that name in that header; a row that
fails is printed. Then, per unit, it prints the code lines (not blank, not
comment-only) and comment lines inside the rows' ranges, counting each header
line once. Those are the lines a unit opens, an upper bound on what it can
delete, not an estimate of its saving.
"""
import collections
import csv
import re
import sys

ents = collections.defaultdict(set)
for row in csv.reader(open(sys.argv[1]), delimiter="\t"):
    ents[(row[0], row[5])].add((int(row[1]), int(row[2])))

rows = list(csv.DictReader(open("docs/agent/records/simplification/candidates.tsv"), delimiter="\t"))
text = {}
bad = 0
lines = collections.defaultdict(set)
for r in rows:
    f = r["file"]
    text.setdefault(f, open(f).read().split("\n"))
    a, _, b = r["lines"].partition("-")
    a, b = int(a), int(b or a)
    if b > len(text[f]) or a > b:
        print(f"range outside the header: {f} {r['lines']}")
        bad += 1
    name = r["entity"]
    if re.fullmatch(r"[A-Za-z_]\w*", name):
        if (a, b) not in ents.get((f, name), set()):
            print(f"not an entity range: {r['unit']} {f}:{r['lines']} {name}; "
                  f"entities.py has {sorted(ents.get((f, name), set()))}")
            bad += 1
    lines[r["unit"]].update((f, n) for n in range(a, b + 1))

print("unit\tcode_lines\tcomment_lines")
for unit in dict.fromkeys(r["unit"] for r in rows):
    code = sum(1 for f, n in lines[unit] if text[f][n - 1].strip()
               and not text[f][n - 1].strip().startswith("//"))
    comment = sum(1 for f, n in lines[unit] if text[f][n - 1].strip().startswith("//"))
    print(f"{unit}\t{code}\t{comment}")
print(f"{len(rows)} rows, {bad} that do not match")
sys.exit(1 if bad else 0)
