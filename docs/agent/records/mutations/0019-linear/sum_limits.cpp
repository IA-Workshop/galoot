// How large a sum of two layouts the compilers build at their default
// limits. CASE_N is the size n of the algebra R(n,0,0); DISJOINT sums
// full<n-1> with the blades that hold e_n; otherwise full<n> plus its
// bivectors.
#include <cstddef>
#include <galoot/core/linear.hpp>
namespace core = galoot::core;
struct alg { static constexpr auto value = core::R<CASE_N, 0, 0>(); };
consteval auto with_last() {
  constexpr auto low = core::full<CASE_N - 1>;
  core::blade_set<low.size()> out{};
  for (std::size_t i = 0; i < low.size(); ++i) {
    out.blades[i] = {.mask = low.blades[i].mask | (core::mask_t{1} << (CASE_N - 1)), .sign = 1};
  }
  return out;
}
#ifdef DISJOINT
using A = core::mv<alg, core::full<CASE_N - 1>, double>;
using B = core::mv<alg, with_last(), double>;
#else
using A = core::mv<alg, core::full<CASE_N>, double>;
using B = core::mv<alg, core::of_grade<CASE_N, 2>, double>;
#endif
double use(const A& a, const B& b) { return (a + b).coefficients[0]; }
