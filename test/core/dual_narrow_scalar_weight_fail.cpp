// A weight whose exact coefficients the scalar type cannot hold. In
// diag(100, 100), the weight of the scalar 1 is left(G right(1)) =
// left(G e12) = 10000 e12, beyond std::int8_t, while its bulk is 1. So the
// check must be the weight's own. This file must not compile.
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
  [[maybe_unused]] const auto r = core::weight(x);
}

} // namespace
