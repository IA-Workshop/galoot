// A point over a scalar with a const member, which has a default
// constructor and cannot be assigned. This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto point_layout = galoot::core::blades<rga3, "e1 e2 e3 e4">;

// A number that keeps the unit it was made with.
struct measured {
  double value = 0;
  // The const member is the mistake under test.
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const int unit = 1;
};

[[maybe_unused]] const galoot::core::mv<rga3, point_layout, measured> wrong{};

} // namespace
