"""Exact recomputation of every case that chains.cpp dumps.

For each case, with V and x the doubles as stored (exact dyadic rationals):
- e, the exact dropped part of V ⟇ x ⟇ V̰, by the independent algebra of
  cga.py, against d, the library's computed dropped part, and tol, its
  tolerance: |e| > tol is drift of V from the versors, |d - e| > tol would
  be a defect of the bound;
- W = V̰ ⟇ V exactly, its antiscalar λ and its defect, the rest of W, and
  the identity e_𝟙 = <x ⟇ W>_𝟙 (the antiscalar part), so that the dropped
  antiscalar is linear in the defect;
- M, the unpadded magnitude of the dropped antiscalar from the kernels'
  terms, rebuilt here from the expansion of the sandwich in V's and x's
  coefficients.

Usage: python3 -I analyze.py <chains.jsonl> <out.tsv>
Prints a summary by family and N; writes one row per case.
"""
import json
import sys
from fractions import Fraction as Fr

sys.path.insert(0, __import__("os").path.dirname(__file__))
import cga  # noqa: E402

U = Fr(1, 2 ** 53)
ANTISCALAR = 31


def hexes(xs):
    return [Fr(float.fromhex(t)) for t in xs]


def main(path, out_path):
    meta = {}
    rows = []
    tol_vs_T = []
    with open(path) as f:
        for line in f:
            r = json.loads(line)
            if "meta" in r:
                meta[r["meta"]] = r
                continue
            m = meta[r["family"]]
            vm = [b[0] for b in m["v"]]; vs = [b[1] for b in m["v"]]
            xm = [b[0] for b in m["x"]]; xs = [b[1] for b in m["x"]]
            v = cga.from_layout(vm, vs, hexes(r["v"]))
            x = cga.from_layout(xm, xs, hexes(r["x"]))
            d = hexes(r["d"]); tol = hexes(r["tol"])
            s = cga.sandwich_even(v, x)
            e = [s.get(mask, Fr(0)) for mask in m["dropped"]]
            w = cga.antigp(cga.antireverse(v), v)
            lam = w.get(ANTISCALAR, Fr(0))
            defect = {k: c for k, c in w.items() if k != ANTISCALAR}
            ident = cga.antigp(x, w).get(ANTISCALAR, Fr(0))
            j = m["dropped"].index(ANTISCALAR)
            assert ident == e[j], "e_antiscalar != <x W>_antiscalar"
            if r["family"] == "flat":
                g = lambda mask: v.get(mask, Fr(0))
                q, a1, a2, a3, b1, b2, b3, sc = (g(mask) for mask in (16, 19, 21, 22, 25, 26, 28, 31))
                h2 = 2 * (-a1 * b3 + a2 * b2 - a3 * b1 + q * sc)
                T = 2 * (abs(a1 * b3) + abs(a2 * b2) + abs(a3 * b1) + abs(q * sc))
                x4 = x.get(8, Fr(0))
                assert e[j] == h2 * x4, "e_antiscalar != 2h x4"
                assert all(e[i] == 0 for i in range(len(e)) if i != j), "a grade-3 dropped part is not zero"
                g10 = 10 * U / (1 - 10 * U)
                if T and x4:
                    tol_vs_T.append(tol[j] / (g10 * abs(x4) * T))
                AB = 2 * (float(a1 * a1 + a2 * a2 + a3 * a3 + q * q) * float(b1 * b1 + b2 * b2 + b3 * b3 + sc * sc)) ** 0.5
                align = float(T) / AB
            else:
                align = float("nan")
            drift = max(abs(a) / t for a, t in zip(e, tol))
            rnd = max(abs(dd - a) / t for dd, a, t in zip(d, e, tol))
            fired = max(abs(dd) / t for dd, t in zip(d, tol))
            # which dropped coefficient is largest relative to its tolerance
            jmax = max(range(len(d)), key=lambda i: abs(d[i]) / tol[i])
            vnorm2 = sum(c * c for c in v.values())
            dnorm = max((abs(c) for c in defect.values()), default=Fr(0))
            rows.append((r["family"], r["N"], r["trial"], float(fired), float(drift), float(rnd),
                         m["dropped"][jmax], float(lam), float(dnorm), float(dnorm / vnorm2 / U),
                         float(tol[j] / (vnorm2 * sum(abs(c) for c in x.values())) / U), align))
    with open(out_path, "w") as f:
        f.write("family\tN\ttrial\tfired_ratio\tdrift_ratio\trounding_ratio\tworst_blade\tlambda\tdefect\tdefect_over_v2_u\ttol_over_v2x1_u\talignment\n")
        for row in rows:
            f.write("\t".join(str(c) if not isinstance(c, float) else "%.6g" % c for c in row) + "\n")
    print("flat: the antiscalar's tolerance over γ_10 |x4| T, over %d cases: min %.17g, max %.17g" % (
        len(tol_vs_T), float(min(tol_vs_T)), float(max(tol_vs_T))))
    import collections
    g = collections.defaultdict(list)
    for row in rows:
        g[(row[0], row[1])].append(row)
    print("family\tN\tcases\tfired\tdrift>1\tmax |d|/tol\tmax |e|/tol\tmax |d-e|/tol\tworst blade when fired\tmedian defect/|V|^2 in u")
    for k in sorted(g):
        rs = g[k]
        fired = [r for r in rs if r[3] > 1]
        blades = sorted({cga.name(r[6]) for r in fired})
        med = sorted(r[9] for r in rs)[len(rs) // 2]
        print("%s\t%d\t%d\t%d\t%d\t%.3g\t%.3g\t%.3g\t%s\t%.3g" % (
            k[0], k[1], len(rs), len(fired), sum(r[4] > 1 for r in rs),
            max(r[3] for r in rs), max(r[4] for r in rs), max(r[5] for r in rs),
            ",".join(blades) or "-", med))
    def quant(xs, p):
        xs = sorted(xs)
        return xs[min(len(xs) - 1, int(p * len(xs)))]
    print()
    print("defect = largest |coefficient| of V̰ ⟇ V off 𝟙, over |V|² (sum of squares) in u; tol of the antiscalar over |V|² |x|_1 in u;")
    print("alignment (flat) = T / (2 |A| |B|), A = (a1, a2, a3, q), B = (b1, b2, b3, s)")
    print("family\tN\tdefect q50\tq99\ttol q01\tq50\talignment q50\talignment q50 when fired")
    for k in sorted(g):
        rs = g[k]
        fired = [r for r in rs if r[3] > 1]
        al = [r[11] for r in rs]
        print("%s\t%d\t%.3g\t%.3g\t%.3g\t%.3g\t%.3g\t%.3g" % (
            k[0], k[1], quant([r[9] for r in rs], .5), quant([r[9] for r in rs], .99),
            quant([r[10] for r in rs], .01), quant([r[10] for r in rs], .5),
            quant(al, .5) if k[0] == "flat" else float("nan"),
            quant([r[11] for r in fired], .5) if fired else float("nan")))
    print()
    edge = [r for r in rows if (r[3] > 1) != (r[4] > 1)]
    fired_all = [r for r in rows if r[3] > 1]
    print("cases where firing (|d| > tol) and drift (|e| > tol) disagree: %d, |e|/tol in [%.3g, %.3g], largest |d-e|/tol %.3g" % (
        len(edge), min(r[4] for r in edge), max(r[4] for r in edge), max(r[5] for r in edge)))
    print("cases that fire: %d, smallest |e|/tol among them %.3g" % (len(fired_all), min(r[4] for r in fired_all)))


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
