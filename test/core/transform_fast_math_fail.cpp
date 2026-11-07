// transform.hpp in a checking build under -ffast-math, which reassociates
// sums and may flush subnormal numbers to zero, so that the rounding check
// of what transform drops could reject a correct result: the header stops
// compilation with its error. Compiled with -ffast-math, this file must not
// compile; without it, it must.
#undef GALOOT_CHECKING
// The checking build, whatever the build that compiles this file.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define GALOOT_CHECKING 1

#include <galoot/core/transform.hpp>
