// The part of a motor on a class derived from the line type, which part
// does not take as its target. This file must not compile.
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

struct DerivedLine : galoot::core::mv<rga3, line_layout, double> {
  using mv::mv;
};

[[maybe_unused]] void drop(const Motor& motor) {
  const auto kept = galoot::core::part<DerivedLine>(motor);
  (void)kept;
}

} // namespace
