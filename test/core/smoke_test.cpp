// Smoke test of the build: the doctest runner, the test labels, the compiler
// launcher, and the core headers work together.
#include <galoot/core.hpp> // IWYU pragma: keep; the umbrella must compile here
#include <galoot/core/assert.hpp>

#include <doctest/doctest.h>

namespace {

constexpr int checked_identity(int x) {
  GALOOT_ASSERT(x >= 0, "smoke: negative input to checked_identity");
  return x;
}

// A true condition never reaches the non-constexpr assertion_failed, so the
// call is still a constant expression.
static_assert(checked_identity(1) == 1);

} // namespace

// Reference: contract: GALOOT_ASSERT acts only on a false condition, as its
//   comment in galoot/core/assert.hpp states, so a true one returns normally
TEST_CASE("GALOOT_ASSERT with a true condition returns normally at run time") {
  // A CHECK argument is not manifestly constant-evaluated, so the call
  // takes the run-time branch of GALOOT_ASSERT.
  CHECK(checked_identity(2) == 2);
}

// Reference: contract: checking_now() is true at run time exactly when
//   GALOOT_CHECKING is 1, as its comment in galoot/core/assert.hpp states
TEST_CASE("GALOOT_ASSERT checks at run time exactly when the build "
          "configuration turns checking on") {
  CHECK(galoot::core::detail::checking_now() ==
        (GALOOT_EXPECTED_CHECKING == 1));
}
