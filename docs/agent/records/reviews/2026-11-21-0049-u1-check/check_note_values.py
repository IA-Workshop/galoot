"""Check sampled values of research note 0028 with the independent ga.py."""
import sys, os
from fractions import Fraction as F
import sympy as sp

sys.path.insert(0, os.path.dirname(__file__))
from ga import Alg, vec, mvb, show, neg, scale, add, sub, clean, popcount

ok = True


def check(name, cond):
    global ok
    print(("PASS " if cond else "FAIL ") + name)
    ok &= bool(cond)


def G(alg, x):
    out = {}
    for m, c in x.items():
        f = 1
        for i in range(alg.n):
            if m >> i & 1:
                f *= alg.sq[i]
        out[m] = f * c
    return clean(out)


def bulk_dual(alg, x):
    return alg.right(G(alg, x))


def classical(alg, x, V):
    gs = {popcount(m) % 2 for m in V}
    assert len(gs) == 1
    g = gs.pop()
    xx = alg.ginv(x) if g else x
    return alg.gp(alg.gp(V, xx), alg.rev(V))


def sym_all(alg, prefix):
    return {m: sp.Symbol(f"{prefix}{m}") for m in alg.blades()}


def sym_parity(alg, parity, prefix):
    return {m: sp.Symbol(f"{prefix}{m}") for m in alg.blades() if popcount(m) % 2 == parity}


# Theorem B in a negative-definite and an indefinite form; Theorem A, n = 3.
for sq, label in [([-1, -1, -1], "R(0,3,0)"), ([1, 1, -1], "R(2,1,0)"), ([1, -1], "R(1,1,0)")]:
    A = Alg(sq)
    x = sym_all(A, "x")
    for g in (0, 1):
        V = sym_parity(A, g, "v")
        cl = classical(A, x, V)
        tb = A.transform_full(x, bulk_dual(A, V))
        tc = A.transform_full(x, A.right(V))
        check(f"{label} g={g}: transform(x, bulk_dual V) = C_V(x)", sub(tb, cl) == {})
        print(f"     {label} g={g}: transform(x, right V) = C_V(x)? {sub(tc, cl) == {}}")

# Theorem D in rigid(3): V on e1..e3 of either parity, x generic.
R4 = Alg([1, 1, 1, 0])
x = sym_all(R4, "x")
for g in (0, 1):
    V = {m: sp.Symbol(f"v{m}") for m in range(8) if popcount(m) % 2 == g}
    check(f"Theorem D rigid(3) g={g}: transform(x, right V) = C_V(x)",
          sub(R4.transform_full(x, R4.right(V)), classical(R4, x, V)) == {})

# Section 2.7: R(2,0,0) quarter turn 1 - e12 agrees on every vector, 2 - e12 not.
E2 = Alg([1, 1])
vv = {1: sp.Symbol("a"), 2: sp.Symbol("b")}
for V, expect, nm in [(mvb((1, ()), (-1, (1, 2))), True, "1 - e12"), (mvb((2, ()), (-1, (1, 2))), False, "2 - e12")]:
    same = sub(E2.transform_full(vv, V), classical(E2, vv, V)) == {}
    check(f"R(2,0,0) transform(v, {nm}) equals classical on every vector: {expect}", same == expect)

# 3.3: !!x in R(3,0,0) and R(2,0,0)
E3 = Alg([1, 1, 1])
check("R(3,0,0): right(right(x)) = x for all x", sub(E3.right(E3.right(sym_all(E3, "x"))), sym_all(E3, "x")) == {})
check("R(2,0,0): right(right(e1)) = -e1", E2.right(E2.right(vec(1, 0))) == vec(-1, 0))

# 2.8 values in R(3,0,0)
a, b, c = vec(1, 0, 0), vec(1, 1, 0), vec(2, 1, 0)
r, s = E3.gp(b, a), E3.gp(c, a)
check("r = 1 - e12, !r = e123 - e3", r == mvb((1, ()), (-1, (1, 2))) and E3.right(r) == mvb((1, (1, 2, 3)), (-1, (3,))))
v = vec(2, -3, 5)
check("(2,-3,5): classical (6,4,10), transform(v,r) (6,4,-10), transform(v,!r) (6,4,10)",
      classical(E3, v, r) == vec(6, 4, 10) and E3.transform_full(v, r) == vec(6, 4, -10)
      and E3.transform_full(v, E3.right(r)) == vec(6, 4, 10))
rows = [((1, 0, 0), (3, 4, 0), (-3, 4, 0)), ((0, 1, 0), (-4, 3, 0), (-4, -3, 0)), ((0, 0, 1), (0, 0, 5), (0, 0, -5))]
for vin, cl, tr in rows:
    vv3 = vec(*vin)
    check(f"s on {vin}: classical {cl}, transform(v,s) {tr}, transform(v,!s) {cl}",
          classical(E3, vv3, s) == vec(*cl) and E3.transform_full(vv3, s) == vec(*tr)
          and E3.transform_full(vv3, E3.right(s)) == vec(*cl))
w = vec(1, 2, 3)
check("one mirror a: -a v a = (-1,2,3), transform(v,a) = (1,-2,-3), transform(v,!a) = (-1,2,3)",
      neg(E3.gp(E3.gp(a, w), a)) == vec(-1, 2, 3) and E3.transform_full(w, a) == vec(1, -2, -3)
      and E3.transform_full(w, E3.right(a)) == vec(-1, 2, 3))
# 3.1 table, middle column
gpl, hpl = E3.right(a), E3.right(b)
check("g = !a = e23, h = !b = e23 + e31", gpl == mvb((1, (2, 3))) and hpl == mvb((1, (2, 3)), (1, (3, 1))))
check("transform((1,2,3), g) = (-1,2,3)", E3.transform_full(w, gpl) == vec(-1, 2, 3))
VV = E3.agp(hpl, gpl)
check("h % g = !r", VV == E3.right(r))
check("V % antirev(V) = 2 antiscalar (R(3,0,0))", E3.agp(VV, E3.antirev(VV)) == {7: 2})
check("transform(e1, V) = 2 e2, transform(e3, V) = 2 e3",
      E3.transform_full(vec(1, 0, 0), VV) == vec(0, 2, 0) and E3.transform_full(vec(0, 0, 1), VV) == vec(0, 0, 2))

# rigid(3) values: planes (gx,gy,gz,gw) on e423, e431, e412, e321
def plane(gx, gy, gz, gw):
    return mvb((gx, (4, 2, 3)), (gy, (4, 3, 1)), (gz, (4, 1, 2)), (gw, (3, 2, 1)))


def motor(vx, vy, vz, vw, mx, my, mz, mw):
    return mvb((vx, (4, 1)), (vy, (4, 2)), (vz, (4, 3)), (vw, (1, 2, 3, 4)),
               (mx, (2, 3)), (my, (3, 1)), (mz, (1, 2)), (mw, ()))


def line(vx, vy, vz, mx, my, mz):
    return mvb((vx, (4, 1)), (vy, (4, 2)), (vz, (4, 3)), (mx, (2, 3)), (my, (3, 1)), (mz, (1, 2)))


P = lambda x, y, z, w=1: vec(x, y, z, w)
check("rigid(3): !(1 - e12) = motor (0,0,1,1,0,0,0,0)", R4.right(mvb((1, ()), (-1, (1, 2)))) == motor(0, 0, 1, 1, 0, 0, 0, 0))
check("rigid(3): !e1 = plane (1,0,0,0)", R4.right(vec(1, 0, 0, 0)) == plane(1, 0, 0, 0))
Q = motor(0, 0, 1, 1, 0, 0, 0, 0)
check("Q: (2,-3,5,1) -> (6,4,10,2)", R4.transform_full(P(2, -3, 5), Q) == P(6, 4, 10, 2))
check("plane x=0 as flector: (1,2,3,1) -> (-1,2,3,1)", R4.transform_full(P(1, 2, 3), plane(1, 0, 0, 0)) == P(-1, 2, 3))
check("compose of planes !b, !a = Q", R4.agp(plane(1, 1, 0, 0), plane(1, 0, 0, 0)) == Q)
# 3.2
z0, z1 = plane(0, 0, 1, 0), plane(0, 0, 1, -1)
M = R4.agp(z1, z0)
check("compose(z=1, z=0) = motor (0,0,0,1,0,0,1,0)", M == motor(0, 0, 0, 1, 0, 0, 1, 0))
check("that motor: (1,2,3,1) -> (1,2,5,1)", R4.transform_full(P(1, 2, 3), M) == P(1, 2, 5))
check("reflect in z=0 then z=1 equals the motor",
      R4.transform_full(R4.transform_full(P(1, 2, 3), z0), z1) == P(1, 2, 5))
x1, y1 = plane(1, 0, 0, -1), plane(0, 1, 0, -1)
meet = R4.left(R4.wedge(R4.right(x1), R4.right(y1)))
check("x=1 meet y=1 = line (0,0,-1,-1,1,0)", meet == line(0, 0, -1, -1, 1, 0))
M2 = R4.agp(y1, x1)
check("compose(y=1, x=1) = motor (0,0,1,0,1,-1,0,0)", M2 == motor(0, 0, 1, 0, 1, -1, 0, 0))
check("lambda = 1", R4.agp(M2, R4.antirev(M2)) == {15: 1})
check("origin -> (2,2,0); (1,1,5) stays; (2,1,0) -> (0,1,0)",
      R4.transform_full(P(0, 0, 0), M2) == P(2, 2, 0) and R4.transform_full(P(1, 1, 5), M2) == P(1, 1, 5)
      and R4.transform_full(P(2, 1, 0), M2) == P(0, 1, 0))
pm = R4.left(R4.wedge(R4.right(z0), R4.right(z1)))
check("z=0 meet z=1 = line (0,0,0,0,0,-1)", pm == line(0, 0, 0, 0, 0, -1))

print("ALL PASS" if ok else "SOME FAILED")
