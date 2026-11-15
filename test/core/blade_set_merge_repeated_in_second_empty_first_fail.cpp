// An empty first input and a second input that lists e2 with both signs: the
// fault is a repeat in the second input, not a conflict between the two.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

constexpr galoot::core::blade_set<0> no_blades{};
constexpr galoot::core::blade_set<2> both_e2{
    {{{.mask = 0b10U, .sign = 1}, {.mask = 0b10U, .sign = -1}}}};
[[maybe_unused]] constexpr auto merged =
    galoot::core::merge<no_blades, both_e2>;

} // namespace
