// Smoke test of the build skeleton (plan 0002, section 4.5): the doctest
// runner, the labels, the compiler launcher, and both core headers work
// together.
#include <galoot/core.hpp> // IWYU pragma: keep; the umbrella must compile here
#include <galoot/core/assert.hpp>

#include <doctest/doctest.h>

namespace {

constexpr int checked_identity(int x) {
  GALOOT_ASSERT(x >= 0, "smoke: negative input to checked_identity");
  return x;
}

// A true condition passes a constant evaluation (decision 0012).
static_assert(checked_identity(1) == 1);

} // namespace

TEST_CASE("GALOOT_ASSERT with a true condition returns normally at run time "
          "(STYLE S9; plan 0002, section 4.5)") {
  // A CHECK argument is not manifestly constant-evaluated, so the call
  // takes the run-time branch of GALOOT_ASSERT.
  CHECK(checked_identity(2) == 2);
}
