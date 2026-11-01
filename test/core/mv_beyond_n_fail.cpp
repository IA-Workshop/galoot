// A layout written out by hand whose last blade is e5, in rigid(3), which
// has four generators.
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
    {.mask = 0b00001, .sign = 1},
    {.mask = 0b00010, .sign = 1},
    {.mask = 0b10000, .sign = 1},
});
constexpr galoot::core::blade_set<3> layout{blades};
[[maybe_unused]] constexpr galoot::core::mv<rga3, layout, double> wrong{};

} // namespace
