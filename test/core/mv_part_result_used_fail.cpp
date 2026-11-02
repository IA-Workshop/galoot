// The part of a motor over double on a line over float, with the result
// used as that line: one error, for the target. This file must not
// compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto motor_layout =
    galoot::core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
using Motor = galoot::core::mv<rga3, motor_layout, double>;
using FloatLine = galoot::core::mv<rga3, line_layout, float>;

[[maybe_unused]] float drop(const Motor& motor) {
  const FloatLine kept = galoot::core::part<FloatLine>(motor);
  return kept.coefficients[0];
}

} // namespace
