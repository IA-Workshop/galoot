// e1234 e1234 = 32767^4 in diag(32767, 32767, 32767, 32767), an odd
// integer above 2^53, which double cannot hold. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>

namespace {

struct diagonal {
  static constexpr auto value = galoot::core::from_gram({
      {32767, 0, 0, 0},
      {0, 32767, 0, 0},
      {0, 0, 32767, 0},
      {0, 0, 0, 32767},
  });
};

inline constexpr auto layout = galoot::core::blades<diagonal, "e1234">;
using Blade = galoot::core::mv<diagonal, layout, double>;

[[maybe_unused]] Blade::scalar_type square(const Blade& x) {
  using galoot::core::detail::BinaryOperation;
  return galoot::core::detail::apply<BinaryOperation::geometric_product>(x, x)
      .coefficients[0];
}

} // namespace
