// What transform gives for a classical rotor and a classical mirror, in
// R(3, 0, 0), R(2, 0, 0), and rigid(3), over int so that every value is
// exact and every claim is a static_assert. The expected values are derived
// by hand in research note 0028; this file only confirms that the library
// computes them.
//
//   g++ -std=c++23 -I include -fsyntax-only transform_sandwich_probe.cpp
//   clang++ -std=c++23 -I include -fsyntax-only transform_sandwich_probe.cpp

#include <galoot/core.hpp>
#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;

namespace {

// --- R(3, 0, 0) -------------------------------------------------------------

struct e3 {
  static constexpr auto value = core::R<3, 0, 0>();
};
using Vector = core::mv<e3, core::blades<e3, "e1 e2 e3">, int>;
using Rotor = core::mv<e3, core::blades<e3, "1 e12 e13 e23">, int>;
using Odd = core::mv<e3, core::blades<e3, "e1 e2 e3 e123">, int>;

// The classical actions, written out with * and ~ only.
constexpr Vector turned(const Rotor& r, const Vector& v) {
  return core::truncate<Vector>(r * v * ~r);
}
constexpr Vector mirrored(const Vector& a, const Vector& v) {
  return -core::truncate<Vector>(a * v * a);
}

constexpr Vector a{1, 0, 0};
constexpr Vector b{1, 1, 0};
constexpr Vector c{2, 1, 0};
constexpr Vector x1{1, 0, 0};
constexpr Vector x2{0, 1, 0};
constexpr Vector x3{0, 0, 1};
constexpr Vector v{2, -3, 5};

// r = b a: a quarter turn from e1 toward e2, scaled by r ~r = 2.
constexpr Rotor r{b * a};
static_assert(r == Rotor{1, -1, 0, 0});
static_assert(turned(r, x1) == Vector{0, 2, 0});
static_assert(turned(r, x2) == Vector{-2, 0, 0});
static_assert(turned(r, x3) == Vector{0, 0, 2});

// transform(x, r) is not that turn. It agrees on e1 and e2, which is the
// coincidence of a quarter turn, and is the negation on e3.
static_assert(core::transform(x1, r) == Vector{0, 2, 0});
static_assert(core::transform(x2, r) == Vector{-2, 0, 0});
static_assert(core::transform(x3, r) == Vector{0, 0, -2});
// The antiproduct sandwich without the odd sign is the negation of that.
static_assert(core::truncate<Vector>(r % x1 % core::antireverse(r)) ==
              Vector{0, -2, 0});

// transform(x, !r) is the turn, on every vector.
static_assert(Odd{!r} == Odd{0, 0, -1, 1});
static_assert(core::transform(x1, !r) == turned(r, x1));
static_assert(core::transform(x2, !r) == turned(r, x2));
static_assert(core::transform(x3, !r) == turned(r, x3));
static_assert(core::transform(v, !r) == turned(r, v));
static_assert(turned(r, v) == Vector{6, 4, 10});

// s = c a: cosine 3/5 and sine 4/5, scaled by s ~s = 5. No coincidence.
constexpr Rotor s{c * a};
static_assert(s == Rotor{2, -1, 0, 0});
static_assert(turned(s, x1) == Vector{3, 4, 0});
static_assert(turned(s, x2) == Vector{-4, 3, 0});
static_assert(turned(s, x3) == Vector{0, 0, 5});
static_assert(core::transform(x1, s) == Vector{-3, 4, 0});
static_assert(core::transform(x2, s) == Vector{-4, -3, 0});
static_assert(core::transform(x3, s) == Vector{0, 0, -5});
static_assert(core::transform(x1, !s) == Vector{3, 4, 0});
static_assert(core::transform(v, !s) == turned(s, v));

// transform(x, V) = ~V x' V, with x' the grade involution of x for a V of
// even grade: here -~s x s on a vector.
static_assert(core::transform(v, s) == -core::truncate<Vector>(~s * v * s));

// One mirror. The classical reflection -a x a negates the part along a;
// transform(x, a) keeps it and negates the rest; transform(x, !a) is the
// classical reflection.
static_assert(mirrored(a, Vector{1, 2, 3}) == Vector{-1, 2, 3});
static_assert(core::transform(Vector{1, 2, 3}, a) == Vector{1, -2, -3});
static_assert(core::transform(Vector{1, 2, 3}, !a) == Vector{-1, 2, 3});
static_assert(core::transform(v, !b) == mirrored(b, v));

// Two mirrors compose in the same order on both sides: !(b a) = !b % !a.
static_assert(Odd{core::compose(!b, !a)} == Odd{!r});

// --- R(2, 0, 0) -------------------------------------------------------------

struct e2 {
  static constexpr auto value = core::R<2, 0, 0>();
};
using Vector2 = core::mv<e2, core::blades<e2, "e1 e2">, int>;
using Rotor2 = core::mv<e2, core::blades<e2, "1 e12">, int>;

constexpr Vector2 turned(const Rotor2& r, const Vector2& v) {
  return core::truncate<Vector2>(r * v * ~r);
}

constexpr Rotor2 quarter{Vector2{1, 1} * Vector2{1, 0}};
constexpr Rotor2 other{Vector2{2, 1} * Vector2{1, 0}};
static_assert(quarter == Rotor2{1, -1} && other == Rotor2{2, -1});
// In the plane a quarter turn is the one rotor for which transform(x, r)
// is the classical turn on every vector.
static_assert(core::transform(Vector2{3, 7}, quarter) ==
              turned(quarter, Vector2{3, 7}));
static_assert(turned(other, Vector2{1, 0}) == Vector2{3, 4});
static_assert(core::transform(Vector2{1, 0}, other) == Vector2{-3, 4});
static_assert(core::transform(Vector2{1, 0}, !other) == Vector2{3, 4});
static_assert(core::transform(Vector2{3, 7}, !other) ==
              turned(other, Vector2{3, 7}));

// --- rigid(3) ---------------------------------------------------------------

using Point = rga3::Point<int>;
using Motor = rga3::Motor<int>;
using Flector = rga3::Flector<int>;
using Direction =
    core::mv<rga3::algebra, core::blades<rga3::algebra, "e1 e2 e3">, int>;
using EuclideanRotor =
    core::mv<rga3::algebra, core::blades<rga3::algebra, "1 e12 e13 e23">, int>;

// The same mirrors and the same rotor, written in rigid(3).
constexpr Direction ga{1, 0, 0};
constexpr Direction gb{1, 1, 0};
constexpr EuclideanRotor gr{gb * ga};
static_assert(gr == EuclideanRotor{1, -1, 0, 0});

// The right complement carries the mirror with normal (1, 0, 0) to the
// plane x = 0, and the rotor to the motor of the quarter turn about the z
// axis, with the factor 2.
static_assert(Flector{!ga} == Flector{0, 0, 0, 0, 1, 0, 0, 0});
static_assert(Flector{!gb} == Flector{0, 0, 0, 0, 1, 1, 0, 0});
static_assert(Motor{!gr} == Motor{0, 0, 1, 1, 0, 0, 0, 0});

// They do to the point (x, y, z, 1) what the classical forms do to the
// vector (x, y, z); the weight is multiplied by the same factor.
static_assert(core::transform(Point{1, 2, 3, 1}, Flector{!ga}) ==
              Point{-1, 2, 3, 1});
static_assert(core::transform(Point{1, 0, 0, 1}, Motor{!gr}) ==
              Point{0, 2, 0, 2});
static_assert(core::transform(Point{0, 0, 1, 1}, Motor{!gr}) ==
              Point{0, 0, 2, 2});
static_assert(core::transform(Point{2, -3, 5, 1}, Motor{!gr}) ==
              Point{6, 4, 10, 2});

// Two mirrors compose to the motor: !gb % !ga = !(gb ga).
static_assert(Motor{core::compose(Flector{!gb}, Flector{!ga})} == Motor{!gr});

} // namespace

int main() {}
