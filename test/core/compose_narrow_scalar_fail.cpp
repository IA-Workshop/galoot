// A composition whose exact coefficients the scalar type cannot hold. In
// diag(200, 1), e2 has the odd antigrade 1, and e2 ⟇ e2 is
// left(right(e2) right(e2)), where right(e2) is -e1 and e1 e1 = 200, beyond
// std::int8_t. This file must not compile.
#include <cstdint>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/transform.hpp>

namespace {

namespace core = galoot::core;

struct uneven {
  static constexpr auto value = core::from_gram({
      {200, 0},
      {0, 1},
  });
};

using Narrow = core::mv<uneven, core::blades<uneven, "e2">, std::int8_t>;

[[maybe_unused]] void use(const Narrow& q2, const Narrow& q1) {
  [[maybe_unused]] const auto r = core::compose(q2, q1);
}

} // namespace
