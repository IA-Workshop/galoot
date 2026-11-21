"""Reproduces the two computed parts of the sample check of the plan 0047 gap register.

Usage, from the root of any Galoot checkout that has the revision:
  python3 -I docs/agent/records/reviews/2026-11-21-0047-v1-sample-check/check.py [revision]

1. Draws the sample: 6 of the 14 G rows and 8 of the 26 A rows that section C
   of the register does not touch, with random.Random(20261121), G first.
2. Counts the status words Checked, Confirmed, Proposed and Study in
   CONVENTIONS.md at the revision (default 01dce7f), over the whole file and
   over lines 18 to 84 and 98 to 116 (sections 1 to 5 and 7), and counts the
   numbered items of those sections.
"""

import random
import re
import subprocess
import sys

revision = sys.argv[1] if len(sys.argv) > 1 else "01dce7f"

g_rows = [f"G{i}" for i in (2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 15, 16, 17)]
a_rows = [f"A{i}" for i in range(1, 28) if i != 23]
draw = random.Random(20261121)
by_number = lambda row: int(row[1:])
print("sample G:", sorted(draw.sample(g_rows, 6), key=by_number))
print("sample A:", sorted(draw.sample(a_rows, 8), key=by_number))

text = subprocess.run(
    ["git", "show", f"{revision}:docs/agent/governance/CONVENTIONS.md"],
    check=True, capture_output=True, text=True).stdout.splitlines()
in_range = text[17:84] + text[97:116]
for word in ("Checked", "Confirmed", "Proposed", "Study"):
    whole = sum(line.count(word) for line in text)
    ranged = sum(line.count(word) for line in in_range)
    print(f"{word}: whole file {whole}, sections 1 to 5 and 7 {ranged}")
items = [line.split()[0] for line in in_range
         if re.match(r"^[1-57]\.[0-9]+ ", line)]
print(f"numbered items in sections 1 to 5 and 7: {len(items)}:", " ".join(items))
