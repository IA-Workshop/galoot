// A point given its coefficients as one std::array, as an aggregate of an
// array would take them. This file must not compile.
#include <array>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto point_layout = galoot::core::blades<rga3, "e1 e2 e3 e4">;
using Point = galoot::core::mv<rga3, point_layout, double>;

constexpr std::array<double, 4> values{1, 2, 3, 1};
[[maybe_unused]] constexpr Point wrong{values};

} // namespace
