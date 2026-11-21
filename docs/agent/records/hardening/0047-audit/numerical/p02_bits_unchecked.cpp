// The public functions of bits.hpp and blade_set.hpp state preconditions that
// GALOOT_ASSERT checks. Built with GALOOT_CHECKING 0 and run under
// -fsanitize=undefined, a call that breaks one shows what a user who calls
// them at run time gets: a shift by the width of mask_t for n = 0 or n > 32,
// and a silently wrong name for a mask beyond e9. With GALOOT_CHECKING 1 the
// same calls abort with the assertion's message (the -DCHECKED build).
#include <cstddef>
#include <cstdio>

#include <galoot/core.hpp>

namespace core = galoot::core;

int main(int argc, char** argv) {
  // From the command line, so that the compiler sees no constant.
  const std::size_t zero = static_cast<std::size_t>(argc) - 1;
  const std::size_t n33 = zero + 33;
  const core::mask_t high = core::mask_t{1} << (zero + 9);
  (void)argv;
  // Unbuffered, so that the lines before an abort are kept.
  std::setvbuf(stdout, nullptr, _IONBF, 0);

  if (argc == 1) {
    std::printf("complement_mask(0b1, 0) = %#x\n",
                core::complement_mask(1U, zero));
    std::printf("complement_mask(0b1, 33) = %#x\n",
                core::complement_mask(1U, n33));
    std::printf("antigrade(0b1, 0) = %d\n", core::antigrade(1U, zero));
    std::printf("right_complement_sign(0b1, 0) = %d\n",
                core::right_complement_sign(1U, zero));
    const auto name = core::canonical_name(high);
    std::printf("canonical_name(1 << 9) = \"%.*s\"\n",
                static_cast<int>(name.text().size()), name.text().data());
    const auto metric = core::R<3, 0, 0>();
    std::printf("R<3,0,0>().is_null(3) read %d\n",
                static_cast<int>(metric.is_null(zero + 3)));
  }
  return 0;
}
