// A motor over double truncated to a line over float, a change of scalar
// that truncate does not make. This file must not compile.
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
inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using FloatLine = galoot::core::mv<rga3, line_layout, float>;

[[maybe_unused]] void drop(const Motor& motor) {
  const auto kept = galoot::core::truncate<FloatLine>(motor);
  (void)kept;
}

} // namespace
