// transform over std::float16_t, which rounding_checked_scalar accepts
// (is_iec559, round to nearest), with a v that is not a versor: v = a(1 +
// e1234) in R(4, 0, 0) applied to the scalar a, which moves part of x onto
// e1234. In a checking build the check aborts for a = 0.1 and passes for a =
// 0.05: the padding of the bound, the smallest normal number over u, is 2^-3
// in float16, so the floor under the tolerance is near the size of the
// inputs. The header states the floor for double and float only. GCC only:
// Clang 23 with libstdc++ 16 has no std::float16_t here.
#include <cstdio>
#include <limits>
#include <stdfloat>
#include <galoot/core.hpp>
#ifndef SCALE
#define SCALE 0.05f
#endif
namespace core = galoot::core;
struct r4 { static constexpr auto value = core::R<4, 0, 0>(); };
using T = std::float16_t;
using S = core::mv<r4, core::blades<r4, "1">, T>;
using V = core::mv<r4, core::blades<r4, "1 e1234">, T>;
int main(int argc, char**) {
  std::printf("checked: %d, padding: %g\n",
              static_cast<int>(core::detail::transform_rounding_checked<
                               r4, V::layout, S::layout, T>),
              static_cast<double>(std::numeric_limits<T>::min() /
                                  (std::numeric_limits<T>::epsilon() / 2)));
  const T a = static_cast<T>(argc) * static_cast<T>(SCALE);
  const auto y = core::transform(S{a}, V{a, a});
  std::printf("a = %g: the check passed, result %g\n", static_cast<double>(a),
              static_cast<double>(y.coefficients[0]));
  return 0;
}
