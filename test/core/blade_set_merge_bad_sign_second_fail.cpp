// A set written out by hand with a sign of 0, given as the second input of a
// merge, so that the check must cover both inputs.
// This file must not compile.
#include <array>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

namespace {

struct r200 {
  static constexpr auto value = galoot::core::R<2, 0, 0>();
};

constexpr auto unsigned_e2_blades =
    std::to_array<galoot::core::oriented_blade>({
        {.mask = 0b10U, .sign = 0},
    });
constexpr galoot::core::blade_set<1> unsigned_e2{unsigned_e2_blades};
[[maybe_unused]] constexpr auto merged =
    galoot::core::merge<galoot::core::blades<r200, "e1">, unsigned_e2>;

} // namespace
