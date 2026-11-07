// transform.hpp in an unchecked build under -ffast-math or
// -ffinite-math-only: the rounding check never runs at run time there, and
// a constant evaluation, where it always runs, is evaluated by the compiler
// in IEEE 754 arithmetic whatever the options, so the header stops only a
// checking build. Compiled with either option, this file must compile.
#undef GALOOT_CHECKING
// The unchecked build, whatever the build that compiles this file.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define GALOOT_CHECKING 0

#include <galoot/core/transform.hpp>
