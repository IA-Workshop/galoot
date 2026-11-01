// +e1 merged with e1 at a sign of 0, so the bad sign is in the second input
// on a blade the first input holds. A sign of 0 is neither orientation, so
// the two sets do not hold e1 with opposite signs.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

constexpr galoot::core::blade_set<1> unsigned_e1{{{{.mask = 0b1U, .sign = 0}}}};
[[maybe_unused]] constexpr auto merged =
    galoot::core::merge<galoot::core::blades<2, "e1">, unsigned_e1>;

} // namespace
