// rga3::rotation about an l that is not a line: its direction v and moment m
// are not orthogonal. The comment of rotation states the precondition
// (v . m = 0) and that it is not checked. This shows what a checking build
// does with such an l, and with a join computed in floating point, which
// meets the precondition only up to rounding.
//
// A motor Q is unitized when its weight norm is 1 and Qv . Qm = 0 (the sum
// over its e41, e42, e43, e1234 coefficients times its e23, e31, e12, 1
// coefficients). The defect printed is that sum. A rigid motion keeps the
// distance between two points, so the distance between the images of two
// points is printed beside the distance between the points. It also takes
// the line through two points to the line through their images, so the
// image of the join p ^ r is printed beside the join of the images, with
// the image's own v . m, which is 0 for a line.
#include <cmath>
#include <cstdio>

#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<double>;
using Line = rga3::Line<double>;
using Motor = rga3::Motor<double>;

double motor_defect(const Motor& q) {
  const auto& c = q.coefficients;
  return (c[0] * c[4]) + (c[1] * c[5]) + (c[2] * c[6]) + (c[3] * c[7]);
}

double distance(const Point& p, const Point& q) {
  const auto& a = p.coefficients;
  const auto& b = q.coefficients;
  const double dx = (a[0] / a[3]) - (b[0] / b[3]);
  const double dy = (a[1] / a[3]) - (b[1] / b[3]);
  const double dz = (a[2] / a[3]) - (b[2] / b[3]);
  return std::sqrt((dx * dx) + (dy * dy) + (dz * dz));
}

void show(const char* what, const Line& l) {
  const auto& c = l.coefficients;
  const double v_dot_m = (c[0] * c[3]) + (c[1] * c[4]) + (c[2] * c[5]);
  const Motor q = rga3::rotation(l, 1.0);
  const Point p{1, 0, 0, 1};
  const Point r{0, 2, 0, 1};
  const Point p2 = core::transform(p, q);
  const Point r2 = core::transform(r, q);
  std::printf("%s: v . m = %g, motor defect Qv . Qm = %g\n", what, v_dot_m,
              motor_defect(q));
  std::printf("  points: weights of the images %.17g and %.17g, distance "
              "%.17g before and %.17g after\n",
              p2.coefficients[3], r2.coefficients[3], distance(p, r),
              distance(p2, r2));
  const Line moved = core::transform(Line{p ^ r}, q);
  const Line joined{p2 ^ r2};
  const auto& m = moved.coefficients;
  const auto& j = joined.coefficients;
  std::printf("  image of p ^ r:     %.6f %.6f %.6f %.6f %.6f %.6f, its v . m "
              "= %g\n",
              m[0], m[1], m[2], m[3], m[4], m[5],
              (m[0] * m[3]) + (m[1] * m[4]) + (m[2] * m[5]));
  std::printf("  join of the images: %.6f %.6f %.6f %.6f %.6f %.6f\n", j[0],
              j[1], j[2], j[3], j[4], j[5]);
}

int main() {
  std::printf("GALOOT_CHECKING = %d\n", GALOOT_CHECKING);
  // The z axis, a line: direction (0, 0, 1), moment 0.
  show("the z axis", Line{0, 0, 1, 0, 0, 0});
  // Direction (0, 0, 1) with moment (0, 0, 1): v . m = 1, not a line.
  show("direction z, moment z", Line{0, 0, 1, 0, 0, 1});
  // The join of two points with coordinates that are not exact in binary.
  const Point a{0.1, 0.2, 0.3, 1};
  const Point b{0.7, -0.4, 1.1, 1};
  show("a join in floating point", Line{a ^ b});
  return 0;
}
