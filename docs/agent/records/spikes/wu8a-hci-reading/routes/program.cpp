// The likeliest mistakes, compiled against mv_routes.hpp with one or more
// routes switched on, to measure what each route changes in the texts.
// NAMED_CLASS declares the user's types as classes derived from mv rather
// than aliases (it needs ROUTE_NAMED_CLASS). Case 0 is the correct program
// and must compile under every route; it also checks that each route keeps
// the type's properties.
// The copy must come first, so that its include guard keeps the real
// mv.hpp out of <galoot/core.hpp>.
// clang-format off
#include "mv_routes.hpp"
#include <galoot/core.hpp>
// clang-format on

#include <cstddef>
#include <type_traits>

#ifndef CASE
#define CASE 0
#endif
#ifndef NAMED_CLASS
#define NAMED_CLASS 0
#endif

namespace gc = galoot::core;

struct rga3 {
  static constexpr auto value = gc::rigid<3>();
};
struct r3 {
  static constexpr auto value = gc::R<3, 0, 0>();
};

inline constexpr auto point_layout = gc::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout = gc::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto motor_layout =
    gc::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;

#if NAMED_CLASS
struct Point : gc::mv<rga3, point_layout, double> {
  using mv::mv;
};
struct Line : gc::mv<rga3, line_layout, double> {
  using mv::mv;
};
struct Motor : gc::mv<rga3, motor_layout, double> {
  using mv::mv;
};
#else
using Point = gc::mv<rga3, point_layout, double>;
using Line = gc::mv<rga3, line_layout, double>;
using Motor = gc::mv<rga3, motor_layout, double>;
#endif

template <class Type, std::size_t K>
constexpr bool keeps_properties =
    !std::is_aggregate_v<Type> && std::is_trivially_copyable_v<Type> &&
    std::is_standard_layout_v<Type> && sizeof(Type) == K * sizeof(double) &&
    !std::is_convertible_v<double, Type> &&
    std::is_constructible_v<Type, double, double, double> &&
    gc::multivector<Type> && gc::has_blade<Type, "e41"> == (K == 6 || K == 8);

double use(int w) {
  Point p{1, 2, 3, 1};
  Line l{0, 0, 1, 0, 0, 0};
  const Motor q{1, 0, 0, 0, 0, 0, 0, 0};
  double r = p.coefficients[0] + l.coefficients[0] + q.coefficients[0] + w;

#if CASE == 0
  static_assert(keeps_properties<Point, 4>);
  static_assert(keeps_properties<Line, 6>);
  static_assert(keeps_properties<Motor, 8>);
  static_assert(Line{1, 2, 3, 4, 5, 6} == Line{1, 2, 3, 4, 5, 6});
  static_assert(get<"e43">(Line{1, 2, 3, 4, 5, 6}) == 3.0);
  constexpr Line zero;
  static_assert(get<"e12">(zero) == 0.0);
  get<"e23">(l) = 2.0;
  r += get<"e43">(l) + get<"e4">(p);
  r += (l != Line{0, 0, 1, 2, 0, 0}) ? 1.0 : 0.0;
#elif CASE == 1
  // One value short.
  const Point s{1, 2, 3};
  r += s.coefficients[0];
#elif CASE == 2
  // An int variable for a double coefficient.
  const Point s{1, 2, 3, w};
  r += s.coefficients[0];
#elif CASE == 3
  // The other orientation.
  r += get<"e14">(l);
#elif CASE == 4
  // A blade the type does not hold.
  r += get<"e1">(l);
#elif CASE == 5
  // A motor assigned to a point.
  const Point s = q;
  r += s.coefficients[0];
#elif CASE == 6
  // A layout of rigid(3) in R(3,0,0).
  const gc::mv<r3, line_layout, double> s{};
  r += s.coefficients[0];
#elif CASE == 7
  // A line compared with a motor.
  r += (l == q) ? 1.0 : 0.0;
#endif
  return r;
}

int main() { return static_cast<int>(use(1)); }
