#include <galoot/presets/rga3.hpp>
#include <cstdio>
namespace r = galoot::presets::rga3;
volatile double z = 0.0;
int main() {
  r::Line<double> l{z, z, z, 0, 0, 0};
  auto m = r::rotation(l, 1.0);
  std::printf("%f\n", m.coefficients[0]);
}
