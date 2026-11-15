// A dot whose exact coefficients the scalar type cannot hold. In
// diag(100, 100), the dot of e12 with itself is det g[12, 12] = 10000,
// beyond std::int8_t, while its antidot, read through the right complement
// of e12, which is 1, is 1. So the check must be the dot's own. This file
// must not compile.
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

[[maybe_unused]] void use(const Narrow& a, const Narrow& b) {
  [[maybe_unused]] const auto r = core::dot(a, b);
}

} // namespace
