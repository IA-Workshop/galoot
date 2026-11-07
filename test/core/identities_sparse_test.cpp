// The identities over forms of 8, 12, and 16 generators, on sparse sets.
#include <random>

#include <galoot/core/metric.hpp>

#include "identities_checks.hpp"

namespace galoot_test::identities {

void large_forms(Identity identity, std::mt19937_64& source) {
  check_each<sparse_a_mv, sparse_b_mv>(
      identity,
      forms<core::R<8, 0, 0>(), core::R<6, 6, 0>(), core::R<15, 0, 1>(),
            core::R<8, 8, 0>()>{},
      source);
}

} // namespace galoot_test::identities
