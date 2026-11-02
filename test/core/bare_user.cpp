// A user of the core with no build system, who adds the headers to a
// project and nothing else: core.bare_compile compiles this file with only
// -std=c++23 -pedantic-errors -I include. It declares a line and a motor of
// rigid(3), makes a line, reads and writes it by name, compares it, converts
// it to a motor, and truncates the motor back to a line.
#include <galoot/core.hpp>
#include <galoot/core/assert.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto motor_layout =
    galoot::core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
using Line = galoot::core::mv<rga3, line_layout, double>;
using Motor = galoot::core::mv<rga3, motor_layout, double>;

} // namespace

int main() {
  GALOOT_ASSERT(true, "bare_user: a true condition");

  Line axis{0, 0, 1, 0, 0, 0};
  get<"e23">(axis) = 2.0;
  const bool moved = axis != Line{0, 0, 1, 0, 0, 0};

  const Motor motor{axis};
  const Line back = truncate<Line>(motor);
  const Line by_layout = truncate<line_layout>(motor);
  const bool round_trip = back == axis && by_layout == axis &&
                          get<"1">(motor) == 0.0 && get<"e23">(motor) == 2.0;
  return moved && round_trip && get<"e43">(axis) == 1.0 ? 0 : 1;
}
