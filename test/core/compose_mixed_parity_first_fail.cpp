// The composition of a versor of mixed parity with a motor: the first
// operand holds 1 and e1, whose antigrades 4 and 3 in rigid(3) are even and
// odd. This file must not compile.
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
using Mixed = core::mv<rga3, core::blades<rga3, "1 e1">, double>;

[[maybe_unused]] void use(const Mixed& q2, const Motor& q1) {
  [[maybe_unused]] const auto r = core::compose(q2, q1);
}

} // namespace
