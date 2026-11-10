#!/usr/bin/env python3
"""A second, independent transcription of the checks and the term generation
of include/galoot/core/basis_change.hpp, written by the reviewer from the
header, to test two claims of the change:

  (a) every value the checks and the generation form from the rows as
      written is the value formed at the same place from the reduced rows
      times a positive integer (or both are zero), with the same pivot swaps;
  (b) the generated map of the full layout, both ways, is the same.

It also reports, for given inputs, the first stage whose values leave the
range [-max, max] of std::int64_t, as written and reduced.

Usage: python3 -I scaling_probe.py [seed count]
"""

import itertools
import math
import random
import sys

MAX = 2**63 - 1


class Trace:
    def __init__(self):
        self.values = []  # (stage, value)
        self.swaps = []

    def note(self, stage, v):
        self.values.append((stage, v))
        return v


def eliminate(M, t):
    n = len(M)
    a = [list(M[i]) + [1 if j == i else 0 for j in range(n)] for i in range(n)]
    previous = 1
    for k in range(n):
        row = k
        while row < n and a[row][k] == 0:
            row += 1
        if row == n:
            t.swaps.append(("singular", k))
            return 0, None
        if row != k:
            a[row], a[k] = a[k], a[row]
            t.swaps.append((row, k))
        for i in range(n):
            if i == k:
                continue
            r = a[i]
            p = a[k]
            for j in range(2 * n):
                if j == k:
                    continue
                x = t.note("elim", p[k] * r[j])
                y = t.note("elim", r[k] * p[j])
                s = t.note("elim", x - y)
                assert s % previous == 0
                r[j] = s // previous
            r[k] = 0
        previous = a[k][k]
    inv = [[a[i][n + j] for j in range(n)] for i in range(n)]
    return previous, inv


def keeps(M, d, G, Gp, t):
    n = len(M)
    RT = [[0] * n for _ in range(n)]
    for i in range(n):
        for l in range(n):
            s = 0
            for k in range(n):
                s = t.note("iso", s + t.note("iso", M[i][k] * Gp[k][l]))
            RT[i][l] = s
    holds = True
    for i in range(n):
        for j in range(n):
            left = 0
            for l in range(n):
                left = t.note("iso", left + t.note("iso", RT[i][l] * M[j][l]))
            right = t.note("iso", t.note("iso", d[i] * G[i][j]) * d[j])
            holds = holds and left == right
    return holds


def inverts(M, p, inv, t):
    n = len(M)
    holds = True
    for i in range(n):
        for j in range(n):
            s = 0
            for k in range(n):
                s = t.note("invcheck", s + t.note("invcheck", M[i][k] * inv[k][j]))
            holds = holds and s == (p if i == j else 0)
    return holds


def inverse_rows(d, p, inv, t):
    n = len(d)
    sign = -1 if p < 0 else 1
    mag = sign * p
    rows, dens = [], []
    for i in range(n):
        common = mag
        r = []
        for j in range(n):
            v = t.note("invrows", sign * inv[i][j] * d[j])
            r.append(v)
            common = math.gcd(common, v)
        rows.append([v // common for v in r])
        dens.append(mag // common)
    return rows, dens


def wedge_sign(blade, j):
    if blade >> j & 1:
        return 0
    above = bin(blade >> (j + 1)).count("1")
    return -1 if above % 2 else 1


def blade_image(rows, a, t):
    n = len(rows)
    image = {0: 1}
    rest = a
    while rest:
        i = (rest & -rest).bit_length() - 1
        rest &= rest - 1
        nxt = {}
        for blade, c in image.items():
            for j in range(n):
                if rows[i][j] == 0:
                    continue
                s = wedge_sign(blade, j)
                if s == 0:
                    continue
                v = t.note("gen", t.note("gen", c * rows[i][j]) * s)
                b = blade | 1 << j
                nxt[b] = t.note("gen", nxt.get(b, 0) + v)
        image = {b: c for b, c in sorted(nxt.items()) if c != 0}
    return image


def change_map(rows, dens, t):
    n = len(rows)
    blades = sorted(range(1 << n), key=lambda m: (bin(m).count("1"), m))
    terms = []  # (a, out blade, coefficient)
    for a in blades:
        for b, c in blade_image(rows, a, t).items():
            terms.append((a, b, c))
    own = []
    q = {}
    for a, b, c in terms:
        o = 1
        for i in range(n):
            if a >> i & 1:
                o = t.note("gen", o * dens[i])
        own.append(o)
        qb = q.get(b, 1)
        q[b] = t.note("gen", qb // math.gcd(qb, o) * o)
    coeffs = []
    for (a, b, c), o in zip(terms, own):
        coeffs.append(t.note("gen", c * (q[b] // o)))
    common = dict(q)
    for (a, b, c), k in zip(terms, coeffs):
        common[b] = math.gcd(common[b], k)
    out = [(a, b, k // common[b]) for (a, b, c), k in zip(terms, coeffs)]
    qs = {b: q[b] // common[b] for b in q}
    return out, qs


def reduced(M, d):
    rows, dens = [], []
    for r, di in zip(M, d):
        g = di
        for x in r:
            g = math.gcd(g, x)
        if g > 1:
            rows.append([x // g for x in r])
            dens.append(di // g)
        else:
            rows.append(list(r))
            dens.append(di)
    return rows, dens


def run(M, d, G, Gp):
    """Every stage, regardless of verdicts, with exact integers."""
    t = Trace()
    p, inv = eliminate(M, t)
    result = {"pivot": p, "trace": t}
    if p == 0:
        return result
    result["iso"] = keeps(M, d, G, Gp, t)
    result["inverts"] = inverts(M, p, inv, t)
    irows, idens = inverse_rows(d, p, inv, t)
    result["forward"] = change_map(M, d, t)
    result["back"] = change_map(irows, idens, t)
    return result


def first_overflow(M, d, G, Gp):
    """The verdict of check_change: the first stage that overflows, or the
    first failing check, or none."""
    t = Trace()
    p, inv = eliminate(M, t)
    if any(abs(v) > MAX for _, v in t.values):
        return "overflow (elimination)"
    if p == 0:
        return "dependent"
    t = Trace()
    if not keeps(M, d, G, Gp, t) or any(abs(v) > MAX for _, v in t.values):
        return ("overflow (inner products)"
                if any(abs(v) > MAX for _, v in t.values) else "isometry")
    t = Trace()
    ok = inverts(M, p, inv, t)
    if any(abs(v) > MAX for _, v in t.values):
        return "overflow (inverse check)"
    if not ok:
        return "inverse"
    t = Trace()
    inverse_rows(d, p, inv, t)
    if any(abs(v) > MAX for _, v in t.values):
        return "overflow (inverse rows)"
    return "none"


def compare(M, d, G, Gp):
    R, e = reduced(M, d)
    w = run(M, d, G, Gp)
    r = run(R, e, G, Gp)
    tw, tr = w["trace"], r["trace"]
    assert tw.swaps == tr.swaps, (tw.swaps, tr.swaps)
    assert len(tw.values) == len(tr.values)
    for (sw, vw), (sr, vr) in zip(tw.values, tr.values):
        assert sw == sr
        if vr == 0:
            assert vw == 0, (sw, vw, vr)
        else:
            assert vw % vr == 0 and vw // vr > 0, (sw, vw, vr)
    assert (w["pivot"] == 0) == (r["pivot"] == 0)
    if w["pivot"] != 0:
        assert w["iso"] == r["iso"] and w["inverts"] == r["inverts"]
        assert w["forward"] == r["forward"], "forward map differs"
        assert w["back"] == r["back"], "back map differs"
    return len(tw.values)


def random_case(rng):
    n = rng.randint(1, 4)
    while True:
        Mr = [[rng.choice([0, 0, rng.randint(-9, 9)]) for _ in range(n)]
              for _ in range(n)]
        dr = [rng.randint(1, 12) for _ in range(n)]
        Mr, dr = reduced(Mr, dr)
        if any(any(r) for r in Mr):
            break
    g = [rng.choice([1, 1, 2, 3, 6, rng.randint(1, 10**6)]) for _ in range(n)]
    M = [[gi * x for x in r] for gi, r in zip(g, Mr)]
    d = [gi * di for gi, di in zip(g, dr)]
    Gp = [[0] * n for _ in range(n)]
    for i in range(n):
        for j in range(i, n):
            Gp[i][j] = Gp[j][i] = rng.randint(-3, 3)
    # G an arbitrary symmetric integer matrix: the scaling claim is about
    # every value formed, whether or not the change is an isometry; on a
    # third of the cases G = P G' P^T scaled to integers is not needed.
    G = [[rng.randint(-3, 3) for _ in range(n)] for _ in range(n)]
    return M, d, G, Gp


def main():
    seed = int(sys.argv[1]) if len(sys.argv) > 1 else 1
    count = int(sys.argv[2]) if len(sys.argv) > 2 else 2000
    rng = random.Random(seed)
    checked = values = singular = 0
    for _ in range(count):
        M, d, G, Gp = random_case(rng)
        values += compare(M, d, G, Gp)
        checked += 1
    print(f"seed {seed}: {checked} random cases, {values} intermediate "
          f"values compared, claims (a) and (b) held on all")

    m = 2**31 - 1
    k = 32767
    cases = {
        "old overflow (k e1, m e'1 / m)": ([[m]], [m], [[k]], [[k]]),
        "old overflow_elimination (R(3,0,0), m e'_i / m)": (
            [[m, 0, 0], [0, m, 0], [0, 0, m]], [m, m, m],
            [[1, 0, 0], [0, 1, 0], [0, 0, 1]], [[1, 0, 0], [0, 1, 0], [0, 0, 1]]),
        "probe 2I to R(2,0,0), m(1,1), m(-1,1) / m": (
            [[m, m], [-m, m]], [m, m], [[2, 0], [0, 2]], [[1, 0], [0, 1]]),
        "new overflow (diag(k,0), e1 = e'1 + e'2/m)": (
            [[m, 1], [0, 1]], [m, 1], [[k, 0], [0, 0]], [[k, 0], [0, 0]]),
        "new overflow_elimination (R(0,0,3), m e'_i)": (
            [[m, 0, 0], [0, m, 0], [0, 0, m]], [1, 1, 1],
            [[0] * 3] * 3, [[0] * 3] * 3),
        "overflow_inverse_check": (
            [[65536, 0, 0], [1, -2, 0], [0, 1073741824, 1048576]],
            [2124569019, 2, 2], [[0] * 3] * 3, [[0] * 3] * 3),
        "overflow_inverse_rows": (
            [[0, 1048576, 0], [0, -16777216, 46341], [2, -378, 0]],
            [1, 1073741824, 2147483647], [[0] * 3] * 3, [[0] * 3] * 3),
    }
    for name, (M, d, G, Gp) in cases.items():
        R, e = reduced(M, d)
        print(f"{name}:\n  as written: {first_overflow(M, d, G, Gp)}"
              f"\n  reduced:    {first_overflow(R, e, G, Gp)}"
              f"  (rows {R} over {e})")
        compare(M, d, G, Gp)
    print("the scaling claims also held on each named input")


if __name__ == "__main__":
    main()
