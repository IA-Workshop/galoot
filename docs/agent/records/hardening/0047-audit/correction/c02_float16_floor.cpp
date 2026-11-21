// What the padding of transform's rounding check does over std::float16_t,
// for the header's own non-versor in R(4,0,0): v = s(1 + e12 + 2 e13 + 3 e14)
// on x = s e1, which moves x onto e2, e3 and e4 as well.
//
// The padding is tau = lambda / u = 2^-14 / 2^-11 = 1/8. It is added to
// every rounded product of the magnitude P, and a tolerance is
// gamma_(K+1) P, so with v = x = 0 the tolerance is the least it can be: the
// floor. This prints tau, the count of roundings K of each dropped
// coefficient, that floor, and then, for several s, each dropped coefficient
// beside its tolerance and what transform does in a checking build.
// GCC only: Clang 23 with libstdc++ 16 has no std::float16_t.
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <stdfloat>

#include <galoot/core.hpp>

namespace core = galoot::core;
namespace detail = galoot::core::detail;

struct r400 {
  static constexpr auto value = core::R<4, 0, 0>();
};
inline constexpr auto versor_layout = core::blades<r400, "1 e12 e13 e14">;
inline constexpr auto object_layout = core::blades<r400, "e1">;
using T = std::float16_t;
using V = core::mv<r400, versor_layout, T>;
using X = core::mv<r400, object_layout, T>;

int main(int argc, char** argv) {
  std::setvbuf(stdout, nullptr, _IONBF, 0);
  if (argc == 2) {
    // One call of transform at the scale given, in a checking build.
    const auto s = static_cast<T>(std::atof(argv[1]));
    const V v{s, s, static_cast<T>(2 * s), static_cast<T>(3 * s)};
    const auto y = core::transform(X{s}, v);
    std::printf("s = %g: transform returned %g on e1, no abort\n",
                static_cast<double>(s), static_cast<double>(y.coefficients[0]));
    return 0;
  }
  const double tau = static_cast<double>(std::numeric_limits<T>::min() /
                                         (std::numeric_limits<T>::epsilon() / 2));
  std::printf("padding tau = %g\n", tau);
  std::printf("roundings K of the dropped coefficients:");
  for (const int k :
       detail::transform_dropped_roundings<r400, versor_layout, object_layout>) {
    std::printf(" %d", k);
  }
  std::printf("\n");
  const auto floor =
      detail::transform_dropped_tolerances<r400, versor_layout, object_layout>(
          V{}.coefficients, X{}.coefficients);
  std::printf("tolerances at v = x = 0 (the floor):");
  for (const T t : floor) {
    std::printf(" %.4g", static_cast<double>(t));
  }
  std::printf("\n");
  for (const double scale : {1.0, 0.1, 0.05, 0.03, 0.01}) {
    const auto s = static_cast<T>(scale);
    const V v{s, s, static_cast<T>(2 * s), static_cast<T>(3 * s)};
    const X x{s};
    const auto dropped =
        detail::transform_outer<r400, versor_layout, object_layout, false>(
            detail::transform_middle<r400, versor_layout, object_layout, true>(
                v.coefficients, x.coefficients),
            v.coefficients);
    const auto tolerances =
        detail::transform_dropped_tolerances<r400, versor_layout,
                                             object_layout>(v.coefficients,
                                                            x.coefficients);
    std::printf("s = %-5g dropped / tolerance:", scale);
    bool caught = false;
    for (std::size_t j = 0; j < dropped.size(); ++j) {
      const double d = static_cast<double>(dropped[j]);
      const double t = static_cast<double>(tolerances[j]);
      caught = caught || (d < 0 ? -d : d) > t;
      std::printf("  %.3g / %.3g", d, t);
    }
    std::printf("  -> %s\n", caught ? "caught" : "passed");
  }
  return 0;
}
