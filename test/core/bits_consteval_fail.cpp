// A mask beyond the algebra in a constant evaluation of galoot/core/bits.hpp.
// This file must not compile, and the diagnostic must show the precondition's
// message (core.bits_consteval).
#include <galoot/core/bits.hpp>

// e3 is not a generator of an algebra of two generators.
static_assert(galoot::core::antigrade(0b100U, 2) == -1);
