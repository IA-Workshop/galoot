// A user of the core with no build system, who adds the headers to a
// project and nothing else: core.bare_compile compiles this file with only
// -std=c++23 -pedantic-errors -I include.
#include <galoot/core.hpp>
#include <galoot/core/assert.hpp>

int main() {
  GALOOT_ASSERT(true, "bare_user: a true condition");
  return 0;
}
