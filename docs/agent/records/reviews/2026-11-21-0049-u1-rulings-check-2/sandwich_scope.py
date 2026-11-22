"""Exact checks of the values and scope statements that commits 74fd859..ad0bf70
add to the documentation design records.

The algebra is written from CONVENTIONS 1.1, 2.1, 3.1 to 3.3, 4.1, 4.2 and 4.4
alone, for any symmetric Gram matrix: a blade is the wedge of its generators
(1.1), the geometric product is built from the contraction e_i . X and the wedge
(e_i X = e_i . X + e_i ^ X), the bulk is the outermorphism of the Gram matrix
(3.2), and the bulk dual is its right complement (3.3). Rational arithmetic only.
Standard library only; run with python3 -I.
"""

import itertools
import random
from fractions import Fraction as F

results = []


def report(label, ok):
    results.append(ok)
    print(("ok    " if ok else "FAIL  ") + label)


def popcount(m):
    return bin(m).count("1")


def wedge_sign(a, b):
    """Sign of e_a ^ e_b for disjoint masks, from reordering into increasing order."""
    s, x = 1, a >> 1
    while x:
        if popcount(x & b) % 2:
            s = -s
        x >>= 1
    return s


class Alg:
    def __init__(self, gram):
        self.g = [[F(v) for v in row] for row in gram]
        self.n = len(gram)
        self.top = (1 << self.n) - 1
        self._gp = {}

    # -- multivectors are dicts mask -> Fraction ------------------------------
    @staticmethod
    def clean(x):
        return {k: v for k, v in x.items() if v != 0}

    def add(self, *xs):
        out = {}
        for x in xs:
            for k, v in x.items():
                out[k] = out.get(k, 0) + v
        return self.clean(out)

    def scale(self, s, x):
        return self.clean({k: s * v for k, v in x.items()})

    def vec(self, coeffs):
        return self.clean({1 << i: F(c) for i, c in enumerate(coeffs)})

    def wedge_blades(self, a, b):
        return {} if a & b else {a | b: F(wedge_sign(a, b))}

    def wedge(self, x, y):
        out = {}
        for a, u in x.items():
            for b, v in y.items():
                if not a & b:
                    out[a | b] = out.get(a | b, 0) + wedge_sign(a, b) * u * v
        return self.clean(out)

    def contract_gen(self, i, b):
        """e_i contracted onto the wedge blade b (left contraction)."""
        out = {}
        idx = [j for j in range(self.n) if b >> j & 1]
        for pos, j in enumerate(idx):
            gij = self.g[i][j]
            if gij:
                rest = b & ~(1 << j)
                out[rest] = out.get(rest, 0) + (-1) ** pos * gij
        return self.clean(out)

    def gen_times_blade(self, i, b):
        """e_i (geometric) e_b = e_i . e_b + e_i ^ e_b."""
        return self.add(self.contract_gen(i, b), self.wedge_blades(1 << i, b))

    def gp_blades(self, a, b):
        key = (a, b)
        if key in self._gp:
            return self._gp[key]
        if a == 0:
            res = {b: F(1)}
        else:
            i = (a & -a).bit_length() - 1
            rest = a & ~(1 << i)
            # e_a = e_i ^ e_rest = e_i e_rest - e_i . e_rest
            left = self.gp(self.gen_left(i), self.gp_blades(rest, b))
            corr = self.gp(self.contract_gen(i, rest), {b: F(1)})
            res = self.add(left, self.scale(-1, corr))
        self._gp[key] = res
        return res

    def gen_left(self, i):
        return {1 << i: F(1)}

    def gp(self, x, y):
        out = {}
        for a, u in x.items():
            for b, v in y.items():
                if popcount(a) == 1:
                    term = self.gen_times_blade(a.bit_length() - 1, b)
                else:
                    term = self.gp_blades(a, b)
                for k, w in term.items():
                    out[k] = out.get(k, 0) + u * v * w
        return self.clean(out)

    def gp3(self, x, y, z):
        return self.gp(self.gp(x, y), z)

    # -- unary operations (2.1, 3.1, 3.2) -------------------------------------
    def reverse(self, x):
        return {k: v * (-1) ** (popcount(k) * (popcount(k) - 1) // 2) for k, v in x.items()}

    def grade_inv(self, x):
        return {k: v * (-1) ** popcount(k) for k, v in x.items()}

    def antigrade(self, k):
        return self.n - popcount(k)

    def antireverse(self, x):
        return {k: v * (-1) ** (self.antigrade(k) * (self.antigrade(k) - 1) // 2) for k, v in x.items()}

    def antigrade_inv(self, x):
        return {k: v * (-1) ** self.antigrade(k) for k, v in x.items()}

    def right(self, x):
        # u ^ right(u) = 1-bar
        return self.clean({self.top & ~k: v * wedge_sign(k, self.top & ~k) for k, v in x.items()})

    def left(self, x):
        # left(u) ^ u = 1-bar
        return self.clean({self.top & ~k: v * wedge_sign(self.top & ~k, k) for k, v in x.items()})

    def bulk(self, x):
        """Outermorphism of the Gram matrix: e_A -> sum_B det g[B, A] e_B."""
        out = {}
        for a, v in x.items():
            ia = [j for j in range(self.n) if a >> j & 1]
            for b in range(1 << self.n):
                if popcount(b) != len(ia):
                    continue
                ib = [j for j in range(self.n) if b >> j & 1]
                d = det([[self.g[r][c] for c in ia] for r in ib])
                if d:
                    out[b] = out.get(b, 0) + v * d
        return self.clean(out)

    def bulk_dual(self, x):
        return self.right(self.bulk(x))

    # -- products and transform (4.2, 4.4) -----------------------------------
    def antiprod(self, x, y):
        return self.left(self.gp(self.right(x), self.right(y)))

    def transform(self, x, V):
        par = {self.antigrade(k) % 2 for k in V}
        assert len(par) == 1, "mixed antigrade parity"
        Vt = self.antireverse(V)
        if par == {0}:
            return self.antiprod(self.antiprod(V, x), Vt)
        return self.scale(-1, self.antiprod(self.antiprod(V, self.antigrade_inv(x)), Vt))

    def classical(self, x, V):
        par = {popcount(k) % 2 for k in V}
        assert len(par) == 1
        xx = x if par == {0} else self.grade_inv(x)
        return self.gp3(V, xx, self.reverse(V))


def det(m):
    m = [row[:] for row in m]
    k = len(m)
    if k == 0:
        return F(1)
    d = F(1)
    for c in range(k):
        p = next((r for r in range(c, k) if m[r][c] != 0), None)
        if p is None:
            return F(0)
        if p != c:
            m[c], m[p] = m[p], m[c]
            d = -d
        d *= m[c][c]
        for r in range(c + 1, k):
            f = m[r][c] / m[c][c]
            for cc in range(c, k):
                m[r][cc] -= f * m[c][cc]
    return d


def diag(*squares):
    n = len(squares)
    return [[squares[i] if i == j else 0 for j in range(n)] for i in range(n)]


def R(p, q, r=0):
    return diag(*([1] * p + [-1] * q + [0] * r))


def coords(x, n):
    return tuple(x.get(1 << i, 0) for i in range(n))


rng = random.Random(20261121)


def rand_vec(A):
    while True:
        v = A.vec([rng.randint(-3, 3) for _ in range(A.n)])
        sq = A.gp(v, v).get(0, 0)
        if v and sq != 0 and set(A.gp(v, v)) <= {0}:
            return v


def rand_versor(A, k):
    V = {0: F(1)}
    for _ in range(k):
        V = A.gp(V, rand_vec(A))
    return V


def rand_full(A):
    return A.clean({m: F(rng.randint(-4, 4)) for m in range(1 << A.n)})


# --- 0. The algebra against CONVENTIONS -------------------------------------
C3 = Alg([[1, 0, 0, 0, 0], [0, 1, 0, 0, 0], [0, 0, 1, 0, 0], [0, 0, 0, 0, -1], [0, 0, 0, -1, 0]])
for name, A in (("conformal(3)", C3), ("[[1,1,0],[1,2,0],[0,0,1]]", Alg([[1, 1, 0], [1, 2, 0], [0, 0, 1]]))):
    ok = True
    for i in range(A.n):
        for j in range(A.n):
            ei, ej = A.gen_left(i), A.gen_left(j)
            ok &= A.add(A.gp(ei, ej), A.gp(ej, ei)) == A.clean({0: 2 * A.g[i][j]})
    for _ in range(20):
        a, b, c = rand_full(A), rand_full(A), rand_full(A)
        ok &= A.gp(A.gp(a, b), c) == A.gp(a, A.gp(b, c))
        ok &= A.left(A.right(a)) == a
        aa, bb = A.vec([rng.randint(-3, 3) for _ in range(A.n)]), A.vec([rng.randint(-3, 3) for _ in range(A.n)])
        dot = sum(aa.get(1 << i, 0) * A.g[i][j] * bb.get(1 << j, 0) for i in range(A.n) for j in range(A.n))
        ok &= A.wedge(aa, A.bulk_dual(bb)) == A.clean({A.top: dot})
    report(f"{name}: e_i e_j + e_j e_i = 2 g_ij, associativity, left(right(a)) = a, a ^ b* = (a . b) 1-bar", ok)
report("conformal(3): e4 e5 = -1 + e45 (CONVENTIONS 1.1)", C3.gp(C3.gen_left(3), C3.gen_left(4)) == {0: -1, 0b11000: 1})
report("conformal(3): bulk of e4 is -e5 (CONVENTIONS 1.8)", C3.bulk(C3.gen_left(3)) == {0b10000: -1})

# --- 1. R5 of authoring-rules.md -------------------------------------------
E3 = Alg(R(3, 0))
t = E3.gp(E3.vec([1, 0, 3]), E3.vec([0, 0, 1]))
report("t = (1, 0, 3)(0, 0, 1) = 3 + e13", t == {0: 3, 0b101: 1})
tr = E3.reverse(t)
report("t~ = 3 - e13", tr == {0: 3, 0b101: -1})
e = [E3.vec([1, 0, 0]), E3.vec([0, 1, 0]), E3.vec([0, 0, 1])]
report("t e1 t~ = (8, 0, -6)", E3.gp3(t, e[0], tr) == E3.vec([8, 0, -6]))
report("t e2 t~ = (0, 10, 0)", E3.gp3(t, e[1], tr) == E3.vec([0, 10, 0]))
report("t e3 t~ = (6, 0, 8)", E3.gp3(t, e[2], tr) == E3.vec([6, 0, 8]))
report("t~ e1 t = (8, 0, 6)", E3.gp3(tr, e[0], t) == E3.vec([8, 0, 6]))
report("t~ e3 t = (-6, 0, 8)", E3.gp3(tr, e[2], t) == E3.vec([-6, 0, 8]))
v = E3.vec([4, -1, 2])
report("t v t~ = (44, -10, -8)", E3.gp3(t, v, tr) == E3.vec([44, -10, -8]))
report("transform(v, t) = -t~ v t = (-20, 10, -40)",
       E3.transform(v, t) == E3.vec([-20, 10, -40]) == E3.scale(-1, E3.gp3(tr, v, t)))
report("!t = 3 e123 - e2", E3.right(t) == {0b111: 3, 0b010: -1})
report("transform(v, !t) = (44, -10, -8)", E3.transform(v, E3.right(t)) == E3.vec([44, -10, -8]))
report("t t~ = 10; cos = 4/5, sin = 3/5 in the e1 e3 plane",
       E3.gp(t, tr) == {0: 10} and E3.gp3(t, e[0], tr) == E3.vec([8, 0, -6]))
# a quarter-turn check: the trap and the target differ on every coordinate
tgt, trap = coords(E3.gp3(t, v, tr), 3), coords(E3.transform(v, t), 3)
report("target and trap differ in every coordinate", all(a != b for a, b in zip(tgt, trap)))


# --- 2. Theorem B in its stated domain R(p, q, 0) ----------------------------
def check_form(name, gram, trials=12, kinds=None, use="bulk_dual", expect=True, full_x=True):
    A = Alg(gram)
    holds_all, fails = True, 0
    total = 0
    for k in (kinds or range(1, A.n + 1)):
        for _ in range(trials):
            V = rand_versor(A, k)
            x = rand_full(A) if full_x else rand_vec(A)
            W = A.bulk_dual(V) if use == "bulk_dual" else A.right(V)
            ok = A.transform(x, W) == A.classical(x, V)
            total += 1
            if not ok:
                holds_all = False
                fails += 1
    if expect:
        report(f"{name}: transform(x, {use}(V)) = classical, {total} versors of 1 to n vectors, full x", holds_all)
    else:
        report(f"{name}: transform(x, {use}(V)) differs from classical on {fails} of {total}", fails > 0)
    return holds_all


for n in range(1, 6):
    for p in range(n + 1):
        q = n - p
        check_form(f"R({p}, {q}, 0)", R(p, q), trials=6 if n == 5 else 10)

# `!` in place of bulk_dual: holds when p = 0 or q = 0, fails when both nonzero
for n in range(1, 6):
    for p in range(n + 1):
        q = n - p
        both = p > 0 and q > 0
        check_form(f"R({p}, {q}, 0)", R(p, q), trials=6, use="right", expect=not both)

# --- 3. Just outside the domain ----------------------------------------------
outside = [
    ("diag(2, 1, 1)", diag(2, 1, 1), False),
    ("diag(1, 1, 4)", diag(1, 1, 4), False),
    ("diag(2, -1)", diag(2, -1), False),
    ("diag(-2, 1, 1)", diag(-2, 1, 1), False),
    ("R(3, 0, 1) = rigid(3)", R(3, 0, 1), False),
    ("R(2, 0, 1) = rigid(2)", R(2, 0, 1), False),
    ("non-diagonal [[1,1,0],[1,2,0],[0,0,1]]", [[1, 1, 0], [1, 2, 0], [0, 0, 1]], False),
]
for name, gram, _ in outside:
    check_form(name, gram, trials=8, expect=False)

# ±1 diagonal forms not in the R(p, q, 0) order: inside Theorem B's proof
check_form("diag(-1, 1, 1)", diag(-1, 1, 1), trials=8)
check_form("diag(1, -1, 1, -1)", diag(1, -1, 1, -1), trials=5)

# Forms whose Gram matrix is its own inverse, outside R(p, q, 0) and not diagonal
c3 = [[1, 0, 0, 0, 0], [0, 1, 0, 0, 0], [0, 0, 1, 0, 0], [0, 0, 0, 0, -1], [0, 0, 0, -1, 0]]
check_form("conformal(3) (not proved)", c3, trials=3)
check_form("Witt [[0,1],[1,0]] + (1) (not proved)", [[0, 1, 0], [1, 0, 0], [0, 0, 1]], trials=8)
check_form("[[3/5,4/5],[4/5,-3/5]] + (1), G G = I (not proved)",
           [[F(3, 5), F(4, 5), 0], [F(4, 5), F(-3, 5), 0], [0, 0, 1]], trials=8)
# Every generator squares to 1 or -1, but the form is not diagonal: outside R(p, q, 0)
check_form("[[1,1,0],[1,-1,0],[0,0,1]], squares 1, -1, 1, not diagonal",
           [[1, 1, 0], [1, -1, 0], [0, 0, 1]], trials=8, expect=False)

# Rigid(3): a Euclidean versor complemented is exact for points; a translator is not
A = Alg(R(3, 0, 1))
Vr = A.gp(A.vec([1, 1, 0, 0]), A.vec([1, 0, 0, 0]))
pt = A.vec([2, -3, 5, 1])
report("rigid(3): Euclidean rotor, bulk dual on a point equals the sandwich",
       A.transform(pt, A.bulk_dual(Vr)) == A.classical(pt, Vr))
Vt = A.gp(A.vec([1, 0, 0, 0]), A.vec([1, 0, 0, 1]))  # 1 + e14, a translator
report("rigid(3): translator 1 + e14, bulk dual does not give the sandwich on a point",
       Vt == {0: 1, 0b1001: 1} and A.transform(pt, A.bulk_dual(Vt)) != A.classical(pt, Vt))

# The single counterexample of the corrected record (commit be37af3)
D = Alg(diag(2, 1, 1))
r = D.gp(D.vec([1, 1, 0]), D.vec([1, 0, 0]))
x = D.vec([2, -3, 5])
report("diag(2, 1, 1): r x r~ = (16, 10, 30)", D.classical(x, r) == D.vec([16, 10, 30]))
report("diag(2, 1, 1): transform(x, bulk_dual(r)) = (40, 28, 60)",
       D.transform(x, D.bulk_dual(r)) == D.vec([40, 28, 60]))

# Section 5.1's T3 and the other changed lines (values of misconceptions.md)
s = E3.gp(E3.vec([2, 1, 0]), E3.vec([1, 0, 0]))
v3 = E3.vec([2, -3, 5])
report("T3: s = 2 - e12, transform(v, s) = (6, 17, -25), classical and transform(v, !s) = (18, -1, 25)",
       s == {0: 2, 0b11: -1} and E3.transform(v3, s) == E3.vec([6, 17, -25])
       and E3.classical(v3, s) == E3.vec([18, -1, 25]) == E3.transform(v3, E3.right(s)))
for label, (a, b, vv) in {
    "T1": ((1, 0, 0), (1, 1, 0), (0, 0, 1)),
    "T2": ((1, 0, 0), (1, 1, 0), (1, 0, 0)),
}.items():
    rr = E3.gp(E3.vec(b), E3.vec(a))
    w = E3.vec(vv)
    print(f"      {label}: r = {dict(sorted(rr.items()))}, classical {coords(E3.classical(w, rr), 3)}, "
          f"transform(v, r) {coords(E3.transform(w, rr), 3)}, transform(v, !r) {coords(E3.transform(w, E3.right(rr)), 3)}")

print(f"{sum(results)} of {len(results)} ok")
raise SystemExit(0 if all(results) else 1)
