// The transform identities over the named forms of four generators.
#include <random>

#include <galoot/core/metric.hpp>

#include "transform_identities_checks.hpp"

namespace galoot_test::transform_identities {

void named_forms_of_four(std::mt19937_64& source) {
  check_each(forms<core::rigid<3>(), core::R<4, 0, 0>(), core::R<2, 2, 0>(),
                   core::R<1, 3, 0>(), core::conformal<2>()>{},
             source);
}

} // namespace galoot_test::transform_identities
