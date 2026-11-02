// Tests of galoot/core/product.hpp: the geometric product and the wedge of
// two multivectors, as functions and as the operators * and ^.
//
// The expected values come from the exact blade-product tables of the
// reference: for the product of two basis blades, the table lists the terms;
// the wedge of two blades with no generator in common is the term of the
// table on the union of their generators, and any other wedge is zero. The
// algebraic identities are checked on integer coefficients small enough that
// every sum is exact.
#include <bitset>
#include <complex>
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
#include <galoot/core/plan.hpp>
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

} // namespace

// Reference: proof: every pair of basis blades of rigid(3) and conformal(3)
//   against the tables of the reference, for the product and the wedge,
//   covering the shared-generator, sign, and zero cases.
TEST_CASE("geometric_product and wedge of basis blades equal the tables") {
  CHECK(table_mismatches<rga3, 4>(golden::gp_tables::rigid_3) == 0);
  CHECK(table_mismatches<cga3, 5>(golden::gp_tables::conformal_3) == 0);
}

// A hand-checked case: in rigid(3) e1 e1 = 1, e1 e2 = e12, e4 e4 = 0.
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

// Reference: definition: a * b is the geometric product geometric_product(a,
//   b), and a ^ b is the wedge of a and b, each with the type and the value
//   of the named function
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

TEST_CASE("the result has the layout the plan gives") {
  using L = core::mv<rga3, core::of_grade<4, 2>, std::int64_t>;
  using V = core::mv<rga3, core::of_grade<4, 1>, std::int64_t>;
  static_assert(
      std::same_as<decltype(core::wedge(V{}, V{})),
                   core::mv<rga3,
                            core::detail::product_plan<
                                core::detail::BinaryOperation::wedge, rga3,
                                core::of_grade<4, 1>, core::of_grade<4, 1>>
                                .result,
                            std::int64_t>>);
  static_assert(std::same_as<decltype(L{} * V{}),
                             decltype(core::geometric_product(L{}, V{}))>);
}

// Reference: proof: associativity of the geometric product and of the wedge
//   on random integer multivectors of rigid(3) and conformal(3), whose
//   sums are exact; the wedge of a vector with itself is zero.
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

// The catch-all overloads are the only error of a bad call, and they make a
// requires-expression answer yes for it, as get does; the valid overloads
// are not captured, and other types' operators are untouched.
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
  static_assert(
      std::same_as<decltype(std::complex<double>{} * std::complex<double>{}),
                   std::complex<double>>);
  static_assert(std::same_as<decltype(std::bitset<4>{} ^ std::bitset<4>{}),
                             std::bitset<4>>);
  CHECK(((std::bitset<4>{0b0101} ^ std::bitset<4>{0b0011}) ==
         std::bitset<4>{0b0110}));
}
