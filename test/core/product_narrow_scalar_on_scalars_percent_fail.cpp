// An antiproduct whose exact coefficients the scalar type cannot hold, on
// scalars alone: percent. In diag(100, 100), the product of 1 and 1 is 1,
// which std::int8_t holds, but the antiproduct of 1 and 1 is
// left(right(1) right(1)) = left(e12 e12) = -10000 e12, since the right
// complement of 1 is e12 and e12 e12 = -100 * 100, beyond std::int8_t. So
// the check must be the antiproduct's own. This file must not compile.
#include <cstdint>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>

namespace {

namespace core = galoot::core;

struct big {
  static constexpr auto value = core::from_gram({
      {100, 0},
      {0, 100},
  });
};

using NarrowScalar = core::mv<big, core::of_grade<2, 0>, std::int8_t>;

[[maybe_unused]] void use(const NarrowScalar& a, const NarrowScalar& b) {
  [[maybe_unused]] const auto r = a % b;
}

} // namespace
