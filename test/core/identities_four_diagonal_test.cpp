// The identities over the diagonal forms of four generators, on every blade.
#include <random>

#include <galoot/core/metric.hpp>

#include "identities_checks.hpp"

namespace galoot_test::identities {

namespace {

// Each diagonal R(p, q, r) with null generators last, rigid(3) among them.
using diagonal_of_four =
    forms<core::R<4, 0, 0>(), core::rigid<3>(), core::R<1, 3, 0>(),
          core::R<3, 1, 0>(), core::R<2, 2, 0>(), core::R<2, 0, 2>(),
          core::R<0, 0, 4>()>;

} // namespace

void diagonal_forms_of_four(Identity identity, std::mt19937_64& source) {
  check_each<full_mv, full_mv>(identity, diagonal_of_four{}, source);
}

} // namespace galoot_test::identities
