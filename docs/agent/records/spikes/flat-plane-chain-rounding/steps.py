"""The defect of a composed chain, step by step, in exact arithmetic.

Reads the dump of `chains steps <seed> <trials>`. For every stored partial
product V_k (k + 1 motors) it computes W = V̰ ⟇ V exactly (cga.py), with λ
its antiscalar and the defect the rest.

Flat-plane chains: W = λ𝟙 + 2h e5, and τ = 2h/λ is additive under exact
composition (τ(m ⟇ V) = τ(m) + τ(V), shown symbolically by identity.py), so
τ(V_k) = Σ_i τ(m_i) + Σ_i ℓ_i exactly, where m_i is the stored motor of step
i, whose own defect comes from composing two exact planes in double, and
ℓ_i = τ(V_i) − τ(V_(i−1)) − τ(m_i) is what rounding the composition of step
i adds. The check passes exactly when |2h| <= about γ_10 T, with
T = 2(|a1 b3| + |a2 b2| + |a3 b1| + |q s|) (identity.py), so the table also
gives T / λ.

Hyperplane chains: W = λ𝟙 + w with w a vector; the table gives |w| / |V|²,
λ / |V|² and the check's dense magnitude relative to |V|².

A second part continues 20 trials of each family from their stored V_63 by
64 exact compositions with the exact versors of the stored planes, so that
no new defect enters, and prints how the relative defect of V_63 evolves.

Usage: python3 -I steps.py <steps.jsonl>
"""
import json
import math
import statistics
import sys
from fractions import Fraction as Fr

sys.path.insert(0, __import__("os").path.dirname(__file__))
import cga  # noqa: E402

U = 2.0 ** -53
ANTI = 31


def fr(xs):
    return [Fr(float.fromhex(t)) for t in xs]


def flat_parts(v):
    g = lambda m: v.get(m, Fr(0))
    q, a1, a2, a3, b1, b2, b3, s = (g(m) for m in (16, 19, 21, 22, 25, 26, 28, 31))
    h = -a1 * b3 + a2 * b2 - a3 * b1 + q * s
    T = 2 * (abs(a1 * b3) + abs(a2 * b2) + abs(a3 * b1) + abs(q * s))
    lam = b1 * b1 + b2 * b2 + b3 * b3 + s * s
    A = a1 * a1 + a2 * a2 + a3 * a3 + q * q
    return h, T, lam, A


def defect(v):
    w = cga.antigp(cga.antireverse(v), v)
    lam = w.get(ANTI, Fr(0))
    rest = {k: c for k, c in w.items() if k != ANTI}
    return lam, rest


def norm2(v):
    return sum(c * c for c in v.values())


def rms(xs):
    return math.sqrt(sum(x * x for x in xs) / len(xs))


def main(path):
    meta, motor_meta = {}, {}
    traj = {}
    with open(path) as f:
        for line in f:
            r = json.loads(line)
            if "meta" in r:
                meta[r["meta"]] = r
                continue
            if "meta_motor" in r:
                motor_meta[r["meta_motor"]] = r
                continue
            traj.setdefault((r["family"], r["trial"]), []).append(r)

    def lay(layout, xs):
        return cga.from_layout([b[0] for b in layout], [b[1] for b in layout], fr(xs))

    marks = [1, 3, 7, 15, 31, 63, 127]  # V_k holds k + 1 motors
    # Flat
    acc = {k: {"tau": [], "motors": [], "rounding": [], "T_over_lam": [], "ratio": [], "step": [], "AB": []} for k in marks}
    trials = sorted(t for (fam, t) in traj if fam == "flat")
    for t in trials:
        rows = traj[("flat", t)]
        tau_prev = None
        sum_m, sum_l = Fr(0), Fr(0)
        for k, r in enumerate(rows):
            v = lay(meta["flat"]["v"], r["q"])
            m = lay(motor_meta["flat"]["m"], r["m"])
            h, T, lam, A2 = flat_parts(v)
            hm, _, lam_m, _ = flat_parts(m)
            tau, tau_m = 2 * h / lam, 2 * hm / lam_m
            step_l = Fr(0)
            if k == 0:
                sum_m = tau_m
            else:
                sum_m += tau_m
                step_l = tau - tau_prev - tau_m
                sum_l += step_l
            assert tau == sum_m + sum_l
            tau_prev = tau
            if k in acc:
                a = acc[k]
                a["tau"].append(float(tau) / U); a["motors"].append(float(sum_m) / U)
                a["rounding"].append(float(sum_l) / U); a["T_over_lam"].append(float(T / lam) / U)
                a["ratio"].append(float(abs(2 * h) / T) / U if T else float("inf"))
                a["step"].append(float(step_l) / U)
                a["AB"].append(math.sqrt(float(A2 / lam)))
    print("flat-plane chains, %d trials: τ = 2h/λ in units of u (RMS over trials), its two sums, the last step's ℓ_k, |A|/|B| = sqrt((a1² + a2² + a3² + q²)/λ), and T/λ" % len(trials))
    print("motors\tRMS τ\tRMS Σ τ(m_i)\tRMS Σ ℓ_i\tRMS ℓ_k\tmedian |A|/|B|\tmedian T/λ\tmedian |2h|/(u T)\t99th pct |2h|/(u T)\tshare |2h| > γ_10 T")
    g10 = 10 * U / (1 - 10 * U)
    for k in marks:
        a = acc[k]
        xs = sorted(a["ratio"])
        print("%d\t%.3g\t%.3g\t%.3g\t%.3g\t%.3g\t%.3g\t%.3g\t%.3g\t%.3f" % (
            k + 1, rms(a["tau"]), rms(a["motors"]), rms(a["rounding"]), rms(a["step"]), statistics.median(a["AB"]),
            statistics.median(a["T_over_lam"]) * U, statistics.median(xs), xs[int(0.99 * len(xs))],
            sum(1 for x in xs if x * U > g10) / len(xs)))
    # Hyper
    print()
    trials_h = sorted(t for (fam, t) in traj if fam == "hyper")
    hacc = {k: {"w": [], "lam": [], "sign": []} for k in marks}
    for t in trials_h:
        rows = traj[("hyper", t)]
        sign_m = 1
        for k, r in enumerate(rows):
            lam_m, _ = defect(lay(motor_meta["hyper"]["m"], r["m"]))
            sign_m *= 1 if lam_m > 0 else -1
            if k not in hacc:
                continue
            v = lay(meta["hyper"]["v"], rows[k]["q"])
            lam, rest = defect(v)
            n2 = norm2(v)
            hacc[k]["w"].append(float(math.sqrt(norm2(rest)) / n2) / U)
            hacc[k]["lam"].append(abs(float(lam / n2)))
            hacc[k]["sign"].append((1 if lam > 0 else -1) == sign_m)
    print("hyperplane chains, %d trials: |w|/|V|² in u and |λ|/|V|² (medians)" % len(trials_h))
    print("motors\tmedian |w|/(u |V|²)\t99th pct\tmedian |λ|/|V|²\tmedian log2 |λ|/|V|²\tshare where λ has the sign of the product of the motors' λ")
    for k in marks:
        w = sorted(hacc[k]["w"]); l = sorted(hacc[k]["lam"])
        print("%d\t%.3g\t%.3g\t%.3g\t%.1f\t%.2f" % (k + 1, statistics.median(w), w[int(0.99 * len(w))],
                                            statistics.median(l), statistics.median(math.log2(x) if x else -1e9 for x in l),
                                            sum(hacc[k]["sign"]) / len(hacc[k]["sign"])))
    # Exact continuation from V_63
    print()
    print("exact continuation of 20 trials from V_63 by the exact versors of the next 64 steps' planes: the relative defect of V_63 carried forward")
    print("family\tsteps\tmedian ratio of relative defect to that of V_63")
    for fam in ("flat", "hyper"):
        pl = motor_meta[fam]["p"]
        ratios = {j: [] for j in (1, 4, 16, 64)}
        for t in sorted(t for (f2, t) in traj if f2 == fam)[:20]:
            rows = traj[(fam, t)]
            v = lay(meta[fam]["v"], rows[63]["q"])

            def rel(v):
                lam, rest = defect(v)
                if fam == "flat":
                    h, T, lam2, A = flat_parts(v)
                    return abs(2 * h) / T  # what the check compares, up to γ_10
                return math.sqrt(norm2(rest)) / norm2(v)
            r0 = rel(v)
            for j in range(1, 65):
                a = lay(pl, rows[63 + j]["a"]); b = lay(pl, rows[63 + j]["b"])
                m = cga.antigp(a, b)
                v = cga.antigp(m, v)
                # rescale exactly by a power of two to keep sizes moderate
                big = max(abs(c) for c in v.values())
                e = math.frexp(float(big))[1]
                v = {k2: c / Fr(2) ** e if e > 0 else c * Fr(2) ** (-e) for k2, c in v.items()}
                if j in ratios:
                    ratios[j].append(float(rel(v) / r0))
        for j in sorted(ratios):
            print("%s\t%d\t%.3g" % (fam, j, statistics.median(ratios[j])))


if __name__ == "__main__":
    main(sys.argv[1])
