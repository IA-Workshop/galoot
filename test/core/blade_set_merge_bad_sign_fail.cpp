// A set written out by hand with a sign of 0, which is neither orientation.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

constexpr galoot::core::blade_set<1> unsigned_e1{{{{.mask = 0b1U, .sign = 0}}}};
[[maybe_unused]] constexpr auto merged =
    galoot::core::merge<unsigned_e1, galoot::core::blades<2, "e2">>;

} // namespace
