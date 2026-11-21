// Built with -DGALOOT_CHECKING=1: this unit expects complement_mask to check
// that n is from 1 to 32.
#include <cstddef>
#include <galoot/core/bits.hpp>
galoot::core::mask_t complement_in_checked_unit(galoot::core::mask_t a,
                                                std::size_t n) {
  return galoot::core::complement_mask(a, n);
}
