// A product whose exact coefficients the scalar type cannot hold:
// geometric_product. In diag(100, 100), e12 e12 = -100 * 100 = -10000, beyond
// std::int8_t. This file must not compile.
#include <cstdint>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>

namespace {

namespace core = galoot::core;

struct big {
  static constexpr auto value = core::from_gram({
      {100, 0},
      {0, 100},
  });
};

using Narrow = core::mv<big, core::full<2>, std::int8_t>;

[[maybe_unused]] void use(const Narrow& a, const Narrow& b) {
  using core::geometric_product;
  [[maybe_unused]] const auto r = geometric_product(a, b);
}

} // namespace
