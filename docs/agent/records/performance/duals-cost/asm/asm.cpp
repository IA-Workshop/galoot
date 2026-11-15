#include <galoot/core.hpp>
namespace core = galoot::core;
struct rga3 { static constexpr auto value = core::rigid<3>(); };
using Point = core::mv<rga3, core::blades<rga3, "e1 e2 e3 e4">, double>;
using Line = core::mv<rga3, core::blades<rga3, "e41 e42 e43 e23 e31 e12">, double>;
using Plane = core::mv<rga3, core::blades<rga3, "e423 e431 e412 e321">, double>;
using Motor = core::mv<rga3, core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">, double>;
auto dot_motors(const Motor& a, const Motor& b) { return core::dot(a, b); }
auto antidot_motors(const Motor& a, const Motor& b) { return core::antidot(a, b); }
auto bulk_line(const Line& l) { return core::bulk(l); }
auto weight_dual_point(const Point& p) { return core::weight_dual(p); }
auto project_point_plane(const Point& p, const Plane& g) { return core::project(p, g); }
