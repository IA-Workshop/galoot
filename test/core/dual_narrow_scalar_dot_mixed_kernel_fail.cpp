// A dot whose kept terms mix a coefficient the scalar type holds with one it
// cannot. In diag(200, 1), the dot of two values on e1 and e2 keeps
// e1 . e1 = 200, beyond std::int8_t, and e2 . e2 = 1, so every kept
// coefficient must be checked, not just one of them. This file must not
// compile.
#include <cstdint>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/dual.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct uneven {
  static constexpr auto value = core::from_gram({
      {200, 0},
      {0, 1},
  });
};

using Narrow = core::mv<uneven, core::blades<uneven, "e1 e2">, std::int8_t>;

[[maybe_unused]] void use(const Narrow& a, const Narrow& b) {
  [[maybe_unused]] const auto r = core::dot(a, b);
}

} // namespace
