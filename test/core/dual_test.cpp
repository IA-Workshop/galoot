// Tests of galoot/core/dual.hpp: the bulk and the weight of a multivector.
//
// The bulk applies the metric exomorphism G, which sends a blade e_A to the
// sum over the blades e_B of its grade of the Gram minor det g[A, B] times
// e_B, and the weight applies the antiexomorphism left(G right(u)). The
// expected values are composed in the test from the reference's table of G
// and its tables of complements, or from the squares of a diagonal form, or
// worked out by hand from those definitions. A result holds the canonical
// blades the operation reaches, each with orientation +1, in order of grade
// and then of index list; the expected layouts are written from that rule.
#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
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

template <class Alg>
concept has_bulk_layout = requires { core::bulk_layout<Alg, core::full<2>>; };
template <class Alg>
concept has_weight_layout =
    requires { core::weight_layout<Alg, core::full<2>>; };

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
