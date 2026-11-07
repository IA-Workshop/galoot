// The antiprojection of a multivector onto one with another scalar type.
// This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/dual.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto plane_layout = core::blades<rga3, "e423 e431 e412 e321">;
using Point = core::mv<rga3, point_layout, double>;
using FloatPlane = core::mv<rga3, plane_layout, float>;

[[maybe_unused]] void use(const Point& a, const FloatPlane& b) {
  [[maybe_unused]] const auto r = core::antiproject(a, b);
}

} // namespace
