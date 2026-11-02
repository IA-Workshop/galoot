// The part of a rigid(3) motor on double, which is not a multivector type.
// This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto motor_layout =
    galoot::core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
using Motor = galoot::core::mv<rga3, motor_layout, double>;

[[maybe_unused]] void drop(const Motor& motor) {
  const auto kept = galoot::core::part<double>(motor);
  (void)kept;
}

} // namespace
