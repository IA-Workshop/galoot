// GCC 16.2.1 wrong code: after a constant evaluation writes into one element
// of a value-initialized array of a class type with default member
// initializers, a later run-time value-initialization of the same type in
// the same translation unit leaves the elements from the written one on
// zero-filled, so q{} reads 0/0 instead of 0/1. Clang 23.1.4 prints 0/1 six
// times in every variant. Galoot code is not involved.
//
// V = 1  struct box { std::array<q, 6> c{}; };            wrong on GCC
// V = 2  struct box { std::array<q, 6> c; };               right
// V = 3  one initializer per element, c{q{}, ..., q{}}     right
// V = 4  struct box { q c[6]{}; };                         wrong on GCC
//
// Build: g++ -std=c++23 -O0 -DV=1 reproducer.cpp && ./a.out
#include <array>
#include <cstdio>
#include <utility>

struct q {
  long n = 0;
  long d = 1;
};

#if V == 1
struct box {
  std::array<q, 6> c{};
};
#elif V == 2
struct box {
  std::array<q, 6> c;
};
#elif V == 3
template <std::size_t... I>
struct boxed {
  std::array<q, sizeof...(I)> c{((void)I, q{})...};
};
using box = boxed<0, 1, 2, 3, 4, 5>;
#elif V == 4
struct box {
  q c[6]{};
};
#endif

constexpr bool writes(int i) {
  box x{};
  x.c[static_cast<unsigned>(i)].n = 7;
  return x.c[static_cast<unsigned>(i)].n == 7;
}
static_assert(writes(3));

int main() {
  const box a{};
  for (const auto& x : a.c) {
    std::printf("%ld/%ld ", x.n, x.d);
  }
  std::printf("\n");
}
