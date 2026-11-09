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
