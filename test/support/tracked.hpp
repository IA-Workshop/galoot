#ifndef GALOOT_TEST_SUPPORT_TRACKED_HPP
#define GALOOT_TEST_SUPPORT_TRACKED_HPP

// A scalar for tests that carries, with each value, what a forward error
// bound for the same computation over double needs, and the check of a
// double result against that bound.
//
// Run a computation over tracked, with each input made by tracked::input,
// and run the same computation over double on the same inputs. Each result
// of the tracked run holds:
//   value      the exact result, a rational;
//   magnitude  the exact result of the same computation with every input
//              and every constant replaced by its absolute value and every
//              subtraction by an addition;
//   roundings  K, the largest number of rounded operations on any path from
//              an input to the result.
// In the standard model of floating-point arithmetic, fl(a op b) = (a op b)
// (1 + d) with |d| <= u for + - *, where u = 2^-53 for double (Higham,
// Accuracy and Stability of Numerical Algorithms, 2nd ed., SIAM, 2002,
// equation (2.4)). Expanded over the products of inputs and constants that
// it sums, the double result is a sum of terms m_i (1 + d_i1) ... (1 + d_ik),
// each with at most K factors, so by Lemma 3.1 there it is
// sum m_i (1 + theta_i) with |theta_i| <= gamma_K = K u / (1 - K u), and
//   |double result - value| <= gamma_K sum |m_i| = gamma_K magnitude,
// as for an inner product in equation (3.5) there. The rules below follow
// from that expansion:
//   an input, an integer constant, a default value (zero): K = 0;
//   -a: K and the magnitude of a, since negation is exact;
//   a + b and a - b: K = max(K_a, K_b) + 1, magnitude m_a + m_b, since the
//     one rounding multiplies every term of both;
//   a * b: K = K_a + K_b + 1, magnitude m_a m_b, since a term of the
//     product is a term of a times a term of b, times the one rounding.
// A rule may count a rounding that the double operation does not make, as
// for a product by 2, which only makes the bound larger. A fused
// multiply-add makes one rounding where the rules count two, so the bound
// holds for a contracted build too.
//
// It is a test oracle, not part of the library. Its rationals stop rather
// than wrap when a value does not fit std::int64_t, so inputs must be sized
// to keep every value and magnitude in range.

#include <bit>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <limits>

#include <support/rational.hpp>

namespace test_support {

/// 2^k, for k from 0 to 62.
[[nodiscard]] constexpr std::int64_t power_of_two(int k) noexcept {
  std::int64_t p = 1;
  for (int i = 0; i < k; ++i) {
    p *= 2;
  }
  return p;
}

/// A value with its exact magnitude and its count of roundings, as above.
///
/// Guarantees: value and magnitude are exact; magnitude >= |value|;
/// roundings >= 0. Trivially copyable, and constructible from the integer
/// coefficients a library converts to its scalar type.
struct tracked {
  rational value;
  rational magnitude;
  int roundings = 0;

  /// Zero, exact.
  constexpr tracked() = default;

  /// The integer c, exact: a double holds every integer the tests use.
  explicit constexpr tracked(std::int64_t c) noexcept
      : value{c}, magnitude{c < 0 ? -c : c} {}

  /// An input: the value x, which the matching double holds exactly.
  [[nodiscard]] static constexpr tracked input(rational x) noexcept {
    tracked t;
    t.value = x;
    t.magnitude = x.numerator < 0 ? -x : x;
    return t;
  }
};

namespace detail {

[[nodiscard]] constexpr int larger(int a, int b) noexcept {
  return a < b ? b : a;
}

} // namespace detail

/// -a: exact, so the count and the magnitude are a's.
[[nodiscard]] constexpr tracked operator-(const tracked& a) noexcept {
  tracked r = a;
  r.value = -a.value;
  return r;
}

/// a + b: one rounding after the larger count of the two.
[[nodiscard]] constexpr tracked operator+(const tracked& a,
                                          const tracked& b) noexcept {
  tracked r;
  r.value = a.value + b.value;
  r.magnitude = a.magnitude + b.magnitude;
  r.roundings = detail::larger(a.roundings, b.roundings) + 1;
  return r;
}

/// a - b: as a + b, with the magnitudes added.
[[nodiscard]] constexpr tracked operator-(const tracked& a,
                                          const tracked& b) noexcept {
  tracked r;
  r.value = a.value - b.value;
  r.magnitude = a.magnitude + b.magnitude;
  r.roundings = detail::larger(a.roundings, b.roundings) + 1;
  return r;
}

/// a * b: one rounding after the counts of both.
[[nodiscard]] constexpr tracked operator*(const tracked& a,
                                          const tracked& b) noexcept {
  tracked r;
  r.value = a.value * b.value;
  r.magnitude = a.magnitude * b.magnitude;
  r.roundings = a.roundings + b.roundings + 1;
  return r;
}

// The check below assumes that each double operation rounds once, to
// nearest, to double's 53 bits.
static_assert(std::numeric_limits<double>::is_iec559);
static_assert(std::numeric_limits<double>::digits == 53);
static_assert(std::numeric_limits<double>::round_style ==
              std::round_to_nearest);
static_assert(FLT_EVAL_METHOD == 0,
              "each double operation must round to double, not to a wider "
              "format");

/// The unit roundoff of double, 2^-53.
inline constexpr double unit_roundoff = 0x1p-53;

/// The number of roundings the bound check adds to K for a result with two
/// or more roundings (one rounding is checked by equality, below), so that
/// the check, itself computed in double, never rejects a result within the
/// true bound.
///
/// Derivation. Let e be the exact value, M the magnitude, K >= 2 the count,
/// and d the double result, so |d - e| <= gamma_K M. The check computes
///   e' = fl(e) and M' = fl(M), each rounded to nearest once, so
///     |e' - e| <= u |e| <= u M (Higham, Theorem 2.2; |e| <= M) and
///     M' >= (1 - u) M;
///   g = fl(k u / (1 - k u)) for k = K + margin, where k u and 1 - k u are
///     exact (k u is a small integer times 2^-53, and 1 - k u is a multiple
///     of 2^-53 in [1/2, 1)), so only the division rounds and
///     g >= (1 - u) gamma_k;
///   and then fl(|d - e'|) <= fl(g M').
// The difference: |d - e'| <= (gamma_K + u) M <= gamma_(K+1) M, by Lemma 3.3
// of Higham (gamma_k + u <= gamma_(k+1)). The tolerance: g M' >= (1 - u)^2
// gamma_(K+2) M with margin 2, and (1 - u)^2 gamma_(j+1) >= gamma_j for
// j = K + 1, since (j + 1)(1 - u)^2 (1 - j u) - j (1 - (j + 1) u) =
// 1 - 2 (j + 1) u + (j + 1)(2 j + 1) u^2 - j (j + 1) u^3 > 0 whenever
// 2 (j + 1) u < 1. So the exact |d - e'| is at most the exact g M'.
// Rounding to nearest is monotone, so the two rounded sides compare the
// same way, and the check passes. Margin 1 is not enough for this argument:
// (1 - u)^2 gamma_(K+1) < gamma_(K+1). Assumes no underflow or overflow,
// which the tests' inputs, multiples of 2^-27 below 1 in magnitude, keep.
inline constexpr int rounding_margin = 2;

/// The double nearest x, for a rational whose denominator is a power of
/// two: the numerator converted to double, then divided by the denominator,
/// which is exact. The language leaves the direction of an inexact integer
/// conversion to the implementation; GCC and Clang round to nearest, as
/// IEEE 754 does by default, and the tests of this header check it.
/// Requires: the denominator is a power of two; checked, and a false check
/// aborts.
[[nodiscard]] inline double nearest_double(rational x) {
  const std::int64_t den = x.denominator;
  if (!std::has_single_bit(static_cast<std::uint64_t>(den))) {
    detail::rational_failed(
        "nearest_double: the denominator is not a power of two");
  }
  return static_cast<double>(x.numerator) / static_cast<double>(den);
}

/// The exact rational value of a finite double: its significand, an odd
/// integer of at most 53 bits, times a power of two. Requires that the value
/// fits the rational, a denominator of at most 2^62 and a magnitude below
/// 2^62, as every value of the tests' inputs does; checked, and a false
/// check aborts.
[[nodiscard]] inline rational exact_rational(double d) {
  if (d == 0.0) {
    return rational{};
  }
  int exponent = 0;
  const double fraction = std::frexp(d, &exponent);
  auto significand = static_cast<std::int64_t>(std::ldexp(fraction, 53));
  int power = exponent - 53;
  while (significand % 2 == 0) {
    significand /= 2;
    ++power;
  }
  if (power > 8 || power < -62) {
    detail::rational_failed(
        "exact_rational: the double does not fit the rational");
  }
  if (power >= 0) {
    return rational{significand * power_of_two(power)};
  }
  return rational{significand, power_of_two(-power)};
}

/// The tolerance of a result with `roundings` K >= 2 and the magnitude
/// `magnitude`: fl(fl(gamma_(K + margin)) fl(magnitude)), as derived above.
[[nodiscard]] inline double rounding_tolerance(int roundings,
                                               rational magnitude) {
  const double k = roundings + rounding_margin;
  const double gamma = (k * unit_roundoff) / (1.0 - (k * unit_roundoff));
  return gamma * nearest_double(magnitude);
}

/// The ratio of the exact error |d - exact| to the bound gamma_K magnitude,
/// computed in double: at most ratio_limit for a result within the bound;
/// zero when the error is zero. Requires roundings >= 1.
[[nodiscard]] inline double bound_ratio(double d, rational exact, int roundings,
                                        rational magnitude) {
  const rational difference = exact_rational(d) - exact;
  if (difference.numerator == 0) {
    return 0.0;
  }
  const double k = roundings;
  const double gamma = (k * unit_roundoff) / (1.0 - (k * unit_roundoff));
  return std::abs(nearest_double(difference)) /
         (gamma * nearest_double(magnitude));
}

/// The largest value bound_ratio returns for a result within its bound,
/// 1 + 6u = 1 + 3 2^-52.
///
/// Derivation. For d within the bound the exact ratio r = |d - e| /
/// (gamma_K M) is at most 1. bound_ratio forms the exact difference d - e
/// as a rational and then rounds five times: converting the difference to
/// double, the division that forms gamma_K (k u and 1 - k u are exact, as
/// for the margin), converting M, the product gamma_K M, and the final
/// division. Each multiplies or divides r by one factor (1 + delta) with
/// |delta| <= u, so the computed ratio is r (1 + theta_5) with
/// |theta_5| <= gamma_5 (Higham, Lemma 3.1), at most 1 + gamma_5.
// gamma_5 = 5u / (1 - 5u) = 5u + 25u^2 / (1 - 5u) < 6u, and the doubles just
// above 1 are 1 + 2u, 1 + 4u, 1 + 6u, so 1 + 6u is the smallest double at or
// above 1 + gamma_5, and a computed ratio of a result within the bound never
// exceeds it. A result whose exact ratio exceeds (1 + 6u) / (1 - gamma_5),
// about 1 + 11u, is rejected.
inline constexpr double ratio_limit = 1.0 + (6 * unit_roundoff);

/// Whether the double d is within the forward error bound of the exact value
/// `exact`, for a computation whose tracked run gave the count `roundings`
/// and the magnitude `magnitude`. The exact value comes from a reference
/// independent of the computation, so the check also catches a wrong value.
///
/// K = 0: no operation rounded, so d must be the exact value itself.
/// K = 1: every operand of the one rounded operation was exact (a value of
/// depth 0 is an input, an integer, a copy, or a negation of one), and IEEE
/// 754 rounds +, -, and * correctly, so d must be the double nearest the
/// exact value, ties to even; a negation or a copy after it keeps that.
/// K >= 2: both fl(|d - fl(exact)|) <= rounding_tolerance, and
/// bound_ratio <= ratio_limit, which measures the error from the exact
/// value and so admits no error that the margin's distance from fl(exact)
/// would.
[[nodiscard]] inline bool within_rounding_bound(double d, rational exact,
                                                int roundings,
                                                rational magnitude) {
  if (roundings == 0) {
    return exact_rational(d) == exact;
  }
  const double nearest = nearest_double(exact);
  if (roundings == 1) {
    return d == nearest;
  }
  return std::abs(d - nearest) <= rounding_tolerance(roundings, magnitude) &&
         bound_ratio(d, exact, roundings, magnitude) <= ratio_limit;
}

} // namespace test_support

#endif // GALOOT_TEST_SUPPORT_TRACKED_HPP
