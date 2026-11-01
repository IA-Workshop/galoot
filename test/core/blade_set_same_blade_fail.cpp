// Two names for one blade, e12 and e21 = -e12: a set holds each blade once.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto twice = galoot::core::blades<4, "e12 e21">;

} // namespace
