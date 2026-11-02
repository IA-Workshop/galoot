// A requires-expression that asks whether part<double>(motor) is valid. The
// rejecting overload's return type is deduced from its body, so the probe
// gives part's message rather than an answer. This file must not compile.
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

[[maybe_unused]] constexpr bool probed =
    requires(const Motor& motor) { galoot::core::part<double>(motor); };

} // namespace
