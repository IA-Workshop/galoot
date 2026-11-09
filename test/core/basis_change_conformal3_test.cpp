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

// Reference: identity: a change of basis preserves the dot, the reverse,
//   the antireverse, and the grade involution, and scales the antiwedge by
//   1 / det P and the bulk duals by det P, since it sends the antiscalar to
//   det(P) times the target's and the complements turn a change by P into
//   a change by P^-T, and where P^-T also keeps the inner products, as for
//   forms whose Gram matrices are their own inverses, it scales the
//   geometric antiproduct, the antidot, the weight duals, and compose by
//   1 / det P and transform by 1 / det(P)^2, on random integer multivectors of
//   every blade for conformal_change<3>(), of det P = -1, and the transform of
//   a point by a motor built in the null basis, the antiproduct of two random
//   hyperplanes, agrees with the same in R(4, 1, 0)
TEST_CASE("conformal_change preserves the operations it must, d = 3") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  using c3 = bc::conformal_tag<3>;
  static_assert(bc::dual_isometry<c3>);
  bc::check_agreement<c3, core::full<5>, core::full<5>>(source, "conformal(3)");
  bc::check_transform<c3, core::of_grade<5, 1>>(source, "conformal(3)");
}
