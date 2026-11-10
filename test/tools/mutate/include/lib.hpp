#ifndef LIB_HPP
#define LIB_HPP

#include <concepts>

namespace lib {

// Twice x, by a different route at compile time than at run time, so that a
// test can change one route and leave the other.
constexpr int twice(int x) {
  if consteval {
    return 2 * x;
  } else {
    return x << 1;
  }
}

// Any argument but an int is refused at compile time.
template <typename T>
  requires(!std::same_as<T, int>)
constexpr int twice(T) {
  static_assert(false, "twice: the argument must be an int");
  return 0;
}

// Half of x, rounded toward zero. A template, so that only a program that
// calls it compiles its body.
template <typename T>
constexpr T halve(T x) {
  return x / 2;
}

// Not constexpr, so a constant evaluation that reaches it fails: the way
// checked() rejects a negative argument at compile time.
inline void reject_negative() {}

// x, which must not be negative.
constexpr int checked(int x) {
  if (x < 0) {
    reject_negative();
  }
  return x;
}

// Not constexpr either: how below_eight() rejects a large argument.
inline void reject_large() {}

// x, which must be below 8.
constexpr int below_eight(int x) {
  if (x >= 8) {
    reject_large();
  }
  return x;
}

// below_eight(N), computed once at compile time in a constexpr variable of
// the header, as a library keeps a table: a guard that fails there stops the
// build at this header's line, not at the test's. The entry after it is
// checked the same way, so that one build can fail at both.
template <int N>
constexpr int below_eight_table() {
  static constexpr int value = below_eight(N);
  static constexpr int next = below_eight(N + 1);
  return next == value + 1 ? value : -1;
}

} // namespace lib

#endif
