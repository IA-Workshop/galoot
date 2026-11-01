// Names in an algebra with no generators. The names are not read, so no
// message about e1 appears.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto empty = galoot::core::blades<0, "e1">;

} // namespace
