// Names read in a type that is not an algebra tag: int carries no metric.
// This file must not compile, and the diagnostic must say that the tag is
// not an algebra.
// blades is used below; its failed check hides that use from the check.
// NOLINTNEXTLINE(misc-include-cleaner)
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto wrong = galoot::core::blades<int, "e1">;

} // namespace
