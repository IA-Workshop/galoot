// A blade beyond the algebra one generator below the widest: e32 in
// R(31,0,0), the top bit of the mask. This file must not compile, and the
// diagnostic must show the precondition's message
// (core.blade_product_beyond_n_31).
#include <galoot/core/bits.hpp>
#include <galoot/core/blade_product.hpp>
#include <galoot/core/metric.hpp>

namespace {

namespace core = galoot::core;

constexpr core::mask_t top = core::mask_t{1} << (core::mask_bits - 1);

static_assert(core::detail::blade_product<2>(
                  core::R<core::mask_bits - 1, 0, 0, core::mask_bits>(), top,
                  0U)
                  .size() == 1);

} // namespace
