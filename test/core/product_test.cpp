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
// that every sum is exact.
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <random>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};

// The coefficient of the result on the blade with this mask, or zero when
// the result's layout lacks it.
template <class Type>
std::int64_t value_on(const Type& r, core::mask_t mask) {
  for (std::size_t k = 0; k < Type::component_count; ++k) {
    const core::oriented_blade blade = Type::layout.blades.at(k);
    if (blade.mask == mask) {
      return blade.sign * r.coefficients.at(k);
    }
  }
  return 0;
}

// The coefficient of the table's product on the blade with this mask.
inline std::int64_t table_value(const golden::product& entry,
                                core::mask_t mask) {
  std::int64_t sum = 0;
  for (std::size_t k = 0; k < entry.size; ++k) {
    if (entry.terms.at(k).blade == mask) {
      sum += entry.terms.at(k).coefficient;
    }
  }
  return sum;
}

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

template <class Type>
Type random_integers(std::mt19937_64& source) {
  std::uniform_int_distribution<std::int64_t> digit(-3, 3);
  Type x{};
  for (auto& c : x.coefficients) {
    c = digit(source);
  }
  return x;
}

template <class X, class Y>
concept can_antimultiply = requires(X x, Y y) { x % y; };
template <class X, class Y>
concept can_antiwedge = requires(X x, Y y) { x & y; };
template <class X, class Y>
concept can_call_geometric_antiproduct =
    requires(X x, Y y) { core::geometric_antiproduct(x, y); };
template <class X, class Y>
concept can_call_antiwedge = requires(X x, Y y) { core::antiwedge(x, y); };

// The two forms of the reference tables beside rigid(3) and conformal(3): a
// degenerate form that is not diagonal, and a seeded random form.
struct degenerate3 {
  static constexpr auto value = core::from_gram({
      {0, 1, 0},
      {1, 0, 0},
      {0, 0, 0},
  });
};
struct random1 {
  static constexpr auto value = core::from_gram({
      {2, 0, -3},
      {0, 1, -3},
      {-3, -3, -2},
  });
};

// The name of a blade, e followed by the indices of its generators, or 1,
// with a minus sign for an orientation of -1, so that a failure names the
// blades it is about.
std::string blade_name(core::mask_t mask, int sign = 1) {
  std::string name = sign < 0 ? "-" : "";
  if (mask == 0) {
    return name + "1";
  }
  name += 'e';
  for (unsigned i = 0; i < 32; ++i) {
    if (((mask >> i) & 1U) != 0) {
      name += std::to_string(i + 1);
    }
  }
  return name;
}

// The coefficients, indexed by mask, of the geometric antiproduct and the
// antiwedge of the canonical blades of the masks a and b, composed from
// the tables alone: a ⟇ b = left(right(a) ⟑ right(b)) and
// a ∨ b = left(right(a) ∧ right(b)), with right and left read from the
// complement table and the product from the product table. The wedge of two
// blades with no generator in common is the term of their product on the
// union of their generators; any other wedge is zero. `found` is false when
// a table does not hold an entry where its index says it does.
struct composition {
  std::vector<std::int64_t> antiproduct_on;
  std::vector<std::int64_t> antiwedge_on;
  bool found;
};

composition compose(std::span<const golden::product> table,
                    std::span<const golden::complement_pair> complements,
                    core::mask_t a, core::mask_t b) {
  const std::size_t size = complements.size();
  composition out{.antiproduct_on = std::vector<std::int64_t>(size),
                  .antiwedge_on = std::vector<std::int64_t>(size),
                  .found = true};
  const golden::signed_blade ra = complements[a].right;
  const golden::signed_blade rb = complements[b].right;
  const golden::product& entry = table[(ra.blade * size) + rb.blade];
  out.found = complements[a].blade == a && complements[b].blade == b &&
              entry.a == ra.blade && entry.b == rb.blade;
  if (!out.found) {
    return out;
  }

  const std::int64_t sign = std::int64_t{ra.sign} * rb.sign;
  for (std::size_t k = 0; k < entry.size; ++k) {
    const golden::term& t = entry.terms.at(k);
    const golden::complement_pair& d = complements[t.blade];
    out.found = out.found && d.blade == t.blade;
    out.antiproduct_on.at(d.left.blade) += sign * t.coefficient * d.left.sign;
  }
  if ((ra.blade & rb.blade) == 0) {
    const core::mask_t joined = ra.blade | rb.blade;
    const golden::complement_pair& d = complements[joined];
    out.antiwedge_on.at(d.left.blade) +=
        sign * table_value(entry, joined) * d.left.sign;
  }
  return out;
}

// The number of coefficients, over every pair of basis blades of the layout
// of X and every blade of the algebra, on which geometric_antiproduct or
// antiwedge differs from the composition of the tables. Each basis blade is
// the blade of its place in the layout with its orientation there, so a
// value of 1 at a place whose orientation is -1 is minus the canonical
// blade. Each difference is reported with the blades it is about.
template <class X>
int antiproduct_mismatches(
    std::span<const golden::product> table,
    std::span<const golden::complement_pair> complements) {
  int mismatches = 0;
  for (std::size_t i = 0; i < X::component_count; ++i) {
    for (std::size_t j = 0; j < X::component_count; ++j) {
      const core::oriented_blade a = X::layout.blades.at(i);
      const core::oriented_blade b = X::layout.blades.at(j);
      X x{};
      X y{};
      x.coefficients.at(i) = 1;
      y.coefficients.at(j) = 1;
      const composition expected = compose(table, complements, a.mask, b.mask);
      if (!expected.found) {
        FAIL_CHECK("no table entry for " << blade_name(a.mask) << " and "
                                         << blade_name(b.mask));
        ++mismatches;
        continue;
      }

      const auto ap = core::geometric_antiproduct(x, y);
      const auto aw = core::antiwedge(x, y);
      const std::int64_t sign = std::int64_t{a.sign} * b.sign;
      for (core::mask_t m = 0; m < complements.size(); ++m) {
        const std::int64_t want_ap = sign * expected.antiproduct_on.at(m);
        const std::int64_t want_aw = sign * expected.antiwedge_on.at(m);
        if (value_on(ap, m) != want_ap) {
          FAIL_CHECK("geometric_antiproduct("
                     << blade_name(a.mask, a.sign) << ", "
                     << blade_name(b.mask, b.sign) << ") on " << blade_name(m)
                     << " is " << value_on(ap, m) << ", not " << want_ap);
          ++mismatches;
        }
        if (value_on(aw, m) != want_aw) {
          FAIL_CHECK("antiwedge(" << blade_name(a.mask, a.sign) << ", "
                                  << blade_name(b.mask, b.sign) << ") on "
                                  << blade_name(m) << " is " << value_on(aw, m)
                                  << ", not " << want_aw);
          ++mismatches;
        }
      }
    }
  }
  return mismatches;
}

} // namespace

// A scope that sees galoot::core's operators by a using-directive, as a
// user's code may, with types of its own: one without operators and one
// with its own % and &. The library's operators must leave both alone.
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

// Reference: comparison: every pair of basis blades of rigid(3) and
// conformal(3)
//   against the exact tables, which satisfy e_i e_j + e_j e_i = 2 g_ij, for
//   the product and the wedge, covering the shared-generator, sign, and zero
//   cases
TEST_CASE("geometric_product and wedge of basis blades equal the tables") {
  CHECK(table_mismatches<rga3, 4>(golden::gp_tables::rigid_3) == 0);
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

// Reference: independent value: for every pair of basis blades of rigid(3),
//   conformal(3), the degenerate form [[0, 1, 0], [1, 0, 0], [0, 0, 0]], and
//   random form 1, the geometric antiproduct left(right(a) ⟑ right(b)) and the
//   antiwedge left(right(a) ∧ right(b)), composed in the test from the exact
//   product tables, which satisfy e_i e_j + e_j e_i = 2 g_ij, and the
//   complement tables, which satisfy u ^ right(u) = antiscalar and
//   left(u) ^ u = antiscalar, covering odd and even n, where the two
//   complements agree and differ
TEST_CASE("geometric_antiproduct and antiwedge of basis blades equal the "
          "composition of the tables") {
  REQUIRE(rga3::value.gram == golden::gp_tables::rigid_3_gram);
  REQUIRE(cga3::value.gram == golden::gp_tables::conformal_3_gram);
  REQUIRE(degenerate3::value.gram == golden::gp_tables::degenerate_3_gram);
  REQUIRE(random1::value.gram == golden::gp_tables::random_1_gram);
  CHECK(antiproduct_mismatches<core::mv<rga3, core::full<4>, std::int64_t>>(
            golden::gp_tables::rigid_3, golden::complements::n4) == 0);
  CHECK(antiproduct_mismatches<core::mv<cga3, core::full<5>, std::int64_t>>(
            golden::gp_tables::conformal_3, golden::complements::n5) == 0);
  CHECK(antiproduct_mismatches<
            core::mv<degenerate3, core::full<3>, std::int64_t>>(
            golden::gp_tables::degenerate_3, golden::complements::n3) == 0);
  CHECK(antiproduct_mismatches<core::mv<random1, core::full<3>, std::int64_t>>(
            golden::gp_tables::random_1, golden::complements::n3) == 0);
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

// Reference: identity: associativity of the geometric antiproduct and of the
//   antiwedge on random integer multivectors of rigid(3) and conformal(3),
//   whose sums are exact
TEST_CASE("the antiproducts are associative") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  using R = core::mv<rga3, core::full<4>, std::int64_t>;
  using C = core::mv<cga3, core::full<5>, std::int64_t>;
  for (int k = 0; k < 6; ++k) {
    const R a = random_integers<R>(source);
    const R b = random_integers<R>(source);
    const R c = random_integers<R>(source);
    CHECK(((a % b) % c) == (a % (b % c)));
    CHECK(((a & b) & c) == (a & (b & c)));
    const C d = random_integers<C>(source);
    const C e = random_integers<C>(source);
    const C f = random_integers<C>(source);
    CHECK(((d % e) % f) == (d % (e % f)));
    CHECK(((d & e) & f) == (d & (e & f)));
  }
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
    const Motor m = random_integers<Motor>(source);
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
