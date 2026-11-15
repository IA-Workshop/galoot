// What GALOOT_ALWAYS_INLINE expands to in each forced-inline mode. The build
// compiles this file three times: with GALOOT_FORCE_INLINE 0, with no value,
// so that the header's default applies, and with the mode the build preset
// declares. Each program is told the mode it should see by
// GALOOT_EXPECTED_FORCE_INLINE, passed apart from the mode itself, so that a
// mode lost on the way fails the test.
#include <string_view>

#include <galoot/core/assert.hpp>

#include <doctest/doctest.h>

// The tokens of a macro's expansion as a string literal: the inner macro
// stringizes its arguments, and the outer one expands them first. An empty
// expansion gives "".
// NOLINTBEGIN(cppcoreguidelines-macro-usage): only a macro can read another
// macro's expansion.
#define GALOOT_TEST_STRINGIZE(...) #__VA_ARGS__
#define GALOOT_TEST_EXPANSION(...) GALOOT_TEST_STRINGIZE(__VA_ARGS__)
// NOLINTEND(cppcoreguidelines-macro-usage)

namespace {

// A function rather than a variable, since a variable initialized from ""
// reads as a redundant initialization to the static checks.
constexpr std::string_view expansion() {
  return GALOOT_TEST_EXPANSION(GALOOT_ALWAYS_INLINE);
}

} // namespace

// Reference: contract: GALOOT_ALWAYS_INLINE is [[gnu::always_inline]] on GCC
//   and Clang when GALOOT_FORCE_INLINE is 1, its default, and nothing when it
//   is 0, as the comments in galoot/core/assert.hpp state
TEST_CASE("GALOOT_ALWAYS_INLINE is the always_inline attribute exactly when "
          "the forced-inline mode is on") {
  CAPTURE(expansion());
  if constexpr (GALOOT_EXPECTED_FORCE_INLINE == 1) {
    CHECK(expansion() == "[[gnu::always_inline]]");
  } else {
    CHECK(expansion().empty());
  }
}
