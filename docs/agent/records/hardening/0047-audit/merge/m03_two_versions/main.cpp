// Calls the units of two versions with n = 40, which complement_mask's
// check rejects in a checking build, so the message printed names the
// definition the linker kept.
#include <cstddef>
#include <cstdio>

#include <galoot/core/bits.hpp>

int old_unit(galoot::core::mask_t a, std::size_t n);
int new_unit(galoot::core::mask_t a, std::size_t n);

int main(int argc, char**) {
  const std::size_t n = argc == 1 ? 3 : 40;
  std::printf("old %d, new %d\n", old_unit(1, n), new_unit(1, n));
  return 0;
}
