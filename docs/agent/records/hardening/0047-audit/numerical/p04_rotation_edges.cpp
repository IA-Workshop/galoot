// rga3::rotation unitizes its axis: the result's weight part (the
// coefficients on e41, e42, e43, e1234) has squared norm 1 up to rounding,
// "for a direction of any positive length", while the Requires clause asks
// for a length between about 1e-150 and 1e150 in double. This prints, for a
// direction along (1, 1, 1) of length L, the defect |w - 1| of the squared
// weight norm w, in units of u = 2^-53, with w summed in binary128 from the
// double coefficients; and what a NaN or infinite angle gives, which nothing
// checks. Built with GALOOT_CHECKING 1, so the length check is active.
#include <cmath>
#include <cstdio>
#include <limits>

#include <galoot/presets/rga3.hpp>

namespace rga3 = galoot::presets::rga3;

namespace {

double defect_in_u(double length, double theta) {
  const double c = length / std::sqrt(3.0);
  const rga3::Line<double> axis{c, c, c, 0, 0, 0};
  const auto q = rotation(axis, theta);
  __float128 w = 0;
  for (const int i : {0, 1, 2, 3}) {
    const auto x = static_cast<__float128>(q.coefficients[i]);
    w += x * x;
  }
  const __float128 d = w > 1 ? w - 1 : 1 - w;
  return static_cast<double>(d * static_cast<__float128>(9007199254740992.0));  // d / 2^-53
}

}  // namespace

int main() {
  std::setvbuf(stdout, nullptr, _IONBF, 0);
  const double theta = 1.0;
  std::printf("length L       |w - 1| / u\n");
  for (const double length : {1.0, 1e-150, 1e-154, 1e-156, 1e-158, 1e-160,
                              1e-161, 1e150, 1e153}) {
    std::printf("%-14g %.3g\n", length, defect_in_u(length, theta));
  }
  const rga3::Line<double> z_axis{0, 0, 1, 0, 0, 0};
  const double nan = std::numeric_limits<double>::quiet_NaN();
  const double inf = std::numeric_limits<double>::infinity();
  for (const double bad : {nan, inf}) {
    const auto q = rotation(z_axis, bad);
    std::printf("theta = %g: e43 %g, e1234 %g (no check fired)\n", bad,
                q.coefficients[2], q.coefficients[3]);
  }
  return 0;
}
