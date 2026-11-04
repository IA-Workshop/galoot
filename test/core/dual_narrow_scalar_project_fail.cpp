// A projection whose exact coefficients the scalar type cannot hold. In
// diag(100, 100), the projection of 1 onto 1 is 1 v (1 ^ weight_dual(1)),
// and the weight dual of 1 is G right(1) = G e12 = 10000 e12, beyond
// std::int8_t, while the bulk dual of 1, right(G 1) = e12, has coefficient
// 1. So the check must read the weight dual's coefficients. This file must
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

[[maybe_unused]] void use(const Narrow& a, const Narrow& b) {
  [[maybe_unused]] const auto r = core::project(a, b);
}

} // namespace
