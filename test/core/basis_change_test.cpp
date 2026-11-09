// Tests of galoot/core/basis_change.hpp against the mathematics it
// implements: what a change of basis accepts, the image of every blade under
// conformal_change<D>() and its inverse against the golden values of
// reference/golden.hpp, and hand values from the definition.
//
// A change P writes the source's generator e_i as row i over its
// denominator, e_i = sum_j P_ij e'_j. It sends the blade e_A to the wedge of
// the images of its generators, the sum over the target's blades e'_B of A's
// grade of the minor det P[A, B] times e'_B, so the antiscalar goes to
// det(P) times the target's antiscalar. In conformal(d), e1 to ed are
// Euclidean, e(d+1) is the origin and e(d+2) the point at infinity, both
// null, with e(d+1) . e(d+2) = -1. conformal_change<D>() keeps e_i = e'_i
// for i <= d and writes the origin as (e'(d+2) - e'(d+1)) / 2 and the point
// at infinity as e'(d+2) + e'(d+1) in R(d+1, 1, 0), where e'(d+1) squares to
// +1 and e'(d+2) to -1; its determinant is -1.
#include <array>
#include <cstddef>
#include <type_traits>

#include <galoot/core/basis_change.hpp>
#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>
#include <support/rational.hpp>

namespace {

namespace core = galoot::core;
using test_support::rational;

// conformal(D), its diagonal counterpart R(D+1, 1, 0), and the change of
// conformal_change<D>() between them.
template <std::size_t D>
struct cga {
  static constexpr auto value = core::conformal<D>();
};
template <std::size_t D>
struct cga_diagonal {
  static constexpr auto value = core::R<D + 1, 1, 0>();
};
template <std::size_t D>
struct cga_to_diagonal {
  using source = cga<D>;
  using target = cga_diagonal<D>;
  static constexpr auto value = core::conformal_change<D>();
};

using cga3 = cga<3>;
using r410 = cga_diagonal<3>;
using cga3_to_r410 = cga_to_diagonal<3>;

// The Witt form of one mode, scaled so that e1 . e2 = 1, against R(1, 1, 0):
// e1 = (e'1 + e'2) / 2 and e2 = e'1 - e'2, which sends the creation and
// annihilation pair c = e1, c† = e2 / 2 to (e'1 + e'2) / 2 and
// (e'1 - e'2) / 2.
struct witt1 {
  static constexpr auto value = core::from_gram({
      {0, 1},
      {1, 0},
  });
};
struct r110 {
  static constexpr auto value = core::R<1, 1, 0>();
};
struct witt1_to_r110 {
  using source = witt1;
  using target = r110;
  static constexpr auto value = core::from_rows(
      {
          {1, 1},
          {1, -1},
      },
      {2, 1});
};

// rigid(3) to itself with its null generator doubled, e4 = 2 e'4: e4 . e4 is
// 0 on both sides, so the change keeps every inner product, and det P = 2.
struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct rga3_doubled {
  using source = rga3;
  using target = rga3;
  static constexpr auto value = core::from_rows(
      {
          {1, 0, 0, 0},
          {0, 1, 0, 0},
          {0, 0, 1, 0},
          {0, 0, 0, 2},
      },
      {1, 1, 1, 1});
};

// Types that are not change tags: an algebra tag, a tag without a target, a
// tag whose value is a metric, and a tag whose value cannot be read in a
// constant expression.
struct no_target {
  using source = cga3;
  [[maybe_unused]] static constexpr auto value = core::conformal_change<3>();
};
struct metric_value {
  using source = cga3;
  using target = r410;
  [[maybe_unused]] static constexpr auto value = core::conformal<3>();
};
struct runtime_value {
  using source = cga3;
  using target = r410;
  static const core::basis_change<5> value;
};
const core::basis_change<5> runtime_value::value{};

// Requires: a change tag names metric_like source and target types and a
// basis_change value usable in a constant expression; anything else is a
// type that the concept refuses rather than an error.
static_assert(core::basis_change_like<cga3_to_r410>);
static_assert(core::basis_change_like<witt1_to_r110>);
static_assert(core::basis_change_like<rga3_doubled>);
static_assert(!core::basis_change_like<cga3>);
static_assert(!core::basis_change_like<no_target>);
static_assert(!core::basis_change_like<metric_value>);
static_assert(!core::basis_change_like<runtime_value>);
static_assert(!core::basis_change_like<int>);

// Guarantees of the value: a structural, trivially copyable aggregate with N
// generators, whose rows and denominators are the arguments of from_rows
// entry by entry, so a row scaled with its denominator is another value of
// the same matrix.
template <core::basis_change<2> C>
struct takes_a_change {
  static constexpr auto value = C;
};
static_assert(std::is_trivially_copyable_v<core::basis_change<5>>);
static_assert(core::basis_change<5>::generator_count == 5);
static_assert(takes_a_change<witt1_to_r110::value>::value ==
              witt1_to_r110::value);
constexpr std::array<std::array<core::entry_type, 2>, 2> witt1_rows{
    {
        {1, 1},
        {1, -1},
    },
};
static_assert(witt1_to_r110::value.rows == witt1_rows);
static_assert(witt1_to_r110::value.denominators ==
              std::array<core::entry_type, 2>{2, 1});
static_assert(core::from_rows({{2, 2}, {1, -1}}, {4, 1}) !=
              witt1_to_r110::value);

// Guarantees of conformal_change<D>(): the rows of 1.4's change, every
// denominator 1 but the origin's, which is 2.
static_assert(core::conformal_change<1>() == core::from_rows(
                                                 {
                                                     {1, 0, 0},
                                                     {0, -1, 1},
                                                     {0, 1, 1},
                                                 },
                                                 {1, 2, 1}));
static_assert(core::conformal_change<3>() == core::from_rows(
                                                 {
                                                     {1, 0, 0, 0, 0},
                                                     {0, 1, 0, 0, 0},
                                                     {0, 0, 1, 0, 0},
                                                     {0, 0, 0, -1, 1},
                                                     {0, 0, 0, 1, 1},
                                                 },
                                                 {1, 1, 1, 2, 1}));

// The coefficient of x on the canonical blade of this mask: the stored
// coefficient times the orientation of the blade in the layout, or zero when
// the layout lacks the blade.
template <class X>
rational value_on(const X& x, core::mask_t mask) {
  for (std::size_t k = 0; k < X::component_count; ++k) {
    const core::oriented_blade blade = X::layout.blades.at(k);
    if (blade.mask == mask) {
      return rational{blade.sign} * x.coefficients.at(k);
    }
  }
  return rational{};
}

// The coefficient of a listed image on the blade of this mask.
template <std::size_t K>
rational listed_on(const std::array<golden::rational_term, K>& terms,
                   std::size_t size, core::mask_t mask) {
  for (std::size_t t = 0; t < size; ++t) {
    if (terms.at(t).blade == mask) {
      const golden::rational& c = terms.at(t).coefficient;
      return rational{c.numerator, c.denominator};
    }
  }
  return rational{};
}

// The number of coefficients, over every blade of the source (or, for the
// inverse, of the target), in which the image of the blade differs from the
// listed one. Each blade is mapped as the full multivector with 1 on that
// blade and 0 elsewhere, through change_basis or change_basis_back, so every
// term of the generated map is read.
template <class C, bool Back, std::size_t K>
int image_mismatches(const std::array<golden::change_image, K>& images) {
  using From = std::conditional_t<Back, typename C::target, typename C::source>;
  constexpr std::size_t n = C::value.generator_count;
  using X = core::mv<From, core::full<n>, rational>;
  int mismatches = 0;
  for (std::size_t a = 0; a < X::component_count; ++a) {
    X x{};
    x.coefficients.at(a) = rational{1};
    const core::mask_t blade = X::layout.blades.at(a).mask;
    const golden::change_image& listed = images.at(blade);
    REQUIRE(listed.blade == blade);
    const auto check = [&](const auto& y) {
      for (core::mask_t b = 0; b < (core::mask_t{1} << n); ++b) {
        if (value_on(y, b) != listed_on(listed.terms, listed.size, b)) {
          ++mismatches;
        }
      }
    };
    if constexpr (Back) {
      check(core::change_basis_back<C>(x));
    } else {
      check(core::change_basis<C>(x));
    }
  }
  return mismatches;
}

// Result layouts: the full layout reaches every blade of the target, in
// canonical order with orientation +1, and the origin e4 alone reaches e'4
// and e'5, which map back onto e4 and e5.
static_assert(core::change_basis_layout<cga3_to_r410, core::full<5>> ==
              core::full<5>);
static_assert(core::change_basis_back_layout<cga3_to_r410, core::full<5>> ==
              core::full<5>);
static_assert(
    core::change_basis_layout<cga3_to_r410, core::blades<cga3, "e4">> ==
    core::blades<r410, "e4 e5">);
static_assert(
    core::change_basis_back_layout<
        cga3_to_r410,
        core::change_basis_layout<cga3_to_r410, core::blades<cga3, "e4">>> ==
    core::blades<cga3, "e4 e5">);
static_assert(
    core::change_basis_layout<cga3_to_r410, core::blades<cga3, "e1 e2 e3">> ==
    core::blades<r410, "e1 e2 e3">);

} // namespace

// Reference: independent value: the change of basis sends e_A to the sum of
//   det P[A, B] e'_B, the minor of P on the rows of A and the columns of B,
//   for conformal_change<D>() from conformal(d) to R(d+1, 1, 0) and for its
//   inverse, d = 1 to 4, on every blade, against the golden images, which
//   the reference scripts compute from the minors of P and again as the
//   wedge of the images of the generators
TEST_CASE("conformal_change maps every blade to its minors, both ways") {
  CHECK(image_mismatches<cga_to_diagonal<1>, false>(
            golden::basis_changes::d1_to_diagonal) == 0);
  CHECK(image_mismatches<cga_to_diagonal<1>, true>(
            golden::basis_changes::d1_from_diagonal) == 0);
  CHECK(image_mismatches<cga_to_diagonal<2>, false>(
            golden::basis_changes::d2_to_diagonal) == 0);
  CHECK(image_mismatches<cga_to_diagonal<2>, true>(
            golden::basis_changes::d2_from_diagonal) == 0);
  CHECK(image_mismatches<cga_to_diagonal<3>, false>(
            golden::basis_changes::d3_to_diagonal) == 0);
  CHECK(image_mismatches<cga_to_diagonal<3>, true>(
            golden::basis_changes::d3_from_diagonal) == 0);
  CHECK(image_mismatches<cga_to_diagonal<4>, false>(
            golden::basis_changes::d4_to_diagonal) == 0);
  CHECK(image_mismatches<cga_to_diagonal<4>, true>(
            golden::basis_changes::d4_from_diagonal) == 0);
}

// Reference: proof: in conformal(3), the origin e4 = (e'5 - e'4) / 2 and
//   the point at infinity e5 = e'4 + e'5 by the definition of the change,
//   e45 goes to the minor of rows 4, 5 and columns 4, 5,
//   (-1/2)(1) - (1/2)(1) = -1, so to -e'45, the antiscalar to det P = -1
//   times the target's, the scalar 1 to 1, and e1 to e'1, and inverting the
//   two null rows gives e'4 = -e4 + e5 / 2 and e'5 = e4 + e5 / 2
TEST_CASE("conformal_change in conformal(3) on hand values") {
  using X = core::mv<cga3, core::full<5>, rational>;
  const auto image = [](core::mask_t blade) {
    X x{};
    for (std::size_t k = 0; k < X::component_count; ++k) {
      if (X::layout.blades.at(k).mask == blade) {
        x.coefficients.at(k) = rational{1};
      }
    }
    return core::change_basis<cga3_to_r410>(x);
  };
  constexpr core::mask_t e4 = 0b01000;
  constexpr core::mask_t e5 = 0b10000;
  constexpr core::mask_t all = 0b11111;

  const auto origin = image(e4);
  CHECK(value_on(origin, e4) == rational{-1, 2});
  CHECK(value_on(origin, e5) == rational{1, 2});
  const auto infinity = image(e5);
  CHECK(value_on(infinity, e4) == rational{1});
  CHECK(value_on(infinity, e5) == rational{1});
  CHECK(value_on(image(e4 | e5), e4 | e5) == rational{-1});
  CHECK(value_on(image(all), all) == rational{-1});
  CHECK(value_on(image(0), 0) == rational{1});
  CHECK(value_on(image(0b00001), 0b00001) == rational{1});

  using Y = core::mv<r410, core::full<5>, rational>;
  Y y{};
  for (std::size_t k = 0; k < Y::component_count; ++k) {
    if (Y::layout.blades.at(k).mask == e4) {
      y.coefficients.at(k) = rational{1};
    }
  }
  const auto back = core::change_basis_back<cga3_to_r410>(y);
  CHECK(value_on(back, e4) == rational{-1});
  CHECK(value_on(back, e5) == rational{1, 2});
}

// Reference: proof: e4 alone maps onto e'4 and e'5 with -1/2 and 1/2, and
//   those map back to e4, with 0 on e5, which truncate drops, and a layout
//   that stores e54 = -e45 gives the image of -e45, which is +e'45 on the
//   canonical e'45
TEST_CASE("the origin's round trip and an oriented layout in conformal(3)") {
  using Origin = core::mv<cga3, core::blades<cga3, "e4">, rational>;
  const Origin origin{rational{1}};
  const auto there = core::change_basis<cga3_to_r410>(origin);
  CHECK(there.coefficients.at(0) == rational{-1, 2});
  CHECK(there.coefficients.at(1) == rational{1, 2});
  const auto back = core::change_basis_back<cga3_to_r410>(there);
  CHECK(back.coefficients.at(0) == rational{1});
  CHECK(back.coefficients.at(1) == rational{});
  CHECK(core::truncate<Origin>(back) == origin);

  using Flipped = core::mv<cga3, core::blades<cga3, "e54">, rational>;
  const auto flipped = core::change_basis<cga3_to_r410>(Flipped{rational{1}});
  CHECK(value_on(flipped, 0b11000) == rational{1});
}

// Reference: proof: rigid(3) with e4 = 2 e'4 sends e4 to 2 e'4 and the
//   antiscalar to det P = 2 times the target's, and its inverse sends e'4 to
//   e4 / 2 and e'1234 to e1234 / 2, and the one-mode Witt change e1 =
//   (e'1 + e'2) / 2, e2 = e'1 - e'2 sends e12 to its determinant
//   (1/2)(-1) - (1/2)(1) = -1 times e'12, and back e'1 = e1 + e2 / 2 and
//   e'2 = e1 - e2 / 2
TEST_CASE("changes with a determinant of 2 and of -1 on hand values") {
  using Point = core::mv<rga3, core::blades<rga3, "e4 e1234">, rational>;
  const Point p{rational{1}, rational{1}};
  const auto doubled = core::change_basis<rga3_doubled>(p);
  CHECK(value_on(doubled, 0b1000) == rational{2});
  CHECK(value_on(doubled, 0b1111) == rational{2});
  const auto halved = core::change_basis_back<rga3_doubled>(p);
  CHECK(value_on(halved, 0b1000) == rational{1, 2});
  CHECK(value_on(halved, 0b1111) == rational{1, 2});

  using Witt = core::mv<witt1, core::full<2>, rational>;
  const auto witt = core::change_basis<witt1_to_r110>(
      Witt{rational{0}, rational{1}, rational{1}, rational{1}});
  CHECK(value_on(witt, 0b01) == rational{3, 2});
  CHECK(value_on(witt, 0b10) == rational{-1, 2});
  CHECK(value_on(witt, 0b11) == rational{-1});
  using Diagonal = core::mv<r110, core::full<2>, rational>;
  const auto back = core::change_basis_back<witt1_to_r110>(
      Diagonal{rational{0}, rational{1}, rational{0}, rational{0}});
  CHECK(value_on(back, 0b01) == rational{1});
  CHECK(value_on(back, 0b10) == rational{1, 2});
  const auto back_second = core::change_basis_back<witt1_to_r110>(
      Diagonal{rational{0}, rational{0}, rational{1}, rational{0}});
  CHECK(value_on(back_second, 0b01) == rational{1});
  CHECK(value_on(back_second, 0b10) == rational{-1, 2});
}
