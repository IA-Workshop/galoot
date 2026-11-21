// Includes assert.hpp from two versions of Galoot in one translation unit,
// first FIRST and then SECOND, each given as a directory. The 1.0.1
// candidate's assert.hpp undefines its helper macro GALOOT_DETAIL_PASTE and
// 1.0.0's does not, so whether that macro is still defined tells which
// version's header the unit used.
#define GALOOT_STRING_(x) #x
#define GALOOT_STRING(x) GALOOT_STRING_(x)
#include GALOOT_STRING(FIRST/galoot/core/assert.hpp)
#include GALOOT_STRING(SECOND/galoot/core/assert.hpp)
#include <cstdio>

int main() {
#ifdef GALOOT_DETAIL_PASTE
  std::puts("the unit used 1.0.0's assert.hpp");
#else
  std::puts("the unit used the 1.0.1 candidate's assert.hpp");
#endif
  return 0;
}
