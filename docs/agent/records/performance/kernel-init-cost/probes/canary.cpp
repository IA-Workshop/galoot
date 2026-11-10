#include <galoot/core.hpp>
#include <support/rational.hpp>
#include <cstdio>
namespace core = galoot::core;
using test_support::rational;
struct rga3 { static constexpr auto value = core::rigid<3>(); };
inline constexpr auto motor_layout = core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
using Motor = core::mv<rga3, motor_layout, rational>;
using Part = core::mv<rga3, core::blades<rga3, "e42 e43">, rational>;
using E1 = core::mv<rga3, core::blades<rga3, "e1">, rational>;
using F2 = core::mv<rga3, core::blades<rga3, "e2">, rational>;
constexpr Motor conv = Motor{Part{rational{1}, rational{2}}};
constexpr Motor prod = core::detail::apply<core::detail::BinaryOperation::wedge, motor_layout>(E1{rational{3}}, F2{rational{5}});
static_assert(prod == Motor{rational{0},rational{0},rational{0},rational{0},rational{0},rational{0},rational{15},rational{0}});
int main() {
  volatile std::int64_t one = 1;
  const Motor m{Part{rational{one}, rational{2}}};
  const Motor p = core::detail::apply<core::detail::BinaryOperation::wedge, motor_layout>(E1{rational{3*one}}, F2{rational{5}});
  int bad = 0;
  std::printf("conv:"); for (auto& c : m.coefficients) { std::printf(" %lld", (long long)c.denominator); bad |= c.denominator != 1; } std::printf("\n");
  std::printf("prod:"); for (auto& c : p.coefficients) { std::printf(" %lld", (long long)c.denominator); bad |= (c.denominator != 1) << 1; } std::printf("\n");
  return bad;
}
