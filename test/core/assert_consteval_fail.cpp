// A failing GALOOT_ASSERT in a constant evaluation. The failed assertion
// calls a function that is not constexpr, so the evaluation stops there:
// this file must not compile, and the diagnostic must show the assertion's
// message (core.assert_consteval).
#include <galoot/core/assert.hpp>

namespace {

constexpr int checked_half(int x) {
  GALOOT_ASSERT(x % 2 == 0, "assert_consteval_fail: odd input to checked_half");
  return x / 2;
}

static_assert(checked_half(3) == 1);

} // namespace
