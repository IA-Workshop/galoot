// transform in checking mode over double with special values, one case per
// run: m06 <case>. Each case prints what transform returned, or the check
// aborts with its message. The rotor is the rotation by 90 degrees in the
// e1 e2 plane, (1 + e12) / sqrt(2) up to rounding, which is a versor.
//   nan-x:      x has NaN on e1.                  The comment: abort (cause 3).
//   inf-v:      v has infinity on its scalar.      The comment: abort (3 or 4).
//   huge-x:     x = 1.2e308 (e1 + e2), a finite result of the same size,
//               whose magnitude, the sum of absolute terms, overflows.
//                                                  The comment: abort (cause 4).
//   subnormal-x: x = 4.9e-324 e1, the least subnormal. The comment: no abort.
//   tiny-nonversor: the non-versor 1 + e12 + 2 e13 + 3 e14 of R(4, 0, 0)
//               scaled by 1e-160, on x = 1e-160 e1, whose dropped part is
//               near 1e-480 and so below the floor. The comment: not seen.
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>

#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct r300 {
  static constexpr auto value = core::R<3, 0, 0>();
};
struct r400 {
  static constexpr auto value = core::R<4, 0, 0>();
};
using Rotor = core::mv<r300, core::blades<r300, "1 e12 e13 e23">, double>;
using Vector = core::mv<r300, core::blades<r300, "e1 e2 e3">, double>;

void print(const Vector& y) {
  std::printf("returned %g e1 + %g e2 + %g e3, no abort\n", y.coefficients[0],
              y.coefficients[1], y.coefficients[2]);
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 2) {
    return 2;
  }
  const char* c = argv[1];
  const double h = 1 / std::sqrt(2.0);
  const Rotor rotor{h, h, 0, 0};
  const double nan = std::numeric_limits<double>::quiet_NaN();
  const double inf = std::numeric_limits<double>::infinity();
  if (std::strcmp(c, "nan-x") == 0) {
    print(core::transform(Vector{nan, 0, 0}, rotor));
  } else if (std::strcmp(c, "inf-v") == 0) {
    print(core::transform(Vector{1, 0, 0}, Rotor{inf, h, 0, 0}));
  } else if (std::strcmp(c, "huge-x") == 0) {
    print(core::transform(Vector{1.2e308, 1.2e308, 0}, rotor));
  } else if (std::strcmp(c, "subnormal-x") == 0) {
    print(core::transform(Vector{std::numeric_limits<double>::denorm_min(), 0,
                                 0},
                          rotor));
  } else if (std::strcmp(c, "tiny-nonversor") == 0) {
    using V = core::mv<r400, core::blades<r400, "1 e12 e13 e14">, double>;
    using X = core::mv<r400, core::blades<r400, "e1">, double>;
    const double s = 1e-160;
    const auto y = core::transform(X{s}, V{s, s, 2 * s, 3 * s});
    std::printf("returned %g e1, no abort\n", y.coefficients[0]);
  }
  return 0;
}
