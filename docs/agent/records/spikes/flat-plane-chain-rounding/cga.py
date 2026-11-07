"""Exact conformal(3) antiproducts over fractions, by a route the library
does not take: geometric products from test/reference/clifford_reference.py
(which diagonalizes the form over fractions) and the complements of
CONVENTIONS 3 and 4.2, a ⟇ b = left(right(a) ⟑ right(b)).

Multivectors are dicts from blade mask (bit i - 1 for e_i, the canonical
wedge-monomial blade, orientation +1) to Fraction.
"""
import os
import sys
from fractions import Fraction

_HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(_HERE, "..", "..", "..", "..", "..", "test", "reference"))
import clifford_reference as cr  # noqa: E402

N = 5
FULL = (1 << N) - 1
_alg = cr.Algebra(cr.conformal(3))


def reorder_sign(a, b):
    return cr.reorder_sign(a, b)


def right(x):
    return {FULL ^ m: c * reorder_sign(m, FULL ^ m) for m, c in x.items()}


def left(x):
    return {FULL ^ m: c * reorder_sign(FULL ^ m, m) for m, c in x.items()}


# The antiproduct of every pair of blades, from the definition.
_ANTI = {}
for _a in range(1 << N):
    for _b in range(1 << N):
        _ANTI[(_a, _b)] = left(_alg.mul(right({_a: Fraction(1)}), right({_b: Fraction(1)})))


def clean(x):
    return {m: c for m, c in x.items() if c != 0}


def antigp(x, y):
    out = {}
    for a, ca in x.items():
        if ca == 0:
            continue
        for b, cb in y.items():
            if cb == 0:
                continue
            p = ca * cb
            for m, c in _ANTI[(a, b)].items():
                out[m] = out.get(m, 0) + p * c
    return clean(out)


def popcount(m):
    return bin(m).count("1")


def antigrade(m):
    return N - popcount(m)


def antireverse(x):
    def s(k):
        return -1 if (k * (k - 1) // 2) & 1 else 1
    return {m: c * s(antigrade(m)) for m, c in x.items()}


def sandwich_even(v, x):
    """V ⟇ x ⟇ V̰ for an even V (every blade of even antigrade)."""
    return antigp(antigp(v, x), antireverse(v))


def name(m):
    return "1" if m == 0 else "e" + "".join(str(i + 1) for i in range(N) if m >> i & 1)


def from_layout(masks, signs, coefficients):
    """A dict from a layout's oriented coefficients."""
    out = {}
    for m, s, c in zip(masks, signs, coefficients):
        out[m] = out.get(m, 0) + s * Fraction(c)
    return clean(out)
