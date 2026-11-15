// Tests of the tracking scalar and of the bound check in
// support/tracked.hpp: the counting rules on hand cases, and the check
// accepting a result within the bound and rejecting one beyond it.
#include <cmath>
#include <cstdint>

#include <doctest/doctest.h>

#include <support/rational.hpp>
#include <support/tracked.hpp>

namespace {

using test_support::rational;
using test_support::tracked;

constexpr double u = 0x1p-53;

} // namespace

// Reference: proof: expanded over the products of inputs, each rounding is
//   one factor (1 + d) on every term below it, so an input, an integer, zero,
//   and a negation round nothing, a sum or difference rounds once after the
//   larger count of its operands and adds their magnitudes, and a product
//   rounds once after both counts and multiplies their magnitudes
TEST_CASE("tracked counts roundings and magnitudes by its rules") {
  const tracked a = tracked::input(rational{3, 4});
  const tracked b = tracked::input(rational{-1, 2});
  const tracked c{-3};
  const tracked zero;

  CHECK(a.value == rational{3, 4});
  CHECK(a.magnitude == rational{3, 4});
  CHECK(a.roundings == 0);
  CHECK(b.magnitude == rational{1, 2});
  CHECK(c.value == rational{-3});
  CHECK(c.magnitude == rational{3});
  CHECK(c.roundings == 0);
  CHECK(zero.value == rational{});
  CHECK(zero.magnitude == rational{});
  CHECK(zero.roundings == 0);

  const tracked n = -b;
  CHECK(n.value == rational{1, 2});
  CHECK(n.magnitude == rational{1, 2});
  CHECK(n.roundings == 0);

  const tracked p = a * b;
  CHECK(p.value == rational{-3, 8});
  CHECK(p.magnitude == rational{3, 8});
  CHECK(p.roundings == 1);

  const tracked s = a + b;
  CHECK(s.value == rational{1, 4});
  CHECK(s.magnitude == rational{5, 4});
  CHECK(s.roundings == 1);

  const tracked d = a - b;
  CHECK(d.value == rational{5, 4});
  CHECK(d.magnitude == rational{5, 4});
  CHECK(d.roundings == 1);

  // The larger count, plus one; and the counts of both factors, plus one:
  // c * s is two deep, so p (c * s) is 1 + 2 + 1.
  const tracked deeper = p + a;
  CHECK(deeper.value == rational{3, 8});
  CHECK(deeper.magnitude == rational{9, 8});
  CHECK(deeper.roundings == 2);
  const tracked deeper_difference = a - p;
  CHECK(deeper_difference.roundings == 2);
  CHECK(deeper_difference.magnitude == rational{9, 8});
  const tracked product_of_products = p * (c * s);
  CHECK(product_of_products.value == rational{9, 32});
  CHECK(product_of_products.magnitude == rational{45, 32});
  CHECK(product_of_products.roundings == 4);
}

// Reference: contract: the conversion of a 64-bit integer to double rounds
//   to nearest, ties to even, so 2^53 + 1 gives 2^53 and 2^53 + 3 gives
//   2^53 + 4, and a rational with a power-of-two denominator becomes the
//   nearest double, and a double converts back exactly to its significand
//   times a power of two
TEST_CASE("conversions between rationals and doubles round to nearest") {
  constexpr std::int64_t two_53 = test_support::power_of_two(53);
  CHECK(test_support::nearest_double(rational{two_53 + 1}) == 0x1p53);
  CHECK(test_support::nearest_double(rational{two_53 + 3}) == 0x1p53 + 4.0);
  CHECK(test_support::nearest_double(rational{two_53 + 1, two_53}) == 1.0);
  CHECK(test_support::nearest_double(rational{-3, 8}) == -0.375);
  CHECK(test_support::exact_rational(0.75) == rational{3, 4});
  CHECK(test_support::exact_rational(0x1p-54) ==
        rational{1, test_support::power_of_two(54)});
  CHECK(test_support::exact_rational(-0.0) == rational{});
  CHECK(test_support::exact_rational(-3 * 0x1p-62) ==
        rational{-3, test_support::power_of_two(62)});
  CHECK(test_support::exact_rational(0x1p53 + 2.0) ==
        rational{test_support::power_of_two(53) + 2});
}

// Reference: contract: exact_rational accepts every double with a
//   denominator of at most 2^62 and a magnitude below 2^62, so the integers
//   1024 = 2^10 and 511 2^9, the largest power of two below 2^62, 2^61, and
//   the largest double below it, (2^53 - 1) 2^9, convert exactly, as does
//   2^-62 with its denominator at the limit, while 2^62, (2^53 - 1) 2^10,
//   and 2^-63 abort, which a program of their own checks
TEST_CASE("exact_rational converts every double within its stated range") {
  const std::int64_t two_53 = test_support::power_of_two(53);
  CHECK(test_support::exact_rational(1024.0) == rational{1024});
  CHECK(test_support::exact_rational(-1024.0) == rational{-1024});
  CHECK(test_support::exact_rational(511.0 * 512.0) ==
        rational{std::int64_t{511} * 512});
  CHECK(test_support::exact_rational(0x1p61) ==
        rational{test_support::power_of_two(61)});
  CHECK(test_support::exact_rational((0x1p53 - 1.0) * 0x1p9) ==
        rational{(two_53 - 1) * 512});
  CHECK(test_support::exact_rational(-(0x1p53 - 1.0) * 0x1p9) ==
        rational{-(two_53 - 1) * 512});
  CHECK(test_support::exact_rational(0x1p-62) ==
        rational{1, test_support::power_of_two(62)});
}

// Reference: proof: one rounding of exact operands is correctly rounded in
//   IEEE 754, so with one rounding only the double nearest the exact value
//   is accepted, ties to even: 1 for 1 + 2^-53 and 1 + 2^-52 for
//   1 + 3 2^-54, and not the other neighbor, though it is within gamma_1
//   times the magnitude
TEST_CASE("with one rounding the check accepts only the nearest double") {
  const std::int64_t two_53 = test_support::power_of_two(53);
  const std::int64_t two_54 = test_support::power_of_two(54);
  const rational tie{two_53 + 1, two_53};
  CHECK(test_support::within_rounding_bound(1.0, tie, 1, tie));
  CHECK_FALSE(test_support::within_rounding_bound(1.0 + (2 * u), tie, 1, tie));
  CHECK(test_support::within_rounding_bound(-1.0, -tie, 1, tie));

  const rational above{two_54 + 3, two_54};
  CHECK(test_support::within_rounding_bound(1.0 + (2 * u), above, 1, above));
  CHECK_FALSE(test_support::within_rounding_bound(1.0, above, 1, above));
}

// Reference: proof: with K = 3 and the exact value and magnitude
//   e = 1 + 2^-53, the double 1 + 4u is 3u from e, within gamma_3 e, but 4u
//   from fl(e) = 1, beyond fl(gamma_3) fl(e), so a check without its margin
//   rejects it, while 1 - 4u is 5u from e, beyond the bound, though within the
//   margin's tolerance of fl(e), and the ratio of its error to the bound
//   rejects it
TEST_CASE("with several roundings the check admits exactly the bound") {
  const std::int64_t two_53 = test_support::power_of_two(53);
  const rational e{two_53 + 1, two_53};

  CHECK(test_support::within_rounding_bound(1.0 + (4 * u), e, 3, e));
  CHECK(test_support::within_rounding_bound(1.0 - (2 * u), e, 3, e));
  const double ratio = test_support::bound_ratio(1.0 + (4 * u), e, 3, e);
  CHECK(ratio <= 1.0);
  CHECK(ratio > 0.999);

  const double outside = 1.0 - (4 * u);
  CHECK(std::abs(outside - 1.0) <= test_support::rounding_tolerance(3, e));
  CHECK(test_support::bound_ratio(outside, e, 3, e) >
        test_support::ratio_limit);
  CHECK_FALSE(test_support::within_rounding_bound(outside, e, 3, e));
  CHECK_FALSE(test_support::within_rounding_bound(1.0 + (8 * u), e, 3, e));
}

// Reference: proof: for e = 5/8 and magnitude 7/4, where doubles are u
//   apart, an error of (K - 1) u is within gamma_K times 7/4 and accepted, an
//   error of 2 K u is beyond it, though within the margin's tolerance, and
//   is rejected, as is twice the tolerance, and with one rounding only 5/8 is
//   accepted, and with no rounding only the exact value, not even the next
//   double
TEST_CASE("the bound check rejects a result beyond the bound") {
  const rational e{5, 8};
  const rational m{7, 4};
  for (int k = 2; k <= 8; ++k) {
    CAPTURE(k);
    const double tolerance = test_support::rounding_tolerance(k, m);
    CHECK(test_support::within_rounding_bound(0.625 + ((k - 1) * u), e, k, m));
    CHECK(test_support::within_rounding_bound(0.625 - ((k - 1) * u), e, k, m));
    CHECK(std::abs(0.625 + (2 * k * u) - 0.625) <= tolerance);
    CHECK_FALSE(
        test_support::within_rounding_bound(0.625 + (2 * k * u), e, k, m));
    CHECK_FALSE(
        test_support::within_rounding_bound(0.625 + (2 * tolerance), e, k, m));
    CHECK_FALSE(
        test_support::within_rounding_bound(0.625 - (2 * tolerance), e, k, m));
  }
  CHECK(test_support::within_rounding_bound(0.625, e, 1, m));
  CHECK_FALSE(
      test_support::within_rounding_bound(std::nextafter(0.625, 1.0), e, 1, m));
  CHECK(test_support::within_rounding_bound(0.625, e, 0, m));
  CHECK_FALSE(
      test_support::within_rounding_bound(std::nextafter(0.625, 1.0), e, 0, m));
  CHECK_FALSE(
      test_support::within_rounding_bound(std::nextafter(0.625, 0.0), e, 0, m));
}

// Reference: proof: with K = 2, gamma_2 = 1 / (2^52 - 1), so for d = 2^-50,
//   e = 0, and M = (2^52 - 1) 2^-50 the exact ratio of the error to
//   gamma_2 M is 1, computed as 1, and accepted, and with
//   M = (2^52 - 9) 2^-50 it is (2^52 - 1) / (2^52 - 9), about 1 + 16u, and
//   rejected, though the margin's tolerance admits it, and with K = 3 and
//   M = (2^57 - 47) / 3 times 2^-54, just above d / gamma_3, the exact ratio
//   is 1 - u / 16 and the computed one 1 + 2u, which the limit 1 + 4u
//   accepts
TEST_CASE("the ratio limit admits the bound and nothing beyond it") {
  const double d = 0x1p-50;
  const rational zero;
  const std::int64_t two_50 = test_support::power_of_two(50);
  const std::int64_t two_52 = test_support::power_of_two(52);

  const rational at_bound{two_52 - 1, two_50};
  CHECK(test_support::bound_ratio(d, zero, 2, at_bound) == 1.0);
  CHECK(test_support::within_rounding_bound(d, zero, 2, at_bound));

  const rational beyond{two_52 - 9, two_50};
  CHECK(d <= test_support::rounding_tolerance(2, beyond));
  CHECK(test_support::bound_ratio(d, zero, 2, beyond) >
        test_support::ratio_limit);
  CHECK_FALSE(test_support::within_rounding_bound(d, zero, 2, beyond));

  const rational just_within{48'038'396'025'285'275,
                             test_support::power_of_two(54)};
  CHECK(test_support::bound_ratio(d, zero, 3, just_within) == 1.0 + (2 * u));
  CHECK(test_support::within_rounding_bound(d, zero, 3, just_within));
}

// Reference: proof: two results of exact value e, each with at most one
//   rounding of exact operands, are both the double nearest e, so the
//   identity check accepts only equal results, here 1 and 1 against the
//   neighbor 1 + 2u, whatever the magnitudes
TEST_CASE("with one rounding a side the identity check accepts only equality") {
  const rational m{3};
  CHECK(test_support::within_identity_bound(1.0, 0, m, 1.0, 1, m));
  CHECK(test_support::within_identity_bound(-0.5, 1, m, -0.5, 1, m));
  CHECK_FALSE(
      test_support::within_identity_bound(1.0, 1, m, 1.0 + (2 * u), 1, m));
  CHECK_FALSE(
      test_support::within_identity_bound(1.0 + (2 * u), 0, m, 1.0, 0, m));
}

// Reference: proof: with K = 3 on both sides and magnitudes 1, the sum of
//   the bounds is 2 gamma_3 = 6u / (1 - 3u), just above 6u, so 1 + 2u
//   against 1 - 4u, 6u apart, is accepted, and the tolerance with one
//   rounding of margin a side is about 8u, so 1 + 4u against 1 - 6u, 10u
//   apart, is rejected, and for K from 2 to 200 and magnitudes of 1 and
//   7/4 the tolerance is at least the sum of gamma_K M of the two sides
TEST_CASE("with several roundings the identity check admits the sum of the "
          "bounds") {
  const rational one{1};
  CHECK(test_support::within_identity_bound(1.0 + (2 * u), 3, one,
                                            1.0 - (4 * u), 3, one));
  CHECK(test_support::within_identity_bound(1.0 - (4 * u), 3, one,
                                            1.0 + (2 * u), 3, one));
  CHECK_FALSE(test_support::within_identity_bound(1.0 + (4 * u), 3, one,
                                                  1.0 - (6 * u), 3, one));
  CHECK(test_support::within_identity_bound(1.0 + (2 * u), 0, one,
                                            1.0 - (4 * u), 5, one));

  const rational m{7, 4};
  for (int k = 2; k <= 200; ++k) {
    CAPTURE(k);
    const double gamma = (k * u) / (1.0 - (k * u));
    CHECK(test_support::identity_tolerance(k, one, k, m) >=
          gamma * (1.0 + 1.75));
    CHECK(test_support::identity_tolerance(k, one, k, m) <
          2 * gamma * (1.0 + 1.75));
  }
}

// Reference: proof: in the standard model a quotient rounds once, so a / b
//   for an exact b adds one rounding after a's and divides a's magnitude by
//   |b|, and a division by a power of two is exact, so 3/4 over 3 is 1/4
//   with one rounding, over 4 is 3/16 with none, over -3 is -1/4 of
//   magnitude 1/4, and a product over 2 keeps the product's one rounding
//   while over 6 it has two
TEST_CASE("tracked counts a division by an exact divisor") {
  const tracked a = tracked::input(rational{3, 4});
  const tracked third = a / tracked{3};
  CHECK(third.value == rational{1, 4});
  CHECK(third.magnitude == rational{1, 4});
  CHECK(third.roundings == 1);
  const tracked quarter = a / tracked{4};
  CHECK(quarter.value == rational{3, 16});
  CHECK(quarter.magnitude == rational{3, 16});
  CHECK(quarter.roundings == 0);
  const tracked negative = a / tracked{-3};
  CHECK(negative.value == rational{-1, 4});
  CHECK(negative.magnitude == rational{1, 4});
  CHECK(negative.roundings == 1);
  const tracked p = a * tracked::input(rational{-1, 2});
  CHECK((p / tracked{2}).roundings == 1);
  CHECK((p / tracked{2}).magnitude == rational{3, 16});
  CHECK((p / tracked{6}).roundings == 2);
  CHECK((p / tracked{6}).value == rational{-1, 16});
}

// Reference: contract: correctly_rounded gives the double nearest a
//   rational, ties to even, as IEEE 754 rounds the quotient of two doubles:
//   1/3, 2/3, and 1/10 as the quotients 1.0 / 3.0, 2.0 / 3.0, and
//   1.0 / 10.0 compute them, 5/4 exactly, -7/3 as -7.0 / 3.0, and
//   (2^53 + 1) / 2, dyadic but no double, as 2^52 by the tie to even
TEST_CASE("correctly_rounded gives the nearest double of any rational") {
  const std::int64_t two_53 = test_support::power_of_two(53);
  CHECK(test_support::correctly_rounded(rational{1, 3}) == 1.0 / 3.0);
  CHECK(test_support::correctly_rounded(rational{2, 3}) == 2.0 / 3.0);
  CHECK(test_support::correctly_rounded(rational{1, 10}) == 1.0 / 10.0);
  CHECK(test_support::correctly_rounded(rational{5, 4}) == 1.25);
  CHECK(test_support::correctly_rounded(rational{-7, 3}) == -7.0 / 3.0);
  CHECK(test_support::correctly_rounded(rational{two_53 + 1, 2}) == 0x1p52);
}

// Reference: proof: with e = M = 1/3 and K = 2, gamma_2 M is a little more
//   than 4/3 of an ulp of 1/3, 2^-54, and fl(1/3) lies 1/3 ulp below 1/3,
//   so the double one ulp below fl(1/3) is 4/3 ulp away, within the bound,
//   and the double two ulps above is 5/3 ulp away, beyond it, while the
//   check by count wants the exact value for K = 0 and fl(1/3) for K = 1
TEST_CASE("the quotient check admits exactly the bound") {
  const rational e{1, 3};
  const double nearest = 1.0 / 3.0;
  const double below = std::nextafter(nearest, 0.0);
  const double above = std::nextafter(std::nextafter(nearest, 1.0), 1.0);
  const double ratio_below = test_support::scaled_bound_ratio(below, e, 2, e);
  CHECK(ratio_below <= 1.0);
  CHECK(ratio_below > 0.99);
  CHECK(test_support::within_quotient_bound(below, e, 2, e));
  CHECK(test_support::scaled_bound_ratio(above, e, 2, e) > 1.2);
  CHECK_FALSE(test_support::within_quotient_bound(above, e, 2, e));
  CHECK(test_support::within_quotient_bound(nearest, e, 1, e));
  CHECK_FALSE(test_support::within_quotient_bound(below, e, 1, e));
  CHECK_FALSE(test_support::within_quotient_bound(nearest, e, 0, e));
  CHECK(test_support::within_quotient_bound(0.25, rational{1, 4}, 0, e));
  CHECK(test_support::scaled_bound_ratio(0.25, rational{1, 4}, 2, e) == 0.0);
}
