import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import sympy as sp
from ga import *

Q = sp.symbols("vx vy vz vw mx my mz mw")
vx, vy, vz, vw, mx, my, mz, mw = Q


def sandwich(Xl, xs, V=MOTOR, Vs=Q):
    K = transform_kernels(V, Xl)
    mid = run(K["first_r"], Vs, xs, len(K["middle"]))
    out = run(K["kept"], mid, Vs, len(Xl))
    # independent full sandwich (all terms) for comparison, generic multivectors
    midall = run(K["first_all"], Vs, xs, len(K["middle"]))
    full = run(K["kept"], midall, Vs, len(Xl))
    drop = run(K["dropped"], midall, Vs, len(K["dropped_masks"]))
    assert all(sp.expand(a - b) == 0 for a, b in zip(out, full))
    return [sp.expand(o) for o in out], [sp.expand(d) for d in drop]


# lambda and the second (geometric) constraint: Q antiproduct antireverse(Q)
Qrev = [antireverse_sign(m) * c for (m, s), c in zip(MOTOR, Q)]
qq_k, qq_res = product_kernel(MOTOR, MOTOR, gap_blade)
QQ = run(qq_k, list(Q), Qrev, len(qq_res))
print("Q antiprod antirev(Q):", {name(m): sp.factor(c) for (m, _), c in zip(qq_res, QQ) if sp.expand(c) != 0})

objs = {
    "point": (POINT, list(sp.symbols("px py pz pw"))),
    "line": (LINE, list(sp.symbols("lvx lvy lvz lmx lmy lmz"))),
    "plane": (PLANE, list(sp.symbols("gx gy gz gw"))),
}


def form_b(outs, xs):
    """Counts of the collected form B: products Q_j Q_k shared, matrix entries, matrix-vector."""
    monos = set()
    M = {}
    for i, o in enumerate(outs):
        poly = sp.Poly(o, *xs)
        for l, xl in enumerate(xs):
            e = sp.expand(poly.coeff_monomial(xl))
            if e != 0:
                M[i, l] = e
                for term in sp.Poly(e, *Q).terms():
                    monos.add(term[0])
    entry_mul = entry_cmul = entry_add = entry_neg = 0
    for e in M.values():
        terms = sp.Poly(e, *Q).terms()
        entry_add += len(terms) - 1
        entry_cmul += sum(1 for _, c in terms if abs(c) != 1)
    apply_mul = len(M)
    apply_add = sum(1 for i in range(len(outs)) for _ in range(sum(1 for l in range(len(xs)) if (i, l) in M) - 1))
    return dict(products=len(monos), entry_cmul=entry_cmul, entry_add=entry_add,
                apply_mul=apply_mul, apply_add=apply_add,
                per_motor_mul=len(monos) + entry_cmul, per_motor_add=entry_add), M


results = {}
for nm, (Xl, xs) in objs.items():
    outs, drop = sandwich(Xl, xs)
    assert all(d == 0 for d in drop), (nm, drop)
    c, M = form_b(outs, xs)
    results[nm] = (outs, M)
    print(f"\n{nm}: B counts {c}")
    print("   per call: mul", c["per_motor_mul"] + c["apply_mul"], "add", c["per_motor_add"] + c["apply_add"])
    for (i, l), e in sorted(M.items()):
        print(f"   M[{i},{l}] = {e}")
    for i, o in enumerate(outs):
        print(f"   out[{i}] = {o}")
