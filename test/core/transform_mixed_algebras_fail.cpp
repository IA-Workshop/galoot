// A point transformed by a versor of another algebra. This file must not
// compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/transform.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};

inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
using Point = core::mv<rga3, point_layout, double>;
using Other = core::mv<cga3, core::of_grade<5, 4>, double>;

[[maybe_unused]] void use(const Point& x, const Other& v) {
  [[maybe_unused]] const auto r = core::transform(x, v);
}

} // namespace
