// run_sum with out{} (the kernels before the fix): a public sum, whose
// target is the union of its operands' layouts, and a direct call of the
// kernel onto a target with one place more, each evaluated at compile time
// and then run at run time in the same translation unit, over the test
// rational. Prints the denominators of the run-time results.
#include <galoot/core.hpp>
#include <support/rational.hpp>
#include <cstdio>
#include <cstdint>
#include <utility>
namespace core = galoot::core;
namespace d = galoot::core::detail;
using test_support::rational;
struct r300 { static constexpr auto value = core::R<3, 0, 0>(); };
constexpr auto A = core::blades<r300, "e3">;
constexpr auto B = core::blades<r300, "e1 e2">;
constexpr auto U = core::sum_layout<r300, A, B>;
constexpr auto W = core::blades<r300, "e1 e2 e3 e12">;
using XA = core::mv<r300, A, rational>;
using XB = core::mv<r300, B, rational>;

template <auto Target>
constexpr std::array<rational, Target.size()> direct(const XA& x, const XB& y) {
  return d::run_sum<d::identity_kernel<A, Target>,
                    d::second_operand_kernel<A, B, Target, 1>, Target.size()>(
      x.coefficients, y.coefficients,
      std::make_index_sequence<d::identity_kernel<A, Target>.size()>{},
      std::make_index_sequence<B.size()>{});
}

constexpr auto public_ce = XA{rational{7}} + XB{rational{1}, rational{2}};
constexpr auto direct_union_ce = direct<U>(XA{rational{7}}, XB{rational{1}, rational{2}});
constexpr auto direct_wide_ce = direct<W>(XA{rational{7}}, XB{rational{1}, rational{2}});

int main() {
  const volatile std::int64_t seven = 7;
  const auto p = XA{rational{seven}} + XB{rational{1}, rational{2}};
  const auto u = direct<U>(XA{rational{seven}}, XB{rational{1}, rational{2}});
  const auto w = direct<W>(XA{rational{seven}}, XB{rational{1}, rational{2}});
  std::printf("union size %zu\npublic:", U.size());
  for (const auto& c : p.coefficients) std::printf(" %lld/%lld", (long long)c.numerator, (long long)c.denominator);
  std::printf("\ndirect union:");
  for (const auto& c : u) std::printf(" %lld/%lld", (long long)c.numerator, (long long)c.denominator);
  std::printf("\ndirect wide:");
  for (const auto& c : w) std::printf(" %lld/%lld", (long long)c.numerator, (long long)c.denominator);
  std::printf("\n");
  return (p.coefficients == public_ce.coefficients && u == direct_union_ce && w == direct_wide_ce) ? 0 : 1;
}
