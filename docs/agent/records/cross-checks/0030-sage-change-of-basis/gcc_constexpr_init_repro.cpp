// GCC 16.2.1 (20260810) gives wrong values at run time for a function that is
// also evaluated in a constant expression in the same translation unit.
// make() value-initializes a 3 x 3 array of a class whose default member
// initializers set d = 1, then stores three elements. Every element must
// have d = 1. Built with g++ -std=c++23 (or c++20), at -O0 or -O2, the
// run-time call returns two elements with d = 0; without the constexpr
// variable `once` it returns none, and with two stores instead of three it
// returns none. Clang 23.1.4 returns none in every case. Exit status 1 means
// the defect is present.
#include <array>
#include <cstdio>

struct fraction {
  long n = 0;
  long d = 1;
  constexpr fraction() = default;
  explicit constexpr fraction(long x) : n(x) {}
};

using matrix = std::array<std::array<fraction, 3>, 3>;

constexpr matrix make() {
  matrix t{};
  t[0][0] = fraction{1};
  t[1][1] = fraction{1};
  t[2][2] = fraction{1};
  return t;
}

#ifndef WITHOUT_CONSTANT_EVALUATION
constexpr matrix once = make();
#endif

int main() {
  const matrix t = make();
  int wrong = 0;
  for (const auto& row : t) {
    for (const fraction& x : row) {
      wrong += x.d != 1 ? 1 : 0;
    }
  }
  std::printf("elements with d != 1 at run time: %d of 9\n", wrong);
  return wrong == 0 ? 0 : 1;
}
