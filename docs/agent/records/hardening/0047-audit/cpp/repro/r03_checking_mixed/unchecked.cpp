// Built with -DGALOOT_CHECKING=0: the same inline function, defined without
// the run-time check.
#include <cstddef>
#include <galoot/core/bits.hpp>
galoot::core::mask_t complement_in_unchecked_unit(galoot::core::mask_t a,
                                                  std::size_t n) {
  return galoot::core::complement_mask(a, n);
}
