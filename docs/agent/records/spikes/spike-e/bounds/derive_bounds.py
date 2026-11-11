"""Forward error bounds of forms A, B, C of transform in rigid(3): the
counts of roundings K, the magnitudes M, and the exact identities that
relate form C to the sandwich off the motor's constraints.

Run from a checkout root with the math-tools environment's interpreter:

  <env>/bin/python -B docs/agent/records/spikes/spike-e/bounds/derive_bounds.py

It writes derive_bounds.txt next to itself and exits 1 if a check fails.
"""

import os
import random
import sys

import sympy as sp

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

import fp                 # noqa: E402
from fp import R, P       # noqa: E402

SEED = 20261110
OUT = []
FAILS = []


def say(*a):
    OUT.append(" ".join(str(x) for x in a))


def check(name, ok):
    say(("PASS " if ok else "FAIL ") + name)
    if not ok:
        FAILS.append(name)


Q = R.symbols(R.MOTOR)
vx, vy, vz, s, mx, my, mz, t = Q
V = [vx, vy, vz]
M3 = [mx, my, mz]
LAM = vx**2 + vy**2 + vz**2 + s**2
H = vx * mx + vy * my + vz * mz + s * t
AQ = {q: sp.Symbol("A" + str(q), nonnegative=True) for q in Q}
ROT = [AQ[vx], AQ[vy], AQ[vz], AQ[s]]
BULK = [AQ[mx], AQ[my], AQ[mz], AQ[t]]
ALAM = sum(r**2 for r in ROT)


def cross(a, b):
    return [a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0]]


def dot(a, b):
    return sum(x * y for x, y in zip(a, b))


def sandwich(layout, suffix, q=Q):
    X = R.build(layout, R.symbols(layout, suffix))
    Y, extra = R.read(layout, R.transform(X, R.build(R.MOTOR, q)))
    assert not extra
    return Y


def rot_lambda(u):
    """R_lambda u = (s^2 - |v|^2) u + 2 (v . u) v + 2 s v x u."""
    c = cross(V, u)
    return [sp.expand((s**2 - dot(V, V)) * u[i] + 2 * dot(V, u) * V[i]
                      + 2 * s * c[i]) for i in range(3)]


T_VEC = [sp.expand(2 * (s * M3[i] - t * V[i] + cross(V, M3)[i]))
         for i in range(3)]


# --- 1. K and M of each form, and collection ------------------------------------

def counts_and_magnitudes():
    say("== 1. Counts of roundings K (core rule / exact doubling) and "
        "magnitudes M")
    table = {}
    for name, layout, suf in fp.OBJECTS:
        progs = fp.form_programs(name, layout, suf)
        tr = {f: fp.track(*progs[f]) for f in progs}
        Y = sandwich(layout, suf)
        # every program computes its polynomial: A and B the sandwich
        for f in ("A", "B"):
            g, outs = progs[f]
            check(f"{name}: form {f} computes the sandwich",
                  all(sp.expand(o.expr - y) == 0 for o, y in zip(outs, Y)))
        for i, (n, _, _) in enumerate(layout):
            row = {f: (tr[f][i][0], tr[f][i][1]) for f in tr}
            table[(name, n)] = row
            say(f"{name} {n}: " + "; ".join(
                f"{f} K={row[f][0]}/{row[f][1]}" for f in tr))
        same = all(sp.expand(tr["A"][i][2] - tr["B"][i][2]) == 0
                   for i in range(len(layout)))
        check(f"{name}: M_A = M_B as polynomials for every output "
              "(collection combines no like terms of opposite sign)", same)
        # the magnitude of A is the sum over the raw sandwich paths: compare
        # the number of raw paths with the number of monomials
        first, second, mid, dropped = P.sandwich_terms(R.MOTOR, layout)
        paths = [0] * len(layout)
        by_mid = {}
        for a, b, o, c in first:
            by_mid.setdefault(o, []).append((a, b, c))
        for a, b, o, c in second:
            paths[o] += len(by_mid[a])
        for i, (n, _, _) in enumerate(layout):
            mono = sp.Poly(Y[i], *Q, *R.symbols(layout, suf)).terms()
            weight = sum(abs(int(c)) for _, c in mono)
            check(f"{name} {n}: {paths[i]} raw paths, {len(mono)} monomials"
                  f" whose |coefficients| sum to {weight} = paths",
                  weight == paths[i])
        table[(name, "M")] = [tr["A"][i][2] for i in range(len(layout))]
        table[(name, "MC")] = [tr["C-pre"][i][2] for i in range(len(layout))]
        table[(name, "MCpost")] = [tr["C-post"][i][2]
                                   for i in range(len(layout))]
    return table


# --- 2. Norm bounds of the magnitude --------------------------------------------

def magnitude_structure(table):
    """Each M is a sum over the object's coefficients |x_j| times a
    polynomial c_j in |Q| that is one of: lambda (the four squares of the
    rotation part), 2(|a||b| + |c||d|) with {a, b, c, d} the four rotation
    coefficients, or 2 sum_i |b_sigma(i)| |r_i| with sigma a bijection from
    the rotation coefficients r to the bulk coefficients b. Each of the
    first two is at most lambda, the third at most 2 |r| |b| (Cauchy-
    Schwarz), which gives the norm bounds of the README."""
    say("== 2. Structure of the magnitudes (the norm lemma)")
    for name, layout, suf in fp.OBJECTS:
        xs = [sp.Symbol("A" + n + suf, nonnegative=True) for n, _, _ in layout]
        ok = True
        kinds = {}
        for i, (n, _, _) in enumerate(layout):
            Mi = sp.Poly(table[(name, "M")][i], *xs)
            for mono, c in Mi.terms():
                j = mono.index(1)
                c = sp.expand(c)
                if sp.expand(c - ALAM) == 0:
                    kinds.setdefault("lambda", 0)
                    kinds["lambda"] += 1
                    continue
                terms = sp.Poly(c, *ROT, *BULK).terms()
                if not all(int(k) == 2 for _, k in terms) or len(terms) not in (2, 4):
                    ok = False
                    continue
                facs = [[v for v, e in zip(ROT + BULK, m) for _ in range(e)]
                        for m, _ in terms]
                if len(terms) == 2:
                    used = sorted(str(v) for f in facs for v in f)
                    ok &= used == sorted(str(v) for v in ROT)
                    kinds.setdefault("pair", 0)
                    kinds["pair"] += 1
                else:
                    rs = sorted(str(f[0]) if f[0] in ROT else str(f[1])
                                for f in facs)
                    bs = sorted(str(f[1]) if f[1] in BULK else str(f[0])
                                for f in facs)
                    ok &= rs == sorted(map(str, ROT)) and \
                        bs == sorted(map(str, BULK))
                    kinds.setdefault("bijection", 0)
                    kinds["bijection"] += 1
        check(f"{name}: every coefficient of M is lambda, a rotation pair "
              f"sum, or a rotation-bulk bijection sum {kinds}", ok)


# --- 3. The point and plane sandwiches ignore h; the line's moment does not -----

def drift_identities():
    say("== 3. The sandwich off the constraints")
    a = sp.Symbol("alpha")
    shifted = [vx, vy, vz, s, mx + a * vx, my + a * vy, mz + a * vz, t + a * s]
    for name, layout, suf in fp.OBJECTS:
        Y = sandwich(layout, suf)
        Ys = sandwich(layout, suf, shifted)
        d = [sp.expand(ys - y) for ys, y in zip(Ys, Y)]
        if name == "line":
            dl = R.symbols(layout, suf)[:3]
            rd = rot_lambda(dl)
            check("line: bulk += alpha * weight leaves the direction and "
                  "adds 2 alpha R_lambda(d) to the moment",
                  all(x == 0 for x in d[:3]) and
                  all(sp.expand(d[3 + i] - 2 * a * rd[i]) == 0
                      for i in range(3)))
        else:
            check(f"{name}: bulk += alpha * weight leaves the sandwich "
                  "unchanged (it does not depend on h)", all(x == 0 for x in d))
    check("h(bulk + alpha weight) = h + alpha lambda",
          sp.expand(H.subs({mx: mx + a * vx, my: my + a * vy, mz: mz + a * vz,
                            t: t + a * s}, simultaneous=True) - H - a * LAM)
          == 0)
    check("T(bulk + alpha weight) = T", all(
        sp.expand(Ti.subs({mx: mx + a * vx, my: my + a * vy,
                           mz: mz + a * vz, t: t + a * s},
                          simultaneous=True) - Ti) == 0 for Ti in T_VEC))
    check("|T|^2 = 4 (lambda |bulk|^2 - h^2), so 2 sqrt(lambda) |bulk| = "
          "|T| when h = 0", sp.expand(dot(T_VEC, T_VEC) - 4 * (
              LAM * (mx**2 + my**2 + mz**2 + t**2) - H**2)) == 0)
    # closed forms of the sandwich for any motor
    p = R.symbols(R.POINT, "p")
    Y = sandwich(R.POINT, "p")
    ru = rot_lambda(p[:3])
    check("point: S = (R_lambda u + w T, lambda w) for any motor",
          all(sp.expand(Y[i] - ru[i] - p[3] * T_VEC[i]) == 0
              for i in range(3)) and sp.expand(Y[3] - LAM * p[3]) == 0)
    g = R.symbols(R.PLANE, "g")
    Y = sandwich(R.PLANE, "g")
    rn = rot_lambda(g[:3])
    check("plane: S = (R_lambda n, lambda g_w) - (0, R_lambda(n) . T) / "
          "lambda, i.e. lambda S_w = lambda^2 g_w - R_lambda(n) . T",
          all(sp.expand(Y[i] - rn[i]) == 0 for i in range(3)) and
          sp.expand(LAM * Y[3] - LAM**2 * g[3] + dot(rn, T_VEC)) == 0)
    ln = R.symbols(R.LINE, "l")
    Y = sandwich(R.LINE, "l")
    rd = rot_lambda(ln[:3])
    rk = rot_lambda(ln[3:])
    tx = cross(T_VEC, rd)
    check("line: S_d = R_lambda d and lambda S_k = lambda R_lambda k + "
          "T x R_lambda d + 2 h R_lambda d for any motor",
          all(sp.expand(Y[i] - rd[i]) == 0 for i in range(3)) and
          all(sp.expand(LAM * Y[3 + i] - LAM * rk[i] - tx[i] - 2 * H * rd[i])
              == 0 for i in range(3)))
    check("R_lambda u . R_lambda w = lambda^2 u . w (R_lambda / lambda is a "
          "rotation for any lambda > 0)",
          sp.expand(dot(rot_lambda(p[:3]), rot_lambda(g[:3])) -
                    LAM**2 * dot(p[:3], g[:3])) == 0)
    dk = sp.expand(dot(Y[:3], Y[3:]))
    check("line: S_d . S_k = lambda^2 (d . k) + 2 h lambda |d|^2 (the "
          "Plücker defect of the image)",
          sp.expand(dk - LAM**2 * dot(ln[:3], ln[3:]) -
                    2 * H * LAM * dot(ln[:3], ln[:3])) == 0)


# --- 4. Form C against the sandwich ----------------------------------------------

def form_C_identities():
    say("== 4. Form C minus the sandwich, as polynomials in lambda - 1 and h")
    for name, layout, suf in fp.OBJECTS:
        Y = sandwich(layout, suf)
        x = R.symbols(layout, suf)
        for label in ("C-pre", "C-post"):
            g, outs = fp.form_programs(name, layout, suf)[label]
            C = [o.expr for o in outs]
            if name == "point":
                ok = all(sp.expand(Y[i] - C[i] - (LAM - 1) * x[i]) == 0
                         for i in range(4))
                check(f"point {label}: S - C = (lambda - 1) p", ok)
            elif name == "plane":
                n = x[:3]
                ok = all(sp.expand(Y[i] - C[i] - (LAM - 1) * n[i]) == 0
                         for i in range(3))
                rn = rot_lambda(n)
                ok &= sp.expand(LAM * (Y[3] - C[3]) - (LAM - 1) *
                                (LAM * x[3] - LAM * dot(n, T_VEC) +
                                 dot(rn, T_VEC))) == 0
                check(f"plane {label}: S_n - C_n = (lambda - 1) n and "
                      "S_w - C_w = (lambda - 1)(g_w - n . T + R^(n) . T), "
                      "R^ = R_lambda / lambda", ok)
            else:
                d, k = x[:3], x[3:]
                ok = all(sp.expand(Y[i] - C[i] - (LAM - 1) * d[i]) == 0
                         for i in range(3))
                rd = rot_lambda(d)
                td = cross(T_VEC, d)
                trd = cross(T_VEC, rd)
                ok &= all(sp.expand(
                    LAM * (Y[3 + i] - C[3 + i]) -
                    (LAM - 1) * (LAM * k[i] + LAM * td[i] - trd[i]) -
                    2 * H * rd[i]) == 0 for i in range(3))
                check(f"line {label}: S_d - C_d = (lambda - 1) d and "
                      "S_k - C_k = (lambda - 1)(k + T x d - T x R^ d) + "
                      "2 h R^ d", ok)
    say("Euclidean reading for a point of weight w = 1: C's image minus the "
        "image under the motion S represents is (lambda - 1) times that "
        "point's displacement (from S - C = (lambda - 1) p; README).")


# --- 5. Magnitudes of form C against form A at unit motors ----------------------

def unit_motor(rng):
    def unit_quat():
        a, b, c = (sp.Rational(rng.randint(-5, 5), rng.randint(1, 3))
                   for _ in range(3))
        n2 = a * a + b * b + c * c
        return [2 * a / (1 + n2), 2 * b / (1 + n2), 2 * c / (1 + n2),
                (n2 - 1) / (1 + n2)]

    rot = R.build(R.MOTOR, unit_quat() + [0, 0, 0, 0])
    tr = R.build(R.MOTOR, [0, 0, 0, 1] +
                 [sp.Rational(rng.randint(-8, 8), 4) for _ in range(3)] + [0])
    c, extra = R.read(R.MOTOR, R.antiproduct(tr, rot))
    assert not extra
    return c


def magnitude_ratios(table, rng, samples=200):
    say("== 5. M_C / M_A at exact unit motors (rotation then translation, "
        f"{samples} samples, objects with |coefficients| in [1/8, 1])")
    for name, layout, suf in fp.OBJECTS:
        xs = [sp.Symbol("A" + n + suf, nonnegative=True) for n, _, _ in layout]
        fa = [sp.lambdify(list(AQ.values()) + xs, m)
              for m in table[(name, "M")]]
        fc = {lab: [sp.lambdify(list(AQ.values()) + xs, m)
                    for m in table[(name, key)]]
              for lab, key in (("C-pre", "MC"), ("C-post", "MCpost"))}
        worst = {lab: [0.0] * len(layout) for lab in fc}
        for _ in range(samples):
            q = [abs(float(c)) for c in unit_motor(rng)]
            x = [rng.uniform(0.125, 1.0) for _ in layout]
            for lab in fc:
                for i in range(len(layout)):
                    r = fc[lab][i](*q, *x) / fa[i](*q, *x)
                    worst[lab][i] = max(worst[lab][i], r)
        for lab in fc:
            say(f"{name} {lab}: largest M_C / M_A per output " +
                ", ".join(f"{n} {w:.3f}" for (n, _, _), w in
                          zip(layout, worst[lab])))


# --- 6. The dropped coefficients of checking mode -----------------------------

def dropped():
    say("== 6. Coefficients transform drops in checking mode (form A)")
    for name, layout, suf in fp.OBJECTS:
        k, masks = fp.dropped_roundings(layout)
        say(f"{name}: dropped blades {[bin(m) for m in masks]}, K = {k}")
        X = R.build(layout, R.symbols(layout, suf))
        full = R.transform(X, R.build(R.MOTOR, Q))
        check(f"{name}: every dropped coefficient is 0 as a polynomial, so a "
              "collected form has nothing to drop or check",
              all(full.get(m, 0) == 0 for m in masks))


def main():
    rng = random.Random(SEED)
    table = counts_and_magnitudes()
    magnitude_structure(table)
    drift_identities()
    form_C_identities()
    magnitude_ratios(table, rng)
    dropped()
    say(f"{len(FAILS)} checks failed" if FAILS else "all checks passed")
    with open(os.path.join(HERE, "derive_bounds.txt"), "w") as f:
        f.write("\n".join(OUT) + "\n")
    print("\n".join(OUT))
    sys.exit(1 if FAILS else 0)


if __name__ == "__main__":
    main()
