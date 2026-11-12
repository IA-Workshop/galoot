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

This is the 1.0.0 release of the core. The rga3 preset ships in the same
tree but is below 1.0, and its interface may still change.

## Using Galoot

Galoot is headers alone, so there is nothing to build and nothing to
install. Include the preset or the core:

```text
#include <galoot/presets/rga3.hpp>   // the rga3 preset, and through it the core
#include <galoot/core.hpp>           // the core alone
```

There are three ways to bring the headers into a project.

**Copy the headers.** Copy or vendor the `include/` directory, put it on the
include path, and compile as C++23. No build system is needed:

```text
g++ -std=c++23 -I galoot/include main.cpp -o main
```

**Add the source tree with CMake.** With Galoot's source tree in the
directory `galoot`, add it and link the target you use,
`galoot::presets::rga3` or `galoot::core`. The target carries the include
path and the C++23 requirement, and Galoot's own tests are not built:

```cmake
cmake_minimum_required(VERSION 4.4)
project(my_program LANGUAGES CXX)

add_subdirectory(galoot)

add_executable(my_program main.cpp)
target_link_libraries(my_program PRIVATE galoot::presets::rga3)
```

**Fetch it with CMake.** `FetchContent` downloads the source tree at
configure time and adds it in the same way. The repository below is a
placeholder to replace with Galoot's repository; the tag names this release:

```cmake
cmake_minimum_required(VERSION 4.4)
project(my_program LANGUAGES CXX)

include(FetchContent)
# Replace the placeholder URL with Galoot's repository.
FetchContent_Declare(galoot
  GIT_REPOSITORY "<galoot-repository-url>"
  GIT_TAG "1.0.0")
FetchContent_MakeAvailable(galoot)

add_executable(my_program main.cpp)
target_link_libraries(my_program PRIVATE galoot::presets::rga3)
```

This release has no `install()` rule and no package for `find_package`. The
two CMake routes need CMake 4.4 or later, because Galoot's own
`CMakeLists.txt` requires it and no older version is tested.

Galoot is tested with GCC 16 and Clang 23, on Linux with libstdc++. It uses
no compiler extensions, throws no exceptions, and compiles with
`-fno-exceptions`.

Preconditions that the types cannot express are checked at run time when
`GALOOT_CHECKING` is defined as 1 (`-DGALOOT_CHECKING=1`). By default it is
0, and they are checked only during constant evaluation.

The core forces its operations inline, so that each compiles to the
arithmetic of its terms where it is called. A file with very many calls,
such as a large test, then takes much longer and more memory to compile,
most of all with sanitizers. Defining `GALOOT_FORCE_INLINE` as 0
(`-DGALOOT_FORCE_INLINE=0`) leaves inlining to the compiler; by default it
is 1. Every file of a program must be compiled with the same value.

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

## Next

[The guide](docs/guide/README.md) starts with the nine facts that most
often surprise a new user, each in a few sentences with an example; read it
next. It continues with [the core](docs/guide/core.md), which covers
declaring an algebra of your own from its metric, every operation, and the
exact change of basis between two algebras; [the rga3
preset](docs/guide/rga3.md); and [the limits](docs/guide/limits.md) on the
size of an algebra and of a layout.

Each `cpp` block of this README and of the guide is compiled and run as one
of Galoot's tests, with both compilers, and each `cmake` block is built as a
project of its own, so the examples stay correct as the library changes.

## License

Galoot is released under the MIT License; see [LICENSE](LICENSE).
