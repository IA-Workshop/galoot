# Tolerance of a quarter turn about z of the point (1, 0, 0, 1), from the
# derivation in ../rotation-tolerance.md. Exact, in fractions; no inputs.
# Run: python3 -I tol.py
import math
import struct
import sys
from fractions import Fraction as F

# pi to 62 decimals, far below 2^-120.
PI = F("3.14159265358979323846264338327950288419716939937510582097494459")


def series(x, first):
    """sin (first=1) or cos (first=0) of x by its Taylor series."""
    term = x if first else F(1)
    total = F(0)
    n = first
    while abs(term) > F(1, 2**200):
        total += term
        term = -term * x * x / ((n + 1) * (n + 2))
        n += 2
    return total


def gamma(k, u):
    return k * u / (1 - k * u)


# The angle each type passes for a quarter turn: pi/2 rounded to nearest.
double_half_pi = math.pi / 2  # fl(pi) / 2, exact halving, equals fl(pi/2)
float_half_pi = struct.unpack("f", struct.pack("f", math.pi / 2))[0]

for name, p, half in (("double", 53, double_half_pi), ("float", 24, float_half_pi)):
    u = F(1, 2**p)
    theta = F(half)  # the angle passed, exactly
    phi = theta / 2  # halving is exact
    s = series(phi, 1)
    c = series(phi, 0)
    delta = abs(PI / 2 - theta)
    assert F(1, 2) <= s < 1 and F(1, 2) <= c < 1  # so ulp(sin phi) = ulp(cos phi) = u
    for k in (F(1, 2), F(1), F(9)):
        a = b = k * u  # bounds on |s_hat - s| and |c_hat - c|
        e1 = a * (2 * s + a) + b * (2 * c + b)
        e2 = 2 * (s * b + c * a + a * b)
        m = 1 + e1  # bounds s_hat^2 + c_hat^2, the magnitude
        tol = [
            gamma(12, u) * m + e1 + delta,
            gamma(12, u) * m + e2 + delta**2 / 2,
            F(0),
            gamma(5, u) * m + e1,
        ]
        print(
            f"{name} k={str(k):>3}: tol/u = "
            + ", ".join(f"{float(t / u):.4f}" for t in tol)
            + f"; tol = " + ", ".join(f"{float(t):.4e}" for t in tol)
            + f" (E1/u {float(e1 / u):.4f}, E2/u {float(e2 / u):.4f}, delta/u {float(delta / u):.4f})"
        )
print("python", sys.version.split()[0])
