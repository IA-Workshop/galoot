// A set written out by hand with a sign of 0, given as the second input of a
// merge, so that the check must cover both inputs.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

constexpr galoot::core::blade_set<1> unsigned_e2{
    {{{.mask = 0b10U, .sign = 0}}}};
[[maybe_unused]] constexpr auto merged =
    galoot::core::merge<galoot::core::blades<2, "e1">, unsigned_e2>;

} // namespace
