"""Checks every total the merged audit register states against its tables.

Usage: python3 -I totals_check.py <register.md>

It reads the register's main table (one row per site), then compares with it:
the summary sentence and the summary table, the ranking of confirmed
defects, the documented-behavior table, the six remedy classes of the H6
section and the counts and lists that follow them. It prints one line per
check and exits with status 1 if any check fails.
"""

import re
import sys
from collections import Counter

text = open(sys.argv[1], encoding="utf-8").read()
failures = 0


def check(what, stated, computed):
    global failures
    ok = stated == computed
    failures += not ok
    print(f"{'ok  ' if ok else 'FAIL'} {what}: stated {stated}, computed {computed}")


def section(title):
    start = text.index(title)
    end = text.find("\n## ", start + len(title))
    return text[start : end if end != -1 else len(text)]


def table_rows(block, first_cell):
    """The rows of the Markdown tables in block whose first cell matches."""
    rows = []
    for line in block.splitlines():
        line = line.strip()
        if not line.startswith("| "):
            continue
        cells = [c.strip() for c in re.split(r"(?<!\\)\|", line)[1:-1]]
        if re.fullmatch(first_cell, cells[0]):
            rows.append(cells)
    return rows


def ids(cell):
    return re.findall(r"A\d\d", cell)


# The register: ID, site, condition, cpp read, numerical read, verdict,
# severity, evidence.
register = {}
for cells in table_rows(section("## The register"), r"A\d\d"):
    register[cells[0]] = {
        "cpp": cells[3],
        "num": cells[4],
        "verdict": cells[5].split(" ")[0],
        "mark": cells[5],
        "severity": cells[6],
    }
numbers = sorted(int(i[1:]) for i in register)
check("rows numbered A01 to the last without a gap", numbers, list(range(1, len(numbers) + 1)))

verdicts = Counter(r["verdict"] for r in register.values())
confirmed = {i for i, r in register.items() if r["verdict"] == "confirmed"}
severities = Counter(register[i]["severity"] for i in confirmed)

# The summary sentence.
summary = section("## Summary")
m = re.search(r"(\d+) sites: \*\*(\d+) confirmed, (\d+) dismissed, (\d+) open\.\*\*", summary)
check("sites", int(m[1]), len(register))
check("confirmed", int(m[2]), verdicts["confirmed"])
check("dismissed", int(m[3]), verdicts["dismissed"])
check("open", int(m[4]), verdicts["open"])
m = re.search(r"none is high, (\d+) are medium, (\d+) low and (\d+) nits", summary)
check("high", 0, severities["high"])
check("medium", int(m[1]), severities["medium"])
check("low", int(m[2]), severities["low"])
check("nits", int(m[3]), severities["nit"])


# The summary table: both reads, one read only, neither, decided after the
# reads (the one-read rows, the neither rows, and A03, A16 and A22).
def reads(r):
    return (r["cpp"] != "—") + (r["num"] != "—")


decided_by_name = {"A03", "A16", "A22"}
for cells in table_rows(summary, r"Confirmed|Dismissed|Open|All"):
    kind = cells[0].lower()
    rows = {i: r for i, r in register.items() if kind == "all" or r["verdict"] == kind}
    both = sum(reads(r) == 2 for r in rows.values())
    one = sum(reads(r) == 1 for r in rows.values())
    neither = sum(reads(r) == 0 for r in rows.values())
    decided = sum(reads(r) < 2 or i in decided_by_name for i, r in rows.items())
    stated = [int(c) for c in cells[1:6]]
    check(f"summary table, {cells[0]}", stated, [len(rows), both, one, neither, decided])

# The ranking.
ranking = table_rows(section("## Confirmed defects ranked by severity"), r"\d+")
check("ranks numbered 1 to the last", [int(c[0]) for c in ranking], list(range(1, len(ranking) + 1)))
check("ranked rows are the confirmed rows", sorted(c[1] for c in ranking), sorted(confirmed))
check(
    "each rank's severity is its row's",
    [c[2] for c in ranking],
    [register[c[1]]["severity"] for c in ranking],
)
order = {"high": 0, "medium": 1, "low": 2, "nit": 3}
levels = [order[c[2]] for c in ranking]
check("ranking ordered by severity", levels, sorted(levels))

# The documented-behavior table: each verdict as the register gives it.
documented = table_rows(section("### Documented behavior: one rule"), r"A\d\d(, A\d\d)*")
for cells in documented:
    for i in ids(cells[0]):
        check(f"documented-behavior table, {i}", cells[4], register[i]["mark"])

# The H6 section.
h6 = section("## What the register implies for the size of H6")
m = re.search(r"\*\*(\d+) confirmed rows\*\*: (\d+) medium, (\d+) low, (\d+) nits", h6)
check("H6, confirmed rows", int(m[1]), len(confirmed))
check("H6, medium", int(m[2]), severities["medium"])
check("H6, low", int(m[3]), severities["low"])
check("H6, nits", int(m[4]), severities["nit"])
classes = table_rows(h6, r"[A-Z].*")[1:]  # the first row is the header
listed = Counter()
for cells in classes:
    rows = ids(cells[1])
    listed.update(rows)
    check(f"H6 class count, {cells[0][:40]}", int(cells[2]), len(rows))
check("H6 classes cover the confirmed rows", sorted(listed), sorted(confirmed))
check("H6 classes are disjoint", max(listed.values()), 1)
header_rows = set(ids(classes[0][1])) | set(ids(classes[1][1]))
m = re.search(r"\*\*at most (\d+) rows\*\* \(the first two classes\).*?\*\*(\d+)\*\* of them", h6)
check("H6, rows that can change a header", int(m[1]), len(header_rows))
check("H6, rows with no alternative", int(m[2]), len(ids(classes[0][1])))
m = re.search(
    r"Of those (\d+), \*\*(\d+)\*\* need the owner's ruling whatever the fix \(([^)]*)\)\. "
    r"\*\*(\d+)\*\* need it if .*? \(([^)]*)\)\. \*\*(\d+)\*\* need none \((.*?)\)\.",
    h6,
)
groups = [set(ids(m[3])), set(ids(m[5])), set(ids(m[7]))]
check("H6, the owner's lists: their size", int(m[1]), len(header_rows))
check("H6, the owner's lists: counts", [int(m[2]), int(m[4]), int(m[6])], [len(g) for g in groups])
check("H6, the owner's lists partition the header rows", sorted(groups[0] | groups[1] | groups[2]), sorted(header_rows))
check("H6, the owner's lists are disjoint", sum(len(g) for g in groups), len(groups[0] | groups[1] | groups[2]))
m = re.search(
    r"Of the (\d+), \*\*(\d+)\*\* are in or reached through code that plan 0048 removes or rewrites \(([^;]*); ([^)]*)\)\. "
    r"Over all (\d+), \*\*(\d+)\*\* confirmed rows are in that code: (\d+) by their own lines \((.*?)\) and (\d+) through the public operations they reach \(([^)]*)\)",
    h6,
)
own_header, reach_header = set(ids(m[3])), set(ids(m[4]))
own_all, reach_all = set(ids(m[8])), set(ids(m[10]))
check("H6, plan 0048 overlap: of the header rows", int(m[1]), len(header_rows))
check("H6, plan 0048 overlap among header rows", int(m[2]), len(own_header | reach_header))
check("H6, plan 0048 overlap rows are header rows", sorted((own_header | reach_header) - header_rows), [])
check("H6, plan 0048 overlap: of all confirmed", int(m[5]), len(confirmed))
check("H6, plan 0048 overlap over all", [int(m[6]), int(m[7]), int(m[9])], [len(own_all | reach_all), len(own_all), len(reach_all)])
overlap = section("## Registered sites in code plan 0048 intends to remove or rewrite")
tables = overlap.split("Sites whose own lines stay")
own_rows = {c[0] for c in table_rows(tables[0], r"A\d\d") if c[1].startswith("confirmed")}
reach_rows = {c[0] for c in table_rows(tables[1], r"A\d\d") if c[1].startswith("confirmed")}
check("H6, own-lines list against the overlap table", sorted(own_all), sorted(own_rows))
check("H6, reached-through list against the overlap table", sorted(reach_all), sorted(reach_rows))
m = re.search(r"\*\*(\d+) sites are open\*\* \(([^)]*)\)", h6)
check("H6, open", [int(m[1]), sorted(ids(m[2]))], [verdicts["open"], sorted(i for i, r in register.items() if r["verdict"] == "open")])
m = re.search(r"\*\*(\d+) sites are dismissed\*\*", h6)
check("H6, dismissed", int(m[1]), verdicts["dismissed"])

print(f"{failures} check(s) failed")
sys.exit(1 if failures else 0)
