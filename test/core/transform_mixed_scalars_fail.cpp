// A point transformed by a motor of another scalar type. This file must not
// compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/transform.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto motor_layout =
    core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;
using Point = core::mv<rga3, point_layout, double>;
using FloatMotor = core::mv<rga3, motor_layout, float>;

[[maybe_unused]] void use(const Point& x, const FloatMotor& v) {
  [[maybe_unused]] const auto r = core::transform(x, v);
}

} // namespace
