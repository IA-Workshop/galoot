// The product of two points written onto a line, which lacks the scalar of
// the product. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto point_layout = galoot::core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using Point = galoot::core::mv<rga3, point_layout, double>;

[[maybe_unused]] double onto(const Point& p, const Point& q) {
  using galoot::core::detail::BinaryOperation;
  return galoot::core::detail::apply<BinaryOperation::gp, line_layout>(p, q)
      .coefficients[0];
}

} // namespace
