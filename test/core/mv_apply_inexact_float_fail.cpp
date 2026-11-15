// e12 e12 = -(24929 * 673) = -(2^24 + 1) in diag(24929, 673), which float
// cannot hold. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>

namespace {

struct diagonal {
  static constexpr auto value = galoot::core::from_gram({
      {24929, 0},
      {0, 673},
  });
};

inline constexpr auto layout = galoot::core::blades<diagonal, "e12">;
using Blade = galoot::core::mv<diagonal, layout, float>;

[[maybe_unused]] Blade::scalar_type square(const Blade& x) {
  using galoot::core::detail::BinaryOperation;
  return galoot::core::detail::apply<BinaryOperation::geometric_product>(x, x)
      .coefficients[0];
}

} // namespace
