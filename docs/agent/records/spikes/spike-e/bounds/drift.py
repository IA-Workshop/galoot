"""Observed rounding error of forms A, B, C of transform in rigid(3)
against their forward bounds, on correctly rounded unit motors and on chains
of 10, 100, and 1000 compositions, in double and in float; and the drift of
the chains off the motor's constraints, with what it does to each form.

Run from a checkout root with the math-tools environment's interpreter:

  <env>/bin/python -B docs/agent/records/spikes/spike-e/bounds/drift.py

It writes drift.txt and drift.json next to itself and exits 1 if an observed
rounding error exceeds its bound. Every value is compared in exact rational
arithmetic (python-flint's fmpq); the floating-point forms run on Python
floats (double) or on doubles rounded to binary32 after each operation
(float; fp.py says why that is exact).
"""

import json
import os
import random
import statistics
import sys

import mpmath as mp
from flint import fmpq

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

import fp                 # noqa: E402
from fp import R          # noqa: E402

SEED = 20261110
PRECISION = {"double": 53, "float": 24}
# (chain length, number of chains); length 1 is a single correctly rounded
# unit motor.
PLAN = [(1, 1000), (10, 300), (100, 100), (1000, 30)]
FORMS = ["A", "B", "C-pre"]
STEPS = "generic"
STEPS_LAM = "product of the steps' lambdas - 1 / u"
OUT = []
FAILS = []


def say(*a):
    OUT.append(" ".join(str(x) for x in a))


# --- Exact rationals and rounding ---------------------------------------------

def round_to(x, p):
    """x (fmpq) rounded to nearest, ties to even, with a p-bit significand,
    as a Python float (exact: p <= 53 and no overflow or underflow here)."""
    if x == 0:
        return 0.0
    neg = x < 0
    a = -x if neg else x
    num, den = int(a.p), int(a.q)
    e = num.bit_length() - den.bit_length()
    if (num << max(0, -e)) < (den << max(0, e)):
        e -= 1
    # a in [2^e, 2^(e+1)); scale so the significand has p bits
    shift = p - 1 - e
    n = num << shift if shift >= 0 else num
    d = den if shift >= 0 else den << -shift
    q, r = divmod(n, d)
    if 2 * r > d or (2 * r == d and q % 2 == 1):
        q += 1
    v = float(q) * 2.0 ** (-shift)
    return -v if neg else v


def gamma(k, u):
    return fmpq(k) * u / (1 - fmpq(k) * u)


def unit_motor(rng):
    """An exact unit motor: a rotor on a rational point of S^3, then a
    translation by up to 2 in each coordinate. The point of S^3 is the
    stereographic image of (a, b, c), with 21-bit dyadic a, b, c in [-2, 2]
    (STEPS "generic") or with small numerators and denominators ("small"),
    whose coefficients round the same way in most draws."""
    if STEPS == "small":
        a, b, c = (fmpq(rng.randint(-6, 6), rng.randint(1, 4))
                   for _ in range(3))
    else:
        a, b, c = (fmpq(rng.randint(-2**20, 2**20), 2**19) for _ in range(3))
    n2 = a * a + b * b + c * c
    rot = [2 * a / (1 + n2), 2 * b / (1 + n2), 2 * c / (1 + n2),
           (n2 - 1) / (1 + n2), fmpq(0), fmpq(0), fmpq(0), fmpq(0)]
    tr = [fmpq(0), fmpq(0), fmpq(0), fmpq(1)] + \
        [fmpq(rng.randint(-8, 8), 8) for _ in range(3)] + [fmpq(0)]
    return fp.compose(tr, rot)


def constraints(q):
    vx, vy, vz, s, mx, my, mz, t = q
    lam = vx * vx + vy * vy + vz * vz + s * s
    h = vx * mx + vy * my + vz * mz + s * t
    habs = abs(vx * mx) + abs(vy * my) + abs(vz * mz) + abs(s * t)
    return lam, h, habs


def renormalize(q):
    """The bulk minus its component along the weight, exactly: h becomes 0
    and the point and plane sandwiches do not change."""
    lam, h, _ = constraints(q)
    a = -h / lam
    return q[:4] + [q[4] + a * q[0], q[5] + a * q[1], q[6] + a * q[2],
                    q[7] + a * q[3]]


# --- Objects -------------------------------------------------------------------

def dyadic(rng, bits, scale):
    return fmpq(rng.randint(-2**bits, 2**bits), 2**bits) * scale


def draw_object(rng, name):
    """Coefficients exact in float and double: a point with 20-bit
    coordinates, a line through two points of 8-bit coordinates (so d . k
    = 0 exactly), a plane with 20-bit coefficients."""
    if name == "point":
        return [dyadic(rng, 20, 2) for _ in range(3)] + \
            [fmpq(rng.randint(2**19, 2**21), 2**20)]
    if name == "plane":
        return [dyadic(rng, 20, 1) for _ in range(4)]
    p = [fmpq(rng.randint(-128, 128), 64) for _ in range(3)]
    q = [fmpq(rng.randint(-128, 128), 64) for _ in range(3)]
    d = [q[i] - p[i] for i in range(3)]
    k = [p[1] * q[2] - p[2] * q[1], p[2] * q[0] - p[0] * q[2],
         p[0] * q[1] - p[1] * q[0]]
    return d + k


# --- Programs -------------------------------------------------------------------

class Form:
    def __init__(self, name, layout, suffix, label, g, outs):
        self.names = fp.MOTOR_NAMES + [n + suffix for n, _, _ in layout]
        self.k = [kx for _, kx, _ in fp.track(g, outs)]  # exact doubling
        self.run = {m: fp.compile_program(g, outs, m)
                    for m in ("double", "float", "exact", "abs")}

    def __call__(self, mode, q, x):
        return self.run[mode](dict(zip(self.names, list(q) + list(x))))


PROGRAMS = {}
for _name, _layout, _suf in fp.OBJECTS:
    for _label, (_g, _outs) in fp.form_programs(_name, _layout, _suf).items():
        PROGRAMS[(_name, _label)] = Form(_name, _layout, _suf, _label, _g,
                                         _outs)


def mpq(x):
    return mp.mpf(int(x.p)) / mp.mpf(int(x.q))


def normalized(name, y):
    """y scaled to its unit, in 200-bit arithmetic: a point to weight 1, a
    line to a unit direction, a plane to a unit normal."""
    v = [mpq(c) for c in y]
    if name == "point":
        return [c / v[3] for c in v]
    n = mp.sqrt(v[0] ** 2 + v[1] ** 2 + v[2] ** 2)
    return [c / n for c in v]


def norm2(v):
    return sum(c * c for c in v)


def translation(q):
    """T = 2 (s m - t v + v x m), the motion's translation times lambda."""
    vx, vy, vz, s, mx, my, mz, t = q
    c = [vy * mz - vz * my, vz * mx - vx * mz, vx * my - vy * mx]
    return [2 * (s * m - t * v + w)
            for m, v, w in zip((mx, my, mz), (vx, vy, vz), c)]


# --- One motor, its objects, every form ------------------------------------------

def evaluate(scalar, q_stored, q_chain, rng, acc):
    u = fmpq(1, 2**PRECISION[scalar])
    qx = [fp.exact(c) for c in q_stored]
    lam, h, habs = constraints(qx)
    acc["lam"].append(float(abs(lam - 1) / u))
    acc["h_rel"].append(float(abs(2 * h) / (u * 2 * habs)) if habs else 0.0)
    q_ren = renormalize(qx)
    for name, layout, suf in fp.OBJECTS:
        x = draw_object(rng, name)
        xf = [float(c) for c in x]
        S = PROGRAMS[(name, "A")]("exact", qx, x)
        S_chain = PROGRAMS[(name, "A")]("exact", q_chain, x)
        geo_ref = normalized(name, S_chain)
        S_ren = PROGRAMS[(name, "A")]("exact", q_ren, x)
        MA = PROGRAMS[(name, "A")]("abs", qx, x)
        KA = PROGRAMS[(name, "A")].k
        unitA = [gamma(k, u) * m for k, m in zip(KA, MA)]
        ys = {}
        for label in FORMS:
            f = PROGRAMS[(name, label)]
            y = f(scalar, q_stored, xf)
            ys[label] = y
            e = f("exact", qx, x)
            mag = f("abs", qx, x)
            key = (name, label)
            for i in range(len(layout)):
                bound = gamma(f.k[i], u) * mag[i]
                err = abs(fp.exact(y[i]) - e[i])
                if bound == 0:
                    ratio = 0.0 if err == 0 else float("inf")
                else:
                    ratio = float(err / bound)
                acc["ratio"].setdefault(key, []).append(ratio)
                if ratio > 1:
                    FAILS.append(f"{scalar} {name} {label} output {i}: "
                                 f"error / bound = {ratio}")
                # against the sandwich of the stored motor, in units of A's
                # rounding bound
                model = abs(e[i] - S[i])
                acc["model"].setdefault(key, []).append(
                    float(model / unitA[i]) if unitA[i] else 0.0)
                tot = abs(fp.exact(y[i]) - S[i])
                acc["vs_S"].setdefault(key, []).append(
                    float(tot / unitA[i]) if unitA[i] else 0.0)
                acc["vs_chain"].setdefault(key, []).append(
                    float(abs(fp.exact(y[i]) - S_chain[i]) / unitA[i])
                    if unitA[i] else 0.0)
            # the geometric error: each result scaled to its unit (a point
            # to weight 1, a line to a unit direction, a plane to a unit
            # normal) against the intended motion's image, relative to the
            # image's norm, in units of u
            g = normalized(name, [fp.exact(c) for c in y])
            diff = mp.sqrt(sum((a - b) ** 2 for a, b in zip(g, geo_ref)))
            size = mp.sqrt(sum(b ** 2 for b in geo_ref))
            acc["geo"].setdefault(key, []).append(
                float(diff / size / mpq(u)))
        # how often a collected form changes the bits of the result
        for i in range(len(layout)):
            acc["bits"].setdefault(name, [0, 0])
            acc["bits"][name][0] += ys["A"][i] != ys["B"][i]
            acc["bits"][name][1] += 1
        if name == "line":
            # the moment's change if h were removed, against A's bound
            for i in range(3, 6):
                acc["line_h"].append(float(abs(S[i] - S_ren[i]) / unitA[i])
                                     if unitA[i] else 0.0)
        # form C's difference from the sandwich against the norm bound of
        # the README, for the outputs where it is not a plain multiple of x
        C = PROGRAMS[(name, "C-pre")]("exact", qx, x)
        tvec = translation(qx)
        tn = float(norm2(tvec)) ** 0.5
        if name == "line":
            d, k = x[:3], x[3:]
            dk = [float(C[i] - S[i]) for i in range(3, 6)]
            apriori = (float(abs(lam - 1)) * (float(norm2(k)) ** 0.5 +
                       2 * tn * float(norm2(d)) ** 0.5) +
                       2 * float(abs(h)) * float(norm2(d)) ** 0.5)
            acc["apriori"].append(sum(c * c for c in dk) ** 0.5 / apriori
                                  if apriori else 0.0)
        if name == "plane":
            n = x[:3]
            apriori = float(abs(lam - 1)) * (float(abs(x[3])) +
                                             2 * tn * float(norm2(n)) ** 0.5)
            acc["apriori"].append(float(abs(C[3] - S[3])) / apriori
                                  if apriori else 0.0)
        # the two arrangements of form C give the same bits: a doubling
        # commutes with rounding
        if PROGRAMS[(name, "C-pre")](scalar, q_stored, xf) != \
                PROGRAMS[(name, "C-post")](scalar, q_stored, xf):
            FAILS.append(f"{scalar} {name}: C-pre and C-post differ")


def chain(scalar, n, rng):
    """n exact unit motors, each rounded correctly to the scalar and composed
    in it, each new one applied after the chain so far; returns (the stored
    result, the exact product of the exact unrounded motors: the motion
    intended, an exact unit versor; the product of the rounded steps'
    lambdas, which is the stored result's lambda had compose been exact)."""
    p = PRECISION[scalar]
    rnd = fp.to_float32 if scalar == "float" else None
    stored = intended = None
    lam_steps = fmpq(1)
    for _ in range(n):
        exact_step = unit_motor(rng)
        step = [round_to(c, p) for c in exact_step]
        lam_steps *= constraints([fp.exact(c) for c in step])[0]
        if stored is None:
            stored, intended = step, exact_step
        else:
            stored = fp.compose(step, stored, rnd)
            intended = fp.compose(exact_step, intended)
    return stored, intended, lam_steps


def summary(values):
    v = sorted(values)
    if not v:
        return {}
    return {"median": statistics.median(v), "p99": v[int(0.99 * (len(v) - 1))],
            "max": v[-1], "count": len(v)}


def write_tables(results, tag):
    rows = [f"{s} N={n}" for s in ("double", "float") for n, _ in PLAN]
    objs = [o for o, _, _ in fp.OBJECTS]

    def cell(r, key, stat="max"):
        v = results[r].get(key)
        return f"{v[stat]:.3g}" if v else "-"

    md = ["# Drift and rounding of forms A, B, C, generated by drift.py", "",
          f"Seed {SEED}, steps {STEPS!r}. N = 1 is one correctly rounded "
          "unit motor; N > 1 a "
          "chain of N correctly rounded unit motors composed in the scalar "
          "with the library's compose (bit for bit, replay.txt). Each motor "
          "moves one point, one line, and one plane. Forms C-pre and C-post "
          "give the same bits, so C stands for both.", "",
          "## Drift of the motor (median / max)", "",
          "| scalar, N | chains | abs(lambda - 1) / u | the same for the "
          "product of the steps' lambdas | abs(2h) / (u T) |",
          "| --- | ---: | ---: | ---: | ---: |"]
    for r, (n, c) in zip(rows, PLAN * 2):
        md.append(f"| {r} | {c} | {cell(r, 'lambda-1 / u', 'median')} / "
                  f"{cell(r, 'lambda-1 / u')} | "
                  f"{cell(r, STEPS_LAM, 'median')} / {cell(r, STEPS_LAM)} | "
                  f"{cell(r, '|2h| / (u T)', 'median')} / "
                  f"{cell(r, '|2h| / (u T)')} |")

    def table(title, what, forms=("A", "B", "C-pre"), stat="max"):
        md.extend(["", f"## {title}", "",
                   "| scalar, N | " + " | ".join(
                       f"{o} {f[0]}" for o in objs for f in forms) + " |",
                   "| --- |" + " ---: |" * (len(objs) * len(forms))])
        for r in rows:
            md.append(f"| {r} | " + " | ".join(
                cell(r, f"{what} {o} {f}", stat) for o in objs for f in forms)
                + " |")

    table("Rounding error over the form's own bound gamma_K M (max)", "ratio")
    table("Error against the sandwich of the stored motor, in units of "
          "form A's bound (max)", "vs_S")
    table("The same, median", "vs_S", stat="median")
    table("Error against the sandwich of the intended motion (the exact "
          "product of the unrounded steps), in units of form A's bound (max)",
          "vs_chain")
    table("The same, median", "vs_chain", stat="median")
    table("Geometric error against the intended motion: each image scaled "
          "to its unit (point weight 1, unit direction, unit normal), "
          "relative to the image's norm, in units of u (max)", "geo")
    table("The same, median", "geo", stat="median")
    md.extend(["", "## Share of output coefficients whose value differs "
               "between forms A and B", "",
               "| scalar, N | point | line | plane |", "| --- | ---: | ---: | "
               "---: |"])
    for r in rows:
        md.append(f"| {r} | " + " | ".join(
            cell(r, f"A and B differ {o}") for o in objs) + " |")
    md.extend(["", "## The line under forms A and B: the moment's change "
               "if h were removed, in units of form A's bound; and form C's "
               "difference from the sandwich over the norm bound of the "
               "README (line moment and plane offset)", "",
               "| scalar, N | line h-change median | max | C - S over its "
               "bound, max |", "| --- | ---: | ---: | ---: |"])
    key_h = "line moment change from h / A bound"
    key_c = "C - S over its norm bound (line moment, plane offset)"
    for r in rows:
        md.append(f"| {r} | {cell(r, key_h, 'median')} | {cell(r, key_h)} | "
                  f"{cell(r, key_c)} |")
    with open(os.path.join(HERE, f"drift{tag}.md"), "w") as f:
        f.write("\n".join(md) + "\n")


def main():
    global PLAN
    global STEPS
    if "--quick" in sys.argv:
        PLAN = [(1, 50), (10, 10), (100, 5), (1000, 2)]
    if "--small-steps" in sys.argv:
        STEPS = "small"
    tag = "-small" if STEPS == "small" else ""
    mp.mp.prec = 200
    rng = random.Random(SEED)
    results = {}
    for scalar in ("double", "float"):
        for n, count in PLAN:
            acc = {"lam": [], "lam_steps": [], "h_rel": [], "ratio": {}, "model": {},
                   "vs_S": {}, "vs_chain": {}, "line_h": [],
                   "apriori": [], "geo": {}, "bits": {}}
            for _ in range(count):
                stored, exact_chain, lam_steps = chain(scalar, n, rng)
                acc["lam_steps"].append(float(abs(lam_steps - 1) / fmpq(
                    1, 2**PRECISION[scalar])))
                evaluate(scalar, stored, exact_chain, rng, acc)
            res = {"lambda-1 / u": summary(acc["lam"]),
                   STEPS_LAM: summary(acc["lam_steps"]),
                   "|2h| / (u T)": summary(acc["h_rel"]),
                   "line moment change from h / A bound":
                       summary(acc["line_h"]),
                   "C - S over its norm bound (line moment, plane offset)":
                       summary(acc["apriori"])}
            for what in ("ratio", "model", "vs_S", "vs_chain", "geo"):
                for key, vals in acc[what].items():
                    res[f"{what} {key[0]} {key[1]}"] = summary(vals)
            for name, (diff, total) in acc["bits"].items():
                res[f"A and B differ {name}"] = {
                    "median": diff / total, "p99": diff / total,
                    "max": diff / total, "count": total}
            results[f"{scalar} N={n}"] = res
            say(f"== {scalar}, chains of {n} ({count} chains)")
            for k, v in res.items():
                if v:
                    say(f"  {k}: median {v['median']:.4g}, 99th pct "
                        f"{v['p99']:.4g}, max {v['max']:.4g} "
                        f"(n = {v['count']})")
    write_tables(results, tag)
    say(f"{len(FAILS)} failures" if FAILS else
        "every observed rounding error is within its bound, and the two "
        "arrangements of form C agree bit for bit")
    for f in FAILS[:20]:
        say("FAIL " + f)
    with open(os.path.join(HERE, f"drift{tag}.txt"), "w") as f:
        f.write("\n".join(OUT) + "\n")
    with open(os.path.join(HERE, f"drift{tag}.json"), "w") as f:
        json.dump(results, f, indent=1, sort_keys=True)
    print("\n".join(OUT))
    sys.exit(1 if FAILS else 0)


if __name__ == "__main__":
    main()
