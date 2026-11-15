// Names read in a tag whose member value is a number, not a metric.
// This file must not compile.
// blades is used below; its failed check hides that use from the check.
// NOLINTNEXTLINE(misc-include-cleaner)
#include <galoot/core/blade_set.hpp>

namespace {

struct number_tag {
  static constexpr int value = 3;
};

[[maybe_unused]] constexpr auto wrong = galoot::core::blades<number_tag, "e1">;

} // namespace
