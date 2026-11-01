// A second input that lists e1 with both signs, where the first input holds
// +e1. The fault is a repeat in the second input; the message about the two
// sets must not appear in place of it.
// This file must not compile.
#include <array>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

namespace {

struct r200 {
  static constexpr auto value = galoot::core::R<2, 0, 0>();
};

constexpr auto both_e1_blades = std::to_array<galoot::core::oriented_blade>({
    {.mask = 0b1U, .sign = 1},
    {.mask = 0b1U, .sign = -1},
});
constexpr galoot::core::blade_set<2> both_e1{both_e1_blades};
[[maybe_unused]] constexpr auto merged =
    galoot::core::merge<galoot::core::blades<r200, "e1">, both_e1>;

} // namespace
