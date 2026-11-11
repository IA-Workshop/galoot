// One function per operation, each the expression its benchmark program in
// test/bench/ times, so that the operation's code can be read on its own in
// the compiler's assembly output.
#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<double>;
using Line = rga3::Line<double>;
using Plane = rga3::Plane<double>;
using Motor = rga3::Motor<double>;

Line wedge_point_point(const Point& p, const Point& q) { return Line{p ^ q}; }

Plane wedge_point_point_point(const Point& p, const Point& q, const Point& r) {
  return Plane{(p ^ q) ^ r};
}

Point antiwedge_line_plane(const Line& l, const Plane& g) {
  return Point{l & g};
}

Point transform_point(const Point& x, const Motor& q) {
  return core::transform(x, q);
}

Line transform_line(const Line& x, const Motor& q) {
  return core::transform(x, q);
}

Plane transform_plane(const Plane& x, const Motor& q) {
  return core::transform(x, q);
}

Motor compose_motors(const Motor& a, const Motor& b) {
  return Motor{core::compose(a, b)};
}
