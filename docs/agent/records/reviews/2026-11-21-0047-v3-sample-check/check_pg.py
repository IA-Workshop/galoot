"""5.6p: close each golden point group's generator versors under my matrix-route
antiproduct (projectively), turn each element into its 3x3 action on points
(via transform), and compare with golden's element matrices and the published
orders of the 32 crystal classes."""
import sys, re
sys.path.insert(0, sys.argv[1])
from mx import *

src = open(sys.argv[2]).read()
ns = src[src.index("namespace point_groups {"):src.index("} // namespace point_groups")]
gpart = ns[:ns.index("elements")]
epart = ns[ns.index("elements"):]
R = Alg(diag(1, 1, 1, 0))

def oriented(name):
    if name == "1": return R.basis(0)
    out = R.basis(0)
    for ch in name[1:]:
        out = R.wedge(out, R.basis(1 << (int(ch) - 1)))
    return out
MOT = [oriented(s) for s in ("e41", "e42", "e43", "e1234", "e23", "e31", "e12", "1")]
FLE = [oriented(s) for s in ("e1", "e2", "e3", "e4", "e423", "e431", "e412", "e321")]

# parse groups: name, order, count, then 3 x (bool, 8 fractions), then first
tok = re.findall(r'"([^"]+)"|(true|false)|\{(-?\d+), (\d+)\}|(?<![\w{-])(\d+)(?=,)', gpart)
groups = []
i = 0
items = []
for t in tok:
    if t[0]: items.append(("name", t[0]))
    elif t[1]: items.append(("bool", t[1] == "true"))
    elif t[2]: items.append(("frac", Q(int(t[2]), int(t[3]))))
    elif t[4]: items.append(("int", int(t[4])))
k = 0
while k < len(items):
    assert items[k][0] == "name", items[k]
    name = items[k][1]; order = items[k + 1][1]; cnt = items[k + 2][1]
    k += 3
    gens = []
    for _ in range(3):
        odd = items[k][1]; k += 1
        cs = [items[k + j][1] for j in range(8)]; k += 8
        gens.append((odd, cs))
    first = items[k][1]; k += 1
    groups.append((name, order, cnt, gens[:cnt], first))
assert len(groups) == 32, len(groups)
fr = [Q(int(a), int(b)) for a, b in re.findall(r"\{(-?\d+), (\d+)\}", epart)]
assert len(fr) == 332 * 9
elements = [tuple(tuple(fr[9 * e + 3 * r + c] for c in range(3)) for r in range(3)) for e in range(332)]

PUBLISHED = {"1": 1, "-1": 2, "2": 2, "m": 2, "2/m": 4, "222": 4, "mm2": 4, "mmm": 8, "4": 4, "-4": 4,
             "4/m": 8, "422": 8, "4mm": 8, "-42m": 8, "4/mmm": 16, "3": 3, "-3": 6, "32": 6, "3m": 6,
             "-3m": 12, "6": 6, "-6": 6, "6/m": 12, "622": 12, "6mm": 12, "-6m2": 12, "6/mmm": 24,
             "23": 12, "m-3": 24, "432": 24, "-43m": 24, "m-3m": 48}

def normalize(v):
    nz = next(c for c in v if c != 0)
    return tuple(c / nz for c in v)

def action(V):
    cols = []
    o = R.transform(R.vec([0, 0, 0, 1]), V)
    w = o[8]
    assert o[1] == o[2] == o[4] == 0, "fixes the origin"
    for i in range(3):
        img = R.transform(R.vec([1 if j == i else 0 for j in range(3)] + [0]), V)
        cols.append([img[1] / w, img[2] / w, img[4] / w])
    return tuple(tuple(cols[c][r] for c in range(3)) for r in range(3))

allok = True
for name, order, cnt, gens, first in groups:
    vs = [R.add(*[R.scale(c, b) for c, b in zip(cs, FLE if odd else MOT)]) for odd, cs in gens]
    one = R.basis(15)
    seen = {normalize(one): one}
    frontier = [one]
    while frontier:
        nxt = []
        for x in frontier:
            for g in vs:
                y = R.agp(g, x)
                key = normalize(y)
                if key not in seen:
                    seen[key] = y; nxt.append(y)
        frontier = nxt
        assert len(seen) <= 96
    mats = sorted({action(v) for v in seen.values()})
    gold = sorted(elements[first:first + order])
    okg = (len(seen) == order == PUBLISHED[name] and mats == gold)
    allok &= okg
    print(("PASS" if okg else "FAIL"), name, "order", len(seen), "golden", order, "published", PUBLISHED[name], "matrices equal" if mats == gold else "matrices DIFFER")
print("5.6p", "PASS" if allok else "FAIL")
