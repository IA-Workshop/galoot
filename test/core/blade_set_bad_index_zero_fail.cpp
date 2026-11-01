// A name with the index 0: generators count from e1.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto wrong = galoot::core::blades<4, "e1 e10">;

} // namespace
