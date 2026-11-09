// The change of basis as an algebra isomorphism through the public
// functions, on every blade, for the Witt forms of one and two modes against
// R(1, 1, 0) and R(2, 2, 0), for the degenerate form [[0, 1, 0], [1, 0, 0],
// [0, 0, 0]] and for rigid(3) with its null generator first, each against
// its exact diagonal counterpart, and for rigid(3) against itself with its
// null generator doubled.
#include <random>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

#include <doctest/doctest.h>

#include "basis_change_checks.hpp"
#include "basis_change_forms.hpp"
#include <support/rational.hpp>

namespace {

namespace core = galoot::core;
namespace bc = galoot_test::basis_changes;

using degenerate = bc::to_diagonal_tag<core::from_gram({
    {0, 1, 0},
    {1, 0, 0},
    {0, 0, 0},
})>;
using null_first = bc::to_diagonal_tag<core::from_gram({
    {0, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1},
})>;

// The Witt changes and the change of rigid(3) with its null generator
// first are between forms whose Gram matrices are their own inverses, of
// det P = -1, and so is the permutation of rigid(3); the doubling of e4
// keeps the inner products of its contragredient too, since a null
// generator's row may be scaled freely, with det P = 2; the degenerate
// form's change to diag(2, -2, 0) does not: through the contragredient,
// e1 . e2 is 4, against 1 in the form.
static_assert(bc::determinant_of<bc::witt_one_tag> ==
              test_support::rational{-1});
static_assert(bc::determinant_of<bc::witt_two_tag> ==
              test_support::rational{-1});
static_assert(bc::determinant_of<null_first> == test_support::rational{-1});
static_assert(bc::determinant_of<bc::doubled_null_tag> ==
              test_support::rational{2});
static_assert(bc::dual_isometry<bc::witt_one_tag>);
static_assert(bc::dual_isometry<bc::witt_two_tag>);
static_assert(bc::dual_isometry<null_first>);
static_assert(bc::dual_isometry<bc::doubled_null_tag>);
static_assert(!bc::dual_isometry<degenerate>);

} // namespace

// Reference: identity: a change P with P G' P^T = G is an algebra map, so
//   change_basis(x y) = change_basis(x) change_basis(y) and the same for the
//   wedge, and the inverse change undoes it both ways, on random integer
//   multivectors of every blade for the Witt changes e(2j-1) =
//   (e'j + e'(m+j)) / 2, e(2j) = e'j - e'(m+j) of one and two modes, the
//   changes of a degenerate form and of rigid(3) with its null generator
//   first to their diagonal counterparts, and rigid(3) with e4 = 2 e'4
TEST_CASE("the Witt, degenerate, and rigid changes are isomorphisms") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  bc::check_products<bc::witt_one_tag, core::full<2>, core::full<2>>(
      source, "Witt, one mode");
  bc::check_round_trips<bc::witt_one_tag, core::full<2>>(source,
                                                         "Witt, one mode");
  bc::check_products<bc::witt_two_tag, core::full<4>, core::full<4>>(
      source, "Witt, two modes");
  bc::check_round_trips<bc::witt_two_tag, core::full<4>>(source,
                                                         "Witt, two modes");
  bc::check_products<degenerate, core::full<3>, core::full<3>>(source,
                                                               "degenerate");
  bc::check_round_trips<degenerate, core::full<3>>(source, "degenerate");
  bc::check_products<null_first, core::full<4>, core::full<4>>(
      source, "rigid(3), null first");
  bc::check_round_trips<null_first, core::full<4>>(source,
                                                   "rigid(3), null first");
  bc::check_products<bc::doubled_null_tag, core::full<4>, core::full<4>>(
      source, "rigid(3), e4 doubled");
  bc::check_round_trips<bc::doubled_null_tag, core::full<4>>(
      source, "rigid(3), e4 doubled");
}

// Reference: identity: a change of basis preserves the dot, the reverse,
//   the antireverse, and the grade involution, and scales the antiwedge by
//   1 / det P and the bulk duals by det P, since it sends the antiscalar to
//   det(P) times the target's and the complements turn a change by P into
//   a change by P^-T, and where P^-T also keeps the inner products, as for
//   forms whose Gram matrices are their own inverses, it scales the
//   geometric antiproduct, the antidot, the weight duals, and compose by
//   1 / det P and transform by 1 / det(P)^2, on random integer multivectors of
//   every blade for the Witt changes, the changes of a degenerate form and of
//   rigid(3) with its null generator first, and rigid(3) with e4 = 2 e'4, of
//   det P = 2, on which 1 / det P and det P differ, and transform by a motor
//   and by a flector, compose, the projection of a vector onto a hyperplane,
//   and the hyperplane's antiprojection onto the vector, each by
//   1 / det(P)^2 but compose by 1 / det P,
//   for all but the degenerate form, whose change's contragredient does not
//   keep the inner products
TEST_CASE("the Witt, degenerate, and rigid changes preserve what they must") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  bc::check_agreement<bc::witt_one_tag, core::full<2>, core::full<2>>(
      source, "Witt, one mode");
  bc::check_transform<bc::witt_one_tag, core::of_grade<2, 1>>(source,
                                                              "Witt, one mode");
  bc::check_agreement<bc::witt_two_tag, core::full<4>, core::full<4>>(
      source, "Witt, two modes");
  bc::check_transform<bc::witt_two_tag, core::of_grade<4, 1>>(
      source, "Witt, two modes");
  bc::check_agreement<degenerate, core::full<3>, core::full<3>>(source,
                                                                "degenerate");
  bc::check_agreement<null_first, core::full<4>, core::full<4>>(
      source, "rigid(3), null first");
  bc::check_transform<null_first, core::of_grade<4, 1>>(source,
                                                        "rigid(3), null first");
  bc::check_agreement<bc::doubled_null_tag, core::full<4>, core::full<4>>(
      source, "rigid(3), e4 doubled");
  bc::check_transform<bc::doubled_null_tag, core::of_grade<4, 1>>(
      source, "rigid(3), e4 doubled");
}
