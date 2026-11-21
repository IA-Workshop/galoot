// The transform identities over the forms of four generators given by their
// Gram matrices.
#include <random>

#include <galoot/core/metric.hpp>

#include "transform_identities_checks.hpp"

namespace galoot_test::transform_identities {

namespace {

// The Witt form of two modes, a degenerate form that is not diagonal, and a
// seeded random symmetric form with entries from -3 to 3.
using gram_four = forms<core::from_gram({
                            {0, 1, 0, 0},
                            {1, 0, 0, 0},
                            {0, 0, 0, 1},
                            {0, 0, 1, 0},
                        }),
                        core::from_gram({
                            {0, 1, 0, 0},
                            {1, 0, 0, 0},
                            {0, 0, 1, 0},
                            {0, 0, 0, 0},
                        }),
                        core::from_gram({
                            {-2, 0, -2, 2},
                            {0, -2, -3, 3},
                            {-2, -3, 1, 3},
                            {2, 3, 3, 3},
                        })>;

} // namespace

void gram_forms_of_four(std::mt19937_64& source) {
  check_each(gram_four{}, source);
}

} // namespace galoot_test::transform_identities
