// How often the rounding check of transform fires for versors built in
// double with rounding of their own, and how close it comes elsewhere.
//
// Each family draws seeded hyperplanes with coefficients uniform in [-1, 1)
// (53-bit, from std::mt19937_64), forms the versor as their geometric
// antiproduct in double (compose, which rounds), and transforms a seeded x.
// For every dropped coefficient d it computes the tolerance the check uses
// and records the largest ratio |d| / tolerance and the number of cases in
// which some ratio exceeds 1, so that the check would abort.
//
//   probe <cases> <seed>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>

#include <galoot/core.hpp>

namespace {

namespace core = galoot::core;

struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

double draw(std::mt19937_64& source) {
  return (static_cast<double>(source() >> 11) * 0x1p-53 * 2.0) - 1.0;
}

template <class M> M random_mv(std::mt19937_64& source) {
  M m;
  for (double& c : m.coefficients) {
    c = draw(source);
  }
  return m;
}

struct tally {
  long cases = 0;
  long fired = 0;
  long dropped = 0;
  double largest = 0.0;
};

template <class Alg, auto VL, auto XL>
void record(const core::mv<Alg, VL, double>& v,
            const core::mv<Alg, XL, double>& x, tally& t) {
  const auto d = core::detail::transform_outer<Alg, VL, XL, false>(
      core::detail::transform_middle<Alg, VL, XL, true>(v.coefficients,
                                                        x.coefficients),
      v.coefficients);
  const auto tol = core::detail::transform_dropped_tolerances<Alg, VL, XL>(
      v.coefficients, x.coefficients);
  bool fired = false;
  for (std::size_t j = 0; j < d.size(); ++j) {
    const double size = d[j] < 0 ? -d[j] : d[j];
    const double ratio = size / tol[j];
    t.largest = ratio > t.largest ? ratio : t.largest;
    fired = fired || ratio > 1.0;
    ++t.dropped;
  }
  ++t.cases;
  t.fired += fired ? 1 : 0;
}

// The versor of `count` hyperplanes on the layout H, composed in double, and
// its transform of a random x on X.
template <class Alg, auto H, auto X, int Count>
tally family(std::mt19937_64& source, long cases) {
  using Hyperplane = core::mv<Alg, H, double>;
  using Object = core::mv<Alg, X, double>;
  tally t;
  for (long i = 0; i < cases; ++i) {
    const auto p1 = random_mv<Hyperplane>(source);
    const auto p2 = random_mv<Hyperplane>(source);
    const auto x = random_mv<Object>(source);
    if constexpr (Count == 1) {
      record(p1, x, t);
    } else if constexpr (Count == 2) {
      record(core::compose(p2, p1), x, t);
    } else {
      const auto p3 = random_mv<Hyperplane>(source);
      record(core::compose(p3, core::compose(p2, p1)), x, t);
    }
  }
  return t;
}

void print(const char* name, const tally& t) {
  std::printf("%s\t%ld\t%ld\t%ld\t%.6g\n", name, t.cases, t.dropped, t.fired,
              t.largest);
}

} // namespace

int main(int argc, char** argv) {
  if (argc != 3) {
    std::fprintf(stderr, "usage: probe <cases> <seed>\n");
    return 2;
  }
  const long cases = std::atol(argv[1]);
  std::mt19937_64 source(std::strtoull(argv[2], nullptr, 10));

  constexpr auto flat_planes = core::blades<cga3, "e1235 e1245 e1345 e2345">;
  constexpr auto hyperplanes = core::of_grade<5, 4>;
  constexpr auto vectors = core::of_grade<5, 1>;
  constexpr auto bivectors = core::of_grade<5, 2>;
  constexpr auto rigid_planes = core::blades<rga3, "e423 e431 e412 e321">;
  constexpr auto rigid_points = core::blades<rga3, "e1 e2 e3 e4">;

  std::printf("family\tcases\tdropped\tfired\tlargest_ratio\n");
  print("cga3 vector, 1 plane",
        family<cga3, flat_planes, vectors, 1>(source, cases));
  print("cga3 vector, 2 planes",
        family<cga3, flat_planes, vectors, 2>(source, cases));
  print("cga3 vector, 3 planes",
        family<cga3, flat_planes, vectors, 3>(source, cases));
  print("cga3 vector, 2 hyperplanes",
        family<cga3, hyperplanes, vectors, 2>(source, cases));
  print("cga3 vector, 3 hyperplanes",
        family<cga3, hyperplanes, vectors, 3>(source, cases));
  print("cga3 bivector, 2 planes",
        family<cga3, flat_planes, bivectors, 2>(source, cases));
  print("rga3 point, 2 planes",
        family<rga3, rigid_planes, rigid_points, 2>(source, cases));
  print("rga3 point, 3 planes",
        family<rga3, rigid_planes, rigid_points, 3>(source, cases));
  return 0;
}
