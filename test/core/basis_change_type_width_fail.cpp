// A change with more generators than a blade mask has bits cannot be
// formed, even by hand. This file must not compile.
#include <galoot/core/basis_change.hpp>
#include <galoot/core/bits.hpp>

namespace {

constexpr auto too_wide = galoot::core::mask_bits + 1;

struct wide_change {
  static constexpr galoot::core::basis_change<too_wide> value{};
};

} // namespace
