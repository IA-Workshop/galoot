// A point over bool, whose sum true + true is true, not a
// ring's. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto point_layout = galoot::core::blades<rga3, "e1 e2 e3 e4">;

[[maybe_unused]] const galoot::core::mv<rga3, point_layout, bool> wrong{};

} // namespace
