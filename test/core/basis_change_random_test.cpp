// The change of basis as an algebra isomorphism through the public
// functions, on every blade, for the seeded random forms of two to four
// generators, entries from -3 to 3, each against its exact diagonal
// counterpart.
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

using random_0 = bc::to_diagonal_tag<core::from_gram({
    {-1, -2},
    {-2, 3},
})>;
using random_1 = bc::to_diagonal_tag<core::from_gram({
    {2, 0, -3},
    {0, 1, -3},
    {-3, -3, -2},
})>;
using random_2 = bc::to_diagonal_tag<core::from_gram({
    {-2, 0, -2, 2},
    {0, -2, -3, 3},
    {-2, -3, 1, 3},
    {2, 3, 3, 3},
})>;
using random_4 = bc::to_diagonal_tag<core::from_gram({
    {3, 1},
    {1, -2},
})>;
using random_5 = bc::to_diagonal_tag<core::from_gram({
    {-3, -3, 0},
    {-3, -2, 2},
    {0, 2, 0},
})>;
using random_6 = bc::to_diagonal_tag<core::from_gram({
    {0, 1, -1, 0},
    {1, -1, 0, -1},
    {-1, 0, 2, 0},
    {0, -1, 0, -1},
})>;
using random_8 = bc::to_diagonal_tag<core::from_gram({
    {3, -2},
    {-2, -2},
})>;
using random_9 = bc::to_diagonal_tag<core::from_gram({
    {-3, 1, -2},
    {1, -3, -2},
    {-2, -2, 1},
})>;

// Random form 1's change to diag(2, 1, -62) has det P = 1/2, and its
// contragredient does not keep the inner products.
static_assert(bc::determinant_of<random_1> == test_support::rational{1, 2});
static_assert(!bc::dual_isometry<random_1>);

template <class C, core::blade_set S>
void check_all(std::mt19937_64& source, const char* name) {
  bc::check_products<C, S, S>(source, name);
  bc::check_round_trips<C, S>(source, name);
}

template <class C, core::blade_set S>
void check_agreement(std::mt19937_64& source, const char* name) {
  bc::check_agreement<C, S, S>(source, name);
}

} // namespace

// Reference: identity: a change P with P G' P^T = G is an algebra map, so
//   change_basis(x y) = change_basis(x) change_basis(y) and the same for the
//   wedge, and the inverse change undoes it both ways, on random integer
//   multivectors of every blade for the changes of the random forms 0, 1,
//   2, 4, 5, 6, 8, and 9 to their diagonal counterparts
TEST_CASE("the random forms' changes to their diagonals are isomorphisms") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  check_all<random_0, core::full<2>>(source, "random 0");
  check_all<random_1, core::full<3>>(source, "random 1");
  check_all<random_2, core::full<4>>(source, "random 2");
  check_all<random_4, core::full<2>>(source, "random 4");
  check_all<random_5, core::full<3>>(source, "random 5");
  check_all<random_6, core::full<4>>(source, "random 6");
  check_all<random_8, core::full<2>>(source, "random 8");
  check_all<random_9, core::full<3>>(source, "random 9");
}

// Reference: identity: a change of basis preserves the dot, the reverse,
//   the antireverse, and the grade involution, and scales the antiwedge by
//   1 / det P and the bulk duals by det P, since it sends the antiscalar to
//   det(P) times the target's and the complements turn a change by P into
//   a change by P^-T, and where P^-T also keeps the inner products, as for
//   forms whose Gram matrices are their own inverses, it scales the
//   geometric antiproduct, the antidot, the weight duals, and compose by
//   1 / det P and transform by 1 / det(P)^2, on random integer multivectors of
//   every blade for the changes of the random forms 0, 1, 2, 4, 5, 6, 8, and 9
//   to their diagonal counterparts, whose det P is not 1 or -1 for most of
//   them, and whose contragredients do not keep the inner products, so the
//   antiproduct family is not among what they preserve
TEST_CASE("the random forms' changes preserve the operations they must") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  check_agreement<random_0, core::full<2>>(source, "random 0");
  check_agreement<random_1, core::full<3>>(source, "random 1");
  check_agreement<random_2, core::full<4>>(source, "random 2");
  check_agreement<random_4, core::full<2>>(source, "random 4");
  check_agreement<random_5, core::full<3>>(source, "random 5");
  check_agreement<random_6, core::full<4>>(source, "random 6");
  check_agreement<random_8, core::full<2>>(source, "random 8");
  check_agreement<random_9, core::full<3>>(source, "random 9");
}
