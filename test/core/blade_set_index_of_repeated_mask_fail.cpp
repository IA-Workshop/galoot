// The position of e1 in a set written out by hand that holds e1 twice, so
// that it has no single position.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

constexpr galoot::core::blade_set<3> twice_e1{{{{.mask = 0b01U, .sign = 1},
                                                {.mask = 0b10U, .sign = 1},
                                                {.mask = 0b01U, .sign = 1}}}};
[[maybe_unused]] constexpr auto at = galoot::core::index_of<twice_e1, 0b01U>;

} // namespace
