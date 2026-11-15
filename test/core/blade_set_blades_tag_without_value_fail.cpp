// Names read in a tag that has no member value, so it carries no metric.
// This file must not compile.
// blades is used below; its failed check hides that use from the check.
// NOLINTNEXTLINE(misc-include-cleaner)
#include <galoot/core/blade_set.hpp>

namespace {

struct no_metric {};

[[maybe_unused]] constexpr auto wrong = galoot::core::blades<no_metric, "e1">;

} // namespace
