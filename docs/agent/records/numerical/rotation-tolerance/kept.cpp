// Counts K and the magnitude of each kept coefficient of a rigid(3) point
// transformed by the motor s e43 + c e1234, over the tracked oracle.
// Build from the repository root:
//   g++ -std=c++23 -O1 -I include -I test kept.cpp -o kept && ./kept
#include <array>
#include <cstdio>
#include <galoot/core.hpp>
#include <support/tracked.hpp>

namespace core = galoot::core;
using test_support::rational;
using test_support::tracked;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto motor_layout =
    core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;

void run(rational s, rational c, std::array<rational, 4> x, const char* what) {
  std::array<tracked, 8> v{};
  v.at(2) = tracked::input(s);
  v.at(3) = tracked::input(c);
  std::array<tracked, 4> xt{};
  for (int k = 0; k < 4; ++k) xt.at(k) = tracked::input(x.at(k));
  const auto kept = core::detail::transform_outer<rga3, motor_layout, point_layout, true>(
      core::detail::transform_middle<rga3, motor_layout, point_layout, false>(v, xt), v);
  for (int j = 0; j < 4; ++j) {
    const auto& t = kept.at(j);
    std::printf("%s coef %d: value %lld/%lld magnitude %lld/%lld K %d\n", what, j,
                (long long)t.value.numerator, (long long)t.value.denominator,
                (long long)t.magnitude.numerator, (long long)t.magnitude.denominator,
                t.roundings);
  }
}

int main() {
  const rational one{1}, zero{0};
  const std::array<std::array<rational, 4>, 4> basis{{{one, zero, zero, zero},
                                                       {zero, one, zero, zero},
                                                       {zero, zero, one, zero},
                                                       {zero, zero, zero, one}}};
  const char* names[] = {"x=e1", "x=e2", "x=e3", "x=e4"};
  for (int b = 0; b < 4; ++b) {
    std::printf("--- (s,c)=(1,0) %s\n", names[b]); run(one, zero, basis[b], names[b]);
    std::printf("--- (s,c)=(0,1) %s\n", names[b]); run(zero, one, basis[b], names[b]);
    std::printf("--- (s,c)=(1,1) %s\n", names[b]); run(one, one, basis[b], names[b]);
  }
  // The quarter-turn example, exact: s = c = 1 after scaling, x = (1,0,0,1).
  std::printf("--- (s,c)=(3/5,4/5) x=(1,0,0,1)\n");
  run(rational{3, 5}, rational{4, 5}, {one, zero, zero, one}, "3/5,4/5");
  std::printf("--- (s,c)=(1,1) x=(1,0,0,1)\n");
  run(one, one, {one, zero, zero, one}, "1,1");
}
