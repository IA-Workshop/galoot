// The public functions of bits.hpp given n outside 1 to 32, at run time in an
// unchecked build (GALOOT_CHECKING 0, the default). Their comments say the
// precondition is "checked by GALOOT_ASSERT"; in this mode it is not, and the
// shift in low_bits is undefined. Build with -fsanitize=undefined to see it.
#include <cstddef>
#include <cstdio>
#include <galoot/core/bits.hpp>
int main(int argc, char**) {
  const std::size_t zero = static_cast<std::size_t>(argc) - 1;  // 0
  const std::size_t forty = 39 + static_cast<std::size_t>(argc); // 40
  std::printf("complement_mask(1, 0) = %u\n",
              galoot::core::complement_mask(1, zero));
  std::printf("complement_mask(1, 40) = %u\n",
              galoot::core::complement_mask(1, forty));
  std::printf("right_complement_sign(1, 0) = %d\n",
              galoot::core::right_complement_sign(1, zero));
  std::printf("antigrade(1, 40) = %d\n", galoot::core::antigrade(1, forty));
  return 0;
}
