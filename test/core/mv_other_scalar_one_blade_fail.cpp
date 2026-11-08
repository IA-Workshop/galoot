// A scalar over float converted to a scalar over double, a layout of one blade.
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

using Scalar = core::mv<rga3, scalar_layout, double>;
using FloatScalar = core::mv<rga3, scalar_layout, float>;

[[maybe_unused]] Scalar convert(const FloatScalar& x) { return Scalar{x}; }

} // namespace
