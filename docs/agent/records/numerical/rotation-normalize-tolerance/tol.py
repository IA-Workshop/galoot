# Tolerance of a quarter turn of the point (1, 0, 0, 1) about the z axis
# given with a direction of length L = 5, from the derivation in
# ../rotation-normalize-tolerance.md, which extends ../rotation-tolerance.md.
# Exact, in fractions; no inputs. Run: python3 -I tol.py
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


double_half_pi = math.pi / 2
float_half_pi = struct.unpack("f", struct.pack("f", math.pi / 2))[0]

k = F(9)  # the library's sin and cos, in ulps
for name, p, half in (("double", 53, double_half_pi), ("float", 24, float_half_pi)):
    u = F(1, 2**p)
    theta = F(half)
    phi = theta / 2
    s = series(phi, 1)
    c = series(phi, 0)
    delta = abs(PI / 2 - theta)
    assert F(1, 2) <= s < 1 and F(1, 2) <= c < 1
    # L = 1 or 2: the motor is the unit axis's bit for bit, a = k u.
    # L = 5: the stored sine is fl(5 fl(s_hat / 5)), within 2u of s_hat
    # (both in [1/2, 1), see the record), so a = (k + 2) u; the record's
    # tighter lemma puts it within u, so a = (k + 1) u.
    for label, a in (
        ("L=1,2", k * u),
        ("L=5", (k + 2) * u),
        ("L=5 (1u lemma)", (k + 1) * u),
    ):
        b = k * u
        e1 = a * (2 * s + a) + b * (2 * c + b)
        e2 = 2 * (s * b + c * a + a * b)
        m = 1 + e1
        tol = [
            gamma(12, u) * m + e1 + delta,
            gamma(12, u) * m + e2 + delta**2 / 2,
            F(0),
            gamma(5, u) * m + e1,
        ]
        print(
            f"{name} {label}: tol/u = "
            + ", ".join(f"{float(t / u):.4f}" for t in tol)
            + "; N = " + ", ".join(str(math.ceil(t / u)) for t in tol)
            + f" (E1/u {float(e1 / u):.4f}, E2/u {float(e2 / u):.4f}, delta/u {float(delta / u):.4f})"
        )
print("python", sys.version.split()[0])
