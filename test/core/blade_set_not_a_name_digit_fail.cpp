// A name that starts with a digit but is not 1, the scalar.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto wrong = galoot::core::blades<4, "e1 12">;

} // namespace
