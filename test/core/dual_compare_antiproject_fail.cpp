// The antiprojection of a plane onto a point compared with the plane: the
// result is on the canonical trivectors, a layout other than the plane's,
// so there is no operator== for the two. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/dual.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto plane_layout = core::blades<rga3, "e423 e431 e412 e321">;
using Point = core::mv<rga3, point_layout, double>;
using Plane = core::mv<rga3, plane_layout, double>;

[[maybe_unused]] bool use(const Plane& g, const Point& p) {
  return core::antiproject(g, p) == g;
}

} // namespace
