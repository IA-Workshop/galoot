// The run-time cost of transform's checking mode over double: transforms a
// rigid(3) point and a conformal(3) vector by a motor, `count` times each,
// with inputs read from the command line so that nothing is folded at
// compile time, and prints a sum of the results so that nothing is removed.
// Built once with GALOOT_CHECKING 1 and once with 0; the two runs' retired
// instructions give the cost of the check.
//
//   cost <count> <seed value>
#include <cstdio>
#include <cstdlib>

#include <galoot/core.hpp>

namespace {

namespace core = galoot::core;

struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

} // namespace

int main(int argc, char** argv) {
  if (argc != 3) {
    std::fprintf(stderr, "usage: cost <count> <seed value>\n");
    return 2;
  }
  const long count = std::atol(argv[1]);
  const double s = std::atof(argv[2]);

  using Point = core::mv<rga3, core::blades<rga3, "e1 e2 e3 e4">, double>;
  using Motor =
      core::mv<rga3, core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">,
               double>;
  using Vector = core::mv<cga3, core::of_grade<5, 1>, double>;
  using Even = core::mv<cga3, core::of_grade<5, 4>, double>;

  // A unitized rotor about the z axis (cos 0.6, sin 0.8) and translator.
  const Motor q{0, 0, 0.6 * s, 0.8 * s, 0.5, -1.0, 0.25, 0};
  Point p{1.0, 2.0 * s, 3.0, 1.0};
  // A conformal(3) flector: the plane through the origin with normal e1,
  // the rigid plane e423 wedged with e5, on e2345.
  const Even g{0, 0, 0, 0, s};
  Vector x{1.0, 2.0, s, 1.0, 7.0};

  double sum = 0;
  for (long i = 0; i < count; ++i) {
    p = core::transform(p, q);
    x = core::transform(x, g);
    sum += core::get<"e1">(p) + core::get<"e1">(x);
  }
  std::printf("%.17g\n", sum);
  return 0;
}
