// Calls the forced and the free unit on the same random inputs and counts
// the inputs where the two results differ in any bit.
#include <bit>
#include <cstdint>
#include <cstdio>
#include <random>

#include <galoot/core.hpp>

namespace core = galoot::core;

struct r300 {
  static constexpr auto value = core::R<3, 0, 0>();
};
using Rotor = core::mv<r300, core::blades<r300, "1 e12 e13 e23">, double>;
using Vector = core::mv<r300, core::blades<r300, "e1 e2 e3">, double>;

Vector forced_unit(const Vector& x, const Rotor& v);
Vector free_unit(const Vector& x, const Rotor& v);

int main() {
  std::mt19937_64 random(20261121);
  std::uniform_real_distribution<double> uniform(-1.0, 1.0);
  const int trials = 100000;
  int differ = 0;
  for (int t = 0; t < trials; ++t) {
    const Rotor v{uniform(random), uniform(random), uniform(random),
                  uniform(random)};
    const Vector x{uniform(random), uniform(random), uniform(random)};
    const Vector a = forced_unit(x, v);
    const Vector b = free_unit(x, v);
    for (std::size_t i = 0; i < 3; ++i) {
      if (std::bit_cast<std::uint64_t>(a.coefficients[i]) !=
          std::bit_cast<std::uint64_t>(b.coefficients[i])) {
        ++differ;
        break;
      }
    }
  }
  std::printf("%d of %d inputs differ in some bit\n", differ, trials);
  return 0;
}
