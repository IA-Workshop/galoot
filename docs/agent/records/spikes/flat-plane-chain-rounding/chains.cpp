// Chains of composed conformal(3) motors, generated exactly as the WU13b
// review probe drift.cpp does (same draws, same order, same seed), with
// every case dumped for an exact recomputation.
//
//   chains <seed>          one JSON line per family (the layouts, the dropped
//                          blades and their counts K), then one per case: the
//                          family, N, the trial, and V, x, the dropped
//                          coefficients d and their tolerances, in hex
//   steps <seed> <trials>  for each trial, a chain of 128 motors of each
//                          family with every factor and every partial
//                          product dumped, for the defect's trajectory
//   renorm <seed>          the flat-plane chains of `chains` (same draws),
//                          with the projection of renormalize() applied once
//                          to the finished chain, or after every
//                          composition; prints the rate at which the check
//                          would fire, by N
//
// Built with -std=c++23 -O2 -ffp-contract=off -DGALOOT_CHECKING=0 against the
// repository's include/ (README.md).
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <galoot/core.hpp>
namespace core = galoot::core;
struct cga3 { static constexpr auto value = core::conformal<3>(); };

double draw(std::mt19937_64& s) { return (static_cast<double>(s() >> 11) * 0x1p-53 * 2.0) - 1.0; }
template <class M> M rnd(std::mt19937_64& s) { M m; for (double& c : m.coefficients) c = draw(s); return m; }
template <class M> M unitize_scalar(M m) {
  double big = 0; for (double c : m.coefficients) big = std::max(big, std::abs(c));
  int e; std::frexp(big, &e); for (double& c : m.coefficients) c = std::ldexp(c, -e); return m;
}

template <std::size_t K> void hex_array(const char* key, const std::array<double, K>& a) {
  std::printf("\"%s\":[", key);
  for (std::size_t i = 0; i < K; ++i) std::printf("%s\"%a\"", i ? "," : "", a[i]);
  std::printf("]");
}
template <auto L> void layout(const char* key) {
  std::printf("\"%s\":[", key);
  for (std::size_t i = 0; i < L.size(); ++i) std::printf("%s[%u,%d]", i ? "," : "", static_cast<unsigned>(L.blades[i].mask), L.blades[i].sign);
  std::printf("]");
}

template <auto P> struct family_types {
  using Plane = core::mv<cga3, P, double>;
  using Motor = decltype(core::compose(Plane{}, Plane{}));
  using Chain = decltype(core::compose(Motor{}, Motor{}));
  using Obj = core::mv<cga3, core::of_grade<5, 1>, double>;
  static_assert(std::is_same_v<Chain, decltype(core::compose(Motor{}, Chain{}))>);
};

template <class Alg, auto VL, auto XL> void meta(const char* name) {
  constexpr auto& second = core::detail::sandwich_second_plan<Alg, VL, XL>;
  std::printf("{\"meta\":\"%s\",", name);
  layout<VL>("v"); std::printf(","); layout<XL>("x");
  std::printf(",\"dropped\":[");
  bool comma = false;
  for (std::size_t o = 0; o < second.result.size(); ++o)
    if (core::detail::position_of(XL, second.result.blades[o].mask) >= XL.size()) {
      std::printf("%s%u", comma ? "," : "", static_cast<unsigned>(second.result.blades[o].mask)); comma = true;
    }
  std::printf("],\"K\":[");
  constexpr auto& K = core::detail::transform_dropped_roundings<Alg, VL, XL>;
  for (std::size_t i = 0; i < K.size(); ++i) std::printf("%s%d", i ? "," : "", K[i]);
  std::printf("]}\n");
}

template <class Alg, auto VL, auto XL>
void dump_case(const char* name, int n, int t, const std::array<double, VL.size()>& v, const std::array<double, XL.size()>& x) {
  auto d = core::detail::transform_outer<Alg, VL, XL, false>(core::detail::transform_middle<Alg, VL, XL, true>(v, x), v);
  auto tol = core::detail::transform_dropped_tolerances<Alg, VL, XL>(v, x);
  std::printf("{\"family\":\"%s\",\"N\":%d,\"trial\":%d,", name, n, t);
  hex_array("v", v); std::printf(","); hex_array("x", x); std::printf(",");
  hex_array("d", d); std::printf(","); hex_array("tol", tol); std::printf("}\n");
}

// The loop of drift.cpp's run(), unchanged but for the dump.
template <auto P> void run(const char* name, std::mt19937_64& s, int trials) {
  using F = family_types<P>;
  using Plane = typename F::Plane; using Chain = typename F::Chain; using Obj = typename F::Obj;
  meta<cga3, Chain::layout, Obj::layout>(name);
  for (int n : {2, 4, 8, 16, 32, 64, 128}) {
    for (int t = 0; t < trials; ++t) {
      const auto m = [&] { return unitize_scalar(core::compose(rnd<Plane>(s), rnd<Plane>(s))); };
      Chain q = unitize_scalar(core::compose(m(), m()));
      for (int i = 2; i < n; ++i) q = unitize_scalar(core::compose(m(), q));
      const Obj x = rnd<Obj>(s);
      dump_case<cga3, Chain::layout, Obj::layout>(name, n, t, q.coefficients, x.coefficients);
    }
  }
}

// One chain of 128 motors per trial, every step dumped. The planes are drawn
// in a fixed order, a then b, so that the dump does not depend on the
// compiler's order of evaluating arguments.
template <auto P> void steps(const char* name, std::mt19937_64& s, int trials) {
  using F = family_types<P>;
  using Plane = typename F::Plane; using Motor = typename F::Motor; using Chain = typename F::Chain;
  meta<cga3, Chain::layout, F::Obj::layout>(name);
  std::printf("{\"meta_motor\":\"%s\",", name); layout<Motor::layout>("m"); std::printf(","); layout<Plane::layout>("p"); std::printf("}\n");
  for (int t = 0; t < trials; ++t) {
    Chain q{};
    for (int i = 0; i < 128; ++i) {
      const Plane a = rnd<Plane>(s);
      const Plane b = rnd<Plane>(s);
      const Motor m = unitize_scalar(core::compose(a, b));
      if (i == 0) q = Chain{m}; else q = unitize_scalar(core::compose(m, q));
      std::printf("{\"family\":\"%s\",\"trial\":%d,\"step\":%d,", name, t, i);
      hex_array("a", a.coefficients); std::printf(","); hex_array("b", b.coefficients); std::printf(",");
      hex_array("m", m.coefficients); std::printf(","); hex_array("q", q.coefficients); std::printf("}\n");
    }
  }
}

// The flat-plane chain layout is e5, e125, e135, e145, e235, e245, e345,
// e12345, here q, a1, a2, b1, a3, b2, b3, s. For any v on it,
// v̰ ⟇ v = λ𝟙 + 2h e5 with λ = b1² + b2² + b3² + s² and
// h = -a1 b3 + a2 b2 - a3 b1 + q s, and v is a versor exactly when h = 0.
// v ⟇ (𝟙 + α e5) keeps λ and has h + αλ, so α = -h / λ removes the defect:
// q += α s, a1 -= α b3, a2 += α b2, a3 -= α b1, with b and s unchanged.
template <class Chain> Chain renormalize(Chain v) {
  auto& c = v.coefficients;
  const double q = c[0], a1 = c[1], a2 = c[2], b1 = c[3], a3 = c[4], b2 = c[5], b3 = c[6], s = c[7];
  const double h = (((-a1 * b3) + (a2 * b2)) - (a3 * b1)) + (q * s);
  const double lambda = (((b1 * b1) + (b2 * b2)) + (b3 * b3)) + (s * s);
  const double alpha = -h / lambda;
  c[0] = q + (alpha * s);
  c[1] = a1 - (alpha * b3);
  c[2] = a2 + (alpha * b2);
  c[4] = a3 - (alpha * b1);
  return v;
}

template <class Alg, auto VL, auto XL>
double ratio(const std::array<double, VL.size()>& v, const std::array<double, XL.size()>& x) {
  auto d = core::detail::transform_outer<Alg, VL, XL, false>(core::detail::transform_middle<Alg, VL, XL, true>(v, x), v);
  auto t = core::detail::transform_dropped_tolerances<Alg, VL, XL>(v, x);
  double r = 0;
  for (std::size_t j = 0; j < d.size(); ++j) r = std::max(r, std::abs(d[j]) / t[j]);
  return r;
}

// The draws of run(), for the flat family only (it runs first there).
template <auto P> void renorm(std::mt19937_64& s, int trials) {
  using F = family_types<P>;
  using Plane = typename F::Plane; using Chain = typename F::Chain; using Obj = typename F::Obj;
  std::printf("N\tcases\tfired raw\tlargest raw\tfired renormalized once\tlargest\tfired renormalized every step\tlargest\n");
  for (int n : {2, 4, 8, 16, 32, 64, 128}) {
    int fired[3] = {0, 0, 0}; double worst[3] = {0, 0, 0};
    for (int t = 0; t < trials; ++t) {
      const auto m = [&] { return unitize_scalar(core::compose(rnd<Plane>(s), rnd<Plane>(s))); };
      // GCC evaluates the arguments of compose(m(), m()) in run() right to
      // left; the raw counts below equal run()'s, which checks this.
      const auto m0 = m();
      const auto m1 = m();
      Chain q = unitize_scalar(core::compose(m1, m0));
      Chain e = renormalize(q);
      for (int i = 2; i < n; ++i) {
        const auto mi = m();
        q = unitize_scalar(core::compose(mi, q));
        e = renormalize(unitize_scalar(core::compose(mi, e)));
      }
      const Obj x = rnd<Obj>(s);
      const double r[3] = {ratio<cga3, Chain::layout, Obj::layout>(q.coefficients, x.coefficients),
                           ratio<cga3, Chain::layout, Obj::layout>(renormalize(q).coefficients, x.coefficients),
                           ratio<cga3, Chain::layout, Obj::layout>(e.coefficients, x.coefficients)};
      for (int k = 0; k < 3; ++k) { fired[k] += r[k] > 1 ? 1 : 0; worst[k] = std::max(worst[k], r[k]); }
    }
    std::printf("%d\t%d\t%d\t%.3g\t%d\t%.3g\t%d\t%.3g\n", n, trials, fired[0], worst[0], fired[1], worst[1], fired[2], worst[2]);
  }
}

int main(int argc, char** argv) {
  constexpr auto flat = core::blades<cga3, "e1235 e1245 e1345 e2345">;
  constexpr auto hyper = core::of_grade<5, 4>;
  if (argc >= 2 && std::strcmp(argv[1], "chains") == 0) {
    std::mt19937_64 s(argc > 2 ? std::strtoull(argv[2], nullptr, 10) : 1);
    run<flat>("flat", s, 1000);
    run<hyper>("hyper", s, 1000);
    return 0;
  }
  if (argc >= 4 && std::strcmp(argv[1], "steps") == 0) {
    std::mt19937_64 s(std::strtoull(argv[2], nullptr, 10));
    const int trials = std::atoi(argv[3]);
    steps<flat>("flat", s, trials);
    steps<hyper>("hyper", s, trials);
    return 0;
  }
  if (argc >= 3 && std::strcmp(argv[1], "renorm") == 0) {
    std::mt19937_64 s(std::strtoull(argv[2], nullptr, 10));
    renorm<flat>(s, 1000);
    return 0;
  }
  std::fprintf(stderr, "usage: chains chains <seed> | chains steps <seed> <trials> | chains renorm <seed>\n");
  return 2;
}
