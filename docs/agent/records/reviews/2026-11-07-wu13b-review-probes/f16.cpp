// Is the rounding check applied to std::float16_t / std::bfloat16_t, and does
// it hold for random rigid(3) motors on points (dropped part zero by identity)?
#include <cstdio>
#include <random>
#include <stdfloat>
#include <galoot/core.hpp>
namespace core = galoot::core;
struct rga3 { static constexpr auto value = core::rigid<3>(); };
template <class T> void run(const char* name) {
  constexpr auto PL = core::blades<rga3, "e1 e2 e3 e4">;
  constexpr auto ML = core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;
  std::printf("%s: rounding_checked_scalar %d, ring_scalar %d\n", name,
              (int)core::detail::rounding_checked_scalar<T>, (int)core::ring_scalar<T>);
  if constexpr (core::detail::rounding_checked_scalar<T>) {
    std::mt19937_64 s(7); std::uniform_real_distribution<float> u(-1.0F, 1.0F);
    int fired = 0; double worst = 0;
    for (int t = 0; t < 200000; ++t) {
      std::array<T, 8> v{}; std::array<T, 4> x{};
      for (auto& c : v) c = static_cast<T>(u(s)); for (auto& c : x) c = static_cast<T>(u(s));
      auto d = core::detail::transform_outer<rga3, ML, PL, false>(core::detail::transform_middle<rga3, ML, PL, true>(v, x), v);
      auto tl = core::detail::transform_dropped_tolerances<rga3, ML, PL>(v, x);
      bool f = false;
      for (std::size_t j = 0; j < d.size(); ++j) { double a = std::abs(double(d[j])), b = double(tl[j]); if (a > b) f = true; if (b > 0) worst = std::max(worst, a / b); }
      fired += f;
    }
    std::printf("  fired %d of 200000, largest |d|/tol %.3g\n", fired, worst);
  }
}
int main() { run<std::float16_t>("float16_t"); run<std::bfloat16_t>("bfloat16_t"); run<float>("float"); }
