// The opening example of mv.hpp, verbatim apart from main, and the S6 echo.
#include <cstdio>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};
inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using Line = galoot::core::mv<rga3, line_layout, double>;
inline constexpr auto motor_layout =
    galoot::core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
using Motor = galoot::core::mv<rga3, motor_layout, double>;

int main() {
  Line axis{0, 0, 1, 0, 0, 0};  // the z axis through the origin
  Line none;                     // every coefficient zero
  Motor m{axis};                                // or Motor(axis)
  Line back = galoot::core::truncate<Line>(m);  // drops 1 and e1234
#if CASE == 1
  Motor wrong = axis;
#elif CASE == 2
  const Line l = m;
  (void)l;
#endif
  std::printf("%d %d\n", int(back == axis), int(none == Line{}));
}
