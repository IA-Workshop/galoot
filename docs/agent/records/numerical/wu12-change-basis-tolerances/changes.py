"""Per-output data of the changes of basis that criterion 8's floating tests use.

For each change P (row i writes e_i in the target generators) and each output
blade B, the coefficient of B is sum over A of det P[A, B] x_A, with A and B of
one grade. Written as integers over one positive denominator per output blade,
the least common multiple of the reduced denominators of its minors, it is

    y_B = (sum_A c_AB x_A) / q_B.

The script prints, per change and direction: the set of denominators q_B, the
largest number of terms m_B, the largest C_B = sum_A |c_AB| (the integer
coefficient sum, which bounds the bits the pre-division sum needs), the
largest sum_A |det P[A, B]|, and the counts of coefficients equal to +1 or -1
(an addition or a subtraction in the kernel, not a product).

The changes are 1.4's conformal change for d = 1 to 4 (target R(d+1, 1, 0)),
its inverse, and the test-side LDL^T change of each seeded random form,
reproduced here with the reference's diagonalize and the squarefree scaling
of each square, and its inverse. Exact, over fractions.Fraction; no floating
arithmetic.

Run from the repository root:
  python3 -I docs/agent/records/numerical/wu12-change-basis-tolerances/changes.py
"""

import itertools
import math
import pathlib
import sys
from fractions import Fraction

ROOT = pathlib.Path(__file__).resolve().parents[5]
sys.path.insert(0, str(ROOT / "test" / "reference"))
import clifford_reference as ref  # noqa: E402


def conformal_change(d):
    """1.4: e_i = e'_i for i <= d, e(d+1) = (e'(d+2) - e'(d+1)) / 2,
    e(d+2) = e'(d+2) + e'(d+1), into R(d+1, 1, 0)."""
    n = d + 2
    p = [[Fraction(int(i == j)) for j in range(n)] for i in range(n)]
    p[d][d], p[d][d + 1] = Fraction(-1, 2), Fraction(1, 2)
    p[d + 1][d], p[d + 1][d + 1] = Fraction(1), Fraction(1)
    return p


def squarefree(k):
    k = abs(k)
    s = 1
    f = 2
    while f * f <= k:
        while k % (f * f) == 0:
            k //= f * f
        if k % f == 0:
            s *= f
            k //= f
        f += 1
    return s * k


def ldlt_change(g):
    """The change from g to its scaled diagonal counterpart: T g T^T = D by
    the reference, each square p/q scaled to the squarefree part of p q (sign
    kept) with row i of T scaled by q / k, where p q = sqf k^2. P = T'^-1."""
    t, d = ref.diagonalize(g)
    n = len(g)
    scaled = []
    squares = []
    for i in range(n):
        if d[i] == 0:
            scaled.append(t[i])
            squares.append(0)
            continue
        p, q = d[i].numerator, d[i].denominator
        sq = squarefree(p * q)
        k = math.isqrt(abs(p * q) // sq)
        assert k * k * sq == abs(p * q)
        s = Fraction(q, k)
        scaled.append([s * x for x in t[i]])
        squares.append((1 if p > 0 else -1) * sq)
    tg = ref.mat_mul(ref.mat_mul(scaled, [[Fraction(x) for x in r] for r in g]),
                     ref.transpose(scaled))
    for i in range(n):
        for j in range(n):
            assert tg[i][j] == (squares[i] if i == j else 0)
    p = ref.inverse(scaled)
    target = ref.diagonal_gram(squares)
    check = ref.mat_mul(ref.mat_mul(p, [[Fraction(x) for x in r] for r in target]),
                        ref.transpose(p))
    assert check == [[Fraction(x) for x in r] for r in g]
    return p, squares


def outputs(p):
    """Per output blade B: (grade, q_B, list of c_AB)."""
    n = len(p)
    result = []
    for k in range(n + 1):
        sets = list(itertools.combinations(range(n), k))
        for cols in sets:
            minors = []
            for rows in sets:
                minor = ref.det([[p[r][c] for c in cols] for r in rows])
                if minor != 0:
                    minors.append(minor)
            q = 1
            for x in minors:
                q = math.lcm(q, x.denominator)
            result.append((k, q, [int(x * q) for x in minors],
                           sum(abs(x) for x in minors)))
    return result


def report(name, p):
    out = outputs(p)
    qs = sorted({q for _, q, _, _ in out})
    m = max(len(c) for _, _, c, _ in out)
    big_c = max(sum(abs(x) for x in c) for _, _, c, _ in out)
    big_s = max(s for _, _, _, s in out)
    unit = sum(1 for _, _, c, _ in out for x in c if abs(x) == 1)
    total = sum(len(c) for _, _, c, _ in out)
    dyadic = all(q & (q - 1) == 0 for q in qs)
    print(f"{name}: q in {qs} ({'dyadic' if dyadic else 'not dyadic'}); "
          f"max m = {m}; max C = {big_c}; max sum|minor| = {big_s}; "
          f"+-1 coefficients {unit} of {total}")
    return out


def main():
    for d in range(1, 5):
        p = conformal_change(d)
        report(f"conformal({d}) -> R({d + 1},1,0)", p)
        report(f"R({d + 1},1,0) -> conformal({d})", ref.inverse(p))
    for k, g in enumerate(ref.random_forms()):
        p, squares = ldlt_change(g)
        rows = [math.lcm(*(x.denominator for x in row)) for row in p]
        print(f"random {k}, n = {len(g)}, target squares {squares}, "
              f"row denominators of P {rows}")
        report(f"  random {k} -> diagonal", p)
        report(f"  diagonal -> random {k}", ref.inverse(p))


if __name__ == "__main__":
    main()
