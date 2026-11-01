// e4 in an algebra of three generators.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto wrong = galoot::core::blades<3, "e1 e14">;

} // namespace
