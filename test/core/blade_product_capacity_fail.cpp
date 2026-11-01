// A capacity too small for the product: in [[0, 1], [1, 0]], e1 e2 = 1 + e12
// has two terms, and the list holds one. This file must not compile, and the
// diagnostic must show the capacity's message (core.blade_product_capacity).
#include <galoot/core/blade_product.hpp>
#include <galoot/core/metric.hpp>

static_assert(galoot::core::detail::blade_product<1>(
                  galoot::core::from_gram({{0, 1}, {1, 0}}), 0b01U, 0b10U)
                  .size() == 2);
