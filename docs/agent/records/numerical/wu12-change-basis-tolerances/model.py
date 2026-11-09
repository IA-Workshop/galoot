"""A model check of the change-of-basis tolerance rule, run after the rule was
committed. It is a model of the assumed operation form in Python floats
(IEEE double, round to nearest), not the library, which does not exist yet.

For each output blade, the model evaluates the integer combination as the
kernel's accumulate does (terms in ascending order of source blade, the first
assigned, +1 and -1 as an addition or a subtraction, other coefficients as
float(c) * x), then divides once by float(q) when q != 1. Beside it, a model
of tracked follows the same tree with the record's rules, including the
division rule. The rule's checks are then applied exactly as the record
states them, with the computed ratio formed in double as the C++ helper
would form it.

Three mutants of rounding.toml are modeled:
  per_term  the division applied to each term before the sum (the tracked
            model then follows the mutated tree, as the real oracle would);
  drop_div  the division term dropped from the rule (regime E: exactness
            expected in place of correct rounding; regime C: K - 1);
  scaled    every floating result times (1 + 2^-48).

It reports, per change and regime, the share of draws in which the rule
rejects at least one coefficient (a kill), and for the unmutated model the
worst exact ratio |d - e| / (gamma_K M) over coefficients with K >= 2.

Run from the repository root:
  python3 -I docs/agent/records/numerical/wu12-change-basis-tolerances/model.py
Seed 20261109, Python's random.Random (Mersenne Twister; its integer draws
are specified by the Python version, here CPython 3.14.9).
"""

import pathlib
import random
import sys
from fractions import Fraction

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import changes  # noqa: E402

ref = changes.ref
U = Fraction(1, 2**53)
RATIO_LIMIT = 1.0 + 4 * 2.0**-53
SEED = 20261109


def is_pow2(q):
    return q > 0 and q & (q - 1) == 0


def odd_part(n):
    while n % 2 == 0:
        n //= 2
    return n


def lcm(a, b):
    from math import lcm as _lcm
    return _lcm(a, b)


def kernel(p):
    """Per output: (q, [(source index, c)]), sources in ascending order."""
    n = len(p)
    import itertools
    blades = []
    for k in range(n + 1):
        blades.extend(itertools.combinations(range(n), k))
    pos = {b: i for i, b in enumerate(blades)}
    out = []
    for cols in blades:
        k = len(cols)
        terms = []
        for rows in itertools.combinations(range(n), k):
            minor = ref.det([[p[r][c] for c in cols] for r in rows])
            if minor != 0:
                terms.append((pos[rows], minor))
        q = 1
        for _, m in terms:
            q = lcm(q, m.denominator)
        out.append((q, [(a, int(m * q)) for a, m in terms]))
    return out


# --- the double model and the tracked model ---------------------------------

def run_double(ker, xs, per_term=False):
    out = []
    for q, terms in ker:
        s = None
        for a, c in terms:
            x = xs[a]
            t = x if c == 1 else (-x if c == -1 else float(c) * x)
            if per_term and q != 1:
                t = t / float(q)
            if s is None:
                s = t
            else:
                # out + value and out - value are the same as out + (+-x).
                s = s + t
        if s is None:
            s = 0.0
        if not per_term and q != 1:
            s = s / float(q)
        out.append(s)
    return out


def t_input(v):
    return (v, abs(v), 0)


def t_add(a, b):
    return (a[0] + b[0], a[1] + b[1], max(a[2], b[2]) + 1)


def t_mul_const(c, a):
    return (c * a[0], abs(c) * a[1], a[2] + 1)


def t_div_const(a, q):
    k = a[2] if is_pow2(q) else a[2] + 1
    return (a[0] / q, a[1] / q, k)


def run_tracked(ker, xs, per_term=False):
    out = []
    for q, terms in ker:
        s = None
        for a, c in terms:
            x = t_input(xs[a])
            if c == 1:
                t = x
            elif c == -1:
                t = (-x[0], x[1], x[2])
            else:
                t = t_mul_const(c, x)
            if per_term and q != 1:
                t = t_div_const(t, q)
            s = t if s is None else t_add(s, t)
        if s is None:
            s = (Fraction(0), Fraction(0), 0)
        if not per_term and q != 1:
            s = t_div_const(s, q)
        out.append(s)
    return out


# --- the rule's checks -------------------------------------------------------

class Stop(Exception):
    """The helper's stop: in the C++ test it aborts, so the test fails."""


def correctly_rounded(e):
    w = odd_part(e.denominator)
    if w == 1:
        # Dyadic: nearest_double rounds the numerator once and scales exactly.
        return float(e)
    a = e * w
    fa = float(a)
    if Fraction(fa) != a:
        raise Stop("w e is not a double")
    r = fa / float(w)
    assert r == float(e)  # Python's own correctly rounded conversion agrees
    return r


def computed_ratio(d, e, k, m):
    w = lcm(odd_part(e.denominator), odd_part(m.denominator))
    diff = Fraction(d) * w - e * w
    if diff == 0:
        return 0.0
    kk = float(k)
    g = (kk * 2.0**-53) / (1.0 - kk * 2.0**-53)
    return abs(float(diff)) / (g * float(m * w))


def exact_ratio(d, e, k, m):
    if m == 0:
        return Fraction(0)
    gk = k * U / (1 - k * U)
    return abs(Fraction(d) - e) / (gk * m)


def check_e(d, tr, q, drop_div):
    e = tr[0]
    if is_pow2(q) or drop_div:
        return Fraction(d) == e
    return d == correctly_rounded(e)


def check_c(d, tr, drop_div):
    e, m, k = tr
    if drop_div and k >= 1:
        k -= 1
    if k == 0:
        return Fraction(d) == e
    if k == 1:
        return d == correctly_rounded(e)
    return computed_ratio(d, e, k, m) <= RATIO_LIMIT


# --- drivers -----------------------------------------------------------------

def draw(rng, size, regime, n_r):
    if regime == "E":
        return [Fraction(rng.randint(-2**20, 2**20), 2**20) for _ in range(size)]
    return [Fraction(rng.randint(-n_r, n_r)) for _ in range(size)]


def study(name, p, regime, draws, rng):
    ker = kernel(p)
    size = len(ker)
    big_c = max(sum(abs(c) for _, c in terms) for _, terms in ker)
    n_r = 2**53
    while big_c * n_r > 2**60:
        n_r //= 2
    kills = {"none": 0, "per_term": 0, "drop_div": 0, "scaled": 0}
    worst = Fraction(0)
    ratios = []
    counted = 0
    rounded = 0
    for _ in range(draws):
        xs_exact = draw(rng, size, regime, n_r)
        xs = [float(x) for x in xs_exact]
        assert all(Fraction(a) == b for a, b in zip(xs, xs_exact))
        tr = run_tracked(ker, xs_exact)
        good = run_double(ker, xs)
        for mutant in kills:
            if mutant == "per_term":
                d = run_double(ker, xs, per_term=True)
                t = run_tracked(ker, xs_exact, per_term=True)
            else:
                d = good
                t = tr
            if mutant == "scaled":
                d = [x * (1.0 + 2.0**-48) for x in d]
            drop = mutant == "drop_div"
            ok = True
            try:
                for i, (q, _) in enumerate(ker):
                    if regime == "E":
                        ok = ok and check_e(d[i], t[i], q, drop)
                    else:
                        ok = ok and check_c(d[i], t[i], drop)
            except Stop:
                ok = False
            if not ok:
                kills[mutant] += 1
        for i in range(size):
            e, m, k = tr[i]
            if Fraction(good[i]) != e:
                rounded += 1
            if regime == "C" and k >= 2:
                r = exact_ratio(good[i], e, k, m)
                counted += 1
                ratios.append(r)
                worst = max(worst, r)
    ratios.sort()
    med = float(ratios[len(ratios) // 2]) if ratios else 0.0
    qs = sorted({q for q, _ in ker})
    line = (f"{name}, regime {regime}: {draws} draws, {size} coefficients each, "
            f"q in {qs}; inexact results {rounded} of {draws * size}; "
            f"rejecting draws: correct {kills['none']}, per_term "
            f"{kills['per_term']}, drop_div {kills['drop_div']}, scaled "
            f"{kills['scaled']}")
    if regime == "C":
        line += (f"; N_R = 2^{n_r.bit_length() - 1}; K >= 2 coefficients "
                 f"{counted}, exact ratio median {med:.3g}, worst "
                 f"{float(worst):.3g}")
    print(line)
    assert kills["none"] == 0, "the rule rejected the unmutated model"


def round_trip(d, draws, rng):
    p = changes.conformal_change(d)
    fwd, back = kernel(p), kernel(ref.inverse(p))
    size = len(fwd)
    for _ in range(draws):
        xs = draw(rng, size, "E", 0)
        y = run_double(fwd, [float(x) for x in xs])
        z = run_double(back, y)
        assert [Fraction(v) for v in z] == xs
    print(f"conformal({d}) round trip, regime E: {draws} draws exact")


def main():
    rng = random.Random(SEED)
    forms = ref.random_forms()
    cases = [("conformal(3) -> R(4,1,0)", changes.conformal_change(3)),
             ("R(4,1,0) -> conformal(3)", ref.inverse(changes.conformal_change(3)))]
    for k in (2, 4, 7, 8, 9):
        p, _ = changes.ldlt_change(forms[k])
        cases.append((f"random {k} -> diagonal", p))
    p7, _ = changes.ldlt_change(forms[7])
    cases.append(("diagonal -> random 7", ref.inverse(p7)))
    for name, p in cases:
        study(name, p, "E", 400, rng)
        study(name, p, "C", 400, rng)
    for d in range(1, 5):
        round_trip(d, 200, rng)


if __name__ == "__main__":
    main()
