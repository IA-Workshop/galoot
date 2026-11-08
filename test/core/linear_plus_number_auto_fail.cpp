// A line plus a number into auto r. This file must not compile.
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

[[maybe_unused]] void use(const Line& x) { [[maybe_unused]] auto r = x + 1.0; }

} // namespace
