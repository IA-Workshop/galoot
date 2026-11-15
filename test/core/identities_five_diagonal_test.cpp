// The identities over R(4,1,0) and R(4,0,1), on every blade.
#include <random>

#include <galoot/core/metric.hpp>

#include "identities_checks.hpp"

namespace galoot_test::identities {

void diagonal_forms_of_five(Identity identity, std::mt19937_64& source) {
  check_each<full_mv, full_mv>(
      identity, forms<core::R<4, 1, 0>(), core::R<4, 0, 1>()>{}, source);
}

} // namespace galoot_test::identities
