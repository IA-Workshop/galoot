// A product that does not fit std::int64_t, in a constant evaluation:
// 3037000500^2 is the smallest square above the maximum. This file must not
// compile, and the diagnostic must show the rational's message.

// The compile-fail tests compile with the library's include paths only, so
// the header is found beside this file.
#include "rational.hpp"

namespace {

using test_support::rational;

static_assert(rational{3037000500} * rational{3037000500} == rational{});

} // namespace
