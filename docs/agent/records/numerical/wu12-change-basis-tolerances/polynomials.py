"""The polynomial and constant checks behind the change-of-basis tolerance rule.

Symbolic checks with sympy, then exact checks over fractions.Fraction with
u = 2^-53 (double) and u = 2^-24 (float). No floating arithmetic is used.

1. Lemma 3.1's two extremes, (1 + u)^k - 1 <= gamma_k and
   1 - (1 - u)^k <= gamma_k, exactly for k = 1 to 64 (the division rule adds
   one factor to each term, so it relies on the lemma for K + 1 factors).
2. The scaled ratio check reuses ratio_limit = 1 + 4u: five roundings give a
   computed ratio r (1 + theta_5), and gamma_5 < 6u whenever 30 u < 1, so a
   double at most 1 + gamma_5 is at most 1 + 4u. The rejection threshold
   (1 + 4u) / (1 - gamma_5) is below 1 + 10u.
3. The alternative computed tolerance with a margin of one rounding,
   fl(fl(gamma_(K+1)) fl(M w)) compared with fl(|w d - w e|):
   (1 - u)^2 gamma_(K+1) >= gamma_K, whose cleared form is
   1 - 2(K + 1)u + (K + 1)(2K + 1)u^2 - K(K + 1)u^3, positive whenever
   2(K + 1)u < 1; and margin 0 fails, (1 - u)^2 gamma_K < gamma_K.
4. The exactness bounds of the small-input regime: with inputs n 2^-b,
   |n| <= N = 2^20, and the coefficient sums of 1.4's change (3 forward,
   2 back), every pre-division value needs at most 53 (double) or 24 (float)
   bits, and so does the round trip (12 N).

Run from the repository root:
  python3 -I docs/agent/records/numerical/wu12-change-basis-tolerances/polynomials.py
"""

from fractions import Fraction

import sympy

U53 = Fraction(1, 2**53)
U24 = Fraction(1, 2**24)


def gamma(k, u):
    return k * u / (1 - k * u)


def check_lemma(u, top=64):
    for k in range(1, top + 1):
        assert (1 + u) ** k - 1 <= gamma(k, u), k
        assert 1 - (1 - u) ** k <= gamma(k, u), k
    print(f"1. Lemma 3.1 extremes hold exactly for k = 1 to {top}, u = {u}")


def check_ratio_limit():
    u = sympy.symbols("u", positive=True)
    g5 = 5 * u / (1 - 5 * u)
    # 6u - gamma_5 = u (1 - 30u) / (1 - 5u): positive whenever 30u < 1.
    diff = sympy.factor(sympy.together(6 * u - g5))
    print(f"2. 6u - gamma_5 = {diff}")
    assert sympy.simplify(diff - u * (1 - 30 * u) / (1 - 5 * u)) == 0
    for uu in (U53, U24):
        assert gamma(5, uu) < 6 * uu
        threshold = (1 + 4 * uu) / (1 - gamma(5, uu))
        assert threshold < 1 + 10 * uu
        print(f"   u = {uu}: gamma_5 < 6u; rejection threshold "
              f"(1 + 4u)/(1 - gamma_5) = 1 + "
              f"{float((threshold - 1) / uu):.6f} u")


def check_margin_one():
    u, k = sympy.symbols("u K", positive=True)
    lhs = (1 - u) ** 2 * (k + 1) * u / (1 - (k + 1) * u)
    rhs = k * u / (1 - k * u)
    # Multiply out by the positive denominators (1 - (K+1)u)(1 - Ku) and
    # divide by u > 0.
    cleared = sympy.expand((1 - u) ** 2 * (k + 1) * (1 - k * u)
                           - k * (1 - (k + 1) * u))
    expected = sympy.expand(1 - 2 * (k + 1) * u + (k + 1) * (2 * k + 1) * u**2
                            - k * (k + 1) * u**3)
    assert sympy.simplify(cleared - expected) == 0
    assert sympy.simplify(
        (lhs - rhs) * (1 - (k + 1) * u) * (1 - k * u) / u - cleared) == 0
    print(f"3. cleared (1-u)^2 gamma_(K+1) - gamma_K = {sympy.factor(cleared)}"
          f" = {expected}")
    # Positivity: with x = (K + 1) u < 1/2, the polynomial is
    # 1 - 2x + x (2K + 1) u - K x u^2 >= 1 - 2x > 0, since
    # (2K + 1) u - K u^2 > 0 for u < 1.
    x = sympy.symbols("x", positive=True)
    rest = sympy.expand(expected.subs(k, x / u - 1))
    print(f"   in x = (K + 1) u: {sympy.collect(rest, x)}")
    for uu in (U53, U24):
        top = 10_000 if uu == U53 else 2_000
        for kk in range(1, top + 1):
            assert (1 - uu) ** 2 * gamma(kk + 1, uu) >= gamma(kk, uu), kk
            assert (1 - uu) ** 2 * gamma(kk, uu) < gamma(kk, uu), kk
        print(f"   u = {uu}: margin 1 holds and margin 0 fails, "
              f"exactly, for K = 1 to {top}")


def check_exact_inputs():
    n = 2**20
    for name, bits in (("double", 53), ("float", 24)):
        forward = 3 * n
        trip = 12 * n
        assert forward <= 2**bits and trip <= 2**bits
        print(f"4. {name}: 3 N = 2^{forward.bit_length() - 1}.x, "
              f"12 N = {trip} <= 2^{bits} = {2**bits}")


def main():
    print(f"sympy {sympy.__version__}")
    check_lemma(U53)
    check_lemma(U24)
    check_ratio_limit()
    check_margin_one()
    check_exact_inputs()


if __name__ == "__main__":
    main()
