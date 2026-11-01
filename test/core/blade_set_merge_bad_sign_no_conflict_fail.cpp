// e1 with a sign of 0 merged with +e1. A sign of 0 is neither orientation, so
// the two sets do not hold e1 with opposite signs, and that message must not
// appear.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

constexpr galoot::core::blade_set<1> unsigned_e1{{{{.mask = 0b1U, .sign = 0}}}};
[[maybe_unused]] constexpr auto merged =
    galoot::core::merge<unsigned_e1, galoot::core::blades<2, "e1">>;

} // namespace
