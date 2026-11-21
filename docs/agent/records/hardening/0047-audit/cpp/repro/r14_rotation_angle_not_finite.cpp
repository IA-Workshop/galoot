// rotation with an angle that is not finite, in a checking build: the axis is
// checked, the angle is not, and the motor holds NaN.
#include <cmath>
#include <cstdio>
#include <limits>
#include <galoot/presets/rga3.hpp>
namespace rga3 = galoot::presets::rga3;
int main() {
  const rga3::Line<double> z_axis{0, 0, 1, 0, 0, 0};
  const auto m = rotation(z_axis, std::numeric_limits<double>::infinity());
  std::printf("GALOOT_CHECKING=%d, rotation(z, inf): e43 %g, e1234 %g\n",
              GALOOT_CHECKING, m.coefficients[2], m.coefficients[3]);
  return 0;
}
