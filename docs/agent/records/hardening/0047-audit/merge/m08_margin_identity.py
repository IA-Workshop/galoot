"""The margin inequality of transform's rounding check, checked with sympy.

Claim (transform.hpp, the comment above transform_rounding_margin):
(1 - u)^(2K+1) gamma_(K+1) >= gamma_K whenever (K + 1)(2K + 1) u < 1,
with gamma_k = k u / (1 - k u).

Route: (1 - u)^(2K+1) >= 1 - (2K + 1) u (Bernoulli), so it suffices that
(1 - (2K + 1) u) gamma_(K+1) - gamma_K >= 0. Multiplying by the positive
(1 - K u)(1 - (K + 1) u) / u leaves the polynomial
f = (K + 1)(1 - K u)(1 - (2K + 1) u) - K (1 - (K + 1) u).
The numerical reader states f = 1 - (K + 1)(2K + 1) u + K (K + 1)(2K + 1) u^2.
This script expands f and compares. It then checks the inequality itself,
without the Bernoulli step, for u = 2^-53, 2^-24 and 2^-11: exactly in
rationals for small K, and with mpmath at 60 significant digits at the
largest K the condition admits, where the exact power is too large to form.
"""

import math

import mpmath
import sympy as sp

K, u = sp.symbols("K u", positive=True)
f = (K + 1) * (1 - K * u) * (1 - (2 * K + 1) * u) - K * (1 - (K + 1) * u)
stated = 1 - (K + 1) * (2 * K + 1) * u + K * (K + 1) * (2 * K + 1) * u**2
print("f - stated expands to:", sp.expand(f - stated))


def gamma(k, uu):
    return k * uu / (1 - k * uu)


def k_max_of(digits):
    """The largest K with (K + 1)(2K + 1) < 2^digits."""
    k = (math.isqrt(8 * 2**digits + 1) - 3) // 4 + 2
    while (k + 1) * (2 * k + 1) >= 2**digits:
        k -= 1
    return k


mpmath.mp.dps = 60
for digits in (53, 24, 11):
    uu = sp.Rational(1, 2**digits)
    k_max = k_max_of(digits)
    for k in (1, 2, 10):
        lhs = (1 - uu) ** (2 * k + 1) * gamma(k + 1, uu)
        print(f"u = 2^-{digits}, K = {k}: holds exactly: {lhs >= gamma(k, uu)}")
    for k in (k_max - 1, k_max):
        um = mpmath.mpf(1) / 2**digits
        lhs = (1 - um) ** (2 * k + 1) * (k + 1) * um / (1 - (k + 1) * um)
        rhs = k * um / (1 - k * um)
        print(f"u = 2^-{digits}, K = {k} (largest admitted {k_max}): "
              f"holds at 60 digits: {lhs >= rhs}, lhs / rhs - 1 = "
              f"{mpmath.nstr(lhs / rhs - 1, 5)}")
