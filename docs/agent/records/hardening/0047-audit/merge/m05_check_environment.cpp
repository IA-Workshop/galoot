// transform's rounding check on versors, under floating-point environments
// its derivation does not assume. For each trial it computes what the check
// computes, the dropped coefficients d and their tolerances, through the
// same detail functions transform calls, and prints the largest |d| / tol
// and the number of trials whose ratio exceeds 1 (where the check would
// abort on a versor).
//
//   m05 <family> <scale of v> <scale of x> <ftz> <rounding>
//
// family r3: v a random even element of R(3, 0, 0), which is a rotor up to
//   scale whatever its rounding, so every dropped coefficient is exactly zero.
// family r4: v = a b for random vectors a, b of R(4, 0, 0), computed in
//   double: a versor whose coefficients were each rounded once.
// ftz 1 sets the processor's flush-to-zero and denormals-are-zero bits, as
//   -mdaz-ftz or -ffast-math's startup code would. rounding is near, up,
//   down or zero, set with fesetround (built with -frounding-math).
// Compile options under test (-fassociative-math and the others) are given
// on the command line of the build.
#include <cfenv>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <random>

#include <xmmintrin.h>

#include <galoot/core.hpp>

namespace core = galoot::core;
namespace detail = galoot::core::detail;

namespace {

struct r300 {
  static constexpr auto value = core::R<3, 0, 0>();
};
struct r400 {
  static constexpr auto value = core::R<4, 0, 0>();
};

template <class Alg, auto V, auto X>
double worst(const std::array<double, V.size()>& v,
             const std::array<double, X.size()>& x, bool& nonfinite) {
  const auto middle = detail::transform_middle<Alg, V, X, true>(v, x);
  const auto dropped = detail::transform_outer<Alg, V, X, false>(middle, v);
  const auto tolerances =
      detail::transform_dropped_tolerances<Alg, V, X>(v, x);
  double ratio = 0;
  for (std::size_t j = 0; j < dropped.size(); ++j) {
    if (!std::isfinite(tolerances[j])) {
      nonfinite = true;
    }
    const double r = std::fabs(dropped[j]) / tolerances[j];
    ratio = r > ratio ? r : ratio;
  }
  return ratio;
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 6) {
    std::fputs("usage: m05 r3|r4 scale_v scale_x ftz near|up|down|zero\n",
               stderr);
    return 2;
  }
  const bool r4 = std::strcmp(argv[1], "r4") == 0;
  const double sv = std::strtod(argv[2], nullptr);
  const double sx = std::strtod(argv[3], nullptr);
  if (argv[4][0] == '1') {
    _mm_setcsr(_mm_getcsr() | 0x8040);
  }
  const char* mode = argv[5];
  const int rounding = std::strcmp(mode, "up") == 0     ? FE_UPWARD
                       : std::strcmp(mode, "down") == 0 ? FE_DOWNWARD
                       : std::strcmp(mode, "zero") == 0 ? FE_TOWARDZERO
                                                        : FE_TONEAREST;
  std::fesetround(rounding);

  std::mt19937_64 random(20261121);
  std::uniform_real_distribution<double> uniform(-1.0, 1.0);
  const int trials = 200000;
  double largest = 0;
  int over = 0;
  bool nonfinite = false;
  for (int t = 0; t < trials; ++t) {
    double ratio = 0;
    if (r4) {
      constexpr auto vector = core::blades<r400, "e1 e2 e3 e4">;
      constexpr auto even = core::blades<r400, "1 e12 e13 e14 e23 e24 e34">;
      using Vector = core::mv<r400, vector, double>;
      const Vector a{sv * uniform(random), sv * uniform(random),
                     sv * uniform(random), sv * uniform(random)};
      const Vector b{uniform(random), uniform(random), uniform(random),
                     uniform(random)};
      const auto ab = core::geometric_product(a, b);
      const std::array<double, 7> v{
          core::get<"1">(ab),   core::get<"e12">(ab), core::get<"e13">(ab),
          core::get<"e14">(ab), core::get<"e23">(ab), core::get<"e24">(ab),
          core::get<"e34">(ab)};
      const std::array<double, 4> x{sx * uniform(random), sx * uniform(random),
                                    sx * uniform(random), sx * uniform(random)};
      ratio = worst<r400, even, vector>(v, x, nonfinite);
    } else {
      constexpr auto vector = core::blades<r300, "e1 e2 e3">;
      constexpr auto even = core::blades<r300, "1 e12 e13 e23">;
      const std::array<double, 4> v{sv * uniform(random), sv * uniform(random),
                                    sv * uniform(random), sv * uniform(random)};
      const std::array<double, 3> x{sx * uniform(random), sx * uniform(random),
                                    sx * uniform(random)};
      ratio = worst<r300, even, vector>(v, x, nonfinite);
    }
    largest = ratio > largest ? ratio : largest;
    over += ratio > 1 ? 1 : 0;
  }
  std::fesetround(FE_TONEAREST);
  std::printf("%s v*%g x*%g ftz %s rounding %s: largest |d|/tol %.3g, "
              "over 1 in %d of %d%s\n",
              argv[1], sv, sx, argv[4], mode, largest, over, trials,
              nonfinite ? ", a tolerance was not finite" : "");
  return 0;
}
