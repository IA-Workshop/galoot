// The identities over conformal(3), on every blade. Its plans for two
// full multivectors are the largest of the identities, so this source is
// built with the raised limit of constant evaluation that the build gives
// it.
#include <random>

#include <galoot/core/metric.hpp>

#include "identities_checks.hpp"

namespace galoot_test::identities {

void conformal_three(Identity identity, std::mt19937_64& source) {
  check_each<full_mv, full_mv>(identity, forms<core::conformal<3>()>{}, source);
}

} // namespace galoot_test::identities
