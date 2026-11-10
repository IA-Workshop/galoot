# Galoot

Galoot is a header-only C++23 library for geometric algebra. Its core
generates an algebra from a compact declaration: a metric, given as the
squares of its generators or as the matrix of their inner products, and the
sets of blades that each type stores.
Every product of two such types is worked out at compile time into a flat
table of multiply-add terms, so a product costs only the terms it needs, and
the same code runs in a constant expression and at run time, over any scalar
type that has ring arithmetic: `int`, `float`, `double`, or a type of your
own.

On top of the core sit the algebra presets, named algebras built on the
core's public interface as you could build your own. The first, `rga3` in
namespace `galoot::presets::rga3`, is the rigid geometric algebra of three
dimensions, R(3, 0, 1). It names the objects of that algebra (`Point`,
`Line`, `Plane`, and the `Motor` and `Flector` that move them) and stores
them in the layouts of Eric Lengyel's rigid geometric algebra
([rigidgeometricalgebra.org](https://rigidgeometricalgebra.org)), so the
coefficients of its published formulas carry over in the order and with the
signs in which they are written there.

Galoot is in early development, and its interface may still change.

## Using Galoot

Galoot is headers alone, so there is nothing to build. Add `include/` to
your include path, compile as C++23, and include the preset or the core:

```text
#include <galoot/presets/rga3.hpp>   // the rga3 preset, and through it the core
#include <galoot/core.hpp>           // the core alone
```

With CMake, add the source tree and link the target you use,
`galoot::presets::rga3` or `galoot::core`:

```text
add_subdirectory(galoot)
target_link_libraries(my_program PRIVATE galoot::presets::rga3)
```

Galoot is tested with GCC 16 and Clang 23, on Linux with libstdc++. It uses
no compiler extensions and throws no exceptions.

Preconditions that the types cannot express are checked at run time when
`GALOOT_CHECKING` is defined as 1 (`-DGALOOT_CHECKING=1`). By default it is
0, and they are checked only during constant evaluation.

## An example

Points are written as (x, y, z, w), as in homogeneous coordinates: a point
of weight w = 1 sits at (x, y, z), and (2, 4, 6, 2) is the same point as
(1, 2, 3, 1). The blades are named by their generators: e1, e2, and e3 are
the axes, and e4 is the projective one, which many texts call e0. The wedge
operator `^` joins two points into the line through them. `rotation` and
`translation` build motors, `compose` chains two of them, and `transform`
moves an object by the result. `get<"e1">` reads the coefficient on a named
blade.

```cpp
#include <numbers>
#include <print>

#include <galoot/presets/rga3.hpp>

int main() {
  namespace core = galoot::core;
  namespace rga3 = galoot::presets::rga3;
  using Point = rga3::Point<double>;
  using Line = rga3::Line<double>;
  using Motor = rga3::Motor<double>;

  // The join of two points is the line through them, here the z axis, with
  // the direction up - origin = (0, 0, 2). It has a layout of its own, which
  // converts to a Line explicitly.
  const Point origin{0, 0, 0, 1};
  const Point up{0, 0, 2, 1};
  const Line z_axis{origin ^ up};

  // A quarter turn about the z axis, then a move by (1, 2, 3). rotation
  // divides the axis by the length of its direction, so the distance
  // between the two points does not change the turn. compose(second, first)
  // chains the two into one motor; like the join, its result has a layout of
  // its own and converts to a Motor with braces, so
  // const Motor motion = core::compose(move, turn); would not compile.
  const Motor turn = rga3::rotation(z_axis, std::numbers::pi / 2);
  const Motor move = rga3::translation(1.0, 2.0, 3.0);
  const Motor motion{core::compose(move, turn)};

  // The turn takes (1, 0, 0) to (0, 1, 0), and the move takes that to
  // (1, 3, 3).
  const Point p = core::transform(Point{1, 0, 0, 1}, motion);
  std::println("({:.3f}, {:.3f}, {:.3f}) at weight {:.3f}", core::get<"e1">(p),
               core::get<"e2">(p), core::get<"e3">(p), core::get<"e4">(p));
  // prints: (1.000, 3.000, 3.000) at weight 1.000
}
```

The calls are qualified with their namespaces. Most would also be found
unqualified, by argument-dependent lookup, but `translation` takes plain
numbers and must be qualified, and an unqualified `transform` in a scope
with `using namespace std::ranges` finds `std::ranges::transform` instead.

`rotation` divides its axis by the length of the axis's direction, so the
line p ^ q through two different points of weight 1 turns by θ, however
far apart they are. In general the join of p and q has a direction that is
not zero exactly when neither point is a multiple of the other and they are
not both directions, of weight 0. The axis must be a line, its direction
and moment orthogonal, as every join of two points is; a `Line` written by
its six coefficients must meet this itself, and this is not checked. Its
direction must not be zero, as that of p ^ p is: a checking build stops
there with a message, and an unchecked one gives coefficients that are
infinite or not a number.

Over an integer scalar type every result is exact, and the same operations
run at compile time. The meet `&` intersects a line and a plane.
`translation` and `rotation` reject an integer scalar type with a message,
since halving a distance or taking a sine would lose the exact result, so
the motors below are written by their eight coefficients. Those are, in
order, the coefficients on e41, e42, e43, e1234, e23, e31, e12, and 1, and
they hold half of each distance and angle:

```cpp
#include <galoot/presets/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::presets::rga3;

constexpr rga3::Point<int> p{1, 0, 0, 1};
constexpr rga3::Point<int> q{1, 2, 0, 1};

// The line from p to q: its direction q - p = (0, 2, 0) on e41, e42, e43,
// and its moment p x q = (0, 0, 2) on e23, e31, e12.
constexpr rga3::Line<int> line{p ^ q};
static_assert(line == rga3::Line<int>{0, 2, 0, 0, 0, 2});

// The plane y = 1, written (0, 1, 0, -1) because it holds the points where
// 1 y - 1 w = 0, meets the line at (1, 1, 0). The weight of the meet, here
// -2, carries the orientations of the line and the plane.
constexpr rga3::Plane<int> plane{0, 1, 0, -1};
constexpr rga3::Point<int> hit{line & plane};
static_assert(hit == rga3::Point<int>{-2, -2, 0, -2});

// Half of (2, 4, 6) on e23, e31, e12, and 1 on e1234: a move by (2, 4, 6).
constexpr rga3::Motor<int> move{0, 0, 0, 1, 1, 2, 3, 0};
static_assert(core::transform(p, move) == rga3::Point<int>{3, 4, 6, 1});

// The z axis's direction (0, 0, 1) times sin 45 degrees on e41, e42, e43,
// and cos 45 degrees on e1234, both multiplied by the square root of 2 to
// make them integers: a quarter turn about the z axis. A motor multiplied
// by s multiplies the weight of what it moves by s squared, here 2.
constexpr rga3::Motor<int> turn{0, 0, 1, 1, 0, 0, 0, 0};
static_assert(core::transform(p, turn) == rga3::Point<int>{0, 2, 0, 2});

int main() {}
```

To print a value, include `<galoot/core/format.hpp>`, which neither
`presets/rga3.hpp` nor `core.hpp` includes, because `<format>` is costly to
compile. A value then prints as each coefficient on its blade's name, and a
specification such as `{:.1f}` applies to each coefficient. The rga3 types
print by the names their layouts are written with, so a line's direction
is on e41, e42, e43, where e41 = -e14:

```cpp
#include <print>

#include <galoot/core/format.hpp> // IWYU pragma: keep
#include <galoot/presets/rga3.hpp>

int main() {
  namespace rga3 = galoot::presets::rga3;
  const rga3::Point<double> p{1, 3, 3, 1};
  std::println("{}", p);
  // prints: {e1: 1, e2: 3, e3: 3, e4: 1}
  std::println("{:.1f}", p);
  // prints: {e1: 1.0, e2: 3.0, e3: 3.0, e4: 1.0}
  const rga3::Line<double> z_axis{0, 0, 1, 0, 0, 0};
  std::println("{}", z_axis);
  // prints: {e41: 0, e42: 0, e43: 1, e23: 0, e31: 0, e12: 0}
}
```

A join `p ^ q` itself is not a `Line` but the wedge's result, which holds
each blade with its canonical orientation, and since the preset names its
blades with its own orientations, the join prints by the canonical names,
such as `e24: -2` where the `Line` made from it prints `e42: 2`.

## Your own algebra, and a change of basis

The rga3 preset is one algebra, and the core builds any other from its
metric. An algebra is a tag: a type whose `static constexpr` member `value`
holds the metric. `R<p, q, z>()` gives p generators that square to +1, then
q that square to -1, then z that square to 0, all orthogonal;
`rigid<D>()` and `conformal<D>()` give the rigid and the conformal algebra
of D dimensions; and `from_gram` takes a symmetric matrix of integers,
within `max_form_entry`, as the inner products e_i · e_j of the generators.
A multivector type is `mv<Tag, layout, scalar>`, with the layout written by
the names of its blades, `blades<Tag, "1 e12">`, where `1` is the scalar.
A product works out the layout of its result.

Two algebras with the same number of generators are isomorphic when a
change of basis carries the inner products of one to those of the other,
and `change_basis` writes a multivector of the first in the generators of
the second. The change is a tag too, naming its `source` and `target`
algebras, with a `value` of type `basis_change<n>`. Its row i writes the
source's generator e_i in the target's generators e'_1 to e'_n, as
integers over one positive denominator per row. `from_rows(rows,
denominators)` builds one, and the first use of the tag checks, exactly,
that the change has an inverse and keeps every inner product.
`change_basis_back` applies the inverse, which the library computes from
the rows.

Below, the two generators square to zero and have inner product 1, the
relations of the operators that create and destroy one fermion,
f f = 0 and f f† + f† f = 1, with the form doubled, since its entries are
integers. A change takes them to R(1, 1, 0):

```cpp
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

// The Gram matrix of e1 and e2: e1 . e1 = e2 . e2 = 0 and e1 . e2 = 1.
struct witt {
  static constexpr auto value = core::from_gram({{0, 1}, {1, 0}});
};

using Vector = core::mv<witt, core::blades<witt, "e1 e2">, int>;
using Even = core::mv<witt, core::blades<witt, "1 e12">, int>;

constexpr Vector f{1, 0};
constexpr Vector g{0, 1};

// f f = 0, and f g = (f . g) + (f ^ g) = 1 + e12, while g f = 1 - e12, so
// f g + g f = 2.
static_assert(f * f == Even{0, 0});
static_assert(f * g == Even{1, 1});
static_assert((f * g) + (g * f) == Even{2, 0});

// R(1, 1, 0), in which e'1 squares to +1 and e'2 to -1, and the change
// e1 = e'1 + e'2, e2 = (e'1 - e'2) / 2: each squares to 0, and their inner
// product is (1 + 1) / 2 = 1.
struct r110 {
  static constexpr auto value = core::R<1, 1, 0>();
};
struct witt_to_r110 {
  using source = witt;
  using target = r110;
  static constexpr core::basis_change<2> value =
      core::from_rows({{1, 1}, {1, -1}}, {1, 2});
};

// ((e'1 + e'2) ^ (e'1 - e'2)) / 2 = -e'12, so f g = 1 + e12 becomes
// 1 - e'12, and the inverse change brings it back.
using EvenR = core::mv<r110, core::blades<r110, "1 e12">, int>;
constexpr auto fg = core::change_basis<witt_to_r110>(f * g);
static_assert(fg == EvenR{1, -1});
static_assert(core::change_basis_back<witt_to_r110>(fg) == f * g);

} // namespace

int main() {}
```

The results are exact over `int` because the even layout's image needs no
division. A layout whose image does is rejected with a message over an
integer scalar, as `change_basis<witt_to_r110>(g)` is, since e2 maps to
(e'1 - e'2) / 2. A floating scalar type, or a rational type of your own
whose division is exact, takes every layout.

`conformal_change<D>()` is the change from conformal(D), whose origin
e(D+1) and point at infinity e(D+2) square to zero with
e(D+1) · e(D+2) = -1, to its diagonal counterpart R(D+1, 1, 0):
e_i = e'_i for i up to D, the origin is (e'(D+2) - e'(D+1)) / 2, and the
point at infinity is e'(D+1) + e'(D+2), where e'(D+1) squares to +1 and
e'(D+2) to -1. A value prints with the same blade names in either algebra,
so the comments say which one it is in:

```cpp
#include <print>

#include <galoot/core.hpp>
#include <galoot/core/format.hpp> // IWYU pragma: keep

namespace core = galoot::core;

namespace {

struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
struct r410 {
  static constexpr auto value = core::R<4, 1, 0>();
};
struct cga3_to_r410 {
  using source = cga3;
  using target = r410;
  static constexpr auto value = core::conformal_change<3>();
};

using Point = core::mv<cga3, core::blades<cga3, "e1 e2 e3 e4 e5">, double>;

// The conformal point of (x, y, z): its coordinates, 1 on the origin e4,
// and half its squared length on the point at infinity e5.
constexpr Point point(double x, double y, double z) {
  return Point{x, y, z, 1, ((x * x) + (y * y) + (z * z)) / 2};
}

} // namespace

int main() {
  const Point p = point(1, 2, 3);
  const Point q = point(4, 6, 3);

  // In R(4, 1, 0): p's 1 on e4 and 7 on e5 give 7 - 1 / 2 on e'4 and
  // 7 + 1 / 2 on e'5. Back in conformal(3), p is as it was.
  const auto p_diagonal = core::change_basis<cga3_to_r410>(p);
  const auto q_diagonal = core::change_basis<cga3_to_r410>(q);
  std::println("{}", p_diagonal);
  // prints: {e1: 1, e2: 2, e3: 3, e4: 6.5, e5: 7.5}
  std::println("{}", core::change_basis_back<cga3_to_r410>(p_diagonal));
  // prints: {e1: 1, e2: 2, e3: 3, e4: 1, e5: 7}

  // The inner product of two conformal points is minus half the squared
  // distance between them, here -25 / 2, in either basis.
  std::println("{} {}", core::get<"1">(p * q),
               core::get<"1">(p_diagonal * q_diagonal));
  // prints: -12.5 -12.5
}
```

The change preserves the geometric product, the wedge, the dot, the
reverse, the antireverse, and the grade involution, so either basis gives
the same result for them. The antiproducts, the duals, and the operations
built on them agree up to a factor set by the change's determinant, under
conditions that the comment at the top of `galoot/core/basis_change.hpp`
states. The complements, the bulk, and the weight are defined on the
coordinates of one basis and are not preserved in general: in conformal(3)
the bulk of the origin e4 is -e5, while computed in R(4, 1, 0) and mapped
back it is -e5 / 2. The image of a layout can also hold more blades than
the layout: the origin alone maps onto e'4 and e'5, so its round trip
comes back on e4 and e5, with zero on e5, and `truncate<Type>(x)` reads it
as its own type.

Each `cpp` block of this README is compiled and run as one of Galoot's
tests, with both compilers, so the examples stay correct as the library
changes.

## License

Galoot is released under the MIT License; see [LICENSE](LICENSE).
