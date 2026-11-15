// A transform whose first product has an exact coefficient the scalar type
// cannot hold, while its second does not. In diag(200, 1), V on e2 is odd,
// and V ⟇ x for x on e2 is left(right(e2) right(e2)), where right(e2) = -e1
// and e1 e1 = 200, beyond std::int8_t; it lands on e12, and e12 ⟇ e2 has the
// coefficient 1 or -1. So a check of the second product alone accepts it.
// This file must not compile.
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

[[maybe_unused]] void use(const Narrow& x, const Narrow& v) {
  [[maybe_unused]] const auto r = core::transform(x, v);
}

} // namespace
