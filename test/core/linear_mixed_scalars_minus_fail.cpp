// A sum of two scalar types: a - b with a line over double and a motor
// over float, both of rigid(3). This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/linear.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

using Line =
    core::mv<rga3, core::blades<rga3, "e41 e42 e43 e23 e31 e12">, double>;
using Motor =
    core::mv<rga3, core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">,
             float>;

[[maybe_unused]] void use(const Line& a, const Motor& b) {
  [[maybe_unused]] const auto r = a - b;
}

} // namespace
