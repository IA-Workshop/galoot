// A set written out by hand that holds e12 with both signs. The fault is in
// one input, so the message about the two sets must not appear.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

constexpr galoot::core::blade_set<2> both_e12{
    {{{.mask = 0b11U, .sign = 1}, {.mask = 0b11U, .sign = -1}}}};
[[maybe_unused]] constexpr auto merged =
    galoot::core::merge<both_e12, galoot::core::blades<2, "e1">>;

} // namespace
