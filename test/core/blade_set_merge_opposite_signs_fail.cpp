// e13 merged with e31 = -e13: the merged set could keep only one of the two
// orientations.
// This file must not compile.
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

namespace {

struct r300 {
  static constexpr auto value = galoot::core::R<3, 0, 0>();
};

[[maybe_unused]] constexpr auto both =
    galoot::core::merge<galoot::core::blades<r300, "e13 e2">,
                        galoot::core::blades<r300, "e31">>;

} // namespace
