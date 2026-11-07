// A point transformed by a versor of mixed parity: the versor holds 1 and
// e1, whose antigrades 4 and 3 in rigid(3) are even and odd. This file must
// not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/transform.hpp>

namespace {

namespace core = galoot::core;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
using Point = core::mv<rga3, point_layout, double>;
using Mixed = core::mv<rga3, core::blades<rga3, "1 e1">, double>;

[[maybe_unused]] void use(const Point& x, const Mixed& v) {
  [[maybe_unused]] const auto r = core::transform(x, v);
}

} // namespace
