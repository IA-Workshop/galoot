"""Independent check of the values on records/docs-design-rulings.

Written from CONVENTIONS 2.1, 3.1, 3.3, 4.2 and 4.4 alone; shares no code
with the branch's scripts. Multivectors are dicts from blade bitmask to
Fraction. Diagonal metrics only.
"""
from fractions import Fraction as Fr
import itertools, random, sys

fails = []
def check(label, ok):
    print(f"{'ok  ' if ok else 'FAIL'} {label}")
    if not ok:
        fails.append(label)

def popc(x): return bin(x).count("1")

def reorder_sign(a, b):
    # sign of moving basis vectors of b past those of a (canonical order)
    s = 0
    a >>= 1
    while a:
        s += popc(a & b)
        a >>= 1
    return -1 if s & 1 else 1

class Alg:
    def __init__(self, squares):
        self.sq = squares
        self.n = len(squares)
        self.I = (1 << self.n) - 1
    def clean(self, m): return {k: Fr(v) for k, v in m.items() if v != 0}
    def add(self, *ms):
        r = {}
        for m in ms:
            for k, v in m.items(): r[k] = r.get(k, 0) + v
        return self.clean(r)
    def scale(self, c, m): return self.clean({k: c * v for k, v in m.items()})
    def gp(self, a, b):
        r = {}
        for ka, va in a.items():
            for kb, vb in b.items():
                s = reorder_sign(ka, kb)
                common = ka & kb
                for i in range(self.n):
                    if common >> i & 1: s *= self.sq[i]
                if s == 0: continue
                k = ka ^ kb
                r[k] = r.get(k, 0) + s * va * vb
        return self.clean(r)
    def wedge(self, a, b):
        r = {}
        for ka, va in a.items():
            for kb, vb in b.items():
                if ka & kb: continue
                k = ka | kb
                r[k] = r.get(k, 0) + reorder_sign(ka, kb) * va * vb
        return self.clean(r)
    def gr(self, k): return popc(k)
    def rc(self, a):  # u ^ rc(u) = I
        r = {}
        for k, v in a.items():
            c = self.I ^ k
            r[c] = r.get(c, 0) + reorder_sign(k, c) * v
        return self.clean(r)
    def lc(self, a):  # lc(u) ^ u = I
        r = {}
        for k, v in a.items():
            c = self.I ^ k
            r[c] = r.get(c, 0) + reorder_sign(c, k) * v
        return self.clean(r)
    def rev(self, a):
        return self.clean({k: v * (-1) ** (popc(k) * (popc(k) - 1) // 2) for k, v in a.items()})
    def antirev(self, a):
        return self.clean({k: v * (-1) ** ((self.n - popc(k)) * (self.n - popc(k) - 1) // 2) for k, v in a.items()})
    def ginv(self, a):
        return self.clean({k: v * (-1) ** popc(k) for k, v in a.items()})
    def aginv(self, a):
        return self.clean({k: v * (-1) ** (self.n - popc(k)) for k, v in a.items()})
    def anti(self, a, b): return self.lc(self.gp(self.rc(a), self.rc(b)))
    def bulk(self, a):
        r = {}
        for k, v in a.items():
            p = 1
            for i in range(self.n):
                if k >> i & 1: p *= self.sq[i]
            if p: r[k] = p * v
        return self.clean(r)
    def bulk_dual(self, a): return self.rc(self.bulk(a))
    def transform(self, x, V):
        par = {(self.n - popc(k)) % 2 for k in V}
        assert len(par) == 1
        if par == {0}:
            return self.anti(self.anti(V, x), self.antirev(V))
        return self.scale(-1, self.anti(self.anti(V, self.aginv(x)), self.antirev(V)))
    def classical(self, x, V):
        par = {popc(k) % 2 for k in V}
        assert len(par) == 1
        xx = x if par == {0} else self.ginv(x)
        return self.gp(self.gp(V, xx), self.rev(V))
    def grade(self, a, g): return {k: v for k, v in a.items() if popc(k) == g}

E1, E2, E3, E12, E13, E23, E123 = 1, 2, 4, 3, 5, 6, 7
R3 = Alg((1, 1, 1))
def vec(*c): return R3.clean({1 << i: x for i, x in enumerate(c)})
E31 = {E13: -1}

# --- the antiproduct lines (notation-key rule 9, authoring A11) ---
g = R3.rc(vec(1, 0, 0)); h = R3.rc(vec(1, 1, 0))
check("right(e1) = e23", g == {E23: 1})
check("right(e2) = e31", R3.rc(vec(0, 1, 0)) == {E13: -1})
check("right(e1 + e2) = e23 + e31", h == R3.add({E23: 1}, E31))
r = R3.gp(vec(1, 1, 0), vec(1, 0, 0))
check("b a = (1,1,0)(1,0,0) = 1 - e12", r == {0: 1, E12: -1})
V = R3.anti(h, g)
check("(e23 + e31) anti e23 = e123 - e3", V == {E123: 1, E3: -1})
check("e123 - e3 = right(1 - e12)", R3.rc(r) == V)
check("antireverse(e123 - e3) = e123 + e3", R3.antirev(V) == {E123: 1, E3: 1})
L = R3.anti(R3.anti(V, {E3: 1}), {E123: 1, E3: 1})
Rr = R3.anti(V, R3.anti({E3: 1}, {E123: 1, E3: 1}))
check("(e123 - e3) % e3 % (e123 + e3) = 2 e3 (left)", L == {E3: 2})
check("same grouped right", Rr == {E3: 2})
check("V antigrades even, so transform(e3, V) is that sandwich = 2 e3",
      R3.transform({E3: 1}, V) == {E3: 2})
# a % b = left(right(a) * right(b)): check against an independent route,
# De Morgan right(a % b) = right(a) * right(b) only gives the same; use
# instead the dual form with left complements: a % b = right(left(a) * left(b))
random.seed(7)
ok = True
for _ in range(50):
    a = R3.clean({k: random.randint(-3, 3) for k in range(8)})
    b = R3.clean({k: random.randint(-3, 3) for k in range(8)})
    ok &= R3.anti(a, b) == R3.rc(R3.gp(R3.lc(a), R3.lc(b)))
check("De Morgan with either complement (4.2), 50 random pairs", ok)

# --- T1 to T5 ---
s = R3.gp(vec(2, 1, 0), vec(1, 0, 0))
check("s = (2,1,0)(1,0,0) = 2 - e12", s == {0: 2, E12: -1})
check("!s = 2 e123 - e3", R3.rc(s) == {E123: 2, E3: -1})
def T(label, Vv, x, cl, tr, trc):
    check(f"{label} classical {cl}", R3.classical(x, Vv) == vec(*cl))
    check(f"{label} transform(v, V) {tr}", R3.transform(x, Vv) == vec(*tr))
    check(f"{label} transform(v, !V) {trc}", R3.transform(x, R3.rc(Vv)) == vec(*trc))
T("T1", r, vec(0, 0, 1), (0, 0, 2), (0, 0, -2), (0, 0, 2))
T("T2", r, vec(1, 0, 0), (0, 2, 0), (0, 2, 0), (0, 2, 0))
T("T3", s, vec(2, -3, 5), (18, -1, 25), (6, 17, -25), (18, -1, 25))
T("T4", s, vec(1, 0, 0), (3, 4, 0), (-3, 4, 0), (3, 4, 0))
a = vec(1, 0, 0)
x5 = vec(1, 2, 3)
check("T5 -a v a = (-1,2,3)", R3.scale(-1, R3.gp(R3.gp(a, x5), a)) == vec(-1, 2, 3))
T("T5", a, x5, (-1, 2, 3), (1, -2, -3), (-1, 2, 3))
# derivation lines of misconceptions 5.1
st = R3.rev(s)
check("s~ = 2 + e12", st == {0: 2, E12: 1})
check("s e1 s~ = 3e1 + 4e2", R3.gp(R3.gp(s, vec(1,0,0)), st) == vec(3, 4, 0))
check("s e2 s~ = -4e1 + 3e2", R3.gp(R3.gp(s, vec(0,1,0)), st) == vec(-4, 3, 0))
check("s e3 s~ = 5e3", R3.gp(R3.gp(s, vec(0,0,1)), st) == vec(0, 0, 5))
check("s s~ = 5", R3.gp(s, st) == {0: 5})
check("s~ e1 = 2e1 - e2", R3.gp(st, vec(1,0,0)) == vec(2, -1, 0))
check("s~ e1 s = 3e1 - 4e2", R3.gp(R3.gp(st, vec(1,0,0)), s) == vec(3, -4, 0))
check("s~ e2 s = 4e1 + 3e2", R3.gp(R3.gp(st, vec(0,1,0)), s) == vec(4, 3, 0))
check("s~ v s = (-6,-17,25)", R3.gp(R3.gp(st, vec(2,-3,5)), s) == vec(-6, -17, 25))
check("transform(v, s) = -s~ v s", R3.transform(vec(2,-3,5), s) == R3.scale(-1, R3.gp(R3.gp(st, vec(2,-3,5)), s)))
check("cos 3/5, sin 4/5 for s/|s|: 3e1+4e2 /5 is unit", 9 + 16 == 25)

# --- card section claims (misconceptions 5.2) ---
def rand_versor(A, nvec, par_vectors):
    # product of random vectors (classical versor) in A
    V = {0: Fr(1)}
    for _ in range(nvec):
        v = A.clean({1 << i: random.randint(-3, 3) for i in range(A.n)})
        if not v: v = {1: Fr(1)}
        V = A.gp(V, v)
    return V
def rand_vec(A): return A.clean({1 << i: random.randint(-4, 4) for i in range(A.n)})
def rand_mv(A): return A.clean({k: random.randint(-3, 3) for k in range(1 << A.n)})

ok = True
for n in range(1, 6):
    A = Alg((1,) * n)
    for nv in (1, 2, 3):
        for _ in range(20):
            Vv = rand_versor(A, nv, None)
            if not Vv: continue
            x = rand_mv(A)
            ok &= A.transform(x, A.rc(Vv)) == A.classical(x, Vv)
check("R(n,0,0), n=1..5: transform(x, !V) = classical(x, V), all x", ok)

forms = [(1, 1, -1), (1, -1), (1, 1, 1, -1), (-1, -1), (-1, -1, -1), (1, -1, -1), (1, 1, -1, -1)]
for sq in forms:
    A = Alg(sq)
    okb = True; okc = True
    for nv in (1, 2):
        for _ in range(20):
            Vv = rand_versor(A, nv, None)
            if not Vv: continue
            x = rand_mv(A)
            okb &= A.transform(x, A.bulk_dual(Vv)) == A.classical(x, Vv)
            okc &= A.transform(x, A.rc(Vv)) == A.classical(x, Vv)
    indefinite = len(set(sq)) > 1
    check(f"R{sq}: transform(x, bulk_dual V) = classical", okb)
    check(f"R{sq}: with ! in place of bulk_dual {'fails (indefinite)' if indefinite else 'holds (definite)'}",
          (not okc) if indefinite else okc)

# Theorem D / card: Euclidean versor complemented in rigid(3) acts on points
# as on vectors, with the weight times lambda = V V~.
RG = Alg((1, 1, 1, 0))
ok = True
for nv in (1, 2, 3):
    for _ in range(20):
        Vv = {0: Fr(1)}
        for _ in range(nv):
            v = RG.clean({1 << i: random.randint(-3, 3) for i in range(3)}) or {1: Fr(1)}
            Vv = RG.gp(Vv, v)
        lam = RG.gp(Vv, RG.rev(Vv))
        lam = lam.get(0, 0)
        p = RG.clean({1: random.randint(-4, 4), 2: random.randint(-4, 4), 4: random.randint(-4, 4), 8: 1})
        out = RG.transform(p, RG.rc(Vv))
        cl3 = R3.classical({k: v for k, v in p.items() if k < 8}, {k: v for k, v in Vv.items()})
        want = dict(cl3); want[8] = lam
        ok &= out == RG.clean(want)
check("rigid(3): transform(point, !V) = (classical(v), lambda) with lambda = V V~", ok)

print("ALL HOLD" if not fails else f"FAILURES {len(fails)}")
sys.exit(1 if fails else 0)
