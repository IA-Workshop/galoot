"""For each case of cases.json: compose the kernels as collect.hpp does and
compare the number of composed paths with the sum of |C| over kept outputs.
Equal means no monomial gathers paths of both signs (Lemma 1's test)."""
import json, sys
from collections import defaultdict
cases = json.load(open(sys.argv[1]))
if isinstance(cases, dict):
    cases = [dict(v, name=k) for k, v in cases.items()]
for case in cases:
    first, second = case["first"], case["second"]
    by_mid = defaultdict(list)
    for a, b, o, c, f in first:
        by_mid[o].append((a, b, c))
    C = defaultdict(int); paths = 0
    pos = defaultdict(int); neg = defaultdict(int)
    for a2, b2, o2, c2, f2 in second:
        for a1, b1, c1 in by_mid[a2]:
            p, q = sorted((a1, b2))
            key = (o2, b1, p, q)
            C[key] += c1 * c2; paths += 1
            (pos if c1 * c2 > 0 else neg)[key] += 1
    s = sum(abs(v) for v in C.values())
    mixed = sum(1 for k in C if pos[k] and neg[k])
    zero = sum(1 for k in C if C[k] == 0)
    print(f"{case.get('name', case.get('case'))}: V={len(case['versor'])} X={len(case['object'])} "
          f"first={len(first)} second={len(second)} paths={paths} sum|C|={s} "
          f"monomials={sum(1 for v in C.values() if v)} mixed-sign={mixed} cancel-to-zero={zero}")
