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
#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <random>
#include <span>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
// The negation of a multivector, found by argument-dependent lookup, which
// the include check does not see.
#include <galoot/core/linear.hpp> // IWYU pragma: keep
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>
#include <galoot/core/transform.hpp>
#include <galoot/core/unary.hpp>

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
std::string text(const rational& v) {
  return v.denominator == 1 ? std::to_string(v.numerator)
                            : std::to_string(v.numerator) + "/" +
                                  std::to_string(v.denominator);
}

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
X::scalar_type value_on(const X& x, core::mask_t mask) {
  using T = X::scalar_type;
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
  using T = X::scalar_type;
  std::uniform_int_distribution<std::int64_t> digit(-9, 9);
  X x{};
  for (auto& c : x.coefficients) {
    if constexpr (std::same_as<T, rational>) {
      std::uniform_int_distribution<std::int64_t> denominator(1, 4);
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

namespace {

struct r300 {
  static constexpr auto value = core::R<3, 0, 0>();
};
// Three generators of squares 16, 16, and 1.
struct wide3 {
  static constexpr auto value = core::from_gram({
      {16, 0, 0},
      {0, 16, 0},
      {0, 0, 1},
  });
};

using Point = core::mv<rga3, point_layout, rational>;
using Line = core::mv<rga3, line_layout, rational>;
using Plane = core::mv<rga3, plane_layout, rational>;
using Motor = core::mv<rga3, motor_layout, rational>;
using Flector = core::mv<rga3, flector_layout, rational>;

// x with each blade of antigrade m multiplied by (-1)^m: the antigrade
// involution, from its definition.
template <class X>
X antigrade_involution_of(X x) {
  using T = X::scalar_type;
  for (std::size_t k = 0; k < X::component_count; ++k) {
    const auto antigrade =
        generators<typename X::algebra_type> -
        static_cast<std::size_t>(std::popcount(X::layout.blades.at(k).mask));
    if (antigrade % 2 != 0) {
      x.coefficients.at(k) = T{} - x.coefficients.at(k);
    }
  }
  return x;
}

// x with each coefficient multiplied by the sign of its blade in `which`, a
// column of the reference's table of the grade operations.
template <class T>
by_mask<T> signed_by(std::span<const golden::blade_signs> signs,
                     int golden::blade_signs::* which, const by_mask<T>& x) {
  by_mask<T> out(x.size());
  for (core::mask_t m = 0; m < x.size(); ++m) {
    out.at(m) = T{signs[m].*which} * x.at(m);
  }
  return out;
}

// The transform of x by v, composed from the tables: v ⟇ x ⟇ antireverse(v)
// for an even v, and -v ⟇ x' ⟇ antireverse(v) for an odd one, with x' the
// antigrade involution, on every blade the sandwich reaches.
template <class T>
by_mask<T> sandwich_of(std::span<const golden::product> table,
                       std::span<const golden::complement_pair> complements,
                       std::span<const golden::blade_signs> signs, bool odd,
                       const by_mask<T>& v, by_mask<T> x) {
  const auto n = static_cast<std::size_t>(std::countr_zero(x.size()));
  if (odd) {
    for (core::mask_t m = 0; m < x.size(); ++m) {
      const std::size_t antigrade =
          n - static_cast<std::size_t>(std::popcount(m));
      x.at(m) = antigrade % 2 == 0 ? x.at(m) : T{} - x.at(m);
    }
  }
  by_mask<T> out = antiproduct_of(
      table, complements, antiproduct_of(table, complements, v, x),
      signed_by(signs, &golden::blade_signs::antireverse, v));
  if (odd) {
    for (T& c : out) {
      c = T{} - c;
    }
  }
  return out;
}

// transform(x, v) against the sandwich composed from the tables, for random
// rational v of the layout of V and x of the layout of X, whose sandwich has
// no part outside the blades of x, as every one-parity v keeps the
// antigrades of x modulo 4; and the result's type is X.
template <class V, class X>
void check_sandwiches(std::mt19937_64& source,
                      std::span<const golden::product> table,
                      std::span<const golden::complement_pair> complements,
                      std::span<const golden::blade_signs> signs, bool odd,
                      const std::string& form) {
  INFO("form: ", form);
  INFO("odd: ", odd);
  INFO("x: ", X::component_count, " blades");
  static_assert(std::same_as<decltype(core::transform(X{}, V{})), X>);
  for (int k = 0; k < 2; ++k) {
    CAPTURE(k);
    const V v = random_value<V>(source);
    const X x = random_value<X>(source);
    const by_mask<rational> whole =
        sandwich_of(table, complements, signs, odd, masks_of(v), masks_of(x));
    by_mask<rational> kept(whole.size());
    for (const core::oriented_blade& blade : X::layout.blades) {
      kept.at(blade.mask) = whole.at(blade.mask);
    }
    CHECK(whole == kept);
    CHECK(difference_from(core::transform(x, v), kept) == "");
  }
}

// check_sandwiches for the even and the odd layouts E and O of the algebra
// Alg, on each of the layouts Xs.
template <class Alg, core::blade_set E, core::blade_set O,
          core::blade_set... Xs>
void check_form(std::mt19937_64& source, std::span<const golden::product> table,
                std::span<const golden::complement_pair> complements,
                std::span<const golden::blade_signs> signs,
                const std::string& form) {
  REQUIRE(tables_in_place(table, complements));
  REQUIRE(signs.size() == complements.size());
  for (core::mask_t m = 0; m < signs.size(); ++m) {
    REQUIRE(signs[m].blade == m);
  }
  (check_sandwiches<core::mv<Alg, E, rational>, core::mv<Alg, Xs, rational>>(
       source, table, complements, signs, false, form),
   ...);
  (check_sandwiches<core::mv<Alg, O, rational>, core::mv<Alg, Xs, rational>>(
       source, table, complements, signs, true, form),
   ...);
}

template <class X, class Y>
concept can_call_transform = requires(X x, Y y) { core::transform(x, y); };

} // namespace

// Reference: independent value: for random rational versors of the even and
//   the odd blades and random rational x of rigid(3), conformal(2),
//   conformal(3), the degenerate form [[0, 1, 0], [1, 0, 0], [0, 0, 0]], and
//   random form 1, on every blade and on the blades of each class of
//   antigrades modulo 4, and for the rigid(3) point, line, plane, motor, and
//   flector under their oriented names by the oriented motor and flector,
//   transform(x, V) is V ⟇ x ⟇ antireverse(V) for an even V and
//   -V ⟇ x' ⟇ antireverse(V) for an odd one, with x' the antigrade involution,
//   composed in the test from the reference's tables of the products of
//   forms whose Gram matrices satisfy e_i e_j + e_j e_i = 2 g_ij, of the
//   complements, which satisfy u ^ right(u) = antiscalar and
//   left(u) ^ u = antiscalar, and of the grade signs, where the reverse of a
//   grade-k blade is (-1)^(k(k-1)/2) times it and the antireverse uses the
//   antigrade, and the sandwich has no part outside the blades of x, and
//   transform(x, V) has the blade set of x
TEST_CASE("transform is the sandwich of the tables") {
  REQUIRE(rga3::value.gram == golden::gp_tables::rigid_3_gram);
  REQUIRE(cga2::value.gram == golden::gp_tables::conformal_2_gram);
  REQUIRE(cga3::value.gram == golden::gp_tables::conformal_3_gram);
  REQUIRE(degenerate3::value.gram == golden::gp_tables::degenerate_3_gram);
  REQUIRE(random1::value.gram == golden::gp_tables::random_1_gram);
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  check_form<rga3, even_blades_of_4<rga3>, odd_blades_of_4<rga3>, core::full<4>,
             core::blades<rga3, "1 e1234">, core::of_grade<4, 1>,
             core::of_grade<4, 2>, core::of_grade<4, 3>>(
      source, golden::gp_tables::rigid_3, golden::complements::n4,
      golden::grade_signs::n4, "rigid(3)");
  check_form<rga3, motor_layout, flector_layout, point_layout, line_layout,
             plane_layout, motor_layout, flector_layout>(
      source, golden::gp_tables::rigid_3, golden::complements::n4,
      golden::grade_signs::n4, "rigid(3), oriented names");
  check_form<cga2, even_blades_of_4<cga2>, odd_blades_of_4<cga2>, core::full<4>,
             core::blades<cga2, "1 e1234">, core::of_grade<4, 1>,
             core::of_grade<4, 2>, core::of_grade<4, 3>>(
      source, golden::gp_tables::conformal_2, golden::complements::n4,
      golden::grade_signs::n4, "conformal(2)");
  check_form<cga3, even_blades_of_5<cga3>, odd_blades_of_5<cga3>, core::full<5>,
             core::blades<cga3, "e1 e2 e3 e4 e5 e12345">,
             core::blades<cga3, "1 e1234 e1235 e1245 e1345 e2345">,
             core::of_grade<5, 2>, core::of_grade<5, 3>>(
      source, golden::gp_tables::conformal_3, golden::complements::n5,
      golden::grade_signs::n5, "conformal(3)");
  check_form<degenerate3, even_blades_of_3<degenerate3>,
             odd_blades_of_3<degenerate3>, core::full<3>, core::of_grade<3, 0>,
             core::of_grade<3, 1>, core::of_grade<3, 2>, core::of_grade<3, 3>>(
      source, golden::gp_tables::degenerate_3, golden::complements::n3,
      golden::grade_signs::n3, "degenerate form");
  check_form<random1, even_blades_of_3<random1>, odd_blades_of_3<random1>,
             core::full<3>, core::of_grade<3, 0>, core::of_grade<3, 1>,
             core::of_grade<3, 2>, core::of_grade<3, 3>>(
      source, golden::gp_tables::random_1, golden::complements::n3,
      golden::grade_signs::n3, "random form 1");
}

// Reference: independent value: by hand in rigid(3), the rotor
//   l sin(phi) + antiscalar cos(phi) about the z axis l = e43, with
//   sin(phi) = 3/5 and cos(phi) = 4/5, turns by 2 phi counterclockwise seen
//   from the tip of +z, so (1, 0, 0) goes to (cos 2phi, sin 2phi, 0) =
//   (7/25, 24/25, 0) with weight 1, exactly, and the rotor 3 e43 + 4
//   antiscalar, 25 times its square, gives 25 times that, (7, 24, 0, 25), over
//   std::int64_t, and and the rotor composed with itself turns by 4 phi, to
//   (cos 4phi, sin 4phi) = (-527/625, 336/625), and 625 times that for the
//   integer rotor
TEST_CASE("a rotor turns a point by twice its angle") {
  const Motor rotor{rational{0}, rational{0}, rational{3, 5}, rational{4, 5},
                    rational{0}, rational{0}, rational{0},    rational{0}};
  const Point p{rational{1}, rational{0}, rational{0}, rational{1}};
  CHECK(core::transform(p, rotor) ==
        Point{rational{7, 25}, rational{24, 25}, rational{0}, rational{1}});
  CHECK(
      core::transform(p, core::compose(rotor, rotor)) ==
      Point{rational{-527, 625}, rational{336, 625}, rational{0}, rational{1}});

  using IntPoint = core::mv<rga3, point_layout, std::int64_t>;
  using IntMotor = core::mv<rga3, motor_layout, std::int64_t>;
  const IntMotor scaled{0, 0, 3, 4, 0, 0, 0, 0};
  CHECK(core::transform(IntPoint{1, 0, 0, 1}, scaled) ==
        IntPoint{7, 24, 0, 25});
  CHECK(core::transform(IntPoint{1, 0, 0, 1}, core::compose(scaled, scaled)) ==
        IntPoint{-527, 336, 0, 625});
}

// Reference: independent value: by hand in rigid(3), the translator
//   t_x e23 + t_y e31 + t_z e12 + antiscalar moves points by 2t: the point
//   (x, y, z) with weight w goes to (x + 2 t_x w, y + 2 t_y w, z + 2 t_z w)
//   with weight w, so e23 + 2 e31 - e12 + antiscalar moves (5, 7, -1, 1) to
//   (7, 11, -3, 1), and likewise for rational t and points of weights 1 and 3
TEST_CASE("the translator t_x e23 + t_y e31 + t_z e12 + antiscalar moves "
          "points by 2t") {
  using IntPoint = core::mv<rga3, point_layout, std::int64_t>;
  using IntMotor = core::mv<rga3, motor_layout, std::int64_t>;
  CHECK(core::transform(IntPoint{5, 7, -1, 1},
                        IntMotor{0, 0, 0, 1, 1, 2, -1, 0}) ==
        IntPoint{7, 11, -3, 1});

  const std::array moves{
      std::array{rational{1, 2}, rational{-3, 4}, rational{2}},
      std::array{rational{0}, rational{5, 3}, rational{-1, 6}},
  };
  const std::array points{
      std::array{rational{5}, rational{7}, rational{-1}, rational{1}},
      std::array{rational{1, 3}, rational{0}, rational{9, 2}, rational{3}},
  };
  for (std::size_t i = 0; i < moves.size(); ++i) {
    for (std::size_t j = 0; j < points.size(); ++j) {
      CAPTURE(i);
      CAPTURE(j);
      const auto& t = moves.at(i);
      const auto& x = points.at(j);
      const Motor translator{rational{0}, rational{0}, rational{0},
                             rational{1}, t.at(0),     t.at(1),
                             t.at(2),     rational{0}};
      const rational two{2};
      const Point moved{x.at(0) + (two * t.at(0) * x.at(3)),
                        x.at(1) + (two * t.at(1) * x.at(3)),
                        x.at(2) + (two * t.at(2) * x.at(3)), x.at(3)};
      CHECK(core::transform(Point{x.at(0), x.at(1), x.at(2), x.at(3)},
                            translator) == moved);
    }
  }
}

// Reference: independent value: by hand in rigid(3), a reflected point
//   keeps a positive weight: the mirror e412, the plane z = 0, sends
//   (1, 2, 3, 1) to (1, 2, -3, 1), where the single-sign form
//   -F ⟇ x ⟇ antireverse(F), without the involution, gives the weight -1,
//   the mirror with the unit normal n = (3/5, 4/5, 0) and offset -2 sends the
//   unitized p to p - 2 (n . p - 2) n with weight 1, and and the flector of the
//   point c = (1, 2, 3) with weight 1 sends p to 2c - p with weight 1, so
//   (5, 7, -1) goes to (-3, -3, 7)
TEST_CASE("a reflected point keeps a positive weight") {
  const Flector mirror{rational{0}, rational{0}, rational{0}, rational{0},
                       rational{0}, rational{0}, rational{1}, rational{0}};
  const Point p{rational{1}, rational{2}, rational{3}, rational{1}};
  CHECK(core::transform(p, mirror) ==
        Point{rational{1}, rational{2}, rational{-3}, rational{1}});
  const Point single_sign = core::part<Point>(-core::geometric_antiproduct(
      core::geometric_antiproduct(mirror, p), core::antireverse(mirror)));
  CHECK(core::get<"e4">(single_sign) == rational{-1});

  const std::array normal{rational{3, 5}, rational{4, 5}, rational{0}};
  const Flector tilted{rational{0},  rational{0},  rational{0},  rational{0},
                       normal.at(0), normal.at(1), normal.at(2), rational{-2}};
  const std::array points{
      std::array{rational{5}, rational{7}, rational{-1}},
      std::array{rational{-1, 2}, rational{1, 3}, rational{4}},
  };
  const Flector at_c{rational{1}, rational{2}, rational{3}, rational{1},
                     rational{0}, rational{0}, rational{0}, rational{0}};
  for (std::size_t j = 0; j < points.size(); ++j) {
    CAPTURE(j);
    const auto& x = points.at(j);
    const rational distance = (normal.at(0) * x.at(0)) +
                              (normal.at(1) * x.at(1)) +
                              (normal.at(2) * x.at(2)) - rational{2};
    const rational two{2};
    const Point q{x.at(0), x.at(1), x.at(2), rational{1}};
    CHECK(core::transform(q, tilted) ==
          Point{x.at(0) - (two * distance * normal.at(0)),
                x.at(1) - (two * distance * normal.at(1)),
                x.at(2) - (two * distance * normal.at(2)), rational{1}});
    CHECK(core::transform(q, at_c) ==
          Point{rational{2} - x.at(0), rational{4} - x.at(1),
                rational{6} - x.at(2), rational{1}});
  }
  CHECK(core::transform(
            Point{rational{5}, rational{7}, rational{-1}, rational{1}}, at_c) ==
        Point{rational{-3}, rational{-3}, rational{7}, rational{1}});
}

// Reference: identity: an odd versor V transforms x as the negated
//   antiproduct of V, the antigrade involution of x, and antireverse(V),
//   -V ⟇ x' ⟇ antireverse(V), with x' multiplying a blade of antigrade m by
//   (-1)^m, and an even one as V ⟇ x ⟇ antireverse(V), and checked through the
//   public products for random rational lines, planes, and motors under a
//   rigid(3) flector and motor, and in rigid(2) and R(3,0,0), where n is
//   odd and x' is the negated grade involution
TEST_CASE("an odd versor V transforms x as the negated antiproduct of V, the "
          "antigrade involution of x, and antireverse(V)") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  const auto check = [&]<class V, class X>(std::type_identity<V>,
                                           std::type_identity<X>, bool odd) {
    for (int k = 0; k < 3; ++k) {
      CAPTURE(k);
      const V v = random_value<V>(source);
      const X x = random_value<X>(source);
      const auto whole = core::geometric_antiproduct(
          core::geometric_antiproduct(v, odd ? antigrade_involution_of(x) : x),
          core::antireverse(v));
      CHECK(core::transform(x, v) ==
            core::part<X>(odd ? decltype(whole){-whole} : whole));
    }
  };
  check(std::type_identity<Flector>{}, std::type_identity<Line>{}, true);
  check(std::type_identity<Flector>{}, std::type_identity<Plane>{}, true);
  check(std::type_identity<Flector>{}, std::type_identity<Motor>{}, true);
  check(std::type_identity<Motor>{}, std::type_identity<Line>{}, false);
  check(std::type_identity<Motor>{}, std::type_identity<Flector>{}, false);
  using Mirror2 = core::mv<rga2, core::of_grade<3, 2>, rational>;
  using Rotor2 = core::mv<rga2, core::blades<rga2, "e1 e2 e3 e123">, rational>;
  using Point2 = core::mv<rga2, core::of_grade<3, 1>, rational>;
  using Line2 = core::mv<rga2, core::of_grade<3, 2>, rational>;
  check(std::type_identity<Mirror2>{}, std::type_identity<Point2>{}, true);
  check(std::type_identity<Mirror2>{}, std::type_identity<Line2>{}, true);
  check(std::type_identity<Rotor2>{}, std::type_identity<Point2>{}, false);
  using Mirror3 = core::mv<r300, core::of_grade<3, 2>, rational>;
  using Vector3 = core::mv<r300, core::of_grade<3, 1>, rational>;
  check(std::type_identity<Mirror3>{}, std::type_identity<Vector3>{}, true);
}

// Reference: independent value: by hand in rigid(3), a motor's or a
//   flector's transform equals the outermorphism of its action on points, so
//   lines and planes move as the points that span them, and a reflected line
//   or plane is the one spanned by the reflected points in the same order:
//   T(p ^ q) = T(p) ^ T(q) and T(p ^ q ^ r) = T(p) ^ T(q) ^ T(r), for the
//   rotor of 3/5 and 4/5 about z followed by a translator, the mirror with
//   the unit normal (3/5, 4/5, 0) and offset -2, and the flector of the point
//   (1, 2, 3), each of which has V ⟇ antireverse(V) = antiscalar, and for the
//   integer rotor 3 e43 + 4 antiscalar, whose V ⟇ antireverse(V) is 25
//   antiscalar, 25 T(p ^ q) = T(p) ^ T(q)
TEST_CASE("lines and planes move as the points that span them") {
  const Motor rotor{rational{0}, rational{0}, rational{3, 5}, rational{4, 5},
                    rational{0}, rational{0}, rational{0},    rational{0}};
  const Motor translator{rational{0},    rational{0},    rational{0},
                         rational{1},    rational{1, 2}, rational{-3},
                         rational{2, 3}, rational{0}};
  const Motor motor{core::compose(translator, rotor)};
  const Flector mirror{rational{0}, rational{0},    rational{0},
                       rational{0}, rational{3, 5}, rational{4, 5},
                       rational{0}, rational{-2}};
  const Flector at_c{rational{1}, rational{2}, rational{3}, rational{1},
                     rational{0}, rational{0}, rational{0}, rational{0}};
  const Point p{rational{1}, rational{2}, rational{3}, rational{1}};
  const Point q{rational{-2}, rational{1, 2}, rational{0}, rational{1}};
  const Point r{rational{4}, rational{-1}, rational{5, 3}, rational{1}};
  const Line l{core::wedge(p, q)};
  const Plane g{core::wedge(core::wedge(p, q), r)};
  const auto check = [&](const auto& v) {
    const Point tp = core::transform(p, v);
    const Point tq = core::transform(q, v);
    const Point tr = core::transform(r, v);
    CHECK(core::transform(l, v) == Line{core::wedge(tp, tq)});
    CHECK(core::transform(g, v) == Plane{core::wedge(core::wedge(tp, tq), tr)});
  };
  check(motor);
  check(mirror);
  check(at_c);

  using IntPoint = core::mv<rga3, point_layout, std::int64_t>;
  using IntLine = core::mv<rga3, line_layout, std::int64_t>;
  using IntMotor = core::mv<rga3, motor_layout, std::int64_t>;
  const IntMotor scaled{0, 0, 3, 4, 0, 0, 0, 0};
  const IntPoint a{1, 2, 3, 1};
  const IntPoint b{-2, 5, 0, 1};
  const IntLine moved = core::transform(IntLine{core::wedge(a, b)}, scaled);
  CHECK(IntLine{moved * std::int64_t{25}} ==
        IntLine{core::wedge(core::transform(a, scaled),
                            core::transform(b, scaled))});
}

// Reference: independent value: by hand in rigid(3), compose(Q2, Q1)
//   applies Q1 first, and applying Q1 and then Q2 is one transform by
//   geometric_antiproduct(Q2, Q1): with Q1 the rotor of 3/5 and 4/5 about z
//   and Q2 the translator by (2, 4, -2), (1, 0, 0) turns to (7/25, 24/25, 0)
//   and moves to (57/25, 124/25, -2), while translating first and then
//   turning gives (3, 4, -2) turned, (-3, 4, -2), so the two do not commute,
//   and the mirror z = 0 after that translator sends (5, 7, -1) to (7, 11, 3)
TEST_CASE("compose(Q2, Q1) applies Q1 first") {
  const Motor q1{rational{0}, rational{0}, rational{3, 5}, rational{4, 5},
                 rational{0}, rational{0}, rational{0},    rational{0}};
  const Motor q2{rational{0}, rational{0}, rational{0},  rational{1},
                 rational{1}, rational{2}, rational{-1}, rational{0}};
  const Point x{rational{1}, rational{0}, rational{0}, rational{1}};
  const Point first_q1{rational{57, 25}, rational{124, 25}, rational{-2},
                       rational{1}};
  const Point first_q2{rational{-3}, rational{4}, rational{-2}, rational{1}};
  CHECK(core::transform(core::transform(x, q1), q2) == first_q1);
  CHECK(core::transform(core::transform(x, q2), q1) == first_q2);
  CHECK(core::transform(x, core::compose(q2, q1)) == first_q1);
  CHECK(core::transform(x, core::geometric_antiproduct(q2, q1)) == first_q1);

  const Line l{core::wedge(
      x, Point{rational{0}, rational{1}, rational{2}, rational{1}})};
  CHECK(core::transform(l, core::compose(q2, q1)) ==
        core::transform(core::transform(l, q1), q2));
  CHECK(core::transform(l, core::compose(q2, q1)) !=
        core::transform(core::transform(l, q2), q1));

  const Flector mirror{rational{0}, rational{0}, rational{0}, rational{0},
                       rational{0}, rational{0}, rational{1}, rational{0}};
  const Point y{rational{5}, rational{7}, rational{-1}, rational{1}};
  CHECK(core::transform(y, core::compose(mirror, q2)) ==
        Point{rational{7}, rational{11}, rational{3}, rational{1}});
  CHECK(core::transform(y, core::compose(mirror, q2)) ==
        core::transform(core::transform(y, q2), mirror));
}

// Reference: independent value: by hand in rigid(2), where n = 3 is odd, an
//   odd V gives V ⟇ x^ ⟇ antireverse(V), and a single mirror maps each
//   vector of the mirror to itself times lambda, the coefficient of
//   V ⟇ antireverse(V) on the antiscalar: the mirror e23, the line x = 0,
//   with lambda = 1, sends (1, 2, 1) to (-1, 2, 1) and fixes (0, 5, 1), and the
//   mirror 2 e23 has lambda = 4 and sends (0, 5, 1) to (0, 20, 4), and the even
//   rotor (3/5) e3 + (4/5) e123, whose grades are odd, turns (1, 0, 1) to
//   (7/25, -24/25, 1). In R(3,0,0) the mirror e12 + e13, the plane spanned by
//   e1 and e2 + e3, has lambda = 2, so it sends e1 to 2 e1 and e2 + e3 to
//   2 (e2 + e3), and the normal e2 - e3 to -2 (e2 - e3)
TEST_CASE("a mirror in an odd number of generators fixes its vectors times "
          "lambda") {
  using Point2 = core::mv<rga2, core::blades<rga2, "e1 e2 e3">, rational>;
  using Line2 = core::mv<rga2, core::blades<rga2, "e23 e31 e12">, rational>;
  using Rotor2 = core::mv<rga2, core::blades<rga2, "e1 e2 e3 e123">, rational>;
  const Line2 mirror{rational{1}, rational{0}, rational{0}};
  CHECK(
      core::transform(Point2{rational{1}, rational{2}, rational{1}}, mirror) ==
      Point2{rational{-1}, rational{2}, rational{1}});
  CHECK(
      core::transform(Point2{rational{0}, rational{5}, rational{1}}, mirror) ==
      Point2{rational{0}, rational{5}, rational{1}});
  CHECK(core::transform(Point2{rational{0}, rational{5}, rational{1}},
                        Line2{rational{2}, rational{0}, rational{0}}) ==
        Point2{rational{0}, rational{20}, rational{4}});
  const Rotor2 rotor{rational{0}, rational{0}, rational{3, 5}, rational{4, 5}};
  CHECK(core::transform(Point2{rational{1}, rational{0}, rational{1}}, rotor) ==
        Point2{rational{7, 25}, rational{-24, 25}, rational{1}});

  using Vector3 = core::mv<r300, core::of_grade<3, 1>, rational>;
  using Mirror3 = core::mv<r300, core::of_grade<3, 2>, rational>;
  const Mirror3 plane{rational{1}, rational{1}, rational{0}};
  const auto vector = [](int x, int y, int z) {
    return Vector3{rational{x}, rational{y}, rational{z}};
  };
  CHECK(core::transform(vector(1, 0, 0), plane) == vector(2, 0, 0));
  CHECK(core::transform(vector(0, 1, 1), plane) == vector(0, 2, 2));
  CHECK(core::transform(vector(0, 1, -1), plane) == vector(0, -2, 2));
  CHECK(core::transform(vector(1, 2, 3), plane) == vector(2, 6, 4));
}

// Reference: independent value: by hand in conformal(3), the plane with
//   normal (g_x, g_y, g_z) and offset g_w is g_x e4235 + g_y e4315 +
//   g_z e4125 + g_w e3215, of antigrade 1, so the antiproduct of two planes
//   has the antigrades 0 and 2 and is even, while its grades 5 and 3 are
//   odd, and the round point with center c and squared radius r^2 is
//   c + e4 + (c^2 + r^2)/2 e5. The motor of the planes x = 0 and then x = 1
//   moves by 2 along x, so the round point at (1, 2, 3) with r^2 = 2,
//   (1, 2, 3, 1, 8), goes to (3, 2, 3, 1, 12), and the motor of x = 0 and then
//   the plane through the origin with normal (3/5, 4/5, 0) reflects the
//   center twice, to (-1, 2, 3) and then to (-11/5, 2/5, 3), and keeps the
//   radius: (-11/5, 2/5, 3, 1, 8), and both motors have
//   V ⟇ antireverse(V) = antiscalar
TEST_CASE("a conformal motor of two planes moves a round point") {
  using Plane5 =
      core::mv<cga3, core::blades<cga3, "e4235 e4315 e4125 e3215">, rational>;
  using Round = core::mv<cga3, core::of_grade<5, 1>, rational>;
  const Plane5 x0{rational{1}, rational{0}, rational{0}, rational{0}};
  const Plane5 x1{rational{1}, rational{0}, rational{0}, rational{-1}};
  const Plane5 tilted{rational{3, 5}, rational{4, 5}, rational{0}, rational{0}};
  const auto translation = core::compose(x1, x0);
  const auto rotation = core::compose(tilted, x0);
  using V = decltype(translation);
  static_assert(core::detail::antigrade_parity<cga3, V::layout> ==
                core::detail::Parity::even);
  for (const core::oriented_blade& blade : V::layout.blades) {
    CHECK(std::popcount(blade.mask) % 2 == 1);
  }

  const auto round = [](rational x, rational y, rational z,
                        rational squared_radius) {
    const rational half{1, 2};
    return Round{x, y, z, rational{1},
                 half * ((x * x) + (y * y) + (z * z) + squared_radius)};
  };
  const Round at = round(rational{1}, rational{2}, rational{3}, rational{2});
  CHECK(at ==
        Round{rational{1}, rational{2}, rational{3}, rational{1}, rational{8}});
  CHECK(core::transform(at, translation) ==
        round(rational{3}, rational{2}, rational{3}, rational{2}));
  CHECK(core::transform(at, rotation) ==
        round(rational{-11, 5}, rational{2, 5}, rational{3}, rational{2}));
}

// Reference: contract: transform(x, V) has the blade set of x: its type is
//   x's own mv type, with x's orientations, for the rigid(3) point, line,
//   plane, motor, and flector under their oriented names, by an oriented or
//   a canonical motor or flector, and by an empty versor, which is even and
//   gives x's layout of zeros
TEST_CASE("transform(x, V) has the blade set of x") {
  using CanonicalMotor = core::mv<rga3, even_blades_of_4<rga3>, rational>;
  using CanonicalFlector = core::mv<rga3, odd_blades_of_4<rga3>, rational>;
  constexpr core::blade_set<0> no_blades{};
  using Empty = core::mv<rga3, no_blades, rational>;
  const auto check = [&]<class X>(std::type_identity<X>) {
    static_assert(std::same_as<decltype(core::transform(X{}, Motor{})), X>);
    static_assert(std::same_as<decltype(core::transform(X{}, Flector{})), X>);
    static_assert(
        std::same_as<decltype(core::transform(X{}, CanonicalMotor{})), X>);
    static_assert(
        std::same_as<decltype(core::transform(X{}, CanonicalFlector{})), X>);
    static_assert(std::same_as<decltype(core::transform(X{}, Empty{})), X>);
    X x{};
    for (std::size_t k = 0; k < X::component_count; ++k) {
      x.coefficients.at(k) = rational{static_cast<std::int64_t>(k) + 1};
    }
    CHECK(core::transform(x, Empty{}) == X{});
  };
  check(std::type_identity<Point>{});
  check(std::type_identity<Line>{});
  check(std::type_identity<Plane>{});
  check(std::type_identity<Motor>{});
  check(std::type_identity<Flector>{});
  static_assert(!std::same_as<decltype(core::transform(Line{}, Motor{})),
                              core::mv<rga3, core::of_grade<4, 2>, rational>>);
}

// Reference: independent value: for x and V of the rigid(3) point, line,
//   plane, motor, and flector layouts with a nonzero coefficient on every
//   blade, 1 to 8 in order, and V a motor or a flector, which need not be a
//   versor, the sandwich composed from the reference's tables (of products
//   with e_i e_j + e_j e_i = 2 g_ij, of complements with
//   u ^ right(u) = antiscalar and left(u) ^ u = antiscalar, and of grade
//   signs, where the reverse of a grade-k blade is (-1)^(k(k-1)/2) times it)
//   has no part outside the blades of x, so transform drops only zeros and
//   passes its check in either checking mode, over std::int64_t and the test
//   rational
TEST_CASE("transform drops only zeros in rigid(3)") {
  const auto filled = []<class X>(std::type_identity<X>) {
    using T = X::scalar_type;
    X x{};
    for (std::size_t k = 0; k < X::component_count; ++k) {
      x.coefficients.at(k) = T{static_cast<std::int64_t>(k) + 1};
    }
    return x;
  };
  const auto check = [&]<class T>(std::type_identity<T>) {
    using M = core::mv<rga3, motor_layout, T>;
    using F = core::mv<rga3, flector_layout, T>;
    const M m = filled(std::type_identity<M>{});
    const F f = filled(std::type_identity<F>{});
    const auto against = [&]<class X>(std::type_identity<X>) {
      const X x = filled(std::type_identity<X>{});
      for (const bool odd : {false, true}) {
        CAPTURE(odd);
        const by_mask<T> whole =
            sandwich_of(golden::gp_tables::rigid_3, golden::complements::n4,
                        golden::grade_signs::n4, odd,
                        odd ? masks_of(f) : masks_of(m), masks_of(x));
        by_mask<T> kept(whole.size());
        for (const core::oriented_blade& blade : X::layout.blades) {
          kept.at(blade.mask) = whole.at(blade.mask);
        }
        CHECK(whole == kept);
        CHECK(
            difference_from(odd ? core::transform(x, f) : core::transform(x, m),
                            kept) == "");
      }
    };
    against(std::type_identity<core::mv<rga3, point_layout, T>>{});
    against(std::type_identity<core::mv<rga3, line_layout, T>>{});
    against(std::type_identity<core::mv<rga3, plane_layout, T>>{});
    against(std::type_identity<M>{});
    against(std::type_identity<F>{});
  };
  check(std::type_identity<std::int64_t>{});
  check(std::type_identity<rational>{});
}

// Reference: independent value: by hand in diag(16, 16, 1), the transform
//   of x on e1 by V on e3, which is even, reads e3 ⟇ e1 = left(right(e3)
//   right(e1)), whose coefficient is the square 16 of e2, the generator
//   both right(e3) = e12 and right(e1) = e23 hold, and then e2 ⟇ e3, whose
//   coefficient is the square 16 of e1, and std::int8_t holds both, so the call
//   is accepted, although e3 ⟇ e3 has the coefficient 16 times 16, and the
//   value is 256 times x times the square of V, so only zero fits, and the
//   result is zero
TEST_CASE("transform checks the coefficients of its two products only") {
  using X = core::mv<wide3, core::blades<wide3, "e1">, std::int8_t>;
  using V = core::mv<wide3, core::blades<wide3, "e3">, std::int8_t>;
  static_assert(std::same_as<decltype(core::transform(X{}, V{})), X>);
  CHECK(core::transform(X{0}, V{1}) == X{0});
}

// Reference: contract: a transform of operands in two algebras or with two
//   scalar types has the catch-all as its only candidate, and a
//   requires-expression answers yes for it, as the library documents, while
//   a valid call is not captured by it, since its result is of x's type,
//   and a multivector and a number have no transform
TEST_CASE("the transform overloads take the right operands") {
  using L = core::mv<rga3, line_layout, double>;
  using M = core::mv<rga3, motor_layout, double>;
  using F = core::mv<rga3, motor_layout, float>;
  using C = core::mv<cga3, core::of_grade<5, 1>, double>;
  static_assert(can_call_transform<L, M>);
  static_assert(can_call_transform<L, F>);
  static_assert(can_call_transform<L, C>);
  static_assert(!can_call_transform<L, double>);
  static_assert(!can_call_transform<double, M>);
  static_assert(std::same_as<decltype(core::transform(L{}, M{})), L>);
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
  friend int transform(gadget /*x*/, gadget /*v*/) { return 2; }
};

template <class X, class Y>
concept has_compose = requires(X x, Y y) { compose(x, y); };
template <class X, class Y>
concept has_transform = requires(X x, Y y) { transform(x, y); };

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

// Reference: contract: with galoot::core's names in scope by a
//   using-directive, a type of the user's without a transform still has
//   none, a type with its own keeps it, a multivector with such a type has
//   none, and two multivectors find the library's by an unqualified call
TEST_CASE("transform leaves other types alone") {
  using R = mv<rga3, motor_layout, double>;
  static_assert(!has_transform<widget, widget>);
  static_assert(!has_transform<R, gadget>);
  static_assert(!has_transform<gadget, R>);
  static_assert(std::same_as<decltype(transform(gadget{}, gadget{})), int>);
  CHECK(transform(gadget{}, gadget{}) == 2);
  static_assert(has_transform<R, R>);
}

} // namespace user_code
} // namespace

namespace {

// The term counts of the transform's kernels for V on the layout V and x on
// the layout X: the first stage restricted and whole, the second stage onto
// x's blades and onto the dropped blades, and the number of dropped blades.
struct kernel_counts {
  std::size_t first_kept;
  std::size_t first_all;
  std::size_t second_kept;
  std::size_t second_dropped;
  std::size_t dropped_blades;
};

template <class Alg, core::blade_set V, core::blade_set X>
constexpr kernel_counts counts_of() {
  return {
      .first_kept = core::detail::first_kernel<Alg, V, X, false>.size(),
      .first_all = core::detail::first_kernel<Alg, V, X, true>.size(),
      .second_kept = core::detail::second_kernel<Alg, V, X, true>.size(),
      .second_dropped = core::detail::second_kernel<Alg, V, X, false>.size(),
      .dropped_blades = core::detail::dropped_blade_count<Alg, V, X>(),
  };
}

constexpr bool operator==(const kernel_counts& a, const kernel_counts& b) {
  return a.first_kept == b.first_kept && a.first_all == b.first_all &&
         a.second_kept == b.second_kept &&
         a.second_dropped == b.second_dropped &&
         a.dropped_blades == b.dropped_blades;
}

// Whether every term of a kernel writes inside a result of R places, and
// exactly the first term on each place reached is marked first.
template <std::size_t R, std::size_t N>
constexpr bool well_formed(const std::array<core::detail::kernel_term, N>& k) {
  std::array<bool, R> seen{};
  for (const auto& t : k) {
    if (t.out >= R || t.first == seen[t.out]) {
      return false;
    }
    seen[t.out] = true;
  }
  return true;
}

} // namespace

// Reference: independent value: in rigid(3), blades A and B have a nonzero
//   antiproduct exactly when A or B holds e4, the null generator, since
//   their complements then share no null generator. Counting the pairs so,
//   by a separate script, a motor or a flector on a point gives 20 terms of
//   V ⟇ x and 48 of that with V, of which 28 reach the point's blades and
//   read every middle blade, and 4 blades are dropped. On a line it gives 36
//   and 48, 36 kept, 2 dropped, on a plane 28 and 48, 20 kept, 4 dropped,
//   and on a motor or a flector 48 and 48, all kept, none dropped
TEST_CASE("the transform's kernels keep the terms that reach x's blades") {
  static_assert(counts_of<rga3, motor_layout, point_layout>() ==
                kernel_counts{
                    .first_kept = 20,
                    .first_all = 20,
                    .second_kept = 28,
                    .second_dropped = 20,
                    .dropped_blades = 4,
                });
  static_assert(counts_of<rga3, flector_layout, point_layout>() ==
                kernel_counts{
                    .first_kept = 20,
                    .first_all = 20,
                    .second_kept = 28,
                    .second_dropped = 20,
                    .dropped_blades = 4,
                });
  static_assert(counts_of<rga3, motor_layout, line_layout>() ==
                kernel_counts{
                    .first_kept = 36,
                    .first_all = 36,
                    .second_kept = 36,
                    .second_dropped = 12,
                    .dropped_blades = 2,
                });
  static_assert(counts_of<rga3, motor_layout, plane_layout>() ==
                kernel_counts{
                    .first_kept = 28,
                    .first_all = 28,
                    .second_kept = 20,
                    .second_dropped = 28,
                    .dropped_blades = 4,
                });
  static_assert(counts_of<rga3, flector_layout, motor_layout>() ==
                kernel_counts{
                    .first_kept = 48,
                    .first_all = 48,
                    .second_kept = 48,
                    .second_dropped = 0,
                    .dropped_blades = 0,
                });
  static_assert(counts_of<rga3, motor_layout, flector_layout>() !=
                kernel_counts{
                    .first_kept = 48,
                    .first_all = 48,
                    .second_kept = 48,
                    .second_dropped = 0,
                    .dropped_blades = 4,
                });

  using core::detail::first_kernel;
  using core::detail::sandwich_middle_layout;
  using core::detail::second_kernel;
  static_assert(well_formed<point_layout.size()>(
      second_kernel<rga3, motor_layout, point_layout, true>));
  static_assert(
      well_formed<4>(second_kernel<rga3, motor_layout, point_layout, false>));
  static_assert(
      well_formed<
          sandwich_middle_layout<rga3, motor_layout, point_layout>.size()>(
          first_kernel<rga3, motor_layout, point_layout, false>));
  static_assert(
      !well_formed<3>(second_kernel<rga3, motor_layout, point_layout, true>));
}

// Reference: independent value: by hand, in the degenerate form with
//   e1 . e2 = 1 and every other product of generators 0, for V on e1, e2,
//   e3, e123 and x on e123, the antiscalar, which is the identity of the
//   antiproduct. V ⟇ x has one term for each blade of V, 4 in all, and its
//   middle blades are V's. The second stage reaches x's blade e123 from a
//   middle blade m and a blade v of V only when right(m) right(v) has a
//   scalar part, and among the bivectors e23, e13 (up to sign), and e12,
//   which are the right complements of e1, e2, and e3, only e12 e12 has
//   one, g12 g21 - g11 g22 = 1. So the kept terms read only the middle
//   blades e3 and e123, and the first stage keeps 2 of its 4 terms
TEST_CASE("the first stage keeps only the terms that the kept terms read") {
  constexpr auto scalar_blade_of_3 = core::blades<degenerate3, "e123">;
  static_assert(core::detail::first_kernel<degenerate3,
                                           even_blades_of_3<degenerate3>,
                                           scalar_blade_of_3, false>
                    .size() == 2);
  static_assert(core::detail::first_kernel<degenerate3,
                                           even_blades_of_3<degenerate3>,
                                           scalar_blade_of_3, true>
                    .size() == 4);
}
