// Every blade of an algebra with more generators than a mask has bits.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

namespace {

[[maybe_unused]] constexpr auto wide =
    galoot::core::full<galoot::core::mask_bits + 1>;

} // namespace
