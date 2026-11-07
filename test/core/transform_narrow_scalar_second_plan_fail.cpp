// A transform whose second product has an exact coefficient the scalar type
// cannot hold, while its first does not. In diag(200, 1), V on e2 is odd,
// and V ⟇ x for x on e1 is left(right(e2) right(e1)), with right(e2) = -e1
// and right(e1) = e2, so its coefficient is 1 or -1; it lands on the scalar,
// and 1 ⟇ e2 is left(right(1) right(e2)) = left(-e12 e1), where
// e12 e1 = -200 e2, beyond std::int8_t. So a check of the first product
// alone accepts it. This file must not compile.
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

using X = core::mv<uneven, core::blades<uneven, "e1">, std::int8_t>;
using V = core::mv<uneven, core::blades<uneven, "e2">, std::int8_t>;

[[maybe_unused]] void use(const X& x, const V& v) {
  [[maybe_unused]] const auto r = core::transform(x, v);
}

} // namespace
