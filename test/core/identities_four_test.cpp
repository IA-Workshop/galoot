// The identities over the forms of four generators, on every blade.
#include <random>

#include <galoot/core/metric.hpp>

#include "identities_checks.hpp"

namespace galoot_test::identities {

namespace {

// Each diagonal R(p, q, r) with null generators last, rigid(3) among them;
// conformal(2); the Witt form of two modes; rigid(3) with its null generator
// first; and two seeded random symmetric forms with entries from -3 to 3.
using of_four =
    forms<core::R<4, 0, 0>(), core::rigid<3>(), core::R<1, 3, 0>(),
          core::R<3, 1, 0>(), core::R<2, 2, 0>(), core::R<2, 0, 2>(),
          core::R<0, 0, 4>(), core::conformal<2>(),
          core::from_gram({
              {0, 1, 0, 0},
              {1, 0, 0, 0},
              {0, 0, 0, 1},
              {0, 0, 1, 0},
          }),
          core::from_gram({
              {0, 0, 0, 0},
              {0, 1, 0, 0},
              {0, 0, 1, 0},
              {0, 0, 0, 1},
          }),
          core::from_gram({
              {-2, 0, -2, 2},
              {0, -2, -3, 3},
              {-2, -3, 1, 3},
              {2, 3, 3, 3},
          }),
          core::from_gram({
              {0, 1, -1, 0},
              {1, -1, 0, -1},
              {-1, 0, 2, 0},
              {0, -1, 0, -1},
          })>;

} // namespace

void forms_of_four(Identity identity, std::mt19937_64& source) {
  check_each<full_mv, full_mv>(identity, of_four{}, source);
}

} // namespace galoot_test::identities
