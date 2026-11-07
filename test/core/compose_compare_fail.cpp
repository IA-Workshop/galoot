// The composition of two motors compared with a motor: the result is on
// the canonical even blades, a layout other than the motor's oriented one,
// so there is no operator== for the two. This file must not compile.
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

[[maybe_unused]] bool use(const Motor& q2, const Motor& q1) {
  return core::compose(q2, q1) == q1;
}

} // namespace
