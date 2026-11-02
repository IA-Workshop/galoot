// The units of the first assembly read: three products of rigid(3) over
// double through the library's straight-line code, the same products
// written out by hand in the same order of terms, and a multivector made
// zero and then written. Each function has external linkage, so that the
// compiler keeps its body to read.
#include <array>

#include <galoot/core.hpp>

namespace gc = galoot::core;
using gc::detail::BinaryOperation;

struct rga3 {
  static constexpr auto value = gc::rigid<3>();
};

inline constexpr auto point_layout = gc::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto motor_layout =
    gc::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
using Point = gc::mv<rga3, point_layout, double>;
using Motor = gc::mv<rga3, motor_layout, double>;

using PointProduct =
    decltype(gc::detail::apply<BinaryOperation::gp>(Point{}, Point{}));
using PointWedge =
    decltype(gc::detail::apply<BinaryOperation::wedge>(Point{}, Point{}));
using MotorPoint =
    decltype(gc::detail::apply<BinaryOperation::gp>(Motor{}, Point{}));

PointProduct library_point_gp_point(const Point& p, const Point& q) {
  return gc::detail::apply<BinaryOperation::gp>(p, q);
}

PointWedge library_point_wedge_point(const Point& p, const Point& q) {
  return gc::detail::apply<BinaryOperation::wedge>(p, q);
}

MotorPoint library_motor_gp_point(const Motor& m, const Point& p) {
  return gc::detail::apply<BinaryOperation::gp>(m, p);
}

// By hand, on (1, e12, e13, e14, e23, e24, e34), with e4 e4 = 0.
std::array<double, 7> hand_point_gp_point(const std::array<double, 4>& p,
                                          const std::array<double, 4>& q) {
  return {
      (p[0] * q[0]) + (p[1] * q[1]) + (p[2] * q[2]),
      (p[0] * q[1]) - (p[1] * q[0]),
      (p[0] * q[2]) - (p[2] * q[0]),
      (p[0] * q[3]) - (p[3] * q[0]),
      (p[1] * q[2]) - (p[2] * q[1]),
      (p[1] * q[3]) - (p[3] * q[1]),
      (p[2] * q[3]) - (p[3] * q[2]),
  };
}

// By hand, on (e12, e13, e14, e23, e24, e34).
std::array<double, 6> hand_point_wedge_point(const std::array<double, 4>& p,
                                             const std::array<double, 4>& q) {
  return {
      (p[0] * q[1]) - (p[1] * q[0]), (p[0] * q[2]) - (p[2] * q[0]),
      (p[0] * q[3]) - (p[3] * q[0]), (p[1] * q[2]) - (p[2] * q[1]),
      (p[1] * q[3]) - (p[3] * q[1]), (p[2] * q[3]) - (p[3] * q[2]),
  };
}

// By hand, on (e1, e2, e3, e4, e123, e124, e134, e234), from the motor
// (1, e41, e42, e43, e23, e31, e12, e1234) with e41 = -e14, e42 = -e24,
// e43 = -e34, e31 = -e13, and the products of the blades by their masks.
std::array<double, 8> hand_motor_gp_point(const std::array<double, 8>& m,
                                          const std::array<double, 4>& p) {
  return {
      (m[0] * p[0]) - (m[5] * p[2]) + (m[6] * p[1]),
      (m[0] * p[1]) + (m[4] * p[2]) - (m[6] * p[0]),
      (m[0] * p[2]) - (m[4] * p[1]) + (m[5] * p[0]),
      (m[0] * p[3]) + (m[1] * p[0]) + (m[2] * p[1]) + (m[3] * p[2]),
      (m[4] * p[0]) + (m[5] * p[1]) + (m[6] * p[2]),
      (m[1] * p[1]) - (m[2] * p[0]) + (m[6] * p[3]) - (m[7] * p[2]),
      (m[1] * p[2]) - (m[3] * p[0]) - (m[5] * p[3]) + (m[7] * p[1]),
      (m[2] * p[2]) - (m[3] * p[1]) + (m[4] * p[3]) - (m[7] * p[0]),
  };
}

// A point made zero by default and then written in every place: the zero
// fill is dead.
Point written_point(double x) {
  Point p;
  p.coefficients[0] = x;
  p.coefficients[1] = 2 * x;
  p.coefficients[2] = 3 * x;
  p.coefficients[3] = 1;
  return p;
}
