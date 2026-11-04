// Tests of galoot/core/dual.hpp: the bulk and the weight of a multivector,
// its four duals, and the dot and the antidot of two multivectors.
//
// The bulk applies the metric exomorphism G, which sends a blade e_A to the
// sum over the blades e_B of its grade of the Gram minor det g[A, B] times
// e_B, and the weight applies the antiexomorphism left(G right(u)). The bulk
// dual is the right complement of the bulk and the weight dual that of the
// weight; the left duals take the left complement instead. The dot of a and
// b is a^T G b on the scalar, and the antidot a^T antiG b on the antiscalar,
// with antiG the antiexomorphism. The expected values are composed in the test
// from the reference's table of G and its tables of complements, or from the
// squares of a diagonal form, or worked out by hand from those definitions. A
// result holds the canonical blades the operation reaches, each with
// orientation +1, in order of grade and then of index list; the expected
// layouts are written from that rule.
#include <array>
#include <bit>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <format>
#include <random>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/dual.hpp>
#include <galoot/core/linear.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>

namespace {

namespace core = galoot::core;

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
struct r110 {
  static constexpr auto value = core::R<1, 1, 0>();
};
struct r211 {
  static constexpr auto value = core::R<2, 1, 1>();
};
struct r130 {
  static constexpr auto value = core::R<1, 3, 0>();
};
struct r300 {
  static constexpr auto value = core::R<3, 0, 0>();
};
struct r202 {
  static constexpr auto value = core::R<2, 0, 2>();
};
struct r003 {
  static constexpr auto value = core::R<0, 0, 3>();
};
// Two generators of square 100 and two null ones.
struct wide_null {
  static constexpr auto value = core::from_gram({
      {100, 0, 0, 0},
      {0, 100, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0},
  });
};

inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto plane_layout = core::blades<rga3, "e423 e431 e412 e321">;
inline constexpr auto motor_layout =
    core::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
// Every blade of rigid(3) under its oriented name.
inline constexpr auto oriented_layout =
    core::blades<rga3, "1 e1 e2 e3 e4 e23 e31 e12 e41 e42 e43 e423 e431 "
                       "e412 e321 e1234">;

using Point = core::mv<rga3, point_layout, std::int64_t>;
using Line = core::mv<rga3, line_layout, std::int64_t>;
using Plane = core::mv<rga3, plane_layout, std::int64_t>;
using Motor = core::mv<rga3, motor_layout, std::int64_t>;
using Oriented = core::mv<rga3, oriented_layout, std::int64_t>;

// The number of generators of an algebra.
template <class Alg>
inline constexpr std::size_t generators =
    std::remove_cvref_t<decltype(Alg::value)>::generator_count;

// Coefficients indexed by mask, one place for each blade of an algebra.
using by_mask = std::vector<std::int64_t>;

// The coefficient of x on the canonical blade of this mask: the stored
// coefficient times the orientation of the blade in the layout, or zero when
// the layout lacks the blade.
template <class X>
std::int64_t value_on(const X& x, core::mask_t mask) {
  for (std::size_t k = 0; k < X::component_count; ++k) {
    const core::oriented_blade blade = X::layout.blades.at(k);
    if (blade.mask == mask) {
      return blade.sign * x.coefficients.at(k);
    }
  }
  return 0;
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
      if (i >= 9) {
        name += ' ';
      }
    }
  }
  return name;
}

// 1, with a failure that shows `what`, when `wrong`; otherwise 0.
int failure_if(bool wrong, const std::string& what) {
  if (wrong) {
    FAIL_CHECK(what);
    return 1;
  }
  return 0;
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

// The image of x under the exomorphism whose blade images `g` lists,
// indexed by mask.
by_mask exomorphism_of(std::span<const golden::blade_image> g,
                       const by_mask& x) {
  by_mask out(x.size());
  for (core::mask_t a = 0; a < x.size(); ++a) {
    const golden::blade_image& row = g[a];
    for (std::size_t k = 0; k < row.size; ++k) {
      out.at(row.terms.at(k).blade) += x.at(a) * row.terms.at(k).coefficient;
    }
  }
  return out;
}

by_mask right_of(std::span<const golden::complement_pair> complements,
                 const by_mask& x) {
  by_mask out(x.size());
  for (core::mask_t a = 0; a < x.size(); ++a) {
    out.at(complements[a].right.blade) += complements[a].right.sign * x.at(a);
  }
  return out;
}

by_mask left_of(std::span<const golden::complement_pair> complements,
                const by_mask& x) {
  by_mask out(x.size());
  for (core::mask_t a = 0; a < x.size(); ++a) {
    out.at(complements[a].left.blade) += complements[a].left.sign * x.at(a);
  }
  return out;
}

// Whether the tables of G and of the complements hold, at each place, the
// entry of the mask of that place.
bool tables_in_place(std::span<const golden::blade_image> g,
                     std::span<const golden::complement_pair> complements) {
  if (g.size() != complements.size()) {
    return false;
  }
  for (core::mask_t m = 0; m < g.size(); ++m) {
    if (g[m].blade != m || complements[m].blade != m) {
      return false;
    }
  }
  return true;
}

// The number of places, over every basis blade s e_A of the layout of X and
// every blade of the algebra, where op(x) differs from expected(x), with x
// that blade; one more if the layout of op's result is not the canonical
// set of the blades that expected reaches from the blades of X.
template <class X, class Op, class Expected>
int map_mismatches(const std::string& name, std::size_t blade_count,
                   const Op& op, const Expected& expected) {
  using R = decltype(op(X{}));
  int mismatches = 0;
  std::vector<bool> reached(blade_count);
  for (std::size_t i = 0; i < X::component_count; ++i) {
    const core::oriented_blade blade = X::layout.blades.at(i);
    X x{};
    x.coefficients.at(i) = 1;
    by_mask unit(blade_count);
    unit.at(blade.mask) = blade.sign;
    const by_mask want = expected(unit);
    const R got = op(x);
    for (core::mask_t m = 0; m < blade_count; ++m) {
      reached.at(m) = reached.at(m) || want.at(m) != 0;
      mismatches += failure_if(value_on(got, m) != want.at(m),
                               name + "(" + blade_name(blade.mask, blade.sign) +
                                   ") on " + blade_name(m) + " is " +
                                   std::to_string(value_on(got, m)) + ", not " +
                                   std::to_string(want.at(m)));
    }
  }
  const std::string layout = layout_difference<R>(reached);
  mismatches += failure_if(!layout.empty(), name + ": " + layout);
  return mismatches;
}

// The bulk and the weight of every blade of X against G and the
// antiexomorphism left(G right(u)), composed from the tables.
template <class X>
int bulk_weight_mismatches(
    std::span<const golden::blade_image> g,
    std::span<const golden::complement_pair> complements) {
  if (!tables_in_place(g, complements)) {
    return failure_if(true, "a table entry is out of its place");
  }
  const std::size_t size = g.size();
  const auto bulk = [](const X& x) { return core::bulk(x); };
  const auto weight = [](const X& x) { return core::weight(x); };
  const auto exomorphism = [&](const by_mask& x) {
    return exomorphism_of(g, x);
  };
  const auto antiexomorphism = [&](const by_mask& x) {
    return left_of(complements, exomorphism_of(g, right_of(complements, x)));
  };
  return map_mismatches<X>("bulk", size, bulk, exomorphism) +
         map_mismatches<X>("weight", size, weight, antiexomorphism);
}

// The bulk and the weight of every blade of the full layout of Alg in a
// diagonal form whose generators square to `squares`: by hand from the
// definitions, G e_A is the product of the squares of the generators of A
// times e_A, and the antiexomorphism left(G right(e_A)) is the product of
// the squares of the generators that A lacks times e_A, since right(e_A) is
// a sign times the blade of those generators and left undoes right.
template <class Alg>
int diagonal_mismatches(std::span<const int> squares) {
  using X = core::mv<Alg, core::full<generators<Alg>>, std::int64_t>;
  if (squares.size() != generators<Alg>) {
    return failure_if(true, "the squares are not one per generator");
  }
  const std::size_t size = std::size_t{1} << generators<Alg>;
  const auto scaled = [&](const by_mask& x, bool of_lacking) {
    by_mask out(size);
    for (core::mask_t a = 0; a < size; ++a) {
      std::int64_t factor = 1;
      for (std::size_t i = 0; i < squares.size(); ++i) {
        const bool has = ((a >> i) & 1U) != 0;
        factor *= has != of_lacking ? squares[i] : 1;
      }
      out.at(a) = factor * x.at(a);
    }
    return out;
  };
  return map_mismatches<X>(
             "bulk", size, [](const X& x) { return core::bulk(x); },
             [&](const by_mask& x) { return scaled(x, false); }) +
         map_mismatches<X>(
             "weight", size, [](const X& x) { return core::weight(x); },
             [&](const by_mask& x) { return scaled(x, true); });
}

template <class X>
X random_integers(std::mt19937_64& source) {
  std::uniform_int_distribution<std::int64_t> digit(-9, 9);
  X x{};
  for (auto& c : x.coefficients) {
    c = digit(source);
  }
  return x;
}

// Empty when x and y have the same coefficient on every canonical blade that
// either layout holds; otherwise the first blade where they differ, with
// both values. Comparing blade by blade does not need the two layouts to be
// one type.
template <class X, class Y>
std::string difference(const X& x, const Y& y) {
  const auto report = [&](core::mask_t mask) {
    return blade_name(mask) + ": " + std::to_string(value_on(x, mask)) +
           " against " + std::to_string(value_on(y, mask));
  };
  for (const core::oriented_blade& blade : X::layout.blades) {
    if (value_on(x, blade.mask) != value_on(y, blade.mask)) {
      return report(blade.mask);
    }
  }
  for (const core::oriented_blade& blade : Y::layout.blades) {
    if (value_on(x, blade.mask) != value_on(y, blade.mask)) {
      return report(blade.mask);
    }
  }
  return "";
}

// The four duals of every blade of X: right(G x), right(left(G right(x))),
// left(G x), and left(left(G right(x))), composed from the tables.
template <class X>
int dual_mismatches(std::span<const golden::blade_image> g,
                    std::span<const golden::complement_pair> complements) {
  if (!tables_in_place(g, complements)) {
    return failure_if(true, "a table entry is out of its place");
  }
  const std::size_t size = g.size();
  const auto bulk = [&](const by_mask& x) { return exomorphism_of(g, x); };
  const auto weight = [&](const by_mask& x) {
    return left_of(complements, exomorphism_of(g, right_of(complements, x)));
  };
  return map_mismatches<X>(
             "bulk_dual", size, [](const X& x) { return core::bulk_dual(x); },
             [&](const by_mask& x) { return right_of(complements, bulk(x)); }) +
         map_mismatches<X>(
             "weight_dual", size,
             [](const X& x) { return core::weight_dual(x); },
             [&](const by_mask& x) {
               return right_of(complements, weight(x));
             }) +
         map_mismatches<X>(
             "left_bulk_dual", size,
             [](const X& x) { return core::left_bulk_dual(x); },
             [&](const by_mask& x) { return left_of(complements, bulk(x)); }) +
         map_mismatches<X>(
             "left_weight_dual", size,
             [](const X& x) { return core::left_weight_dual(x); },
             [&](const by_mask& x) { return left_of(complements, weight(x)); });
}

// (-1)^(gr ag) for the blade of `mask` in an algebra of n generators: the
// sign that turns the right complement of the blade into its left one.
int complement_parity(core::mask_t mask, std::size_t n) {
  const int k = std::popcount(mask);
  const int m = static_cast<int>(n) - k;
  return (k * m) % 2 == 0 ? 1 : -1;
}

// x with each coefficient multiplied by `factor` and by complement_parity of
// its blade.
template <class X>
X by_complement_parity(X x, std::int64_t factor = 1) {
  for (std::size_t k = 0; k < X::component_count; ++k) {
    x.coefficients.at(k) *=
        factor * complement_parity(X::layout.blades.at(k).mask,
                                   generators<typename X::algebra_type>);
  }
  return x;
}

// The number of pairs of basis blades s e_A and t e_B of one grade in the
// layout of X for which a ^ d(b) differs from c (a . b) times the antiscalar,
// for the four duals d: c is 1 for the right duals and (-1)^(gr ag) for the
// left ones, and a . b is s t G_AB for the bulk duals and s t times the
// antiexomorphism's entry for the weight duals, composed from the tables.
// Pairs of different grades are skipped.
template <class X>
int wedge_dual_mismatches(
    std::span<const golden::blade_image> g,
    std::span<const golden::complement_pair> complements) {
  if (!tables_in_place(g, complements)) {
    return failure_if(true, "a table entry is out of its place");
  }
  const std::size_t size = g.size();
  const core::mask_t antiscalar = static_cast<core::mask_t>(size - 1);
  int mismatches = 0;
  for (std::size_t i = 0; i < X::component_count; ++i) {
    for (std::size_t j = 0; j < X::component_count; ++j) {
      const core::oriented_blade a = X::layout.blades.at(i);
      const core::oriented_blade b = X::layout.blades.at(j);
      if (std::popcount(a.mask) != std::popcount(b.mask)) {
        continue;
      }
      X x{};
      X y{};
      x.coefficients.at(i) = 1;
      y.coefficients.at(j) = 1;
      by_mask unit(size);
      unit.at(b.mask) = b.sign;
      const by_mask bulk_b = exomorphism_of(g, unit);
      const by_mask weight_b =
          left_of(complements, exomorphism_of(g, right_of(complements, unit)));
      const std::int64_t dot = a.sign * bulk_b.at(a.mask);
      const std::int64_t antidot = a.sign * weight_b.at(a.mask);
      const std::int64_t parity =
          complement_parity(a.mask, generators<typename X::algebra_type>);
      const std::string pair = "(" + blade_name(a.mask, a.sign) + ", " +
                               blade_name(b.mask, b.sign) + ")";
      const auto check = [&](const char* op, const auto& got,
                             std::int64_t want) {
        const std::int64_t on = value_on(got, antiscalar);
        mismatches +=
            failure_if(on != want, std::string("a ^ ") + op + "(b) for " +
                                       pair + " is " + std::to_string(on) +
                                       ", not " + std::to_string(want));
      };
      check("bulk_dual", core::wedge(x, core::bulk_dual(y)), dot);
      check("weight_dual", core::wedge(x, core::weight_dual(y)), antidot);
      check("left_bulk_dual", core::wedge(x, core::left_bulk_dual(y)),
            parity * dot);
      check("left_weight_dual", core::wedge(x, core::left_weight_dual(y)),
            parity * antidot);
    }
  }
  return mismatches;
}

// The double duals of random values of X: zero when det g is zero, and
// otherwise (-1)^(gr ag) det(g) x, which follows from right(G right(G u)) =
// right(right(left(G right(G u)))) and G composed with the antiexomorphism
// being det g times the identity, and likewise for the other three duals.
template <class X>
void check_double_duals(std::mt19937_64& source, std::int64_t det,
                        const std::string& form) {
  INFO("form: ", form);
  for (int k = 0; k < 4; ++k) {
    const X x = random_integers<X>(source);
    const X want = by_complement_parity(x, det);
    CHECK(difference(core::bulk_dual(core::bulk_dual(x)), want) == "");
    CHECK(difference(core::weight_dual(core::weight_dual(x)), want) == "");
    CHECK(difference(core::left_bulk_dual(core::left_bulk_dual(x)), want) ==
          "");
    CHECK(difference(core::left_weight_dual(core::left_weight_dual(x)), want) ==
          "");
  }
}

// The number of pairs of basis blades s e_A and t e_B of the layout of X for
// which the dot differs from s t G_AB on the scalar or the antidot from
// s t times the antiexomorphism's entry on the antiscalar, both composed
// from the tables; one more for each result layout that is not the scalar
// alone or the antiscalar alone, since every form has G 1 = 1.
template <class X>
int dot_mismatches(std::span<const golden::blade_image> g,
                   std::span<const golden::complement_pair> complements) {
  if (!tables_in_place(g, complements)) {
    return failure_if(true, "a table entry is out of its place");
  }
  const std::size_t size = g.size();
  const core::mask_t antiscalar = static_cast<core::mask_t>(size - 1);
  using D = decltype(core::dot(X{}, X{}));
  using A = decltype(core::antidot(X{}, X{}));
  int mismatches = 0;
  std::vector<bool> scalar_only(size);
  scalar_only.at(0) = true;
  std::vector<bool> antiscalar_only(size);
  antiscalar_only.at(antiscalar) = true;
  const std::string dot_layout = layout_difference<D>(scalar_only);
  const std::string antidot_layout = layout_difference<A>(antiscalar_only);
  mismatches += failure_if(!dot_layout.empty(), "dot: " + dot_layout);
  mismatches +=
      failure_if(!antidot_layout.empty(), "antidot: " + antidot_layout);
  for (std::size_t i = 0; i < X::component_count; ++i) {
    for (std::size_t j = 0; j < X::component_count; ++j) {
      const core::oriented_blade a = X::layout.blades.at(i);
      const core::oriented_blade b = X::layout.blades.at(j);
      X x{};
      X y{};
      x.coefficients.at(i) = 1;
      y.coefficients.at(j) = 1;
      by_mask unit(size);
      unit.at(b.mask) = b.sign;
      const std::int64_t dot = a.sign * exomorphism_of(g, unit).at(a.mask);
      const std::int64_t antidot =
          a.sign *
          left_of(complements, exomorphism_of(g, right_of(complements, unit)))
              .at(a.mask);
      const std::string pair = "(" + blade_name(a.mask, a.sign) + ", " +
                               blade_name(b.mask, b.sign) + ")";
      const std::int64_t got_dot = value_on(core::dot(x, y), 0);
      const std::int64_t got_antidot =
          value_on(core::antidot(x, y), antiscalar);
      mismatches += failure_if(got_dot != dot,
                               "dot" + pair + " is " + std::to_string(got_dot) +
                                   ", not " + std::to_string(dot));
      mismatches +=
          failure_if(got_antidot != antidot,
                     "antidot" + pair + " is " + std::to_string(got_antidot) +
                         ", not " + std::to_string(antidot));
    }
  }
  return mismatches;
}

// The symmetry of the dot and the antidot, and the De Morgan relation
// right(antidot(a, b)) = dot(right(a), right(b)), on random a of A and b of
// B.
template <class A, class B>
void check_dot_relations(std::mt19937_64& source, const std::string& form) {
  INFO("form: ", form);
  for (int k = 0; k < 4; ++k) {
    const A a = random_integers<A>(source);
    const B b = random_integers<B>(source);
    CHECK(difference(core::dot(a, b), core::dot(b, a)) == "");
    CHECK(difference(core::antidot(a, b), core::antidot(b, a)) == "");
    CHECK(difference(core::right_complement(core::antidot(a, b)),
                     core::dot(core::right_complement(a),
                               core::right_complement(b))) == "");
  }
}

template <class X, class Y>
concept can_call_dot = requires(X x, Y y) { core::dot(x, y); };
template <class X, class Y>
concept can_call_antidot = requires(X x, Y y) { core::antidot(x, y); };
template <class Alg>
concept has_dot_layout =
    requires { core::dot_layout<Alg, core::full<2>, core::full<2>>; };
template <class Alg>
concept has_antidot_layout =
    requires { core::antidot_layout<Alg, core::full<2>, core::full<2>>; };

template <class Alg>
concept has_bulk_layout = requires { core::bulk_layout<Alg, core::full<2>>; };
template <class Alg>
concept has_weight_layout =
    requires { core::weight_layout<Alg, core::full<2>>; };
template <class Alg>
concept has_bulk_dual_layout =
    requires { core::bulk_dual_layout<Alg, core::full<2>>; };
template <class Alg>
concept has_weight_dual_layout =
    requires { core::weight_dual_layout<Alg, core::full<2>>; };
template <class Alg>
concept has_left_bulk_dual_layout =
    requires { core::left_bulk_dual_layout<Alg, core::full<2>>; };
template <class Alg>
concept has_left_weight_dual_layout =
    requires { core::left_weight_dual_layout<Alg, core::full<2>>; };

} // namespace

// Reference: independent value: for every blade of rigid(3), conformal(2),
//   conformal(3), the degenerate form [[0, 1, 0], [1, 0, 0], [0, 0, 0]], and
//   random form 1, whose Gram matrices satisfy e_i e_j + e_j e_i = 2 g_ij, the
//   bulk is G x, where the metric exomorphism sends e_A to the sum of
//   det g[A, B] e_B, and the weight is the antiexomorphism left(G right(x)),
//   composed in the test from the complements, which satisfy
//   u ^ right(u) = antiscalar and left(u) ^ u = antiscalar, each result holds
//   the canonical blades it reaches, with sign +1, by grade and then index
//   list
TEST_CASE("bulk and weight of each basis blade are G and the antiexomorphism") {
  REQUIRE(rga3::value.gram == golden::gp_tables::rigid_3_gram);
  REQUIRE(cga2::value.gram == golden::gp_tables::conformal_2_gram);
  REQUIRE(cga3::value.gram == golden::gp_tables::conformal_3_gram);
  REQUIRE(degenerate3::value.gram == golden::gp_tables::degenerate_3_gram);
  REQUIRE(random1::value.gram == golden::gp_tables::random_1_gram);
  CHECK(bulk_weight_mismatches<core::mv<rga3, core::full<4>, std::int64_t>>(
            golden::exomorphisms::rigid_3, golden::complements::n4) == 0);
  CHECK(bulk_weight_mismatches<core::mv<cga2, core::full<4>, std::int64_t>>(
            golden::exomorphisms::conformal_2, golden::complements::n4) == 0);
  CHECK(bulk_weight_mismatches<core::mv<cga3, core::full<5>, std::int64_t>>(
            golden::exomorphisms::conformal_3, golden::complements::n5) == 0);
  CHECK(bulk_weight_mismatches<
            core::mv<degenerate3, core::full<3>, std::int64_t>>(
            golden::exomorphisms::degenerate_3, golden::complements::n3) == 0);
  CHECK(bulk_weight_mismatches<core::mv<random1, core::full<3>, std::int64_t>>(
            golden::exomorphisms::random_1, golden::complements::n3) == 0);
}

// Reference: independent value: the same for every blade of rigid(3) stored
//   under its oriented names, where an oriented name lists generators in the
//   order of their product, so that a coefficient on e31 = -e13 multiplies
//   -e13, with G, since the metric exomorphism sends e_A to the sum of
//   det g[A, B] e_B, and the complements, which satisfy
//   u ^ right(u) = antiscalar and left(u) ^ u = antiscalar
TEST_CASE("bulk and weight of oriented blades carry their orientations") {
  const auto& names = golden::oriented_names::rigid_3;
  REQUIRE(names.size() == Oriented::component_count);
  for (std::size_t k = 0; k < names.size(); ++k) {
    CAPTURE(names.at(k).name);
    REQUIRE(Oriented::layout.blades.at(k).mask == names.at(k).blade.blade);
    REQUIRE(Oriented::layout.blades.at(k).sign == names.at(k).blade.sign);
  }
  CHECK(bulk_weight_mismatches<Oriented>(golden::exomorphisms::rigid_3,
                                         golden::complements::n4) == 0);
}

// Reference: independent value: in R(1,1,0), R(2,1,1), and R(1,3,0), where
//   R(p, q, r) squares p generators to +1, q to -1, and r to 0, the bulk of a
//   blade is the product of the squares of its generators times the blade,
//   and the weight the product of the squares of the generators it lacks, by
//   hand from G and left(G right(u)), so in R(1,1,0) the bulk of e2 is -e2,
//   kept with its sign, and in R(2,1,1) every blade with e4 has no bulk
TEST_CASE("bulk and weight in a diagonal form are the products of squares") {
  const auto squares_of = [](const auto& signatures, int p, int q, int r) {
    for (const auto& s : signatures) {
      if (s.p == p && s.q == q && s.r == r) {
        return std::vector<int>(s.squares.begin(), s.squares.end());
      }
    }
    return std::vector<int>{};
  };
  const std::vector<int> s110 =
      squares_of(golden::diagonal_signatures::n2, 1, 1, 0);
  const std::vector<int> s211 =
      squares_of(golden::diagonal_signatures::n4, 2, 1, 1);
  const std::vector<int> s130 =
      squares_of(golden::diagonal_signatures::n4, 1, 3, 0);
  for (std::size_t i = 0; i < s110.size(); ++i) {
    REQUIRE(r110::value.gram.at(i).at(i) == s110.at(i));
  }
  for (std::size_t i = 0; i < s211.size(); ++i) {
    REQUIRE(r211::value.gram.at(i).at(i) == s211.at(i));
  }
  for (std::size_t i = 0; i < s130.size(); ++i) {
    REQUIRE(r130::value.gram.at(i).at(i) == s130.at(i));
  }
  CHECK(diagonal_mismatches<r110>(s110) == 0);
  CHECK(diagonal_mismatches<r211>(s211) == 0);
  CHECK(diagonal_mismatches<r130>(s130) == 0);
}

// Reference: independent value: by hand from the definitions in rigid(3),
//   where e1, e2, e3 square to 1 and e4 to 0, the bulk keeps the blades
//   without e4 and the weight those with e4, each on the canonical blades
//   with sign +1 in order of grade and then of index list: e1 e2 e3 and e4
//   for a point, e12 e13 e23 and e14 e24 e34 for a line, e123 and e124 e134
//   e234 for a plane, and 1 e12 e13 e23 and e14 e24 e34 e1234 for a motor,
//   the layout names give the same sets and take only an algebra
TEST_CASE("the bulk and weight layouts are the canonical sets of rigid(3)") {
  constexpr auto bulk_of_point = core::blades<rga3, "e1 e2 e3">;
  constexpr auto weight_of_point = core::blades<rga3, "e4">;
  constexpr auto bulk_of_line = core::blades<rga3, "e12 e13 e23">;
  constexpr auto weight_of_line = core::blades<rga3, "e14 e24 e34">;
  static_assert(core::bulk_layout<rga3, point_layout> == bulk_of_point);
  static_assert(core::weight_layout<rga3, point_layout> == weight_of_point);
  static_assert(core::bulk_layout<rga3, line_layout> == bulk_of_line);
  static_assert(core::weight_layout<rga3, line_layout> == weight_of_line);
  static_assert(core::bulk_layout<rga3, plane_layout> ==
                core::blades<rga3, "e123">);
  static_assert(core::weight_layout<rga3, plane_layout> ==
                core::blades<rga3, "e124 e134 e234">);
  static_assert(core::bulk_layout<rga3, motor_layout> ==
                core::blades<rga3, "1 e12 e13 e23">);
  static_assert(core::weight_layout<rga3, motor_layout> ==
                core::blades<rga3, "e14 e24 e34 e1234">);
  static_assert(has_bulk_layout<rga3>);
  static_assert(has_weight_layout<rga3>);
  static_assert(!has_bulk_layout<int>);
  static_assert(!has_weight_layout<int>);

  static_assert(std::same_as<decltype(core::bulk(Point{})),
                             core::mv<rga3, bulk_of_point, std::int64_t>>);
  static_assert(std::same_as<decltype(core::weight(Point{})),
                             core::mv<rga3, weight_of_point, std::int64_t>>);
  static_assert(std::same_as<decltype(core::bulk(Line{})),
                             core::mv<rga3, bulk_of_line, std::int64_t>>);
  static_assert(std::same_as<decltype(core::weight(Line{})),
                             core::mv<rga3, weight_of_line, std::int64_t>>);
  static_assert(
      std::same_as<
          decltype(core::bulk(Line{})),
          core::mv<rga3, core::bulk_layout<rga3, line_layout>, std::int64_t>>);
  static_assert(
      std::same_as<decltype(core::weight(Point{})),
                   core::mv<rga3, core::weight_layout<rga3, point_layout>,
                            std::int64_t>>);
  CHECK(core::bulk_layout<rga3, line_layout>.size() == 3);
}

// Reference: independent value: by hand in rigid(3), a line with direction
//   (1, 2, 3) on e41 e42 e43 and moment (4, 5, 6) on e23 e31 e12 has bulk
//   6 e12 - 5 e13 + 4 e23, since e31 = -e13, which reads back as the moment
//   alone under the line's orientations, and weight -e14 - 2 e24 - 3 e34,
//   which reads back as the direction alone, the weight of a point
//   x e1 + y e2 + z e3 + w e4 is w on e4
TEST_CASE("bulk and weight of a line and a point by hand") {
  const Line l{1, 2, 3, 4, 5, 6};
  const auto b = core::bulk(l);
  const auto w = core::weight(l);
  CHECK(b.coefficients == std::array<std::int64_t, 3>{6, -5, 4});
  CHECK(w.coefficients == std::array<std::int64_t, 3>{-1, -2, -3});
  CHECK(Line{b} == Line{0, 0, 0, 4, 5, 6});
  CHECK(Line{w} == Line{1, 2, 3, 0, 0, 0});

  const Point p{2, 3, 5, 7};
  CHECK(core::get<"e4">(core::weight(p)) == 7);
  CHECK(Point{core::bulk(p)} == Point{2, 3, 5, 0});
}

// Reference: identity: in rigid(3) the bulk is the part without e4 and the
//   weight the part with it, so bulk(x) + weight(x) = x, on random integer
//   multivectors of every blade and of every blade under its oriented names,
//   with the bulk's layout the blades without e4 and the weight's the blades
//   with e4
TEST_CASE("in rigid(3) the bulk and the weight split a multivector") {
  using X = core::mv<rga3, core::full<4>, std::int64_t>;
  static_assert(
      std::same_as<
          decltype(core::bulk(X{})),
          core::mv<rga3, core::blades<rga3, "1 e1 e2 e3 e12 e13 e23 e123">,
                   std::int64_t>>);
  static_assert(
      std::same_as<
          decltype(core::weight(X{})),
          core::mv<rga3,
                   core::blades<rga3, "e4 e14 e24 e34 e124 e134 e234 e1234">,
                   std::int64_t>>);
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  for (int k = 0; k < 8; ++k) {
    const X x = random_integers<X>(source);
    CHECK((X{core::bulk(x)} + X{core::weight(x)}) == x);
    const Oriented y = random_integers<Oriented>(source);
    CHECK((Oriented{core::bulk(y)} + Oriented{core::weight(y)}) == y);
  }
}

// Reference: identity: the bulk and the weight are not complementary parts
//   outside rigid(d): in R(3,0,0), where G is the identity, both are x, in
//   conformal(3), where the antiexomorphism is -G, the weight is the negated
//   bulk, as Lengyel's conformal metric page states, on random integer
//   multivectors of every blade
TEST_CASE("bulk and weight in R(3,0,0) and conformal(3)") {
  using E = core::mv<r300, core::full<3>, std::int64_t>;
  using C = core::mv<cga3, core::full<5>, std::int64_t>;
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  for (int k = 0; k < 8; ++k) {
    const E x = random_integers<E>(source);
    CHECK(difference(core::bulk(x), x) == "");
    CHECK(difference(core::weight(x), x) == "");
    const C y = random_integers<C>(source);
    CHECK(difference(core::weight(y), -core::bulk(y)) == "");
  }
}

// Reference: independent value: by hand from the definitions, in
//   conformal(3), where e4 . e5 = -1 and e4, e5 are null, G e4 = -e5 and the
//   weight of e4 is +e5, in the degenerate form [[0, 1, 0], [1, 0, 0],
//   [0, 0, 0]], G e1 = e2 and G e12 = det [[0, 1], [1, 0]] e12 = -e12, so the
//   bulk moves blades, each result is on the canonical blade it reaches
TEST_CASE("bulk and weight move blades in a form that is not diagonal") {
  using E4 = core::mv<cga3, core::blades<cga3, "e4">, std::int64_t>;
  using E5 = core::mv<cga3, core::blades<cga3, "e5">, std::int64_t>;
  static_assert(std::same_as<decltype(core::bulk(E4{})), E5>);
  static_assert(std::same_as<decltype(core::weight(E4{})), E5>);
  CHECK(core::bulk(E4{3}) == E5{-3});
  CHECK(core::weight(E4{3}) == E5{3});

  using D1 =
      core::mv<degenerate3, core::blades<degenerate3, "e1">, std::int64_t>;
  using D2 =
      core::mv<degenerate3, core::blades<degenerate3, "e2">, std::int64_t>;
  using D12 =
      core::mv<degenerate3, core::blades<degenerate3, "e12">, std::int64_t>;
  static_assert(std::same_as<decltype(core::bulk(D1{})), D2>);
  static_assert(std::same_as<decltype(core::bulk(D12{})), D12>);
  CHECK(core::bulk(D1{5}) == D2{5});
  CHECK(core::bulk(D12{5}) == D12{-5});
}

// Reference: independent value: by hand from the definitions in R(2,0,2),
//   where e3 and e4 are null, a blade with some but not all of e3 and e4 has
//   neither bulk nor weight, so the bulk and the weight of a value on e3 e4
//   e13 e24 have empty layouts, while e12 has bulk e12 and no weight and e34
//   weight e34 and no bulk, in R(0,0,3) the bulk keeps only the scalar and
//   the weight only e123, so the bulk of the vectors is empty
TEST_CASE("bulk and weight in degenerate diagonal forms") {
  using Mixed =
      core::mv<r202, core::blades<r202, "e3 e4 e13 e24">, std::int64_t>;
  static_assert(decltype(core::bulk(Mixed{}))::component_count == 0);
  static_assert(decltype(core::weight(Mixed{}))::component_count == 0);
  using Pair = core::mv<r202, core::blades<r202, "e12 e34">, std::int64_t>;
  using E12 = core::mv<r202, core::blades<r202, "e12">, std::int64_t>;
  using E34 = core::mv<r202, core::blades<r202, "e34">, std::int64_t>;
  static_assert(std::same_as<decltype(core::bulk(Pair{})), E12>);
  static_assert(std::same_as<decltype(core::weight(Pair{})), E34>);
  CHECK(core::bulk(Pair{2, 3}) == E12{2});
  CHECK(core::weight(Pair{2, 3}) == E34{3});

  using Full = core::mv<r003, core::full<3>, std::int64_t>;
  using Scalar = core::mv<r003, core::blades<r003, "1">, std::int64_t>;
  using Top = core::mv<r003, core::blades<r003, "e123">, std::int64_t>;
  using Vectors = core::mv<r003, core::of_grade<3, 1>, std::int64_t>;
  static_assert(std::same_as<decltype(core::bulk(Full{})), Scalar>);
  static_assert(std::same_as<decltype(core::weight(Full{})), Top>);
  static_assert(decltype(core::bulk(Vectors{}))::component_count == 0);
  const Full x{1, 2, 3, 4, 5, 6, 7, 8};
  CHECK(core::bulk(x) == Scalar{1});
  CHECK(core::weight(x) == Top{8});
}

// Reference: independent value: for every blade of rigid(3), conformal(2),
//   conformal(3), the degenerate form [[0, 1, 0], [1, 0, 0], [0, 0, 0]], and
//   random form 1, whose Gram matrices satisfy e_i e_j + e_j e_i = 2 g_ij, the
//   bulk dual is right(G x) and the weight dual right(left(G right(x))),
//   and the left duals take the left complement in place of the outer right
//   one, where the metric exomorphism sends e_A to the sum of
//   det g[A, B] e_B and the complements satisfy u ^ right(u) = antiscalar
//   and left(u) ^ u = antiscalar, each result on the canonical blades it
//   reaches
TEST_CASE("the four duals of each basis blade are composed from G") {
  REQUIRE(rga3::value.gram == golden::gp_tables::rigid_3_gram);
  REQUIRE(cga2::value.gram == golden::gp_tables::conformal_2_gram);
  REQUIRE(cga3::value.gram == golden::gp_tables::conformal_3_gram);
  REQUIRE(degenerate3::value.gram == golden::gp_tables::degenerate_3_gram);
  REQUIRE(random1::value.gram == golden::gp_tables::random_1_gram);
  CHECK(dual_mismatches<core::mv<rga3, core::full<4>, std::int64_t>>(
            golden::exomorphisms::rigid_3, golden::complements::n4) == 0);
  CHECK(dual_mismatches<core::mv<cga2, core::full<4>, std::int64_t>>(
            golden::exomorphisms::conformal_2, golden::complements::n4) == 0);
  CHECK(dual_mismatches<core::mv<cga3, core::full<5>, std::int64_t>>(
            golden::exomorphisms::conformal_3, golden::complements::n5) == 0);
  CHECK(dual_mismatches<core::mv<degenerate3, core::full<3>, std::int64_t>>(
            golden::exomorphisms::degenerate_3, golden::complements::n3) == 0);
  CHECK(dual_mismatches<core::mv<random1, core::full<3>, std::int64_t>>(
            golden::exomorphisms::random_1, golden::complements::n3) == 0);
}

// Reference: independent value: the same for every blade of rigid(3) stored
//   under its oriented names, where an oriented name lists generators in the
//   order of their product, with G, since the metric exomorphism sends e_A
//   to the sum of det g[A, B] e_B, and the complements, which satisfy
//   u ^ right(u) = antiscalar and left(u) ^ u = antiscalar
TEST_CASE("the duals of oriented blades carry their orientations") {
  const auto& names = golden::oriented_names::rigid_3;
  REQUIRE(names.size() == Oriented::component_count);
  for (std::size_t k = 0; k < names.size(); ++k) {
    CAPTURE(names.at(k).name);
    REQUIRE(Oriented::layout.blades.at(k).mask == names.at(k).blade.blade);
    REQUIRE(Oriented::layout.blades.at(k).sign == names.at(k).blade.sign);
  }
  CHECK(dual_mismatches<Oriented>(golden::exomorphisms::rigid_3,
                                  golden::complements::n4) == 0);
}

// Reference: proof: for every pair of blades a and b of equal grade of
//   rigid(3), of rigid(3) under its oriented names, and of conformal(2),
//   whose Gram matrices satisfy e_i e_j + e_j e_i = 2 g_ij,
//   a ^ bulk_dual(b) = (a . b) antiscalar for a and b of equal grade, and
//   a ^ weight_dual(b) is the antidot times the antiscalar, with a . b the
//   entry of G, since the metric exomorphism sends e_A to the sum of
//   det g[A, B] e_B, and the antidot the entry of left(G right(u)), composed
//   with the complements, which satisfy u ^ right(u) = antiscalar and
//   left(u) ^ u = antiscalar, never with the library's dot, and the left
//   duals are the right duals times (-1)^(gr ag), so at the odd grades of
//   n = 4 the left duals give the negation, the sign that tells them apart
TEST_CASE("a wedged with the dual of b is the dot times the antiscalar") {
  REQUIRE(rga3::value.gram == golden::gp_tables::rigid_3_gram);
  REQUIRE(cga2::value.gram == golden::gp_tables::conformal_2_gram);
  CHECK(wedge_dual_mismatches<core::mv<rga3, core::full<4>, std::int64_t>>(
            golden::exomorphisms::rigid_3, golden::complements::n4) == 0);
  CHECK(wedge_dual_mismatches<Oriented>(golden::exomorphisms::rigid_3,
                                        golden::complements::n4) == 0);
  CHECK(wedge_dual_mismatches<core::mv<cga2, core::full<4>, std::int64_t>>(
            golden::exomorphisms::conformal_2, golden::complements::n4) == 0);
}

// Reference: independent value: by hand in rigid(3), from right(e1) = e234,
//   right(e2) = -e134, right(e3) = e124, and right(e4) = -e123, the bulk dual
//   of the point x e1 + y e2 + z e3 + w e4 read as a plane is (x, y, z, 0),
//   the left bulk dual is (-x, -y, -z, 0), since the left complement of a
//   vector is the negated right one when n = 4, the weight dual is
//   (0, 0, 0, w), and the left weight dual (0, 0, 0, -w), so the right and
//   left duals differ in sign, the bulk dual differs from the weight dual,
//   and right(G x) differs from G right(x), which is (0, 0, 0, w)
TEST_CASE("the duals of a point are planes with the dual-sign convention") {
  const Point p{2, 3, 5, 7};
  static_assert(
      std::same_as<
          decltype(core::bulk_dual(p)),
          core::mv<rga3, core::blades<rga3, "e124 e134 e234">, std::int64_t>>);
  static_assert(
      std::same_as<decltype(core::weight_dual(p)),
                   core::mv<rga3, core::blades<rga3, "e123">, std::int64_t>>);
  CHECK(Plane{core::bulk_dual(p)} == Plane{2, 3, 5, 0});
  CHECK(Plane{core::left_bulk_dual(p)} == Plane{-2, -3, -5, 0});
  CHECK(Plane{core::weight_dual(p)} == Plane{0, 0, 0, 7});
  CHECK(Plane{core::left_weight_dual(p)} == Plane{0, 0, 0, -7});
  CHECK(Plane{core::bulk(core::right_complement(p))} == Plane{0, 0, 0, 7});
}

// Reference: independent value: by hand in rigid(3), a line with direction
//   (1, 2, 3) on e41 e42 e43 and moment (4, 5, 6) on e23 e31 e12 has bulk
//   dual 4 e14 + 5 e24 + 6 e34, from right(e23) = e14, right(e13) = -e24, and
//   right(e12) = e34, which reads as the line with direction (-4, -5, -6) and
//   no moment, and weight dual -1 e23 + 2 e13 - 3 e12, from right(e14) = e23,
//   right(e24) = -e13, and right(e34) = e12, the line with moment
//   (-1, -2, -3) and no direction, and the left duals of a line are the same,
//   since gr ag = 4 is even, a plane with normal (2, 3, 5) and offset 7 has
//   bulk dual -7 e4 and weight dual -2 e1 - 3 e2 - 5 e3
TEST_CASE("the duals of a line and a plane by hand") {
  const Line l{1, 2, 3, 4, 5, 6};
  CHECK(Line{core::bulk_dual(l)} == Line{-4, -5, -6, 0, 0, 0});
  CHECK(Line{core::weight_dual(l)} == Line{0, 0, 0, -1, -2, -3});
  CHECK(Line{core::left_bulk_dual(l)} == Line{-4, -5, -6, 0, 0, 0});
  CHECK(Line{core::left_weight_dual(l)} == Line{0, 0, 0, -1, -2, -3});
  const Plane g{2, 3, 5, 7};
  CHECK(Point{core::bulk_dual(g)} == Point{0, 0, 0, -7});
  CHECK(Point{core::weight_dual(g)} == Point{-2, -3, -5, 0});
}

// Reference: independent value: by hand from the definitions in rigid(3),
//   the duals of a point are on the canonical complements of its bulk and
//   weight, e124 e134 e234 for the bulk duals and e123 for the weight duals,
//   and those of a line on e14 e24 e34 and e12 e13 e23, each with sign +1 in
//   order of grade and then of index list, and the layout names give the
//   same sets and take only an algebra
TEST_CASE("the dual layout names are the canonical sets of rigid(3)") {
  constexpr auto point_bulk_dual = core::blades<rga3, "e124 e134 e234">;
  constexpr auto point_weight_dual = core::blades<rga3, "e123">;
  constexpr auto line_bulk_dual = core::blades<rga3, "e14 e24 e34">;
  constexpr auto line_weight_dual = core::blades<rga3, "e12 e13 e23">;
  static_assert(core::bulk_dual_layout<rga3, point_layout> == point_bulk_dual);
  static_assert(core::left_bulk_dual_layout<rga3, point_layout> ==
                point_bulk_dual);
  static_assert(core::weight_dual_layout<rga3, point_layout> ==
                point_weight_dual);
  static_assert(core::left_weight_dual_layout<rga3, point_layout> ==
                point_weight_dual);
  static_assert(core::bulk_dual_layout<rga3, line_layout> == line_bulk_dual);
  static_assert(core::left_bulk_dual_layout<rga3, line_layout> ==
                line_bulk_dual);
  static_assert(core::weight_dual_layout<rga3, line_layout> ==
                line_weight_dual);
  static_assert(core::left_weight_dual_layout<rga3, line_layout> ==
                line_weight_dual);
  static_assert(has_bulk_dual_layout<rga3>);
  static_assert(has_weight_dual_layout<rga3>);
  static_assert(has_left_bulk_dual_layout<rga3>);
  static_assert(has_left_weight_dual_layout<rga3>);
  static_assert(!has_bulk_dual_layout<int>);
  static_assert(!has_weight_dual_layout<int>);
  static_assert(!has_left_bulk_dual_layout<int>);
  static_assert(!has_left_weight_dual_layout<int>);

  static_assert(
      std::same_as<decltype(core::bulk_dual(Line{})),
                   core::mv<rga3, core::bulk_dual_layout<rga3, line_layout>,
                            std::int64_t>>);
  static_assert(
      std::same_as<decltype(core::weight_dual(Line{})),
                   core::mv<rga3, core::weight_dual_layout<rga3, line_layout>,
                            std::int64_t>>);
  static_assert(std::same_as<
                decltype(core::left_bulk_dual(Point{})),
                core::mv<rga3, core::left_bulk_dual_layout<rga3, point_layout>,
                         std::int64_t>>);
  static_assert(
      std::same_as<
          decltype(core::left_weight_dual(Point{})),
          core::mv<rga3, core::left_weight_dual_layout<rga3, point_layout>,
                   std::int64_t>>);
  CHECK(core::bulk_dual_layout<rga3, point_layout>.size() == 3);
}

// Reference: identity: the left duals are the right duals times
//   (-1)^(gr ag), since the left complement of a blade is (-1)^(gr ag) times
//   its right complement, on random integer multivectors of every blade of
//   rigid(3), conformal(2), and conformal(3) and of rigid(3) under its
//   oriented names
TEST_CASE("the left duals are the right duals times a sign by grade") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  const auto check = [&]<class X>(std::type_identity<X>) {
    for (int k = 0; k < 4; ++k) {
      const X x = random_integers<X>(source);
      const auto b = core::bulk_dual(x);
      const auto w = core::weight_dual(x);
      CHECK(difference(core::left_bulk_dual(x), by_complement_parity(b)) == "");
      CHECK(difference(core::left_weight_dual(x), by_complement_parity(w)) ==
            "");
    }
  };
  check(std::type_identity<core::mv<rga3, core::full<4>, std::int64_t>>{});
  check(std::type_identity<core::mv<cga2, core::full<4>, std::int64_t>>{});
  check(std::type_identity<core::mv<cga3, core::full<5>, std::int64_t>>{});
  check(std::type_identity<Oriented>{});
}

// Reference: identity: in a degenerate metric the double bulk dual is zero,
//   and so is every double dual, by value, on random integer multivectors of
//   every blade of rigid(3), R(0,0,3), R(2,0,2), and the degenerate form
//   [[0, 1, 0], [1, 0, 0], [0, 0, 0]], where det g = 0, and in rigid(3) the
//   double bulk dual has no blade at all, since G drops every blade with e4
//   and the bulk dual of a blade without e4 has e4, in R(3,0,0), R(1,3,0),
//   conformal(2), conformal(3), and random form 1 each double dual is
//   (-1)^(gr ag) det(g) x instead, with det g 1, -1, and the determinants of
//   the Gram matrices, which satisfy e_i e_j + e_j e_i = 2 g_ij
TEST_CASE("the double duals are zero exactly when the metric is degenerate") {
  using R = core::mv<rga3, core::full<4>, std::int64_t>;
  static_assert(
      decltype(core::bulk_dual(core::bulk_dual(R{})))::component_count == 0);
  REQUIRE(cga2::value.gram == golden::gp_tables::conformal_2_gram);
  REQUIRE(cga3::value.gram == golden::gp_tables::conformal_3_gram);
  REQUIRE(random1::value.gram == golden::gp_tables::random_1_gram);
  REQUIRE(golden::gp_tables::rigid_3_det == 0);
  REQUIRE(golden::gp_tables::degenerate_3_det == 0);
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  check_double_duals<R>(source, 0, "rigid(3)");
  check_double_duals<core::mv<r003, core::full<3>, std::int64_t>>(source, 0,
                                                                  "R(0,0,3)");
  check_double_duals<core::mv<r202, core::full<4>, std::int64_t>>(source, 0,
                                                                  "R(2,0,2)");
  check_double_duals<core::mv<degenerate3, core::full<3>, std::int64_t>>(
      source, 0, "degenerate");
  check_double_duals<core::mv<r300, core::full<3>, std::int64_t>>(source, 1,
                                                                  "R(3,0,0)");
  check_double_duals<core::mv<r130, core::full<4>, std::int64_t>>(source, -1,
                                                                  "R(1,3,0)");
  check_double_duals<core::mv<cga2, core::full<4>, std::int64_t>>(
      source, golden::gp_tables::conformal_2_det, "conformal(2)");
  check_double_duals<core::mv<cga3, core::full<5>, std::int64_t>>(
      source, golden::gp_tables::conformal_3_det, "conformal(3)");
  check_double_duals<core::mv<random1, core::full<3>, std::int64_t>>(
      source, golden::gp_tables::random_1_det, "random form 1");
}

// Reference: independent value: in conformal(3), the bulk dual of the real
//   sphere with center p and radius r, -e1234 + p_x e4235 + p_y e4315 +
//   p_z e4125 - (p^2 - r^2)/2 e3215 as Lengyel's sphere page writes it, is
//   -(p + e4 + (p^2 - r^2)/2 e5), a round point with r^2 negated, as
//   Lengyel's round point page relates it, worked by hand for p = (1, 2, 3)
//   and r^2 = 4, where (p^2 - r^2)/2 = 5
TEST_CASE("the bulk dual of a conformal sphere is a round point") {
  using Sphere =
      core::mv<cga3, core::blades<cga3, "e1234 e4235 e4315 e4125 e3215">,
               std::int64_t>;
  using Vector = core::mv<cga3, core::of_grade<5, 1>, std::int64_t>;
  const Sphere s{-1, 1, 2, 3, -5};
  CHECK(Vector{core::bulk_dual(s)} == Vector{-1, -2, -3, -1, -5});
}

// Reference: independent value: for every pair of blades of rigid(3),
//   conformal(2), conformal(3), the degenerate form
//   [[0, 1, 0], [1, 0, 0], [0, 0, 0]], and random form 1, whose Gram matrices
//   satisfy e_i e_j + e_j e_i = 2 g_ij, the dot is a^T G b and the antidot
//   a^T antiG b times the antiscalar, with G, since the metric exomorphism
//   sends e_A to the sum of det g[A, B] e_B, and antiG = left(G right(u))
//   composed with the complements, which satisfy u ^ right(u) = antiscalar
//   and left(u) ^ u = antiscalar, the dot on the scalar alone and the
//   antidot on the antiscalar alone
TEST_CASE("dot and antidot of basis blades are the entries of G and antiG") {
  REQUIRE(rga3::value.gram == golden::gp_tables::rigid_3_gram);
  REQUIRE(cga2::value.gram == golden::gp_tables::conformal_2_gram);
  REQUIRE(cga3::value.gram == golden::gp_tables::conformal_3_gram);
  REQUIRE(degenerate3::value.gram == golden::gp_tables::degenerate_3_gram);
  REQUIRE(random1::value.gram == golden::gp_tables::random_1_gram);
  CHECK(dot_mismatches<core::mv<rga3, core::full<4>, std::int64_t>>(
            golden::exomorphisms::rigid_3, golden::complements::n4) == 0);
  CHECK(dot_mismatches<core::mv<cga2, core::full<4>, std::int64_t>>(
            golden::exomorphisms::conformal_2, golden::complements::n4) == 0);
  CHECK(dot_mismatches<core::mv<cga3, core::full<5>, std::int64_t>>(
            golden::exomorphisms::conformal_3, golden::complements::n5) == 0);
  CHECK(dot_mismatches<core::mv<degenerate3, core::full<3>, std::int64_t>>(
            golden::exomorphisms::degenerate_3, golden::complements::n3) == 0);
  CHECK(dot_mismatches<core::mv<random1, core::full<3>, std::int64_t>>(
            golden::exomorphisms::random_1, golden::complements::n3) == 0);
}

// Reference: independent value: the same for every pair of blades of
//   rigid(3) stored under their oriented names, where an oriented name lists
//   generators in the order of their product, with G, since the metric
//   exomorphism sends e_A to the sum of det g[A, B] e_B, and the
//   complements, which satisfy u ^ right(u) = antiscalar and
//   left(u) ^ u = antiscalar, so the dot is a^T G b and the antidot
//   a^T antiG b times the antiscalar
TEST_CASE("dot and antidot of oriented blades carry their orientations") {
  const auto& names = golden::oriented_names::rigid_3;
  REQUIRE(names.size() == Oriented::component_count);
  for (std::size_t k = 0; k < names.size(); ++k) {
    CAPTURE(names.at(k).name);
    REQUIRE(Oriented::layout.blades.at(k).mask == names.at(k).blade.blade);
    REQUIRE(Oriented::layout.blades.at(k).sign == names.at(k).blade.sign);
  }
  CHECK(dot_mismatches<Oriented>(golden::exomorphisms::rigid_3,
                                 golden::complements::n4) == 0);
}

// Reference: independent value: by hand in rigid(3), where e1, e2, e3
//   square to 1 and e4 to 0, the dot of two points is
//   x1 x2 + y1 y2 + z1 z2 on the scalar and their antidot w1 w2 on e1234,
//   and the dot of two motors is the product of their scalars plus the
//   moments' dot product, since e31 = -e13 enters twice, and their antidot
//   the directions' dot product plus the product of their e1234 parts
TEST_CASE("dot and antidot of points and motors by hand") {
  const Point p{2, 3, 5, 7};
  const Point q{11, 13, 17, 19};
  using Scalar = core::mv<rga3, core::blades<rga3, "1">, std::int64_t>;
  using Top = core::mv<rga3, core::blades<rga3, "e1234">, std::int64_t>;
  static_assert(std::same_as<decltype(core::dot(p, q)), Scalar>);
  static_assert(std::same_as<decltype(core::antidot(p, q)), Top>);
  CHECK(core::get<"1">(core::dot(p, q)) == (2 * 11) + (3 * 13) + (5 * 17));
  CHECK(core::get<"e1234">(core::antidot(p, q)) == 7 * 19);

  const Motor m{1, 2, 3, 4, 5, 6, 7, 8};
  const Motor n{9, 10, 11, 12, 13, 14, 15, 16};
  static_assert(std::same_as<decltype(core::dot(m, n)), Scalar>);
  static_assert(std::same_as<decltype(core::antidot(m, n)), Top>);
  CHECK(core::get<"1">(core::dot(m, n)) ==
        (1 * 9) + (5 * 13) + (6 * 14) + (7 * 15));
  CHECK(core::get<"e1234">(core::antidot(m, n)) ==
        (2 * 10) + (3 * 11) + (4 * 12) + (8 * 16));
}

// Reference: independent value: by hand in R(3,0,0), e12 . e12 = +1, since
//   the dot is a^T G b and G e12 = det [[1, 0], [0, 1]] e12 = e12, where the
//   scalar part of the geometric product e12 e12 is -1
TEST_CASE("the dot of e12 with itself is +1 in a Euclidean metric") {
  using Bivector = core::mv<r300, core::blades<r300, "e12">, std::int64_t>;
  const Bivector e12{1};
  CHECK(core::get<"1">(core::dot(e12, e12)) == 1);
}

// Reference: identity: the dot and the antidot are symmetric, since G and
//   antiG are symmetric, and right(antidot(a, b)) = dot(right(a), right(b)),
//   since antiG = left G right and the right complement is a signed
//   permutation whose inverse, the left complement, is its transpose, on
//   random integer multivectors of every blade of rigid(3), conformal(2),
//   conformal(3), the degenerate form, and random form 1, and on a point and
//   a motor of rigid(3)
TEST_CASE("dot and antidot are symmetric and related through complements") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  using R = core::mv<rga3, core::full<4>, std::int64_t>;
  check_dot_relations<R, R>(source, "rigid(3)");
  check_dot_relations<Point, Motor>(source, "rigid(3), point and motor");
  check_dot_relations<Oriented, R>(source, "rigid(3), oriented");
  check_dot_relations<core::mv<cga2, core::full<4>, std::int64_t>,
                      core::mv<cga2, core::full<4>, std::int64_t>>(
      source, "conformal(2)");
  check_dot_relations<core::mv<cga3, core::full<5>, std::int64_t>,
                      core::mv<cga3, core::full<5>, std::int64_t>>(
      source, "conformal(3)");
  check_dot_relations<core::mv<degenerate3, core::full<3>, std::int64_t>,
                      core::mv<degenerate3, core::full<3>, std::int64_t>>(
      source, "degenerate");
  check_dot_relations<core::mv<random1, core::full<3>, std::int64_t>,
                      core::mv<random1, core::full<3>, std::int64_t>>(
      source, "random form 1");
}

// Reference: independent value: by hand from the definitions in rigid(3),
//   the dot holds the scalar only when the operands share a grade on which G
//   is not zero, and the antidot holds e1234 only when they share one on
//   which antiG is not zero, so the dot and the antidot of a point and a
//   line, the dot of two multiples of e4, and the antidot of two multiples
//   of e1 have no blade at all, while two points or two lines give the
//   scalar and e1234, and the layout names give the same sets and take only
//   an algebra
TEST_CASE("the dot and antidot layouts are the scalar, the antiscalar, or "
          "empty") {
  constexpr auto scalar = core::blades<rga3, "1">;
  constexpr auto antiscalar = core::blades<rga3, "e1234">;
  constexpr auto e1 = core::blades<rga3, "e1">;
  constexpr auto e4 = core::blades<rga3, "e4">;
  static_assert(core::dot_layout<rga3, point_layout, point_layout> == scalar);
  static_assert(core::dot_layout<rga3, line_layout, line_layout> == scalar);
  static_assert(core::antidot_layout<rga3, point_layout, point_layout> ==
                antiscalar);
  static_assert(core::antidot_layout<rga3, line_layout, line_layout> ==
                antiscalar);
  static_assert(core::dot_layout<rga3, point_layout, line_layout>.size() == 0);
  static_assert(core::antidot_layout<rga3, point_layout, line_layout>.size() ==
                0);
  static_assert(core::dot_layout<rga3, e4, e4>.size() == 0);
  static_assert(core::antidot_layout<rga3, e1, e1>.size() == 0);
  static_assert(core::dot_layout<rga3, e1, e1> == scalar);
  static_assert(core::antidot_layout<rga3, e4, e4> == antiscalar);
  static_assert(has_dot_layout<rga3>);
  static_assert(has_antidot_layout<rga3>);
  static_assert(!has_dot_layout<int>);
  static_assert(!has_antidot_layout<int>);

  const Point p{2, 3, 5, 7};
  const Line l{1, 2, 3, 4, 5, 6};
  static_assert(decltype(core::dot(p, l))::component_count == 0);
  static_assert(decltype(core::antidot(l, p))::component_count == 0);
  static_assert(
      std::same_as<
          decltype(core::dot(p, l)),
          core::mv<rga3, core::dot_layout<rga3, point_layout, line_layout>,
                   std::int64_t>>);
  static_assert(
      std::same_as<
          decltype(core::antidot(l, l)),
          core::mv<rga3, core::antidot_layout<rga3, line_layout, line_layout>,
                   std::int64_t>>);
  CHECK(core::dot_layout<rga3, point_layout, point_layout>.size() == 1);
}

// Reference: contract: the dot computes its one coefficient by assigning
//   the first term rather than adding it to zero, so a lone product of
//   -0.0 and 1.0 keeps its sign, as IEEE 754 gives -0.0 * 1.0 = -0.0 and
//   0.0 + -0.0 = +0.0
TEST_CASE("the dot keeps a negative zero") {
  using V = core::mv<rga3, core::blades<rga3, "e1">, double>;
  const V a{-0.0};
  const V b{1.0};
  const double d = core::get<"1">(core::dot(a, b));
  INFO("dot: ", std::format("{}", d));
  CHECK(std::signbit(d));
}

// Reference: independent value: by hand in diag(100, 100, 0, 0), the dot of
//   two values on e1 e123 e124 reads only the terms on the scalar, of which
//   e1 . e1 = 100 is the only one, since e3 and e4 are null and the form is
//   diagonal, so std::int8_t holds it, although the whole product
//   e123 reverse(e124) is -10000 e34, beyond std::int8_t, and likewise the
//   antidot of two values on e234 e3 e4, whose right complements are -e1,
//   e124, and -e123
TEST_CASE("the dot and antidot accept a scalar that holds their own "
          "coefficients") {
  using Small =
      core::mv<wide_null, core::blades<wide_null, "e1 e123 e124">, std::int8_t>;
  using Complements =
      core::mv<wide_null, core::blades<wide_null, "e234 e3 e4">, std::int8_t>;
  const Small a{1, 0, 0};
  const Complements b{1, 0, 0};
  static_assert(
      std::same_as<
          decltype(core::dot(a, a)),
          core::mv<wide_null, core::blades<wide_null, "1">, std::int8_t>>);
  static_assert(
      std::same_as<
          decltype(core::antidot(b, b)),
          core::mv<wide_null, core::blades<wide_null, "e1234">, std::int8_t>>);
  CHECK(core::get<"1">(core::dot(a, a)) == 100);
  CHECK(core::get<"e1234">(core::antidot(b, b)) == 100);
}

// Reference: independent value: in conformal(3), the round point
//   p + e4 + (p^2 + r^2)/2 e5 has P . P = -r^2, as Lengyel's dot products
//   page gives, since e4 . e5 = -1, worked by hand for p = (1, 2, 3) and
//   r^2 = 2, where (p^2 + r^2)/2 = 8 and P . P = 14 - 16 = -2
TEST_CASE("the dot of a round point with itself is minus its squared radius") {
  using Vector = core::mv<cga3, core::of_grade<5, 1>, std::int64_t>;
  const Vector round{1, 2, 3, 1, 8};
  CHECK(core::get<"1">(core::dot(round, round)) == -2);
}

// Reference: contract: a dot or antidot of operands in two algebras or with
//   two scalar types has the catch-all as its only candidate, and a
//   requires-expression answers yes for it, as the library documents, while
//   a valid call is not captured by it, since its result is on the scalar or
//   the antiscalar and not the first operand's type, and a multivector and
//   a number have no dot
TEST_CASE("the dot and antidot overloads take the right operands") {
  using R = core::mv<rga3, core::full<4>, double>;
  using F = core::mv<rga3, core::full<4>, float>;
  using C = core::mv<cga3, core::full<5>, double>;
  static_assert(can_call_dot<R, R>);
  static_assert(can_call_dot<R, F>);
  static_assert(can_call_dot<R, C>);
  static_assert(can_call_antidot<R, F>);
  static_assert(can_call_antidot<R, C>);
  static_assert(!can_call_dot<R, double>);
  static_assert(!can_call_dot<double, R>);
  static_assert(!can_call_antidot<R, int>);
  static_assert(!can_call_antidot<int, R>);
  static_assert(std::same_as<decltype(core::dot(R{}, R{})),
                             core::mv<rga3, core::blades<rga3, "1">, double>>);
  static_assert(
      std::same_as<decltype(core::antidot(R{}, R{})),
                   core::mv<rga3, core::blades<rga3, "e1234">, double>>);
}

// A scope that sees galoot::core's names by a using-directive, as a user's
// code may, with types of its own: one with no dot or antidot and one with
// its own. The library's catch-alls must leave both alone.
namespace {
namespace user_code {

using namespace galoot::core;

struct widget {};

struct gadget {
  friend int dot(gadget /*a*/, gadget /*b*/) { return 1; }
  friend int antidot(gadget /*a*/, gadget /*b*/) { return 2; }
};

template <class X, class Y>
concept has_dot = requires(X x, Y y) { dot(x, y); };
template <class X, class Y>
concept has_antidot = requires(X x, Y y) { antidot(x, y); };

// Reference: contract: with galoot::core's names in scope by a
//   using-directive, a type of the user's without a dot still has none, a
//   type with its own dot and antidot keeps them, a multivector with such a
//   type has none, and two multivectors find the library's by an
//   unqualified call
TEST_CASE("the dot and antidot leave other types alone") {
  using R = mv<rga3, full<4>, double>;
  static_assert(!has_dot<widget, widget>);
  static_assert(!has_antidot<widget, widget>);
  static_assert(!has_dot<R, widget>);
  static_assert(!has_antidot<gadget, R>);
  static_assert(std::same_as<decltype(dot(gadget{}, gadget{})), int>);
  static_assert(std::same_as<decltype(antidot(gadget{}, gadget{})), int>);
  CHECK(dot(gadget{}, gadget{}) == 1);
  CHECK(antidot(gadget{}, gadget{}) == 2);
  static_assert(has_dot<R, R>);
  static_assert(has_antidot<R, R>);
}

} // namespace user_code
} // namespace
