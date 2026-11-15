// A second input that lists e1 twice, where the first input also holds e1:
// the repeat must be found in the second input although the merged list
// already holds e1 from the first.
// This file must not compile.
#include <array>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

namespace {

struct r200 {
  static constexpr auto value = galoot::core::R<2, 0, 0>();
};

constexpr auto twice_e1_blades = std::to_array<galoot::core::oriented_blade>({
    {.mask = 0b1U, .sign = 1},
    {.mask = 0b1U, .sign = 1},
});
constexpr galoot::core::blade_set<2> twice_e1{twice_e1_blades};
[[maybe_unused]] constexpr auto merged =
    galoot::core::merge<galoot::core::blades<r200, "e1">, twice_e1>;

} // namespace
