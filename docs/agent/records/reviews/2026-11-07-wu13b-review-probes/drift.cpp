// Composes N random flat-plane motors of conformal(3) (and rigid(3) as a
// control) in double, then moves a random vector; prints the fraction of
// trials where the rounding check would fire and the largest |d|/tol, by N.
// Also dumps firing cases of the 2-plane family (V, x in hex) for an exact
// recomputation.
#include <cstdio>
#include <random>
#include <galoot/core.hpp>
namespace core = galoot::core;
struct cga3 { static constexpr auto value = core::conformal<3>(); };
struct rga3 { static constexpr auto value = core::rigid<3>(); };
double draw(std::mt19937_64& s) { return (static_cast<double>(s() >> 11) * 0x1p-53 * 2.0) - 1.0; }
template <class M> M rnd(std::mt19937_64& s) { M m; for (double& c : m.coefficients) c = draw(s); return m; }
template <class Alg, auto VL, auto XL>
double ratio(const core::mv<Alg, VL, double>& v, const core::mv<Alg, XL, double>& x) {
  auto d = core::detail::transform_outer<Alg, VL, XL, false>(
      core::detail::transform_middle<Alg, VL, XL, true>(v.coefficients, x.coefficients), v.coefficients);
  auto t = core::detail::transform_dropped_tolerances<Alg, VL, XL>(v.coefficients, x.coefficients);
  double r = 0; for (std::size_t j = 0; j < d.size(); ++j) { double a = d[j] < 0 ? -d[j] : d[j]; r = a / t[j] > r ? a / t[j] : r; }
  return r;
}
template <class M> M unitize_scalar(M m) { // rescale by a power of two to stay of ordinary size
  double big = 0; for (double c : m.coefficients) big = std::max(big, std::abs(c));
  int e; std::frexp(big, &e); for (double& c : m.coefficients) c = std::ldexp(c, -e); return m;
}
template <class Alg, auto P, auto X>
void run(const char* name, std::mt19937_64& s, int trials) {
  using Plane = core::mv<Alg, P, double>;
  using Obj = core::mv<Alg, X, double>;
  using Motor = decltype(core::compose(Plane{}, Plane{}));
  using Chain = decltype(core::compose(Motor{}, Motor{}));
  static_assert(std::is_same_v<Chain, decltype(core::compose(Motor{}, Chain{}))>);
  for (int n : {2, 4, 8, 16, 32, 64, 128}) {
    int fired = 0; double worst = 0;
    for (int t = 0; t < trials; ++t) {
      const auto m = [&] { return unitize_scalar(core::compose(rnd<Plane>(s), rnd<Plane>(s))); };
      Chain q = unitize_scalar(core::compose(m(), m()));
      for (int i = 2; i < n; ++i) q = unitize_scalar(core::compose(m(), q));
      const Obj x = rnd<Obj>(s);
      double r = ratio(q, x); worst = std::max(worst, r); fired += r > 1 ? 1 : 0;
    }
    std::printf("%s\tN=%d\tfired %d/%d\tlargest %.3g\n", name, n, fired, trials, worst);
  }
}
int main(int argc, char** argv) {
  std::mt19937_64 s(argc > 1 ? std::strtoull(argv[1], nullptr, 10) : 1);
  constexpr auto flat = core::blades<cga3, "e1235 e1245 e1345 e2345">;
  constexpr auto hyper = core::of_grade<5, 4>;
  run<cga3, flat, core::of_grade<5, 1>>("cga3 flat-plane motors, vector", s, 1000);
  run<cga3, hyper, core::of_grade<5, 1>>("cga3 hyperplane motors, vector", s, 1000);
  run<rga3, core::blades<rga3, "e423 e431 e412 e321">, core::blades<rga3, "e1 e2 e3 e4">>("rga3 motors, point", s, 1000);
}
