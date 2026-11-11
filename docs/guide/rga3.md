# The rga3 guide

The rga3 preset, in namespace `galoot::presets::rga3`, is the rigid
geometric algebra of three dimensions, R(3, 0, 1): e1, e2, and e3 square to
1 and e4, the projective generator, to 0. It names the objects of that
algebra, `Point`, `Line`, `Plane`, `Motor`, and `Flector`, each a template
on its scalar type, and stores them in the layouts of Eric Lengyel's rigid
geometric algebra ([rigidgeometricalgebra.org](https://rigidgeometricalgebra.org)),
so the coefficients of its published formulas carry over in the order and
with the signs in which they are written there. Include
`<galoot/presets/rga3.hpp>`, which brings the whole core with it.

The types are the core's multivectors, so every operation in [the core
guide](core.md) applies to them; this page covers what the preset adds. A
blade is named by its generators in the order of their product, so
e41 = −e14, e31 = −e13, and e321 = −e123, and a coefficient written on e41
multiplies −e14. The antiscalar is e1234. The preset's tag,
`galoot::presets::rga3::algebra`, gives these names to the printer, so the
types print by them.

## Points

A point is x e1 + y e2 + z e3 + w e4, written `Point<T>{x, y, z, w}`. Its
weight is w, and it is unitized when w = 1. As in homogeneous coordinates,
(2, 4, 6, 2) is the same point as (1, 2, 3, 1), and a point of weight 0 is a
direction, a point at infinity.

## Lines

A line stores its direction (vx, vy, vz) on e41, e42, e43 and then its
moment (mx, my, mz) on e23, e31, e12: `Line<T>{vx, vy, vz, mx, my, mz}`.
The join p ∧ q of two unitized points runs from p to q, so its direction is
q − p, and its moment is p × q for points of any weight. The direction and
the moment of a line are orthogonal.

## Planes

A plane stores its normal (gx, gy, gz) on e423, e431, e412 and then its
offset gw on e321: `Plane<T>{gx, gy, gz, gw}`. A point (x, y, z, w) lies on
it when gx x + gy y + gz z + gw w = 0, so the plane z = 1 is (0, 0, 1, −1).
The plane p ∧ q ∧ r through three points that run counterclockwise when
seen from the tip of its normal has that normal.

```cpp
#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<int>;
using Line = rga3::Line<int>;
using Plane = rga3::Plane<int>;

constexpr Point p{1, 0, 0, 1};
constexpr Point q{1, 2, 0, 1};

// Direction q - p = (0, 2, 0); moment p x q = (0 * 0 - 0 * 2,
// 0 * 1 - 1 * 0, 1 * 2 - 0 * 1) = (0, 0, 2).
static_assert(Line{p ^ q} == Line{0, 2, 0, 0, 0, 2});

// The same points at weight 2 give the same direction times 4, as the join
// is bilinear, and the moment p x q of the weighted points, also times 4.
static_assert(Line{Point{2, 0, 0, 2} ^ Point{2, 4, 0, 2}} ==
              Line{0, 8, 0, 0, 0, 8});

// (0, 0, 1), (1, 0, 1), (0, 1, 1) run counterclockwise seen from above:
// the normal (1, 0, 0) x (0, 1, 0) = (0, 0, 1), and the offset -1, so that
// the points with z = 1 lie on the plane.
constexpr Point a{0, 0, 1, 1};
constexpr Point b{1, 0, 1, 1};
constexpr Point c{0, 1, 1, 1};
static_assert(Plane{(a ^ b) ^ c} == Plane{0, 0, 1, -1});

// The coefficient of each name, as the layouts list them.
static_assert(core::get<"e42">(Line{p ^ q}) == 2);
static_assert(core::get<"e12">(Line{p ^ q}) == 2);
static_assert(core::get<"e321">(Plane{0, 0, 1, -1}) == -1);

int main() {}
```

## Motors

A motor stores (Q_vx, Q_vy, Q_vz, Q_vw) on e41, e42, e43, e1234 and then
(Q_mx, Q_my, Q_mz, Q_mw) on e23, e31, e12, 1, the order and coefficient
names of Lengyel's Motor page: `Motor<T>{vx, vy, vz, vw, mx, my, mz, mw}`.
Its blades are those of even grade. `transform(x, Q)` moves any object to
Q ⟇ x ⟇ Q̰, where Q̰ is the antireverse, so lines and planes move as the
points that span them.

A motor's coefficients hold half of each angle and distance. The translator
tx e23 + ty e31 + tz e12 + e1234 moves points by 2t, and the rotor
l sin φ + e1234 cos φ, for a unitized line l, turns by 2φ, counterclockwise
when seen from the tip of the direction of l. So a motor that moves by
(2, 4, 6) is `Motor<T>{0, 0, 0, 1, 1, 2, 3, 0}`.

## Flectors

A flector stores (F_px, F_py, F_pz, F_pw) on e1, e2, e3, e4 and then
(F_gx, F_gy, F_gz, F_gw) on e423, e431, e412, e321, the order and
coefficient names of Lengyel's Flector page:
`Flector<T>{px, py, pz, pw, gx, gy, gz, gw}`. Its blades are those of odd
grade. A plane written as a flector, with its four coefficients on the plane
blades and zero on the point blades, is the reflection in that plane. Its
action is in [reflections](#reflections).

## Join and meet

The wedge `^` joins: two points give the line through them, and a line and
a point the plane through both. The antiwedge `&` meets: two planes give the
line where they cross, a line and a plane the point where they cross, and a
point and a plane the scalar gx x + gy y + gz z + gw w. Each result is on
the core's canonical layout; convert it to the named type with braces,
`Line{p ^ q}`, `Point{l & g}`, as [the first page](README.md) explains.

Orientation follows from the order of the operands: q ^ p is the line from
q to p, with the opposite direction and moment, and a meet carries the
orientations of its operands in its weight, which may be negative. A point
of negative weight is the same point; divide by the weight to read it.

```cpp
#include <galoot/presets/rga3.hpp>

namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<int>;
using Line = rga3::Line<int>;
using Plane = rga3::Plane<int>;

constexpr Point p{1, 0, 0, 1};
constexpr Point q{1, 2, 0, 1};
constexpr Line l{p ^ q};

// Reversing the points reverses the line.
static_assert(Line{q ^ p} == -l);

// The plane y = 1 is (0, 1, 0, -1). It meets the line at (1, 1, 0), here
// with weight -2: the point (-2, -2, 0, -2).
static_assert(Point{l & Plane{0, 1, 0, -1}} == Point{-2, -2, 0, -2});

// The planes x = 0 and y = 0 meet in the z axis. With the complements
// right(e423) = -e1 and right(e431) = -e2, their meet is
// left(-e1 ^ -e2) = left(e12) = e34 = -e43: the direction (0, 0, -1).
static_assert(Line{Plane{1, 0, 0, 0} & Plane{0, 1, 0, 0}} ==
              Line{0, 0, -1, 0, 0, 0});

// p, q, and the origin span the plane z = 0, with the normal
// (q - p) x (origin - p) = (0, 2, 0) x (-1, 0, 0) = (0, 0, 2).
static_assert(Plane{l ^ Point{0, 0, 0, 1}} == Plane{0, 0, 2, 0});

int main() {}
```

## Rotation and translation

`rotation(l, theta)` is the motor that turns by the full angle theta, in
radians, about the line l, counterclockwise when seen from the tip of l's
direction. It divides all six coefficients of l by the length of its
direction, so the direction may have any positive length, and the join of
two points can be passed as it is, converted: `rotation(Line<double>{p ^ q},
theta)`. The line must be a line, its direction and moment orthogonal, as
every join is; that is not checked. A zero direction, as for p ∧ p, is
checked in checking mode, and otherwise gives coefficients that are
infinite or not a number. `rotation` needs a floating scalar type, since it
takes a sine and a cosine, and theta must have the line's scalar type
exactly: `1.0` over `double`, `1.0f` over `float`. It runs at run time
only, because `std::sin` and `std::cos` are not constant expressions on
every compiler; for a motor in a constant expression, write its
coefficients.

`translation(dx, dy, dz)` is the motor that moves points by
d = (dx, dy, dz), with d / 2 on e23, e31, e12 and 1 on e1234. A point of
weight w moves by w d, so a direction is unchanged. Its three arguments have
one type, and an integer type is rejected, since halving d would truncate.
It takes plain numbers, which name no namespace of the preset, so it is
always called by its full name, `galoot::presets::rga3::translation`.
`rotation` is found through its `Line` argument, so it works unqualified as
well; the examples write `rga3::rotation`, as the README does, so that the
two motions read alike.

Both build unitized motors, for which `transform` keeps the weight of what
it moves.

```cpp
#include <numbers>
#include <print>

#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<double>;
using Line = rga3::Line<double>;

namespace {

void show(const Point& p) {
  std::println("({:.3f}, {:.3f}, {:.3f}) at weight {:.3f}", core::get<"e1">(p),
               core::get<"e2">(p), core::get<"e3">(p), core::get<"e4">(p));
}

} // namespace

int main() {
  constexpr double quarter = std::numbers::pi / 2;

  // The vertical line through (1, 0, 0), with a direction of length 2.
  const Line axis{Point{1, 0, 0, 1} ^ Point{1, 0, 2, 1}};

  // About it, (2, 0, 0) is 1 from the axis along x, and a quarter turn takes
  // that offset to 1 along y: (1, 1, 0). The weight stays 1.
  show(core::transform(Point{2, 0, 0, 1}, rga3::rotation(axis, quarter)));
  // prints: (1.000, 1.000, 0.000) at weight 1.000

  // A move by (1, 2, 3) shifts the point (1, 0, 0) and leaves the direction
  // (1, 0, 0), a point of weight 0, as it is.
  const auto move = rga3::translation(1.0, 2.0, 3.0);
  show(core::transform(Point{1, 0, 0, 1}, move));
  // prints: (2.000, 2.000, 3.000) at weight 1.000
  show(core::transform(Point{1, 0, 0, 0}, move));
  // prints: (1.000, 0.000, 0.000) at weight 0.000
}
```

## Composing motions

`compose(Q2, Q1)` is Q2 ⟇ Q1, the motor that applies Q1 first and then Q2,
so `transform(x, compose(Q2, Q1))` is `transform(transform(x, Q1), Q2)`. The
order matters: a turn and a move do not commute. The composition of two
motors is a motor and is converted with braces, `Motor<T>{compose(q2, q1)}`.

```cpp
#include <numbers>
#include <print>

#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<double>;
using Line = rga3::Line<double>;
using Motor = rga3::Motor<double>;

int main() {
  const Line z_axis{0, 0, 1, 0, 0, 0};
  const Motor turn = rga3::rotation(z_axis, std::numbers::pi / 2);
  const Motor move = rga3::translation(1.0, 0.0, 0.0);
  const Point p{1, 0, 0, 1};

  // Turn first: (1, 0, 0) to (0, 1, 0), then move to (1, 1, 0).
  const Point a = core::transform(p, Motor{core::compose(move, turn)});
  // Move first: (1, 0, 0) to (2, 0, 0), then turn to (0, 2, 0).
  const Point b = core::transform(p, Motor{core::compose(turn, move)});
  std::println("({:.3f}, {:.3f}) and ({:.3f}, {:.3f})", core::get<"e1">(a),
               core::get<"e2">(a), core::get<"e1">(b), core::get<"e2">(b));
  // prints: (1.000, 1.000) and (0.000, 2.000)
}
```

## Reflections

A flector F moves any object x to −F ⟇ x̂ ⟇ F̰, where x̂ is the grade
involution of x. A reflected point keeps a positive weight, and a reflected
line or plane is the one spanned by the reflected points in the same order,
so a reflection reverses the orientation of what it moves, as a mirror does.
Lengyel's Flector page writes the single form −F ⟇ x ⟇ F̰ at every grade,
which agrees on lines but negates points and planes; the preset uses the
form with the grade involution, which agrees with the reflection of the
points at every grade. The antiproduct of two flectors is a motor, and that
of a motor and a flector, in either order, is a flector, so a composition
converts to `Motor<T>` or `Flector<T>`.

```cpp
#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<int>;
using Line = rga3::Line<int>;
using Plane = rga3::Plane<int>;
using Motor = rga3::Motor<int>;
using Flector = rga3::Flector<int>;

// The plane x = 0, (1, 0, 0, 0), as a flector.
constexpr Flector mirror{0, 0, 0, 0, 1, 0, 0, 0};

// (1, 2, 3) reflects to (-1, 2, 3), with weight +1.
static_assert(core::transform(Point{1, 2, 3, 1}, mirror) ==
              Point{-1, 2, 3, 1});

// The line from (1, 0, 0) to (1, 2, 0) reflects to the line from (-1, 0, 0)
// to (-1, 2, 0): direction (0, 2, 0) and moment
// (-1, 0, 0) x (-1, 2, 0) = (0, 0, -2).
constexpr Line l{Point{1, 0, 0, 1} ^ Point{1, 2, 0, 1}};
static_assert(core::transform(l, mirror) == Line{0, 2, 0, 0, 0, -2});

// In the plane z = 0, the plane z = 1, (0, 0, 1, -1), reflects to z = -1,
// spanned by the reflected points in the same order: (0, 0, 1, 1).
constexpr Flector ground_mirror{0, 0, 0, 0, 0, 0, 1, 0};
static_assert(core::transform(Plane{0, 0, 1, -1}, ground_mirror) == Plane{0, 0, 1, 1});

// A move by (2, 0, 0) after the reflection is a flector; two reflections in
// one plane are the identity motor.
constexpr Motor move{0, 0, 0, 1, 1, 0, 0, 0};
constexpr Flector moved_mirror{core::compose(move, mirror)};
static_assert(core::transform(Point{1, 2, 3, 1}, moved_mirror) ==
              Point{1, 2, 3, 1});
static_assert(Motor{core::compose(mirror, mirror)} ==
              Motor{0, 0, 0, 1, 0, 0, 0, 0});

int main() {}
```

## Incidence

The join and the meet test incidence exactly over an exact scalar type:

- a point p lies on a plane g when the scalar `p & g` is zero;
- a point p lies on a line l when `p ^ l`, a plane, is zero;
- a line l lies in a plane g when `l & g`, a point, is zero.

Over `double` a computed zero may be a rounding error away from zero, so a
test there needs a tolerance that suits the computation; with `int` or
another exact type the test is exact, and the same code runs in a constant
expression.

```cpp
#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<int>;
using Line = rga3::Line<int>;
using Plane = rga3::Plane<int>;

// The line x = 1, z = 0, along y, and the plane z = 0.
constexpr Line l{Point{1, 0, 0, 1} ^ Point{1, 2, 0, 1}};
constexpr Plane ground{0, 0, 1, 0};

static_assert(core::get<"1">(Point{5, 7, 0, 1} & ground) == 0);
static_assert(core::get<"1">(Point{5, 7, 1, 1} & ground) == 1);

// (1, 1, 0) is on the line, (2, 1, 0) is not.
static_assert(Plane{Point{1, 1, 0, 1} ^ l} == Plane{});
static_assert(Plane{Point{2, 1, 0, 1} ^ l} != Plane{});

// The line lies in the ground plane, but not in the plane z = 1.
static_assert(Point{l & ground} == Point{});
static_assert(Point{l & Plane{0, 0, 1, -1}} != Point{});

int main() {}
```

## Not in this release

The preset has no `unitize`, no norms, and no exponential or logarithm of a
motor in this release. To unitize a point, divide its coefficients by its
weight. The squared norms are at hand in the core: `dot(x, x)` is the
squared bulk norm and `antidot(x, x)` the squared weight norm times e1234,
as [the core guide](core.md#dot-and-antidot) shows. A motor of a given angle
and distance comes from `rotation`, `translation`, and `compose`.
