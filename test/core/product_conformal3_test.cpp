// Tests of galoot/core/product.hpp that build the geometric antiproduct or
// the antiwedge of two full multivectors of conformal(3). Those plans are the
// largest of the product test, so this source is built with the raised limit
// of constant evaluation that the build gives it.
#include <cstdint>
#include <random>

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

} // namespace

// Reference: independent value: for every pair of basis blades of rigid(3),
//   conformal(2), conformal(3), the degenerate form
//   [[0, 1, 0], [1, 0, 0], [0, 0, 0]], and random form 1, the geometric
//   antiproduct left(right(a) ⟑ right(b)) and the
//   antiwedge left(right(a) ∧ right(b)), composed in the test from the exact
//   product tables, which satisfy e_i e_j + e_j e_i = 2 g_ij, and the
//   complement tables, which satisfy u ^ right(u) = antiscalar and
//   left(u) ^ u = antiscalar, covering odd and even n, where the two
//   complements agree and differ, and forms that are not diagonal at both
TEST_CASE("geometric_antiproduct and antiwedge of basis blades equal the "
          "composition of the tables") {
  REQUIRE(rga3::value.gram == golden::gp_tables::rigid_3_gram);
  REQUIRE(cga2::value.gram == golden::gp_tables::conformal_2_gram);
  REQUIRE(cga3::value.gram == golden::gp_tables::conformal_3_gram);
  REQUIRE(degenerate3::value.gram == golden::gp_tables::degenerate_3_gram);
  REQUIRE(random1::value.gram == golden::gp_tables::random_1_gram);
  CHECK(antiproduct_mismatches<core::mv<rga3, core::full<4>, std::int64_t>>(
            golden::gp_tables::rigid_3, golden::complements::n4) == 0);
  CHECK(antiproduct_mismatches<core::mv<cga2, core::full<4>, std::int64_t>>(
            golden::gp_tables::conformal_2, golden::complements::n4) == 0);
  CHECK(antiproduct_mismatches<core::mv<cga3, core::full<5>, std::int64_t>>(
            golden::gp_tables::conformal_3, golden::complements::n5) == 0);
  CHECK(antiproduct_mismatches<
            core::mv<degenerate3, core::full<3>, std::int64_t>>(
            golden::gp_tables::degenerate_3, golden::complements::n3) == 0);
  CHECK(antiproduct_mismatches<core::mv<random1, core::full<3>, std::int64_t>>(
            golden::gp_tables::random_1, golden::complements::n3) == 0);
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
