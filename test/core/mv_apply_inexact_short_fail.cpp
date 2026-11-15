// e12 e12 = -4096^2 = -2^24 in diag(4096, 4096), which short cannot
// hold. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>

namespace {

struct diagonal {
  static constexpr auto value = galoot::core::from_gram({
      {4096, 0},
      {0, 4096},
  });
};

inline constexpr auto layout = galoot::core::blades<diagonal, "e12">;
using Blade = galoot::core::mv<diagonal, layout, short>;

[[maybe_unused]] Blade::scalar_type square(const Blade& x) {
  using galoot::core::detail::BinaryOperation;
  return galoot::core::detail::apply<BinaryOperation::geometric_product>(x, x)
      .coefficients[0];
}

} // namespace
