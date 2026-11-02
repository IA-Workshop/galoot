// A point assigned to a motor: the motor holds every blade of the point,
// but conversions between layouts are explicit, so the compiler reports it
// in its own words. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto point_layout = galoot::core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto motor_layout =
    galoot::core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
using Point = galoot::core::mv<rga3, point_layout, double>;
using Motor = galoot::core::mv<rga3, motor_layout, double>;

[[maybe_unused]] void assign(const Point& point) {
  const Motor motor = point;
  (void)motor;
}

} // namespace
