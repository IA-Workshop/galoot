// Misuse case 10 of plan 0022 R7: control: reverse(fl) + l, already catalogued. Must not compile.
#include <galoot/core.hpp>

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
struct notag {};
inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto motor_layout =
    core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;
using Point = core::mv<rga3, point_layout, double>;
using Line = core::mv<rga3, line_layout, double>;
using FloatLine = core::mv<rga3, line_layout, float>;
using CgaLine = core::mv<cga3, line_layout, double>;
using Motor = core::mv<rga3, motor_layout, double>;

auto f(const FloatLine& fl, const Line& l) { return core::reverse(fl) + l; }
