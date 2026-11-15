// A line over float converted to a line over double, written with parentheses.
// This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
// R(4, 0, 0), another algebra with as many generators as rigid(3).
struct r4 {
  static constexpr auto value = core::R<4, 0, 0>();
};

inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto motor_layout =
    core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
inline constexpr auto scalar_layout = core::blades<rga3, "1">;

using Line = core::mv<rga3, line_layout, double>;
using FloatLine = core::mv<rga3, line_layout, float>;

[[maybe_unused]] Line convert(const FloatLine& x) { return Line(x); }

} // namespace
