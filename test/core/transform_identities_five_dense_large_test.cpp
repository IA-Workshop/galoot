// The transform identities over R(4,1,0), a dense random form of five
// generators, and two forms of eight.
#include <random>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

#include "transform_identities_checks.hpp"

namespace galoot_test::transform_identities {

namespace {

// A random form of five generators whose entries are almost all nonzero.
using dense_forms = forms<core::from_gram({
    {3, 1, -3, -1, -2},
    {1, -3, -2, 1, 0},
    {-3, -2, -2, 3, 0},
    {-1, 1, 3, 3, 2},
    {-2, 0, 0, 2, -1},
})>;

// Forms of eight generators, checked with sparse hyperplanes.
using large_forms = forms<core::R<8, 0, 0>(), core::rigid<7>()>;

// The hyperplanes of eight generators used here: the blades e1...e7 and
// e2...e8, of grade 7.
inline constexpr core::blade_set<2> sparse_hyperplanes{
    {
        {
            {.mask = 0x7F, .sign = 1},
            {.mask = 0xFE, .sign = 1},
        },
    },
};

} // namespace

// R(4,1,0) as the other forms of five; the dense form with the grades
// stopping at 3, the versors at two hyperplanes, and the compositions at
// pairs of single hyperplanes; the forms of eight at grades up to 2.
void diagonal_dense_and_large_forms(std::mt19937_64& source) {
  check_each(forms<core::R<4, 1, 0>()>{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_identities<form<G>, core::of_grade<5, 4>, 3, 2>(s), ...);
  }(dense_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_identities<form<G>, sparse_hyperplanes, 2>(s), ...);
  }(large_forms{}, source);
}

} // namespace galoot_test::transform_identities
