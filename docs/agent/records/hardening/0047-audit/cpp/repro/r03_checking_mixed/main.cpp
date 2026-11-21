// Calls the checked unit with n = 40, which its check must reject. Whether
// the program aborts depends on which unit's definition of the inline
// function the linker keeps.
#include <cstddef>
#include <cstdio>
#include <galoot/core/bits.hpp>
galoot::core::mask_t complement_in_checked_unit(galoot::core::mask_t a,
                                                std::size_t n);
galoot::core::mask_t complement_in_unchecked_unit(galoot::core::mask_t a,
                                                  std::size_t n);
int main(int argc, char**) {
  const std::size_t n = 39 + static_cast<std::size_t>(argc);  // 40
  std::printf("checked unit returned %u\n",
              complement_in_checked_unit(1, n));
  std::printf("unchecked unit returned %u\n",
              complement_in_unchecked_unit(1, n % 8));
  return 0;
}
