// The 1u lemma of ../rotation-normalize-tolerance.md, checked exhaustively
// over float: for every float s in [1/2, 1), |fl(5 fl(s / 5)) - s| <= u,
// with u = epsilon / 2. The negative half is its mirror under round to
// nearest. Built and run with
//   g++ -std=c++23 -O0 -ffp-contract=off five.cpp -o five && ./five
// and the same with clang++; output in five.out.
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>

int main() {
  const float u = std::numeric_limits<float>::epsilon() / 2;
  float worst = 0;
  std::uint64_t count = 0;
  std::uint64_t above_half_u = 0;
  for (std::uint32_t b = std::bit_cast<std::uint32_t>(0.5f);
       b < std::bit_cast<std::uint32_t>(1.0f); ++b) {
    // volatile, so that each operation rounds to float where it is written.
    const volatile float s = std::bit_cast<float>(b);
    const volatile float q = s / 5.0f;
    const volatile float r = 5.0f * q;
    const float d = std::fabs(r - s);
    if (d > worst) {
      worst = d;
    }
    if (d > u / 2) {
      ++above_half_u;
    }
    ++count;
  }
  std::printf("floats %llu, max |fl(5 fl(s/5)) - s| = %g u, "
              "count above u/2: %llu\n",
              static_cast<unsigned long long>(count), worst / u,
              static_cast<unsigned long long>(above_half_u));
  return worst <= u ? 0 : 1;
}
