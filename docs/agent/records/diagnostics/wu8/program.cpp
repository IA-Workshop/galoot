// The program a first-time user writes with WU8a's mv.hpp, with one of the
// three likeliest mistakes chosen by WU8_CASE, numbered as in the api-design
// spike's program.cpp (docs/agent/records/spikes/api-design/) so that the
// texts compare case by case. WU8_SPELL chooses how the types are declared:
// 'I' with the layout inline in the alias, as the spike's spelling M did,
// and 'L' with a named layout, as the examples recommend. Case 0 is the
// correct program and must compile.
#include <galoot/core.hpp>

#ifndef WU8_SPELL
#define WU8_SPELL 'L'
#endif
#ifndef WU8_CASE
#define WU8_CASE 0
#endif

namespace gc = galoot::core;

struct rga3 {
  static constexpr auto value = gc::rigid<3>();
};

#if WU8_SPELL == 'I'
using Point = gc::mv<rga3, gc::blades<rga3, "e1 e2 e3 e4">, double>;
using Line = gc::mv<rga3, gc::blades<rga3, "e41 e42 e43 e23 e31 e12">, double>;
using Motor =
    gc::mv<rga3, gc::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">, double>;
#else
inline constexpr auto point_layout = gc::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout = gc::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto motor_layout =
    gc::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
using Point = gc::mv<rga3, point_layout, double>;
using Line = gc::mv<rga3, line_layout, double>;
using Motor = gc::mv<rga3, motor_layout, double>;
#endif

double use(int w) {
  Point p{1, 2, 3, 1};
  Line l{0, 0, 1, 0, 0, 0};
  const Motor q{1, 0, 0, 0, 0, 0, 0, 0};
  double r = q.coefficients[0];

#if WU8_CASE == 0
  get<"e41">(l) = 2;
  r += get<"e41">(l) + get<"e3">(p);
#elif WU8_CASE == 1
  // Too few coefficients: the plane z = 0 meant as a point, or a forgotten w.
  const Point s{1, 2, 3};
  r += s.coefficients[3];
#elif WU8_CASE == 5
  // The opposite orientation: the line stores e41, the user asks for e14.
  r += get<"e14">(l);
#elif WU8_CASE == 6
  // A blade the algebra has, but the type does not hold.
  r += get<"e1">(l);
#elif WU8_CASE == 8
  // An implicit lossy conversion: the motor's other components are dropped.
  const Point s = q;
  r += s.coefficients[0];
#endif
  return r + p.coefficients[0] + l.coefficients[0] + w;
}

int main() { return static_cast<int>(use(1)); }
