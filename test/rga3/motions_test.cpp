// Tests of the motions of the rga3 model, galoot/rga3.hpp: rotation(l,
// theta), which builds the rotor l sin(theta/2) + antiscalar cos(theta/2),
// and translation(dx, dy, dz), which builds half of d = (dx, dy, dz) on e23,
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
// test. Its accuracy as a turn by theta in floating point is not tested
// here: that needs a bound that includes the C library's error for sin and
// cos.
#include <array>
#include <bit>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <format>
#include <type_traits>

#include <galoot/rga3.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>
#include <support/rational.hpp>

namespace {

namespace core = galoot::core;
namespace rga3 = galoot::rga3;
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
  const std::array<Point, 4> points{{
      {rational{1}, rational{-2}, rational{3}, rational{1}},
      {rational{1, 2}, rational{1, 3}, rational{-1}, rational{2}},
      {o, o, o, rational{1}},
      {rational{2}, o, rational{-1}, o},
  }};
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

// Reference: contract: rotation(l, theta) builds l sin(theta/2) + antiscalar
//   cos(theta/2), the rotor l sin(phi) + antiscalar cos(phi) with
//   phi = theta/2, which turns by 2 phi about l, so rotation(l, theta) turns
//   by theta: over float and double, with theta of the line's scalar type,
//   it equals bit for bit the Motor holding l_i std::sin(theta/2) on e41,
//   e42, e43, e23, e31, e12, std::cos(theta/2) on e1234, and 0 on 1, in the
//   order of the motor's layout, for axes along x, y, and z, lines off the
//   origin, angles of either sign, zero, and beyond a full turn, which pins
//   the half angle, which of sine and cosine goes where, and the order,
//   while the expected motor uses the same std::sin and std::cos, so this
//   verifies the construction, not the accuracy of the C library
TEST_CASE_TEMPLATE("rotation(l, theta) builds l sin(theta/2) + antiscalar "
                   "cos(theta/2) bit for bit",
                   T, float, double) {
  using Line = rga3::Line<T>;
  using Motor = rga3::Motor<T>;
  static_assert(std::same_as<decltype(rga3::rotation(Line{}, T{})), Motor>);

  constexpr std::array<std::array<double, 6>, 6> axes{{
      {1, 0, 0, 0, 0, 0},
      {0, 1, 0, 0, 0, 0},
      {0, 0, 1, 0, 0, 0},
      {0, 0, 1, 0, -1, 0},
      {0.6, 0.8, 0, 0, 0, 1.5},
      {0, -0.6, -0.8, 0, 1.6, -1.2},
  }};
  constexpr std::array<double, 8> angles{0.0, 0.5,    -1.25, 3.0,
                                         7.5, -100.0, 0.001, 3.14159265358979};
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
      const T s = std::sin(theta / T{2});
      const T c = std::cos(theta / T{2});
      const Motor expected{v[0] * s, v[1] * s, v[2] * s, c,
                           v[3] * s, v[4] * s, v[5] * s, T{0}};
      CHECK(bits_of(rga3::rotation(l, theta)) == bits_of(expected));
    }
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
  CHECK(rotation(rga3::Line<double>{}, 0.0) ==
        rga3::Motor<double>{0, 0, 0, 1, 0, 0, 0, 0});
}
