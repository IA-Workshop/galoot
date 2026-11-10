"""My own form C (unit and geometric constraints used) for point, line, plane, with op counts.

Op rule: every * and + / - of two non-constant values is counted; a multiply by
the constant 2 is counted separately (it can be an addition x + x). MA counts a
multiplication whose only use is one addition or subtraction, fused with it, as one.
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import sympy as sp
from ga import *
from formb import results, Q, objs

vx, vy, vz, vw, mx, my, mz, mw = Q
lam = vx**2 + vy**2 + vz**2 + vw**2
h = vw * mw + vx * mx + vy * my + vz * mz


class Ctr:
    def __init__(self):
        self.mul = self.add = self.two = self.ma = 0


def cross(c, a, b):
    # each component a_i b_j - a_j b_i: 2 mul 1 sub; MA: 1 mul + 1 fused = 2
    c.mul += 6; c.add += 3; c.ma += 6
    return [a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]]


def scale_add(c, s, a, b, sign=1):
    # b + sign * s a, componentwise: 3 mul 3 add, 3 MA
    c.mul += 3; c.add += 3; c.ma += 3
    return [bi + sign * s * ai for ai, bi in zip(a, b)]


def add3(c, a, b, sign=1):
    c.add += 3; c.ma += 3
    return [ai + sign * bi for ai, bi in zip(a, b)]


def twice_add(c, a, b):
    # b + 2 a: 3 "two" mul and 3 add, fused: 3 MA
    c.two += 3; c.add += 3; c.ma += 3
    return [bi + 2 * ai for ai, bi in zip(a, b)]


V = [vx, vy, vz]
Mv = [mx, my, mz]


def rotate(c, p):
    a = cross(c, V, p)              # v x p
    b = cross(c, V, a)              # v x (v x p)
    s = scale_add(c, vw, a, b)      # vw a + b
    return twice_add(c, s, p)       # p + 2 s


def translation(c):
    # t = 2 (vw m - mw v + v x m)
    t = cross(c, V, Mv)
    t = scale_add(c, vw, Mv, t)
    t = scale_add(c, mw, V, t, -1)
    c.two += 3; c.ma += 3
    return [2 * e for e in t]


def check(nm, got, sandwich):
    gens = list(Q) + sorted({s for e in list(got) + list(sandwich) for s in sp.sympify(e).free_symbols} - set(Q), key=str)
    G = sp.groebner([lam - 1, h], *gens, order="grevlex")
    diffs = [sp.expand(g - s) for g, s in zip(got, sandwich)]
    on = all(G.reduce(d)[1] == 0 for d in diffs)
    G1 = sp.groebner([lam - 1], *gens, order="grevlex")
    on_lam_only = all(G1.reduce(d)[1] == 0 for d in diffs)
    off = any(d != 0 for d in diffs)
    print(f"{nm}: equal mod (lam-1, h): {on}; mod (lam-1) only: {on_lam_only}; differs off constraints: {off}")


# point
px, py, pz, pw = objs["point"][1]
c = Ctr()
r = rotate(c, [px, py, pz])
t = translation(c)
out = scale_add(c, pw, t, r) + [pw]
check("point C", out, results["point"][0])
print("   counts", vars(c))

# plane: n' = R n, gw' = gw + u . n with u = 2 (mw v - vw m + v x m)
gx, gy, gz, gw = objs["plane"][1]
c = Ctr()
n = rotate(c, [gx, gy, gz])
u = cross(c, V, Mv)
u = scale_add(c, mw, V, u)
u = scale_add(c, vw, Mv, u, -1)
c.two += 3; c.ma += 3
u = [2 * e for e in u]
c.mul += 3; c.add += 3; c.ma += 3
gw2 = gw + u[0] * gx + u[1] * gy + u[2] * gz
check("plane C (u . g, no t)", n + [gw2], results["plane"][0])
print("   counts", vars(c))
# plane alternative: gw' = gw - t . n'
c = Ctr()
n = rotate(c, [gx, gy, gz])
t = translation(c)
c.mul += 3; c.add += 3; c.ma += 3
check("plane C (gw - t . R n)", n + [gw - (t[0] * n[0] + t[1] * n[1] + t[2] * n[2])], results["plane"][0])
print("   counts", vars(c))

# line: d' = R d, m' = R m + t x d'
lvx, lvy, lvz, lmx, lmy, lmz = objs["line"][1]
c = Ctr()
d = rotate(c, [lvx, lvy, lvz])
mm = rotate(c, [lmx, lmy, lmz])
t = translation(c)
tx = cross(c, t, d)
mm2 = add3(c, mm, tx)
c.ma -= 3  # the additions fuse into the cross product's second multiply
check("line C (R m + t x R d)", d + mm2, results["line"][0])
print("   counts", vars(c))
