#ifndef GALOOT_CORE_ASSERT_HPP
#define GALOOT_CORE_ASSERT_HPP

// Preconditions that the type system cannot express (STYLE S9).
//
// GALOOT_CHECKING selects the checking mode (constitution C8). It is 1 for a
// checking build and 0 for an unchecked one. When it has no value, this header
// uses 0, so that a project that only adds Galoot's headers gets an unchecked
// build (decision 0020). Galoot's own builds always give it a value, in each
// preset (plan 0002, section 4.4).
#ifndef GALOOT_CHECKING
// A macro, because users set the mode with -D and the preprocessor tests it.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define GALOOT_CHECKING 0
#endif

#if GALOOT_CHECKING != 0 && GALOOT_CHECKING != 1
#error "GALOOT_CHECKING must be 0 or 1"
#endif

#include <cstdio>
#include <cstdlib>

namespace galoot::detail {

/// Reports a failed GALOOT_ASSERT and ends the program.
///
/// Requires: `message`, `file`, and `line` describe the failed assertion.
/// Guarantees: writes `galoot: assertion failed: <message> (<file>:<line>)`
/// to stderr and calls std::abort. It is not constexpr, so reaching it during
/// a constant evaluation ends that evaluation without a throw (decision 0012).
[[noreturn]] inline void assertion_failed(const char* message, const char* file,
                                          int line) noexcept {
  // fprintf, because <print>, <format>, or <iostream> would add a heavy
  // include to every core header.
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,modernize-use-std-print)
  std::fprintf(stderr, "galoot: assertion failed: %s (%s:%d)\n", message, file,
               line);
  std::abort();
}

/// Whether GALOOT_ASSERT checks its condition where it is evaluated.
///
/// Guarantees: true during a constant evaluation, in every checking mode;
/// otherwise true exactly when GALOOT_CHECKING is 1.
[[nodiscard]] constexpr bool checking_now() noexcept {
  if consteval {
    return true;
  }
  return GALOOT_CHECKING == 1;
}

} // namespace galoot::detail

/// Checks a precondition: GALOOT_ASSERT(condition, message).
///
/// Requires: `message` is a string literal that states the precondition in the
/// terms of the algebra (C9), contains no quote characters, and starts on the
/// line of `GALOOT_ASSERT(`, so that a compile-fail test can match it in the
/// source line the compiler echoes (plan 0002, risk 8).
/// Guarantees: during a constant evaluation, a false `condition` ends the
/// evaluation with a diagnostic that shows the line holding `message`. At run
/// time with GALOOT_CHECKING 1, a false `condition` reports `message` and
/// aborts. At run time with GALOOT_CHECKING 0, `condition` is not evaluated.
// A macro, because only a macro can leave the condition unevaluated at run
// time and capture the caller's file and line.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define GALOOT_ASSERT(condition, message)                                      \
  ((!::galoot::detail::checking_now() || static_cast<bool>(condition))         \
       ? static_cast<void>(0)                                                  \
       : ::galoot::detail::assertion_failed(message, __FILE__, __LINE__))

#endif // GALOOT_CORE_ASSERT_HPP
