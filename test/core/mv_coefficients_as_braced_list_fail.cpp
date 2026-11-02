// A point given its coefficients in a second pair of braces, as an
// aggregate holding a std::array would take them. This file must not
// compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto point_layout = galoot::core::blades<rga3, "e1 e2 e3 e4">;
using Point = galoot::core::mv<rga3, point_layout, double>;

[[maybe_unused]] constexpr Point wrong{{1, 2, 3, 1}};

} // namespace
