// A weight dual whose exact coefficients the scalar type cannot hold. In
// diag(100, 100), the weight dual of the scalar 1 is G right(1) = G e12 =
// 10000 e12, beyond std::int8_t, while its bulk dual, right(G 1) = e12, has
// coefficient 1. So the check must be the weight dual's own. This file must
// not compile.
#include <cstdint>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/dual.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct big {
  static constexpr auto value = core::from_gram({
      {100, 0},
      {0, 100},
  });
};

using Narrow = core::mv<big, core::blades<big, "1">, std::int8_t>;

[[maybe_unused]] void use(const Narrow& x) {
  [[maybe_unused]] const auto r = core::weight_dual(x);
}

} // namespace
