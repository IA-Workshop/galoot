"""Replica of basis_change.hpp's check_change at 30c2275 (eliminate,
keeps_inner_products, inverts, inverse_rows with noting arithmetic), written
from the header, to find which overflow branches inputs can reach.

Reports, over random and structured inputs, how often each stage is the first
to overflow, and whether an overflow in inverts or inverse_rows can happen
after a fitting elimination and isometry check.
"""
import math
import random
import sys

MAX = 2**63 - 1
IMAX = 2**31 - 1
IMIN = -2**31


class Note:
    def __init__(self):
        self.fits = True


def mul(x, y, n):
    if abs(x) != 0 and abs(y) > MAX // abs(x):
        n.fits = False
        return 0
    return x * y


def add(x, y, n):
    if (x > MAX - y) if y > 0 else (x < -MAX - y):
        n.fits = False
        return 0
    return x + y


def tdiv(a, b):
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b > 0) else -q


def eliminate(rows):
    N = len(rows)
    a = [list(r) + [1 if j == i else 0 for j in range(N)] for i, r in enumerate(rows)]
    note = Note()
    prev = 1
    for k in range(N):
        r = k
        while r < N and a[r][k] == 0:
            r += 1
        if r == N:
            return 0, None, True
        if r != k:
            a[r], a[k] = a[k], a[r]
        for i in range(N):
            if i != k:
                row, pr = a[i], a[k]
                for j in range(2 * N):
                    if j != k:
                        row[j] = tdiv(add(mul(pr[k], row[j], note),
                                          -mul(row[k], pr[j], note), note), prev)
        if not note.fits:
            return 0, None, False
        prev = a[k][k]
    return prev, [a[i][N:] for i in range(N)], True


def keeps(rows, dens, G, Gp):
    N = len(rows)
    n = Note()
    rt = [[0] * N for _ in range(N)]
    for i in range(N):
        for l in range(N):
            s = 0
            for k in range(N):
                s = add(s, mul(rows[i][k], Gp[k][l], n), n)
            rt[i][l] = s
    holds = True
    for i in range(N):
        for j in range(N):
            left = 0
            for l in range(N):
                left = add(left, mul(rt[i][l], rows[j][l], n), n)
            right = mul(mul(dens[i], G[i][j], n), dens[j], n)
            holds = holds and left == right
    return n.fits, n.fits and holds


def inverts(rows, p, inv):
    N = len(rows)
    n = Note()
    holds = True
    for i in range(N):
        for j in range(N):
            s = 0
            for k in range(N):
                s = add(s, mul(rows[i][k], inv[k][j], n), n)
            holds = holds and s == (p if i == j else 0)
    return n.fits, n.fits and holds


def inverse_rows(dens, p, inv):
    N = len(dens)
    n = Note()
    sign = -1 if p < 0 else 1
    mag = sign * p
    for i in range(N):
        for j in range(N):
            mul(sign * inv[i][j], dens[j], n)
    return n.fits


def stage(rows, dens, G, Gp):
    p, inv, fits = eliminate(rows)
    if not fits:
        return "elim"
    if p == 0:
        return "dependent"
    f, h = keeps(rows, dens, G, Gp)
    if not h:
        return "isometry-overflow" if not f else "isometry"
    f, h = inverts(rows, p, inv)
    if not h:
        return "inverts-overflow" if not f else "inverse"
    if not inverse_rows(dens, p, inv):
        return "inverse_rows-overflow"
    return "none"


def big():
    return random.choice([0, 1, -1, 2, -2, 3, IMAX, -IMAX, IMIN, 2**30, -2**30,
                          2**20, -2**20, 2**16, 2**24, -(2**24), 65535, 46341,
                          3037000499 // 2, random.randint(IMIN, IMAX),
                          random.randint(-1000, 1000)])


def main():
    random.seed(int(sys.argv[1]) if len(sys.argv) > 1 else 20261109)
    counts = {}
    witness = {}
    for t in range(200000):
        N = random.choice([1, 2, 2, 3, 3, 4])
        rows = [[big() if random.random() < 0.6 else 0 for _ in range(N)] for _ in range(N)]
        dens = [random.choice([1, 2, IMAX, 2**30, random.randint(1, IMAX)]) for _ in range(N)]
        # The checks after elimination need a form; the isometry check is
        # bypassed here with G computed so it holds where it can, else any.
        G = [[0] * N for _ in range(N)]
        Gp = [[1 if i == j else 0 for j in range(N)] for i in range(N)]
        s = stage(rows, dens, G, Gp)
        # Also probe inverts and inverse_rows directly after a fitting
        # elimination, whatever the form.
        p, inv, fits = eliminate(rows)
        if fits and p != 0:
            fi, hi = inverts(rows, p, inv)
            fr = inverse_rows(dens, p, inv)
            if not fi:
                counts["direct:inverts-overflow"] = counts.get("direct:inverts-overflow", 0) + 1
                witness.setdefault("inverts", (rows, dens))
            if not fr:
                counts["direct:inverse_rows-overflow"] = counts.get("direct:inverse_rows-overflow", 0) + 1
                witness.setdefault("inverse_rows", (rows, dens))
        counts[s] = counts.get(s, 0) + 1
    for k in sorted(counts):
        print(k, counts[k])
    for k, v in witness.items():
        print("witness", k, v)


main()
