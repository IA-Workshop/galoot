// Prints the first and second (dropped) kernels of several transforms as JSON
// lines, and for a fixed probe input the core's dropped values and
// tolerances, so that a Python simulation can be checked against the core.
#include <cstdio>
#include <galoot/core.hpp>
namespace core = galoot::core;
struct rga3 { static constexpr auto value = core::rigid<3>(); };
struct cga3 { static constexpr auto value = core::conformal<3>(); };
struct r400 { static constexpr auto value = core::R<4, 0, 0>(); };
struct ga5 { static constexpr auto value = core::R<5, 0, 0>(); };

template <class Alg, core::blade_set V, core::blade_set X>
void dump(const char* name) {
  constexpr const auto& k1 = core::detail::first_kernel<Alg, V, X, true>;
  constexpr const auto& k2 = core::detail::second_kernel<Alg, V, X, false>;
  constexpr auto& K = core::detail::transform_dropped_roundings<Alg, V, X>;
  std::printf("{\"name\":\"%s\",\"nv\":%zu,\"nx\":%zu,\"nm\":%zu,\"nd\":%zu,\"first\":[", name,
              V.size(), X.size(), core::detail::sandwich_middle_layout<Alg, V, X>.size(),
              core::detail::dropped_blade_count<Alg, V, X>());
  for (std::size_t i = 0; i < k1.size(); ++i)
    std::printf("%s[%zu,%zu,%zu,%lld,%d]", i ? "," : "", k1[i].a, k1[i].b, k1[i].out,
                (long long)k1[i].coefficient, (int)k1[i].first);
  std::printf("],\"second\":[");
  for (std::size_t i = 0; i < k2.size(); ++i)
    std::printf("%s[%zu,%zu,%zu,%lld,%d]", i ? "," : "", k2[i].a, k2[i].b, k2[i].out,
                (long long)k2[i].coefficient, (int)k2[i].first);
  std::printf("],\"K\":[");
  for (std::size_t i = 0; i < K.size(); ++i) std::printf("%s%d", i ? "," : "", K[i]);
  // probe: v_k = (k+1)/7 * (-1)^k, x_k = (k+2)/3
  std::array<double, V.size()> v{};
  std::array<double, X.size()> x{};
  for (std::size_t k = 0; k < v.size(); ++k) v[k] = (k % 2 ? -1.0 : 1.0) * double(k + 1) / 7.0;
  for (std::size_t k = 0; k < x.size(); ++k) x[k] = double(k + 2) / 3.0;
  auto d = core::detail::transform_outer<Alg, V, X, false>(
      core::detail::transform_middle<Alg, V, X, true>(v, x), v);
  auto t = core::detail::transform_dropped_tolerances<Alg, V, X>(v, x);
  std::printf("],\"probe_d\":[");
  for (std::size_t i = 0; i < d.size(); ++i) std::printf("%s\"%a\"", i ? "," : "", d[i]);
  std::printf("],\"probe_tol\":[");
  for (std::size_t i = 0; i < t.size(); ++i) std::printf("%s\"%a\"", i ? "," : "", t[i]);
  std::printf("]}\n");
}
int main() {
  constexpr auto point = core::blades<rga3, "e1 e2 e3 e4">;
  constexpr auto line = core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
  constexpr auto plane = core::blades<rga3, "e423 e431 e412 e321">;
  constexpr auto motor = core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;
  constexpr auto flector = core::blades<rga3, "e1 e2 e3 e4 e423 e431 e412 e321">;
  dump<rga3, motor, point>("rga3 motor point");
  dump<rga3, motor, line>("rga3 motor line");
  dump<rga3, motor, plane>("rga3 motor plane");
  dump<rga3, flector, point>("rga3 flector point");
  dump<rga3, flector, line>("rga3 flector line");
  using H = core::mv<cga3, core::of_grade<5, 4>, double>;
  using M = decltype(H{} % H{});
  using F = decltype((H{} % H{}) % H{});
  dump<cga3, core::of_grade<5, 4>, core::of_grade<5, 1>>("cga3 plane vector");
  dump<cga3, M::layout, core::of_grade<5, 1>>("cga3 motor vector");
  dump<cga3, F::layout, core::of_grade<5, 1>>("cga3 flector vector");
  dump<cga3, M::layout, core::of_grade<5, 2>>("cga3 motor bivector");
  dump<r400, core::blades<r400, "1 e12 e13 e14 e23 e24 e34 e1234">, core::blades<r400, "1">>("r400 even scalar");
  dump<r400, core::blades<r400, "1 e12 e13 e14 e23 e24 e34 e1234">, core::of_grade<4, 1>>("r400 even vector");
}
