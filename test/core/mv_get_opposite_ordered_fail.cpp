// e21 read from the bivectors of rigid(3), a layout of_grade generates,
// which holds e12. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

using Bivector = galoot::core::mv<rga3, galoot::core::of_grade<4, 2>, double>;

[[maybe_unused]] double read(const Bivector& b) { return get<"e21">(b); }

} // namespace
