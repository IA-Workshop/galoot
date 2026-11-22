"""Theorem B's bulk-dual form on diagonal forms whose squares are not +-1.

Reuses the algebra of values_check.py (read from this directory, not
imported). Prints, for each form, whether transform(x, bulk_dual(V)) equals
the classical action of V on 60 random vectors, and whether the two are at
least proportional. Expected: neither, on every form listed.

Run: python3 -I bulk_dual_forms.py
"""
import pathlib
import random

src = (pathlib.Path(__file__).resolve().parent / "values_check.py").read_text()
ns = {}
exec(src.split("E1, E2, E3, E12")[0], ns)
Alg = ns["Alg"]

random.seed(3)
for sq in [(2, 1, 1), (2, 1), (3, -1), (1, 1, 2), (-2, 1, 1)]:
    A = Alg(sq)
    equal = True
    proportional = True
    for nv in (1, 2):
        for _ in range(30):
            V = {0: 1}
            for _ in range(nv):
                v = A.clean({1 << i: random.randint(-3, 3) for i in range(A.n)}) or {1: 1}
                V = A.gp(V, v)
            x = A.clean({1 << i: random.randint(-3, 3) for i in range(A.n)})
            t = A.transform(x, A.bulk_dual(V))
            c = A.classical(x, V)
            equal &= t == c
            if c:
                k0 = next(iter(c))
                f = t.get(k0, 0) / c[k0]
                proportional &= all(t.get(k, 0) == f * c.get(k, 0) for k in set(t) | set(c))
    print(sq, "equal" if equal else "not equal", "| proportional" if proportional else "| not proportional")
