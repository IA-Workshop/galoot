// Tests of the straight-line reference kernels that the benchmarks time the
// rga3 operations against: on seeded operands, each kernel gives exactly what
// its operation gives over the test rational, and over double the same
// values, or for a transform the same bits.
//
// The kernels come from test/reference/reference_kernels.py, which writes
// each operation's terms from the definitions: the products of one
// coefficient of each operand, with their signs, in order of the first
// operand's coefficient and then the second's. A point is stored on
// e1 e2 e3 e4, a line on e41 e42 e43 e23 e31 e12, a plane on
// e423 e431 e412 e321, and a motor on e41 e42 e43 e1234 e23 e31 e12 1, where
// e41 = -e14 and e321 = -e123.
//
// Over double the tolerance is zero: equal values for the products and
// compose, and equal bits for the transforms. It is fixed by the following
// derivation, not by a run. The library's kernels form the same products as the
// reference kernels and add them in the same order, assigning each output's
// first term and adding or subtracting each later one. A transform writes into
// x's layout with every sign folded into its terms, as the reference kernel
// does, so the two evaluate the same operations and give the same bits. A
// product is computed on its blades in increasing order, e14 rather than e41,
// and then moved to the layout of the named type, negated where the
// orientations differ, while the reference kernel folds that sign into the
// terms. Rounding to nearest is symmetric, fl(-a) = -fl(a), so each partial sum
// is the library's or its negation and the results are equal, though a zero may
// differ in sign, which == ignores. The test builds use no contraction of
// multiply and add, -ffp-contract=off, so each product is rounded on its own
// on both sides.
//
// The double cases draw operands of two kinds: uniform in [-1, 1), which
// test the order of the terms, since a sum added in another order rounds
// differently, and small integers in {-1, 0, 1}, whose products and sums are
// exact and often zero, so that the sign of a zero tells a transform's equal
// bits from merely equal values.
#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <random>
#include <tuple>
#include <type_traits>

#include <galoot/presets/rga3.hpp>

#include <doctest/doctest.h>

#include <bench/reference_kernels.hpp>
#include <support/rational.hpp>

namespace {

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using test_support::rational;

constexpr int rational_cases = 100;
constexpr int double_cases = 1000; // of each kind of draw

// The kinds of draw of a double coefficient.
enum class Draw : std::uint8_t { uniform, small_integer };

// One coefficient drawn from rng. A rational has a numerator in [-9, 9] and a
// denominator in [1, 4], so that no sum of products of three of them leaves
// std::int64_t. A uniform double lies in [-1, 1), from the generator's top 53
// bits, so the draw is the same with every standard library: std::mt19937_64
// is specified in full. A small integer is -1, 0, or 1.
template <class T>
T draw(std::mt19937_64& rng, Draw kind) {
  if constexpr (std::same_as<T, rational>) {
    const auto numerator = static_cast<std::int64_t>(rng() % 19) - 9;
    const auto denominator = static_cast<std::int64_t>(rng() % 4) + 1;
    return rational{numerator, denominator};
  } else if (kind == Draw::small_integer) {
    return static_cast<double>(static_cast<int>(rng() % 3) - 1);
  } else {
    return (static_cast<double>(rng() >> 11U) * 0x1p-52) - 1.0;
  }
}

template <class Mv>
Mv drawn(std::mt19937_64& rng, Draw kind) {
  Mv x;
  for (auto& c : x.coefficients) {
    c = draw<typename Mv::scalar_type>(rng, kind);
  }
  return x;
}

// How two results must agree: equal values, or, over double, equal bits.
enum class Agreement : std::uint8_t { values, bits };

template <Agreement A, class T, std::size_t N>
bool agree(const std::array<T, N>& a, const std::array<T, N>& b) {
  if constexpr (A == Agreement::bits && std::same_as<T, double>) {
    using Bits = std::array<std::uint64_t, N>;
    return std::bit_cast<Bits>(a) == std::bit_cast<Bits>(b);
  } else {
    return a == b;
  }
}

// Draws `cases` sets of operands of the types Operands, in order, from
// doctest's seed (--rand-seed), and checks that the kernel gives on their
// coefficients what the operation gives on them. Over double it does so for
// each kind of draw.
template <Agreement A, class... Operands>
void check_cases(int cases, const auto& kernel, const auto& operation) {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 rng(seed);
  constexpr bool exact =
      (std::same_as<typename Operands::scalar_type, rational> && ...);
  for (const Draw kind : {Draw::uniform, Draw::small_integer}) {
    if (exact && kind == Draw::small_integer) {
      break;
    }
    CAPTURE(kind == Draw::small_integer);
    for (int k = 0; k < cases; ++k) {
      CAPTURE(k);
      const std::tuple<Operands...> operands{drawn<Operands>(rng, kind)...};
      const auto expected = std::apply(operation, operands).coefficients;
      const auto found = std::apply(
          [&](const Operands&... x) { return kernel(x.coefficients...); },
          operands);
      CHECK(agree<A>(found, expected));
    }
  }
}

template <class T>
using Point = rga3::Point<T>;
template <class T>
using Line = rga3::Line<T>;
template <class T>
using Plane = rga3::Plane<T>;
template <class T>
using Motor = rga3::Motor<T>;

constexpr auto join = [](const auto& p, const auto& q) {
  using T = std::remove_cvref_t<decltype(p)>::scalar_type;
  return Line<T>{p ^ q};
};
constexpr auto join_of_three = [](const auto& p, const auto& q, const auto& r) {
  using T = std::remove_cvref_t<decltype(p)>::scalar_type;
  return Plane<T>{(p ^ q) ^ r};
};
constexpr auto meet = [](const auto& l, const auto& g) {
  using T = std::remove_cvref_t<decltype(l)>::scalar_type;
  return Point<T>{l & g};
};
constexpr auto transform = [](const auto& x, const auto& q) {
  return core::transform(x, q);
};
constexpr auto compose = [](const auto& a, const auto& b) {
  using T = std::remove_cvref_t<decltype(a)>::scalar_type;
  return Motor<T>{core::compose(a, b)};
};

} // namespace

// Reference: independent value: the kernel holds the products of p's and q's
//   coefficients that the wedge of the two points gives by its definition,
//   written by a script that shares no code with the library and checked
//   there against the Pluecker coordinates w_p q - w_q p and p x q, so the
//   rga3 Line{p ^ q} of seeded points over the test rational is the kernel's
TEST_CASE("the reference kernel of p ^ q gives the rga3 Line{p ^ q} of "
          "seeded points over the test rational") {
  check_cases<Agreement::values, Point<rational>, Point<rational>>(
      rational_cases, reference_kernels::wedge_point_point<rational>, join);
}

// Reference: independent value: the kernel of p ^ q written from the
//   definitions, so over double the rga3 Line{p ^ q} of seeded points equals
//   the kernel's, with a tolerance of zero because the two add the same
//   products in the same order and moving an orientation's sign from a sum
//   into its terms changes no value under rounding to nearest
TEST_CASE("the reference kernel of p ^ q gives the values of the rga3 "
          "Line{p ^ q} of seeded points over double") {
  check_cases<Agreement::values, Point<double>, Point<double>>(
      double_cases, reference_kernels::wedge_point_point<double>, join);
}

// Reference: independent value: the kernel joins p and q on the bivectors and
//   that with r, each term from the definition of the wedge, written by a
//   script that shares no code with the library and checked there against
//   the cofactors det[e_j, p, q, r], so the rga3 Plane{(p ^ q) ^ r} of seeded
//   points over the test rational is the kernel's
TEST_CASE("the reference kernel of p ^ q ^ r gives the rga3 Plane{p ^ q ^ r} "
          "of seeded points over the test rational") {
  check_cases<Agreement::values, Point<rational>, Point<rational>,
              Point<rational>>(
      rational_cases, reference_kernels::wedge_point_point_point<rational>,
      join_of_three);
}

// Reference: independent value: the kernel of p ^ q ^ r written from the
//   definitions, so over double the rga3 Plane{p ^ q ^ r} of seeded points
//   equals the kernel's, with a tolerance of zero because the two form the
//   same join p ^ q on the bivectors, add the same products with r in the
//   same order, and moving an orientation's sign from a sum into its terms
//   changes no value under rounding to nearest
TEST_CASE("the reference kernel of p ^ q ^ r gives the values of the rga3 "
          "Plane{p ^ q ^ r} of seeded points over double") {
  check_cases<Agreement::values, Point<double>, Point<double>, Point<double>>(
      double_cases, reference_kernels::wedge_point_point_point<double>,
      join_of_three);
}

// Reference: independent value: the kernel holds the products of l's and g's
//   coefficients that the antiwedge gives by its definition, written by a
//   script that shares no code with the library and checked there against
//   (g . p) q - (g . q) p for the line through p and q, so the rga3
//   Point{l & g} of a seeded line and plane over the test rational is the
//   kernel's
TEST_CASE("the reference kernel of l & g gives the rga3 Point{l & g} of a "
          "seeded line and plane over the test rational") {
  check_cases<Agreement::values, Line<rational>, Plane<rational>>(
      rational_cases, reference_kernels::antiwedge_line_plane<rational>, meet);
}

// Reference: independent value: the kernel of l & g written from the
//   definitions, so over double the rga3 Point{l & g} of a seeded line and
//   plane equals the kernel's, with a tolerance of zero because the two add
//   the same products of l's and g's coefficients in the same order
TEST_CASE("the reference kernel of l & g gives the values of the rga3 "
          "Point{l & g} of a seeded line and plane over double") {
  check_cases<Agreement::values, Line<double>, Plane<double>>(
      double_cases, reference_kernels::antiwedge_line_plane<double>, meet);
}

// Reference: independent value: the kernel computes Q v x v antireverse(Q),
//   the first antiproduct where the second reads it and the second on x's
//   blades, each term from the definitions, written by a script that shares
//   no code with the library and checked there against the whole sandwich,
//   so transform(x, Q) of a seeded point and motor over the test rational is
//   the kernel's
TEST_CASE("the reference kernel of transform(point, motor) gives the rga3 "
          "transform of a seeded point over the test rational") {
  check_cases<Agreement::values, Point<rational>, Motor<rational>>(
      rational_cases, reference_kernels::transform_point<rational>, transform);
}

// Reference: independent value: the kernel of the transform written from
//   the definitions, so over double transform(x, Q) of a seeded point and
//   motor has the kernel's bits, a tolerance of zero because the two form
//   the same products with the same signs and add them in the same order
TEST_CASE("the reference kernel of transform(point, motor) gives the bits of "
          "the rga3 transform of a seeded point over double") {
  check_cases<Agreement::bits, Point<double>, Motor<double>>(
      double_cases, reference_kernels::transform_point<double>, transform);
}

// Reference: independent value: the kernel computes Q v x v antireverse(Q),
//   the first antiproduct where the second reads it and the second on x's
//   blades, each term from the definitions, written by a script that shares
//   no code with the library and checked there against the whole sandwich,
//   so transform(x, Q) of a seeded line and motor over the test rational is
//   the kernel's
TEST_CASE("the reference kernel of transform(line, motor) gives the rga3 "
          "transform of a seeded line over the test rational") {
  check_cases<Agreement::values, Line<rational>, Motor<rational>>(
      rational_cases, reference_kernels::transform_line<rational>, transform);
}

// Reference: independent value: the kernel of the transform written from
//   the definitions, so over double transform(x, Q) of a seeded line and
//   motor has the kernel's bits, a tolerance of zero because the two form
//   the same products with the same signs and add them in the same order
TEST_CASE("the reference kernel of transform(line, motor) gives the bits of "
          "the rga3 transform of a seeded line over double") {
  check_cases<Agreement::bits, Line<double>, Motor<double>>(
      double_cases, reference_kernels::transform_line<double>, transform);
}

// Reference: independent value: the kernel computes Q v x v antireverse(Q),
//   the first antiproduct where the second reads it and the second on x's
//   blades, each term from the definitions, written by a script that shares
//   no code with the library and checked there against the whole sandwich,
//   so transform(x, Q) of a seeded plane and motor over the test rational is
//   the kernel's
TEST_CASE("the reference kernel of transform(plane, motor) gives the rga3 "
          "transform of a seeded plane over the test rational") {
  check_cases<Agreement::values, Plane<rational>, Motor<rational>>(
      rational_cases, reference_kernels::transform_plane<rational>, transform);
}

// Reference: independent value: the kernel of the transform written from
//   the definitions, so over double transform(x, Q) of a seeded plane and
//   motor has the kernel's bits, a tolerance of zero because the two form
//   the same products with the same signs and add them in the same order
TEST_CASE("the reference kernel of transform(plane, motor) gives the bits of "
          "the rga3 transform of a seeded plane over double") {
  check_cases<Agreement::bits, Plane<double>, Motor<double>>(
      double_cases, reference_kernels::transform_plane<double>, transform);
}

// Reference: independent value: the kernel holds the products of the two
//   motors' coefficients that their antiproduct gives by its definition,
//   written by a script that shares no code with the library and checked
//   there against the antiproduct of whole multivectors, so the rga3
//   Motor{compose(a, b)} of seeded motors over the test rational is the
//   kernel's
TEST_CASE("the reference kernel of compose gives the rga3 "
          "Motor{compose(a, b)} of seeded motors over the test rational") {
  check_cases<Agreement::values, Motor<rational>, Motor<rational>>(
      rational_cases, reference_kernels::compose_motors<rational>, compose);
}

// Reference: independent value: the kernel of compose written from the
//   definitions, so over double the rga3 Motor{compose(a, b)} of seeded
//   motors equals the kernel's, with a tolerance of zero because the two add
//   the same products in the same order and moving an orientation's sign
//   from a sum into its terms changes no value under rounding to nearest
TEST_CASE("the reference kernel of compose gives the values of the rga3 "
          "Motor{compose(a, b)} of seeded motors over double") {
  check_cases<Agreement::values, Motor<double>, Motor<double>>(
      double_cases, reference_kernels::compose_motors<double>, compose);
}
