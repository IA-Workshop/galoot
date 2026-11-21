// The exact values that the documentation design records quote: the test
// value of each misconception and the numbers of the route from rotors to
// motors. Each is derived by hand in the record that quotes it; this file
// confirms that the library computes it. Over int, so every claim is a
// static_assert.
//
//   g++ -std=c++23 -I include -fsyntax-only design_values_probe.cpp
//   clang++ -std=c++23 -I include -fsyntax-only design_values_probe.cpp

#include <concepts>

#include <galoot/core.hpp>
#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;

namespace {

struct e2 {
  static constexpr auto value = core::R<2, 0, 0>();
};
struct e3 {
  static constexpr auto value = core::R<3, 0, 0>();
};
using Vector2 = core::mv<e2, core::blades<e2, "e1 e2">, int>;
using Vector = core::mv<e3, core::blades<e3, "e1 e2 e3">, int>;
using Bivector = core::mv<e3, core::blades<e3, "e23 e31 e12">, int>;
using Canonical = core::mv<e3, core::blades<e3, "e12 e13 e23">, int>;
using Even = core::mv<e3, core::blades<e3, "1 e12 e13 e23">, int>;
using Odd = core::mv<e3, core::blades<e3, "e1 e2 e3 e123">, int>;
using Scalar = core::mv<e3, core::blades<e3, "1">, int>;

constexpr Vector a{1, 2, 0};
constexpr Vector b{3, 1, 0};

// The dot is a multivector on the scalar, not a number.
static_assert(std::same_as<decltype(core::dot(a, b)), Scalar>);
static_assert(core::get<"1">(core::dot(a, b)) == 5);

// e21 = -e12, and the wedge of (1, 2, 3) and (4, 5, 6) holds the numbers of
// their cross product (-3, 6, -3) on e23, e31, e12.
static_assert(core::get<"e12">(Vector{0, 1, 0} ^ Vector{1, 0, 0}) == -1);
static_assert(Bivector{Vector{1, 2, 3} ^ Vector{4, 5, 6}} ==
              Bivector{-3, 6, -3});
static_assert(Canonical{Vector{1, 2, 3} ^ Vector{4, 5, 6}} ==
              Canonical{-3, -6, -3});

// The product of two vectors is a scalar plus a bivector, and it does not
// commute: a b = 5 - 5 e12 and b a = 5 + 5 e12.
static_assert(Even{a * b} == Even{5, -5, 0, 0});
static_assert(Even{b * a} == Even{5, 5, 0, 0});

// The sandwich of a rotor reaches e123 as well, with zero there.
constexpr Even r{Vector{1, 1, 0} * Vector{1, 0, 0}};
static_assert(Odd{r * Vector{0, 0, 1} * ~r} == Odd{0, 0, 2, 0});

// Two complements: -x for a vector of two generators, x in three.
static_assert(Vector2{!Vector2{1, 0}} == Vector2{0, 1});
static_assert(Vector2{!!Vector2{1, 0}} == Vector2{-1, 0});
static_assert(Vector{!!Vector{1, 2, 3}} == Vector{1, 2, 3});

// In three dimensions the complement of the normal (1, 2, 3) is the plane
// e23 + 2 e31 + 3 e12, and two planes through the origin meet in the line
// along the cross product of their normals: (1, 1, 0) x (0, 1, 1) =
// (1, -1, 1).
static_assert(Bivector{!Vector{1, 2, 3}} == Bivector{1, 2, 3});
static_assert(Vector{(!Vector{1, 0, 0}) & (!Vector{0, 1, 0})} ==
              Vector{0, 0, 1});
static_assert(Vector{(!Vector{1, 1, 0}) & (!Vector{0, 1, 1})} ==
              Vector{1, -1, 1});

// The complement carries the geometric product to the antiproduct.
static_assert(Odd{core::compose(!Vector{1, 1, 0}, !Vector{1, 0, 0})} ==
              Odd{!r});
static_assert(Odd{(!Vector{1, 1, 0}) % (!Vector{1, 0, 0})} == Odd{!r});

// The factor a versor leaves is the same on both sides: r ~r = 2, and
// !r % antireverse(!r) = 2 e123.
static_assert(core::get<"1">(r * ~r) == 2);
static_assert(Odd{core::antireverse(!r)} == Odd{0, 0, 1, 1});
static_assert(Odd{(!r) % core::antireverse(!r)} == Odd{0, 0, 0, 2});

// --- rigid(3) ---------------------------------------------------------------

using Point = rga3::Point<int>;
using Line = rga3::Line<int>;
using Plane = rga3::Plane<int>;
using Motor = rga3::Motor<int>;
using Flector = rga3::Flector<int>;

// One point, two values: == compares coefficients.
static_assert(Point{2, 4, 6, 2} != Point{1, 2, 3, 1});

// e4 squares to zero: the dot of a point with itself ignores its weight, and
// the bulk and the weight split it.
static_assert(core::get<"1">(core::dot(Point{1, 2, 3, 2}, Point{1, 2, 3, 2})) ==
              14);
static_assert(Point{core::bulk(Point{1, 2, 3, 2})} == Point{1, 2, 3, 0});
static_assert(Point{core::weight(Point{1, 2, 3, 2})} == Point{0, 0, 0, 2});

// The complement of the origin is the plane e321; its bulk dual is zero.
static_assert(Plane{!Point{0, 0, 0, 1}} == Plane{0, 0, 0, 1});
static_assert(Plane{core::bulk_dual(Point{0, 0, 0, 1})} == Plane{});

// The planes z = 0 and z = 1 are parallel and still meet: in a line with
// zero direction.
constexpr Plane z0{0, 0, 1, 0};
constexpr Plane z1{0, 0, 1, -1};
static_assert(Line{z0 & z1} == Line{0, 0, 0, 0, 0, -1});

// Reflected in z = 1, the point (1, 2, 3) goes to (1, 2, -1) and keeps the
// weight +1.
static_assert(core::transform(Point{1, 2, 3, 1}, Flector{z1}) ==
              Point{1, 2, -1, 1});

// Two parallel mirrors move by twice their distance, from the first toward
// the second: z = 0 and then z = 1 move by (0, 0, 2).
constexpr Motor shift{core::compose(Flector{z1}, Flector{z0})};
static_assert(shift == Motor{0, 0, 0, 1, 0, 0, 1, 0});
static_assert(core::transform(Point{1, 2, 3, 1}, shift) == Point{1, 2, 5, 1});

// Two crossing mirrors turn by twice their angle about their common line:
// x = 1 and then y = 1, at a right angle, make a half turn about the
// vertical line through (1, 1, 0), which misses the origin.
constexpr Plane x1{1, 0, 0, -1};
constexpr Plane y1{0, 1, 0, -1};
static_assert(Line{x1 & y1} == Line{0, 0, -1, -1, 1, 0});
constexpr Motor half_turn{core::compose(Flector{y1}, Flector{x1})};
static_assert(half_turn == Motor{0, 0, 1, 0, 1, -1, 0, 0});
static_assert(Motor{half_turn % core::antireverse(half_turn)} ==
              Motor{0, 0, 0, 1, 0, 0, 0, 0});
static_assert(core::transform(Point{0, 0, 0, 1}, half_turn) ==
              Point{2, 2, 0, 1});
static_assert(core::transform(Point{1, 1, 5, 1}, half_turn) ==
              Point{1, 1, 5, 1});
static_assert(core::transform(Point{2, 1, 0, 1}, half_turn) ==
              Point{0, 1, 0, 1});

} // namespace

int main() {}
