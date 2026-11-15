// The position of e3 in the blades of two generators, which do not hold it.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto at =
    galoot::core::index_of<galoot::core::full<2>, 0b100U>;

} // namespace
