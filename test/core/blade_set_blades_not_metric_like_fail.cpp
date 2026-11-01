// Names read in a type that is not an algebra tag: int carries no metric.
// This file must not compile, and the diagnostic must name the constraint.
// blades is used below; its failed constraint hides that use from the check.
// NOLINTNEXTLINE(misc-include-cleaner)
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto wrong = galoot::core::blades<int, "e1">;

} // namespace
