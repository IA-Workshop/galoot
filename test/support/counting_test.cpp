// Tests of the counting scalar in support/counting.hpp: that it is an exact
// ring scalar with ==, so that the library takes the paths it takes for an
// exact scalar, and that each of its operations computes the integer result
// and adds one to its own count and to no other.
#include <concepts>
#include <cstdint>
#include <limits>

#include <galoot/core/mv.hpp>

#include <doctest/doctest.h>

#include <support/counting.hpp>

namespace {

using test_support::counting;
using test_support::operation_counts;

// Each count of seen against the same count of expected, one check each, so
// that a failure names the kind that differs.
void check_counts(const operation_counts& seen,
                  const operation_counts& expected) {
  CHECK(seen.multiplications == expected.multiplications);
  CHECK(seen.additions == expected.additions);
  CHECK(seen.subtractions == expected.subtractions);
  CHECK(seen.negations == expected.negations);
  CHECK(seen.comparisons == expected.comparisons);
  CHECK(seen.integer_constructions == expected.integer_constructions);
  CHECK(seen.default_constructions == expected.default_constructions);
}

} // namespace

// Reference: contract: a multivector over an exact scalar with == checks
//   what it drops, and its numeric_limits saying is_exact is what makes a
//   scalar exact, so the counting scalar declares itself exact and has ==,
//   as integer arithmetic is
TEST_CASE("counting is an exact ring scalar with ==") {
  static_assert(galoot::core::ring_scalar<counting>);
  static_assert(galoot::core::exact_scalar<counting>);
  static_assert(std::equality_comparable<counting>);
  CHECK(std::numeric_limits<counting>::is_exact);
}

// Reference: proof: each operation of the scalar is one integer operation
//   on its operands' values, and adds one to the count of its own kind,
//   while a copy is not an operation and counts nothing
TEST_CASE("each operation of counting computes its integer result and adds "
          "one to its own count") {
  const counting a{7};
  const counting b{-3};
  const counting also_seven{7};

  test_support::reset_counts();
  const counting sum = a + b;
  check_counts(test_support::counts(), {.additions = 1});
  CHECK(sum.value == 4);

  test_support::reset_counts();
  const counting difference = a - b;
  check_counts(test_support::counts(), {.subtractions = 1});
  CHECK(difference.value == 10);

  test_support::reset_counts();
  const counting product = a * b;
  check_counts(test_support::counts(), {.multiplications = 1});
  CHECK(product.value == -21);

  test_support::reset_counts();
  const counting negated = -a;
  check_counts(test_support::counts(), {.negations = 1});
  CHECK(negated.value == -7);

  test_support::reset_counts();
  const bool equal = a == b;
  const bool equal_to_seven = a == also_seven;
  check_counts(test_support::counts(), {.comparisons = 2});
  CHECK_FALSE(equal);
  CHECK(equal_to_seven);

  test_support::reset_counts();
  const counting from_integer{std::int64_t{5}};
  check_counts(test_support::counts(), {.integer_constructions = 1});
  CHECK(from_integer.value == 5);

  test_support::reset_counts();
  const counting zero{};
  check_counts(test_support::counts(), {.default_constructions = 1});
  CHECK(zero.value == 0);

  test_support::reset_counts();
  const counting copy = a;
  counting assigned = a;
  assigned = b;
  check_counts(test_support::counts(), {});
  CHECK(copy.value == 7);
  CHECK(assigned.value == -3);
}

// Reference: contract: reset_counts sets every count to zero, and the counts
//   then grow only with the operations made after it
TEST_CASE("reset_counts sets every count to zero") {
  const counting a{2};
  const counting b = (a * a) + (a - (-a));
  static_cast<void>(b == a);
  test_support::reset_counts();
  check_counts(test_support::counts(), {});

  const counting c = (b * b) - a;
  check_counts(test_support::counts(),
               {.multiplications = 1, .subtractions = 1});
  CHECK(c.value == 62);
}
