// The run cost of transform in each checking mode, on fixed inputs that the
// compiler must reload every iteration. DENSE 1: every coefficient nonzero;
// DENSE 0: a motor, a point, a plane, and a vector with zero coefficients.
#include <cstdio>
#include <cstdlib>
#include <galoot/core.hpp>
namespace {
namespace core = galoot::core;
struct cga3 { static constexpr auto value = core::conformal<3>(); };
struct rga3 { static constexpr auto value = core::rigid<3>(); };
}
int main(int argc, char** argv) {
  if (argc != 3) return 2;
  const long count = std::atol(argv[1]);
  const double s = std::atof(argv[2]);
  using Point = core::mv<rga3, core::blades<rga3, "e1 e2 e3 e4">, double>;
  using Motor = core::mv<rga3, core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">, double>;
  using Vector = core::mv<cga3, core::of_grade<5, 1>, double>;
  using Even = core::mv<cga3, core::of_grade<5, 4>, double>;
#if DENSE
  Motor q{0.1 * s, 0.2, 0.3, 0.48 * s, 0.6, 0.36, 0.48, 0.52};
  Point p{1.0, 2.0 * s, 3.0, 1.0};
  Even g{0.3, 0.4 * s, 0.5, 0.2, 0.7};
  Vector x{1.0, 2.0, s, 1.0, 7.0};
#else
  Motor q{0, 0, 0.6 * s, 0.8 * s, 0.5, -1.0, 0.25, 0};
  Point p{1.0, 2.0 * s, 3.0, 1.0};
  Even g{0, 0, 0, 0, s};
  Vector x{1.0, 2.0, s, 1.0, 7.0};
#endif
  double sum = 0;
  for (long i = 0; i < count; ++i) {
    asm volatile("" : "+m"(q), "+m"(p), "+m"(g), "+m"(x));
    const Point np = core::transform(p, q);
    const Vector nx = core::transform(x, g);
    sum += core::get<"e1">(np) + core::get<"e1">(nx);
  }
  std::printf("%.17g\n", sum);
  return 0;
}
