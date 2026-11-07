#include <cstdio>
#include <random>
#include <galoot/core.hpp>
namespace core = galoot::core;
struct cga3 { static constexpr auto value = core::conformal<3>(); };
double draw(std::mt19937_64& s) { return (static_cast<double>(s() >> 11) * 0x1p-53 * 2.0) - 1.0; }
template <class M> M rnd(std::mt19937_64& s) { M m; for (double& c : m.coefficients) c = draw(s); return m; }
template <class M> M unit(M m) { double big = 0; for (double c : m.coefficients) big = std::max(big, std::abs(c));
  int e; std::frexp(big, &e); for (double& c : m.coefficients) c = std::ldexp(c, -e); return m; }
int main() {
  constexpr auto flat = core::blades<cga3, "e1235 e1245 e1345 e2345">;
  using Plane = core::mv<cga3, flat, double>;
  using Motor = decltype(core::compose(Plane{}, Plane{}));
  using Chain = decltype(core::compose(Motor{}, Motor{}));
  using Obj = core::mv<cga3, core::of_grade<5, 1>, double>;
  constexpr auto VL = Chain::layout; constexpr auto XL = Obj::layout;
  constexpr const auto& k1 = core::detail::first_kernel<cga3, VL, XL, true>;
  constexpr const auto& k2 = core::detail::second_kernel<cga3, VL, XL, false>;
  constexpr auto& K = core::detail::transform_dropped_roundings<cga3, VL, XL>;
  std::printf("{\"nv\":%zu,\"nx\":%zu,\"nm\":%zu,\"nd\":%zu,\"first\":[", VL.size(), XL.size(),
    core::detail::sandwich_middle_layout<cga3, VL, XL>.size(), core::detail::dropped_blade_count<cga3, VL, XL>());
  for (std::size_t i = 0; i < k1.size(); ++i) std::printf("%s[%zu,%zu,%zu,%lld,%d]", i ? "," : "", k1[i].a, k1[i].b, k1[i].out, (long long)k1[i].coefficient, (int)k1[i].first);
  std::printf("],\"second\":[");
  for (std::size_t i = 0; i < k2.size(); ++i) std::printf("%s[%zu,%zu,%zu,%lld,%d]", i ? "," : "", k2[i].a, k2[i].b, k2[i].out, (long long)k2[i].coefficient, (int)k2[i].first);
  std::printf("],\"K\":[");
  for (std::size_t i = 0; i < K.size(); ++i) std::printf("%s%d", i ? "," : "", K[i]);
  std::printf("],\"cases\":[");
  std::mt19937_64 s(20261107); int printed = 0;
  for (int t = 0; t < 3000 && printed < 30; ++t) {
    const auto m = [&] { return unit(core::compose(rnd<Plane>(s), rnd<Plane>(s))); };
    Chain q = unit(core::compose(m(), m()));
    for (int i = 2; i < 16; ++i) q = unit(core::compose(m(), q));
    const Obj x = rnd<Obj>(s);
    auto d = core::detail::transform_outer<cga3, VL, XL, false>(core::detail::transform_middle<cga3, VL, XL, true>(q.coefficients, x.coefficients), q.coefficients);
    auto tl = core::detail::transform_dropped_tolerances<cga3, VL, XL>(q.coefficients, x.coefficients);
    bool fire = false; for (std::size_t j = 0; j < d.size(); ++j) fire = fire || std::abs(d[j]) > tl[j];
    if (!fire) continue;
    std::printf("%s{\"v\":[", printed++ ? "," : "");
    for (std::size_t k = 0; k < VL.size(); ++k) std::printf("%s\"%a\"", k ? "," : "", q.coefficients[k]);
    std::printf("],\"x\":[");
    for (std::size_t k = 0; k < XL.size(); ++k) std::printf("%s\"%a\"", k ? "," : "", x.coefficients[k]);
    std::printf("]}");
  }
  std::printf("]}\n");
}
