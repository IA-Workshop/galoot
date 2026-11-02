// A second input that lists e2 twice, where the first input lacks e2, so
// the repeat is found among the second input's own blades.
// This file must not compile.
#include <array>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

namespace {

struct r200 {
  static constexpr auto value = galoot::core::R<2, 0, 0>();
};

constexpr auto twice_e2_blades = std::to_array<galoot::core::oriented_blade>({
    {.mask = 0b10U, .sign = 1},
    {.mask = 0b10U, .sign = 1},
});
constexpr galoot::core::blade_set<2> twice_e2{twice_e2_blades};
[[maybe_unused]] constexpr auto merged =
    galoot::core::merge<galoot::core::blades<r200, "e1">, twice_e2>;

} // namespace
