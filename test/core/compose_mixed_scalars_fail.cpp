// The composition of a motor with a motor of another scalar type. This
// file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/transform.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

inline constexpr auto motor_layout =
    core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;
using Motor = core::mv<rga3, motor_layout, double>;
using FloatMotor = core::mv<rga3, motor_layout, float>;

[[maybe_unused]] void use(const Motor& q2, const FloatMotor& q1) {
  [[maybe_unused]] const auto r = core::compose(q2, q1);
}

} // namespace
