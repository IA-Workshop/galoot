# Galoot

Galoot is a header-only C++23 library for geometric algebra. Its core
generates an algebra from a compact declaration: a metric, given as the
squares of its generators, and the sets of blades that each type stores.
Every product of two such types is worked out at compile time into a flat
table of multiply-add terms, so a product costs only the terms it needs, and
the same code runs in a constant expression and at run time, over any scalar
type that has ring arithmetic: `int`, `float`, `double`, or a type of your
own.

On top of the core sits `rga3`, the rigid geometric algebra of three
dimensions, R(3, 0, 1). It names the objects of that algebra (`Point`,
`Line`, `Plane`, and the `Motor` and `Flector` that move them) and stores
them in the layouts of Eric Lengyel's rigid geometric algebra
([rigidgeometricalgebra.org](https://rigidgeometricalgebra.org)), so the
coefficients of its published formulas carry over in the order and with the
signs in which they are written there.

Galoot is in early development, and its interface may still change.

## Using Galoot

Galoot is headers alone, so there is nothing to build. Add `include/` to
your include path, compile as C++23, and include the model or the core:

```text
#include <galoot/rga3.hpp>   // the rga3 model, and through it the core
#include <galoot/core.hpp>   // the core alone
```

With CMake, add the source tree and link the target you use, `galoot::rga3`
or `galoot::core`:

```text
add_subdirectory(galoot)
target_link_libraries(my_program PRIVATE galoot::rga3)
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

#include <galoot/rga3.hpp>

int main() {
  namespace core = galoot::core;
  namespace rga3 = galoot::rga3;
  using Point = rga3::Point<double>;
  using Line = rga3::Line<double>;
  using Motor = rga3::Motor<double>;

  // The join of two points is the line through them, here the z axis. It
  // has a layout of its own, which converts to a Line explicitly.
  const Point origin{0, 0, 0, 1};
  const Point up{0, 0, 1, 1};
  const Line z_axis{origin ^ up};

  // A quarter turn about the z axis, then a move by (1, 2, 3). rotation
  // needs an axis whose direction has length 1, as the z axis's (0, 0, 1)
  // does here. compose(second, first) chains the two into one motor; like
  // the join, its result has a layout of its own and converts to a Motor
  // with braces, so const Motor motion = core::compose(move, turn); would
  // not compile.
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

`rotation` does not check its axis. The line p ^ q through two points of
weight 1 has the direction q - p, so unless p and q are 1 apart, divide the
line's six coefficients by the length of q - p before the call. About a
direction of another length L, the motor turns by 2 atan(L tan(θ/2)) in
place of θ and multiplies the weight of what it moves by
L² sin²(θ/2) + cos²(θ/2): an axis of length 2 makes a quarter turn into a
turn of about 127 degrees.

Over an integer scalar type every result is exact, and the same operations
run at compile time. The meet `&` intersects a line and a plane.
`translation` and `rotation` reject an integer scalar type with a message,
since halving a distance or taking a sine would lose the exact result, so
the motors below are written by their eight coefficients. Those are, in
order, the coefficients on e41, e42, e43, e1234, e23, e31, e12, and 1, and
they hold half of each distance and angle:

```cpp
#include <galoot/rga3.hpp>

namespace core = galoot::core;
namespace rga3 = galoot::rga3;

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

Each `cpp` block of this README is compiled and run as one of Galoot's
tests, with both compilers, so the examples stay correct as the library
changes.

## License

Galoot is released under the MIT License; see [LICENSE](LICENSE).
