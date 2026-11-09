// The change of basis as an algebra isomorphism through the public
// functions, for conformal_change<3>() from conformal(3) to R(4, 1, 0), on
// every blade. Its products of two full multivectors are the largest of the
// change's identities, so this source is built with the raised limit of
// constant evaluation that the build gives it.
#include <random>

#include <galoot/core/blade_set.hpp>

#include <doctest/doctest.h>

#include "basis_change_checks.hpp"
#include "basis_change_forms.hpp"

namespace {

namespace core = galoot::core;
namespace bc = galoot_test::basis_changes;

} // namespace

// Reference: identity: a change P with P G' P^T = G is an algebra map, so
//   change_basis(x y) = change_basis(x) change_basis(y) and the same for the
//   wedge, and the inverse change undoes it both ways, on random integer
//   multivectors of every blade for conformal_change<3>()
TEST_CASE("conformal_change is an isomorphism for d = 3") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  using c3 = bc::conformal_tag<3>;
  bc::check_products<c3, core::full<5>, core::full<5>>(source, "conformal(3)");
  bc::check_round_trips<c3, core::full<5>>(source, "conformal(3)");
}
