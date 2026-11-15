// An antiprojection whose second operand gives the weight dual a coefficient
// the scalar type cannot hold, while its first does not. In diag(100, 100),
// the antiprojection of a on e12 onto b on 1 needs the weight dual of 1,
// which is G e12 = 10000 e12, beyond std::int8_t; the weight dual of e12 is
// G 1 = 1. So the check must read the second operand. This file must not
// compile.
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
using Other = core::mv<big, core::blades<big, "e12">, std::int8_t>;

[[maybe_unused]] void use(const Other& a, const Narrow& b) {
  [[maybe_unused]] const auto r = core::antiproject(a, b);
}

} // namespace
