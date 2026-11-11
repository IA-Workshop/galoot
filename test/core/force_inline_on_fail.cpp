// The forced-inline mode set to ON, the spelling of the build's own option,
// before a core header is included. ON is a name the preprocessor does not
// know, and a plain #if reads such a name as 0. A build may pass its own mode
// with -D, so it is replaced here. This file must not compile.
#undef GALOOT_FORCE_INLINE
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage): the mode a user would set.
#define GALOOT_FORCE_INLINE ON

#include <galoot/core/assert.hpp>
