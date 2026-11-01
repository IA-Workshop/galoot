// A list of names that names no blade.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto none = galoot::core::blades<4, "   ">;

} // namespace
