// A sum that does not fit std::int64_t, in a constant evaluation: 2^62 + 2^62
// is 2^63, one above the maximum. This file must not compile, and the
// diagnostic must show the rational's message.
#include <cstdint>

// The compile-fail tests compile with the library's include paths only, so
// the header is found beside this file.
#include "rational.hpp"

namespace {

using test_support::rational;

constexpr std::int64_t half = 4611686018427387904; // 2^62
static_assert(rational{half} + rational{half} == rational{});

} // namespace
