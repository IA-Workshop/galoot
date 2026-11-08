// The checking mode set to 2, which is neither 0 nor 1, before a core header
// is included. The build passes its own mode with -D, so it is replaced
// here. This file must not compile.
#undef GALOOT_CHECKING
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage): the mode a user would set.
#define GALOOT_CHECKING 2

#include <galoot/core/assert.hpp>
