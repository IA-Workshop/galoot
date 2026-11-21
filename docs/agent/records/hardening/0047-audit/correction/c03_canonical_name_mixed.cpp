// canonical_name at run time for blades with a generator beyond e9. Built
// with GALOOT_CHECKING 0 the precondition is not checked, and the loop
// spells only e1 to e9, so a blade wholly beyond e9 comes out as "e" and a
// blade with generators on both sides of e9 as the name of another blade.
// Built with GALOOT_CHECKING 1 the first call aborts.
#include <cstdio>

#include <galoot/core/blade_set.hpp>

namespace core = galoot::core;

void show(const char* blade, core::mask_t mask) {
  const auto name = core::canonical_name(mask);
  std::printf("canonical_name(%s) = \"%.*s\"\n", blade,
              static_cast<int>(name.text().size()), name.text().data());
}

int main(int argc, char**) {
  // Shifted by argc - 1 = 0, so that no call is a constant evaluation.
  const auto shift = static_cast<unsigned>(argc - 1);
  show("e10", core::mask_t{0x200} << shift);
  show("e10 e11", core::mask_t{0x600} << shift);
  show("e1 e10", core::mask_t{0x201} << shift);
  show("e2 e9 e12", core::mask_t{0x902} << shift);
  show("e1 (for comparison)", core::mask_t{0x001} << shift);
  return 0;
}
