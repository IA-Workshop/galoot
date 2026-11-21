// The transform identities over the forms of two and three generators.
#include <random>

#include <galoot/core/metric.hpp>

#include "transform_identities_checks.hpp"

namespace galoot_test::transform_identities {

namespace {

// Diagonal R(p, q, r) with null generators last, conformal(1), the Witt form
// of one mode, a degenerate form that is not diagonal, and seeded random
// symmetric forms of two and three generators with entries from -3 to 3.
using up_to_three = forms<
    // n = 2
    core::R<2, 0, 0>(), core::R<1, 1, 0>(), core::R<1, 0, 1>(),
    core::from_gram({
        {0, 1},
        {1, 0},
    }),
    core::from_gram({
        {3, 1},
        {1, -2},
    }),
    // n = 3
    core::R<3, 0, 0>(), core::rigid<2>(), core::R<2, 1, 0>(),
    core::R<1, 1, 1>(), core::conformal<1>(),
    core::from_gram({
        {2, 0, -3},
        {0, 1, -3},
        {-3, -3, -2},
    })>;

} // namespace

void forms_up_to_three(std::mt19937_64& source) {
  check_each(up_to_three{}, source);
}

} // namespace galoot_test::transform_identities
