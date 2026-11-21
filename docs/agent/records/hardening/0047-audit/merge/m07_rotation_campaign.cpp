// rga3::rotation over double inside its documented range: random directions
// whose length is log-uniform from 1e-100 to 1e100, random moments, and
// random angles from -100 to 100. For each motor it measures the defect
// |w - 1| of the squared weight norm w, summed in binary128 from the double
// coefficients, in units of u = 2^-53, and prints the largest defect seen.
// This is a measurement on random inputs, not a bound.
#include <cmath>
#include <cstdio>
#include <random>

#include <galoot/presets/rga3.hpp>

namespace rga3 = galoot::presets::rga3;

int main() {
  std::mt19937_64 random(20261121);
  std::uniform_real_distribution<double> unit(-1.0, 1.0);
  std::uniform_real_distribution<double> exponent(-100.0, 100.0);
  std::uniform_real_distribution<double> angle(-100.0, 100.0);
  const int trials = 1000000;
  double largest = 0;
  double at_length = 0;
  double at_angle = 0;
  for (int t = 0; t < trials; ++t) {
    double d[3];
    double norm2 = 0;
    do {
      norm2 = 0;
      for (double& c : d) {
        c = unit(random);
        norm2 += c * c;
      }
    } while (norm2 < 1e-4 || norm2 > 1);
    const double length = std::pow(10.0, exponent(random));
    const double scale = length / std::sqrt(norm2);
    const rga3::Line<double> axis{d[0] * scale, d[1] * scale, d[2] * scale,
                                  unit(random), unit(random), unit(random)};
    const double theta = angle(random);
    const auto q = rotation(axis, theta);
    __float128 w = 0;
    for (const int i : {0, 1, 2, 3}) {
      const auto x = static_cast<__float128>(q.coefficients[i]);
      w += x * x;
    }
    const __float128 e = w > 1 ? w - 1 : 1 - w;
    const double defect =
        static_cast<double>(e * static_cast<__float128>(9007199254740992.0));
    if (defect > largest) {
      largest = defect;
      at_length = length;
      at_angle = theta;
    }
  }
  std::printf("%d motors: largest |w - 1| / u %.3g (direction length %.3g, "
              "angle %.3g)\n",
              trials, largest, at_length, at_angle);
  return 0;
}
