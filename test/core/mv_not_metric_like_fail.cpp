// A multivector over a type that is not an algebra tag: int carries no
// metric. This file must not compile, and the diagnostic must name the
// constraint.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
// mv is used below; its failed constraint hides that use from the check.
// NOLINTNEXTLINE(misc-include-cleaner)
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

inline constexpr auto line_layout =
    galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;

[[maybe_unused]] constexpr galoot::core::mv<int, line_layout, double> wrong{};

} // namespace
