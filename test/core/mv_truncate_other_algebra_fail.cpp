// A rigid(3) motor truncated to a line of R(4,0,0), another algebra with
// the same blades. This file must not compile.
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
struct r4 {
  static constexpr auto value = galoot::core::R<4, 0, 0>();
};
using EuclideanLine =
    galoot::core::mv<r4, galoot::core::blades<r4, "e41 e42 e43 e23 e31 e12">,
                     double>;

[[maybe_unused]] void drop(const Motor& motor) {
  const auto kept = galoot::core::truncate<EuclideanLine>(motor);
  (void)kept;
}

} // namespace
