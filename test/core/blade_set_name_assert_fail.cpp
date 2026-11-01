// canonical_name of a blade with no compact name, at run time. Built with
// GALOOT_CHECKING=1, it must abort with the message
// (core.blade_set_name_assert).
#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>

int main(int argc, char** /*argv*/) {
  // argc is at least 1, so the mask holds e10; the compiler cannot know it,
  // so the check happens at run time.
  const auto mask = static_cast<galoot::core::mask_t>(argc) << 9U;
  return galoot::core::canonical_name(mask).length == 0 ? 1 : 0;
}
