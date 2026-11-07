"""Term counts of the transform's kernels in rigid(3), the expected values
of the kernel test in test/core/transform_test.cpp, with an algebra of its
own that shares no code with the library: in the antiproduct of rigid(3), blades A and B multiply to a
nonzero blade exactly when their complements share no null generator e4,
that is when A or B holds e4; the result's mask is the complement of the
symmetric difference of the complements.

Run: python3 -I counts.py. For each versor and object it prints the terms
of V % x (whole), of that with V (whole), of the first stage restricted to
the middle blades the kept terms read, of the second stage onto x's
blades, and the number of dropped blades.
"""
N = 4; FULL = 15; E4 = 8
def anti(a, b):
    ca, cb = FULL ^ a, FULL ^ b
    if ca & cb & E4: return None
    return FULL ^ (ca ^ cb)
def masks(names):
    out = []
    for nm in names.split():
        out.append(0 if nm == "1" else sum(1 << (int(c) - 1) for c in nm[1:]))
    return out
point = masks("e1 e2 e3 e4"); line = masks("e14 e24 e34 e23 e13 e12")
plane = masks("e234 e134 e124 e123")
motor = masks("e14 e24 e34 e1234 e23 e13 e12 1")
flector = masks("e1 e2 e3 e4 e234 e134 e124 e123")
def counts(V, X):
    first = [(i, j, anti(v, x)) for i, v in enumerate(V) for j, x in enumerate(X) if anti(v, x) is not None]
    middle = sorted({m for _, _, m in first})
    second = [(m, k, anti(m, v)) for m in middle for k, v in enumerate(V) if anti(m, v) is not None]
    kept = [t for t in second if t[2] in X]
    read = {t[0] for t in kept}
    first_kept = [t for t in first if t[2] in read]
    reached = {t[2] for t in second}
    dropped = [r for r in reached if r not in X]
    return len(first), len(second), len(first_kept), len(kept), len(dropped)
for vn, V in (("motor", motor), ("flector", flector)):
    for xn, X in (("point", point), ("line", line), ("plane", plane), ("motor", motor), ("flector", flector)):
        f, s, fk, k, d = counts(V, X)
        print(f"{vn:8} {xn:8} whole {f}+{s}={f+s} restricted {fk}+{k}={fk+k} dropped blades {d}")
