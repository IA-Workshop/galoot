// The change of basis as an algebra isomorphism through the public
// functions, for conformal_change<d>() from conformal(d) to R(d+1, 1, 0):
// d = 1 and 2 on every blade, and d = 4 on vectors and bivectors, whose
// full products are beyond what one compiler evaluates at its default
// limits. conformal(3), on every blade, has a source of its own.
#include <random>

#include <galoot/core/blade_set.hpp>

#include <doctest/doctest.h>

#include "basis_change_checks.hpp"
#include "basis_change_forms.hpp"
#include <support/rational.hpp>

namespace {

namespace core = galoot::core;
namespace bc = galoot_test::basis_changes;

using c1 = bc::conformal_tag<1>;
using c2 = bc::conformal_tag<2>;
using c4 = bc::conformal_tag<4>;

// The change of conformal(d) has det P = -1, and its contragredient keeps
// the inner products, since the null pair's block [[0, -1], [-1, 0]] and
// diag(1, ..., 1, -1) are their own inverses.
static_assert(bc::determinant_of<c1> == test_support::rational{-1});
static_assert(bc::determinant_of<c4> == test_support::rational{-1});
static_assert(bc::dual_isometry<c1>);
static_assert(bc::dual_isometry<c2>);
static_assert(bc::dual_isometry<c4>);

} // namespace

// Reference: identity: a change P with P G' P^T = G is an algebra map, so
//   change_basis(x y) = change_basis(x) change_basis(y) and the same for the
//   wedge, which needs no metric, and the inverse change undoes it, so
//   truncate(change_basis_back(change_basis(x))) = x and the same through
//   the target, on random integer multivectors for conformal_change<d>(),
//   on every blade for d = 1 and 2 and on vectors and bivectors for d = 4
TEST_CASE("conformal_change is an isomorphism for d = 1, 2, and 4") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  bc::check_products<c1, core::full<3>, core::full<3>>(source, "conformal(1)");
  bc::check_round_trips<c1, core::full<3>>(source, "conformal(1)");
  bc::check_products<c2, core::full<4>, core::full<4>>(source, "conformal(2)");
  bc::check_round_trips<c2, core::full<4>>(source, "conformal(2)");
  bc::check_products<c4, core::of_grade<6, 1>, core::of_grade<6, 2>>(
      source, "conformal(4)");
  bc::check_products<c4, core::of_grade<6, 2>, core::of_grade<6, 2>>(
      source, "conformal(4)");
  bc::check_round_trips<c4, core::of_grade<6, 1>>(source, "conformal(4)");
  bc::check_round_trips<c4, core::of_grade<6, 2>>(source, "conformal(4)");
}

// Reference: identity: a change of basis preserves the dot, the reverse,
//   the antireverse, and the grade involution, and scales the antiwedge by
//   1 / det P and the bulk duals by det P, since it sends the antiscalar to
//   det(P) times the target's and the complements turn a change by P into
//   a change by P^-T, and where P^-T also keeps the inner products, as for
//   forms whose Gram matrices are their own inverses, it scales the
//   geometric antiproduct, the antidot, the weight duals, and compose by
//   1 / det P and transform by 1 / det(P)^2, on random integer multivectors for
//   conformal_change<d>(), on every blade for d = 1 and 2 and on vectors
//   and bivectors for d = 4, and for d = 1 and 2 the transform of a point
//   by a motor built in the null basis, the antiproduct of two random
//   hyperplanes, and by a flector, one random hyperplane, and its projection
//   and antiprojection onto that hyperplane agree with the same in
//   R(d+1, 1, 0) up to 1 / det(P)^2
TEST_CASE("conformal_change preserves the operations it must, d = 1, 2, 4") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  bc::check_agreement<c1, core::full<3>, core::full<3>>(source, "conformal(1)");
  bc::check_transform<c1, core::of_grade<3, 1>>(source, "conformal(1)");
  bc::check_agreement<c2, core::full<4>, core::full<4>>(source, "conformal(2)");
  bc::check_transform<c2, core::of_grade<4, 1>>(source, "conformal(2)");
  bc::check_agreement<c4, core::of_grade<6, 1>, core::of_grade<6, 2>>(
      source, "conformal(4)");
  bc::check_agreement<c4, core::of_grade<6, 2>, core::of_grade<6, 2>>(
      source, "conformal(4)");
}
