"""Spike E, part B: exact derivations, forms (A), (B), (C), checks, counts.

Run from a checkout root with the math-tools environment's interpreter:

  <env>/bin/python -B docs/agent/records/spikes/spike-e/derive.py

It writes derivations.txt, counts.json and counts.md next to itself, and exits
1 if any check fails. Every number in those files comes from this run.
"""

import importlib.util
import itertools
import json
import os
import random
import sys
from fractions import Fraction

import sympy as sp

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

import dag as D          # noqa: E402
import forms as F        # noqa: E402
import plans as P        # noqa: E402
import rigid3 as R       # noqa: E402

ROOT = os.path.abspath(os.path.join(HERE, *[".."] * 5))
SEED = 20261110
FAILS = []
OUT = []


def say(*a):
    OUT.append(" ".join(str(x) for x in a))


def check(name, ok):
    say(("PASS " if ok else "FAIL ") + name)
    if not ok:
        FAILS.append(name)


Q = R.symbols(R.MOTOR)
vx, vy, vz, s_, mx, my, mz, t_ = Q
LAM = vx**2 + vy**2 + vz**2 + s_**2
HGEO = vx * mx + vy * my + vz * mz + s_ * t_


def sandwich(layout, suf):
    X = R.build(layout, R.symbols(layout, suf))
    Y, extra = R.read(layout, R.transform(X, R.build(R.MOTOR, Q)))
    assert not extra
    return Y


def zero(e):
    return sp.expand(e) == 0


# --- 0. The rigid(3) tables against the reference ---------------------------

def check_tables():
    path = os.path.join(ROOT, "test", "reference", "clifford_reference.py")
    spec = importlib.util.spec_from_file_location("cref", path)
    cref = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(cref)
    alg = cref.Algebra(cref.rigid(3))
    ok = True
    for a in range(16):
        for b in range(16):
            mine = R.gp({a: 1}, {b: 1})
            ref = alg.mul({a: Fraction(1)}, {b: Fraction(1)})
            ok &= {k: Fraction(int(v)) for k, v in mine.items()} == \
                {k: Fraction(v) for k, v in ref.items()}
    check("geometric product table of rigid3.py equals clifford_reference.py"
          " on all 256 blade pairs", ok)


# --- 1. The products ---------------------------------------------------------

def cross_(a, b):
    return [a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0]]


def dot_(a, b):
    return sum(x * y for x, y in zip(a, b))


def products():
    p, q, r = (R.symbols(R.POINT, c) for c in "pqr")
    P_, Q_, R_ = (R.build(R.POINT, c) for c in (p, q, r))
    line, e1 = R.read(R.LINE, R.wedge(P_, Q_))
    plane, e2 = R.read(R.PLANE, R.wedge(R.wedge(P_, Q_), R_))
    l = R.symbols(R.LINE, "l")
    g = R.symbols(R.PLANE, "g")
    pt, e3 = R.read(R.POINT, R.antiwedge(R.build(R.LINE, l),
                                         R.build(R.PLANE, g)))
    check("the products land on the layouts' blades only", not (e1 or e2 or e3))
    say("\n== point ^ point (line: vx vy vz mx my mz) ==")
    for n, e in zip([x[0] for x in R.LINE], line):
        say(f"  {n} = {e}")
    say("\n== point ^ point ^ point (plane: gx gy gz gw) ==")
    for n, e in zip([x[0] for x in R.PLANE], plane):
        say(f"  {n} = {e}")
    say("\n== line v plane (point: x y z w) ==")
    for n, e in zip([x[0] for x in R.POINT], pt):
        say(f"  {n} = {e}")

    # independent closed forms, plain linear algebra
    pxyz, qxyz, rxyz = p[:3], q[:3], r[:3]
    d = [p[3] * qxyz[i] - q[3] * pxyz[i] for i in range(3)]
    k = cross_(pxyz, qxyz)
    check("point^point: direction w_p q - w_q p, moment p x q",
          all(zero(a - b) for a, b in zip(line, d + k)))
    M = sp.Matrix([p, q, r])
    cof = []
    for j in range(4):
        # coefficient of the plane on e_j-dual: det[e_j; p; q; r]
        e = sp.Matrix([[1 if i == j else 0 for i in range(4)]])
        cof.append(sp.Matrix.vstack(e, M).det())
    want = cof
    got = plane
    # fix the unknown global sign of the cofactor convention by one entry
    sign = 1 if zero(got[0] - want[0]) else -1
    check("point^point^point: coefficient j is the cofactor det[e_j; p; q; r]"
          f" (the plane's value at x is det[x; p; q; r]), one overall sign"
          f" {sign:+d} fixed by gx and the same for gy, gz, gw",
          all(zero(a - sign * b) for a, b in zip(got, want)))
    # line v plane for a line through two points: (g.p) q - (g.q) p
    gv = R.symbols(R.PLANE, "g")
    lp = line
    Ln = R.build(R.LINE, lp)
    pt2, _ = R.read(R.POINT, R.antiwedge(Ln, R.build(R.PLANE, gv)))
    gp_ = sum(a * b for a, b in zip(gv[:3], p[:3])) + gv[3] * p[3]
    gq_ = sum(a * b for a, b in zip(gv[:3], q[:3])) + gv[3] * q[3]
    check("(p^q) v g = (g.p) q - (g.q) p  (the knowledge base's form)",
          all(zero(pt2[i] - (gp_ * q[i] - gq_ * p[i])) for i in range(4)))
    check("line v plane: point = (gw d - n x k, -n.d) on a general line",
          all(zero(pt[i] - ((g[3] * l[i] - cross_(g[:3], l[3:])[i]) if i < 3
                            else -dot_(g[:3], l[:3]))) for i in range(4)))
    return line, plane, pt


# --- 2. The transforms: polynomials ------------------------------------------

def transform_polys():
    res = {}
    for nm, layout, suf in (("point", R.POINT, "p"), ("line", R.LINE, "l"),
                            ("plane", R.PLANE, "g")):
        Y = sandwich(layout, suf)
        res[nm] = Y
        names = [n + suf for n, _, _ in layout]
        say(f"\n== transform({nm}, motor): output i = sum_j M_ij x_j ==")
        for i, y in enumerate(Y):
            say(f"  {layout[i][0]}' =")
            for j, n in enumerate(names):
                c = sp.expand(sp.diff(y, sp.Symbol(n)))
                if c != 0:
                    say(f"      {n:4s} * ( {c} )")
    # the point's closed form, by hand (quaternion and dual-part algebra)
    p = R.symbols(R.POINT, "p")
    v = [vx, vy, vz]
    m = [mx, my, mz]
    u = p[:3]
    w = p[3]
    Rex = [(s_**2 - sum(x * x for x in v)) * u[i] + 2 * dot_(v, u) * v[i]
           + 2 * s_ * cross_(v, u)[i] for i in range(3)]
    T = [2 * (s_ * m[i] - t_ * v[i] + cross_(v, m)[i]) for i in range(3)]
    byhand = [Rex[i] + w * T[i] for i in range(3)] + [LAM * w]
    check("point: sandwich equals (s^2-|v|^2)u + 2(v.u)v + 2s v x u + wT,"
          " w' = lambda w, T = 2(s m - t v + v x m)  (no constraint used)",
          all(zero(a - b) for a, b in zip(res["point"], byhand)))
    Q_ = R.build(R.MOTOR, Q)
    check("the dropped blades of every transform are zero as polynomials"
          " (motor layout, any coefficients)", True)  # asserted in sandwich()
    return res


# --- 3. compose -------------------------------------------------------------

def compose_poly():
    a = R.symbols(R.MOTOR, "a")
    b = R.symbols(R.MOTOR, "b")
    A = R.build(R.MOTOR, a)
    B = R.build(R.MOTOR, b)
    C, extra = R.read(R.MOTOR, R.antiproduct(A, B))
    check("compose(Qa, Qb) lands on the motor's eight blades", not extra)
    say("\n== compose(Qa, Qb) = Qa antiproduct Qb (Qb applies first) ==")
    for (n, _, _), e in zip(R.MOTOR, C):
        say(f"  {n} = {e}")
    # composition law on a point, exact: degree 4 in the motors
    p = R.build(R.POINT, R.symbols(R.POINT, "p"))
    lhs = R.transform(p, R.antiproduct(A, B))
    rhs = R.transform(R.transform(p, B), A)
    check("transform(x, compose(Qa, Qb)) = transform(transform(x, Qb), Qa)"
          " as polynomials, for a point", not R.add(lhs, R.scale(-1, rhs)))
    return C, a, b


# --- 4. Forms: equality, constraints, differences ----------------------------

def rnd_rational(rng):
    return Fraction(rng.randint(-9, 9), rng.randint(1, 5))


def eval_at(expr, env):
    return sp.nsimplify(expr.subs(env))


def versor_unit(rng):
    """A rational unit versor: a rotor on a rational point of S^3 composed
    with a rational translator and another rotor; returns its coefficients
    (lambda = 1, h = 0 exactly)."""
    def unit_quat():
        # stereographic image of a rational point of R^3 on S^3
        a, b, c = (sp.Rational(rng.randint(-5, 5), rng.randint(1, 3))
                   for _ in range(3))
        n2 = a * a + b * b + c * c
        return [2 * a / (1 + n2), 2 * b / (1 + n2), 2 * c / (1 + n2),
                (n2 - 1) / (1 + n2)]

    def rotor():
        q = unit_quat()
        return R.build(R.MOTOR, q + [0, 0, 0, 0])

    def translator():
        tx, ty, tz = (sp.Rational(rng.randint(-5, 5), 2) for _ in range(3))
        return R.build(R.MOTOR, [0, 0, 0, 1, tx, ty, tz, 0])

    V = R.antiproduct(R.antiproduct(rotor(), translator()), rotor())
    c, extra = R.read(R.MOTOR, V)
    assert not extra
    return c


def forms_and_counts(polys, rng):
    rows = {}
    ops = (("point", R.POINT, "p", F.form_C_point),
           ("line", R.LINE, "l", F.form_C_line),
           ("plane", R.PLANE, "g", F.form_C_plane))
    G = sp.groebner([LAM - 1, HGEO], *Q, order="lex")
    for nm, layout, suf, Cf in ops:
        Y = polys[nm]
        xs = list(R.symbols(layout, suf))
        gens = list(Q) + xs
        say(f"\n== forms for transform({nm}, motor) ==")
        # A
        g, outs = F.form_A(layout, suf)
        check(f"A({nm}): the restricted terms equal the sandwich exactly",
              all(zero(o.expr - y) for o, y in zip(outs, Y)))
        rows[(nm, "A")] = (g, outs)
        # B
        best = None
        for grouped in (False, True):
            gb, ob, ent = F.form_B(layout, suf, grouped)
            check(f"B({nm}, grouped={grouped}): collected matrix form equals"
                  " the sandwich exactly",
                  all(zero(o.expr - y) for o, y in zip(ob, Y)))
            c = D.count_dag(gb, ob)
            tot = c[0] + c[1] + c[3]
            if best is None or tot < best[0]:
                best = (tot, gb, ob)
        rows[(nm, "B")] = (best[1], best[2])
        # C, two arrangements
        for pre in (False, True):
            gc, oc = Cf(pre)
            diff = [sp.expand(o.expr - y) for o, y in zip(oc, Y)]
            rem = [sp.reduced(d, list(G.exprs), *gens, order="lex")[1]
                   for d in diff]
            only_lam = [sp.reduced(d, [LAM - 1], *gens, order="lex")[1] == 0
                        for d in diff]
            only_h = [sp.reduced(d, [HGEO], *gens, order="lex")[1] == 0
                      for d in diff]
            check(f"C({nm}, {'doubled motor' if pre else 'doubled result'}):"
                  " equals the sandwich modulo (lambda - 1, h): the remainder"
                  " of every output on division by the Groebner basis is 0",
                  all(r == 0 for r in rem))
            say(f"   difference vanishes modulo lambda - 1 alone, per output:"
                f" {only_lam}; modulo h alone: {only_h}")
            rows[(nm, "C-pre" if pre else "C-call")] = (gc, oc)
            rows[(nm, "needs")] = (only_lam, only_h)
        # off the constraints
        sampleX = {x: sp.Rational(rng.randint(-4, 4), rng.randint(1, 3))
                   for x in xs}
        gc, oc = Cf(True)
        # three motors: scaled versor, perturbed unit versor, random
        base = versor_unit(rng)
        scaled = [2 * c for c in base]          # versor, lambda = 4, h = 0
        pert = list(base)
        pert[7] = pert[7] + sp.Rational(1, 7)   # lambda = 1 kept, h != 0
        rand = [sp.Rational(rng.randint(-5, 5), rng.randint(1, 3))
                for _ in range(8)]
        cases = (("a versor with lambda = 4 (h = 0)", scaled),
                 ("lambda = 1 and h != 0", pert),
                 ("a random element of the layout", rand))
        for label, mot in cases:
            env = dict(zip(Q, mot))
            lam = LAM.subs(env)
            hh = HGEO.subs(env)
            dvals = [sp.nsimplify((o.expr - y).subs(env).subs(sampleX))
                     for o, y in zip(oc, Y)]
            say(f"   off the constraints: {label}: lambda={lam}, h={hh},"
                f" C - sandwich = {dvals}")
            rows[(nm, "off", label)] = (lam, hh, dvals)
        base_env = dict(zip(Q, base))
        dvals = [sp.nsimplify((o.expr - y).subs(base_env).subs(sampleX))
                 for o, y in zip(oc, Y)]
        check(f"C({nm}) equals the sandwich at a rational unit versor"
              " (lambda = 1, h = 0)", all(v == 0 for v in dvals))
    return rows


# --- 5. Counts ---------------------------------------------------------------

def split(g, outs):
    dep = D.depends(g, lambda n: not n.startswith("Q"))
    seen = D.reachable(g, outs)
    setup = {i for i in seen if not dep[i]}
    obj = {i for i in seen if dep[i]}
    return (D.count_dag(g, outs), D.count_dag(g, outs, setup),
            D.count_dag(g, outs, obj))


def ma(c):
    """multiply-adds without the doublings: mul + add - fused."""
    return c[0] + c[1] - c[4]


def counter2(g, outs, layout, suf):
    """Run generated text on counting numbers at random rational inputs and
    compare the tally and the values with counter 1."""
    rng = random.Random(SEED)
    env = {}
    for nm in g.vars:
        env[nm] = Fraction(rng.randint(-9, 9), rng.randint(1, 4))
    tally, vals, src = D.count_by_execution(g, outs, env)
    c = D.count_dag(g, outs)
    ok = (tally["mul"], tally["add"], tally["neg"], tally["dbl"]) == c[:4]
    expect = [sp.Rational(o.expr.subs({sp.Symbol(k): sp.Rational(v.numerator,
                                                                 v.denominator)
                                       for k, v in env.items()}))
              for o in outs]
    ok2 = all(Fraction(int(e.p), int(e.q)) == v for e, v in zip(expect, vals))
    return ok, ok2, tally


def fused_by_text(g, outs):
    """A second tally of fused multiply-adds, by node: an addition absorbs
    one multiplication operand that nothing else reads, taken greedily, and
    a tree of additions may be re-associated so that each addition absorbs
    one multiplication leaf (the chain of the rule). Written apart from
    dag.count_dag: it works on the generated statements."""
    lines, res = D.generate_text(g, outs)
    stmts = {}
    for ln in lines:
        if ln.startswith("#"):
            continue
        lhs, rhs = ln.split(" = ")
        stmts[lhs] = rhs
    uses = {k: 0 for k in stmts}
    for rhs in stmts.values():
        for tok in rhs.replace(".dbl()", "").replace("-", " ").replace(
                "+", " ").replace("*", " ").split():
            if tok in uses:
                uses[tok] += 1
    for r in res:
        uses[r.strip("()-")] += 1
    kind = {}
    for k, rhs in stmts.items():
        if " * " in rhs:
            kind[k] = "m"
        elif " + " in rhs or " - " in rhs:
            kind[k] = "a"
        elif rhs.endswith(".dbl()"):
            kind[k] = "d"
        else:
            kind[k] = "v"
    ops = {k: [t for t in rhs.replace(".dbl()", "").replace("-", " ").replace(
        "+", " ").replace("*", " ").split() if t in stmts]
        for k, rhs in stmts.items()}
    total = 0
    done = set()
    for k in stmts:  # roots of sums
        if kind[k] != "a" or k in done:
            continue
        parents = [q for q in stmts if k in ops[q]]
        if len(parents) == 1 and kind[parents[0]] == "a" and uses[k] == 1:
            continue
        stack, n_add, n_mul = [k], 0, 0
        while stack:
            x = stack.pop()
            done.add(x)
            n_add += 1
            for y in ops[x]:
                if kind[y] == "a" and uses[y] == 1:
                    stack.append(y)
                elif kind[y] == "m" and uses[y] == 1:
                    n_mul += 1
        total += min(n_add, n_mul)
    return total


# --- 6. Form A by the kernel's own accumulation ------------------------------

def kernel_form_A(layout, suf, Y):
    first, second, mid, dropped = P.sandwich_terms(R.MOTOR, layout)
    qv = list(Q)
    xs = list(R.symbols(layout, suf))
    mids = P.run_kernel(first, len(mid), qv, xs, lambda a: -a,
                        lambda a, b: a + b, lambda a, b: a - b, sp.Integer(0))
    outs = P.run_kernel(second, len(layout), mids, qv, lambda a: -a,
                        lambda a, b: a + b, lambda a, b: a - b, sp.Integer(0))
    ok = all(zero(o - y) for o, y in zip(outs, Y))
    # drop check: the terms onto the dropped blades sum to zero
    sums = {}
    for a, b, o, c in dropped:
        sums[o] = sums.get(o, 0) + c * mids[a] * qv[b]
    ok_drop = all(zero(v) for v in sums.values())
    c1 = P.kernel_counts(first, len(mid))
    c2 = P.kernel_counts(second, len(layout))
    # the same accumulation on counting numbers
    Cn = D.Cn
    Cn.tally = {"mul": 0, "add": 0, "neg": 0, "dbl": 0}
    mq = [Cn(Fraction(i + 2)) for i in range(8)]
    mx_ = [Cn(Fraction(i + 3)) for i in range(len(layout))]
    m1 = P.run_kernel(first, len(mid), mq, mx_, lambda a: -a,
                      lambda a, b: a + b, lambda a, b: a - b, Cn(0))
    P.run_kernel(second, len(layout), m1, mq, lambda a: -a,
                 lambda a, b: a + b, lambda a, b: a - b, Cn(0))
    t = dict(Cn.tally)
    tot = tuple(a + b for a, b in zip(c1, c2))   # mul, add, sub, neg
    ok_cnt = (t["mul"], t["add"], t["neg"]) == (tot[0], tot[1] + tot[2],
                                                tot[3])
    return ok, ok_drop, tot, ok_cnt, (len(first), len(second), len(mid),
                                      len(dropped))


# --- 7. The other operations -------------------------------------------------

def other_ops():
    out = {}
    # point ^ point -> line
    def terms(la, lb, op, target):
        t = []
        tpos = {m: i for i, (_, m, _) in enumerate(target)} if target else None
        for ai, (_, ma, sa) in enumerate(la):
            for bi, (_, mb, sb) in enumerate(lb):
                if op == "wedge":
                    if ma & mb:
                        continue
                    sg, m = R.reorder_sign(ma, mb), ma | mb
                else:
                    fn = R.antiwedge if op == "antiwedge" else R.antiproduct
                    x = fn({ma: 1}, {mb: 1})
                    if not x:
                        continue
                    (m, sg), = x.items()
                    sg = int(sg)
                c = sg * sa * sb
                if target:
                    c *= target[tpos[m]][2]
                    t.append((ai, bi, tpos[m], c))
                else:
                    t.append((ai, bi, m, c))
        if not target:
            masks = sorted({x[2] for x in t}, key=P.canonical_key)
            pos = {m: i for i, m in enumerate(masks)}
            t = [(a, b, pos[m], c) for a, b, m, c in t]
            return t, [(f"c{m}", m, 1) for m in masks]
        return t, target

    def kc(t):
        return P.kernel_counts(t, 0)

    # A for products into the typed layouts (orientation folded) and canonical
    for nm, la, lb, op, target in (
            ("point^point", R.POINT, R.POINT, "wedge", R.LINE),
            ("line v plane", R.LINE, R.PLANE, "antiwedge", R.POINT)):
        t, _ = terms(la, lb, op, target)
        tc, _ = terms(la, lb, op, None)
        out[nm] = {"typed": kc(t), "canonical": kc(tc)}
    # point ^ point ^ point: two wedges, the middle on the canonical layout
    t1, mid = terms(R.POINT, R.POINT, "wedge", None)
    t2, _ = terms(mid, R.POINT, "wedge", R.PLANE)
    t2c, _ = terms(mid, R.POINT, "wedge", None)
    a, b = kc(t1), kc(t2)
    ac, bc = kc(t1), kc(t2c)
    out["point^point^point"] = {
        "typed": tuple(x + y for x, y in zip(a, b)),
        "canonical": tuple(x + y for x, y in zip(ac, bc)),
        "stages": (len(t1), len(t2))}
    # compose
    tc, _ = terms(R.MOTOR, R.MOTOR, "antiproduct", None)
    tt, _ = terms(R.MOTOR, R.MOTOR, "antiproduct", R.MOTOR)
    out["compose"] = {"typed": kc(tt), "canonical": kc(tc)}
    return out


def expanded_monomials(line, plane, pt, C):
    """Monomial counts of the output polynomials (the collected forms of the
    products), against the term counts of form A."""
    res = {}
    syms = lambda e: sorted(e.free_symbols, key=str)
    for nm, polys in (("point^point", line), ("point^point^point", plane),
                      ("line v plane", pt), ("compose", C)):
        mons = [len(sp.Poly(e, *syms(e)).terms()) if e != 0 else 0
                for e in polys]
        res[nm] = mons
    return res


def write_tables(result):
    lab = {"A": "A (rule S)", "B": "B", "C-call": "C, factor 2 on results",
           "C-pre": "C, motor doubled first"}
    L = []
    L.append("# Counts, generated by derive.py\n")
    L.append("Rule S (COUNTING-RULE.md): mul = products of two non-constant"
             " values; add = additions and subtractions; neg = negations that"
             " cannot be carried as a sign; scl = doublings; MA = mul + add"
             " - fused, with scalings left out, and MA+scl with them added.\n")
    L.append("## Transforms, per call\n")
    L.append("| operation | form | mul | add | neg | scl | MA | MA+scl |")
    L.append("| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |")
    for nm in ("point", "line", "plane"):
        k = result[nm]["A_K"]
        L.append(f"| transform({nm}, motor) | A (rule K, the kernels) |"
                 f" {k['mul']} | {k['add'] + k['sub']} | {k['neg']} | 0 |"
                 f" {k['mul']} | {k['mul']} |")
        for f in ("A", "B", "C-call", "C-pre"):
            r = result[nm][f]
            L.append(f"| transform({nm}, motor) | {lab[f]} | {r['mul']} |"
                     f" {r['add']} | {r['neg']} | {r['scl']} | {r['MA']} |"
                     f" {r['MA'] + r['scl']} |")
    L.append("\n## Checking mode on (exact scalar): the whole first stage,"
             " the kept terms of the second, and the terms onto the dropped"
             " blades (rule K)\n")
    L.append("| operation | mul | add | sub | neg | compare | of which the"
             " dropped-blade terms: mul | add | sub | neg |")
    L.append("| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |"
             " ---: |")
    for nm in ("point", "line", "plane"):
        d = result[nm]["dropped_K"]
        c = result[nm]["checking_K"]
        L.append(f"| transform({nm}, motor) | {c['mul']} | {c['add']} |"
                 f" {c['sub']} | {c['neg']} | {d['compare']} | {d['mul']} |"
                 f" {d['add']} | {d['sub']} | {d['neg']} |")
    L.append("\n## Transforms, split for a batch under one motor\n")
    L.append("| operation | form | motor-only: mul | add | scl | MA+scl |"
             " per object: mul | add | scl | MA+scl |")
    L.append("| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |"
             " ---: |")
    for nm in ("point", "line", "plane"):
        for f in ("B", "C-call", "C-pre"):
            r = result[nm][f]
            su, ob = r["setup"], r["object"]
            L.append(f"| transform({nm}, motor) | {lab[f]} | {su['mul']} |"
                     f" {su['add']} | {su['scl']} | {su['MA'] + su['scl']} |"
                     f" {ob['mul']} | {ob['add']} | {ob['scl']} |"
                     f" {ob['MA'] + ob['scl']} |")
    L.append("\n## Products and compose, form A by the kernels' accumulation"
             " (rule K)\n")
    L.append("| operation | layout of the result | mul | add | sub | neg |"
             " monomials per output |")
    L.append("| --- | --- | ---: | ---: | ---: | ---: | --- |")
    for k in ("point^point", "point^point^point", "line v plane", "compose"):
        v = result[k]
        for lay, key in (("declared type", "A_K_typed"),
                         ("canonical, no type", "A_K_canonical")):
            c = v[key]
            L.append(f"| {k} | {lay} | {c[0]} | {c[1]} | {c[2]} | {c[3]} |"
                     f" {v['monomials']} |")
    with open(os.path.join(HERE, "counts.md"), "w") as f:
        f.write("\n".join(L) + "\n")


# --- main --------------------------------------------------------------------

def main():
    rng = random.Random(SEED)
    say(f"seed {SEED}; python {sys.version.split()[0]}; sympy {sp.__version__}")
    check_tables()
    line, plane, pt = products()
    polys = transform_polys()
    C, a, b = compose_poly()
    rows = forms_and_counts(polys, rng)

    result = {}
    say("\n== counts (mul, add+sub, neg, scl, fused; MA = mul + add - fused) ==")
    for nm, layout, suf in (("point", R.POINT, "p"), ("line", R.LINE, "l"),
                            ("plane", R.PLANE, "g")):
        Y = polys[nm]
        ok, okd, tot, okc, sizes = kernel_form_A(layout, suf, Y)
        check(f"A({nm}) by the kernels' accumulation on sympy: equals the"
              " sandwich; the terms onto dropped blades sum to zero",
              ok and okd)
        check(f"A({nm}) kernel accumulation on counting numbers tallies"
              " as the term lists say", okc)
        say(f"   A({nm}) kernel terms: stage 1 {sizes[0]}, stage 2 {sizes[1]},"
            f" middle blades {sizes[2]}, dropped-blade terms {sizes[3]};"
            f" rule K counts mul/add/sub/neg = {tot}")
        _f, _s, _m, dropped = P.sandwich_terms(R.MOTOR, layout)
        dk = P.kernel_counts(dropped, 0)
        fa = P.kernel_counts(P.sandwich_terms.first_all, 0)
        k2 = P.kernel_counts(_s, 0)
        chk = tuple(a + b + c for a, b, c in zip(fa, k2, dk))
        result[nm] = {"A_K": {"mul": tot[0], "add": tot[1], "sub": tot[2],
                              "neg": tot[3]},
                      "dropped_K": {"mul": dk[0], "add": dk[1], "sub": dk[2],
                                    "neg": dk[3],
                                    "compare": len({t[2] for t in dropped})},
                      "checking_K": {"mul": chk[0], "add": chk[1],
                                     "sub": chk[2], "neg": chk[3]}}
        for form in ("A", "B", "C-call", "C-pre"):
            g, outs = rows[(nm, form)]
            c, setup, obj = split(g, outs)
            ok1, ok2, tally = counter2(g, outs, layout, suf)
            f2 = fused_by_text(g, outs)
            check(f"{form}({nm}): counter 1 equals counter 2 (execution of"
                  " the generated text), and the executed values equal the"
                  " polynomial", ok1 and ok2)
            check(f"{form}({nm}): fused count of counter 1 equals the"
                  " separate tally from the text", f2 == c[4])
            result[nm][form] = {
                "mul": c[0], "add": c[1], "neg": c[2], "scl": c[3],
                "fused": c[4], "MA": ma(c),
                "setup": {"mul": setup[0], "add": setup[1], "neg": setup[2],
                          "scl": setup[3], "MA": ma(setup)},
                "object": {"mul": obj[0], "add": obj[1], "neg": obj[2],
                           "scl": obj[3], "MA": ma(obj)}}
            say(f"   {form}({nm}): total {c[:4]} fused {c[4]} MA {ma(c)};"
                f" motor-only {setup[:4]} MA {ma(setup)};"
                f" per-object {obj[:4]} MA {ma(obj)}")
    # the numerical half's recount of the collected forms from the
    # polynomials themselves, with no reference to the program
    for nm, layout, suf in (("point", R.POINT, "p"), ("line", R.LINE, "l"),
                            ("plane", R.PLANE, "g")):
        M = F.collected_matrix(layout, suf)
        monos = set()
        for row in M:
            for e in row:
                if e != 0:
                    monos |= {m for m, _ in sp.Poly(e, *Q).terms()}
        nnz = sum(1 for row in M for e in row if e != 0)
        rowadds = sum(sum(1 for e in row if e != 0) - 1 for row in M)
        b = result[nm]["B"]
        check(f"B({nm}) recounted from the polynomials: {len(monos)} distinct"
              f" quadratic monomials = setup multiplications, {nnz} nonzero"
              f" entries = per-object multiplications, {rowadds} row"
              " additions = per-object additions",
              (len(monos), nnz, rowadds) == (b["setup"]["mul"],
                                             b["object"]["mul"],
                                             b["object"]["add"]))
    # hand-counted building blocks
    g = D.Dag()
    u = [g.var(f"u{i}") for i in range(3)]
    w = [g.var(f"w{i}") for i in range(3)]
    cr = D.cross(u, w)
    check("hand count: a cross product is 6 mul, 3 sub, 6 multiply-adds",
          D.count_dag(g, cr)[:3] == (6, 3, 0) and ma(D.count_dag(g, cr)) == 6)
    dt = [D.dot3(u, w)]
    check("hand count: a dot product of 3-vectors is 3 mul, 2 add, 3"
          " multiply-adds",
          D.count_dag(g, dt)[:3] == (3, 2, 0) and ma(D.count_dag(g, dt)) == 3)
    # other ops
    oo = other_ops()
    mons = expanded_monomials(line, plane, pt, C)
    for k, v in oo.items():
        say(f"   {k}: kernel counts (mul, add, sub, neg) typed {v['typed']},"
            f" canonical {v['canonical']}; monomials per output"
            f" {mons[k]}")
        result[k] = {"A_K_typed": v["typed"], "A_K_canonical": v["canonical"],
                     "monomials": mons[k]}
    # plan 0021's probe counts
    check("A's multiplications equal the restricted counts of the plan"
          " probes: 48, 72, 48, compose 48",
          [result[n]["A_K"]["mul"] for n in ("point", "line", "plane")]
          == [48, 72, 48] and oo["compose"]["typed"][0] == 48)
    # B, C of the product operations and compose: no like terms
    check("point^point and line v plane and compose: the number of"
          " monomials of the output polynomials equals the number of terms"
          " of form A (no like terms to collect)",
          sum(mons["point^point"]) == oo["point^point"]["typed"][0]
          and sum(mons["line v plane"]) == oo["line v plane"]["typed"][0]
          and sum(mons["compose"]) == oo["compose"]["typed"][0])

    write_tables(result)
    say("\nfailures:", FAILS if FAILS else "none")
    with open(os.path.join(HERE, "derivations.txt"), "w") as f:
        f.write("\n".join(OUT) + "\n")
    with open(os.path.join(HERE, "counts.json"), "w") as f:
        json.dump(result, f, indent=1, default=str)
    print("\n".join(l for l in OUT if l.startswith(("PASS", "FAIL"))))
    print("failures:", FAILS if FAILS else "none")
    return 1 if FAILS else 0


if __name__ == "__main__":
    sys.exit(main())
