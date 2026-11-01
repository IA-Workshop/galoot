// The compact name of e10, which has none: its index is two digits.
// This file must not compile.
#include <galoot/core/blade_set.hpp>

static_assert(galoot::core::canonical_name(0b10'0000'0000U).length == 3);
