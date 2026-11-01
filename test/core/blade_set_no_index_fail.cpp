// A name that is e alone, with no generator.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto wrong = galoot::core::blades<4, "e1 e">;

} // namespace
