"""The defect left by the flat-motor projection of chains.cpp's renormalize(),
in exact arithmetic.

Python floats are IEEE 754 doubles that round to nearest, and every
operation below is the one renormalize() performs, in its order, without
fused multiply-adds, so the floats here are the probe's. For every flat
chain of `chains chains <seed>`, before and after the projection, it prints
the exact |2h| / (u T), the quantity the check compares with about γ_10
(identity.py), and the largest relative change of a coefficient.

Usage: python3 -I renorm_check.py <chains.jsonl>
"""
import json
import sys
from fractions import Fraction as Fr

U = Fr(1, 2 ** 53)


def renormalize(c):
    q, a1, a2, b1, a3, b2, b3, s = c
    h = (((-a1 * b3) + (a2 * b2)) - (a3 * b1)) + (q * s)
    lam = (((b1 * b1) + (b2 * b2)) + (b3 * b3)) + (s * s)
    alpha = -h / lam
    return [q + (alpha * s), a1 - (alpha * b3), a2 + (alpha * b2), b1, a3 - (alpha * b1), b2, b3, s]


def parts(c):
    q, a1, a2, b1, a3, b2, b3, s = (Fr(x) for x in c)
    h2 = 2 * (-a1 * b3 + a2 * b2 - a3 * b1 + q * s)
    T = 2 * (abs(a1 * b3) + abs(a2 * b2) + abs(a3 * b1) + abs(q * s))
    return h2, T


def main(path):
    by_n = {}
    for line in open(path):
        r = json.loads(line)
        if r.get("family") != "flat":
            continue
        v = [float.fromhex(t) for t in r["v"]]
        w = renormalize(v)
        h0, t0 = parts(v)
        h1, t1 = parts(w)
        change = max(abs(Fr(b) - Fr(a)) / abs(Fr(a)) for a, b in zip(v, w) if a != 0)
        big = max(abs(a) for a in v)
        change_norm = max(abs(Fr(b) - Fr(a)) for a, b in zip(v, w)) / Fr(big)
        by_n.setdefault(r["N"], []).append((abs(h0) / t0 / U, abs(h1) / t1 / U, change / U, change_norm / U))
    print("N\tcases\tmax |2h|/(uT) before\tmax after\tmedian after\tmax relative change of a coefficient (u)\tmax change / max |coefficient| (u)")
    for n in sorted(by_n):
        rs = by_n[n]
        after = sorted(float(r[1]) for r in rs)
        print("%d\t%d\t%.3g\t%.3g\t%.3g\t%.3g\t%.3g" % (n, len(rs), max(float(r[0]) for r in rs), after[-1],
                                                     after[len(after) // 2], max(float(r[2]) for r in rs),
                                                     max(float(r[3]) for r in rs)))


if __name__ == "__main__":
    main(sys.argv[1])
