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

// Reference: proof: with one rounding and the exact value 1 + 2^-53 of
//   magnitude 1 + 2^-53, the double 1 + 2^-52 is 2^-53 from it, within
//   gamma_1 times the magnitude, while the nearest double to the exact value
//   is 1, 2^-52 away, so a check without its margin rejects it, and the
//   check accepts errors up to fl(gamma_3) from the nearest double and
//   rejects the next double beyond
TEST_CASE("the bound check accepts a result within the bound") {
  constexpr std::int64_t two_53 = test_support::power_of_two(53);
  const rational e{two_53 + 1, two_53};
  CHECK(test_support::within_rounding_bound(1.0 + 0x1p-52, e, 1, e));
  CHECK(test_support::within_rounding_bound(1.0, e, 1, e));
  CHECK(test_support::within_rounding_bound(1.0 - (3 * u), e, 1, e));
  CHECK_FALSE(test_support::within_rounding_bound(1.0 - (4 * u), e, 1, e));
  CHECK_FALSE(
      test_support::within_rounding_bound(1.0 + (2 * 0x1p-52), e, 1, e));

  // The ratio of the error to gamma_1 times the magnitude is just below 1.
  const double ratio = test_support::bound_ratio(1.0 + 0x1p-52, e, 1, e);
  CHECK(ratio <= 1.0);
  CHECK(ratio > 0.999);
}

// Reference: proof: the tolerance is fl(fl(gamma_(K+2)) fl(M)), so a result
//   twice the tolerance away is rejected for any K, and with no rounding
//   only the exact value is accepted, not even the next double
TEST_CASE("the bound check rejects a result beyond the bound") {
  const rational e{5, 8};
  const rational m{7, 4};
  for (int k = 1; k <= 8; ++k) {
    CAPTURE(k);
    const double tolerance = test_support::rounding_tolerance(k, m);
    CHECK(tolerance > 0.0);
    CHECK(
        test_support::within_rounding_bound(0.625 + (tolerance / 2), e, k, m));
    CHECK_FALSE(
        test_support::within_rounding_bound(0.625 + (2 * tolerance), e, k, m));
    CHECK_FALSE(
        test_support::within_rounding_bound(0.625 - (2 * tolerance), e, k, m));
  }
  CHECK(test_support::within_rounding_bound(0.625, e, 0, m));
  CHECK_FALSE(
      test_support::within_rounding_bound(std::nextafter(0.625, 1.0), e, 0, m));
  CHECK_FALSE(
      test_support::within_rounding_bound(std::nextafter(0.625, 0.0), e, 0, m));
}
