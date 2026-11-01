// The position of e1 in a set written out by hand with a sign of 2.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

constexpr galoot::core::blade_set<2> doubled_e2{
    {{{.mask = 0b01U, .sign = 1}, {.mask = 0b10U, .sign = 2}}}};
[[maybe_unused]] constexpr auto at = galoot::core::index_of<doubled_e2, 0b01U>;

} // namespace
