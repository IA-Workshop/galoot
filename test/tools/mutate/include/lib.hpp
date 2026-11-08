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

} // namespace lib

#endif
