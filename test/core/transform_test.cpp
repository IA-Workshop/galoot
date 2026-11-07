// Tests of galoot/core/transform.hpp: the composition of two versors and the
// transform of a multivector by a versor.
//
// compose(Q2, Q1) is the geometric antiproduct Q2 ⟇ Q1, with
// a ⟇ b = left(right(a) ⟑ right(b)). The parity of a versor is that of the
// antigrades n - k of its blades. The transform of x by an even versor V is
// V ⟇ x ⟇ antireverse(V), and by an odd one -V ⟇ x' ⟇ antireverse(V), where
// x' is the antigrade involution of x, which multiplies a blade of
// antigrade m by (-1)^m; the result keeps the blades of x. The antireverse
// multiplies a blade of antigrade m by (-1)^(m(m-1)/2). The expected values
// are composed in the test from the reference's tables of the products and
// the complements, or worked out by hand from those definitions. A result
// that is not of x's type holds the canonical blades the operation reaches,
// each with orientation +1, in order of grade and then of index list; the
// expected layouts are written from that rule.
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <random>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>
#include <galoot/core/transform.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>
#include <support/rational.hpp>

namespace {

namespace core = galoot::core;
using test_support::rational;

struct rga2 {
  static constexpr auto value = core::rigid<2>();
};
struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga2 {
  static constexpr auto value = core::conformal<2>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
// The degenerate form that is not diagonal, and seeded random form 1, of the
// reference's product tables.
struct degenerate3 {
  static constexpr auto value = core::from_gram({
      {0, 1, 0},
      {1, 0, 0},
      {0, 0, 0},
  });
};
struct random1 {
  static constexpr auto value = core::from_gram({
      {2, 0, -3},
      {0, 1, -3},
      {-3, -3, -2},
  });
};

inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto plane_layout = core::blades<rga3, "e423 e431 e412 e321">;
inline constexpr auto motor_layout =
    core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;
inline constexpr auto flector_layout =
    core::blades<rga3, "e1 e2 e3 e4 e423 e431 e412 e321">;

// The blades of even and of odd antigrade, in an algebra of three, four,
// and five generators: n - k is even for the grades k of the same parity as
// n.
template <class Alg>
inline constexpr auto even_blades_of_3 = core::blades<Alg, "e1 e2 e3 e123">;
template <class Alg>
inline constexpr auto odd_blades_of_3 = core::blades<Alg, "1 e12 e13 e23">;
template <class Alg>
inline constexpr auto even_blades_of_4 =
    core::blades<Alg, "1 e12 e13 e14 e23 e24 e34 e1234">;
template <class Alg>
inline constexpr auto odd_blades_of_4 =
    core::blades<Alg, "e1 e2 e3 e4 e123 e124 e134 e234">;

template <class Alg>
inline constexpr auto even_blades_of_5 =
    core::blades<Alg, "e1 e2 e3 e4 e5 e123 e124 e125 e134 e135 e145 e234 "
                      "e235 e245 e345 e12345">;
template <class Alg>
inline constexpr auto odd_blades_of_5 =
    core::blades<Alg, "1 e12 e13 e14 e15 e23 e24 e25 e34 e35 e45 e1234 "
                      "e1235 e1245 e1345 e2345">;

// The number of generators of an algebra.
template <class Alg>
inline constexpr std::size_t generators =
    std::remove_cvref_t<decltype(Alg::value)>::generator_count;

// Coefficients indexed by mask, one place for each blade of an algebra.
template <class T>
using by_mask = std::vector<T>;

std::string text(std::int64_t v) { return std::to_string(v); }

// The name of a blade, e followed by the indices of its generators, or 1,
// with a minus sign for an orientation of -1, so that a failure names the
// blade it is about.
std::string blade_name(core::mask_t mask, int sign = 1) {
  std::string name = sign < 0 ? "-" : "";
  if (mask == 0) {
    return name + "1";
  }
  name += 'e';
  for (unsigned i = 0; i < 32; ++i) {
    if (((mask >> i) & 1U) != 0) {
      name += std::to_string(i + 1);
    }
  }
  return name;
}

// The coefficient of x on the canonical blade of this mask: the stored
// coefficient times the orientation of the blade in the layout, or zero when
// the layout lacks the blade.
template <class X>
typename X::scalar_type value_on(const X& x, core::mask_t mask) {
  using T = typename X::scalar_type;
  for (std::size_t k = 0; k < X::component_count; ++k) {
    const core::oriented_blade blade = X::layout.blades.at(k);
    if (blade.mask == mask) {
      return T{blade.sign} * x.coefficients.at(k);
    }
  }
  return T{};
}

// The coefficients of x indexed by mask.
template <class X>
by_mask<typename X::scalar_type> masks_of(const X& x) {
  by_mask<typename X::scalar_type> out(std::size_t{1}
                                       << generators<typename X::algebra_type>);
  for (core::mask_t m = 0; m < out.size(); ++m) {
    out.at(m) = value_on(x, m);
  }
  return out;
}

// Empty when x holds `want` on every blade of its algebra, where a blade its
// layout lacks holds zero; otherwise the first blade where they differ.
template <class X>
std::string difference_from(const X& x,
                            const by_mask<typename X::scalar_type>& want) {
  for (core::mask_t m = 0; m < want.size(); ++m) {
    if (value_on(x, m) != want.at(m)) {
      return blade_name(m) + ": " + text(value_on(x, m)) + ", not " +
             text(want.at(m));
    }
  }
  return "";
}

// Whether the tables hold, at each place, the entry of the masks of that
// place: complements[m] for the blade m and table[a * size + b] for the
// product of a and b.
bool tables_in_place(std::span<const golden::product> table,
                     std::span<const golden::complement_pair> complements) {
  const std::size_t size = complements.size();
  if (table.size() != size * size) {
    return false;
  }
  for (std::size_t k = 0; k < table.size(); ++k) {
    if (table[k].a != k / size || table[k].b != k % size) {
      return false;
    }
  }
  for (core::mask_t m = 0; m < size; ++m) {
    if (complements[m].blade != m) {
      return false;
    }
  }
  return true;
}

template <class T>
by_mask<T> right_of(std::span<const golden::complement_pair> complements,
                    const by_mask<T>& x) {
  by_mask<T> out(x.size());
  for (core::mask_t a = 0; a < x.size(); ++a) {
    out.at(complements[a].right.blade) =
        out.at(complements[a].right.blade) +
        (T{complements[a].right.sign} * x.at(a));
  }
  return out;
}

template <class T>
by_mask<T> left_of(std::span<const golden::complement_pair> complements,
                   const by_mask<T>& x) {
  by_mask<T> out(x.size());
  for (core::mask_t a = 0; a < x.size(); ++a) {
    out.at(complements[a].left.blade) = out.at(complements[a].left.blade) +
                                        (T{complements[a].left.sign} * x.at(a));
  }
  return out;
}

// The geometric product x y, from the product table.
template <class T>
by_mask<T> product_of(std::span<const golden::product> table,
                      const by_mask<T>& x, const by_mask<T>& y) {
  const std::size_t size = x.size();
  by_mask<T> out(size);
  for (core::mask_t a = 0; a < size; ++a) {
    for (core::mask_t b = 0; b < size; ++b) {
      if (x.at(a) == T{} || y.at(b) == T{}) {
        continue;
      }
      const golden::product& entry = table[(a * size) + b];
      for (std::size_t k = 0; k < entry.size; ++k) {
        const golden::term& t = entry.terms.at(k);
        out.at(t.blade) =
            out.at(t.blade) + (T{t.coefficient} * x.at(a) * y.at(b));
      }
    }
  }
  return out;
}

// The geometric antiproduct x ⟇ y = left(right(x) ⟑ right(y)), from the
// tables.
template <class T>
by_mask<T> antiproduct_of(std::span<const golden::product> table,
                          std::span<const golden::complement_pair> complements,
                          const by_mask<T>& x, const by_mask<T>& y) {
  return left_of(complements, product_of(table, right_of(complements, x),
                                         right_of(complements, y)));
}

// Whether the canonical blade of a comes before that of b in the order of a
// result: by grade, then lexicographically by index list. Two index lists of
// one length first differ at the lowest generator that one has and the other
// lacks, and the list that has it comes first.
bool canonically_before(core::mask_t a, core::mask_t b) {
  if (std::popcount(a) != std::popcount(b)) {
    return std::popcount(a) < std::popcount(b);
  }
  const core::mask_t differ = a ^ b;
  return differ != 0 && (a & (differ & (~differ + 1))) != 0;
}

// Empty when the layout of R is the canonical set of the masks marked in
// `reached`: each with sign +1, in order of grade and then of index list,
// and no other; otherwise what differs.
template <class R>
std::string layout_difference(const std::vector<bool>& reached) {
  std::vector<core::mask_t> expected;
  for (core::mask_t m = 0; m < reached.size(); ++m) {
    if (reached.at(m)) {
      expected.push_back(m);
    }
  }
  for (std::size_t i = 1; i < expected.size(); ++i) {
    for (std::size_t j = i;
         j > 0 && canonically_before(expected.at(j), expected.at(j - 1)); --j) {
      std::swap(expected.at(j), expected.at(j - 1));
    }
  }
  std::string got;
  for (const core::oriented_blade& blade : R::layout.blades) {
    got += " " + blade_name(blade.mask, blade.sign);
  }
  std::string want;
  for (const core::mask_t m : expected) {
    want += " " + blade_name(m);
  }
  return got == want ? "" : "layout" + got + ", not" + want;
}

// A random multivector of the layout of X: integers from -9 to 9, or for
// the test rational, fractions p/q with p from -9 to 9 and q from 1 to 4.
template <class X>
X random_value(std::mt19937_64& source) {
  using T = typename X::scalar_type;
  std::uniform_int_distribution<std::int64_t> digit(-9, 9);
  std::uniform_int_distribution<std::int64_t> denominator(1, 4);
  X x{};
  for (auto& c : x.coefficients) {
    if constexpr (std::same_as<T, rational>) {
      c = rational{digit(source), denominator(source)};
    } else {
      c = T{digit(source)};
    }
  }
  return x;
}

// compose(q2, q1) against q2 ⟇ q1 composed from the tables, for random
// integer q2 of the layout of Q2 and q1 of the layout of Q1, and the layout
// of the result against the canonical set of the blades that the tables'
// antiproduct reaches from a pair of basis blades of the two layouts.
template <class Q2, class Q1>
void check_compositions(std::mt19937_64& source,
                        std::span<const golden::product> table,
                        std::span<const golden::complement_pair> complements,
                        const std::string& form) {
  INFO("form: ", form);
  REQUIRE(tables_in_place(table, complements));
  const std::size_t size = complements.size();
  for (int k = 0; k < 4; ++k) {
    CAPTURE(k);
    const Q2 q2 = random_value<Q2>(source);
    const Q1 q1 = random_value<Q1>(source);
    CHECK(difference_from(core::compose(q2, q1),
                          antiproduct_of(table, complements, masks_of(q2),
                                         masks_of(q1))) == "");
  }
  std::vector<bool> reached(size);
  for (std::size_t i = 0; i < Q2::component_count; ++i) {
    for (std::size_t j = 0; j < Q1::component_count; ++j) {
      Q2 a{};
      Q1 b{};
      a.coefficients.at(i) = 1;
      b.coefficients.at(j) = 1;
      const by_mask<std::int64_t> p =
          antiproduct_of(table, complements, masks_of(a), masks_of(b));
      for (core::mask_t m = 0; m < size; ++m) {
        reached.at(m) = reached.at(m) || p.at(m) != 0;
      }
    }
  }
  using R = decltype(core::compose(Q2{}, Q1{}));
  CHECK(layout_difference<R>(reached) == "");
}

// check_compositions for the four pairs of the even and odd layouts E and O
// of the algebra Alg.
template <class Alg, core::blade_set E, core::blade_set O>
void check_parity_pairs(std::mt19937_64& source,
                        std::span<const golden::product> table,
                        std::span<const golden::complement_pair> complements,
                        const std::string& form) {
  using Even = core::mv<Alg, E, std::int64_t>;
  using Odd = core::mv<Alg, O, std::int64_t>;
  check_compositions<Even, Even>(source, table, complements, form);
  check_compositions<Even, Odd>(source, table, complements, form);
  check_compositions<Odd, Even>(source, table, complements, form);
  check_compositions<Odd, Odd>(source, table, complements, form);
}

template <class X, class Y>
concept can_call_compose = requires(X x, Y y) { core::compose(x, y); };
template <class Alg>
concept has_compose_layout =
    requires { core::compose_layout<Alg, core::full<2>, core::full<2>>; };

} // namespace

// Reference: independent value: for random integer versors of the even and
//   the odd blades of rigid(3), conformal(2), conformal(3), the degenerate
//   form [[0, 1, 0], [1, 0, 0], [0, 0, 0]], and random form 1, and of the
//   rigid(3) motor and flector under their oriented names, compose(Q2, Q1)
//   is the geometric antiproduct Q2 ⟇ Q1 = left(right(Q2) ⟑ right(Q1)),
//   composed in the test from the reference's tables of the products of
//   forms whose Gram matrices satisfy e_i e_j + e_j e_i = 2 g_ij and of the
//   complements, which satisfy u ^ right(u) = antiscalar and
//   left(u) ^ u = antiscalar, on the canonical set of the blades it reaches,
//   each with sign +1, by grade and then index list
TEST_CASE("compose is the geometric antiproduct of the tables") {
  REQUIRE(rga3::value.gram == golden::gp_tables::rigid_3_gram);
  REQUIRE(cga2::value.gram == golden::gp_tables::conformal_2_gram);
  REQUIRE(cga3::value.gram == golden::gp_tables::conformal_3_gram);
  REQUIRE(degenerate3::value.gram == golden::gp_tables::degenerate_3_gram);
  REQUIRE(random1::value.gram == golden::gp_tables::random_1_gram);
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  check_parity_pairs<rga3, even_blades_of_4<rga3>, odd_blades_of_4<rga3>>(
      source, golden::gp_tables::rigid_3, golden::complements::n4, "rigid(3)");
  check_parity_pairs<rga3, motor_layout, flector_layout>(
      source, golden::gp_tables::rigid_3, golden::complements::n4,
      "rigid(3), oriented names");
  check_parity_pairs<cga2, even_blades_of_4<cga2>, odd_blades_of_4<cga2>>(
      source, golden::gp_tables::conformal_2, golden::complements::n4,
      "conformal(2)");
  check_parity_pairs<cga3, even_blades_of_5<cga3>, odd_blades_of_5<cga3>>(
      source, golden::gp_tables::conformal_3, golden::complements::n5,
      "conformal(3)");
  check_parity_pairs<degenerate3, even_blades_of_3<degenerate3>,
                     odd_blades_of_3<degenerate3>>(
      source, golden::gp_tables::degenerate_3, golden::complements::n3,
      "degenerate form");
  check_parity_pairs<random1, even_blades_of_3<random1>,
                     odd_blades_of_3<random1>>(
      source, golden::gp_tables::random_1, golden::complements::n3,
      "random form 1");
}

// Reference: independent value: by hand from the definitions in rigid(3),
//   the antiproduct of two even layouts reaches the even blades and that of
//   an even and an odd layout the odd blades, since antigrades add modulo 2
//   under ⟇, and so compose of two motors holds 1, the six bivectors, and
//   e1234, and of a motor and a flector the four vectors and the four
//   trivectors, each with sign +1 in order of grade and then of index list,
//   which are not the motor's or the flector's oriented layouts, and the layout
//   name is that of the geometric antiproduct and takes only an algebra
TEST_CASE("compose_layout is the canonical set of the antiproduct") {
  using Motor = core::mv<rga3, motor_layout, std::int64_t>;
  using Flector = core::mv<rga3, flector_layout, std::int64_t>;
  static_assert(core::compose_layout<rga3, motor_layout, motor_layout> ==
                core::blades<rga3, "1 e12 e13 e14 e23 e24 e34 e1234">);
  static_assert(core::compose_layout<rga3, motor_layout, flector_layout> ==
                core::blades<rga3, "e1 e2 e3 e4 e123 e124 e134 e234">);
  static_assert(core::compose_layout<rga3, flector_layout, flector_layout> ==
                core::blades<rga3, "1 e12 e13 e14 e23 e24 e34 e1234">);
  static_assert(core::compose_layout<rga3, motor_layout, motor_layout> !=
                motor_layout);
  static_assert(
      core::compose_layout<rga3, line_layout, point_layout> ==
      core::geometric_antiproduct_layout<rga3, line_layout, point_layout>);
  static_assert(has_compose_layout<rga3>);
  static_assert(!has_compose_layout<int>);
  static_assert(
      std::same_as<
          decltype(core::compose(Motor{}, Flector{})),
          core::mv<rga3,
                   core::compose_layout<rga3, motor_layout, flector_layout>,
                   std::int64_t>>);
  static_assert(
      !std::same_as<decltype(core::compose(Motor{}, Motor{})), Motor>);
  CHECK(core::compose_layout<rga3, motor_layout, motor_layout>.size() == 8);
}

// Reference: independent value: by hand, the parity of a versor is the
//   parity of the antigrades of its blades: even when every blade has an
//   even antigrade n - k, odd when every one has an odd antigrade, and mixed
//   otherwise, so that an empty layout is even. In rigid(2), with n = 3, the
//   vectors and e123 are even and the scalar and the bivectors odd, the
//   opposite of the parity of their grades. In rigid(3), with n = 4, the two
//   parities agree, and in conformal(3), with n = 5, the vectors are even
TEST_CASE("the parity of a versor is the parity of the antigrades of its "
          "blades") {
  using core::detail::antigrade_parity;
  using core::detail::Parity;

  static_assert(antigrade_parity<rga2, even_blades_of_3<rga2>> == Parity::even);
  static_assert(antigrade_parity<rga2, core::blades<rga2, "e1">> ==
                Parity::even);
  static_assert(antigrade_parity<rga2, odd_blades_of_3<rga2>> == Parity::odd);
  static_assert(antigrade_parity<rga2, core::blades<rga2, "1 e12">> ==
                Parity::odd);
  static_assert(antigrade_parity<rga2, core::blades<rga2, "1 e1">> ==
                Parity::mixed);
  static_assert(antigrade_parity<rga2, core::blades<rga2, "e1 e12">> !=
                Parity::even);
  static_assert(antigrade_parity<rga2, core::blades<rga2, "e1 e12">> !=
                Parity::odd);
  static_assert(antigrade_parity<rga2, core::blade_set<0>{}> == Parity::even);

  static_assert(antigrade_parity<rga3, even_blades_of_4<rga3>> == Parity::even);
  static_assert(antigrade_parity<rga3, motor_layout> == Parity::even);
  static_assert(antigrade_parity<rga3, flector_layout> == Parity::odd);
  static_assert(antigrade_parity<rga3, plane_layout> == Parity::odd);
  static_assert(antigrade_parity<rga3, core::blades<rga3, "1 e1">> ==
                Parity::mixed);
  static_assert(antigrade_parity<rga3, core::blades<rga3, "e1 e1234">> !=
                Parity::odd);
  static_assert(antigrade_parity<rga3, core::full<4>> == Parity::mixed);
  static_assert(antigrade_parity<rga3, core::blade_set<0>{}> == Parity::even);

  static_assert(antigrade_parity<cga3, even_blades_of_5<cga3>> == Parity::even);
  static_assert(antigrade_parity<cga3, core::of_grade<5, 1>> == Parity::even);
  static_assert(antigrade_parity<cga3, core::of_grade<5, 3>> == Parity::even);
  static_assert(antigrade_parity<cga3, odd_blades_of_5<cga3>> == Parity::odd);
  static_assert(antigrade_parity<cga3, core::of_grade<5, 4>> == Parity::odd);
  static_assert(antigrade_parity<cga3, core::blades<cga3, "e1 e12">> ==
                Parity::mixed);
  static_assert(antigrade_parity<cga3, core::blades<cga3, "1 e12345">> ==
                Parity::mixed);
  static_assert(antigrade_parity<cga3, core::blade_set<0>{}> == Parity::even);
  CHECK(antigrade_parity<cga3, core::blades<cga3, "e1 e12">> == Parity::mixed);
}

// Reference: contract: a composition of operands in two algebras or with
//   two scalar types has the catch-all as its only candidate, and a
//   requires-expression answers yes for it, as the library documents, while
//   a valid call is not captured by it, since the composition of two motors
//   is on the canonical even blades and not of the motor's type, and a
//   multivector and a number have no composition
TEST_CASE("the compose overloads take the right operands") {
  using M = core::mv<rga3, motor_layout, double>;
  using F = core::mv<rga3, motor_layout, float>;
  using C = core::mv<cga3, core::of_grade<5, 1>, double>;
  static_assert(can_call_compose<M, M>);
  static_assert(can_call_compose<M, F>);
  static_assert(can_call_compose<M, C>);
  static_assert(!can_call_compose<M, double>);
  static_assert(!can_call_compose<double, M>);
  static_assert(!std::same_as<decltype(core::compose(M{}, M{})), M>);
  static_assert(
      std::same_as<
          decltype(core::compose(M{}, M{})),
          core::mv<rga3, core::blades<rga3, "1 e12 e13 e14 e23 e24 e34 e1234">,
                   double>>);
}

// A scope that sees galoot::core's names by a using-directive, as a user's
// code may, with types of its own: one with no compose or transform and one
// with its own. The library's catch-alls must leave both alone.
namespace {
namespace user_code {

using namespace galoot::core;

struct widget {};

struct gadget {
  friend int compose(gadget /*a*/, gadget /*b*/) { return 1; }
};

template <class X, class Y>
concept has_compose = requires(X x, Y y) { compose(x, y); };

// Reference: contract: with galoot::core's names in scope by a
//   using-directive, a type of the user's without a compose still has none,
//   a type with its own keeps it, a multivector with such a type has none,
//   and two multivectors find the library's by an unqualified call
TEST_CASE("compose leaves other types alone") {
  using R = mv<rga3, motor_layout, double>;
  static_assert(!has_compose<widget, widget>);
  static_assert(!has_compose<R, gadget>);
  static_assert(!has_compose<gadget, R>);
  static_assert(std::same_as<decltype(compose(gadget{}, gadget{})), int>);
  CHECK(compose(gadget{}, gadget{}) == 1);
  static_assert(has_compose<R, R>);
}

} // namespace user_code
} // namespace
