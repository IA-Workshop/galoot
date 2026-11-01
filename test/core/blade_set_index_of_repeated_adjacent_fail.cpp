// The position of e1 in a set written out by hand that holds e1 in two
// neighboring places, so that the repeat is the next blade after it.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

constexpr galoot::core::blade_set<3> e2_e1_e1{{{{.mask = 0b10U, .sign = 1},
                                                {.mask = 0b01U, .sign = 1},
                                                {.mask = 0b01U, .sign = 1}}}};
[[maybe_unused]] constexpr auto at = galoot::core::index_of<e2_e1_e1, 0b01U>;

} // namespace
