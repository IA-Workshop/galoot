"""A transcription of check_change's row reduction and fraction-free
elimination (basis_change.hpp, reduced and eliminate) for a rotation of
R(3, 0, 0), in exact integers, noting each product and difference that leaves
-(2^63 - 1) to 2^63 - 1, as multiply_noting and add_noting do.

Run: python3 -I r12_elimination_sim.py
It prints, for the quaternion (79, 2, 1, 1), every entry of the elimination
that overflows, and for three families of quaternions the first that the
elimination refuses. Python's integers do not overflow, so the values after
an overflow are the exact ones, where the C++ code stops at that step.
"""
from math import gcd

MAX = 2**63 - 1


def fits(v):
    return -MAX <= v <= MAX


def rotation(w, x, y, z):
    """Integer rows M and denominators of P = M / N, N = w^2 + x^2 + y^2 + z^2."""
    n = w * w + x * x + y * y + z * z
    rows = [
        [w * w + x * x - y * y - z * z, 2 * (x * y - w * z), 2 * (x * z + w * y)],
        [2 * (x * y + w * z), w * w - x * x + y * y - z * z, 2 * (y * z - w * x)],
        [2 * (x * z - w * y), 2 * (y * z + w * x), w * w - x * x - y * y + z * z],
    ]
    return rows, [n, n, n]


def reduced(rows, denominators):
    """Each row and its denominator divided by their greatest common divisor."""
    out_rows, out_denominators = [], []
    for row, d in zip(rows, denominators):
        g = d
        for v in row:
            g = gcd(g, v)
        if g > 1:
            out_rows.append([v // g for v in row])  # exact division
            out_denominators.append(d // g)
        else:
            out_rows.append(list(row))
            out_denominators.append(d)
    return out_rows, out_denominators


def overflows(rows):
    """The entries (step, row, column, block, largest product) whose update
    leaves the range, in the Gauss-Jordan form of Bareiss's method that
    eliminate uses on [M | I]."""
    n = len(rows)
    a = [list(rows[i]) + [1 if j == i else 0 for j in range(n)] for i in range(n)]
    previous = 1
    found = []
    for k in range(n):
        r = k
        while r < n and a[r][k] == 0:
            r += 1
        if r == n:
            return "dependent"
        a[r], a[k] = a[k], a[r]
        for i in range(n):
            if i == k:
                continue
            for j in range(2 * n):
                if j == k:
                    continue
                p1 = a[k][k] * a[i][j]
                p2 = a[i][k] * a[k][j]
                if not (fits(p1) and fits(p2) and fits(p1 - p2)):
                    block = "left" if j < n else "right"
                    found.append((k, i, j, block, max(abs(p1), abs(p2))))
                assert (p1 - p2) % previous == 0
                a[i][j] = (p1 - p2) // previous
            a[i][k] = 0
        previous = a[k][k]
    return found


if __name__ == "__main__":
    rows, dens = reduced(*rotation(79, 2, 1, 1))
    print("(79, 2, 1, 1): reduced rows", rows, "over", dens)
    for entry in overflows(rows):
        print("  overflow at step %d, row %d, column %d (%s block): %d" % entry)
    families = {
        "(w, w-1, w-2, w-3)": lambda w: (w, w - 1, w - 2, w - 3),
        "(w, 2, 1, 1)": lambda w: (w, 2, 1, 1),
        "(w, w/2, w/3, 1)": lambda w: (w, w // 2, w // 3, 1),
    }
    for name, family in families.items():
        for w in range(4, 5000):
            q = family(w)
            rows, dens = reduced(*rotation(*q))
            if overflows(rows):
                print(name, "first refused:", q, "N =", sum(v * v for v in q),
                      "largest reduced entry",
                      max(abs(v) for row in rows for v in row))
                break
