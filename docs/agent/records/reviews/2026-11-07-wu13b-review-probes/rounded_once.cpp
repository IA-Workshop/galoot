// Versors computed exactly over the test rational from hyperplanes with small
// non-dyadic rational coefficients, then rounded once per component to
// double (correctly rounded versors), against the same versors computed by
// compose in double. Counts firings of the rounding check and the largest
// componentwise relative error of V.
#include <cstdio>
#include <random>
#include <galoot/core.hpp>
#include <support/rational.hpp>
namespace core = galoot::core;
using test_support::rational;
// n/q to double: both exact in long double (64-bit significand), one long
// double division and one conversion, so within (1 + 2^-11) u of n/q.
double near(const rational& r) { return static_cast<double>(static_cast<long double>(r.numerator) / static_cast<long double>(r.denominator)); }
struct cga3 { static constexpr auto value = core::conformal<3>(); };
template <class Alg, auto VL, auto XL>
double ratio(const std::array<double, VL.size()>& v, const std::array<double, XL.size()>& x) {
  auto d = core::detail::transform_outer<Alg, VL, XL, false>(core::detail::transform_middle<Alg, VL, XL, true>(v, x), v);
  auto t = core::detail::transform_dropped_tolerances<Alg, VL, XL>(v, x);
  double r = 0; for (std::size_t j = 0; j < d.size(); ++j) r = std::max(r, std::abs(d[j]) / t[j]); return r;
}
template <auto HL, int Count>
void family(const char* name, std::mt19937_64& s, int cases) {
  using HR = core::mv<cga3, HL, rational>; using HD = core::mv<cga3, HL, double>;
  using X = core::mv<cga3, core::of_grade<5, 1>, double>;
  std::uniform_int_distribution<int> num(-60, 60), den(1, 61);
  int fired_once = 0, fired_composed = 0; double worst_once = 0, worst_composed = 0, worst_eps = 0;
  for (int c = 0; c < cases; ++c) {
    std::array<HR, 3> hr; std::array<HD, 3> hd;
    for (int k = 0; k < 3; ++k) for (std::size_t i = 0; i < HL.size(); ++i) {
      int n = num(s), q = den(s); if (n == 0) n = 1;
      hr[k].coefficients[i] = rational{n, q};
      hd[k].coefficients[i] = near(rational{n, q});
    }
    auto vr = [&] { if constexpr (Count == 2) return core::compose(hr[1], hr[0]); else return core::compose(hr[2], core::compose(hr[1], hr[0])); }();
    auto vc = [&] { if constexpr (Count == 2) return core::compose(hd[1], hd[0]); else return core::compose(hd[2], core::compose(hd[1], hd[0])); }();
    using VR = decltype(vr); constexpr auto VL = VR::layout;
    std::array<double, VL.size()> once{}, composed{};
    for (std::size_t i = 0; i < VL.size(); ++i) {
      once[i] = near(vr.coefficients[i]); composed[i] = vc.coefficients[i];
      if (once[i] != 0) worst_eps = std::max(worst_eps, std::abs(composed[i] - once[i]) / std::abs(once[i]) / 0x1p-53);
    }
    X x; for (auto& e : x.coefficients) e = near(rational{num(s), den(s)});
    double r1 = ratio<cga3, VL, X::layout>(once, x.coefficients), r2 = ratio<cga3, VL, X::layout>(composed, x.coefficients);
    worst_once = std::max(worst_once, r1); worst_composed = std::max(worst_composed, r2);
    fired_once += r1 > 1; fired_composed += r2 > 1;
  }
  std::printf("%s: rounded once fired %d/%d (largest %.3g); composed in double fired %d (largest %.3g); largest component error of composed V %.3g u\n",
              name, fired_once, cases, worst_once, fired_composed, worst_composed, worst_eps);
}
int main() {
  std::mt19937_64 s(20261107);
  family<core::blades<cga3, "e1235 e1245 e1345 e2345">, 2>("cga3 vector, 2 flat planes", s, 20000);
  family<core::blades<cga3, "e1235 e1245 e1345 e2345">, 3>("cga3 vector, 3 flat planes", s, 20000);
  family<core::of_grade<5, 4>, 2>("cga3 vector, 2 hyperplanes", s, 20000);
}
