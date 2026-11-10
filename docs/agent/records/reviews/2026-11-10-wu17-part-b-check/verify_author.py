"""Check the author's stated closed forms and declared-layout product counts with my algebra."""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import sympy as sp
from ga import *
from formb import results, Q, objs

vx, vy, vz, vw, mx, my, mz, mw = Q
s, t = vw, mw
v = sp.Matrix([vx, vy, vz]); m = sp.Matrix([mx, my, mz])
lam = vx**2 + vy**2 + vz**2 + vw**2
h = vw * mw + vx * mx + vy * my + vz * mz
T = 2 * (s * m - t * v + v.cross(m))


def red(diffs, gens_extra, ideal):
    gens = list(Q) + gens_extra
    if not ideal:
        return [sp.expand(d) for d in diffs]
    G = sp.groebner(ideal, *gens, order="grevlex")
    return [G.reduce(sp.expand(d))[1] for d in diffs]


def report(nm, form, sandwich, extra):
    diffs = [sp.expand(a - b) for a, b in zip(form, sandwich)]
    both = red(diffs, extra, [lam - 1, h])
    lo = red(diffs, extra, [lam - 1])
    ho = red(diffs, extra, [h])
    print(f"{nm}: mod(lam-1,h) zero={all(d == 0 for d in both)}; "
          f"mod(lam-1) zero per output={[d == 0 for d in lo]}; mod(h) zero per output={[d == 0 for d in ho]}; "
          f"raw zero per output={[d == 0 for d in diffs]}")


# point: p' = p + 2 (s c + v x c - w t v), c = v x p + w m, w' = w
px, py, pz, pw = objs["point"][1]
P = sp.Matrix([px, py, pz])
c = v.cross(P) + pw * m
pp = P + 2 * (s * c + v.cross(c) - pw * t * v)
report("author point C", list(pp) + [pw], results["point"][0], [px, py, pz, pw])
# also the unconstrained hand form
hand = (s**2 - v.dot(v)) * P + 2 * v.dot(P) * v + 2 * s * v.cross(P) + pw * T
assert all(sp.expand(a - b) == 0 for a, b in zip(list(hand) + [lam * pw], results["point"][0]))
print("author's unconstrained point form equals the sandwich: True")

# plane: n' = n + 2 (s a + v x a), a = v x n; g_w' = g_w - n' . T
gx, gy, gz, gw = objs["plane"][1]
n = sp.Matrix([gx, gy, gz]); a = v.cross(n)
n2 = n + 2 * (s * a + v.cross(a))
report("author plane C", list(n2) + [gw - n2.dot(T)], results["plane"][0], [gx, gy, gz, gw])

# line: d' = d + 2 (s a + v x a), k' = R k + T x d'
lvx, lvy, lvz, lmx, lmy, lmz = objs["line"][1]
d = sp.Matrix([lvx, lvy, lvz]); k = sp.Matrix([lmx, lmy, lmz])
R = lambda u: u + 2 * (s * v.cross(u) + v.cross(v.cross(u)))
d2 = R(d)
k2 = R(k) + T.cross(d2)
report("author line C", list(d2) + list(k2), results["line"][0], [lvx, lvy, lvz, lmx, lmy, lmz])

# line v plane closed form: (g_w d - n x k, -n . d)
L = [lvx, lvy, lvz, lmx, lmy, lmz]; G = [gx, gy, gz, gw]
lp_k, lp_res = product_kernel(LINE, PLANE, antiwedge_blade)
lp = [sp.expand(e) for e in run(lp_k, L, G, 4)]
cf = list(gw * d - n.cross(k)) + [-n.dot(d)]
print("line v plane closed form equal:", all(sp.expand(a - b) == 0 for a, b in zip(lp, cf)))

# declared-layout product counts
print("p^p onto Line   ", kernel_counts(product_kernel(POINT, POINT, wedge_blade, LINE)[0]))
pp_k, pp_res = product_kernel(POINT, POINT, wedge_blade)
print("(p^p canonical)^p onto Plane", kernel_counts(product_kernel(pp_res, POINT, wedge_blade, PLANE)[0]))
print("(p^p onto Line)^p onto Plane", kernel_counts(product_kernel(LINE, POINT, wedge_blade, PLANE)[0]))
print("(p^p onto Line)^p canonical ", kernel_counts(product_kernel(LINE, POINT, wedge_blade)[0]))
print("compose onto Motor", kernel_counts(product_kernel(MOTOR, MOTOR, gap_blade, MOTOR)[0]))
cm_k, cm_res = product_kernel(MOTOR, MOTOR, gap_blade)
per = [sum(1 for tt in cm_k if tt[2] == i) for i in range(8)]
print("compose canonical terms per output", [name(mm) for mm, _ in cm_res], per)

# off-constraint values: lambda = 4 versor (2 x a unit rotor), lambda = 1 with h != 0
num = {vx: 0, vy: 0, vz: sp.Rational(6, 5), vw: sp.Rational(8, 5), mx: 1, my: 2, mz: 0, mw: 0}
print("lambda, h at the scaled rotor:", lam.subs(num), h.subs(num))
num2 = {vx: 0, vy: 0, vz: sp.Rational(3, 5), vw: sp.Rational(4, 5), mx: 0, my: 0, mz: 1, mw: 0}
print("lambda, h at the h != 0 motor:", lam.subs(num2), h.subs(num2))
pt = {px: 1, py: 2, pz: 3, pw: 1}
print("point C - sandwich at lambda=4:", [sp.expand(a - b).subs(num).subs(pt) for a, b in zip(list(pp) + [pw], results["point"][0])])
print("point C - sandwich at h!=0:", [sp.expand(a - b).subs(num2).subs(pt) for a, b in zip(list(pp) + [pw], results["point"][0])])
ln = {lvx: 1, lvy: 0, lvz: 0, lmx: 0, lmy: 0, lmz: 1}
print("line C - sandwich at h!=0:", [sp.expand(a - b).subs(num2).subs(ln) for a, b in zip(list(d2) + list(k2), results["line"][0])])
