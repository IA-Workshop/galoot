// Tests of the test-only rational scalar, test/support/rational.hpp, which
// other tests use as an exact oracle. The hand values are worked out on
// paper; the identities are the field axioms of the rationals, checked on
// seeded random fractions; equality is checked against cross-multiplication,
// computed here in plain integers.
#include <cstdint>
#include <format>
#include <limits>
#include <numeric>
#include <random>

#include <doctest/doctest.h>

#include <support/rational.hpp>

namespace {

using test_support::rational;

// Numerators and denominators of the random fractions are below 2^15 in
// magnitude. The deepest expression below, a b + a c, forms numerators of at
// most 2 * 2^30 * 2^30 = 2^61 and denominators of at most 2^60, so no
// intermediate overflows std::int64_t and every value is exact.
constexpr std::int64_t random_bound = 32768; // 2^15
constexpr int random_cases = 2000;

// A fraction with numerator and denominator drawn below random_bound, the
// denominator nonzero and of either sign.
rational random_fraction(std::mt19937_64& source) {
  std::uniform_int_distribution<std::int64_t> numerator(-random_bound + 1,
                                                        random_bound - 1);
  std::uniform_int_distribution<std::int64_t> denominator(1, random_bound - 1);
  std::bernoulli_distribution negative(0.5);
  const std::int64_t d = denominator(source);
  return rational{numerator(source), negative(source) ? -d : d};
}

// The members of a value written by hand, for the checks below.
constexpr bool has_members(rational x, std::int64_t numerator,
                           std::int64_t denominator) {
  return x.numerator == numerator && x.denominator == denominator;
}

static_assert(std::numeric_limits<rational>::is_specialized);
static_assert(std::numeric_limits<rational>::is_exact);

} // namespace

// Reference: independent value: fractions worked by hand, 1/3 + 1/6 = 1/2,
//   -2/4 = 1/-2 = -1/2, (2/3)(9/4) = 3/2, 1/2 - 1/2 = 0 = 0/1, and
//   (3/7) / (6/7) = 1/2, each kept in lowest terms with a positive
//   denominator
TEST_CASE("rational gives the hand values in lowest terms") {
  static_assert(has_members(rational{1, 3} + rational{1, 6}, 1, 2));
  static_assert(has_members(rational{-2, 4}, -1, 2));
  static_assert(has_members(rational{1, -2}, -1, 2));
  static_assert(rational{-2, 4} == rational{1, -2});
  static_assert(has_members(rational{2, 3} * rational{9, 4}, 3, 2));
  static_assert(has_members(rational{1, 2} - rational{1, 2}, 0, 1));
  static_assert(has_members(rational{3, 7} / rational{6, 7}, 1, 2));
  static_assert(has_members(-rational{3, 7}, -3, 7));
  static_assert(has_members(rational{}, 0, 1));
  static_assert(has_members(rational{0, -5}, 0, 1));

  // The same at run time, from values the compiler cannot know.
  volatile std::int64_t one = 1;
  const auto r = [&one](std::int64_t n, std::int64_t d) {
    return rational{n * one, d * one};
  };
  CHECK(has_members(r(1, 3) + r(1, 6), 1, 2));
  CHECK(has_members(r(-2, 4), -1, 2));
  CHECK(has_members(r(1, -2), -1, 2));
  CHECK(has_members(r(2, 3) * r(9, 4), 3, 2));
  CHECK(has_members(r(1, 2) - r(1, 2), 0, 1));
  CHECK(has_members(r(3, 7) / r(6, 7), 1, 2));
  CHECK(has_members(-r(3, 7), -3, 7));
  CHECK(has_members(r(0, -5), 0, 1));
}

// Reference: identity: the rationals form a field, so addition and
// multiplication are commutative and associative, multiplication distributes
// over addition, 0 and 1 are identities, a + (-a) = 0, a - b = a + (-b), and
// a (1 / a) = 1 for a nonzero a; proof: a fraction is in lowest terms with a
// positive denominator when gcd(n, d) = 1 and d > 0, checked here with
// std::gcd
TEST_CASE("rational satisfies the field identities on random fractions") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  const rational zero{};
  const rational one{1};

  for (int i = 0; i < random_cases; ++i) {
    const rational a = random_fraction(source);
    const rational b = random_fraction(source);
    const rational c = random_fraction(source);
    CAPTURE(i);
    INFO(std::format("a = {}/{}, b = {}/{}, c = {}/{}", a.numerator,
                     a.denominator, b.numerator, b.denominator, c.numerator,
                     c.denominator));

    CHECK(a + b == b + a);
    CHECK(a * b == b * a);
    CHECK((a + b) + c == a + (b + c));
    CHECK((a * b) * c == a * (b * c));
    CHECK(a * (b + c) == a * b + a * c);
    CHECK(a + zero == a);
    CHECK(a * one == a);
    CHECK(a * zero == zero);
    CHECK(a + (-a) == zero);
    CHECK(a - b == a + (-b));
    if (a != zero) {
      CHECK(a * (one / a) == one);
      CHECK((b / a) * a == b);
    }

    for (const rational x : {a, a + b, a * b, a - c, a * (b + c)}) {
      CHECK(x.denominator > 0);
      CHECK(std::gcd(x.numerator, x.denominator) == 1);
    }
  }
}

// Reference: identity: n1/d1 = n2/d2 exactly when n1 d2 = n2 d1, computed
//   here in plain integers on small fractions, so that equal pairs written
//   with different numerators and denominators occur often
TEST_CASE("rational equality agrees with cross-multiplication") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  std::uniform_int_distribution<std::int64_t> numerator(-6, 6);
  std::uniform_int_distribution<std::int64_t> denominator(1, 6);
  std::bernoulli_distribution negative(0.5);

  int equal_pairs = 0;
  for (int i = 0; i < random_cases; ++i) {
    const std::int64_t n1 = numerator(source);
    const std::int64_t n2 = numerator(source);
    const std::int64_t d1 =
        negative(source) ? -denominator(source) : denominator(source);
    const std::int64_t d2 =
        negative(source) ? -denominator(source) : denominator(source);
    CAPTURE(n1);
    CAPTURE(d1);
    CAPTURE(n2);
    CAPTURE(d2);
    const bool equal = n1 * d2 == n2 * d1;
    CHECK((rational{n1, d1} == rational{n2, d2}) == equal);
    equal_pairs += equal ? 1 : 0;
  }
  // Both answers must occur, or the check above could not fail.
  CHECK(equal_pairs > 0);
  CHECK(equal_pairs < random_cases);
}

// Reference: contract: a result within -max to max of std::int64_t is formed
//   exactly, at the largest sums and products that fit: (max - 1) + 1 = max,
//   -(max - 1) - 1 = -max, and 3037000499^2 = 9223372030926249001, the
//   largest square below max = 9223372036854775807, and a product whose parts
//   cancel, (2^62 / 3)(3 / 2^62) = 1, and a sum whose denominators share a
//   factor, 1/2^62 + 1/2^62 = 1/2^61
TEST_CASE("rational forms results up to the bounds of std::int64_t") {
  constexpr std::int64_t max = std::numeric_limits<std::int64_t>::max();
  static_assert(has_members(rational{max - 1} + rational{1}, max, 1));
  static_assert(has_members(rational{-(max - 1)} - rational{1}, -max, 1));
  static_assert(has_members(rational{3037000499} * rational{3037000499},
                            9223372030926249001, 1));
  static_assert(has_members(rational{1, max - 1} * rational{1, 1}, 1, max - 1));

  // (2^62 / 3)(3 / 2^62) = 1 only if each numerator is first divided by its
  // common factor with the other denominator: the products of the parts,
  // 3 x 2^62, do not fit.
  constexpr std::int64_t two_62 = 4611686018427387904; // 2^62
  static_assert(rational{two_62, 3} * rational{3, two_62} == rational{1});
  const volatile std::int64_t one = 1;
  CHECK(rational{two_62 * one, 3} * rational{3, two_62 * one} == rational{1});

  // 1/2^62 + 1/2^62 = 1/2^61 only if the denominators are first divided by
  // their common factor: their product, 2^124, does not fit.
  constexpr std::int64_t two_61 = 2305843009213693952; // 2^61
  static_assert(rational{1, two_62} + rational{1, two_62} ==
                rational{1, two_61});
  CHECK(rational{one, two_62 * one} + rational{one, two_62 * one} ==
        rational{1, two_61});
}
