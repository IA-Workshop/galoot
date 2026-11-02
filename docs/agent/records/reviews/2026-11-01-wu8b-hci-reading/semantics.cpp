// What a user expects the conversions and truncate to do, checked at run
// time and printed. Built and run, not just compiled.
#include <cstdint>
#include <cstdio>
#include <format>
#include <type_traits>

#include <galoot/core.hpp>

namespace gc = galoot::core;

struct rga3 {
  static constexpr auto value = gc::rigid<3>();
};

inline constexpr auto point_layout = gc::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout = gc::blades<rga3, "e41 e42 e43 e23 e31 e12">;
// The same line with the direction stored the other way round, as some
// papers write it.
inline constexpr auto line14_layout =
    gc::blades<rga3, "e14 e24 e34 e23 e31 e12">;
inline constexpr auto motor_layout =
    gc::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
using Point = gc::mv<rga3, point_layout, double>;
using Line = gc::mv<rga3, line_layout, double>;
using Line14 = gc::mv<rga3, line14_layout, double>;
using Motor = gc::mv<rga3, motor_layout, double>;
using IMotor = gc::mv<rga3, motor_layout, std::int64_t>;
using ILine = gc::mv<rga3, line_layout, std::int64_t>;
using Bivector = gc::mv<rga3, gc::of_grade<4, 2>, double>;

int main() {
  const Line l{1, 2, 3, 4, 5, 6};

  // Task 1: widen a line into a motor, in the three spellings.
  const Motor m1{l};
  const Motor m2(l);
  const auto m3 = static_cast<Motor>(l);
  std::printf("Motor{l}        %s\n", std::format("{}", m1.coefficients).c_str());
  std::printf("m1==m2==m3      %d\n", m1 == m2 && m2 == m3);

  // Re-signing: the same line on e14 e24 e34.
  const Line14 l14{l};
  std::printf("Line14{l}       %s\n", std::format("{}", l14.coefficients).c_str());
  std::printf("Line{Line14{l}} == l  %d\n", Line{l14} == l);

  // Task 2: take the line part of a motor back.
  const Motor m{7, 1, 2, 3, 4, 5, 6, 8};
  const Line back = gc::truncate<Line>(m);
  const Line same = gc::truncate<line_layout>(m);
  std::printf("truncate<Line>  %s\n", std::format("{}", back.coefficients).c_str());
  std::printf("by type == by layout  %d\n", back == same);
  // truncate to a layout that holds blades the source lacks: those are zero.
  const auto widened = gc::truncate<Motor>(l);
  std::printf("truncate<Motor>(l) == Motor{l}  %d\n", widened == Motor{l});
  // A grade projection by layout: the bivector part of a motor.
  const auto bv = gc::truncate<gc::of_grade<4, 2>>(m);
  std::printf("grade 2 of m    %s  (canonical: e12 e13 e14 e23 e24 e34)\n",
              std::format("{}", bv.coefficients).c_str());
  // A target sharing no blade with the source: all zero, no diagnostic.
  const auto p = gc::truncate<Point>(m);
  std::printf("truncate<Point>(m) %s\n", std::format("{}", p.coefficients).c_str());
  // Target spelled with const and reference.
  const auto cr = gc::truncate<const Line&>(m);
  std::printf("truncate<const Line&> is Line: %d\n",
              std::is_same_v<decltype(cr), const Line>);

  // Probes generic code might ask.
  std::printf("is_convertible<Line, Motor>      %d\n", std::is_convertible_v<Line, Motor>);
  std::printf("is_constructible<Motor, Line>    %d\n", std::is_constructible_v<Motor, Line>);
  std::printf("is_constructible<Line, Motor>    %d  (lossy: compile error if used)\n",
              std::is_constructible_v<Line, Motor>);
  std::printf("is_constructible<Point, std::array<double,4>> %d\n",
              std::is_constructible_v<Point, std::array<double, 4>>);
  std::printf("constructible_from<Line, ILine>  %d\n", std::constructible_from<Line, ILine>);

  // An exact scalar at run time, unchecked by default (GALOOT_CHECKING=%d).
  const IMotor im{7, 1, 2, 3, 4, 5, 6, 8};
  const ILine il = gc::truncate<ILine>(im);
  std::printf("GALOOT_CHECKING=%d: truncate<ILine>(int motor, nonzero scalar) %s\n",
              GALOOT_CHECKING, std::format("{}", il.coefficients).c_str());
  return 0;
}
