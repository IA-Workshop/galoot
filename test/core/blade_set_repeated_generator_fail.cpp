// A name that lists e1 twice: e1 e2 e1 is not a basis blade of grade 3, so
// e121 names none.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto wrong = galoot::core::blades<4, "e121">;

} // namespace
