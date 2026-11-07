// The antiprojection of a multivector onto one of another algebra. This
// file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/dual.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};

inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
using Point = core::mv<rga3, point_layout, double>;
using Other = core::mv<cga3, core::full<5>, double>;

[[maybe_unused]] void use(const Point& a, const Other& b) {
  [[maybe_unused]] const auto r = core::antiproject(a, b);
}

} // namespace
