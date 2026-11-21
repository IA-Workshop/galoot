// e1 read from the product of two points written onto its result set, the
// scalar and the bivectors in canonical order. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto point_layout = galoot::core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto product_layout =
    galoot::core::blades<rga3, "1 e12 e13 e14 e23 e24 e34">;
using Point = galoot::core::mv<rga3, point_layout, double>;

[[maybe_unused]] double read(const Point& p, const Point& q) {
  using galoot::core::detail::BinaryOperation;
  return get<"e1">(
      galoot::core::detail::apply<BinaryOperation::geometric_product,
                                  product_layout>(p, q));
}

} // namespace
