#include <galoot/core.hpp>

namespace core = galoot::core;
struct alg { static constexpr auto value = core::R<15, 0, 1>(); };
constexpr core::blade_set<4> point_layout{{{{1U, 1}, {2U, 1}, {4U, 1}, {32768U, 1}}}};
using Point = core::mv<alg, point_layout, double>;
constexpr core::blade_set<6> line_layout{{{{32769U, -1}, {32770U, -1}, {32772U, -1}, {6U, 1}, {5U, -1}, {3U, 1}}}};
using Line = core::mv<alg, line_layout, double>;
constexpr core::blade_set<4> plane_layout{{{{32774U, 1}, {32773U, -1}, {32771U, 1}, {7U, -1}}}};
using Plane = core::mv<alg, plane_layout, double>;
constexpr core::blade_set<8> motor_layout{{{{0U, 1}, {32769U, -1}, {32770U, -1}, {32772U, -1}, {6U, 1}, {5U, -1}, {3U, 1}, {32775U, 1}}}};
using Motor = core::mv<alg, motor_layout, double>;
double use(double s) {
  const Point point{s, s, s, s};
  const Line line{s, s, s, s, s, s};
  const Plane plane{s, s, s, s};
  const Motor motor{s, s, s, s, s, s, s, s};
  return point.coefficients[0] + point.coefficients[0] + line.coefficients[0] + plane.coefficients[0] + motor.coefficients[0] + point.coefficients[0] + point.coefficients[0] + motor.coefficients[0];
}
