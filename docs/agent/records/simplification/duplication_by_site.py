"""The plan's duplication measure, with each duplicated token traced to its line and unit.

Usage, from the repository root:
    python3 -I docs/agent/records/simplification/duplication_by_site.py

Tokenizes as records/performance/simplification-survey/dup.py does (comments
after // removed, string literals masked as "S", tokens are identifiers,
numbers or single other characters) and finds the same duplicated windows:
type 1, exact token windows of 40; type 2, windows of 60 with every identifier
that is not one of dup.py's keywords replaced by one symbol. Windows are
compared as tuples, so no hash collision can merge two. The totals must equal
dup.py's (5,061 and 10,309 of 40,192 at the plan's revision); the script stops
otherwise. It then gives the share of duplicated tokens inside the line ranges
of each unit of candidates.tsv, and the type 1 and type 2 rates for other window
lengths, to show how much the figure depends on the window.
"""
import collections
import csv
import glob
import re
import sys

FILES = sorted(glob.glob("include/galoot/**/*.hpp", recursive=True))
KW = set("template class struct constexpr consteval const auto static inline if else for while "
         "return requires concept using namespace typename noexcept true false static_assert "
         "bool int".split())


def tokens(path):
    out = []
    for n, line in enumerate(open(path).read().split("\n"), 1):
        line = re.sub(r"//.*", "", line)
        line = re.sub(r'"(?:[^"\\]|\\.)*"', '"S"', line)
        out += [(t, n) for t in re.findall(r'[A-Za-z_]\w*|\d+|\S', line)]
    return out


TOK = {f: tokens(f) for f in FILES}
TOTAL = sum(len(v) for v in TOK.values())


def duplicated(window, normalize):
    seen = collections.defaultdict(list)
    for f in FILES:
        tk = [t for t, _ in TOK[f]]
        if normalize:
            tk = [t if (t in KW or not re.match(r"[A-Za-z_]", t)) else "I" for t in tk]
        for i in range(len(tk) - window):
            seen[tuple(tk[i:i + window])].append((f, i))
    dup = set()
    for places in seen.values():
        if len(places) > 1:
            for f, i in places:
                dup.update((f, k) for k in range(i, i + window))
    return dup


type1, type2 = duplicated(40, False), duplicated(60, True)
print(f"type 1, exact, window 40: {len(type1)} of {TOTAL} = {100 * len(type1) / TOTAL:.1f}%")
print(f"type 2, identifiers normalized, window 60: {len(type2)} of {TOTAL} = {100 * len(type2) / TOTAL:.1f}%")
if (len(type1), len(type2), TOTAL) != (5061, 10309, 40192):
    print("totals differ from dup.py's at the plan's revision; the tree has changed", file=sys.stderr)

unit_of = collections.defaultdict(set)
for r in csv.DictReader(open("docs/agent/records/simplification/candidates.tsv"), delimiter="\t"):
    a, _, b = r["lines"].partition("-")
    for n in range(int(a), int(b or a) + 1):
        unit_of[(r["file"], n)].add(r["unit"])

print("\nShare of duplicated tokens inside each unit's line ranges (a token in two units counts in both)")
print("unit\ttype1_tokens\ttype1_share\ttype2_tokens\ttype2_share")
units = list(dict.fromkeys(u for us in unit_of.values() for u in sorted(us)))
for dup, name in ((type1, "type1"), (type2, "type2")):
    pass
for u in units + ["(any unit)", "(no unit)"]:
    row = [u]
    for dup in (type1, type2):
        def hit(f, k):
            us = unit_of.get((f, TOK[f][k][1]), set())
            return bool(us) if u == "(any unit)" else (not us if u == "(no unit)" else u in us)
        c = sum(1 for f, k in dup if hit(f, k))
        row += [str(c), f"{100 * c / len(dup):.1f}%"]
    print("\t".join(row))

print("\nRates for other windows")
print("window\ttype1_percent\ttype2_percent")
for w in (20, 40, 60, 80, 100, 150):
    print(f"{w}\t{100 * len(duplicated(w, False)) / TOTAL:.1f}\t{100 * len(duplicated(w, True)) / TOTAL:.1f}")
