// The transform identities over rigid(4) and conformal(3).
#include <random>

#include <galoot/core/metric.hpp>

#include "transform_identities_checks.hpp"

namespace galoot_test::transform_identities {

void rigid_and_conformal_forms_of_five(std::mt19937_64& source) {
  check_each(forms<core::rigid<4>(), core::conformal<3>()>{}, source);
}

} // namespace galoot_test::transform_identities
