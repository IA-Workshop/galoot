#ifndef GALOOT_TEST_SUPPORT_COUNTING_HPP
#define GALOOT_TEST_SUPPORT_COUNTING_HPP

// A scalar for tests that counts, at run time, the arithmetic made on it, so
// that a test can compare what an operation computes with the terms it is
// specified to compute.
//
// Each kind is counted apart:
//   multiplications          a * b
//   additions                a + b
//   subtractions             a - b
//   negations                -a
//   comparisons              a == b, and a != b, which C++ rewrites as ==
//   integer_constructions    counting{c} from a std::int64_t c
//   default_constructions    counting{}, the zero
// A copy or an assignment is not an operation and counts nothing, and
// neither does reading `value`.
//
// The value is an exact integer, and numeric_limits declare the type exact,
// so a library that treats exact scalars apart (checking that what it drops
// is zero, say) treats this one so. Inputs must be small enough that no
// result leaves std::int64_t; nothing checks this.
//
// The counts are one tally for the program, read with counts() and set to
// zero with reset_counts(). It is test state for one thread, and an
// operation evaluated at compile time cannot count, so no operation is
// constexpr.

#include <cstdint>
#include <limits>

namespace test_support {

/// The number of operations of each kind made on counting values.
struct operation_counts {
  int multiplications = 0;
  int additions = 0;
  int subtractions = 0;
  int negations = 0;
  int comparisons = 0;
  int integer_constructions = 0;
  int default_constructions = 0;
};

namespace detail {

// The program's tally. A function's static, so that the header can be
// included in several files of one program without a second definition.
inline operation_counts& tally() noexcept {
  static operation_counts counts;
  return counts;
}

} // namespace detail

/// The operations counted since the start of the program or the last
/// reset_counts().
[[nodiscard]] inline operation_counts counts() noexcept {
  return detail::tally();
}

/// Sets every count to zero.
inline void reset_counts() noexcept { detail::tally() = {}; }

/// An exact integer that counts the operations made on it, as above.
///
/// Guarantees: each operation gives the integer result of the same
/// operation on the values, and adds one to the count of its own kind and
/// to no other. Semiregular, and constructible from the integer
/// coefficients a library converts to its scalar type.
struct counting {
  std::int64_t value = 0;

  /// Zero; counts a default construction.
  counting() noexcept { ++detail::tally().default_constructions; }

  /// The integer v; counts a construction from an integer.
  explicit counting(std::int64_t v) noexcept : value(v) {
    ++detail::tally().integer_constructions;
  }

  /// a + b; counts an addition.
  friend counting operator+(const counting& a, const counting& b) noexcept {
    ++detail::tally().additions;
    return of(a.value + b.value);
  }

  /// a - b; counts a subtraction.
  friend counting operator-(const counting& a, const counting& b) noexcept {
    ++detail::tally().subtractions;
    return of(a.value - b.value);
  }

  /// a * b; counts a multiplication.
  friend counting operator*(const counting& a, const counting& b) noexcept {
    ++detail::tally().multiplications;
    return of(a.value * b.value);
  }

  /// -a; counts a negation.
  friend counting operator-(const counting& a) noexcept {
    ++detail::tally().negations;
    return of(-a.value);
  }

  /// Whether a and b hold the same integer; counts a comparison.
  friend bool operator==(const counting& a, const counting& b) noexcept {
    ++detail::tally().comparisons;
    return a.value == b.value;
  }

private:
  // A result of an operation, which is not a construction from an integer
  // and counts nothing beyond the operation.
  struct result_tag {};
  counting(result_tag /*tag*/, std::int64_t v) noexcept : value(v) {}
  static counting of(std::int64_t v) noexcept { return {result_tag{}, v}; }
};

} // namespace test_support

/// counting is exact, as an integer is.
template <>
struct std::numeric_limits<test_support::counting> {
  static constexpr bool is_specialized = true;
  static constexpr bool is_exact = true;
};

#endif // GALOOT_TEST_SUPPORT_COUNTING_HPP
