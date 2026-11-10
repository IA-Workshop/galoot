// Whether a plain deleted == (C++11, no C++26 message) shows the fix in the
// quoted source line of the compilers' note. This file must not compile.
#include <array>
#include <concepts>
template <int N> struct change {
  std::array<int, N> rows;
  bool operator==(const change&) const = delete; // compare with same_rows(a, b)
};
static_assert(!std::equality_comparable<change<2>>);
constexpr change<2> a{}, b{};
static_assert(a == b);
