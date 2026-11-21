"""Independent exact check of what transform computes (conventions 4.4)."""
import sys
from fractions import Fraction as F
import itertools
import sympy as sp

sys.path.insert(0, __import__("os").path.dirname(__file__))
from ga import Alg, vec, mvb, show, neg, scale, add, sub, clean, popcount

ok = True


def check(name, cond):
    global ok
    print(("PASS " if cond else "FAIL ") + name)
    ok &= bool(cond)


# --- Sanity of the definitions against the specification -----------------
R4 = Alg([1, 1, 1, 0])  # rigid(3)
full = {R4.full: 1}
for m in R4.blades():
    u = {m: 1}
    check(f"rigid(3) u ^ right(u) = antiscalar, mask {m}", R4.wedge(u, R4.right(u)) == full)
    check(f"rigid(3) left(u) ^ u = antiscalar, mask {m}", R4.wedge(R4.left(u), u) == full)
# Spec 1.6 oriented names: right complement of e1 is e234 = e423
check("rigid(3) right(e1) = e423", R4.right(vec(1)) == mvb((1, (4, 2, 3))))
# antiscalar is the identity of the antiproduct
x = mvb((2, (1,)), (3, (2, 3)), (5, ()), (7, (1, 2, 3, 4)))
check("antiscalar is the antiproduct identity", R4.agp(full, x) == x and R4.agp(x, full) == x)

# --- R(3,0,0): the classical rotor of the plan's chapter 5 ---------------
E3 = Alg([1, 1, 1])
a = vec(1, 0, 0)
b = vec(1, 1, 0)
r = E3.gp(b, a)  # 1 - e12
print("r = b a =", show(r, 3), " parity:", E3.parity(r))
rr = E3.rev(r)
for v, nm in [(vec(1, 0, 0), "e1"), (vec(0, 1, 0), "e2"), (vec(0, 0, 1), "e3"), (vec(1, 2, 3), "(1,2,3)")]:
    classical = E3.gp(E3.gp(r, v), rr)
    t = E3.transform_full(v, r)
    raw = E3.agp(E3.agp(r, v), E3.antirev(r))
    tr = E3.transform_full(v, E3.right(r))
    print(f"  v={nm}: r v ~r = {show(classical,3)} | transform(v,r) = {show(t,3)} | "
          f"r%v%antirev(r) = {show(raw,3)} | transform(v,right(r)) = {show(tr,3)}")
check("R3 quarter turn: transform(e1,r) = r e1 ~r = 2 e2",
      E3.transform_full(vec(1, 0, 0), r) == E3.gp(E3.gp(r, vec(1, 0, 0)), rr) == vec(0, 2, 0))
check("R3 quarter turn: transform(e3,r) = -2 e3 while r e3 ~r = +2 e3",
      E3.transform_full(vec(0, 0, 1), r) == vec(0, 0, -2)
      and E3.gp(E3.gp(r, vec(0, 0, 1)), rr) == vec(0, 0, 2))

# --- Symbolic: general rotor and general x of each grade in R(n,0,0) ------
def sym_mv(alg, grade, prefix):
    out = {}
    for m in alg.blades():
        if popcount(m) == grade:
            out[m] = sp.Symbol(f"{prefix}{m}")
    return out


for n in [2, 3, 4, 5]:
    A = Alg([1] * n)
    rot = {0: sp.Symbol("s")}
    rot.update(sym_mv(A, 2, "b"))
    for k in range(n + 1):
        xk = sym_mv(A, k, "x")
        t = A.transform_full(xk, rot)
        inv = scale(A.gp(A.gp(A.rev(rot), xk), rot), (-1) ** k)  # r~ x^ r
        check(f"R({n},0,0) grade {k}: transform(x, r) = (-1)^k ~r x r", sub(t, inv) == {})
        cl = A.gp(A.gp(rot, xk), A.rev(rot))
        trc = A.transform_full(xk, A.right(rot))
        sign = 1
        check(f"R({n},0,0) grade {k}: transform(x, right(r)) = r x ~r", sub(trc, cl) == {})
        trc2 = A.transform_full(xk, A.left(rot))
        check(f"R({n},0,0) grade {k}: transform(x, left(r)) = r x ~r", sub(trc2, cl) == {})
        if k == 1:
            check(f"R({n},0,0) vectors: transform(v,r) != r v ~r in general", sub(t, cl) != {})

# --- rga3 motors ----------------------------------------------------------
P = lambda x, y, z, w=1: vec(x, y, z, w)
anti = {R4.full: 1}


def rga3_line(p, q):
    return R4.wedge(p, q)


# rotation by 90 degrees about the z axis: l = e43, l sin45 + 1bar cos45, scaled by sqrt2
l = mvb((1, (4, 3)))
Q = add(l, anti)
lam = R4.agp(Q, R4.antirev(Q))
print("Q =", show(Q, 4), " Q % antirev(Q) =", show(lam, 4), " parity", R4.parity(Q))
check("rga3 Q = e43 + 1bar: transform((1,0,0,1)) = 2 (0,1,0,1)",
      R4.transform_full(P(1, 0, 0), Q) == scale(P(0, 1, 0), 2))
check("rga3 Q = e43 + 1bar: transform((0,0,1,1)) = 2 (0,0,1,1)",
      R4.transform_full(P(0, 0, 1), Q) == scale(P(0, 0, 1), 2))

# rotation about a line off the origin: through (1,2,0) along z, sin phi=3/5, cos phi=4/5
p0, q0 = P(1, 2, 0), P(1, 2, 1)
L = rga3_line(p0, q0)
print("line through (1,2,0) along z:", show(L, 4))
Q2 = add(scale(L, F(3, 5)), scale(anti, F(4, 5)))
check("Q2 unitized: Q2 % antirev(Q2) = 1bar", R4.agp(Q2, R4.antirev(Q2)) == anti)
img = R4.transform_full(P(2, 2, 5), Q2)
c2, s2 = F(7, 25), F(24, 25)
check("rga3 turn by 2 phi (cos 7/25, sin 24/25) about the off-origin line, counterclockwise",
      img == P(1 + c2, 2 + s2, 5))
print("   image of (2,2,5,1):", show(img, 4))

# translation: translation(dx,dy,dz) = d/2 on e23,e31,e12 plus 1bar
T = add(mvb((F(1, 2), (2, 3)), (F(3, 2), (3, 1)), (F(-1), (1, 2))), anti)
check("rga3 translation by (1,3,-2) moves (0,0,0,1) to (1,3,-2,1)",
      R4.transform_full(P(0, 0, 0), T) == P(1, 3, -2))

# compose order and line transform as outermorphism
QT = R4.agp(T, Q2)
check("transform(x, T % Q2) = transform(transform(x, Q2), T)",
      R4.transform_full(P(2, 2, 5), QT) == R4.transform_full(R4.transform_full(P(2, 2, 5), Q2), T))
pa, pb = P(1, 0, 0), P(0, 1, 2)
check("lines move as the points that span them (unitized motor)",
      R4.transform_full(R4.wedge(pa, pb), QT)
      == R4.wedge(R4.transform_full(pa, QT), R4.transform_full(pb, QT)))

# geometric-product (classical) rotor used naively in rga3: 1 + e12 has antigrades 4 and 2 (even)
cr = mvb((1, ()), (1, (1, 2)))
print("rga3 transform(e1+e4, 1+e12) =", show(R4.transform_full(P(1, 0, 0), cr), 4),
      " parity", R4.parity(cr))

print("ALL PASS" if ok else "SOME FAILED")
