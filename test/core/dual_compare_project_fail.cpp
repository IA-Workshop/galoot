// The projection of a line onto a plane compared with the line: the result
// is on the canonical bivectors, a layout other than the line's, so there is
// no operator== for the two. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/dual.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto plane_layout = core::blades<rga3, "e423 e431 e412 e321">;
using Line = core::mv<rga3, line_layout, double>;
using Plane = core::mv<rga3, plane_layout, double>;

[[maybe_unused]] bool use(const Line& l, const Plane& g) {
  return core::project(l, g) == l;
}

} // namespace
