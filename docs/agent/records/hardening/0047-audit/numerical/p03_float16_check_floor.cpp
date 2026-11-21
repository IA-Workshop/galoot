// transform's rounding check pads every rounded product by tau = lambda / u,
// the smallest normal number over the unit roundoff, so that its bound covers
// underflow. The header gives the floor this puts under the bound for double
// (near 1e-306) and float (near 1e-36, a gross non-versor unseen once v and x
// are below about 2^-41). std::float16_t is also an IEEE 754 binary type that
// rounds to nearest, so it is checked, and for it tau = 2^-14 / 2^-11 = 1/8.
//
// The non-versor is the header's own example: in R(4,0,0),
// v = 1 + e12 + 2 e13 + 3 e14 moves x on e1 alone onto e2, e3, and e4 as well.
// For each scale s of x, this prints the largest ratio of a dropped
// coefficient to its tolerance, the comparison the check makes: a ratio
// above 1 is caught, below 1 is passed. With an argument, it calls transform
// itself in checking mode at one scale, where float aborts and float16
// returns.
#include <cstdio>
#include <stdfloat>

#include <galoot/core.hpp>

namespace core = galoot::core;
namespace detail = galoot::core::detail;

struct r400 {
  static constexpr auto value = core::R<4, 0, 0>();
};
inline constexpr auto versor_layout = core::blades<r400, "1 e12 e13 e14">;
inline constexpr auto object_layout = core::blades<r400, "e1">;

template <class T>
double worst_ratio(double scale) {
  using V = core::mv<r400, versor_layout, T>;
  using X = core::mv<r400, object_layout, T>;
  const V v{T(1), T(1), T(2), T(3)};
  const X x{static_cast<T>(scale)};
  const auto middle =
      detail::transform_middle<r400, versor_layout, object_layout, true>(
          v.coefficients, x.coefficients);
  const auto dropped =
      detail::transform_outer<r400, versor_layout, object_layout, false>(
          middle, v.coefficients);
  const auto tolerances =
      detail::transform_dropped_tolerances<r400, versor_layout, object_layout>(
          v.coefficients, x.coefficients);
  double worst = 0;
  for (std::size_t j = 0; j < dropped.size(); ++j) {
    const double d = dropped[j] < T{} ? -static_cast<double>(dropped[j])
                                      : static_cast<double>(dropped[j]);
    const double ratio = d / static_cast<double>(tolerances[j]);
    worst = ratio > worst ? ratio : worst;
  }
  return worst;
}

int main(int argc, char** argv) {
  std::setvbuf(stdout, nullptr, _IONBF, 0);
  if (argc == 1) {
    std::printf("scale of x   float16 |d|/tol   float |d|/tol\n");
    for (const double scale : {1.0, 1e-1, 1e-2, 1e-3, 3e-4, 2e-4, 1e-4}) {
      std::printf("%-12g %-17.4g %.4g\n", scale,
                  worst_ratio<std::float16_t>(scale), worst_ratio<float>(scale));
    }
    return 0;
  }
  // Checking mode, x at 1e-4, a normal float16 (the least is about 6.1e-5):
  // float16 passes the non-versor, float aborts.
  const bool half = argv[1][0] == 'h';
  if (half) {
    using V = core::mv<r400, versor_layout, std::float16_t>;
    using X = core::mv<r400, object_layout, std::float16_t>;
    const V v{1.0f16, 1.0f16, 2.0f16, 3.0f16};
    const auto y = core::transform(X{0.0001f16}, v);
    std::printf("float16 transform returned %g on e1: not caught\n",
                static_cast<double>(y.coefficients[0]));
    // What it dropped, computed again in double from the same inputs.
    using VD = core::mv<r400, versor_layout, double>;
    using XD = core::mv<r400, object_layout, double>;
    const VD vd{1.0, 1.0, 2.0, 3.0};
    const XD xd{static_cast<double>(0.0001f16)};
    const auto dropped =
        detail::transform_outer<r400, versor_layout, object_layout, false>(
            detail::transform_middle<r400, versor_layout, object_layout, true>(
                vd.coefficients, xd.coefficients),
            vd.coefficients);
    for (const double d : dropped) {
      std::printf("dropped, in double: %g\n", d);
    }
  } else {
    using V = core::mv<r400, versor_layout, float>;
    using X = core::mv<r400, object_layout, float>;
    const V v{1.0f, 1.0f, 2.0f, 3.0f};
    const auto y = core::transform(X{0.0001f}, v);
    std::printf("float transform returned %g on e1\n",
                static_cast<double>(y.coefficients[0]));
  }
  return 0;
}
