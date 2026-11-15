// The identities over R(4,2,0), R(3,3,0), and conformal(4), on vectors and
// bivectors, since the products of all their blades are beyond what one
// compiler evaluates at its default limits.
#include <random>

#include <galoot/core/metric.hpp>

#include "identities_checks.hpp"

namespace galoot_test::identities {

void forms_of_six(Identity identity, std::mt19937_64& source) {
  check_each<vectors_mv, bivectors_mv>(
      identity,
      forms<core::R<4, 2, 0>(), core::R<3, 3, 0>(), core::conformal<4>()>{},
      source);
}

} // namespace galoot_test::identities
