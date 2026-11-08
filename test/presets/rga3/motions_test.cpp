// Tests of the motions of the rga3 model, galoot/presets/rga3.hpp: rotation(l,
// theta), which builds the rotor (l / |v|) sin(theta/2) + antiscalar
// cos(theta/2), with |v| the length of the line's direction, and
// translation(dx, dy, dz), which builds half of d = (dx, dy, dz) on e23,
// e31, e12 plus the antiscalar. A motor is stored on e41, e42, e43, e1234,
// e23, e31, e12, 1, the order of Lengyel's Motor page
// (rigidgeometricalgebra.org), and moves x to Q ⟇ x ⟇ antireverse(Q).
//
// The constructors take the whole angle and the whole distance, while the
// raw coefficients hold half of each, so each test checks both what the
// constructor stores and how the motor it builds moves points. The exact
// tests use the test rational, and doubles and floats only where every
// value is a small dyadic fraction, so that no product rounds. rotation
// needs std::sin and std::cos, so its exact turns are checked on rotors
// written by their coefficients from rational half-angle sines and cosines,
// and rotation itself is checked against the same formula evaluated in the
// test. Its accuracy as a turn by theta in floating point is checked for a
// quarter turn about the z axis, given with directions of length 1, 2, and
// 5, within a bound that includes the C library's error for sin and cos, and
// its division by the length for a half turn about two lines off the axes,
// against the exact fifths of the unitized lines.
#include <array>
#include <bit>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <format>
#include <limits>
#include <numbers>
#include <type_traits>
#include <utility>

#include <galoot/presets/rga3.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>
#include <support/rational.hpp>

namespace {

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using test_support::rational;

// n / d as T: exact for the test rational, and for float and double when d
// is a power of 2 and n is small, which is all the tests below ask of them.
template <class T>
T ratio(std::int64_t n, std::int64_t d = 1) {
  if constexpr (std::same_as<T, rational>) {
    return rational{n, d};
  } else {
    return static_cast<T>(n) / static_cast<T>(d);
  }
}

// The reference's rationals as the test rational, and its points and lines
// as the model's types over it.
rational scalar(const golden::rational& v) {
  return rational{v.numerator, v.denominator};
}
rga3::Line<rational> line_of(const golden::line& l) {
  return {scalar(l[0]), scalar(l[1]), scalar(l[2]),
          scalar(l[3]), scalar(l[4]), scalar(l[5])};
}

// The point x moved by the rigid motion [R | t]: R (x, y, z) + w t, with
// its weight w unchanged, computed from the matrix's rows.
rga3::Point<rational> moved_by(const golden::affine& m,
                               const rga3::Point<rational>& x) {
  const auto& c = x.coefficients;
  std::array<rational, 3> out{};
  for (std::size_t i = 0; i < 3; ++i) {
    const auto& row = m.at(i);
    out.at(i) = scalar(row[0]) * c[0] + scalar(row[1]) * c[1] +
                scalar(row[2]) * c[2] + scalar(row[3]) * c[3];
  }
  return {out[0], out[1], out[2], c[3]};
}

// The bits of each coefficient of a motor, so that two motors compare equal
// only when they are equal bit for bit, the signs of zeros included.
template <class T>
auto bits_of(const rga3::Motor<T>& q) {
  using U = std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>;
  static_assert(sizeof(U) == sizeof(T));
  std::array<U, 8> out{};
  for (std::size_t i = 0; i < out.size(); ++i) {
    out.at(i) = std::bit_cast<U>(q.coefficients.at(i));
  }
  return out;
}

// Whether rotation and translation accept these arguments, by a qualified
// call, and whether an unqualified call finds them from outside the model's
// namespace, by argument-dependent lookup.
template <class L, class S>
concept can_call_rotation = requires(L l, S s) { rga3::rotation(l, s); };
template <class X, class Y, class Z>
concept can_call_translation =
    requires(X x, Y y, Z z) { rga3::translation(x, y, z); };
template <class L, class S>
concept finds_rotation = requires(L l, S s) { rotation(l, s); };
template <class X, class Y, class Z>
concept finds_translation = requires(X x, Y y, Z z) { translation(x, y, z); };

} // namespace

TYPE_TO_STRING_AS("rational", rational);
TYPE_TO_STRING_AS("float", float);
TYPE_TO_STRING_AS("double", double);

// Reference: independent value: by hand, the translator
//   t_x e23 + t_y e31 + t_z e12 + antiscalar moves points by 2t, so
//   translation(dx, dy, dz) moves points by d = (dx, dy, dz) when it builds
//   half of d on e23, e31, e12 and 1 on e1234: with d = (3, -5, 1/2) it holds
//   0, 0, 0, 1, 3/2, -5/2, 1/4, 0 on e41, e42, e43, e1234, e23, e31, e12, 1,
//   the point (1, 2, 3) of weight 1 goes to (4, -3, 7/2), the point
//   (1/2, -1, 2) of weight 2 moves by 2d to (13/2, -11, 3), a direction,
//   (1, 2, 3, 0) or (-1/4, 0, 5, 0), is unchanged, translation(0, 0, 0) is
//   the identity motor, and Q ⟇ antireverse(Q) is the antiscalar, so the
//   motor is unitized, and every value is a small dyadic fraction, so float and
//   double hold each product exactly
TEST_CASE_TEMPLATE("translation(dx, dy, dz) moves points by "
                   "d = (dx, dy, dz)",
                   T, float, double, rational) {
  using Point = rga3::Point<T>;
  using Motor = rga3::Motor<T>;
  const auto v = [](std::int64_t n, std::int64_t d = 1) {
    return ratio<T>(n, d);
  };
  static_assert(
      std::same_as<decltype(rga3::translation(T{}, T{}, T{})), Motor>);

  const Motor t = rga3::translation(v(3), v(-5), v(1, 2));
  CHECK(t == Motor{v(0), v(0), v(0), v(1), v(3, 2), v(-5, 2), v(1, 4), v(0)});
  CHECK(core::transform(Point{v(1), v(2), v(3), v(1)}, t) ==
        Point{v(4), v(-3), v(7, 2), v(1)});
  CHECK(core::transform(Point{v(1, 2), v(-1), v(2), v(2)}, t) ==
        Point{v(13, 2), v(-11), v(3), v(2)});
  CHECK(core::transform(Point{v(1), v(2), v(3), v(0)}, t) ==
        Point{v(1), v(2), v(3), v(0)});
  CHECK(core::transform(Point{v(-1, 4), v(0), v(5), v(0)}, t) ==
        Point{v(-1, 4), v(0), v(5), v(0)});

  const Motor identity = rga3::translation(v(0), v(0), v(0));
  CHECK(identity == Motor{v(0), v(0), v(0), v(1), v(0), v(0), v(0), v(0)});
  CHECK(core::transform(Point{v(1), v(2), v(3), v(1)}, identity) ==
        Point{v(1), v(2), v(3), v(1)});

  const auto unit = core::geometric_antiproduct(t, core::antireverse(t));
  CHECK(core::get<"e1234">(unit) == v(1));
  std::size_t nonzero = 0;
  for (const T& c : unit.coefficients) {
    if (!(c == v(0))) {
      ++nonzero;
    }
  }
  CHECK(nonzero == 1);
}

// Reference: independent value: by hand, translation(dx, dy, dz) moves
//   points by d = (dx, dy, dz) over the test rational for a d whose halves
//   no binary fraction holds: with d = (1/3, -5/7, 2) it holds 1/6, -5/14, 1
//   on e23, e31, e12 and 1 on e1234, the point (1/2, 0, -1) of weight 1 goes
//   to (5/6, -5/7, 1), the point (1, 2, 3) of weight 3 moves by 3d to
//   (2, -1/7, 9), and the direction (4, -1, 2/3, 0) is unchanged
TEST_CASE("translation(dx, dy, dz) moves points by d = (dx, dy, dz) over the "
          "test rational") {
  using Point = rga3::Point<rational>;
  using Motor = rga3::Motor<rational>;
  const rational o{0};
  const rational one{1};
  const Motor t =
      rga3::translation(rational{1, 3}, rational{-5, 7}, rational{2});
  CHECK(t == Motor{o, o, o, one, rational{1, 6}, rational{-5, 14}, one, o});
  CHECK(core::transform(Point{rational{1, 2}, o, rational{-1}, one}, t) ==
        Point{rational{5, 6}, rational{-5, 7}, one, one});
  CHECK(core::transform(Point{one, rational{2}, rational{3}, rational{3}}, t) ==
        Point{rational{2}, rational{-1, 7}, rational{9}, rational{3}});
  const Point direction{rational{4}, rational{-1}, rational{2, 3}, o};
  CHECK(core::transform(direction, t) == direction);
}

// Reference: independent value: the line p ^ q has direction w_p q - w_q p
//   and moment p x q, computed in the reference by the wedge and by cross
//   products, which agree, for seeded points of several weights;
//   independent value: by hand, translation(dx, dy, dz) moves points by
//   d = (dx, dy, dz), and p and q moved by w_p d and w_q d give the line with
//   the same direction v and the moment
//   (p + w_p d) x (q + w_q d) = p x q + d x v, so a translated rga3 Line
//   keeps its direction and gains d x v on its moment, here with
//   d = (1/3, -5/7, 2)
TEST_CASE("translation(dx, dy, dz) keeps a line's direction and adds d x v "
          "to its moment") {
  using Line = rga3::Line<rational>;
  const std::array<rational, 3> d{rational{1, 3}, rational{-5, 7}, rational{2}};
  const auto t = rga3::translation(d[0], d[1], d[2]);
  for (std::size_t i = 0; i < golden::rigid_lines::joins.size(); ++i) {
    CAPTURE(i);
    const Line l = line_of(golden::rigid_lines::joins.at(i).pq);
    const auto& c = l.coefficients;
    const Line expected{c[0],
                        c[1],
                        c[2],
                        c[3] + (d[1] * c[2] - d[2] * c[1]),
                        c[4] + (d[2] * c[0] - d[0] * c[2]),
                        c[5] + (d[0] * c[1] - d[1] * c[0])};
    CHECK(core::transform(l, t) == expected);
  }
}

// Reference: independent value: the rotor l sin(phi) + antiscalar cos(phi)
//   turns by 2 phi about l, computed in the reference by the antiproduct
//   sandwich and by Rodrigues' formula, which agree, as the right-handed
//   rigid motion [R | t] with cos 2 phi = c^2 - s^2 and sin 2 phi = 2sc, for
//   rational half angles with (s, c) = (3/5, 4/5), (5/13, 12/13), and
//   (4/5, -3/5) about the x, y, and z axes and about lines off the origin,
//   and the rga3 Motor holding l s on e41, e42, e43, e23, e31, e12 and c on
//   e1234, which rotation(l, theta) builds with phi = theta/2, moves each of
//   the points (1, -2, 3) of weight 1, (1/2, 1/3, -1) of weight 2, the
//   origin, and the direction (2, 0, -1, 0) as [R | t] does, by hand
//   R (x, y, z) + w t with the weight w kept
TEST_CASE("an rga3 rotor Motor turns as the reference's right-handed rigid "
          "motions") {
  using Point = rga3::Point<rational>;
  using Motor = rga3::Motor<rational>;
  const rational o{0};
  const std::array<Point, 4> points{
      {
          {rational{1}, rational{-2}, rational{3}, rational{1}},
          {rational{1, 2}, rational{1, 3}, rational{-1}, rational{2}},
          {o, o, o, rational{1}},
          {rational{2}, o, rational{-1}, o},
      },
  };
  std::size_t off_origin = 0;
  for (std::size_t i = 0; i < golden::rigid_rotations::rotations.size(); ++i) {
    CAPTURE(i);
    const golden::rotation_case& r = golden::rigid_rotations::rotations.at(i);
    const rga3::Line<rational> axis = line_of(r.axis);
    const auto& l = axis.coefficients;
    const rational s = scalar(r.sin);
    const rational c = scalar(r.cos);
    const Motor rotor{l[0] * s, l[1] * s, l[2] * s, c,
                      l[3] * s, l[4] * s, l[5] * s, o};
    if (!(l[3] == o && l[4] == o && l[5] == o)) {
      ++off_origin;
    }
    for (std::size_t k = 0; k < points.size(); ++k) {
      CAPTURE(k);
      CHECK(core::transform(points.at(k), rotor) ==
            moved_by(r.motion, points.at(k)));
    }
  }
  CHECK(off_origin > 0);
}

// Reference: contract: rotation(l, theta) builds (l / |v|) sin(theta/2) +
//   antiscalar cos(theta/2), with |v| the length of l's direction v, the
//   rotor l sin(phi) + antiscalar cos(phi) of the unitized line with
//   phi = theta/2, which turns by 2 phi about l, so rotation(l, theta) turns
//   by theta: over float and double, with theta of the line's scalar type,
//   it equals bit for bit the Motor holding l_i (std::sin(theta/2) / |v|) on
//   e41, e42, e43, e23, e31, e12, with |v| = std::sqrt((v_x v_x + v_y v_y) +
//   v_z v_z), std::cos(theta/2) on e1234, and 0 on 1, in the order of the
//   motor's layout, for axes along x, y, and z, lines off the origin,
//   directions of length 1, 2, 5, and 5/2, angles of either sign, zero, and
//   beyond a full turn, which pins the half angle, which of sine and cosine
//   goes where, the order, and the division of all six coefficients by the
//   direction's length, while the expected motor uses the same std::sin,
//   std::cos, and std::sqrt, so this verifies the construction, not the
//   accuracy of the C library
TEST_CASE_TEMPLATE("rotation(l, theta) builds (l / |v|) sin(theta/2) + "
                   "antiscalar cos(theta/2) bit for bit",
                   T, float, double) {
  using Line = rga3::Line<T>;
  using Motor = rga3::Motor<T>;
  static_assert(std::same_as<decltype(rga3::rotation(Line{}, T{})), Motor>);

  constexpr std::array<std::array<double, 6>, 10> axes{
      {
          {1, 0, 0, 0, 0, 0},
          {0, 1, 0, 0, 0, 0},
          {0, 0, 1, 0, 0, 0},
          {0, 0, 1, 0, -1, 0},
          {0.6, 0.8, 0, 0, 0, 1.5},
          {0, -0.6, -0.8, 0, 1.6, -1.2},
          {0, 0, 2, 0, -2, 0},
          {0, 0, 5, 0, -5, 0},
          {0, 3, 4, 0, 4, -3},
          {1.5, -2, 0, 2, 1.5, 0.5},
      },
  };
  constexpr std::array<double, 8> angles{
      0.0, 0.5, -1.25, 3.0, 7.5, -100.0, 0.001, std::numbers::pi,
  };
  for (std::size_t a = 0; a < axes.size(); ++a) {
    CAPTURE(a);
    std::array<T, 6> v{};
    for (std::size_t i = 0; i < v.size(); ++i) {
      v.at(i) = static_cast<T>(axes.at(a).at(i));
    }
    const Line l{v[0], v[1], v[2], v[3], v[4], v[5]};
    for (const double angle : angles) {
      const T theta = static_cast<T>(angle);
      INFO(std::format("theta = {}", theta));
      const T length =
          std::sqrt(((v[0] * v[0]) + (v[1] * v[1])) + (v[2] * v[2]));
      const T s = std::sin(theta / T{2}) / length;
      const T c = std::cos(theta / T{2});
      const Motor expected{
          v[0] * s, v[1] * s, v[2] * s, c, v[3] * s, v[4] * s, v[5] * s, T{0},
      };
      CHECK(bits_of(rga3::rotation(l, theta)) == bits_of(expected));
    }
  }
}

// Reference: independent value: by hand, the join p ^ q of points of weight
//   1 has direction q - p and moment p x q, so the join of (0, 0, 0) and
//   (0, 0, 2) is the z axis (0, 0, 2, 0, 0, 0) with a direction of length 2,
//   that of (1, 0, 0) and (1, 0, 4) the line (0, 0, 4, 0, -4, 0) through
//   (1, 0, 0) along z, of length 4, and that of (1, 0, 0) and (1, 0, 5) the
//   line (0, 0, 5, 0, -5, 0), of length 5; contract: rotation(l, theta) turns
//   by theta about a line whose direction has any positive length, by
//   building the rotor of l divided by the length of its direction, so each
//   gives the motor of the unitized line, its six coefficients divided by
//   the length, (0, 0, 1, 0, 0, 0) or (0, 0, 1, 0, -1, 0) with the signs of
//   the join's zeros kept, and over float and double, dividing by a power of 2
//   and multiplying back is exact, so the lengths 2 and 4 give that motor
//   bit for bit, while for the length 5 the sine on e43 and e31 is
//   5 fl(fl(sin(theta/2)) / 5), within u of fl(sin(theta/2)) when its
//   magnitude is in [1/2, 1), u the unit roundoff, and every other
//   coefficient is the unitized line's bit for bit
TEST_CASE_TEMPLATE("rotation(l, theta) about a line whose direction has "
                   "length 2, 4, or 5 builds the motor of the unitized line",
                   T, float, double) {
  using Point = rga3::Point<T>;
  using Line = rga3::Line<T>;
  using Motor = rga3::Motor<T>;
  constexpr T u = std::numeric_limits<T>::epsilon() / T{2};
  const Point origin{T{0}, T{0}, T{0}, T{1}};
  const Point on_x{T{1}, T{0}, T{0}, T{1}};
  const Line z_axis_2{origin ^ Point{T{0}, T{0}, T{2}, T{1}}};
  const Line off_axis_4{on_x ^ Point{T{1}, T{0}, T{4}, T{1}}};
  const Line off_axis_5{on_x ^ Point{T{1}, T{0}, T{5}, T{1}}};
  REQUIRE(z_axis_2 == Line{T{0}, T{0}, T{2}, T{0}, T{0}, T{0}});
  REQUIRE(off_axis_4 == Line{T{0}, T{0}, T{4}, T{0}, T{-4}, T{0}});
  REQUIRE(off_axis_5 == Line{T{0}, T{0}, T{5}, T{0}, T{-5}, T{0}});

  // Each line divided by its length, exactly: the zeros keep their signs.
  const auto unitized = [](const Line& l, T length) {
    const auto& c = l.coefficients;
    return Line{
        c[0] / length, c[1] / length, c[2] / length,
        c[3] / length, c[4] / length, c[5] / length,
    };
  };
  const Line z_axis = unitized(z_axis_2, T{2});
  const Line off_axis = unitized(off_axis_4, T{4});
  const Line off_axis_1 = unitized(off_axis_5, T{5});
  REQUIRE(z_axis == Line{T{0}, T{0}, T{1}, T{0}, T{0}, T{0}});
  REQUIRE(off_axis == Line{T{0}, T{0}, T{1}, T{0}, T{-1}, T{0}});
  REQUIRE(off_axis_1 == off_axis);

  // Angles whose half has a sine of magnitude in [1/2, 1).
  constexpr std::array<double, 6> angles{
      std::numbers::pi / 2, 1.2, 2.0, 3.0, -1.25, -2.5,
  };
  for (const double angle : angles) {
    const T theta = static_cast<T>(angle);
    INFO(std::format("theta = {}", theta));
    CHECK(bits_of(rga3::rotation(z_axis_2, theta)) ==
          bits_of(rga3::rotation(z_axis, theta)));
    CHECK(bits_of(rga3::rotation(off_axis_4, theta)) ==
          bits_of(rga3::rotation(off_axis, theta)));

    // The length 5: the sine on e43 and e31 within u, the rest bit for bit.
    const Motor unit = rga3::rotation(off_axis_1, theta);
    const Motor five = rga3::rotation(off_axis_5, theta);
    const T sine = unit.coefficients[2];
    INFO(std::format("sine on e43 = {}", sine));
    REQUIRE(std::abs(sine) >= T{1} / T{2});
    REQUIRE(std::abs(sine) < T{1});
    const auto unit_bits = bits_of(unit);
    const auto five_bits = bits_of(five);
    for (std::size_t i = 0; i < unit_bits.size(); ++i) {
      CAPTURE(i);
      if (i == 2 || i == 5) {
        const T difference =
            std::abs(five.coefficients.at(i) - unit.coefficients.at(i));
        INFO(std::format("difference = {}", difference));
        CHECK(difference <= u);
      } else {
        CHECK(five_bits.at(i) == unit_bits.at(i));
      }
    }
  }
}

// The tolerance of each coefficient of a quarter turn of (1, 0, 0, 1) about
// the z axis by rotation(l, theta), in units of u = epsilon / 2, the unit
// roundoff of float or double. With phi = theta / 2, the motor holds
// s^ = fl(sin phi) on e43 and c^ = fl(cos phi) on e1234; their errors are
// |a|, |b| <= k u with k = 9, the error in ulps above which the GNU C
// Library's test suite reports sin and cos as wrong (its manual, "Errors in
// Math Functions"; a stated ceiling, not a proved bound), and an ulp of a
// value in [1/2, 1) is u. Then, with s = sin phi and c = cos phi,
//   E1 = |a| (2|s| + |a|) + |b| (2|c| + |b|)
//      bounds the errors of c^2 - s^2 against cos theta and of c^2 + s^2
//      against 1, the motor's lack of unit norm;
//   E2 = 2 (|s| |b| + |c| |a| + |a| |b|) bounds that of 2 s^ c^ against
//      sin theta;
// and since s + c <= sqrt 2, E1, E2 <= 2 sqrt(2) k u + 2 k^2 u^2. transform's
// forward error bound is gamma_K = K u / (1 - K u) times the magnitude of
// each coefficient, at most 1 + E1 here, with K = 12 roundings on e1, e2, e3
// and 5 on e4. The angle passed is theta = fl(pi/2), and with
// delta = |pi/2 - theta|, |cos theta| <= delta and 1 - sin theta <=
// delta^2 / 2. So the result differs from (0, 1, 0, 1) by at most
//   tol_0 = gamma_12 (1 + E1) + E1 + delta        on e1
//   tol_1 = gamma_12 (1 + E1) + E2 + delta^2 / 2  on e2
//   tol_2 = 0                                     on e3, whose terms are all 0
//   tol_3 = gamma_5 (1 + E1) + E1                 on e4
// which are 38.01, 37.46, 0, and 30.46 u in double (delta = 0.5515 u) and
// 38.19, 37.46, 0, and 30.46 u in float (delta = 0.7334 u), rounded up to
// the integers below. The comparisons add no rounding: the differences from
// 0 are exact, those from 1 exact by Sterbenz's lemma, since the
// coefficients lie in [1/2, 2], and an integer times u is exact.
namespace {
constexpr std::array<int, 4> quarter_turn_tolerance_in_u{39, 38, 0, 31};
} // namespace

// Reference: independent value: by hand, a quarter turn counterclockwise
//   about the z axis, seen from +z, takes (1, 0, 0) to (0, 1, 0), so
//   rotation(l, theta) with l the z axis, 1 on e43, and theta = pi/2 moves
//   the point (1, 0, 0, 1) to (0, 1, 0, 1) over float and double, each
//   coefficient within its tolerance: the error bound of transform for the
//   motor it is given, gamma_K = K u / (1 - K u) times each coefficient's
//   magnitude, with K = 12 on e1, e2, e3 and 5 on e4, plus the error of the
//   motor's sine and cosine at k = 9 ulps, the ceiling the GNU C Library
//   states for its sin and cos, and the gap between pi/2 and the angle that
//   float or double holds: 39 u, 38 u, 0, and 31 u on e1, e2, e3, e4, for
//   the unit roundoff u, fixed before the test was first run
TEST_CASE_TEMPLATE("rotation(l, pi/2) about the z axis turns (1, 0, 0, 1) to "
                   "(0, 1, 0, 1) within its error bound",
                   T, float, double) {
  using Point = rga3::Point<T>;
  using Line = rga3::Line<T>;
  constexpr T u = std::numeric_limits<T>::epsilon() / T{2};
  const T theta = std::numbers::pi_v<T> / T{2};
  INFO(std::format("theta = {}", theta));
  const Line z_axis{T{0}, T{0}, T{1}, T{0}, T{0}, T{0}};
  const Point moved = core::transform(Point{T{1}, T{0}, T{0}, T{1}},
                                      rga3::rotation(z_axis, theta));
  constexpr std::array<T, 4> expected{T{0}, T{1}, T{0}, T{1}};
  for (std::size_t j = 0; j < expected.size(); ++j) {
    CAPTURE(j);
    const T coefficient = moved.coefficients.at(j);
    const T tolerance = static_cast<T>(quarter_turn_tolerance_in_u.at(j)) * u;
    INFO(std::format("coefficient = {}, tolerance = {}", coefficient,
                     tolerance));
    CHECK(std::abs(coefficient - expected.at(j)) <= tolerance);
  }
}

// The tolerance of the same quarter turn about the z axis given with a
// direction of length 5, (0, 0, 5, 0, 0, 0). rotation divides the stored
// sine by the length: 25 and its square root are exact, and the sine it
// stores is s' = fl(5 q) with q = fl(s^ / 5). For |s^| in [1/2, 1), q lies
// in [1/16, 1/4), where float and double values are spaced at most u / 4,
// so |5 q - s^| <= 5u / 8 and 5 q < 1. Rounding 5 q then moves it by at
// most u / 2 when 5 q >= 1/2 and u / 4 below, and s' and s^ are multiples
// of u in [1/2, 1] or of u / 2 below, so |s' - s^| <= u, as an exhaustive
// run over every float in [1/2, 1) also finds. The sine's error is then
// |a| <= (k + 1) u in place of k u, the cosine's |b| <= k u as before, and
// the same derivation gives 39.42, 38.87, 0, and 31.87 u in double and
// 39.60, 38.87, 0, and 31.87 u in float, rounded up below. A direction of
// length 2 gives the unit axis's motor bit for bit, since dividing by 2 and
// multiplying back is exact, so it keeps the tolerances above.
namespace {
constexpr std::array<int, 4> quarter_turn_length_5_tolerance_in_u{
    40,
    39,
    0,
    32,
};
} // namespace

// Reference: independent value: by hand, a quarter turn counterclockwise
//   about the z axis, seen from +z, takes (1, 0, 0) to (0, 1, 0), whatever
//   the length of the direction the axis is given with, since rotation(l,
//   theta) turns by theta about a line whose direction has any positive
//   length, so rotation(l, theta) with l the join of (0, 0, 0) and
//   (0, 0, 2), direction (0, 0, 2), or the line (0, 0, 5, 0, 0, 0), and
//   theta = pi/2, moves the point (1, 0, 0, 1) to (0, 1, 0, 1) over float
//   and double, each coefficient within its tolerance: for the length 2,
//   that of the unit axis, 39 u, 38 u, 0, and 31 u on e1, e2, e3, e4, and
//   for the length 5, with the error of dividing the sine by 5 and
//   multiplying it back, 40 u, 39 u, 0, and 32 u in float and double, for
//   the unit roundoff u, fixed before the test first ran with them
TEST_CASE_TEMPLATE("rotation(l, pi/2) about a z axis of length 2 or 5 turns "
                   "(1, 0, 0, 1) to (0, 1, 0, 1) within its error bound",
                   T, float, double) {
  using Point = rga3::Point<T>;
  using Line = rga3::Line<T>;
  constexpr T u = std::numeric_limits<T>::epsilon() / T{2};
  const T theta = std::numbers::pi_v<T> / T{2};
  INFO(std::format("theta = {}", theta));
  const Point origin{T{0}, T{0}, T{0}, T{1}};
  const Line z_axis_2{origin ^ Point{T{0}, T{0}, T{2}, T{1}}};
  const Line z_axis_5{T{0}, T{0}, T{5}, T{0}, T{0}, T{0}};
  REQUIRE(z_axis_2 == Line{T{0}, T{0}, T{2}, T{0}, T{0}, T{0}});

  const std::array<std::pair<Line, std::array<int, 4>>, 2> cases{
      {
          {z_axis_2, quarter_turn_tolerance_in_u},
          {z_axis_5, quarter_turn_length_5_tolerance_in_u},
      },
  };
  constexpr std::array<T, 4> expected{T{0}, T{1}, T{0}, T{1}};
  for (std::size_t c = 0; c < cases.size(); ++c) {
    CAPTURE(c);
    const auto& [axis, tolerance_in_u] = cases.at(c);
    const Point moved = core::transform(Point{T{1}, T{0}, T{0}, T{1}},
                                        rga3::rotation(axis, theta));
    for (std::size_t j = 0; j < expected.size(); ++j) {
      CAPTURE(j);
      const T coefficient = moved.coefficients.at(j);
      const T tolerance = static_cast<T>(tolerance_in_u.at(j)) * u;
      INFO(std::format("coefficient = {}, tolerance = {}", coefficient,
                       tolerance));
      CHECK(std::abs(coefficient - expected.at(j)) <= tolerance);
    }
  }
}

// The tolerance of each line coefficient of a half turn, rotation(l, fl(pi)),
// about a line whose direction has length 5 or 5/2, against the exact fifths
// of the unitized line, in units of the unit roundoff u and indexed by the
// numerator's magnitude n = 0 to 4. Both lengths are computed exactly, the
// half of fl(pi) is exact, and sin(fl(pi) / 2) = cos(delta / 2) with
// delta = |pi - fl(pi)|, within delta^2 / 8 < 1e-7 u of 1, so the library's
// sine is within a = k u + delta^2 / 8 of 1, with k = 9 ulps as for the
// quarter turn. A coefficient is v_i fl(s^ / |v|) rounded, which is
// (n / 5) s^ (1 + e1)(1 + e2) with |e1|, |e2| <= u, within
// (n / 5) ((1 + a)(1 + u)^2 - 1) < (n / 5) 11.01 u of n / 5. The expected
// value n / 5 as T is one correctly rounded division, within half an ulp:
// u / 8, u / 4, u / 2, and u / 2 for n = 1 to 4. The difference of the two is
// exact by Sterbenz's lemma, and a zero coefficient is 0 times the quotient,
// a zero. The bounds 0, 2.33, 4.65, 7.11, and 9.31 u are rounded up below.
namespace {
constexpr std::array<int, 5> half_turn_fifths_tolerance_in_u{0, 3, 5, 8, 10};
} // namespace

// Reference: independent value: by hand, the line through (1, 0, 0) with
//   direction (0, 3, 4) has moment (1, 0, 0) x (0, 3, 4) = (0, -4, 3), so it
//   is (0, 3, 4, 0, -4, 3), and (1.5, -2, 0, 2, 1.5, 1/2) is a line, since
//   v . m = 3 - 3 + 0 = 0, and their directions have lengths 5 and 5/2, so
//   divided by them they are (0, 3, 4, 0, -4, 3) / 5 and
//   (3, -4, 0, 4, 3, 1) / 5; contract: rotation(l, theta) builds
//   (l / |v|) sin(theta/2) + antiscalar cos(theta/2), and sin(theta/2) is 1
//   for a half turn, so rotation(l, fl(pi)) holds those fifths on e41, e42,
//   e43, e23, e31, e12 over float and double, each within its tolerance from
//   the error of the C library's sine at k = 9 ulps and the roundings of the
//   division and the products, 0, 3, 5, 8, or 10 u for a numerator of
//   magnitude 0 to 4, and 0 on 1, for the unit roundoff u, fixed before the
//   test was first run
TEST_CASE_TEMPLATE("rotation(l, pi) about a line off the axes whose direction "
                   "has length 5 or 5/2 holds the unitized line's fifths",
                   T, float, double) {
  using Line = rga3::Line<T>;
  constexpr T u = std::numeric_limits<T>::epsilon() / T{2};
  const T theta = std::numbers::pi_v<T>;
  INFO(std::format("theta = {}", theta));

  // Each line, and the numerators of its unitized coefficients over 5.
  const std::array<std::pair<Line, std::array<int, 6>>, 2> cases{
      {
          {Line{T{0}, T{3}, T{4}, T{0}, T{-4}, T{3}}, {0, 3, 4, 0, -4, 3}},
          {
              Line{T{1.5}, T{-2}, T{0}, T{2}, T{1.5}, T{0.5}},
              {3, -4, 0, 4, 3, 1},
          },
      },
  };
  for (std::size_t c = 0; c < cases.size(); ++c) {
    CAPTURE(c);
    const auto& [axis, fifths] = cases.at(c);
    const rga3::Motor<T> q = rga3::rotation(axis, theta);
    // The line blades in the motor's order: e41, e42, e43, then e23, e31,
    // e12 after e1234.
    constexpr std::array<std::size_t, 6> line_places{0, 1, 2, 4, 5, 6};
    for (std::size_t i = 0; i < fifths.size(); ++i) {
      CAPTURE(i);
      const int n = fifths.at(i);
      const T expected = static_cast<T>(n) / T{5};
      const T coefficient = q.coefficients.at(line_places.at(i));
      const T tolerance = static_cast<T>(half_turn_fifths_tolerance_in_u.at(
                              static_cast<std::size_t>(std::abs(n)))) *
                          u;
      INFO(std::format("coefficient = {}, expected = {}, tolerance = {}",
                       coefficient, expected, tolerance));
      CHECK(std::abs(coefficient - expected) <= tolerance);
    }
    CHECK(core::get<"1">(q) == T{0});
  }
}

// Reference: contract: rotation takes an rga3 Line over a floating scalar
//   type and an angle of that same type, and translation three scalars of a
//   type that is not a built-in integer, each returning a Motor of that
//   scalar type, a rotation over an integer or exact scalar, about an axis
//   that is not a Line, or with an angle of another type, and a translation
//   over integers or over scalars of two types, each reach a function whose
//   only effect is its error message, so a requires-expression that asks
//   whether such a call is valid is told yes, as the library documents,
//   while a number is no axis and a pointer is no scalar of a translation,
//   and rotation is found by an unqualified call, since its Line argument
//   names the model's namespace, and translation, whose arguments are plain
//   scalars, only by a qualified one
TEST_CASE("the rotation and translation overloads take the right arguments") {
  using std::int64_t;
  static_assert(can_call_rotation<rga3::Line<double>, double>);
  static_assert(can_call_rotation<rga3::Line<float>, float>);
  static_assert(
      std::same_as<decltype(rga3::rotation(rga3::Line<float>{}, float{})),
                   rga3::Motor<float>>);
  static_assert(can_call_rotation<rga3::Line<int64_t>, int64_t>);
  static_assert(can_call_rotation<rga3::Line<rational>, rational>);
  static_assert(can_call_rotation<rga3::Point<double>, double>);
  static_assert(can_call_rotation<rga3::Line<double>, int>);
  static_assert(can_call_rotation<rga3::Line<float>, double>);
  static_assert(!can_call_rotation<double, double>);

  static_assert(can_call_translation<double, double, double>);
  static_assert(can_call_translation<rational, rational, rational>);
  static_assert(std::same_as<decltype(rga3::translation(rational{}, rational{},
                                                        rational{})),
                             rga3::Motor<rational>>);
  static_assert(can_call_translation<int, int, int>);
  static_assert(can_call_translation<int64_t, int64_t, int64_t>);
  static_assert(can_call_translation<double, int, double>);
  static_assert(!can_call_translation<const char*, const char*, const char*>);
  static_assert(!can_call_translation<const char*, int, int>);

  static_assert(finds_rotation<rga3::Line<double>, double>);
  static_assert(!finds_translation<double, double, double>);
  CHECK(rotation(rga3::Line<double>{0, 0, 1, 0, 0, 0}, 0.0) ==
        rga3::Motor<double>{0, 0, 0, 1, 0, 0, 0, 0});
}
