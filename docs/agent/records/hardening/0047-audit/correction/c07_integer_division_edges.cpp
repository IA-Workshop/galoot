// x / s over integer scalars at the two edges the declaration of operator/
// states: a zero divisor, and min() / -1.
//
//   c07 zero   x / 0 over int. With GALOOT_CHECKING 1 the assertion aborts
//              with its message; with 0 the division is undefined behavior.
//   c07 min    min() / -1 over int, the scalar's own overflow, undefined
//              behavior in either mode, and over short, where the quotient
//              is computed in int and converted back to min().
#include <climits>
#include <cstdio>
#include <cstring>

#include <galoot/core.hpp>

namespace core = galoot::core;

struct r200 {
  static constexpr auto value = core::R<2, 0, 0>();
};
template <class T>
using Vector = core::mv<r200, core::blades<r200, "e1 e2">, T>;

int main(int argc, char** argv) {
  if (argc != 2) {
    return 2;
  }
  // Unbuffered, so that a line printed before a trap is not lost.
  std::setvbuf(stdout, nullptr, _IONBF, 0);
  // Read from argc, so that no division is a constant evaluation.
  const int zero = argc - 2;
  if (std::strcmp(argv[1], "zero") == 0) {
    const auto y = Vector<int>{7, -7} / zero;
    std::printf("(7, -7) / 0 over int returned (%d, %d)\n", y.coefficients[0],
                y.coefficients[1]);
  } else {
    const auto minus_one = static_cast<short>(zero - 1);
    const auto s = Vector<short>{SHRT_MIN, 0} / minus_one;
    std::printf("SHRT_MIN / -1 over short returned %d\n", s.coefficients[0]);
    const auto y = Vector<int>{INT_MIN, 0} / (zero - 1);
    std::printf("INT_MIN / -1 over int returned %d\n", y.coefficients[0]);
  }
  return 0;
}
