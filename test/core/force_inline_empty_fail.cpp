// The forced-inline mode defined with no value, as -DGALOOT_FORCE_INLINE=
// defines it, before a core header is included. A build may pass its own
// mode with -D, so it is replaced here. This file must not compile.
#undef GALOOT_FORCE_INLINE
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage): the mode a user would set.
#define GALOOT_FORCE_INLINE

#include <galoot/core/assert.hpp>
