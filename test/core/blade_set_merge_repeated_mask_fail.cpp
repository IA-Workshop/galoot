// A set written out by hand that holds e12 with both signs. The fault is in
// one input, so the message about the two sets must not appear.
// This file must not compile.
#include <array>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

namespace {

struct r200 {
  static constexpr auto value = galoot::core::R<2, 0, 0>();
};

constexpr auto both_e12_blades = std::to_array<galoot::core::oriented_blade>({
    {.mask = 0b11U, .sign = 1},
    {.mask = 0b11U, .sign = -1},
});
constexpr galoot::core::blade_set<2> both_e12{both_e12_blades};
[[maybe_unused]] constexpr auto merged =
    galoot::core::merge<both_e12, galoot::core::blades<r200, "e1">>;

} // namespace
