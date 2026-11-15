#ifndef GALOOT_CORE_ASSERT_HPP
#define GALOOT_CORE_ASSERT_HPP

// Preconditions that the type system cannot express.
//
// GALOOT_CHECKING selects the checking mode. It is 1 for a checking build and
// 0 for an unchecked one. When it has no value, this header uses 0, so that a
// project that only adds Galoot's headers gets an unchecked build without
// configuring anything. Galoot's own builds give it a value in every preset.
#ifndef GALOOT_CHECKING
// A macro, because users set the mode with -D and the preprocessor tests it.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define GALOOT_CHECKING 0
#endif

#if GALOOT_CHECKING != 0 && GALOOT_CHECKING != 1
#error "GALOOT_CHECKING must be 0 or 1"
#endif

// GALOOT_FORCE_INLINE selects whether the core forces its operations inline
// (GALOOT_ALWAYS_INLINE, below). It is 1, the default, to force them, and 0
// to leave inlining to the compiler's own heuristics. Forcing them makes
// every call of an operation carry the operation's body, so a unit with many
// calls, such as a test that checks many identities, can take much longer
// and much more memory to compile, most of all with sanitizers or at -O2 on
// Clang. With 0 such a unit compiles faster, and an operation may then
// compile to a call. Over an exact scalar the values do not depend on the
// mode, nor over a rounding scalar without multiply-add contraction
// (-ffp-contract=off); with contraction, which products a compiler fuses can
// depend on what it inlines. Every translation unit of a program must use the
// same mode, since the two modes give the core's inline functions different
// definitions.
#ifndef GALOOT_FORCE_INLINE
// A macro, because users set the mode with -D and the preprocessor tests it.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define GALOOT_FORCE_INLINE 1
#endif

// The mode is pasted onto a name defined only for 0 and 1, because a numeric
// #if reads a name it does not know, such as ON, as 0, takes 01 and 0x1 for
// 1, and reports an empty value with an error of its own.
// NOLINTBEGIN(*-macro-usage,*-macro-to-enum): only the preprocessor can test
// the spelling of a macro's value, and #if cannot see an enumerator.
#define GALOOT_DETAIL_PASTE_(a, b) a##b
#define GALOOT_DETAIL_PASTE(a, b) GALOOT_DETAIL_PASTE_(a, b)
#define GALOOT_DETAIL_FORCE_INLINE_IS_0 1
#define GALOOT_DETAIL_FORCE_INLINE_IS_1 1
// NOLINTEND(*-macro-usage,*-macro-to-enum)
#if !GALOOT_DETAIL_PASTE(GALOOT_DETAIL_FORCE_INLINE_IS_, GALOOT_FORCE_INLINE)
#error "GALOOT_FORCE_INLINE must be 0 or 1"
#endif

#include <cstdio>
#include <cstdlib>

/// Makes a function inline at every call: [[gnu::always_inline]] on GCC
/// and Clang when GALOOT_FORCE_INLINE is 1, and nothing when it is 0 or on
/// another compiler.
///
/// The core puts it on each function between a public operation and the
/// generated code of the operation's terms, and on those public operations
/// that the structure test found out of line, so that an operation
/// compiles to the straight-line arithmetic of its terms at its call site, as
/// a hand-written form would. Left to its own heuristics, a compiler can keep
/// some of those functions out of line, and the call then takes a large
/// share of the operation's time. That no call is left is checked for a
/// list of the rga3 preset's operations, on its own types and on
/// multivectors of all sixteen of its blades, over double, at -O2 without
/// the checking mode, on x86-64, with GALOOT_FORCE_INLINE 1. It is not
/// promised elsewhere: with GALOOT_FORCE_INLINE 0, on other layouts
/// and algebras (some operations on conformal(3) multivectors of all or of even
/// blades still compile to a call), at other optimization levels such as -Os,
/// and in a checking build, where transform calls its checks out of line by
/// design, a compiler may still call an operation. It is not part of the
/// interface: a program neither uses nor defines it.
///
/// A function forced inline cannot be inlined into a caller compiled for
/// fewer instruction-set features than its translation unit, and both
/// compilers then stop with an error rather than call it. For example, in a
/// file built with -march=x86-64-v3, a fallback declared
/// [[gnu::target("arch=x86-64")]] that calls transform does not compile. Call
/// such an operation from a function built for the file's own target, or put
/// the fallback in a file of its own built for the fallback's target.
// A macro, because the attribute is written only for the compilers that
// know it, and only in the forced-inline mode.
#if GALOOT_FORCE_INLINE == 1 && (defined(__GNUC__) || defined(__clang__))
#define GALOOT_ALWAYS_INLINE [[gnu::always_inline]]
#else
#define GALOOT_ALWAYS_INLINE
#endif

namespace galoot::core::detail {

/// Reports a failed GALOOT_ASSERT and ends the program.
///
/// Requires: `message`, `file`, and `line` describe the failed assertion.
/// Guarantees: writes `galoot: assertion failed: <message> (<file>:<line>)`
/// to stderr and calls std::abort. It is not constexpr, so reaching it during
/// a constant evaluation ends that evaluation without a throw, and the core
/// needs no exceptions.
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

} // namespace galoot::core::detail

/// Checks a precondition: GALOOT_ASSERT(condition, message).
///
/// Requires: `message` is a string literal that states the precondition in the
/// terms of the algebra, contains no quote characters, and starts on the line
/// of `GALOOT_ASSERT(`, because a failed constant evaluation shows the message
/// only in the source line the compiler echoes.
/// Guarantees: during a constant evaluation, a false `condition` ends the
/// evaluation with a diagnostic that shows the line holding `message`. At run
/// time with GALOOT_CHECKING 1, a false `condition` reports `message` and
/// aborts. At run time with GALOOT_CHECKING 0, `condition` is not evaluated.
// A macro, because only a macro can leave the condition unevaluated at run
// time and capture the caller's file and line.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define GALOOT_ASSERT(condition, message)                                      \
  ((!::galoot::core::detail::checking_now() || static_cast<bool>(condition))   \
       ? static_cast<void>(0)                                                  \
       : ::galoot::core::detail::assertion_failed(message, __FILE__,           \
                                                  __LINE__))

#endif // GALOOT_CORE_ASSERT_HPP
