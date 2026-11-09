// The defect reaches a Galoot conversion between layouts over the test
// rational. Motor{part} copies a value on the layout "e42 e43" onto the motor
// layout "1 e41 e42 e43 e23 e31 e12 e1234" through the library's run-time
// map kernel, which value-initializes its result array and assigns the two
// places the source holds, 2 and 3. The constant `once` evaluates the same
// kernel instantiation, whose first write lands on place 2. Every other
// place of the run-time result must be 0/1. Exit status 1 means some place
// is not, and the program prints the denominators.
// Build from the repository root:
//   g++ -std=c++23 -O0 -Iinclude -Itest <this file>
// With -DWITHOUT_CONSTANT_EVALUATION the constant is left out.
#include <galoot/core.hpp>
#include <support/rational.hpp>

#include <cstdio>

namespace core = galoot::core;
using test_support::rational;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
inline constexpr auto motor_layout =
    core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
inline constexpr auto part_layout = core::blades<rga3, "e42 e43">;
using Motor = core::mv<rga3, motor_layout, rational>;
using Part = core::mv<rga3, part_layout, rational>;

#ifndef WITHOUT_CONSTANT_EVALUATION
constexpr Motor once = Motor{Part{rational{1}, rational{2}}};
static_assert(once == Motor{rational{0}, rational{0}, rational{1}, rational{2},
                            rational{0}, rational{0}, rational{0}, rational{0}});
#endif

int main() {
  volatile std::int64_t one = 1;
  const Motor m{Part{rational{one}, rational{2}}};
  const Motor expected{rational{0}, rational{0}, rational{1}, rational{2},
                       rational{0}, rational{0}, rational{0}, rational{0}};
  std::printf("denominators:");
  for (const rational& c : m.coefficients) {
    std::printf(" %lld", static_cast<long long>(c.denominator));
  }
  std::printf("; equal to the expected motor: %s\n",
              m == expected ? "yes" : "no");
  return m == expected ? 0 : 1;
}
