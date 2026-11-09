// The identities over the forms of one to three generators, on every blade.
#include <random>

#include <galoot/core/metric.hpp>

#include "identities_checks.hpp"

namespace galoot_test::identities {

namespace {

// Each diagonal R(p, q, r) with null generators last; conformal(1); the
// Witt form of one mode; a degenerate form that is not diagonal; and seeded
// random symmetric forms of two and three generators with entries from -3 to
// 3.
using up_to_three = forms<
    // n = 1
    core::R<1, 0, 0>(), core::R<0, 1, 0>(), core::R<0, 0, 1>(),
    // n = 2
    core::R<2, 0, 0>(), core::R<0, 2, 0>(), core::R<1, 1, 0>(),
    core::R<1, 0, 1>(), core::R<0, 0, 2>(),
    core::from_gram({
        {0, 1},
        {1, 0},
    }),
    core::from_gram({
        {-1, -2},
        {-2, 3},
    }),
    core::from_gram({
        {3, 1},
        {1, -2},
    }),
    core::from_gram({
        {3, -2},
        {-2, -2},
    }),
    // n = 3
    core::R<3, 0, 0>(), core::R<2, 1, 0>(), core::R<2, 0, 1>(),
    core::R<1, 1, 1>(), core::R<0, 0, 3>(), core::conformal<1>(),
    core::from_gram({
        {0, 1, 0},
        {1, 0, 0},
        {0, 0, 0},
    }),
    core::from_gram({
        {2, 0, -3},
        {0, 1, -3},
        {-3, -3, -2},
    }),
    core::from_gram({
        {-3, -3, 0},
        {-3, -2, 2},
        {0, 2, 0},
    }),
    core::from_gram({
        {-3, 1, -2},
        {1, -3, -2},
        {-2, -2, 1},
    })>;

// Two degenerate forms whose radical lies along no generator, so that no
// zero on the diagonal marks it: one of rank 2 whose radical is spanned by
// e1 - e2 - e3, although e2 squares to zero, and the form of rank 1 with
// every entry 1, in which no generator squares to zero. They are a group of
// their own, checked after every other group, so that the multivectors the
// other groups draw from a seed stay as they were.
using off_axis = forms<core::from_gram({
                           {1, 1, 0},
                           {1, 0, 1},
                           {0, 1, -1},
                       }),
                       core::from_gram({
                           {1, 1, 1},
                           {1, 1, 1},
                           {1, 1, 1},
                       })>;

} // namespace

void forms_up_to_three(Identity identity, std::mt19937_64& source) {
  check_each<full_mv, full_mv>(identity, up_to_three{}, source);
}

void forms_off_axis(Identity identity, std::mt19937_64& source) {
  check_each<full_mv, full_mv>(identity, off_axis{}, source);
}

} // namespace galoot_test::identities
