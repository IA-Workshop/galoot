#include <galoot/core.hpp>
namespace core = galoot::core;
struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
using Point = core::mv<rga3, core::blades<rga3, "e1 e2 e3 e4">, double>;
using Line =
    core::mv<rga3, core::blades<rga3, "e41 e42 e43 e23 e31 e12">, double>;
using Plane = core::mv<rga3, core::blades<rga3, "e423 e431 e412 e321">, double>;
using Motor =
    core::mv<rga3, core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">,
             double>;
using Flector =
    core::mv<rga3, core::blades<rga3, "e1 e2 e3 e4 e423 e431 e412 e321">,
             double>;
Point transform_point_motor(const Point& p, const Motor& q) {
  return core::transform(p, q);
}
Point transform_point_flector(const Point& p, const Flector& f) {
  return core::transform(p, f);
}
Line transform_line_motor(const Line& l, const Motor& q) {
  return core::transform(l, q);
}
Plane transform_plane_motor(const Plane& g, const Motor& q) {
  return core::transform(g, q);
}
auto compose_motors(const Motor& a, const Motor& b) {
  return core::compose(a, b);
}
auto antiproject_plane_point(const Plane& g, const Point& p) {
  return core::antiproject(g, p);
}
