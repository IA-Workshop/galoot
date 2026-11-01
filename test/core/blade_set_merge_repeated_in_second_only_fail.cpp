// A second input that lists e2 twice, where the first input lacks e2, so
// the repeat is found among the second input's own blades.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

constexpr galoot::core::blade_set<2> twice_e2{
    {{{.mask = 0b10U, .sign = 1}, {.mask = 0b10U, .sign = 1}}}};
[[maybe_unused]] constexpr auto merged =
    galoot::core::merge<galoot::core::blades<2, "e1">, twice_e2>;

} // namespace
