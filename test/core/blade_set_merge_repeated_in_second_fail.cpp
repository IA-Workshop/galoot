// A second input that lists e1 twice, where the first input also holds e1:
// the repeat must be found in the second input although the merged list
// already holds e1 from the first.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

constexpr galoot::core::blade_set<2> twice_e1{
    {{{.mask = 0b1U, .sign = 1}, {.mask = 0b1U, .sign = 1}}}};
[[maybe_unused]] constexpr auto merged =
    galoot::core::merge<galoot::core::blades<2, "e1">, twice_e1>;

} // namespace
