// A blade beyond the algebra in a constant evaluation of blade_product: e4 in
// R(3,0,0). This file must not compile, and the diagnostic must show the
// precondition's message (core.blade_product_beyond_n).
#include <galoot/core/blade_product.hpp>
#include <galoot/core/metric.hpp>

static_assert(galoot::core::detail::blade_product<4>(galoot::core::R<3, 0, 0>(),
                                                     0b1000U, 0U)
                  .size() == 1);
