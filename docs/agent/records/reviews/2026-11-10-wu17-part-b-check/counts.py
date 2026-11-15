import sys
import os; sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import sympy as sp
from ga import *

# ---------- sanity checks against CONVENTIONS ----------
x, y, z, w, X2, Y2, Z2, W2, X3, Y3, Z3, W3 = sp.symbols("x y z w x2 y2 z2 w2 x3 y3 z3 w3")
p = [x, y, z, w]; q = [X2, Y2, Z2, W2]; r = [X3, Y3, Z3, W3]

pp_k, pp_res = product_kernel(POINT, POINT, wedge_blade)
pq = run(pp_k, p, q, len(pp_res))
# as Line coefficients: convert canonical (+1) to LINE orientation
def convert(vals, src, dst):
    out = []
    for m, s in dst:
        i = [k for k, (mm, _) in enumerate(src) if mm == m][0]
        out.append(sp.expand(vals[i] * src[i][1] * s))
    return out
L = convert(pq, pp_res, LINE)
print("p^q as Line:", L)
# 5.2: direction q - p for unit points, moment p x q
Lu = [sp.expand(e.subs({w: 1, W2: 1})) for e in L]
assert Lu[:3] == [sp.expand(X2 - x), sp.expand(Y2 - y), sp.expand(Z2 - z)]
assert L[3:] == [sp.expand(y * Z2 - z * Y2), sp.expand(z * X2 - x * Z2), sp.expand(x * Y2 - y * X2)]

ppp_k, ppp_res = product_kernel(pp_res, POINT, wedge_blade)
pqr = run(ppp_k, pq, r, len(ppp_res))
G = convert(pqr, ppp_res, PLANE)
# 5.3: points (0,0,0),(1,0,0),(0,1,0) ccw seen from +z -> normal +z
sub = {x: 0, y: 0, z: 0, w: 1, X2: 1, Y2: 0, Z2: 0, W2: 1, X3: 0, Y3: 1, Z3: 0, W3: 1}
print("plane through e1-e2 triangle:", [g.subs(sub) for g in G])
# point on plane: p v g = gx x + gy y + gz z + gw w
gs = sp.symbols("gx gy gz gw")
pg_k, pg_res = product_kernel(POINT, PLANE, antiwedge_blade)
print("p v g result blades:", [name(m) for m, _ in pg_res])
pv = run(pg_k, p, list(gs), len(pg_res))
print("p v g =", pv)

# translator moves by 2t
tx, ty, tz = sp.symbols("tx ty tz")
T = [0, 0, 0, 1, tx, ty, tz, 0]
K = transform_kernels(MOTOR, POINT)
mid = run(K["first_r"], T, p, len(K["middle"]))
out = run(K["kept"], mid, T, len(POINT))
print("translator on point:", [sp.expand(o) for o in out])

# ---------- form A counts ----------
print()
print("FORM A (checking off)")
print("point^point      ", kernel_counts(pp_k))
print("point^point^point", "first", kernel_counts(pp_k), "second", kernel_counts(ppp_k))
lp_k, lp_res = product_kernel(LINE, PLANE, antiwedge_blade)
print("line v plane     ", kernel_counts(lp_k), [name(m) for m, _ in lp_res])
for nm, Xl in (("point", POINT), ("line", LINE), ("plane", PLANE), ("motor", MOTOR)):
    K = transform_kernels(MOTOR, Xl)
    a = kernel_counts(K["first_r"]); b = kernel_counts(K["kept"])
    tot = {k: a[k] + b[k] for k in ("mul", "cmul", "add", "sub", "neg")}
    print(f"transform({nm},motor) first {a}\n   second {b}\n   total {tot}  middle size {len(K['middle'])}"
          f" middle read {a['outs']} dropped blades {[name(m) for m in K['dropped_masks']]}")
    ca = kernel_counts(K["first_all"]); cd = kernel_counts(K["dropped"])
    ctot = {k: ca[k] + b[k] + cd[k] for k in ("mul", "cmul", "add", "sub", "neg")}
    print(f"   checking mode: first_all {ca['mul']} kept {b['mul']} dropped {cd['mul']} total {ctot}")
cm_k, cm_res = product_kernel(MOTOR, MOTOR, gap_blade)
print("compose(motor,motor)", kernel_counts(cm_k), [name(m) for m, _ in cm_res])
