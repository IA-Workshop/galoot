// The change of basis as an algebra isomorphism through the public
// functions, for the seeded random forms 3 and 7, of five generators with
// almost every entry nonzero, against their exact diagonal counterparts, on
// vectors and bivectors: the products of all their blades are beyond what
// one compiler evaluates at its default limits.
#include <random>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

#include <doctest/doctest.h>

#include "basis_change_checks.hpp"
#include "basis_change_forms.hpp"

namespace {

namespace core = galoot::core;
namespace bc = galoot_test::basis_changes;

using random_3 = bc::to_diagonal_tag<core::from_gram({
    {3, 1, -3, -1, -2},
    {1, -3, -2, 1, 0},
    {-3, -2, -2, 3, 0},
    {-1, 1, 3, 3, 2},
    {-2, 0, 0, 2, -1},
})>;
using random_7 = bc::to_diagonal_tag<core::from_gram({
    {-3, 1, 2, -1, -2},
    {1, -3, 0, -3, -1},
    {2, 0, 0, 3, -3},
    {-1, -3, 3, 1, -2},
    {-2, -1, -3, -2, 2},
})>;

template <class C>
void check_all(std::mt19937_64& source, const char* name) {
  constexpr auto v = core::of_grade<5, 1>;
  constexpr auto b = core::of_grade<5, 2>;
  bc::check_products<C, v, v>(source, name);
  bc::check_products<C, v, b>(source, name);
  bc::check_products<C, b, b>(source, name);
  bc::check_round_trips<C, v>(source, name);
  bc::check_round_trips<C, b>(source, name);
}

} // namespace

// Reference: identity: a change P with P G' P^T = G is an algebra map, so
//   change_basis(x y) = change_basis(x) change_basis(y) and the same for the
//   wedge, and the inverse change undoes it both ways, on random integer
//   vectors and bivectors for the changes of the random forms 3 and 7 to
//   their diagonal counterparts, whose denominators reach 7755 and 12
TEST_CASE("the dense random forms' changes to their diagonals are "
          "isomorphisms") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  check_all<random_3>(source, "random 3");
  check_all<random_7>(source, "random 7");
}
