// The values and scope statements of commits 74fd859..ad0bf70, against the
// library. Compiles only when every assertion holds; the program then prints
// one line.
#include <galoot/core.hpp>

#include <cstdio>

namespace core = galoot::core;

template <class Tag>
inline constexpr std::size_t gens = std::remove_cvref_t<decltype(Tag::value)>::generator_count;

template <class Tag> using Full = core::mv<Tag, core::full<gens<Tag>>, int>;

// classical(x, V) is V x Ṽ for an even V and V x̂ Ṽ for an odd one, on a
// full multivector x, so nothing is truncated.
template <class Tag, class V>
constexpr Full<Tag> classical_even(const Full<Tag>& x, const V& v) {
  return core::truncate<Full<Tag>>(v * x * ~v);
}
template <class Tag, class V>
constexpr Full<Tag> classical_odd(const Full<Tag>& x, const V& v) {
  return core::truncate<Full<Tag>>(v * core::grade_involution(x) * ~v);
}

// R5: the rotor t = (1, 0, 3)(0, 0, 1) on (4, -1, 2) in R(3, 0, 0), with the
// rotor in the blade order 1 e31 e23 e12 rather than the records' order.
struct e3 { static constexpr auto value = core::R<3, 0, 0>(); };
using Vec3 = core::mv<e3, core::blades<e3, "e3 e1 e2">, int>;
using Even3 = core::mv<e3, core::blades<e3, "1 e31 e23 e12">, int>;
constexpr Vec3 a{3, 1, 0}, b{1, 0, 0}, v{2, 4, -1};
constexpr Even3 t = core::truncate<Even3>(a * b);
static_assert(t == Even3{3, -1, 0, 0}, "t = 3 + e13 = 3 - e31");
static_assert(core::transform(v, t) == Vec3{-40, -20, 10}, "trap (-20, 10, -40)");
static_assert(core::transform(v, !t) == Vec3{-8, 44, -10}, "target (44, -10, -8)");
static_assert(core::truncate<Vec3>(t * v * ~t) == Vec3{-8, 44, -10}, "t v t~");
static_assert(core::truncate<Vec3>(~t * v * t) == Vec3{40, 20, -10}, "t~ v t, Theorem A");
static_assert(core::get<"e123">(!t) == 3 && core::get<"e2">(!t) == -1,
              "!t = 3 e123 - e2");

// Theorem B inside R(p, q, 0): one even and one odd versor in each form.
template <class Tag> struct vecs {
  using V = core::mv<Tag, core::of_grade<gens<Tag>, 1>, int>;
};
template <class Tag> constexpr bool theorem_b(int seed) {
  using V = typename vecs<Tag>::V;
  constexpr std::size_t n = gens<Tag>;
  V p{}, q{}, r{};
  Full<Tag> x{};
  // coefficients from a fixed pattern; none of these vectors is null in the
  // forms below
  for (std::size_t i = 0; i < n; ++i) {
    p.coefficients[i] = static_cast<int>((seed + 2 * i) % 5) - 1;
    q.coefficients[i] = static_cast<int>((3 * seed + i) % 4) + 1;
    r.coefficients[i] = static_cast<int>((seed * i + 1) % 3) - (i % 2 == 0 ? 0 : 2);
  }
  for (std::size_t i = 0; i < x.coefficients.size(); ++i) {
    x.coefficients[i] = static_cast<int>((7 * i + seed) % 9) - 4;
  }
  const auto ev = p * q;
  const auto od = p * q * r;
  return core::transform(x, core::bulk_dual(ev)) == classical_even<Tag>(x, ev) &&
         core::transform(x, core::bulk_dual(od)) == classical_odd<Tag>(x, od);
}
template <class Tag> constexpr bool complement_form(int seed) {
  using V = typename vecs<Tag>::V;
  constexpr std::size_t n = gens<Tag>;
  V p{}, q{};
  Full<Tag> x{};
  for (std::size_t i = 0; i < n; ++i) {
    p.coefficients[i] = static_cast<int>((seed + 2 * i) % 5) - 1;
    q.coefficients[i] = static_cast<int>((3 * seed + i) % 4) + 1;
  }
  for (std::size_t i = 0; i < x.coefficients.size(); ++i) {
    x.coefficients[i] = static_cast<int>((7 * i + seed) % 9) - 4;
  }
  const auto ev = p * q;
  return core::transform(x, !ev) == classical_even<Tag>(x, ev);
}

struct r210 { static constexpr auto value = core::R<2, 1, 0>(); };
struct r120 { static constexpr auto value = core::R<1, 2, 0>(); };
struct r220 { static constexpr auto value = core::R<2, 2, 0>(); };
struct r030 { static constexpr auto value = core::R<0, 3, 0>(); };
struct r110 { static constexpr auto value = core::R<1, 1, 0>(); };
static_assert(theorem_b<r210>(1) && theorem_b<r210>(2), "R(2, 1, 0)");
static_assert(theorem_b<r120>(1) && theorem_b<r120>(3), "R(1, 2, 0)");
static_assert(theorem_b<r220>(1) && theorem_b<r220>(2), "R(2, 2, 0)");
static_assert(complement_form<r030>(1) && complement_form<r030>(2), "! holds in R(0, 3, 0)");
static_assert(!complement_form<r110>(1), "! fails in R(1, 1, 0)");
static_assert(!complement_form<r210>(1), "! fails in R(2, 1, 0)");

// Just outside: a square other than ±1, a degenerate form, and a form whose
// squares are 1, -1, 1 but which is not diagonal.
struct d114 { static constexpr auto value = core::from_gram({{1, 0, 0}, {0, 1, 0}, {0, 0, 4}}); };
struct nd { static constexpr auto value = core::from_gram({{1, 1, 0}, {1, -1, 0}, {0, 0, 1}}); };
static_assert(!theorem_b<d114>(1), "diag(1, 1, 4) fails");
static_assert(!theorem_b<nd>(1), "[[1, 1, 0], [1, -1, 0], [0, 0, 1]] fails");

struct rg3 { static constexpr auto value = core::rigid<3>(); };
using P4 = core::mv<rg3, core::blades<rg3, "e1 e2 e3 e4">, int>;
constexpr P4 m1{1, 0, 0, 0}, m2{1, 0, 0, 1}, pt{2, -3, 5, 1};
constexpr auto tr = m1 * m2;  // 1 + e14, a translator of the classical sandwich
static_assert(core::transform(pt, core::bulk_dual(tr)) != core::truncate<P4>(tr * pt * ~tr),
              "rigid(3): bulk dual of a translator is not its sandwich");
static_assert(core::transform(pt, core::bulk_dual(tr)) == pt, "its bulk dual keeps the point");

// Forms the records do not claim and nothing proves: conformal(3), and a ±1
// diagonal form in another order.
struct c3 { static constexpr auto value = core::conformal<3>(); };
struct dm11 { static constexpr auto value = core::from_gram({{-1, 0, 0}, {0, 1, 0}, {0, 0, 1}}); };
static_assert(theorem_b<c3>(1) && theorem_b<c3>(2), "conformal(3) holds here, unproved");
static_assert(theorem_b<dm11>(1) && theorem_b<dm11>(2), "diag(-1, 1, 1), inside Theorem B");

int main() {
  constexpr Vec3 w = core::transform(v, !t);
  std::printf("transform((4, -1, 2), !t) = (%d, %d, %d)\n", core::get<"e1">(w), core::get<"e2">(w),
              core::get<"e3">(w));
}
