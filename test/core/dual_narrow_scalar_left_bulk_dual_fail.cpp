// A left bulk dual whose exact coefficients the scalar type cannot hold.
// In diag(100, 100), the left bulk dual of e12 is left(G e12) =
// left(10000 e12), beyond std::int8_t, while its left weight dual is 1. So
// the check must be the left bulk dual's own. This file must not compile.
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

using Narrow = core::mv<big, core::blades<big, "e12">, std::int8_t>;

[[maybe_unused]] void use(const Narrow& x) {
  [[maybe_unused]] const auto r = core::left_bulk_dual(x);
}

} // namespace
