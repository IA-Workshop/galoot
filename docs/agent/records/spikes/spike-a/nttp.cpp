// Spike A: the metric and blade set as class-type template arguments (value
// NTTP) against tag types that hold the value. Both variants share the plan
// generator; they differ only in what the user-facing types are keyed on.
//
//   -DSPIKE_VALUE   mv<metric M, blade_set B>
//   -DSPIKE_TAG     mv<class Algebra, class Blades>, each with ::value
//   -DSPIKE_HYBRID  mv<class Algebra, blade_set B>: a tag for the algebra,
//                   a value for the blade set
//   -DSPIKE_MISUSE  multiply a conformal(3) vector by an R(4,1,0) vector:
//                   same n, different algebras, which must not compile.
//
// Without SPIKE_MISUSE the unit defines `use`, whose call of gp is emitted
// out of line at -O0, so `nm` shows the mangled name of gp's instantiation.
#include "gen.hpp"

#include <array>
#include <cstddef>

namespace spike {

// The blades with a surviving term in plan<M, A, B>, in mask order.
template <auto M, auto A, auto B> constexpr auto result_set() {
  constexpr std::size_t size = std::size_t{1} << M.g.size();
  constexpr auto seen = [] {
    std::array<bool, size> s{};
    for (const auto& t : plan<M, A, B>) {
      s[t.out] = true;
    }
    return s;
  }();
  constexpr std::size_t count = [&] {
    std::size_t c = 0;
    for (bool b : seen) {
      c += b ? 1 : 0;
    }
    return c;
  }();
  blade_set<count> r{};
  std::size_t k = 0;
  for (std::size_t m = 0; m < size; ++m) {
    if (seen[m]) {
      r.masks[k++] = static_cast<mask_t>(m);
    }
  }
  return r;
}

template <std::size_t K>
constexpr std::size_t index_of(const blade_set<K>& s, mask_t m) {
  for (std::size_t i = 0; i < K; ++i) {
    if (s.masks[i] == m) {
      return i;
    }
  }
  return K;
}

// Shared kernel: r += a * b over the plan's terms.
template <auto M, auto A, auto B, class R, class X, class Y>
constexpr void apply_gp(R& r, const X& a, const Y& b) {
  constexpr auto out = result_set<M, A, B>();
  for (const auto& t : plan<M, A, B>) {
    r[index_of(out, t.out)] +=
        static_cast<double>(t.coef) * a[t.a] * b[t.b];
  }
}

constexpr metric<5> r410 = [] {
  metric<5> m = euclidean<5>();
  m.g[4][4] = -1;
  return m;
}();
constexpr metric<5> conf3 = conformal<5>();
constexpr blade_set<5> vectors5{{1, 2, 4, 8, 16}};

} // namespace spike

#if defined(SPIKE_VALUE)
namespace value {

template <spike::metric M, spike::blade_set B> struct mv {
  std::array<double, B.masks.size()> c{};
};

template <spike::metric M, spike::blade_set A, spike::blade_set B>
mv<M, spike::result_set<M, A, B>()> gp(const mv<M, A>& a, const mv<M, B>& b) {
  mv<M, spike::result_set<M, A, B>()> r{};
  spike::apply_gp<M, A, B>(r.c, a.c, b.c);
  return r;
}

using cga_vector = mv<spike::conf3, spike::vectors5>;
using r410_vector = mv<spike::r410, spike::vectors5>;

} // namespace value
using value::gp;
using cga_vector = value::cga_vector;
using r410_vector = value::r410_vector;

#elif defined(SPIKE_TAG)
namespace tag {

struct conformal3 {
  static constexpr auto value = spike::conf3;
};
struct r410 {
  static constexpr auto value = spike::r410;
};
struct vectors5 {
  static constexpr auto value = spike::vectors5;
};
template <class Algebra, class A, class B> struct gp_blades {
  static constexpr auto value =
      spike::result_set<Algebra::value, A::value, B::value>();
};

template <class Algebra, class Blades> struct mv {
  std::array<double, Blades::value.masks.size()> c{};
};

template <class Algebra, class A, class B>
mv<Algebra, gp_blades<Algebra, A, B>> gp(const mv<Algebra, A>& a,
                                         const mv<Algebra, B>& b) {
  mv<Algebra, gp_blades<Algebra, A, B>> r{};
  spike::apply_gp<Algebra::value, A::value, B::value>(r.c, a.c, b.c);
  return r;
}

using cga_vector = mv<conformal3, vectors5>;
using r410_vector = mv<r410, vectors5>;

} // namespace tag
using tag::gp;
using cga_vector = tag::cga_vector;
using r410_vector = tag::r410_vector;

#elif defined(SPIKE_HYBRID)
namespace hybrid {

struct conformal3 {
  static constexpr auto value = spike::conf3;
};
struct r410 {
  static constexpr auto value = spike::r410;
};

template <class Algebra, spike::blade_set B> struct mv {
  std::array<double, B.masks.size()> c{};
};

template <class Algebra, spike::blade_set A, spike::blade_set B>
mv<Algebra, spike::result_set<Algebra::value, A, B>()>
gp(const mv<Algebra, A>& a, const mv<Algebra, B>& b) {
  mv<Algebra, spike::result_set<Algebra::value, A, B>()> r{};
  spike::apply_gp<Algebra::value, A, B>(r.c, a.c, b.c);
  return r;
}

using cga_vector = mv<conformal3, spike::vectors5>;
using r410_vector = mv<r410, spike::vectors5>;

} // namespace hybrid
using hybrid::gp;
using cga_vector = hybrid::cga_vector;
using r410_vector = hybrid::r410_vector;

#else
#error "define SPIKE_VALUE, SPIKE_TAG, or SPIKE_HYBRID"
#endif

#ifdef SPIKE_MISUSE
double misuse(const cga_vector& a, const r410_vector& b) {
  return gp(a, b).c[0];
}
#else
double use(const cga_vector& a, const cga_vector& b) { return gp(a, b).c[0]; }
// Products of products: the result type of each gp feeds the next.
double use3(const cga_vector& a, const cga_vector& b, const cga_vector& c,
            const cga_vector& d) {
  return gp(gp(gp(a, b), c), d).c[0];
}
#endif
