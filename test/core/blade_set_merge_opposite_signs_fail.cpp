// e13 merged with e31 = -e13: the merged set could keep only one of the two
// orientations.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto both =
    galoot::core::merge<galoot::core::blades<3, "e13 e2">,
                        galoot::core::blades<3, "e31">>;

} // namespace
