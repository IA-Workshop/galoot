// A user of the core with no build system (constitution C13; plan 0001,
// DoD-6): core.bare_compile compiles this file with only -std=c++23
// -pedantic-errors -I include.
#include <galoot/core.hpp>
#include <galoot/core/assert.hpp>

int main() {
  GALOOT_ASSERT(true, "bare_user: a true condition");
  return 0;
}
