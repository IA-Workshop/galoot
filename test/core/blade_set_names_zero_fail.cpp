// Names in an algebra with no generators. A tag whose metric has no
// generators is not an algebra, so blades rejects it before any name is read,
// and no message about e1 appears.
// This file must not compile.
// blades is used below; its failed constraint hides that use from the check.
// NOLINTNEXTLINE(misc-include-cleaner)
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

namespace {

struct no_generators {
  static constexpr galoot::core::metric<0> value{};
};

[[maybe_unused]] constexpr auto empty =
    galoot::core::blades<no_generators, "e1">;

} // namespace
