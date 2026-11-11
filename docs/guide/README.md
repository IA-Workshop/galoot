# Galoot: the first things to know

This page is for a reader who knows geometric algebra and C++ and has read
the [README](../../README.md). It gathers the nine facts that most often
cost a new user an hour: each has a short explanation, an example that
compiles, and a link to the part of the guide that says more. It takes
about ten minutes to read.

The examples use the rga3 preset, the rigid geometric algebra of three
dimensions. A point is written (x, y, z, w), with weight w; e1, e2, and e3
are the axes and e4 is the projective generator, which squares to zero.
Each object is constructed from its coefficients in the order its layout
lists its blades:

| Object | Coefficients, in order | On the blades |
| --- | --- | --- |
| `Point<T>` | x, y, z, w | e1, e2, e3, e4 |
| `Line<T>` | direction vx, vy, vz, then moment mx, my, mz | e41, e42, e43, then e23, e31, e12 |
| `Plane<T>` | normal gx, gy, gz, then offset gw | e423, e431, e412, then e321 |
| `Motor<T>` | vx, vy, vz, vw, then mx, my, mz, mw | e41, e42, e43, e1234, then e23, e31, e12, 1 |
| `Flector<T>` | px, py, pz, pw, then gx, gy, gz, gw | e1, e2, e3, e4, then e423, e431, e412, e321 |

So `Line<double>{0, 0, 1, 0, 0, 0}` is the z axis through the origin, and a
point (x, y, z, w) lies on the plane (gx, gy, gz, gw) when
gx x + gy y + gz z + gw w = 0. A motor holds half of each angle and
distance; [the rga3 guide](rga3.md) gives each object's meaning. The rest
of the guide is in three pages: [the core](core.md), [the rga3
preset](rga3.md), and [the limits](limits.md).

## 1. Results live on their own layouts

A product does not return a named type such as `Line`. Its result is stored
on the blades the product can reach, each with its canonical orientation
(indices in increasing order, sign +1), and that layout has no name of its
own. The same holds for the meet `l & g`, the right complement `!x`, the
antiproduct `a % b`, and `compose`. Convert the result to a named type with
braces: `Line{p ^ q}`, `Point{l & g}`, `Plane{!p}`, `Motor{compose(b, a)}`.
The conversion is explicit, so `const Motor m = compose(b, a);` does not
compile; the compiler reports that no conversion exists. Values compare
with `==` only on one layout, so convert before comparing:
`Line{p ^ q} == l`. A mismatch is reported as no match for `operator==`,
or no conversion, between two `mv<...>` types that the compiler prints in
full: each type spells its layout as a `blade_set` of masks and signs, and
two types that differ only in that list are two layouts of one algebra,
one of which must be converted. With `auto`, the value keeps the canonical
layout, and is read and printed by canonical names: the join of two points
holds its direction on e14, e24, e34, where a `Line` holds it on e41, e42,
e43, with the opposite signs.

```cpp
#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<int>;
using Line = rga3::Line<int>;
using Motor = rga3::Motor<int>;

constexpr Point origin{0, 0, 0, 1};
constexpr Point q{0, 1, 0, 1};

// The join runs from the origin to q, so its direction is q - origin =
// (0, 1, 0). A Line stores the direction on e41, e42, e43.
constexpr Line y_axis{origin ^ q};
static_assert(core::get<"e42">(y_axis) == 1);

// Kept with auto, the join is on the canonical blades. Since e42 = -e24, the
// same direction reads -1 on e24.
constexpr auto join = origin ^ q;
static_assert(core::get<"e24">(join) == -1);
static_assert(Line{join} == y_axis);

// A move by (2, 0, 0) holds half of it, 1, on e23, and 1 on e1234. Two such
// moves make one by (4, 0, 0). compose returns its own layout too, so the
// result is converted with braces.
constexpr Motor move{0, 0, 0, 1, 1, 0, 0, 0};
constexpr Motor twice{core::compose(move, move)};
static_assert(twice == Motor{0, 0, 0, 1, 2, 0, 0, 0});

int main() {}
```

More: [result layouts](core.md#result-layouts) and
[conversions](core.md#conversions) in the core guide.

## 2. Scalars must match exactly

A multivector over `double` is scaled only by a `double`. `2 * line` is
rejected, with a message that says to convert 2 to the multivector's scalar
type, as 2.0 for 2 over `double`; the same holds for `a * s` and `a / s`.
The rga3 constructors follow the same rule, each with its own message.
`translation(1.0, 0, 2)` is rejected because its three numbers are not of
one type, and `translation(1, 2, 3)`, all `int`, because halving a distance
would truncate in an integer type. `rotation(line, 1)` is rejected because
the angle is not of the line's scalar type. `translation(1.0, 0.0, 2.0)` and
`rotation(line, 1.0)` are accepted. A
coefficient written in braces is the exception, as in brace initialization
anywhere: `Line<double>{0, 0, 1, 0, 0, 0}` takes integer literals.

```cpp rejected
// rejected: s * a: the scalar is not of the multivector's scalar type, the second of the two types shown
#include <galoot/presets/rga3.hpp>

using Line = galoot::presets::rga3::Line<double>;

Line twice(const Line& line) { return 2 * line; }
```

```cpp rejected
// rejected: translation: dx, dy, and dz are not all of one scalar type, the types shown
#include <galoot/presets/rga3.hpp>

using Motor = galoot::presets::rga3::Motor<double>;

Motor up_and_over() { return galoot::presets::rga3::translation(1.0, 0, 2); }
```

Written with `double` values throughout, the same work compiles. The
examples qualify `rotation` as `rga3::rotation`, as they must
`translation`, so that both motions read alike; argument-dependent lookup
would also find `rotation` unqualified, through its `Line` argument:

```cpp
#include <print>

#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;

int main() {
  const rga3::Line<double> z_axis{0, 0, 1, 0, 0, 0};
  const rga3::Line<double> doubled = 2.0 * z_axis;

  // rotation divides the axis by the length of its direction, so the
  // doubled axis turns by the same 1 radian as z_axis would.
  const rga3::Motor<double> turn = rga3::rotation(doubled, 1.0);
  const rga3::Motor<double> move = rga3::translation(1.0, 0.0, 2.0);
  const rga3::Motor<double> motion{core::compose(move, turn)};

  // The turn takes (1, 0, 0) to (cos 1, sin 1, 0), about (0.540, 0.841, 0),
  // and the move adds (1, 0, 2).
  const rga3::Point<double> p =
      core::transform(rga3::Point<double>{1, 0, 0, 1}, motion);
  std::println("({:.3f}, {:.3f}, {:.3f})", core::get<"e1">(p),
               core::get<"e2">(p), core::get<"e3">(p));
  // prints: (1.540, 0.841, 2.000)
}
```

More: [scalars](core.md#scalars) in the core guide and
[rotation and translation](rga3.md#rotation-and-translation) in the rga3
guide.

## 3. Two operators to learn: `%` and `!`

`a % b` is the geometric antiproduct a ⟇ b, the product that motors and
flectors act by; its identity is the antiscalar e1234, as 1 is the
geometric product's, and `compose(b, a)` is `b % a`. `!a` is the right
complement, fixed on each blade u by u ∧ !u = e1234. So `!x` is a
multivector, not a `bool`: `if (!x)` does not compile. And `!!x` is not x:
it is x with each blade multiplied by (−1) to the power of its grade times
its antigrade, so in rigid(3), where a point's blades have grade 1 and
antigrade 3, `!!p` is `-p`. Like a product, `!x` is on a layout of its
own, the complements of x's blades in canonical order, so convert it before
comparing it with a named type: `Plane{!p} == g`. To compare it with a value
of its own layout, parenthesize it, `(!x) == y`, since GCC warns that
`!x == y` may mean `!(x == y)`.

```cpp
#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<int>;
using Plane = rga3::Plane<int>;
using Motor = rga3::Motor<int>;
using Antiscalar =
    core::mv<rga3::algebra, core::blades<rga3::algebra, "e1234">, int>;

constexpr Point p{1, 2, 3, 1};

// The antiscalar is the identity of the antiproduct.
static_assert(Point{Antiscalar{1} % p} == p);

// compose(b, a) is b % a.
constexpr Motor move{0, 0, 0, 1, 1, 0, 0, 0};
static_assert(Motor{move % move} == Motor{core::compose(move, move)});

// u ^ !u = e1234 gives !e1 = e234 = e423, !e2 = -e134 = e431,
// !e3 = e124 = e412, and !e4 = -e123 = e321: the right complement of the
// point (x, y, z, w) is the plane with the same four coefficients.
static_assert(Plane{!p} == Plane{1, 2, 3, 1});

// Applied twice, a vector of rigid(3) is multiplied by (-1)^(1 * 3) = -1.
static_assert(Point{!!p} == -p);

int main() {}
```

More: [involutions and complements](core.md#involutions-and-complements)
and [operators and their precedence](core.md#operators-and-their-precedence)
in the core guide.

## 4. `^` and `&` bind more loosely than `==`

C++ gives `^` and `&` a lower precedence than `==`, `+`, and `*`, so
`p ^ q == l` means `p ^ (q == l)`. That compares a point with a line, which
does not compile, since values of different layouts have no `==`. Put every
join and meet in parentheses when it stands beside another operator. A
comparison also needs both sides on one layout, so a join is compared with a
`Line` after it is converted: `Line{p ^ q} == l`.

```cpp
#include <galoot/presets/rga3.hpp>

namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<int>;
using Line = rga3::Line<int>;
using Plane = rga3::Plane<int>;

constexpr Point p{1, 0, 0, 1};
constexpr Point q{1, 2, 0, 1};

// The line from p to q: its direction q - p = (0, 2, 0) on e41, e42, e43,
// and its moment p x q = (0, 0, 2) on e23, e31, e12.
constexpr Line l{0, 2, 0, 0, 0, 2};
static_assert(Line{p ^ q} == l);

// Two values of the join's own layout compare directly, in parentheses.
constexpr auto join = p ^ q;
static_assert((p ^ q) == join);

// The plane y = 1 holds the points where 1 y - 1 w = 0, so it is written
// (0, 1, 0, -1). It meets the line at (1, 1, 0); the weight of the meet,
// -2, carries the orientations of the line and the plane.
constexpr Plane g{0, 1, 0, -1};
static_assert(Point{(p ^ q) & g} == Point{-2, -2, 0, -2});

int main() {}
```

More: [operators and their precedence](core.md#operators-and-their-precedence)
in the core guide.

## 5. Names are oriented

A blade name lists generators in the order of their product, so e41 is
−e14. `get<"e41">(line)` reads the coefficient the `Line` stores on e41,
the x part of its direction. The line also holds e14, with the other
orientation, but `get<"e14">(line)` is rejected, with a message that says
the layout stores the blade the other way round: read it by the layout's
name and negate it. A canonical layout, such as a join kept with `auto`,
names its blades with increasing indices, so there it is `get<"e14">`.

```cpp
#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<int>;
using Line = rga3::Line<int>;

constexpr Point origin{0, 0, 0, 1};
constexpr Point x{1, 0, 0, 1};

// The x axis, from the origin to x: direction (1, 0, 0).
constexpr Line x_axis{origin ^ x};
static_assert(core::get<"e41">(x_axis) == 1);

// The canonical join holds the same direction on e14 = -e41.
static_assert(core::get<"e14">(origin ^ x) == -1);

int main() {}
```

```cpp rejected
// rejected: use the name its layout gives it and negate the value
#include <galoot/presets/rga3.hpp>

using Line = galoot::presets::rga3::Line<double>;

double direction_x(const Line& line) { return galoot::core::get<"e14">(line); }
```

More: [reading and writing
coefficients](core.md#reading-and-writing-coefficients) in the core guide,
and the coefficient order of each object in [the rga3 guide](rga3.md).

## 6. How many generators

An algebra may have up to 16 generators. A factory such as `R` accepts more
when it is given a larger bound, up to 32, but nothing above 16 is tested.
Blade names have one digit per index, so `blades<Tag, "...">`, `get<"...">`
and printing work up to 9 generators. Above 9, a layout is written with
masks, in which bit i − 1 stands for e_i, and a coefficient is read by its
position. Dense layouts reach the compilers' limits on constant evaluation
long before 16: on Clang, the geometric product of the scalar with every
blade of 8 generators already fails at the default settings.

```cpp
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

// Twelve generators, each squaring to +1.
struct r12 {
  static constexpr auto value = core::R<12, 0, 0>();
};

// The vectors e1 and e10, as masks: bit 0 stands for e1, bit 9 for e10.
constexpr core::blade_set<2> layout{{{{0b1, 1}, {0b10'0000'0000, 1}}}};
using Vector = core::mv<r12, layout, int>;

// v v is the scalar 3^2 + 4^2 = 25, with zero on the bivector e1 e10. The
// product's layout is canonical, the scalar first, so it is position 0.
constexpr Vector v{3, 4};
static_assert((v * v).coefficients[0] == 25);
static_assert((v * v).coefficients[1] == 0);

} // namespace

int main() {}
```

More: [number of generators](limits.md#number-of-generators) and [blade
names](limits.md#blade-names) on the limits page.

## 7. `transform` does not unitize

`transform(x, V)` computes V ⟇ x ⟇ V̰, where V̰ is the antireverse of V.
When V ⟇ V̰ = λ e1234, it multiplies the weight of what it moves by λ. A
motor built by `rotation` or `translation` has λ = 1, but one written by its
coefficients may not, and a point moved by it comes back with another
weight: the same point, written differently. This release has no
`unitize`; write a unit motor, or divide by the weight.

```cpp
#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<int>;
using Motor = rga3::Motor<int>;

// A quarter turn about the z axis holds sin 45 degrees times the direction
// (0, 0, 1) on e43 and cos 45 degrees on e1234. Both are multiplied by the
// square root of 2 to make them integers.
constexpr Motor turn{0, 0, 1, 1, 0, 0, 0, 0};

// So turn % antireverse(turn) is 2 e1234 rather than e1234: lambda = 2.
static_assert(Motor{turn % core::antireverse(turn)} ==
              Motor{0, 0, 0, 2, 0, 0, 0, 0});

// The point (1, 0, 0) turns to (0, 1, 0), and its weight doubles.
static_assert(core::transform(Point{1, 0, 0, 1}, turn) == Point{0, 2, 0, 2});

int main() {}
```

More: [transform and compose](core.md#transform-and-compose) in the core
guide.

## 8. Printing needs its own header

Include `<galoot/core/format.hpp>` to print. Neither `<galoot/core.hpp>`
nor `<galoot/presets/rga3.hpp>` includes it, because `<format>` costs more
to compile than the rest of the core. A value prints as each coefficient on
its blade's name, in the order of its layout, zeros included. The rga3 types
print by the preset's names, such as e41; a value on a canonical layout, such
as a join kept with `auto`, prints by canonical names.

```cpp
#include <print>

#include <galoot/core/format.hpp> // IWYU pragma: keep
#include <galoot/presets/rga3.hpp>

namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<int>;
using Line = rga3::Line<int>;

int main() {
  const Point origin{0, 0, 0, 1};
  const Point q{0, 1, 0, 1};

  // The direction is (0, 1, 0): 1 on e42 in the Line, -1 on e24 = -e42 in
  // the canonical join.
  std::println("{}", Line{origin ^ q});
  // prints: {e41: 0, e42: 1, e43: 0, e23: 0, e31: 0, e12: 0}
  std::println("{}", origin ^ q);
  // prints: {e12: 0, e13: 0, e14: 0, e23: 0, e24: -1, e34: 0}
}
```

More: [printing](core.md#printing) in the core guide.

## 9. Qualify `transform` near `std::ranges`

The core's functions are found by argument-dependent lookup, so most calls
need no namespace. But in a scope with `using namespace std::ranges` or
`using namespace std::views`, an unqualified `transform(x, V)` finds the
standard library's `transform` and does not compile; write
`galoot::core::transform(x, V)`. The same holds for `reverse`. And
`translation` takes plain numbers, which name no namespace of the preset,
so it must always be qualified: `galoot::presets::rga3::translation`.

```cpp
#include <algorithm>
#include <array>
#include <print>

#include <galoot/presets/rga3.hpp>

namespace rga3 = galoot::presets::rga3;
using Point = rga3::Point<double>;

int main() {
  using namespace std::ranges;

  const auto shift = rga3::translation(1.0, 2.0, 3.0);
  const std::array<Point, 2> points{Point{0, 0, 0, 1}, Point{1, 0, 0, 1}};
  std::array<Point, 2> moved{};

  // Here an unqualified transform is std::ranges::transform, so Galoot's
  // is named in full.
  transform(points, moved.begin(), [&](const Point& p) {
    return galoot::core::transform(p, shift);
  });

  // (1, 0, 0) moved by (1, 2, 3).
  std::println("({:.3f}, {:.3f}, {:.3f})", galoot::core::get<"e1">(moved[1]),
               galoot::core::get<"e2">(moved[1]),
               galoot::core::get<"e3">(moved[1]));
  // prints: (2.000, 2.000, 3.000)
}
```

More: [calling the functions](core.md#calling-the-functions) in the core
guide.
