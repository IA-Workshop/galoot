"""Replica of check_change in basis_change.hpp, written from the header,
with and without the reduction of each row by the gcd of its entries and
its denominator. Prints, for each input of the overflow tests, the stage
that rejects it (or none) both ways, and the reduced rows."""
import math
import sys

MAX = 2**63 - 1


class Note:
    def __init__(self):
        self.fits = True


def mul(x, y, n):
    if x != 0 and abs(y) > MAX // abs(x):
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
            return 0, None, note.fits
        if r != k:
            a[r], a[k] = a[k], a[r]
        for i in range(N):
            if i != k:
                row, pr = a[i], a[k]
                for j in range(2 * N):
                    if j != k:
                        row[j] = tdiv(add(mul(pr[k], row[j], note),
                                          -mul(row[k], pr[j], note), note), prev)
                row[k] = 0
        if not note.fits:
            return 0, None, False
        prev = a[k][k]
    return prev, [a[i][N:] for i in range(N)], note.fits


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
    out = []
    for i in range(N):
        row = [mul(sign * inv[i][j], dens[j], n) for j in range(N)]
        common = sign * p
        for x in row:
            common = math.gcd(common, x)
        out.append(([x // common for x in row], sign * p // common))
    return n.fits, out


def reduced(rows, dens):
    out_r, out_d = [], []
    for r, d in zip(rows, dens):
        g = d
        for x in r:
            g = math.gcd(g, x)
        if g > 1:
            out_r.append([tdiv(x, g) for x in r])
            out_d.append(d // g)
        else:
            out_r.append(list(r))
            out_d.append(d)
    return out_r, out_d


def stage(rows, dens, G, Gp, reduce):
    if any(d <= 0 for d in dens):
        return "denominator", None
    if reduce:
        rows, dens = reduced(rows, dens)
    p, inv, fits = eliminate(rows)
    if not fits:
        return "overflow (elimination)", None
    if p == 0:
        return "dependent", None
    f, h = keeps(rows, dens, G, Gp)
    if not h:
        return ("isometry" if f else "overflow (inner products)"), None
    f, h = inverts(rows, p, inv)
    if not h:
        return ("inverse" if f else "overflow (inverse check)"), None
    f, inverse = inverse_rows(dens, p, inv)
    if not f:
        return "overflow (inverse rows)", None
    return "none", (rows, dens, inverse)


def zeros(n):
    return [[0] * n for _ in range(n)]


def eye(n, k=1):
    return [[k if i == j else 0 for j in range(n)] for i in range(n)]


m = 2**31 - 1
k = math.isqrt((2**31 - 1) // 2)
cases = {
    # Inputs of the overflow tests at the branch point.
    "old overflow: k e1 against itself, m e'1 over m":
        ([[m]], [m], [[k]], [[k]]),
    "old overflow_elimination: R(3,0,0), m e'_i over m":
        (eye(3, m), [m] * 3, eye(3), eye(3)),
    "overflow_inverse_check: R(0,0,3)":
        ([[65536, 0, 0], [1, -2, 0], [0, 1073741824, 1048576]], [2124569019, 2, 2], zeros(3), zeros(3)),
    "overflow_inverse_rows: R(0,0,3)":
        ([[0, 1048576, 0], [0, -16777216, 46341], [2, -378, 0]], [1, 1073741824, 2147483647], zeros(3), zeros(3)),
    # The delta review's probe: 2I against R(2,0,0).
    "probe: 2I to R(2,0,0), rows m(1,1), m(-1,1) over m":
        ([[m, m], [-m, m]], [m, m], eye(2, 2), eye(2)),
    # Replacement inputs, reduced, that still overflow.
    "new overflow: diag(k, 0) against itself, e1 = e'1 + e'2 / m":
        ([[m, 1], [0, 1]], [m, 1], [[k, 0], [0, 0]], [[k, 0], [0, 0]]),
    "new overflow_elimination: R(0,0,3), m e'_i over 1":
        (eye(3, m), [1] * 3, zeros(3), zeros(3)),
}
for name, (r, d, G, Gp) in cases.items():
    print(name)
    for reduce in (False, True):
        s, data = stage(r, d, G, Gp, reduce)
        print("  reduced" if reduce else "  as written", "->", s)
        if data:
            print("    rows", data[0], "dens", data[1], "inverse", data[2])
    print("  reduced rows:", reduced(r, d))
