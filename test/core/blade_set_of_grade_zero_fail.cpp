// A grade of an algebra with no generators.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto empty = galoot::core::of_grade<0, 0>;

} // namespace
