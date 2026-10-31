// A failing GALOOT_ASSERT in a constant evaluation (plan 0002, section 4.5;
// decision 0012). This file must not compile, and the diagnostic must show
// the assertion's message (core.assert_consteval).
#include <galoot/core/assert.hpp>

namespace {

constexpr int checked_half(int x) {
  GALOOT_ASSERT(x % 2 == 0, "assert_consteval_fail: odd input to checked_half");
  return x / 2;
}

static_assert(checked_half(3) == 1);

} // namespace
