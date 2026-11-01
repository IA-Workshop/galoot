// A first input that lists e2, then e1, then e2 again. Inserting e1 in front
// moves the listed e2 one place on, and the repeat must still be found there.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

constexpr galoot::core::blade_set<3> e2_e1_e2{{{{.mask = 0b10U, .sign = 1},
                                                {.mask = 0b01U, .sign = 1},
                                                {.mask = 0b10U, .sign = 1}}}};
[[maybe_unused]] constexpr auto merged =
    galoot::core::merge<e2_e1_e2, galoot::core::blades<2, "e12">>;

} // namespace
