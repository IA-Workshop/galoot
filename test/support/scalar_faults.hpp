#ifndef GALOOT_TEST_SUPPORT_SCALAR_FAULTS_HPP
#define GALOOT_TEST_SUPPORT_SCALAR_FAULTS_HPP

// Number types that each lack exactly one of the operations a ring scalar
// needs, named for the one they lack: a + b, a - b, a * b, -a, or
// construction from an integer coefficient. Each holds an integer, has a
// default constructor that makes zero, and copies, so it is semiregular and
// differs from a ring scalar only in the operation its name gives. The
// tests of the multivector use them to give each requirement a scalar that
// fails it alone.

#include <cstdint>

namespace test_support {

struct lacks_addition {
  std::int64_t value = 0;

  constexpr lacks_addition() = default;
  explicit constexpr lacks_addition(std::int64_t v) noexcept : value(v) {}

  [[maybe_unused]] friend constexpr lacks_addition
  operator-(lacks_addition a, lacks_addition b) noexcept {
    return lacks_addition{a.value - b.value};
  }
  [[maybe_unused]] friend constexpr lacks_addition
  operator*(lacks_addition a, lacks_addition b) noexcept {
    return lacks_addition{a.value * b.value};
  }
  [[maybe_unused]] friend constexpr lacks_addition
  operator-(lacks_addition a) noexcept {
    return lacks_addition{-a.value};
  }
};

struct lacks_subtraction {
  std::int64_t value = 0;

  constexpr lacks_subtraction() = default;
  explicit constexpr lacks_subtraction(std::int64_t v) noexcept : value(v) {}

  [[maybe_unused]] friend constexpr lacks_subtraction
  operator+(lacks_subtraction a, lacks_subtraction b) noexcept {
    return lacks_subtraction{a.value + b.value};
  }
  [[maybe_unused]] friend constexpr lacks_subtraction
  operator*(lacks_subtraction a, lacks_subtraction b) noexcept {
    return lacks_subtraction{a.value * b.value};
  }
  [[maybe_unused]] friend constexpr lacks_subtraction
  operator-(lacks_subtraction a) noexcept {
    return lacks_subtraction{-a.value};
  }
};

struct lacks_multiplication {
  std::int64_t value = 0;

  constexpr lacks_multiplication() = default;
  explicit constexpr lacks_multiplication(std::int64_t v) noexcept : value(v) {}

  [[maybe_unused]] friend constexpr lacks_multiplication
  operator+(lacks_multiplication a, lacks_multiplication b) noexcept {
    return lacks_multiplication{a.value + b.value};
  }
  [[maybe_unused]] friend constexpr lacks_multiplication
  operator-(lacks_multiplication a, lacks_multiplication b) noexcept {
    return lacks_multiplication{a.value - b.value};
  }
  [[maybe_unused]] friend constexpr lacks_multiplication
  operator-(lacks_multiplication a) noexcept {
    return lacks_multiplication{-a.value};
  }
};

struct lacks_unary_minus {
  std::int64_t value = 0;

  constexpr lacks_unary_minus() = default;
  explicit constexpr lacks_unary_minus(std::int64_t v) noexcept : value(v) {}

  [[maybe_unused]] friend constexpr lacks_unary_minus
  operator+(lacks_unary_minus a, lacks_unary_minus b) noexcept {
    return lacks_unary_minus{a.value + b.value};
  }
  [[maybe_unused]] friend constexpr lacks_unary_minus
  operator-(lacks_unary_minus a, lacks_unary_minus b) noexcept {
    return lacks_unary_minus{a.value - b.value};
  }
  [[maybe_unused]] friend constexpr lacks_unary_minus
  operator*(lacks_unary_minus a, lacks_unary_minus b) noexcept {
    return lacks_unary_minus{a.value * b.value};
  }
};

// The declared default constructor keeps this from being an aggregate,
// which could otherwise be made from an integer by parenthesized
// initialization of its member.
struct lacks_integer_construction {
  std::int64_t value = 0;

  constexpr lacks_integer_construction() = default;

  [[maybe_unused]] friend constexpr lacks_integer_construction
  operator+(lacks_integer_construction a,
            lacks_integer_construction b) noexcept {
    return of(a.value + b.value);
  }
  [[maybe_unused]] friend constexpr lacks_integer_construction
  operator-(lacks_integer_construction a,
            lacks_integer_construction b) noexcept {
    return of(a.value - b.value);
  }
  [[maybe_unused]] friend constexpr lacks_integer_construction
  operator*(lacks_integer_construction a,
            lacks_integer_construction b) noexcept {
    return of(a.value * b.value);
  }
  [[maybe_unused]] friend constexpr lacks_integer_construction
  operator-(lacks_integer_construction a) noexcept {
    return of(-a.value);
  }

private:
  static constexpr lacks_integer_construction of(std::int64_t v) noexcept {
    lacks_integer_construction out;
    out.value = v;
    return out;
  }
};

} // namespace test_support

#endif // GALOOT_TEST_SUPPORT_SCALAR_FAULTS_HPP
