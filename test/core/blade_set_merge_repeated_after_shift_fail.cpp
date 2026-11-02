// A first input that lists e2, then e1, then e2 again. Inserting e1 in front
// moves the listed e2 one place on, and the repeat must still be found there.
// This file must not compile.
#include <array>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

namespace {

struct r200 {
  static constexpr auto value = galoot::core::R<2, 0, 0>();
};

constexpr auto e2_e1_e2_blades = std::to_array<galoot::core::oriented_blade>({
    {.mask = 0b10U, .sign = 1},
    {.mask = 0b01U, .sign = 1},
    {.mask = 0b10U, .sign = 1},
});
constexpr galoot::core::blade_set<3> e2_e1_e2{e2_e1_e2_blades};
[[maybe_unused]] constexpr auto merged =
    galoot::core::merge<e2_e1_e2, galoot::core::blades<r200, "e12">>;

} // namespace
