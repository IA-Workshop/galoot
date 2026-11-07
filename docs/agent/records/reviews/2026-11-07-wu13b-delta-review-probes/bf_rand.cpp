#include <cstdio>
#include <random>
#include <stdfloat>
#include <galoot/core.hpp>
namespace core = galoot::core;
struct rga3 { static constexpr auto value = core::rigid<3>(); };
using B = std::bfloat16_t;
using Point = core::mv<rga3, core::blades<rga3, "e1 e2 e3 e4">, B>;
using Motor = core::mv<rga3, core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">, B>;
int main() {
  std::mt19937_64 s(3); std::uniform_real_distribution<float> u(-1, 1); float sum = 0;
  for (int t = 0; t < 1000; ++t) {
    Point p; Motor q; for (auto& c : p.coefficients) c = B(u(s)); for (auto& c : q.coefficients) c = B(u(s));
    sum += float(core::transform(p, q).coefficients[0]);
  }
  std::printf("ran 1000 random bfloat16 transforms, sum %g\n", sum);
}
