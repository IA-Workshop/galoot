// A layout written out by hand that holds e14 and e41 = -e14: one blade in
// both orientations, so the check must compare masks, not signs.
// This file must not compile.
#include <array>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct rga3 {
  static constexpr auto value = galoot::core::rigid<3>();
};

constexpr auto blades = std::to_array<galoot::core::oriented_blade>({
    {.mask = 0b1001, .sign = 1},
    {.mask = 0b1001, .sign = -1},
});
constexpr galoot::core::blade_set<2> layout{blades};
[[maybe_unused]] constexpr galoot::core::mv<rga3, layout, double> wrong{};

} // namespace
