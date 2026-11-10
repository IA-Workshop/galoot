"""Forms (A), (B), (C) of the transforms of rigid(3) as shared-node programs.

All three build on `dag.Dag`. Variable names are those of the layouts in
rigid3.py: the motor Qvx Qvy Qvz Qvw Qmx Qmy Qmz Qmw, a point x y z w, a line
vx vy vz mx my mz suffixed `l`, a plane gx gy gz gw suffixed `g`.
"""

import sympy as sp

import dag as D
import plans as P
import rigid3 as R

MOTOR_NAMES = [n for n, _, _ in R.MOTOR]


def motor_vars(g):
    q = {n: g.var(n) for n in MOTOR_NAMES}
    v = [q["Qvx"], q["Qvy"], q["Qvz"]]
    s = q["Qvw"]
    m = [q["Qmx"], q["Qmy"], q["Qmz"]]
    t = q["Qmw"]
    return q, v, s, m, t


def layout_vars(g, layout, suffix):
    return [g.var(n + suffix) for n, _, _ in layout]


# --- Form A under rule S: the restricted terms, signs carried ----------------

def form_A(layout, suffix):
    """The kernels' terms of the transform by a motor, evaluated as sums with
    carried signs (rule S). Returns (dag, outputs, input names)."""
    g = D.Dag()
    q, v, s, m, t = motor_vars(g)
    qv = [q[n] for n in MOTOR_NAMES]
    xv = layout_vars(g, layout, suffix)
    first, second, mid, dropped = P.sandwich_terms(R.MOTOR, layout)
    w = {}
    by_out = {}
    for a, b, o, c in first:
        by_out.setdefault(o, []).append((qv[a] * xv[b]) * c)
    for o, ts in by_out.items():
        w[o] = D.lin(ts)
    by_out = {}
    for a, b, o, c in second:
        by_out.setdefault(o, []).append((w[a] * qv[b]) * c)
    outs = [D.lin(by_out[o]) for o in range(len(layout))]
    return g, outs


# --- Form B: collected, motor quadratics shared ------------------------------

def collected_matrix(layout, suffix):
    """M[i][j]: the coefficient polynomial of input j in output i, from the
    sandwich polynomial itself."""
    xs = R.symbols(layout, suffix)
    q = R.symbols(R.MOTOR)
    X = R.build(layout, xs)
    V = R.build(R.MOTOR, q)
    Y, extra = R.read(layout, R.transform(X, V))
    assert not extra
    M = [[sp.expand(sp.diff(y, x)) for x in xs] for y in Y]
    for i, y in enumerate(Y):
        assert sp.expand(y - sum(M[i][j] * xs[j] for j in range(len(xs)))) == 0
    return M


def form_B(layout, suffix, grouped=False):
    """grouped: an entry whose atoms all carry the factor 2 is summed and
    doubled once; otherwise each doubled atom is doubled where it is made
    and shared."""
    g = D.Dag()
    q, v, s, m, t = motor_vars(g)
    xv = layout_vars(g, layout, suffix)
    M = collected_matrix(layout, suffix)
    syms = {sp.Symbol(n): q[n] for n in MOTOR_NAMES}
    entries = []
    for i, row in enumerate(M):
        er = []
        for j, poly in enumerate(row):
            if poly == 0:
                er.append(None)
                continue
            atoms = []
            for mon, c in sp.Poly(poly, *syms.keys()).terms():
                names = []
                for sym, e in zip(syms.keys(), mon):
                    names += [syms[sym]] * e
                assert len(names) == 2
                c = int(c)
                assert abs(c) in (1, 2), c
                atoms.append((names[0] * names[1], c))
            if grouped and all(abs(c) == 2 for _, c in atoms):
                inner = D.lin([p * (1 if c > 0 else -1) for p, c in atoms])
                er.append(inner.dbl())
            else:
                er.append(D.lin([(p.dbl() if abs(c) == 2 else p)
                                 * (1 if c > 0 else -1) for p, c in atoms]))
        entries.append(er)
    outs = []
    for i, er in enumerate(entries):
        terms = [er[j] * xv[j] for j in range(len(xv)) if er[j] is not None]
        outs.append(D.lin(terms))
    return g, outs, entries


# --- Form C: the unit and geometric constraints ------------------------------

def double(g, x):
    return x.dbl()


def form_C_point(pre):
    """p' = p + 2(s c + v x c - w t v), c = v x p + w m, w' = w.
    pre: the motor's doubles are made first (a motor-only step)."""
    g = D.Dag()
    q, v, s, m, t = motor_vars(g)
    p = layout_vars(g, R.POINT, "p")
    pxyz, w = p[:3], p[3]
    a = D.cross(v, pxyz)
    c = [a[i] + w * m[i] for i in range(3)]
    vc = D.cross(v, c)
    wt = w * t
    if pre:
        v2 = [x.dbl() for x in v]
        s2 = s.dbl()
        # v2 x c and wt v2 stand for 2 (v x c) and 2 wt v
        vc2 = D.cross(v2, c)
        outs = [pxyz[i] + D.lin([s2 * c[i], vc2[i], -(wt * v2[i])])
                for i in range(3)]
    else:
        outs = [pxyz[i] + D.lin([s * c[i], vc[i], -(wt * v[i])]).dbl()
                for i in range(3)]
    outs.append(w)
    return g, outs


def translation(g, v, s, m, t, pre):
    """T = 2(s m - t v + v x m)."""
    vm = D.cross(v, m)
    parts = [D.lin([s * m[i], -(t * v[i]), vm[i]]) for i in range(3)]
    return [x.dbl() for x in parts]


def rotate(g, v, s, u, pre):
    """u + 2(s a + v x a), a = v x u; with pre, the doubled v and s."""
    a = D.cross(v, u)
    if pre:
        v2 = [x.dbl() for x in v]
        s2 = s.dbl()
        va = D.cross(v2, a)
        return [u[i] + D.lin([s2 * a[i], va[i]]) for i in range(3)]
    va = D.cross(v, a)
    return [u[i] + D.lin([s * a[i], va[i]]).dbl() for i in range(3)]


def form_C_line(pre):
    """d' = R d; k' = R k + T x d'."""
    g = D.Dag()
    q, v, s, m, t = motor_vars(g)
    l = layout_vars(g, R.LINE, "l")
    d, k = l[:3], l[3:]
    d2 = rotate(g, v, s, d, pre)
    k2 = rotate(g, v, s, k, pre)
    T = translation(g, v, s, m, t, pre)
    Td = D.cross(T, d2)
    outs = d2 + [k2[i] + Td[i] for i in range(3)]
    return g, outs


def form_C_plane(pre):
    """n' = R n; gw' = gw - n' . T."""
    g = D.Dag()
    q, v, s, m, t = motor_vars(g)
    p = layout_vars(g, R.PLANE, "g")
    n, gw = p[:3], p[3]
    n2 = rotate(g, v, s, n, pre)
    T = translation(g, v, s, m, t, pre)
    outs = n2 + [gw - D.dot3(n2, T)]
    return g, outs
