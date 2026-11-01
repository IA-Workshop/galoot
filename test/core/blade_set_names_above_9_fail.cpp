// Compact names in an algebra of twelve generators, where e11 could mean
// e1 e1 or e11. The names are not read, so no message about them appears.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto wide = galoot::core::blades<12, "e1 e11">;

} // namespace
