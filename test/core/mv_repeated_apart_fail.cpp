// A layout written out by hand that lists e2, e1, e2: the repeat is two
// places apart.
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
    {.mask = 0b0010, .sign = 1},
    {.mask = 0b0001, .sign = 1},
    {.mask = 0b0010, .sign = 1},
});
constexpr galoot::core::blade_set<3> layout{blades};
[[maybe_unused]] constexpr galoot::core::mv<rga3, layout, double> wrong{};

} // namespace
