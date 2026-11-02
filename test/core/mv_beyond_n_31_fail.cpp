// A layout written out by hand that holds e32 in R(31, 0, 0), whose
// generators are e1 to e31: the mask's top bit, one past the algebra.
// This file must not compile.
#include <array>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace {

struct r31 {
  static constexpr auto value = galoot::core::R<31, 0, 0, 31>();
};

constexpr auto blades = std::to_array<galoot::core::oriented_blade>({
    {.mask = 0b1, .sign = 1},
    {.mask = galoot::core::mask_t{1} << 31U, .sign = 1},
});
constexpr galoot::core::blade_set<2> layout{blades};
[[maybe_unused]] constexpr galoot::core::mv<r31, layout, double> wrong{};

} // namespace
