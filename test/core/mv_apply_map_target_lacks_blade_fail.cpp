// The right complement of a point, a plane, written onto a point's layout,
// which lacks every blade of it. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto point_layout = galoot::core::blades<rga3, "e1 e2 e3 e4">;
using Point = galoot::core::mv<rga3, point_layout, double>;

[[maybe_unused]] double onto(const Point& p) {
  using galoot::core::detail::UnaryOperation;
  return galoot::core::detail::apply<UnaryOperation::right_complement,
                                     point_layout>(p)
      .coefficients[0];
}

} // namespace
