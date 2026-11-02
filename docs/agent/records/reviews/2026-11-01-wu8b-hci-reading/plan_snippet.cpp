// Plan 0016 section 4, "Moving between blade sets", inside a function.
#include <galoot/core.hpp>

namespace gc = galoot::core;

struct rga3 {
  static constexpr auto value = gc::rigid<3>();
};
inline constexpr auto line_layout = gc::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using Line = gc::mv<rga3, line_layout, double>;
inline constexpr auto motor_layout =
    gc::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
using Motor = gc::mv<rga3, motor_layout, double>;

int main() {
  Line axis{0.0, 0.0, 1.0, 0.0, 0.0, 0.0};
  Motor as_motor{axis};                       // drops nothing; also Motor(axis)
  Line back = gc::truncate<Line>(as_motor);   // drops 1 and e1234
  Line same = gc::truncate<line_layout>(as_motor);  // the same, by blade set
#ifdef LOST
  Line lost{as_motor};
#endif
#ifdef IMPLICIT
  Motor implicit = axis;
#endif
  return back == same && back == axis ? 0 : 1;
}
