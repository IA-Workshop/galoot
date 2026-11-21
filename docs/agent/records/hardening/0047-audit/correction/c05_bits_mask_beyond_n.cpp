// The second stated precondition of the bit functions, that the mask names
// only e_1 to e_n, broken at run time with n itself in range. Built with
// GALOOT_CHECKING 0 nothing is checked and no shift is out of range, so
// there is no undefined behavior, only values that break the guarantees:
// antigrade below zero, and a complement that is not one. Built with
// GALOOT_CHECKING 1 the first call aborts.
#include <cstdio>

#include <galoot/core/bits.hpp>

namespace core = galoot::core;

int main(int argc, char**) {
  // n = 2 at run time, so that no call is a constant evaluation.
  const auto n = static_cast<std::size_t>(argc + 1);
  const core::mask_t e123 = 0b111;
  const core::mask_t e3 = 0b100;
  std::printf("antigrade(e123, 2) = %d (guarantee: 0 to 2)\n",
              core::antigrade(e123, n));
  const core::mask_t c = core::complement_mask(e3, n);
  std::printf("complement_mask(e3, 2) = %u, and e3 | it = %u (guarantee: 3, "
              "the antiscalar)\n",
              c, e3 | c);
  std::printf("complement_mask applied twice to e3 = %u (guarantee: 4, e3 "
              "again)\n",
              core::complement_mask(c, n));
  return 0;
}
