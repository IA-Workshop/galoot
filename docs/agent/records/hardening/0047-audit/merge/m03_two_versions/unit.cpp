// One unit of a program whose units include different versions of Galoot.
// Built with -DUNIT=<name> and the include directory of one version. It
// calls antireverse_sign and complement_mask, inline functions that both
// versions define: their bodies differ between 1.0.0 and the 1.0.1
// candidate only in a local name, and in the edited copy only in the
// message of complement_mask's check.
#include <cstddef>

#include <galoot/core/bits.hpp>

int UNIT(galoot::core::mask_t a, std::size_t n) {
  return galoot::core::antireverse_sign(a, n) *
         static_cast<int>(galoot::core::complement_mask(a, n));
}
