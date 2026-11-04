#include <galoot/core.hpp>

namespace core = galoot::core;
struct alg { static constexpr auto value = core::R<3, 0, 1>(); };
constexpr core::blade_set<4> point_layout{{{{1U, 1}, {2U, 1}, {4U, 1}, {8U, 1}}}};
using Point = core::mv<alg, point_layout, double>;
constexpr core::blade_set<6> line_layout{{{{9U, -1}, {10U, -1}, {12U, -1}, {6U, 1}, {5U, -1}, {3U, 1}}}};
using Line = core::mv<alg, line_layout, double>;
constexpr core::blade_set<4> plane_layout{{{{14U, 1}, {13U, -1}, {11U, 1}, {7U, -1}}}};
using Plane = core::mv<alg, plane_layout, double>;
constexpr core::blade_set<8> motor_layout{{{{0U, 1}, {9U, -1}, {10U, -1}, {12U, -1}, {6U, 1}, {5U, -1}, {3U, 1}, {15U, 1}}}};
using Motor = core::mv<alg, motor_layout, double>;
template <class X> double sum(const X& x) {
  double total = 0;
  for (const double c : x.coefficients) { total += c; }
  return total;
}
double use(double s) {
  const Point point{s, s, s, s};
  const Line line{s, s, s, s, s, s};
  const Plane plane{s, s, s, s};
  const Motor motor{s, s, s, s, s, s, s, s};
  return sum(core::project(point, plane)) + sum(core::project(point, line)) + sum(core::project(line, plane)) + sum(core::project(point, point));
}
