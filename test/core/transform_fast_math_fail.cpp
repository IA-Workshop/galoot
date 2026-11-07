// transform.hpp in a checking build under -ffast-math, which reassociates
// sums and may flush subnormal numbers to zero, so that the rounding check
// of what transform drops could reject a correct result, or under
// -ffinite-math-only, which lets the compiler assume that no value is a NaN
// or an infinity, so that the check could pass one: the header stops
// compilation with the error that names the option. Compiled with either
// option, this file must not compile; without both, it must.
#undef GALOOT_CHECKING
// The checking build, whatever the build that compiles this file.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define GALOOT_CHECKING 1

#include <galoot/core/transform.hpp>
