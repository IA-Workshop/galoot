// The implicit-conversion question: what a user sees with the branch's
// explicit catch-alls, and with catch-alls that copy-initialization reaches.
#include <cstdio>
#include <type_traits>
#include <variant>
#include <vector>

#include <galoot/core.hpp>

#ifndef CASE
#define CASE 0
#endif

namespace gc = galoot::core;

struct rga3 {
  static constexpr auto value = gc::rigid<3>();
};
inline constexpr auto point_layout = gc::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout = gc::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto motor_layout =
    gc::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
using Point = gc::mv<rga3, point_layout, double>;
using Line = gc::mv<rga3, line_layout, double>;
using Motor = gc::mv<rga3, motor_layout, double>;

void take_motor(const Motor&) {}
void f(const Point&) {}
void f(const Line&) {}
void g(const Motor&) {}
void g(const Point&) {}

// Generic code that asks whether a conversion is implicit before using it.
template <class X>
Point as_point(const X& x) {
  if constexpr (std::is_convertible_v<X, Point>) {
    return x;
  } else {
    return gc::truncate<Point>(x);
  }
}

Motor as_motor(const Line& l) {
#if CASE == 4
  return l;
#else
  return Motor{l};
#endif
}

int main() {
  Line l{1, 2, 3, 4, 5, 6};
  const Line cl{1, 2, 3, 4, 5, 6};
  const Motor q{7, 1, 2, 3, 4, 5, 6, 8};
  // Direct-initialization must still pick the real conversion, for a const
  // lvalue, a non-const lvalue, and an rvalue.
  const Motor a{cl};
  const Motor b(l);
  const auto c = static_cast<Motor>(Line{1, 2, 3, 4, 5, 6});
  std::vector<Motor> motors;
  motors.emplace_back(l);
  const Point p = {1, 2, 3, 1};
  const Point back = as_point(Point{1, 2, 3, 1});
#if CASE == 73
  // Generic code that probes is_convertible before converting.
  const Point from_motor = as_point(q);
#else
  const Point from_motor = gc::truncate<Point>(q);
#endif
  double r = a.coefficients[1] + b.coefficients[1] + c.coefficients[1] +
             motors[0].coefficients[1] + p.coefficients[0] +
             back.coefficients[0] + from_motor.coefficients[0] +
             as_motor(l).coefficients[1];
  std::printf("is_convertible<Line, Motor> %d, <Motor, Point> %d, "
              "is_constructible<Motor, Line> %d, Motor{l} == expected %d\n",
              std::is_convertible_v<Line, Motor>,
              std::is_convertible_v<Motor, Point>,
              std::is_constructible_v<Motor, Line>,
              a == Motor{0, 1, 2, 3, 4, 5, 6, 0});
#if CASE == 1
  const Point s = q;
  r += s.coefficients[0];
#elif CASE == 3
  const Motor s = l;
  r += s.coefficients[0];
#elif CASE == 5
  take_motor(l);
#elif CASE == 7
  motors.push_back(l);
#elif CASE == 22
  const Point s = {{1, 2, 3, 1}};
  r += s.coefficients[0];
#elif CASE == 70
  f(q);  // a motor to a point or a line: both lossy
#elif CASE == 71
  g(l);  // a line to a motor (lossless) or a point (lossy)
#elif CASE == 72
  const std::variant<Point, Line> v = q;
  r += v.index();
#endif
  return r > 0 ? 0 : 1;
}
