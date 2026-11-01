// One name given twice.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto twice = galoot::core::blades<4, "e12 e3 e12">;

} // namespace
