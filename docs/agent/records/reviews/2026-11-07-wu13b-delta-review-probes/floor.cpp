// The smallest input scale s at which the check still sees a gross non-versor:
// R(4,0,0), V = s (1 + e1234), x = s on the scalar, dropped 2 s^3 against its
// tolerance; and a conformal(3) vector moved by a flat-plane motor perturbed
// on one component by 1% (relative), all inputs scaled by s.
#include <cmath>
#include <cstdio>
#include <random>
#include <galoot/core.hpp>
namespace core = galoot::core;
struct r400 { static constexpr auto value = core::R<4, 0, 0>(); };
struct cga3 { static constexpr auto value = core::conformal<3>(); };
template <class T, class Alg, auto VL, auto XL>
bool fires(const std::array<T, VL.size()>& v, const std::array<T, XL.size()>& x) {
  auto d = core::detail::transform_outer<Alg, VL, XL, false>(core::detail::transform_middle<Alg, VL, XL, true>(v, x), v);
  auto t = core::detail::transform_dropped_tolerances<Alg, VL, XL>(v, x);
  for (std::size_t j = 0; j < d.size(); ++j) if (!(std::abs(d[j]) <= t[j] && t[j] <= std::numeric_limits<T>::max())) return true;
  return false;
}
template <class T> void scan(const char* name) {
  constexpr auto P = core::blades<r400, "1 e1234">; constexpr auto X = core::blades<r400, "1">;
  int last = 0;
  for (int e = 0; e < std::numeric_limits<T>::max_exponent + 60; ++e) {
    const T s = std::ldexp(T{1}, -e);
    if (fires<T, r400, P, X>({s, s}, {s})) last = e; else break;
  }
  std::printf("%s r400 non-versor (dropped = 2 s^3): detected down to s = 2^-%d\n", name, last);
  constexpr auto FL = core::blades<cga3, "e1235 e1245 e1345 e2345">;
  using Pl = core::mv<cga3, FL, T>;
  using Mo = decltype(core::compose(Pl{}, Pl{}));
  constexpr auto ML = Mo::layout; constexpr auto VX = core::of_grade<5, 1>;
  std::mt19937_64 g(5); std::uniform_real_distribution<double> u(-1, 1);
  Pl a, b; for (auto& c : a.coefficients) c = T(u(g)); for (auto& c : b.coefficients) c = T(u(g));
  Mo m = core::compose(a, b); m.coefficients[0] = T(m.coefficients[0] * 1.01);
  std::array<T, 5> x{}; for (auto& c : x) c = T(u(g));
  int lastc = -1;
  for (int e = 0; e < std::numeric_limits<T>::max_exponent + 60; ++e) {
    std::array<T, ML.size()> v{}; std::array<T, 5> xs{};
    for (std::size_t i = 0; i < v.size(); ++i) v[i] = std::ldexp(m.coefficients[i], -e);
    for (std::size_t i = 0; i < 5; ++i) xs[i] = std::ldexp(x[i], -e);
    if (fires<T, cga3, ML, VX>(v, xs)) lastc = e; else break;
  }
  std::printf("%s cga3 motor perturbed 1%%: detected at scale 2^0 %s; down to 2^-%d\n", name, lastc >= 0 ? "yes" : "NO", lastc);
}
int main() { scan<float>("float "); scan<double>("double"); }
