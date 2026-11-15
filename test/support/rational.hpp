#ifndef GALOOT_TEST_SUPPORT_RATIONAL_HPP
#define GALOOT_TEST_SUPPORT_RATIONAL_HPP

// An exact rational scalar for tests: a fraction of two std::int64_t in
// lowest terms with a positive denominator. Tests use it as an exact scalar
// type whose values need not be integers, so that a product computed by the
// library can be compared exactly with one composed by hand.
//
// It is a test oracle, not part of the library, and it never rounds: an
// operation whose exact result, or any intermediate it forms, does not fit
// std::int64_t stops instead. In a constant evaluation the evaluation fails;
// at run time it writes a message to stderr and aborts. Every operation is
// constexpr, so a test can use it in a static_assert and at run time alike.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>

namespace test_support {

namespace detail {

// Ends the program with `message`. It is not constexpr, so a constant
// evaluation that reaches it fails, and the compiler's diagnostic shows the
// line of the call, which holds the message.
[[noreturn]] inline void rational_failed(const char* message) noexcept {
  std::fputs(message, stderr);
  std::fputs("\n", stderr);
  std::abort();
}

// Numerators and denominators stay within -max to max, so that negating one
// never overflows: std::numeric_limits<std::int64_t>::min() is excluded.
inline constexpr std::int64_t max = std::numeric_limits<std::int64_t>::max();

constexpr std::int64_t magnitude(std::int64_t x) noexcept {
  return x < 0 ? -x : x;
}

constexpr std::int64_t checked_add(std::int64_t a, std::int64_t b) noexcept {
  const bool fits = b >= 0 ? a <= max - b : a >= -max - b;
  if (!fits) {
    rational_failed("rational: a result does not fit std::int64_t");
  }
  return a + b;
}

constexpr std::int64_t checked_multiply(std::int64_t a,
                                        std::int64_t b) noexcept {
  const bool fits = a == 0 || magnitude(b) <= max / magnitude(a);
  if (!fits) {
    rational_failed("rational: a result does not fit std::int64_t");
  }
  return a * b;
}

// Euclid's algorithm on magnitudes; gcd(0, 0) is 0.
constexpr std::int64_t gcd(std::int64_t a, std::int64_t b) noexcept {
  a = magnitude(a);
  b = magnitude(b);
  while (b != 0) {
    const std::int64_t r = a % b;
    a = b;
    b = r;
  }
  return a;
}

} // namespace detail

/// An exact fraction numerator / denominator.
///
/// Guarantees: the denominator is positive and shares no factor with the
/// numerator, so zero is 0/1 and two equal values have equal members; both
/// lie in -max to max of std::int64_t. A structural type, so a value can be a
/// template argument, and trivially copyable.
struct rational {
  std::int64_t numerator = 0;
  std::int64_t denominator = 1;

  /// Zero.
  constexpr rational() = default;

  /// The fraction n / d in lowest terms.
  ///
  /// Requires: d is not zero, and neither n nor d is the minimum of
  /// std::int64_t; otherwise the evaluation fails or the program aborts.
  explicit constexpr rational(std::int64_t n, std::int64_t d = 1) noexcept {
    if (d == 0) {
      detail::rational_failed("rational: the denominator is zero");
    }
    if (n < -detail::max || d < -detail::max) {
      detail::rational_failed("rational: a result does not fit std::int64_t");
    }
    const std::int64_t g = detail::gcd(n, d);
    const std::int64_t sign = d < 0 ? -1 : 1;
    numerator = sign * (n / g);
    denominator = sign * (d / g);
  }

  /// Equal when the fractions are, since both are in lowest terms.
  [[nodiscard]] constexpr bool operator==(const rational&) const = default;
};

/// The negation, -n / d.
[[nodiscard]] constexpr rational operator-(rational a) noexcept {
  return rational{-a.numerator, a.denominator};
}

/// The exact sum. The denominators are first divided by their common factor,
/// so that an intermediate is no larger than the result needs.
[[nodiscard]] constexpr rational operator+(rational a, rational b) noexcept {
  const std::int64_t g = detail::gcd(a.denominator, b.denominator);
  const std::int64_t a_scale = b.denominator / g;
  const std::int64_t b_scale = a.denominator / g;
  return rational{
      detail::checked_add(detail::checked_multiply(a.numerator, a_scale),
                          detail::checked_multiply(b.numerator, b_scale)),
      detail::checked_multiply(a.denominator, a_scale)};
}

/// The exact difference, a + (-b).
[[nodiscard]] constexpr rational operator-(rational a, rational b) noexcept {
  return a + (-b);
}

/// The exact product. Each numerator is first divided by its common factor
/// with the other denominator, which is at least 1, so neither factor is 0.
[[nodiscard]] constexpr rational operator*(rational a, rational b) noexcept {
  const std::int64_t g = detail::gcd(a.numerator, b.denominator);
  const std::int64_t h = detail::gcd(b.numerator, a.denominator);
  return rational{
      detail::checked_multiply(a.numerator / g, b.numerator / h),
      detail::checked_multiply(a.denominator / h, b.denominator / g)};
}

/// The exact quotient.
///
/// Requires: b is not zero; otherwise the evaluation fails or the program
/// aborts.
[[nodiscard]] constexpr rational operator/(rational a, rational b) noexcept {
  if (b.numerator == 0) {
    detail::rational_failed("rational: division by zero");
  }
  return a * rational{b.denominator, b.numerator};
}

} // namespace test_support

/// The rational is exact: no operation rounds.
template <>
struct std::numeric_limits<test_support::rational> {
  static constexpr bool is_specialized = true;
  static constexpr bool is_exact = true;
};

#endif // GALOOT_TEST_SUPPORT_RATIONAL_HPP
