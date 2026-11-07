// Tests of the objects of the rga3 model, galoot/rga3.hpp: the algebra tag,
// the layouts of a point, line, plane, motor, and flector, the alias
// templates that name their types, and the facts of rigid(3) that those
// types carry: the weight of a point, the direction and moment of a line,
// the normal and offset of a plane, and how motors and flectors move them.
//
// rigid(3) is R(3, 0, 1): e1, e2, e3 square to 1 and e4 to 0. Lengyel's
// rigid geometric algebra (rigidgeometricalgebra.org) names each blade by
// its generators in the order of their product, so e41 = -e14, e31 = -e13,
// e423 = +e234, e431 = -e134, e412 = +e124, and e321 = -e123. The expected
// layouts are written here from those names as masks and signs, never read
// from the model, and the expected values are worked out by hand from the
// definitions: a point (x, y, z, w) on e1, e2, e3, e4; a line with its
// direction on e41, e42, e43 and its moment on e23, e31, e12; a plane with
// its normal on e423, e431, e412 and its offset on e321; a motor on e41,
// e42, e43, e1234, e23, e31, e12, 1, and a flector on e1, e2, e3, e4, e423,
// e431, e412, e321, the orders of Lengyel's Motor and Flector pages. A motor
// Q moves x to Q ⟇ x ⟇ antireverse(Q), and a flector F to
// -F ⟇ x^ ⟇ antireverse(F), where x^ is the grade involution of x.
#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>

#include <galoot/rga3.hpp>

#include <doctest/doctest.h>

#include <reference/golden.hpp>
#include <support/rational.hpp>

namespace {

namespace core = galoot::core;
namespace rga3 = galoot::rga3;
using test_support::rational;

// An oriented blade of rigid(3) with the name it has in Lengyel's notation.
struct named_blade {
  std::string_view name;
  core::oriented_blade blade;
};

// The layouts of the five objects, each blade as the mask of its generators
// (bit i - 1 for e_i) and the sign that takes the canonical blade, whose
// indices increase, to the named one: e41 = e4 e1 = -e1 e4.
inline constexpr std::array<named_blade, 4> point_names{{
    {"e1", {0b0001, 1}},
    {"e2", {0b0010, 1}},
    {"e3", {0b0100, 1}},
    {"e4", {0b1000, 1}},
}};
inline constexpr std::array<named_blade, 6> line_names{{
    {"e41", {0b1001, -1}},
    {"e42", {0b1010, -1}},
    {"e43", {0b1100, -1}},
    {"e23", {0b0110, 1}},
    {"e31", {0b0101, -1}},
    {"e12", {0b0011, 1}},
}};
inline constexpr std::array<named_blade, 4> plane_names{{
    {"e423", {0b1110, 1}},
    {"e431", {0b1101, -1}},
    {"e412", {0b1011, 1}},
    {"e321", {0b0111, -1}},
}};
inline constexpr std::array<named_blade, 8> motor_names{{
    {"e41", {0b1001, -1}},
    {"e42", {0b1010, -1}},
    {"e43", {0b1100, -1}},
    {"e1234", {0b1111, 1}},
    {"e23", {0b0110, 1}},
    {"e31", {0b0101, -1}},
    {"e12", {0b0011, 1}},
    {"1", {0b0000, 1}},
}};
inline constexpr std::array<named_blade, 8> flector_names{{
    {"e1", {0b0001, 1}},
    {"e2", {0b0010, 1}},
    {"e3", {0b0100, 1}},
    {"e4", {0b1000, 1}},
    {"e423", {0b1110, 1}},
    {"e431", {0b1101, -1}},
    {"e412", {0b1011, 1}},
    {"e321", {0b0111, -1}},
}};

// The blade set of a list of named blades, in the order listed.
template <std::size_t K>
constexpr core::blade_set<K> set_of(const std::array<named_blade, K>& names) {
  core::blade_set<K> set{};
  for (std::size_t i = 0; i < K; ++i) {
    set.blades.at(i) = names.at(i).blade;
  }
  return set;
}

// The blade that the reference's table of oriented names gives a name, or
// a blade with sign 0, which no layout holds, when the table lacks it.
template <std::size_t N>
core::oriented_blade
blade_named(const std::array<golden::oriented_name, N>& table,
            std::string_view name) {
  for (const golden::oriented_name& entry : table) {
    if (entry.name == name) {
      return {entry.blade.blade, entry.blade.sign};
    }
  }
  return {0, 0};
}

// Whether every blade of the layout of X has a grade of this parity, 0 for
// even and 1 for odd.
template <class X>
constexpr bool all_of_parity(unsigned parity) {
  for (const core::oriented_blade& blade : X::layout.blades) {
    if (static_cast<unsigned>(std::popcount(blade.mask)) % 2 != parity) {
      return false;
    }
  }
  return true;
}

// A value used as a template argument, read back.
template <auto Value>
constexpr auto as_template_argument = Value;

// The reference's rationals as the test rational, and its points and lines
// as the model's types over it.
rational scalar(const golden::rational& v) {
  return rational{v.numerator, v.denominator};
}
rga3::Point<rational> point_of(const golden::point& p) {
  return {scalar(p[0]), scalar(p[1]), scalar(p[2]), scalar(p[3])};
}
rga3::Line<rational> line_of(const golden::line& l) {
  return {scalar(l[0]), scalar(l[1]), scalar(l[2]),
          scalar(l[3]), scalar(l[4]), scalar(l[5])};
}

} // namespace

TYPE_TO_STRING_AS("std::int64_t", std::int64_t);
TYPE_TO_STRING_AS("rational", rational);
TYPE_TO_STRING_AS("float", float);
TYPE_TO_STRING_AS("double", double);

// Reference: independent value: by hand from the oriented names of
//   Lengyel's rigid geometric algebra, the rga3 layouts hold the oriented
//   blades of their names: a point on e1, e2, e3, e4, a line on e41 = -e14,
//   e42 = -e24, e43 = -e34, e23, e31 = -e13, e12, a plane on e423 = +e234,
//   e431 = -e134, e412 = +e124, e321 = -e123, a motor is stored on e41,
//   e42, e43, e1234, e23, e31, e12, 1 in that order, a flector is stored on
//   e1, e2, e3, e4, e423, e431, e412, e321 in that order, each set differs
//   from the same masks with the canonical orientations, and the motor's from
//   the grade order 1, e41, e42, e43, e23, e31, e12, e1234;
//   independent value: an oriented name lists generators in the order of
//   their product, so the reference's table of oriented names gives each
//   name the mask and sign written here
TEST_CASE("the rga3 layouts hold the oriented blades of their names") {
  static_assert(rga3::point_layout == set_of(point_names));
  static_assert(rga3::line_layout == set_of(line_names));
  static_assert(rga3::plane_layout == set_of(plane_names));
  static_assert(rga3::motor_layout == set_of(motor_names));
  static_assert(rga3::flector_layout == set_of(flector_names));

  static_assert(rga3::line_layout !=
                core::blades<rga3::algebra, "e14 e24 e34 e23 e13 e12">);
  static_assert(rga3::plane_layout !=
                core::blades<rga3::algebra, "e234 e134 e124 e123">);
  static_assert(rga3::motor_layout !=
                core::blades<rga3::algebra, "e14 e24 e34 e1234 e23 e13 e12 1">);
  static_assert(rga3::motor_layout !=
                core::blades<rga3::algebra, "1 e41 e42 e43 e23 e31 e12 e1234">);
  static_assert(rga3::flector_layout !=
                core::blades<rga3::algebra, "e1 e2 e3 e4 e234 e134 e124 e123">);

  const auto check =
      [&]<std::size_t K>(const std::array<named_blade, K>& names) {
        for (const named_blade& named : names) {
          CAPTURE(named.name);
          CHECK(blade_named(golden::oriented_names::rigid_3, named.name) ==
                named.blade);
        }
      };
  check(point_names);
  check(line_names);
  check(plane_names);
  check(motor_names);
  check(flector_names);
}

// Reference: contract: the algebra tag of the model holds rigid(3), which
//   is R(3, 0, 1), and the types Point<T>, Line<T>, Plane<T>, Motor<T>, and
//   Flector<T> are the multivectors of that algebra on the five layouts with
//   the scalar T given, which has no default, so Point<float> and
//   Point<double> are distinct types that both name their scalar and their
//   algebra
TEST_CASE("the rga3 types are multivectors of rigid(3) on the model's "
          "layouts") {
  static_assert(rga3::algebra::value == core::rigid<3>());
  static_assert(rga3::algebra::value == core::R<3, 0, 1>());
  static_assert(rga3::algebra::value != core::R<4, 0, 0>());

  static_assert(
      std::same_as<rga3::Point<double>,
                   core::mv<rga3::algebra, rga3::point_layout, double>>);
  static_assert(
      std::same_as<rga3::Line<double>,
                   core::mv<rga3::algebra, rga3::line_layout, double>>);
  static_assert(
      std::same_as<rga3::Plane<double>,
                   core::mv<rga3::algebra, rga3::plane_layout, double>>);
  static_assert(
      std::same_as<rga3::Motor<double>,
                   core::mv<rga3::algebra, rga3::motor_layout, double>>);
  static_assert(
      std::same_as<rga3::Flector<double>,
                   core::mv<rga3::algebra, rga3::flector_layout, double>>);

  static_assert(rga3::Point<float>::layout == set_of(point_names));
  static_assert(rga3::Line<float>::layout == set_of(line_names));
  static_assert(rga3::Plane<float>::layout == set_of(plane_names));
  static_assert(rga3::Motor<float>::layout == set_of(motor_names));
  static_assert(rga3::Flector<float>::layout == set_of(flector_names));

  static_assert(!std::same_as<rga3::Point<float>, rga3::Point<double>>);
  static_assert(std::same_as<rga3::Point<float>::scalar_type, float>);
  static_assert(std::same_as<rga3::Motor<rational>::scalar_type, rational>);
  static_assert(
      std::same_as<rga3::Flector<std::int64_t>::algebra_type, rga3::algebra>);
}

// Reference: contract: a named type stores only its components,
//   contiguously, so its size is its number of components times the size of
//   its scalar: 4 for a point and a plane, 6 for a line, and 8 for a motor
//   and a flector, so a motor over double takes 64 bytes, each is trivially
//   copyable and standard-layout, and over double a structural type, so a
//   value is a template argument that reads back equal
TEST_CASE_TEMPLATE("the rga3 types hold their coefficients and nothing else", T,
                   float, double) {
  static_assert(sizeof(rga3::Point<T>) == 4 * sizeof(T));
  static_assert(sizeof(rga3::Line<T>) == 6 * sizeof(T));
  static_assert(sizeof(rga3::Plane<T>) == 4 * sizeof(T));
  static_assert(sizeof(rga3::Motor<T>) == 8 * sizeof(T));
  static_assert(sizeof(rga3::Flector<T>) == 8 * sizeof(T));

  static_assert(std::is_trivially_copyable_v<rga3::Point<T>>);
  static_assert(std::is_trivially_copyable_v<rga3::Line<T>>);
  static_assert(std::is_trivially_copyable_v<rga3::Plane<T>>);
  static_assert(std::is_trivially_copyable_v<rga3::Motor<T>>);
  static_assert(std::is_trivially_copyable_v<rga3::Flector<T>>);
  static_assert(std::is_standard_layout_v<rga3::Motor<T>>);
  static_assert(std::is_standard_layout_v<rga3::Flector<T>>);

  static_assert(sizeof(rga3::Motor<double>) == 64);
  constexpr rga3::Point<double> point{1, 2, 3, 1};
  constexpr rga3::Line<double> line{1, 2, 3, 4, 5, 6};
  constexpr rga3::Plane<double> plane{0, 0, 1, -2};
  constexpr rga3::Motor<double> motor{1, 2, 3, 4, 5, 6, 7, 8};
  constexpr rga3::Flector<double> flector{1, 2, 3, 4, 5, 6, 7, 8};
  static_assert(as_template_argument<point> == point);
  static_assert(as_template_argument<line> == line);
  static_assert(as_template_argument<plane> == plane);
  static_assert(as_template_argument<motor> == motor);
  static_assert(as_template_argument<flector> == flector);
  CHECK(as_template_argument<motor>.coefficients[7] == 8.0);
}

// Reference: independent value: by hand, a point x e1 + y e2 + z e3 + w e4
//   has weight w, and so an rga3 Point x e1 + y e2 + z e3 + w e4 has weight
//   w: the Point (1, 2, 3, 5) holds 1, 2, 3, 5 on e1, e2, e3, e4, and its
//   weight, the part with e4, is 5 e4, a unitized point, (-2, 7, 0, 1), has
//   weight 1, and a direction, (1, 2, 3, 0), weight 0
TEST_CASE_TEMPLATE("an rga3 Point x e1 + y e2 + z e3 + w e4 has weight w", T,
                   std::int64_t, rational) {
  using Point = rga3::Point<T>;
  constexpr Point p{T{1}, T{2}, T{3}, T{5}};
  static_assert(core::get<"e1">(p) == T{1});
  static_assert(core::get<"e2">(p) == T{2});
  static_assert(core::get<"e3">(p) == T{3});
  static_assert(core::get<"e4">(p) == T{5});
  static_assert(core::get<"e4">(core::weight(p)) == T{5});
  static_assert(core::get<"e4">(core::weight(Point{T{-2}, T{7}, T{0}, T{1}})) ==
                T{1});
  static_assert(core::get<"e4">(core::weight(Point{T{1}, T{2}, T{3}, T{0}})) ==
                T{0});
  CHECK(core::get<"e4">(core::weight(p)) == T{5});
}

// Reference: independent value: by hand, the line through unitized points p
//   and q has direction q - p, and the line p ^ q has direction
//   w_p q - w_q p and moment p x q, so the rga3 Line p ^ q of unitized
//   points has direction q - p and moment p x q: for p = (1, 2, 3) and
//   q = (4, 6, 8), both of weight 1, the direction (3, 4, 5) on e41, e42,
//   e43 and the moment p x q = (-2, 4, -2) on e23, e31, e12, with weights 2
//   and 3 the direction is 2q - 3p = (5, 6, 7) and the moment the same, and
//   q ^ p is the line with the opposite direction and moment
TEST_CASE_TEMPLATE("the rga3 Line p ^ q of unitized points has direction "
                   "q - p and moment p x q",
                   T, std::int64_t, rational) {
  using Point = rga3::Point<T>;
  using Line = rga3::Line<T>;
  constexpr Point p{T{1}, T{2}, T{3}, T{1}};
  constexpr Point q{T{4}, T{6}, T{8}, T{1}};
  static_assert(Line{p ^ q} == Line{T{3}, T{4}, T{5}, T{-2}, T{4}, T{-2}});
  static_assert(Line{q ^ p} == Line{T{-3}, T{-4}, T{-5}, T{2}, T{-4}, T{2}});

  constexpr Point p2{T{1}, T{2}, T{3}, T{2}};
  constexpr Point q3{T{4}, T{6}, T{8}, T{3}};
  static_assert(Line{p2 ^ q3} == Line{T{5}, T{6}, T{7}, T{-2}, T{4}, T{-2}});
  CHECK(Line{p ^ q} == Line{T{3}, T{4}, T{5}, T{-2}, T{4}, T{-2}});
}

// Reference: independent value: the line p ^ q has direction w_p q - w_q p
//   and moment p x q, computed in the reference by the wedge and by cross
//   products, which agree, for seeded points of weights 1, 2, -1, 1/2, and 3,
//   and the rga3 Line p ^ q of the same points over the test rational holds
//   the same coefficients
TEST_CASE("the rga3 Line p ^ q of seeded points holds the reference's line") {
  for (std::size_t i = 0; i < golden::rigid_lines::joins.size(); ++i) {
    CAPTURE(i);
    const golden::line_join& c = golden::rigid_lines::joins.at(i);
    CHECK(rga3::Line<rational>{point_of(c.p) ^ point_of(c.q)} == line_of(c.pq));
  }
}

// Reference: independent value: by hand, a point p lies on the plane g when
//   p v g = 0, and p v g = g_x x + g_y y + g_z z + g_w w, so the antiwedge of
//   an rga3 Point and Plane is g_x x + g_y y + g_z z + g_w w on the scalar:
//   (1, 2, 3, 4) v (5, 6, 7, 8) = 5 + 12 + 21 + 32 = 70, the point (1, 1, 1)
//   lies on the plane x + y + z - 3 = 0, (1, 1, 1, -3), and gives 0, and on
//   x + y + z - 2 = 0 it gives 1
TEST_CASE_TEMPLATE("the antiwedge of an rga3 Point and Plane is "
                   "g_x x + g_y y + g_z z + g_w w",
                   T, std::int64_t, rational) {
  using Point = rga3::Point<T>;
  using Plane = rga3::Plane<T>;
  constexpr Point p{T{1}, T{2}, T{3}, T{4}};
  constexpr Plane g{T{5}, T{6}, T{7}, T{8}};
  static_assert(core::get<"1">(core::antiwedge(p, g)) == T{70});
  static_assert(core::get<"1">(p & g) == T{70});

  constexpr Point on{T{1}, T{1}, T{1}, T{1}};
  static_assert(core::get<"1">(on & Plane{T{1}, T{1}, T{1}, T{-3}}) == T{0});
  static_assert(core::get<"1">(on & Plane{T{1}, T{1}, T{1}, T{-2}}) == T{1});
  CHECK(core::get<"1">(core::antiwedge(p, g)) == T{70});
}

// Reference: independent value: by hand, the plane p ^ q ^ r through
//   unitized points is (n, -n . p) with n = (q - p) x (r - p), so the rga3
//   Plane p ^ q ^ r of points counterclockwise about the tip of a normal has
//   that normal: the origin, (1, 0, 0), and (0, 1, 0) run counterclockwise
//   seen from +z and give the plane (0, 0, 1, 0), and in the other order
//   (0, 0, -1, 0), (0, 0, 2), (1, 0, 2), (0, 1, 2) give (0, 0, 1, -2), and
//   (1, 0, 0), (0, 1, 0), (0, 0, 1), counterclockwise seen from (1, 1, 1),
//   give (1, 1, 1, -1)
TEST_CASE_TEMPLATE("the rga3 Plane p ^ q ^ r of points counterclockwise "
                   "about the tip of a normal has that normal",
                   T, std::int64_t, rational) {
  using Point = rga3::Point<T>;
  using Plane = rga3::Plane<T>;
  constexpr Point o{T{0}, T{0}, T{0}, T{1}};
  constexpr Point x{T{1}, T{0}, T{0}, T{1}};
  constexpr Point y{T{0}, T{1}, T{0}, T{1}};
  constexpr Point z{T{0}, T{0}, T{1}, T{1}};
  static_assert(Plane{(o ^ x) ^ y} == Plane{T{0}, T{0}, T{1}, T{0}});
  static_assert(Plane{(o ^ y) ^ x} == Plane{T{0}, T{0}, T{-1}, T{0}});
  static_assert(
      Plane{(Point{T{0}, T{0}, T{2}, T{1}} ^ Point{T{1}, T{0}, T{2}, T{1}}) ^
            Point{T{0}, T{1}, T{2}, T{1}}} == Plane{T{0}, T{0}, T{1}, T{-2}});
  static_assert(Plane{(x ^ y) ^ z} == Plane{T{1}, T{1}, T{1}, T{-1}});
  CHECK(Plane{(x ^ y) ^ z} == Plane{T{1}, T{1}, T{1}, T{-1}});
}

// Reference: independent value: by hand, compose of two rga3 Motors that do
//   not commute applies its second argument first, since applying Q1 and
//   then Q2 is one transform by geometric_antiproduct(Q2, Q1): with Q1 the
//   rotor 3/5 e43 + 4/5 antiscalar, a turn about z by 2 phi with
//   cos 2 phi = 7/25 and sin 2 phi = 24/25, and Q2 the translator
//   e23 + 2 e31 - e12 + antiscalar, a move by (2, 4, -2), the point
//   (1, 0, 0) turns to (7/25, 24/25, 0) and moves to (57/25, 124/25, -2),
//   while moving first and then turning gives (3, 4, -2) turned, (-3, 4, -2),
//   over std::int64_t the rotor 3 e43 + 4 antiscalar scales a point's
//   weight by 25, so the two orders give (57, 124, -50, 25) and
//   (-75, 100, -50, 25), and the antiproduct of two motors or two flectors
//   is a motor, and of a motor and a flector a flector, so the compositions
//   convert to Motor and Flector, the blades of a composition of two motors
//   or two flectors are of even grade, and those of a motor and a flector of
//   odd grade
TEST_CASE("compose of two rga3 Motors that do not commute applies its second "
          "argument first") {
  using IntPoint = rga3::Point<std::int64_t>;
  using IntMotor = rga3::Motor<std::int64_t>;
  constexpr IntMotor int_rotor{0, 0, 3, 4, 0, 0, 0, 0};
  constexpr IntMotor int_translator{0, 0, 0, 1, 1, 2, -1, 0};
  constexpr IntPoint int_x{1, 0, 0, 1};
  static_assert(
      core::transform(int_x, core::compose(int_translator, int_rotor)) ==
      IntPoint{57, 124, -50, 25});
  static_assert(
      core::transform(int_x, core::compose(int_rotor, int_translator)) ==
      IntPoint{-75, 100, -50, 25});

  using Point = rga3::Point<rational>;
  using Motor = rga3::Motor<rational>;
  using Flector = rga3::Flector<rational>;
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
  CHECK(core::transform(x, core::compose(q1, q2)) == first_q2);
  CHECK(core::transform(x, Motor{core::compose(q2, q1)}) == first_q1);
  CHECK(core::transform(x, core::geometric_antiproduct(q2, q1)) == first_q1);

  static_assert(std::constructible_from<Motor, decltype(core::compose(
                                                   Motor{}, Motor{}))>);
  static_assert(std::constructible_from<Motor, decltype(core::compose(
                                                   Flector{}, Flector{}))>);
  static_assert(std::constructible_from<Flector, decltype(core::compose(
                                                     Motor{}, Flector{}))>);
  static_assert(std::constructible_from<Flector, decltype(core::compose(
                                                     Flector{}, Motor{}))>);
  static_assert(all_of_parity<decltype(core::compose(Motor{}, Motor{}))>(0));
  static_assert(
      all_of_parity<decltype(core::compose(Flector{}, Flector{}))>(0));
  static_assert(all_of_parity<decltype(core::compose(Motor{}, Flector{}))>(1));
  static_assert(!all_of_parity<decltype(core::compose(Motor{}, Flector{}))>(0));
}

// Reference: independent value: by hand, the translator
//   t_x e23 + t_y e31 + t_z e12 + antiscalar moves points by 2t, so an rga3
//   translator Motor moves points by 2t: the point (x, y, z) of weight w
//   goes to (x + 2 t_x w, y + 2 t_y w, z + 2 t_z w) with weight w, with
//   t = (1, 2, -3), (1, 2, 3, 1) goes to (3, 6, -3, 1), (1, 2, 3, 3) to
//   (7, 14, -15, 3), and the direction (1, 2, 3, 0) is unchanged, with
//   t = (1/2, -3/4, 2), (1/3, 0, 9/2, 3) goes to (10/3, -9/2, 33/2, 3)
TEST_CASE("an rga3 translator Motor moves points by 2t") {
  using IntPoint = rga3::Point<std::int64_t>;
  using IntMotor = rga3::Motor<std::int64_t>;
  constexpr IntMotor translator{0, 0, 0, 1, 1, 2, -3, 0};
  static_assert(core::transform(IntPoint{1, 2, 3, 1}, translator) ==
                IntPoint{3, 6, -3, 1});
  static_assert(core::transform(IntPoint{1, 2, 3, 3}, translator) ==
                IntPoint{7, 14, -15, 3});
  static_assert(core::transform(IntPoint{1, 2, 3, 0}, translator) ==
                IntPoint{1, 2, 3, 0});

  using Point = rga3::Point<rational>;
  using Motor = rga3::Motor<rational>;
  const Motor half{rational{0},    rational{0},     rational{0}, rational{1},
                   rational{1, 2}, rational{-3, 4}, rational{2}, rational{0}};
  CHECK(core::transform(
            Point{rational{1, 3}, rational{0}, rational{9, 2}, rational{3}},
            half) ==
        Point{rational{10, 3}, rational{-9, 2}, rational{33, 2}, rational{3}});
  CHECK(core::transform(IntPoint{1, 2, 3, 1}, translator) ==
        IntPoint{3, 6, -3, 1});
}

// Reference: independent value: by hand, the rotor
//   l sin(phi) + antiscalar cos(phi) turns by 2 phi about l, so an rga3
//   rotor Motor turns by 2 phi counterclockwise about its line, seen from
//   the tip of the line's direction: with sin(phi) = 3/5 and
//   cos(phi) = 4/5, cos 2 phi = 7/25 and sin 2 phi = 24/25, so about the z
//   axis, e43, (1, 0, 0) goes to (7/25, 24/25, 0), about the x axis, e41,
//   (0, 1, 0) goes to (0, 7/25, 24/25), about the y axis, e42, (0, 0, 1)
//   goes to (24/25, 0, 7/25), about the line through (1, 0, 0) along z,
//   with direction (0, 0, 1) and moment (1, 0, 0) x (0, 0, 1) = (0, -1, 0),
//   (2, 0, 0) goes to (32/25, 24/25, 0) and (1, 0, 5) on the line stays,
//   over std::int64_t the rotors 3 e43 + 4 antiscalar and the same about
//   the line through (1, 0, 0) give 25 times these points, (7, 24, 0, 25)
//   and (32, 24, 0, 25)
TEST_CASE("an rga3 rotor Motor turns by 2 phi counterclockwise about its "
          "line") {
  using IntPoint = rga3::Point<std::int64_t>;
  using IntMotor = rga3::Motor<std::int64_t>;
  static_assert(
      core::transform(IntPoint{1, 0, 0, 1}, IntMotor{0, 0, 3, 4, 0, 0, 0, 0}) ==
      IntPoint{7, 24, 0, 25});
  static_assert(core::transform(IntPoint{2, 0, 0, 1},
                                IntMotor{0, 0, 3, 4, 0, -3, 0, 0}) ==
                IntPoint{32, 24, 0, 25});

  using Point = rga3::Point<rational>;
  using Motor = rga3::Motor<rational>;
  const rational s{3, 5};
  const rational c{4, 5};
  const rational o{0};
  const rational one{1};
  const rational cos2{7, 25};
  const rational sin2{24, 25};
  CHECK(core::transform(Point{one, o, o, one}, Motor{o, o, s, c, o, o, o, o}) ==
        Point{cos2, sin2, o, one});
  CHECK(core::transform(Point{o, one, o, one}, Motor{s, o, o, c, o, o, o, o}) ==
        Point{o, cos2, sin2, one});
  CHECK(core::transform(Point{o, o, one, one}, Motor{o, s, o, c, o, o, o, o}) ==
        Point{sin2, o, cos2, one});

  const Motor off_axis{o, o, s, c, o, -s, o, o};
  CHECK(core::transform(Point{rational{2}, o, o, one}, off_axis) ==
        Point{rational{32, 25}, sin2, o, one});
  CHECK(core::transform(Point{one, o, rational{5}, one}, off_axis) ==
        Point{one, o, rational{5}, one});
}

// Reference: independent value: by hand, a reflected point keeps a positive
//   weight, and the rga3 Flector e412 reflects (1, 2, 3, 1) to
//   (1, 2, -3, 1) with weight +1: the flector holding 1 on e412 is the plane
//   z = 0, while the form -F ⟇ x ⟇ antireverse(F) without the grade
//   involution gives the weight -1, the mirror with the unit normal
//   (3/5, 4/5, 0) and offset -2 sends the unitized p to p - 2 (n . p - 2) n,
//   so (5, 7, -1) goes to (-73/25, -89/25, -1), and the flector of the point
//   c = (1, 2, 3) sends p to 2c - p, so (5, 7, -1) goes to (-3, -3, 7)
TEST_CASE("the rga3 Flector e412 reflects (1, 2, 3, 1) to (1, 2, -3, 1) with "
          "weight +1") {
  using IntPoint = rga3::Point<std::int64_t>;
  using IntFlector = rga3::Flector<std::int64_t>;
  constexpr IntFlector int_mirror{0, 0, 0, 0, 0, 0, 1, 0};
  static_assert(core::transform(IntPoint{1, 2, 3, 1}, int_mirror) ==
                IntPoint{1, 2, -3, 1});

  using Point = rga3::Point<rational>;
  using Flector = rga3::Flector<rational>;
  const Flector mirror{rational{0}, rational{0}, rational{0}, rational{0},
                       rational{0}, rational{0}, rational{1}, rational{0}};
  const Point p{rational{1}, rational{2}, rational{3}, rational{1}};
  const Point reflected = core::transform(p, mirror);
  CHECK(reflected ==
        Point{rational{1}, rational{2}, rational{-3}, rational{1}});
  CHECK(core::get<"e4">(reflected) == rational{1});
  const Point single_sign = core::part<Point>(-core::geometric_antiproduct(
      core::geometric_antiproduct(mirror, p), core::antireverse(mirror)));
  CHECK(core::get<"e4">(single_sign) == rational{-1});

  const Flector tilted{rational{0}, rational{0},    rational{0},
                       rational{0}, rational{3, 5}, rational{4, 5},
                       rational{0}, rational{-2}};
  const Point q{rational{5}, rational{7}, rational{-1}, rational{1}};
  CHECK(core::transform(q, tilted) ==
        Point{rational{-73, 25}, rational{-89, 25}, rational{-1}, rational{1}});
  const Flector at_c{rational{1}, rational{2}, rational{3}, rational{1},
                     rational{0}, rational{0}, rational{0}, rational{0}};
  CHECK(core::transform(q, at_c) ==
        Point{rational{-3}, rational{-3}, rational{7}, rational{1}});
}

// Reference: independent value: by hand, a motor's or a flector's
//   transform is the outermorphism of its action on points, so rga3 Lines
//   and Planes move as the Points that span them, and a reflected line or
//   plane is the one spanned by the reflected points in the same order:
//   T(p ^ q) = T(p) ^ T(q) and T(p ^ q ^ r) = T(p) ^ T(q) ^ T(r), for the
//   rotor 3/5 e43 + 4/5 antiscalar followed by a translator, the mirror with
//   the unit normal (3/5, 4/5, 0) and offset -2, the flector of the point
//   (1, 2, 3), the motor that composes those two flectors, and the flector
//   that composes the motor and the mirror, each of which has
//   V ⟇ antireverse(V) = antiscalar, and a Line or Plane transformed by a
//   Motor or a Flector stays a Line or a Plane
TEST_CASE("rga3 Lines and Planes move as the Points that span them") {
  using Point = rga3::Point<rational>;
  using Line = rga3::Line<rational>;
  using Plane = rga3::Plane<rational>;
  using Motor = rga3::Motor<rational>;
  using Flector = rga3::Flector<rational>;
  static_assert(std::same_as<decltype(core::transform(Line{}, Motor{})), Line>);
  static_assert(
      std::same_as<decltype(core::transform(Line{}, Flector{})), Line>);
  static_assert(
      std::same_as<decltype(core::transform(Plane{}, Motor{})), Plane>);
  static_assert(
      std::same_as<decltype(core::transform(Plane{}, Flector{})), Plane>);

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
  const Motor two_mirrors{core::compose(mirror, at_c)};
  const Flector moved_mirror{core::compose(motor, mirror)};

  const Point p{rational{1}, rational{2}, rational{3}, rational{1}};
  const Point q{rational{-2}, rational{1, 2}, rational{0}, rational{1}};
  const Point r{rational{4}, rational{-1}, rational{5, 3}, rational{1}};
  const Line l{p ^ q};
  const Plane g{(p ^ q) ^ r};
  const auto check = [&](const auto& v) {
    const Point tp = core::transform(p, v);
    const Point tq = core::transform(q, v);
    const Point tr = core::transform(r, v);
    CHECK(core::transform(l, v) == Line{tp ^ tq});
    CHECK(core::transform(g, v) == Plane{(tp ^ tq) ^ tr});
  };
  check(motor);
  check(mirror);
  check(at_c);
  check(two_mirrors);
  check(moved_mirror);
}
