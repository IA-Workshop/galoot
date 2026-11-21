// Whether transform's rounding check rejects a versor when the rounding
// direction is not to nearest, which its bound assumes. Under a directed
// mode an operation's error can be twice that of rounding to nearest, and
// errors of one sign add up instead of cancelling, so the question is
// whether a dropped coefficient of a true versor can exceed its tolerance.
//
// The inputs are built to make each rounding as large as it can be: every
// coefficient is a power of two times 1 + k 2^-52 for a small k, so that
// each product lies just above a power of two, where one unit in the last
// place is the largest fraction of the value, and almost every product is
// inexact. v is an even element of R(3,0,0), a rotor up to scale whatever
// its coefficients, so the dropped e123 coefficient of v on a vector x is
// exactly zero and what is computed is rounding error alone.
//
//   c06 near|up|down|zero <trials>
//
// Prints the largest ratio of the dropped coefficient to its tolerance, the
// comparison the check makes: above 1 the check would abort on a versor.
// Built with -frounding-math, so that the compiler does not fold the
// arithmetic under another mode.
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>

#include <galoot/core.hpp>

namespace core = galoot::core;
namespace detail = galoot::core::detail;

struct r300 {
  static constexpr auto value = core::R<3, 0, 0>();
};
inline constexpr auto vector = core::blades<r300, "e1 e2 e3">;
inline constexpr auto even = core::blades<r300, "1 e12 e13 e23">;

double ratio(const std::array<double, 4>& v, const std::array<double, 3>& x) {
  const auto middle = detail::transform_middle<r300, even, vector, true>(v, x);
  const auto dropped =
      detail::transform_outer<r300, even, vector, false>(middle, v);
  const auto tolerances =
      detail::transform_dropped_tolerances<r300, even, vector>(v, x);
  return std::fabs(dropped[0]) / tolerances[0];
}

int main(int argc, char** argv) {
  if (argc != 3) {
    std::fputs("usage: c06 near|up|down|zero <trials>\n", stderr);
    return 2;
  }
  const char* mode = argv[1];
  const int rounding = std::strcmp(mode, "up") == 0     ? FE_UPWARD
                       : std::strcmp(mode, "down") == 0 ? FE_DOWNWARD
                       : std::strcmp(mode, "zero") == 0 ? FE_TOWARDZERO
                                                        : FE_TONEAREST;
  const long trials = std::atol(argv[2]);
  std::mt19937_64 random(20261121);
  std::uniform_int_distribution<int> low_bits(0, 4095);
  std::uniform_int_distribution<int> sign(0, 1);
  std::uniform_int_distribution<int> exponent(-1, 1);
  // One draw per statement, so that both compilers draw in the same order.
  const auto draw = [&] {
    const double mantissa = 1.0 + (low_bits(random) * 0x1p-52);
    const double magnitude = std::ldexp(mantissa, exponent(random));
    return sign(random) == 1 ? magnitude : -magnitude;
  };

  std::fesetround(rounding);
  double largest = 0;
  long over = 0;
  for (long t = 0; t < trials; ++t) {
    const std::array<double, 4> v{draw(), draw(), draw(), draw()};
    const std::array<double, 3> x{draw(), draw(), draw()};
    const double r = ratio(v, x);
    largest = r > largest ? r : largest;
    over += r > 1 ? 1 : 0;
  }
  std::fesetround(FE_TONEAREST);
  std::printf("rounding %s: roundings K = %d, largest |d|/tol %.3f, over 1 in "
              "%ld of %ld\n",
              mode,
              detail::transform_dropped_roundings<r300, even, vector>[0],
              largest, over, trials);
  return 0;
}
