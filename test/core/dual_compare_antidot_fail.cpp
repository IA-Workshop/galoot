// The antidot of two points compared with a point: the result is on the
// antiscalar, a layout other than the point's, so there is no operator==
// for the two. This file must not compile.
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
using Point = core::mv<rga3, point_layout, double>;

[[maybe_unused]] bool use(const Point& p) { return core::antidot(p, p) == p; }

} // namespace
