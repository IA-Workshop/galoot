// A name that starts with a letter other than e.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto wrong = galoot::core::blades<4, "e1 x2">;

} // namespace
