// A layout written out by hand that holds e1 with the sign 0.
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
    {.mask = 0b0001, .sign = 0},
});
constexpr galoot::core::blade_set<1> layout{blades};
[[maybe_unused]] constexpr galoot::core::mv<rga3, layout, double> wrong{};

} // namespace
