// What kind of motor rga3::rotation returns about an l that is not a line:
// direction v = (0, 0, 1) and moment m = (0, 0, 1), so v . m = 1, with
// theta = 1. Motor coefficients are named as in Lengyel's Motor page and the
// preset's motor_layout: Qv = (Qvx, Qvy, Qvz, Qvw) on e41, e42, e43, e1234
// and Qm = (Qmx, Qmy, Qmz, Qmw) on e23, e31, e12, 1. The weight norm is |Qv|.
// A motor is a versor exactly when Q ⟇ antireverse(Q) is a multiple of the
// antiscalar 𝟙, and that product's scalar part is 2 Qv . Qm.
// Printed: the motor, |Qv|^2, Qv . Qm, every nonzero coefficient of
// Q ⟇ antireverse(Q), and the weight of each of two points once moved.
#include <cmath>
#include <cstdio>

#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<double>;
using Line = rga3::Line<double>;
using Motor = rga3::Motor<double>;

void show(const char* what, const Line& l) {
  const Motor q = rga3::rotation(l, 1.0);
  const auto& c = q.coefficients;
  std::printf("%s\n  motor:", what);
  for (double x : c) {
    std::printf(" %.6f", x);
  }
  const double weight = (c[0] * c[0]) + (c[1] * c[1]) + (c[2] * c[2]) +
                        (c[3] * c[3]);
  const double defect = (c[0] * c[4]) + (c[1] * c[5]) + (c[2] * c[6]) +
                        (c[3] * c[7]);
  std::printf("\n  |Qv|^2 = %.17g, Qv . Qm = %.6f\n", weight, defect);
  const auto qq = core::geometric_antiproduct(q, core::antireverse(q));
  std::printf("  Q ⟇ antireverse(Q), nonzero coefficients:");
  for (std::size_t i = 0; i < qq.component_count; ++i) {
    if (qq.coefficients[i] != 0) {
      const auto name = core::canonical_name(qq.layout.blades[i].mask);
      std::printf(" %.*s %.6f", static_cast<int>(name.length),
                  name.chars.data(), qq.coefficients[i]);
    }
  }
  const Point p2 = core::transform(Point{1, 0, 0, 1}, q);
  const Point r2 = core::transform(Point{0, 2, 0, 1}, q);
  std::printf("\n  weights of two moved points: %.17g and %.17g\n",
              p2.coefficients[3], r2.coefficients[3]);
}

int main() {
  std::printf("GALOOT_CHECKING = %d\n", GALOOT_CHECKING);
  show("about the z axis (a line)", Line{0, 0, 1, 0, 0, 0});
  show("about direction z, moment z (v . m = 1)", Line{0, 0, 1, 0, 0, 1});
  return 0;
}
