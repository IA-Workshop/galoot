// Spike D: which memoization form evaluates a plan once per argument set per
// translation unit. The plan is Spike A's full gp plan for R(5,0,0), with the
// algebra as a tag that carries its metric (decision 0021) and the blade sets
// as values. At -O0 it compiles under default constant-evaluation limits on
// both compilers. At -O1 and above Clang's fill pass exceeds its default step
// limit (2^20; it passes at 2^21), because libstdc++ 16 then checks element
// access in constant evaluation through a call of std::__is_constant_evaluated
// (bits/c++config.h, which enables _GLIBCXX_ASSERTIONS only without
// __OPTIMIZE__). run_memo.py therefore raises the limits, as Spike A did.
//
//   -DSPIKE_MEMO=0  none: each use holds its own `static constexpr` table,
//                   the control that is not memoized across uses
//   -DSPIKE_MEMO=1  inline constexpr variable template
//   -DSPIKE_MEMO=2  class-static constexpr data member of a class template
//   -DSPIKE_MEMO=3  function-local static constexpr in a constexpr function
//                   template that returns a reference to it (C++23, P2647)
//   -DSPIKE_MEMO=4  calibration: as 0, but each use calls a distinct
//                   specialization make_plan<..., K>, so no compiler can share
//                   one evaluation between uses; U evaluations are expected
//   -DSPIKE_USES=U  U distinct function template specializations use<K> read
//                   the plan, both at run time (the loop) and in a constant
//                   expression (the result array's size); U = 0 is the
//                   baseline, the header and an empty main
//   -DSPIKE_INIT=consteval or constexpr: the specifier of make_plan
//   -DSPIKE_SECOND  a second, different plan (gp of vectors by bivectors)
//                   used once beside the first, to show the forms keep
//                   argument sets apart
//
// Run by run_memo.py, which passes -I to records/spikes/spike-a for gen.hpp.
#include "gen.hpp"

#include <array>
#include <cstddef>
#include <span>
#include <utility>

#ifndef SPIKE_MEMO
#error "define SPIKE_MEMO"
#endif
#ifndef SPIKE_USES
#define SPIKE_USES 1
#endif
#ifndef SPIKE_INIT
#define SPIKE_INIT consteval
#endif

namespace memo {

using spike::blade_set;
using spike::mask_t;
using spike::plan_term;

struct r500 {
  static constexpr spike::metric<5> value = spike::euclidean<5>();
};

// The counting pass runs once per specialization of make_plan, because size
// is a constant of the specialization. The filling pass runs each time the
// call is evaluated, which is what the forms below differ in.
template <class Alg, blade_set A, blade_set B, int Distinct = 0>
SPIKE_INIT auto make_plan() {
  constexpr std::size_t n = Alg::value.g.size();
  constexpr std::size_t size = spike::generate<n>(
      Alg::value, std::span<const mask_t>(A.masks),
      std::span<const mask_t>(B.masks), {});
  std::array<plan_term, size> out{};
  spike::generate<n>(Alg::value, std::span<const mask_t>(A.masks),
                     std::span<const mask_t>(B.masks),
                     std::span<plan_term>(out));
  return out;
}

#if SPIKE_MEMO == 1
template <class Alg, blade_set A, blade_set B>
inline constexpr auto plan_v = make_plan<Alg, A, B>();
#define SPIKE_PLAN(Alg, A, B) (plan_v<Alg, A, B>)
#elif SPIKE_MEMO == 2
template <class Alg, blade_set A, blade_set B> struct plan_c {
  static constexpr auto value = make_plan<Alg, A, B>();
};
#define SPIKE_PLAN(Alg, A, B) (plan_c<Alg, A, B>::value)
#elif SPIKE_MEMO == 3
template <class Alg, blade_set A, blade_set B> constexpr const auto& plan_f() {
  static constexpr auto p = make_plan<Alg, A, B>();
  return p;
}
#define SPIKE_PLAN(Alg, A, B) (plan_f<Alg, A, B>())
#endif

constexpr auto all5 = spike::all<5>();

// One use: the result size is read in a constant expression and the terms at
// run time, as a product kernel would.
template <int K, class Alg, blade_set A, blade_set B>
double use(const double* a, const double* b) {
#if SPIKE_MEMO == 0
  static constexpr auto plan = make_plan<Alg, A, B>();
#elif SPIKE_MEMO == 4
  static constexpr auto plan = make_plan<Alg, A, B, K>();
#else
  constexpr const auto& plan = SPIKE_PLAN(Alg, A, B);
#endif
  std::array<double, plan.size() % 7 + 1> acc{};
  for (const auto& t : plan) {
    acc[t.out % acc.size()] += static_cast<double>(t.coef) * a[t.a] * b[t.b];
  }
  double s = K;
  for (double x : acc) {
    s += x;
  }
  return s;
}

} // namespace memo

int main(int argc, char**) {
  double a[32]{};
  double b[32]{};
  a[0] = argc;
  b[0] = argc;
  const double s = [&]<int... K>(std::integer_sequence<int, K...>) {
    return (0.0 + ... + memo::use<K, memo::r500, memo::all5, memo::all5>(a, b));
  }(std::make_integer_sequence<int, SPIKE_USES>{});
#ifdef SPIKE_SECOND
  constexpr spike::blade_set<10> bivectors{
      {3, 5, 6, 9, 10, 12, 17, 18, 20, 24}};
  constexpr spike::blade_set<5> vectors{{1, 2, 4, 8, 16}};
  const double t = memo::use<0, memo::r500, vectors, bivectors>(a, b);
#else
  const double t = 0;
#endif
  return s + t > 0 ? 0 : 1;
}
