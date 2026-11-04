// An antidot whose kept terms mix a coefficient the scalar type holds with
// one it cannot. In diag(200, 1), the right complements of e1 and e2 are e2
// and -e1, so the antidot of two values on e1 and e2 keeps e2 . e2 = 1 and
// e1 . e1 = 200, beyond std::int8_t. This file must not compile.
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
  [[maybe_unused]] const auto r = core::antidot(a, b);
}

} // namespace
