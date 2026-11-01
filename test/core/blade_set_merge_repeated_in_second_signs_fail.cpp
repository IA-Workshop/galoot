// A second input that lists e1 with both signs, where the first input holds
// +e1. The fault is a repeat in the second input; the message about the two
// sets must not appear in place of it.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

constexpr galoot::core::blade_set<2> both_e1{
    {{{.mask = 0b1U, .sign = 1}, {.mask = 0b1U, .sign = -1}}}};
[[maybe_unused]] constexpr auto merged =
    galoot::core::merge<galoot::core::blades<2, "e1">, both_e1>;

} // namespace
