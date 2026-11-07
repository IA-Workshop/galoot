// The composition of a motor with a versor of another algebra. This file
// must not compile.
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
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};

using Motor = core::mv<rga3, motor_layout, double>;
using Other = core::mv<cga3, core::of_grade<5, 1>, double>;

[[maybe_unused]] void use(const Motor& q2, const Other& q1) {
  [[maybe_unused]] const auto r = core::compose(q2, q1);
}

} // namespace
