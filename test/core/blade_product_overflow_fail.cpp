// A product whose exact coefficient does not fit coefficient_type. This file
// must not compile, and the diagnostic must show the overflow's message
// (core.blade_product_overflow).
//
// The form is k I + (J - I), where J has every entry 1 and k is the largest
// entry a form may hold: every entry is nonzero, so the contraction terms of
// the product all appear. Its eigenvalues are k - 1, four times, and k + 4,
// so det g = (k - 1)^4 (k + 4). The pseudoscalar e12345 squares to
// (-1)^(5 * 4 / 2) det g = det g, which for k = 2^15 - 1 is about 2^75, past
// the largest coefficient, about 2^63; the static_asserts below check that
// from the type's own limits. If no guard fired, every step would have been
// exact and the result would be that value, so a guard must fire.
#include <cstdint>
#include <limits>

#include <galoot/core/blade_product.hpp>
#include <galoot/core/metric.hpp>

namespace {

namespace core = galoot::core;

constexpr core::coefficient_type k = core::max_form_entry;
constexpr core::coefficient_type max =
    std::numeric_limits<core::coefficient_type>::max();
constexpr core::coefficient_type fourth_power =
    (k - 1) * (k - 1) * (k - 1) * (k - 1);
static_assert(max / fourth_power < k + 4,
              "(k - 1)^4 (k + 4) must exceed the largest coefficient");

constexpr int d = core::max_form_entry;
constexpr auto dense = core::from_gram({{d, 1, 1, 1, 1},
                                        {1, d, 1, 1, 1},
                                        {1, 1, d, 1, 1},
                                        {1, 1, 1, d, 1},
                                        {1, 1, 1, 1, d}});

static_assert(
    core::detail::blade_product<16>(dense, 0b11111U, 0b11111U).size() == 1);

} // namespace
