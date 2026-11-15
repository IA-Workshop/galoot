"""Multiply-add contraction and the rounding check of transform, in exact
arithmetic, for rigid(3) motors acting on points, lines, and planes.

The kernels are those of the library, from the spike-E emulation
(../../spikes/spike-e/plans.py), whose term order replay.py there matched
with the library bit for bit; probe.py here matches it again. Every value is
held as an exact rational (python-flint fmpq) and every operation is rounded
by `rnd` below, correctly to nearest even with gradual underflow, to binary64
or binary32. A fused multiply-add is the exact a*b + c rounded once.

A contraction pattern says, for each accumulation out +/- x*y of a kernel
after an output's first term, whether the compiler fuses it, and which
product it fuses:
  off     none (the library's model, -ffp-contract=off);
  current every accumulation fuses its own product: fma(+/-x, y, out);
  first   as current, except that the second term of an output fuses the
          first term's product, held unrounded: fma(+/-x1, y1, fl(x2*y2));
  random  each accumulation, independently: no fusion, current, or (at the
          second term) first, with equal probability.
The padded magnitude is run unfused, as the library's model, and fused
(each fl(fl(|x||y|) + tau) as fma(|x|, |y|, tau)), and under random, each
with the library's padding tau = lambda/u and with tau = lambda, the
smallest the derivation admits (lambda the smallest normal number).

For each case it checks, for every coefficient y of the kept and the dropped
outputs, with K the core's count (kernel_roundings), K' the count of the
contracted program (a fused node counts max(K_a + K_b, K_s) + 1), and P the
exact padded magnitude with tau = lambda:
  (1) K' <= K;
  (2) |y_computed - y_exact| <= gamma_K' P  (Lemma A under contraction);
  (3) |d| <= tol for every dropped d, tol computed as the library does,
      from a magnitude computed unfused, fused, or at random;
  (4) the kept coefficients of the checked path (every term of the first
      stage) and of the unchecked path (only the terms the second stage
      reads) have the same bits when both paths contract by the same
      pattern;
  and it counts how often they differ when the two paths contract by
  different patterns.

  <env>/bin/python -B sim.py [--cases N] [--seed S]

Writes sim.txt beside itself; exits 1 if (1), (2), (3), or (4) fails.
"""

import argparse
import os
import random
import sys

from flint import fmpq

HERE = os.path.dirname(os.path.abspath(__file__))
SPIKE = os.path.join(HERE, "..", "..", "spikes", "spike-e")
sys.path.insert(0, SPIKE)

import plans as P   # noqa: E402
import rigid3 as R  # noqa: E402

FORMATS = {
    # precision p, exponent of the smallest normal number
    "double": (53, -1022),
    "float": (24, -126),
}
OBJECTS = [("point", R.POINT), ("line", R.LINE), ("plane", R.PLANE)]
PATTERNS = ["off", "current", "first", "random"]


def pow2(e):
    return fmpq(2) ** e if e >= 0 else fmpq(1, 2 ** (-e))


def floor_log2(a):
    """floor(log2 a) for a positive fmpq."""
    num, den = int(a.p), int(a.q)
    e = num.bit_length() - den.bit_length()
    if e >= 0:
        if num < (den << e):
            e -= 1
    else:
        if (num << -e) < den:
            e -= 1
    return e


def rnd(q, fmt):
    """q rounded to nearest, ties to even, in the format: precision p,
    gradual underflow below 2^emin. No overflow is possible here."""
    if q == 0:
        return fmpq(0)
    p, emin = FORMATS[fmt]
    a = abs(q)
    e = max(floor_log2(a), emin)
    quantum = pow2(e - p + 1)
    t = a / quantum
    n, d = int(t.p), int(t.q)
    k, r = divmod(n, d)
    if 2 * r > d or (2 * r == d and k % 2 == 1):
        k += 1
    v = quantum * k
    return v if q > 0 else -v


def gamma(k, u):
    return fmpq(k) * u / (1 - fmpq(k) * u)


# --- The kernels ---------------------------------------------------------

def kernels(layout):
    """(first_all, first_kept, second_kept, dropped, n_mid, n_dropped):
    term lists (a, b, out, c), outputs as positions."""
    first_kept, second, mid, dropped = P.sandwich_terms(R.MOTOR, layout)
    first_all = P.sandwich_terms.first_all
    masks = sorted({t[2] for t in dropped}, key=P.canonical_key)
    pos = {m: i for i, m in enumerate(masks)}
    dropped = [(a, b, pos[o], c) for a, b, o, c in dropped]
    for t in first_all + second + dropped:
        assert t[3] in (1, -1)  # rigid(3): no coefficient other than +/-1
    return first_all, first_kept, second, dropped, len(mid), len(masks)


def exact_kernel(terms, nout, x, y, absolute=False, tau=None):
    """The exact result (or, with absolute, the exact padded magnitude)."""
    out = [fmpq(0)] * nout
    for a, b, o, c in terms:
        if absolute:
            out[o] += abs(x[a]) * abs(y[b]) + tau
        else:
            out[o] += c * x[a] * y[b]
    return out


def run_kernel(terms, nout, x, y, kx, ky, fmt, pattern, rng):
    """The computed result under a contraction pattern, and K' per output.

    x, y: operand values (fmpq, already in the format); kx, ky: their
    counts. Each output holds its running value and count, and, after its
    first term, that term's operands, so that the second term can fuse the
    first product."""
    val = [fmpq(0)] * nout
    cnt = [0] * nout
    first_term = [None] * nout
    seen = [False] * nout
    for a, b, o, c in terms:
        xa, yb = x[a], y[b]
        kp = kx[a] + ky[b]  # count of the unrounded product's factors
        if not seen[o]:
            seen[o] = True
            val[o] = c * rnd(xa * yb, fmt)
            cnt[o] = kp + 1
            first_term[o] = (c, xa, yb, kp)
            continue
        choice = pattern
        if pattern == "random":
            options = ["off", "current"]
            if first_term[o] is not None:
                options.append("first")
            choice = rng.choice(options)
        if choice == "first" and first_term[o] is None:
            choice = "current"
        if choice == "off":
            p = rnd(xa * yb, fmt)
            val[o] = rnd(val[o] + c * p, fmt)
            cnt[o] = max(cnt[o], kp + 1) + 1
        elif choice == "current":
            val[o] = rnd(val[o] + c * xa * yb, fmt)
            cnt[o] = max(cnt[o], kp) + 1
        else:  # first: fma(c1 x1, y1, c fl(x y))
            c1, x1, y1, kp1 = first_term[o]
            p = rnd(xa * yb, fmt)
            val[o] = rnd(c1 * x1 * y1 + c * p, fmt)
            cnt[o] = max(kp1, kp + 1) + 1
        first_term[o] = None
    return val, cnt


def run_magnitude(terms, nout, x, y, fmt, tau, pattern, rng):
    """The computed padded magnitude: fl(fl(|x||y|) + tau) per term, or
    fma(|x|, |y|, tau) when fused, summed in the kernel's order."""
    out = [fmpq(0)] * nout
    seen = [False] * nout
    for a, b, o, _ in terms:
        fuse = pattern == "fused" or (pattern == "random" and rng.random() < 0.5)
        z = abs(x[a]) * abs(y[b])
        term = rnd(z + tau, fmt) if fuse else rnd(rnd(z, fmt) + tau, fmt)
        out[o] = term if not seen[o] else rnd(out[o] + term, fmt)
        seen[o] = True
    return out


def roundings(terms, nout, kx, ky):
    """The core's count (kernel_roundings), every coefficient +/-1."""
    out = [None] * nout
    for a, b, o, _ in terms:
        t = kx[a] + ky[b] + 1
        out[o] = t if out[o] is None else max(out[o], t) + 1
    return out


# --- Inputs --------------------------------------------------------------

def draw_value(rng, fmt, lo, hi, zero_rate):
    p, _ = FORMATS[fmt]
    if rng.random() < zero_rate:
        return fmpq(0)
    m = rng.randrange(2 ** (p - 1), 2 ** p)
    e = rng.randint(lo, hi) - p
    v = fmpq(m) * pow2(e)
    return v if rng.random() < 0.5 else -v


REGIMES = {
    # name: (exponent range of the coefficients, rate of zeros)
    "ordinary": ((-2, 2), 0.0),
    "wide": ((-24, 24), 0.1),
    "underflow": (None, 0.0),  # set per format below
}
UNDERFLOW = {"double": (-345, -337), "float": (-46, -40)}


def draw_case(rng, fmt, regime, n_obj):
    (lo, hi), zr = REGIMES[regime] if regime != "underflow" else \
        (UNDERFLOW[fmt], 0.0)
    q = [draw_value(rng, fmt, lo, hi, zr) for _ in range(8)]
    x = [draw_value(rng, fmt, lo, hi, zr) for _ in range(n_obj)]
    return q, x


# --- Main ----------------------------------------------------------------

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--cases", type=int, default=400)
    ap.add_argument("--seed", type=int, default=20261110)
    args = ap.parse_args()
    rng = random.Random(args.seed)
    lines = [f"sim.py --cases {args.cases} --seed {args.seed}", ""]
    ok = True
    for fmt in FORMATS:
        p, emin = FORMATS[fmt]
        u = pow2(-p)
        lam = pow2(emin)
        # the library's padding, and the smallest the derivation admits
        taus = {"lambda/u": lam / u, "lambda": lam}
        for name, layout in OBJECTS:
            f_all, f_kept, second, dropped, n_mid, n_drop = kernels(layout)
            n_obj = len(layout)
            k_mid = roundings(f_all, n_mid, [0] * 8, [0] * n_obj)
            k_kept = roundings(second, n_obj, k_mid, [0] * 8)
            k_drop = roundings(dropped, n_drop, k_mid, [0] * 8)
            for regime in REGIMES:
                st = {
                    "cases": 0, "coeffs": 0, "kfail": 0, "bfail": 0,
                    "tfail": 0, "same_fail": 0, "diff_pairs": 0,
                    "diff_coeffs": 0, "max_ratio": {}, "max_tol_ratio": {},
                    "max_dk": 0, "fused_seen": False,
                }
                for _ in range(args.cases):
                    q, x = draw_case(rng, fmt, regime, n_obj)
                    st["cases"] += 1
                    mid_e = exact_kernel(f_all, n_mid, q, x)
                    kept_e = exact_kernel(second, n_obj, mid_e, q)
                    drop_e = exact_kernel(dropped, n_drop, mid_e, q)
                    # (2) is tested against the smaller padding, lambda,
                    # which makes it the stronger statement
                    mid_p = exact_kernel(f_all, n_mid, q, x, True, lam)
                    kept_p = exact_kernel(second, n_obj, mid_p, q, True, lam)
                    drop_p = exact_kernel(dropped, n_drop, mid_p, q, True, lam)
                    assert all(d == 0 for d in drop_e)
                    kept_by = {}
                    for pat in PATTERNS:
                        # checked path: every first-stage term
                        seed = rng.random()
                        r1 = random.Random(seed)
                        mid, kmid = run_kernel(f_all, n_mid, q, x,
                                               [0] * 8, [0] * n_obj, fmt,
                                               pat, r1)
                        kept, kk = run_kernel(second, n_obj, mid, q, kmid,
                                              [0] * 8, fmt, pat, r1)
                        drop, kd = run_kernel(dropped, n_drop, mid, q, kmid,
                                              [0] * 8, fmt, pat, r1)
                        kept_by[pat] = kept
                        for vals, ks, ex, pm, kc in (
                                (kept, kk, kept_e, kept_p, k_kept),
                                (drop, kd, drop_e, drop_p, k_drop)):
                            for v, k1, e, pp, k0 in zip(vals, ks, ex, pm, kc):
                                st["coeffs"] += 1
                                if k1 > k0:
                                    st["kfail"] += 1
                                st["max_dk"] = max(st["max_dk"], k0 - k1)
                                err = abs(v - e)
                                bound = gamma(k1, u) * pp
                                if err > bound:
                                    st["bfail"] += 1
                                if bound > 0:
                                    rr = err / bound
                                    st["max_ratio"][pat] = max(
                                        st["max_ratio"].get(pat, 0.0),
                                        float(rr))
                        # unchecked path, same pattern: only the first-stage
                        # terms the kept second stage reads (random has no
                        # "same pattern" here and is skipped)
                        if pat != "random":
                            mid_u, kmu = run_kernel(f_kept, n_mid, q, x,
                                                    [0] * 8, [0] * n_obj,
                                                    fmt, pat, None)
                            kept_u, _ = run_kernel(second, n_obj, mid_u, q,
                                                   kmu, [0] * 8, fmt, pat,
                                                   None)
                            if kept_u != kept:
                                st["same_fail"] += 1
                        # the check, with three ways of computing P, for
                        # both paddings
                        for mpat, tname in [(m, t) for m in
                                            ("off", "fused", "random")
                                            for t in taus]:
                            tau = taus[tname]
                            mr = random.Random(rng.random())
                            mm = run_magnitude(f_all, n_mid,
                                               [abs(c) for c in q],
                                               [abs(c) for c in x], fmt, tau,
                                               mpat, mr)
                            md = run_magnitude(dropped, n_drop, mm,
                                               [abs(c) for c in q], fmt, tau,
                                               mpat, mr)
                            for j in range(n_drop):
                                k = k_drop[j] + 1
                                g = rnd(fmpq(k) * u / (1 - fmpq(k) * u), fmt)
                                tol = rnd(g * md[j], fmt)
                                if abs(drop[j]) > tol:
                                    st["tfail"] += 1
                                if tol > 0:
                                    key = (pat, mpat, tname)
                                    st["max_tol_ratio"][key] = max(
                                        st["max_tol_ratio"].get(key, 0.0),
                                        float(abs(drop[j]) / tol))
                    # kept bits under different patterns of the two paths
                    for pa in PATTERNS:
                        for pb in PATTERNS:
                            if pa < pb:
                                for va, vb in zip(kept_by[pa], kept_by[pb]):
                                    st["diff_pairs"] += 1
                                    st["diff_coeffs"] += va != vb
                bad = st["kfail"] or st["bfail"] or st["tfail"] or \
                    st["same_fail"]
                ok &= not bad
                mr = ", ".join(f"{k} {v:.3f}"
                               for k, v in st["max_ratio"].items())
                tr = {t: max([v for k, v in st["max_tol_ratio"].items()
                              if k[2] == t] or [0.0]) for t in taus}
                tr = ", ".join(f"tau {t} {v:.3g}" for t, v in tr.items())
                lines.append(
                    f"{'FAIL' if bad else 'PASS'} {fmt} {name} {regime}: "
                    f"{st['cases']} cases x {len(PATTERNS)} patterns, "
                    f"{st['coeffs']} coefficients; K'>K {st['kfail']}, "
                    f"bound exceeded {st['bfail']}, check fired "
                    f"{st['tfail']}, same-pattern paths differ "
                    f"{st['same_fail']}; largest K-K' {st['max_dk']}; "
                    f"largest |err|/(gamma_K' P): {mr}; largest |d|/tol "
                    f"{tr}; kept coefficients that differ between two "
                    f"patterns {st['diff_coeffs']} of {st['diff_pairs']}")
                print(lines[-1], flush=True)
    lines.append("")
    lines.append(f"overall "
                 f"{'PASS' if ok else 'FAIL'}")
    with open(os.path.join(HERE, "sim.txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
