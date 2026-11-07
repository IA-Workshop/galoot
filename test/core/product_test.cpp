// Tests of galoot/core/product.hpp: the geometric product and the wedge of
// two multivectors, and their antiproducts, as functions and as the
// operators * ^ % &.
//
// The expected values come from the exact blade-product tables of the
// reference: for the product of two basis blades, the table lists the terms;
// the wedge of two blades with no generator in common is the term of the
// table on the union of their generators, and any other wedge is zero. The
// antiproducts are composed from those tables and the complement tables of
// the reference, as left(right(a) ⟑ right(b)) and left(right(a) ∧ right(b)).
// The algebraic identities are checked on integer coefficients small enough
// that every sum is exact. The test cases that build an antiproduct of two
// full multivectors of conformal(3) are in product_conformal3_test.cpp.
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <random>
#include <span>
#include <type_traits>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>

#include <doctest/doctest.h>

#include "product_checks.hpp"
#include <reference/golden.hpp>

namespace {

namespace core = galoot::core;
using galoot_test::products::antiproduct_mismatches;
using galoot_test::products::cga2;
using galoot_test::products::cga3;
using galoot_test::products::random_integers;
using galoot_test::products::rga3;
using galoot_test::products::table_value;
using galoot_test::products::value_on;

// The number of blades, among all of them, on which the product or the wedge
// of the basis blades at positions i and j differs from the table. The wedge
// keeps the term on the union of two blades with no generator in common.
template <class X>
int pair_mismatches(const golden::product& entry, std::size_t i,
                    std::size_t j) {
  X x{};
  X y{};
  x.coefficients.at(i) = 1;
  y.coefficients.at(j) = 1;
  const auto gp = core::geometric_product(x, y);
  const auto w = core::wedge(x, y);
  int mismatches = 0;
  for (core::mask_t m = 0; m < X::component_count; ++m) {
    const std::int64_t expected_gp = table_value(entry, m);
    const bool disjoint = (entry.a & entry.b) == 0;
    const std::int64_t expected_wedge =
        disjoint && m == (entry.a | entry.b) ? expected_gp : 0;
    mismatches += value_on(gp, m) == expected_gp ? 0 : 1;
    mismatches += value_on(w, m) == expected_wedge ? 0 : 1;
  }
  return mismatches;
}

// The number of pairs of basis blades of the algebra, over all pairs, for
// which the product or the wedge differs from the table; a table entry out
// of its place counts as one.
template <class Alg, std::size_t N>
int table_mismatches(std::span<const golden::product> table) {
  using X = core::mv<Alg, core::full<N>, std::int64_t>;
  int mismatches = 0;
  for (std::size_t i = 0; i < X::component_count; ++i) {
    for (std::size_t j = 0; j < X::component_count; ++j) {
      const core::mask_t a = X::layout.blades.at(i).mask;
      const core::mask_t b = X::layout.blades.at(j).mask;
      const golden::product& entry = table[(a * X::component_count) + b];
      mismatches +=
          entry.a == a && entry.b == b ? pair_mismatches<X>(entry, i, j) : 1;
    }
  }
  return mismatches;
}

template <class X, class Y>
concept can_multiply = requires(X x, Y y) { x * y; };
template <class X, class Y>
concept can_wedge = requires(X x, Y y) { x ^ y; };
template <class X, class Y>
concept can_call_geometric_product =
    requires(X x, Y y) { core::geometric_product(x, y); };

template <class X, class Y>
concept can_antimultiply = requires(X x, Y y) { x % y; };
template <class X, class Y>
concept can_antiwedge = requires(X x, Y y) { x & y; };
template <class X, class Y>
concept can_call_geometric_antiproduct =
    requires(X x, Y y) { core::geometric_antiproduct(x, y); };
template <class X, class Y>
concept can_call_antiwedge = requires(X x, Y y) { core::antiwedge(x, y); };

} // namespace

// A scope that sees galoot::core's operators by a using-directive, as a
// user's code may, with types of its own: one without operators and one
// with its own % and &. The library's operators must leave both alone.
namespace {
namespace user_code {

using namespace galoot::core;

struct widget {};

struct gadget {
  friend int operator%(gadget /*a*/, gadget /*b*/) { return 1; }
  friend int operator&(gadget /*a*/, gadget /*b*/) { return 2; }
  friend int operator*(gadget /*a*/, gadget /*b*/) { return 3; }
  friend int operator^(gadget /*a*/, gadget /*b*/) { return 4; }
};

template <class X, class Y>
concept has_percent = requires(X x, Y y) { x % y; };
template <class X, class Y>
concept has_ampersand = requires(X x, Y y) { x & y; };
template <class X, class Y>
concept has_times = requires(X x, Y y) { x * y; };
template <class X, class Y>
concept has_caret = requires(X x, Y y) { x ^ y; };

} // namespace user_code
} // namespace

// Reference: comparison: every pair of basis blades of rigid(3),
// conformal(2), and conformal(3)
//   against the exact tables, which satisfy e_i e_j + e_j e_i = 2 g_ij, for
//   the product and the wedge, covering the shared-generator, sign, and zero
//   cases, in a diagonal form and in forms that are not diagonal with an even
//   and an odd number of generators
TEST_CASE("geometric_product and wedge of basis blades equal the tables") {
  REQUIRE(cga2::value.gram == golden::gp_tables::conformal_2_gram);
  CHECK(table_mismatches<rga3, 4>(golden::gp_tables::rigid_3) == 0);
  CHECK(table_mismatches<cga2, 4>(golden::gp_tables::conformal_2) == 0);
  CHECK(table_mismatches<cga3, 5>(golden::gp_tables::conformal_3) == 0);
}

// Reference: independent value: by hand, in rigid(3), e1 e1 = 1, e1 e2 = e12,
//   e2 e1 = -e12, e4 e4 = 0, and e1 ^ e1 = 0
TEST_CASE("products of two vectors") {
  constexpr auto vectors = core::of_grade<4, 1>;
  using V = core::mv<rga3, vectors, std::int64_t>;
  const V e1{1, 0, 0, 0};
  const V e2{0, 1, 0, 0};
  const V e4{0, 0, 0, 1};
  CHECK(value_on(core::geometric_product(e1, e1), 0) == 1);
  CHECK(value_on(core::geometric_product(e1, e2), 0b0011) == 1);
  CHECK(value_on(core::geometric_product(e2, e1), 0b0011) == -1);
  CHECK(value_on(core::geometric_product(e4, e4), 0) == 0);
  CHECK(value_on(core::wedge(e1, e2), 0b0011) == 1);
  CHECK(value_on(core::wedge(e2, e1), 0b0011) == -1);
  CHECK(value_on(core::wedge(e1, e1), 0b0011) == 0);
}

// Reference: identity: a * b is the geometric product geometric_product(a, b)
//   and a ^ b is the wedge, each with the type and the value of the named
//   function
TEST_CASE("the operators are the named functions") {
  using X = core::mv<rga3, core::full<4>, double>;
  using L = core::mv<rga3, core::of_grade<4, 2>, double>;
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  std::uniform_real_distribution<double> any(-2.0, 2.0);
  const auto random_value = [&]<class Type>(std::type_identity<Type>) {
    Type x{};
    for (auto& c : x.coefficients) {
      c = any(source);
    }
    return x;
  };
  for (int k = 0; k < 8; ++k) {
    const X a = random_value(std::type_identity<X>{});
    const X b = random_value(std::type_identity<X>{});
    const L l = random_value(std::type_identity<L>{});
    static_assert(
        std::same_as<decltype(a * b), decltype(core::geometric_product(a, b))>);
    static_assert(std::same_as<decltype(a ^ b), decltype(core::wedge(a, b))>);
    static_assert(
        std::same_as<decltype(l * a), decltype(core::geometric_product(l, a))>);
    static_assert(std::same_as<decltype(l ^ a), decltype(core::wedge(l, a))>);
    CHECK((a * b) == core::geometric_product(a, b));
    CHECK((a ^ b) == core::wedge(a, b));
    CHECK((l * a) == core::geometric_product(l, a));
    CHECK((l ^ a) == core::wedge(l, a));
  }
}

// Reference: contract: the result of a product has the blades of the plan's
//   result set and the scalar type of the operands
TEST_CASE("the result has the layout the plan gives") {
  using L = core::mv<rga3, core::of_grade<4, 2>, std::int64_t>;
  using V = core::mv<rga3, core::of_grade<4, 1>, std::int64_t>;
  static_assert(
      std::same_as<decltype(core::wedge(V{}, V{})),
                   core::mv<rga3, core::of_grade<4, 2>, std::int64_t>>);
  static_assert(std::same_as<decltype(L{} * V{}),
                             decltype(core::geometric_product(L{}, V{}))>);
}

// Reference: independent value: by hand from the definitions, the result of a
//   product holds the blades it reaches, each with sign +1, in order of grade
//   and then of index list: two vectors of rigid(3) wedge to the six
//   bivectors and multiply to the scalar and the six bivectors, and two lines
//   multiply to the scalar, the six bivectors, and e1234, whatever the
//   orientations of the operands' blades
TEST_CASE("the result of a product is the canonical set of its blades") {
  using Point = core::mv<rga3, core::blades<rga3, "e1 e2 e3 e4">, std::int64_t>;
  using Line = core::mv<rga3, core::blades<rga3, "e41 e42 e43 e23 e31 e12">,
                        std::int64_t>;
  using Bivectors = core::mv<rga3, core::of_grade<4, 2>, std::int64_t>;
  using Even = core::mv<rga3, core::blades<rga3, "1 e12 e13 e14 e23 e24 e34">,
                        std::int64_t>;
  using EvenFull =
      core::mv<rga3, core::blades<rga3, "1 e12 e13 e14 e23 e24 e34 e1234">,
               std::int64_t>;
  const Point p{1, 0, 0, 1};
  const Point q{0, 1, 0, 1};
  const Line l{0, 0, 1, 0, 0, 0};
  static_assert(std::same_as<decltype(p ^ q), Bivectors>);
  static_assert(std::same_as<decltype(core::wedge(p, q)), Bivectors>);
  static_assert(std::same_as<decltype(p * q), Even>);
  static_assert(std::same_as<decltype(core::geometric_product(p, q)), Even>);
  static_assert(std::same_as<decltype(l * l), EvenFull>);
  static_assert(
      std::same_as<decltype(core::geometric_product(l, l)), EvenFull>);
  // The values on the canonical blades: p ^ q = (e1 + e4) ^ (e2 + e4) =
  // e12 + e14 - e24, since e4 ^ e2 = -e24 and e4 ^ e4 = 0.
  CHECK(value_on(p ^ q, 0b0011) == 1);
  CHECK(value_on(p ^ q, 0b1001) == 1);
  CHECK(value_on(p ^ q, 0b1010) == -1);
}

// Reference: identity: associativity of the geometric product and of the wedge
//   on random integer multivectors of rigid(3) and conformal(3), whose sums
//   are exact, and the wedge of a vector with itself is zero
TEST_CASE("the products are associative and a vector wedges to zero") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  using R = core::mv<rga3, core::full<4>, std::int64_t>;
  using C = core::mv<cga3, core::full<5>, std::int64_t>;
  using RV = core::mv<rga3, core::of_grade<4, 1>, std::int64_t>;
  using CV = core::mv<cga3, core::of_grade<5, 1>, std::int64_t>;
  for (int k = 0; k < 6; ++k) {
    const R a = random_integers<R>(source);
    const R b = random_integers<R>(source);
    const R c = random_integers<R>(source);
    CHECK(((a * b) * c) == (a * (b * c)));
    CHECK(((a ^ b) ^ c) == (a ^ (b ^ c)));
    const C d = random_integers<C>(source);
    const C e = random_integers<C>(source);
    const C f = random_integers<C>(source);
    CHECK(((d * e) * f) == (d * (e * f)));
    CHECK(((d ^ e) ^ f) == (d ^ (e ^ f)));
    const RV v = random_integers<RV>(source);
    const CV w = random_integers<CV>(source);
    CHECK((v ^ v) == decltype(v ^ v){});
    CHECK((w ^ w) == decltype(w ^ w){});
  }
}
// Reference: contract: a mixed call has the catch-all as its only candidate and
//   a requires-expression answers yes for it, as for get, while the valid
//   overloads are not captured and the operators of other types are untouched
TEST_CASE("the overloads take the right operands") {
  using R = core::mv<rga3, core::full<4>, double>;
  using F = core::mv<rga3, core::full<4>, float>;
  using C = core::mv<cga3, core::full<5>, double>;
  static_assert(can_multiply<R, R>);
  static_assert(can_multiply<R, F>);
  static_assert(can_multiply<R, C>);
  static_assert(can_wedge<R, C>);
  static_assert(can_call_geometric_product<R, F>);
  static_assert(!can_multiply<R, double>);
  static_assert(!can_multiply<double, R>);
  static_assert(!can_wedge<R, int>);
  static_assert(core::detail::product_operands<R, R>);
  static_assert(!core::detail::product_operands<R, F>);
  static_assert(!core::detail::product_operands<R, C>);
  static_assert(!core::detail::product_operands<R, double>);
  static_assert(core::detail::mismatched_operands<R, F>);
  static_assert(core::detail::mismatched_operands<R, C>);
  static_assert(!core::detail::mismatched_operands<R, R>);
  static_assert(!core::detail::mismatched_operands<R, double>);
  static_assert(core::detail::same_algebra_v<R, F>);
  static_assert(!core::detail::same_algebra_v<R, C>);
  static_assert(core::detail::same_scalar_v<R, C>);
  static_assert(!core::detail::same_scalar_v<R, F>);
}

// Reference: independent value: the same composition for every pair of the
//   sixteen blades of rigid(3) stored under their oriented names, where an
//   oriented name lists generators in the order of their product, so that a
//   coefficient on e31 = -e13 multiplies -e13, with the exact product table,
//   which satisfies e_i e_j + e_j e_i = 2 g_ij, and the complement table,
//   which satisfies u ^ right(u) = antiscalar and left(u) ^ u = antiscalar
TEST_CASE("the antiproducts of oriented blades carry their orientations") {
  using Oriented = core::mv<
      rga3,
      core::blades<rga3, "1 e1 e2 e3 e4 e23 e31 e12 e41 e42 e43 e423 e431 "
                         "e412 e321 e1234">,
      std::int64_t>;
  const auto& names = golden::oriented_names::rigid_3;
  REQUIRE(names.size() == Oriented::component_count);
  for (std::size_t k = 0; k < names.size(); ++k) {
    CAPTURE(names.at(k).name);
    REQUIRE(Oriented::layout.blades.at(k).mask == names.at(k).blade.blade);
    REQUIRE(Oriented::layout.blades.at(k).sign == names.at(k).blade.sign);
  }
  CHECK(antiproduct_mismatches<Oriented>(golden::gp_tables::rigid_3,
                                         golden::complements::n4) == 0);
}

// Reference: independent value: by hand in rigid(3), a point
//   x e1 + y e2 + z e3 + w e4 and a plane
//   gx e423 + gy e431 + gz e412 + gw e321 meet in the scalar
//   p ∨ g = gx x + gy y + gz z + gw w, and g ∨ p is its negation, since
//   a ∨ b = (-1)^(ag(a) ag(b)) b ∨ a and the antigrades are 3 and 1
TEST_CASE("the antiwedge of a point and a plane is the plane at the point") {
  using Point = core::mv<rga3, core::blades<rga3, "e1 e2 e3 e4">, std::int64_t>;
  using Plane =
      core::mv<rga3, core::blades<rga3, "e423 e431 e412 e321">, std::int64_t>;
  const Point p{2, 3, 5, 7};
  const Plane g{11, 13, 17, 19};
  const std::int64_t at_p = (11 * 2) + (13 * 3) + (17 * 5) + (19 * 7);
  static_assert(
      std::same_as<decltype(p & g),
                   core::mv<rga3, core::of_grade<4, 0>, std::int64_t>>);
  CHECK(value_on(p & g, 0) == at_p);
  CHECK(value_on(core::antiwedge(p, g), 0) == at_p);
  CHECK(value_on(g & p, 0) == -at_p);
}

// Reference: identity: a % b is the geometric antiproduct
//   geometric_antiproduct(a, b) and a & b is the antiwedge antiwedge(a, b),
//   each with the type and the value of the named function
TEST_CASE("the antiproduct operators are the named functions") {
  using X = core::mv<rga3, core::full<4>, double>;
  using L = core::mv<rga3, core::of_grade<4, 2>, double>;
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  std::uniform_real_distribution<double> any(-2.0, 2.0);
  const auto random_value = [&]<class Type>(std::type_identity<Type>) {
    Type x{};
    for (auto& c : x.coefficients) {
      c = any(source);
    }
    return x;
  };
  for (int k = 0; k < 8; ++k) {
    const X a = random_value(std::type_identity<X>{});
    const X b = random_value(std::type_identity<X>{});
    const L l = random_value(std::type_identity<L>{});
    static_assert(std::same_as<decltype(a % b),
                               decltype(core::geometric_antiproduct(a, b))>);
    static_assert(
        std::same_as<decltype(a & b), decltype(core::antiwedge(a, b))>);
    static_assert(std::same_as<decltype(l % a),
                               decltype(core::geometric_antiproduct(l, a))>);
    static_assert(
        std::same_as<decltype(l & a), decltype(core::antiwedge(l, a))>);
    CHECK((a % b) == core::geometric_antiproduct(a, b));
    CHECK((a & b) == core::antiwedge(a, b));
    CHECK((l % a) == core::geometric_antiproduct(l, a));
    CHECK((l & a) == core::antiwedge(l, a));
  }
}

// Reference: independent value: by hand from the definitions in rigid(3),
//   the result of an antiproduct holds the blades it reaches, each with
//   sign +1, in order of grade and then of index list: two planes meet in
//   the six bivectors, a line and a plane in the four vectors, a point and a
//   plane in the scalar, two motors give the scalar, the six bivectors, and
//   e1234, and two points give e12, e13, e23, and e1234, because the
//   complements of points are trivectors and e4 e4 = 0 removes every product
//   of two of them that shares e4
TEST_CASE("the result of an antiproduct is the canonical set of its blades") {
  using Point = core::mv<rga3, core::blades<rga3, "e1 e2 e3 e4">, std::int64_t>;
  using Line = core::mv<rga3, core::blades<rga3, "e41 e42 e43 e23 e31 e12">,
                        std::int64_t>;
  using Plane =
      core::mv<rga3, core::blades<rga3, "e423 e431 e412 e321">, std::int64_t>;
  using Motor =
      core::mv<rga3, core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">,
               std::int64_t>;
  using Full = core::mv<rga3, core::full<4>, std::int64_t>;
  using Scalar = core::mv<rga3, core::of_grade<4, 0>, std::int64_t>;
  using Vectors = core::mv<rga3, core::of_grade<4, 1>, std::int64_t>;
  using Bivectors = core::mv<rga3, core::of_grade<4, 2>, std::int64_t>;
  using Even =
      core::mv<rga3, core::blades<rga3, "1 e12 e13 e14 e23 e24 e34 e1234">,
               std::int64_t>;
  using PointPoint =
      core::mv<rga3, core::blades<rga3, "e12 e13 e23 e1234">, std::int64_t>;
  const Point p{};
  const Line l{};
  const Plane g{};
  const Motor m{};
  const Full f{};
  static_assert(std::same_as<decltype(g & g), Bivectors>);
  static_assert(std::same_as<decltype(core::antiwedge(g, g)), Bivectors>);
  static_assert(std::same_as<decltype(l & g), Vectors>);
  static_assert(std::same_as<decltype(core::antiwedge(l, g)), Vectors>);
  static_assert(std::same_as<decltype(p & g), Scalar>);
  static_assert(std::same_as<decltype(m % m), Even>);
  static_assert(
      std::same_as<decltype(core::geometric_antiproduct(m, m)), Even>);
  static_assert(std::same_as<decltype(p % p), PointPoint>);
  static_assert(
      std::same_as<decltype(core::geometric_antiproduct(p, p)), PointPoint>);
  static_assert(std::same_as<decltype(f % f), Full>);
  static_assert(std::same_as<decltype(f & f), Full>);
  // A value on a blade of each: e423 ∨ e431 = e34, from right(e423) = -e1,
  // right(e431) = -e2, and left(e12) = e34.
  const Plane x_plane{1, 0, 0, 0};
  const Plane y_plane{0, 1, 0, 0};
  CHECK(value_on(x_plane & y_plane, 0b1100) == 1);
}

namespace {

template <class Alg>
concept has_wedge_layout =
    requires { core::wedge_layout<Alg, core::full<2>, core::full<2>>; };

} // namespace

// Reference: independent value: by hand from the definitions in rigid(3),
//   the layout names hold the blades each product of a point and a point, a
//   plane and a plane, or a point and a plane reaches, with sign +1 in order
//   of grade and then of index list: the scalar and the six bivectors for the
//   geometric product of two points, the six bivectors for their wedge, e12,
//   e13, e23, and e1234 for their geometric antiproduct, the six bivectors
//   for the antiwedge of two planes, and the scalar for the antiwedge of a
//   point and a plane, and they take only an algebra
TEST_CASE("the product layout names are the canonical sets of the blades") {
  constexpr auto point = core::blades<rga3, "e1 e2 e3 e4">;
  constexpr auto plane = core::blades<rga3, "e423 e431 e412 e321">;
  static_assert(core::geometric_product_layout<rga3, point, point> ==
                core::blades<rga3, "1 e12 e13 e14 e23 e24 e34">);
  static_assert(core::wedge_layout<rga3, point, point> == core::of_grade<4, 2>);
  static_assert(core::geometric_antiproduct_layout<rga3, point, point> ==
                core::blades<rga3, "e12 e13 e23 e1234">);
  static_assert(core::antiwedge_layout<rga3, plane, plane> ==
                core::of_grade<4, 2>);
  static_assert(core::antiwedge_layout<rga3, point, plane> ==
                core::of_grade<4, 0>);
  static_assert(has_wedge_layout<rga3>);
  static_assert(!has_wedge_layout<int>);
  CHECK(core::wedge_layout<rga3, point, point>.size() == 6);
}

// Reference: identity: the antiscalar is e12...n with sign +1, and it is the
//   identity of the geometric antiproduct, 𝟙 ⟇ x = x ⟇ 𝟙 = x, as Lengyel
//   states for rigid geometric algebra, because right(𝟙) is the scalar 1,
//   on random integer multivectors of rigid(3) and conformal(3) and on a
//   motor stored under its oriented names
TEST_CASE("the antiscalar is the identity of the geometric antiproduct") {
  using R = core::mv<rga3, core::full<4>, std::int64_t>;
  using C = core::mv<cga3, core::full<5>, std::int64_t>;
  using Motor =
      core::mv<rga3, core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">,
               std::int64_t>;
  using RI = core::mv<rga3, core::blades<rga3, "e1234">, std::int64_t>;
  using CI = core::mv<cga3, core::blades<cga3, "e12345">, std::int64_t>;
  REQUIRE(RI::layout.blades.at(0).mask ==
          golden::units::antiscalars.at(3).blade);
  REQUIRE(CI::layout.blades.at(0).mask ==
          golden::units::antiscalars.at(4).blade);
  REQUIRE(RI::layout.blades.at(0).sign == 1);
  REQUIRE(CI::layout.blades.at(0).sign == 1);
  const RI ri{1};
  const CI ci{1};
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  for (int k = 0; k < 8; ++k) {
    const R x = random_integers<R>(source);
    const C y = random_integers<C>(source);
    const auto m = random_integers<Motor>(source);
    CHECK((ri % x) == x);
    CHECK((x % ri) == x);
    CHECK((ci % y) == y);
    CHECK((y % ci) == y);
    CHECK(Motor{ri % m} == m);
    CHECK(Motor{m % ri} == m);
  }
}

// Reference: contract: a mixed call of an antiproduct has the catch-all as
//   its only candidate and a requires-expression answers yes for it, while a
//   multivector and a scalar have no antiproduct
TEST_CASE("the antiproduct overloads take the right operands") {
  using R = core::mv<rga3, core::full<4>, double>;
  using F = core::mv<rga3, core::full<4>, float>;
  using C = core::mv<cga3, core::full<5>, double>;
  static_assert(can_antimultiply<R, R>);
  static_assert(can_antiwedge<R, R>);
  static_assert(can_antimultiply<R, F>);
  static_assert(can_antimultiply<R, C>);
  static_assert(can_antiwedge<R, F>);
  static_assert(can_antiwedge<R, C>);
  static_assert(can_call_geometric_antiproduct<R, F>);
  static_assert(can_call_geometric_antiproduct<R, C>);
  static_assert(can_call_antiwedge<R, F>);
  static_assert(can_call_antiwedge<R, C>);
  static_assert(!can_antimultiply<R, double>);
  static_assert(!can_antimultiply<double, R>);
  static_assert(!can_antiwedge<R, int>);
  static_assert(!can_antiwedge<int, R>);
  static_assert(!can_call_geometric_antiproduct<R, double>);
  static_assert(!can_call_antiwedge<double, R>);
}

namespace {
namespace user_code {

// Reference: contract: with galoot::core's operators in scope by a
//   using-directive, a type of the user's without the operators * ^ % &
//   still has none, a type with its own keeps them, and a multivector with
//   such a type has none
TEST_CASE("the product operators leave other types alone") {
  using R = mv<rga3, full<4>, double>;
  static_assert(!has_percent<widget, widget>);
  static_assert(!has_ampersand<widget, widget>);
  static_assert(!has_times<widget, widget>);
  static_assert(!has_caret<widget, widget>);
  static_assert(!has_percent<R, widget>);
  static_assert(!has_ampersand<widget, R>);
  static_assert(!has_times<R, gadget>);
  static_assert(!has_caret<gadget, R>);
  static_assert(std::same_as<decltype(gadget{} % gadget{}), int>);
  static_assert(std::same_as<decltype(gadget{} & gadget{}), int>);
  static_assert(std::same_as<decltype(gadget{} * gadget{}), int>);
  static_assert(std::same_as<decltype(gadget{} ^ gadget{}), int>);
  CHECK((gadget{} % gadget{}) == 1);
  CHECK((gadget{} & gadget{}) == 2);
  CHECK((gadget{} * gadget{}) == 3);
  CHECK((gadget{} ^ gadget{}) == 4);
  static_assert(has_percent<R, R>);
  static_assert(has_ampersand<R, R>);
}

} // namespace user_code
} // namespace
