# The core guide

The core of Galoot turns the declaration of an algebra into its products and
operations. This page teaches it in the order a program needs it: declaring
an algebra, writing layouts, making multivectors, choosing a scalar type, and
then each operation, with the convention it follows stated where it is used.
The [first page](README.md) of the guide collects the facts most likely to
surprise a new user, and [the limits page](limits.md) gives the sizes the
compilers can build. A [reference](#reference) at the end has one entry for
each public name.

Every example on this page is a whole program that uses only the core. Each
is compiled and run by Galoot's tests with GCC and Clang, so the values its
comments state are checked. Include `<galoot/core.hpp>` for the whole core;
printing needs `<galoot/core/format.hpp>` as well.

Notation: n is the number of generators, gr(u) the grade and
ag(u) = n − gr(u) the antigrade of a blade u, **1** the scalar unit, and 𝟙
the antiscalar e12…n.

## Declaring an algebra

An algebra is named by a tag: a type with a `static constexpr` member
`value` that holds its metric, the symmetric matrix of the inner products
e_i · e_j of its generators. The tag's name, not the matrix, then appears in
types and compiler messages. Four factories build the metric:

- `R<P, Q, Z>()` is the diagonal signature R(p, q, z): the first P
  generators square to +1, the next Q to −1, and the last Z to 0, all
  orthogonal. A null generator comes last, where some texts put a null e0
  first.
- `rigid<D>()` is R(D, 0, 1): e1 to eD are Euclidean, and e(D+1) is the null
  projective generator, e4 when D = 3.
- `conformal<D>()` has e1 to eD Euclidean, the origin e(D+1), and the point
  at infinity e(D+2), both null, with e(D+1) · e(D+2) = −1. Its form is not
  diagonal.
- `from_gram({{...}, ...})` takes the matrix itself, for any other form. Its
  entries are integers within `max_form_entry` in magnitude, and it must be
  symmetric; both are checked at compile time with a message.

Generators are named e1 to en, counting from 1. An algebra has from 1 to
`max_generators`, 16, generators; a zero or a larger count is rejected at
compile time. Each factory takes a last template argument `MaxN` that raises
the bound, up to `mask_bits`, 32: the algebra then works, but nothing above
16 generators is tested. `from_gram` deduces its size first, so a raised
bound is written `from_gram<3, 32>({...})`.

```cpp
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct e3 {
  static constexpr auto value = core::R<3, 0, 0>();
};
struct pga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
// Two null generators that pair to 1.
struct witt {
  static constexpr auto value = core::from_gram({{0, 1}, {1, 0}});
};

static_assert(core::metric_like<e3> && core::metric_like<witt>);
static_assert(e3::value.gram.size() == 3 && e3::value.is_diagonal());

// e4 of rigid(3) squares to zero; e1 does not.
static_assert(pga3::value.is_null(3) && !pga3::value.is_null(0));

// gram[i][j] is e_(i+1) . e_(j+1): in conformal(3), e4 . e5 = -1.
static_assert(cga3::value.gram[3][4] == -1 && !cga3::value.is_diagonal());
static_assert(witt::value.gram[0][1] == 1 && witt::value.gram[0][0] == 0);

// Twenty generators need a raised bound, and are not tested.
static_assert(core::max_generators == 16);
static_assert(core::R<20, 0, 0, 32>().gram.size() == 20);

} // namespace

int main() {}
```

The examples on this page declare their tags in an anonymous namespace,
because each is a program of one file. A tag in a header that several files
include goes in a named namespace instead: an anonymous one gives each file
its own tag, and so its own algebra type.

## Layouts

A layout is the list of blades a type stores, each with an orientation: a
coefficient stored on the oriented blade s e_A multiplies s e_A, where s is
+1 or −1. So a type can hold the coefficients of a published formula as they
are written there, with no hidden negation. Internally a blade is a mask, in
which bit i − 1 stands for e_i, and its canonical form lists its indices in
increasing order: the mask 0b1011 is e124.

`blades<Tag, "...">` reads a layout from names separated by spaces. `1` is
the scalar, and any other name is e followed by one digit per generator, in
the order of their product: e41 stands for e4 e1 = −e14. Every name must be
valid in the algebra and no blade may be named twice, in either orientation;
each fault is rejected at compile time with a message. Because each index is
one digit, names work only for algebras of up to 9 generators.

The core also builds layouts of its own, each with orientation +1 on every
blade and in canonical order, which sorts by grade and then
lexicographically by index list, so e14 comes before e23:

- `full<N>`, every blade of N generators, 2^N of them;
- `of_grade<N, K>`, the blades of grade K, C(N, K) of them;
- `merge<A, B>`, the blades of two layouts, each once, in canonical order,
  each keeping its own orientation; a blade that the two hold with opposite
  orientations is rejected.

A layout can also be written directly as a `blade_set`, a list of
`oriented_blade` values `{mask, sign}`. That is the only way above 9
generators. `index_of<S, Mask>` gives the position of a mask in a layout.

```cpp
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct pga3 {
  static constexpr auto value = core::rigid<3>();
};

// A line's direction on e41, e42, e43 and its moment on e23, e31, e12.
constexpr auto line_layout = core::blades<pga3, "e41 e42 e43 e23 e31 e12">;
static_assert(line_layout.size() == 6);

// e41 = -e14: the mask of e1 and e4, 0b1001, with sign -1.
static_assert(line_layout.blades[0] ==
              core::oriented_blade{.mask = 0b1001, .sign = -1});
// e23 is already canonical, and stands at position 3.
static_assert(core::index_of<line_layout, 0b0110> == 3);

// 2^3 blades in three generators; C(4, 2) = 6 bivectors in four, in the
// order e12, e13, e14, e23, e24, e34, so e14 is third.
static_assert(core::full<3>.size() == 8);
static_assert(core::of_grade<4, 2>.size() == 6);
static_assert(core::of_grade<4, 2>.blades[2].mask == 0b1001);

// {e1, e2} and {e2, e3} share e2, so their merge holds three blades.
static_assert(core::merge<core::blades<pga3, "e1 e2">,
                          core::blades<pga3, "e2 e3">>.size() == 3);

// The same as blades<pga3, "e1 e4">, written as masks.
constexpr core::oriented_blade e1{.mask = 0b0001, .sign = 1};
constexpr core::oriented_blade e4{.mask = 0b1000, .sign = 1};
constexpr core::blade_set<2> by_masks{.blades = {{e1, e4}}};
static_assert(by_masks == core::blades<pga3, "e1 e4">);

} // namespace

int main() {}
```

A layout is a list of blades and keeps no algebra: `blades<Tag, "...">`
reads only the tag's number of generators, so a layout serves any algebra
with enough generators.

## Multivectors

`mv<Tag, Layout, T>` holds one coefficient of the scalar type T on each blade
of the layout, in the layout's order. It is a plain value: trivially
copyable when T is, its size the number of blades times the size of T, and
it never allocates. Its coefficients are the public array `coefficients`.

A multivector is constructed from exactly one value per blade, in the
layout's order; a wrong count is one error that prints the two counts. A
default-constructed value is zero. With braces each value converts to T as
in any brace initialization, so an integer literal is accepted for a
floating scalar and a variable that would narrow is rejected; with
parentheses, values may narrow without a word. A layout of one blade has an
explicit constructor, so a scalar never turns into a multivector by
accident.

```cpp
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct pga3 {
  static constexpr auto value = core::rigid<3>();
};
constexpr auto line_layout = core::blades<pga3, "e41 e42 e43 e23 e31 e12">;
using Line = core::mv<pga3, line_layout, double>;

// The z axis through the origin: direction (0, 0, 1), moment zero.
constexpr Line z_axis{0, 0, 1, 0, 0, 0};
static_assert(z_axis.coefficients[2] == 1.0);
static_assert(Line::component_count == 6);

// Zero by default.
static_assert(Line{} == Line{0, 0, 0, 0, 0, 0});

// Two values of one type compare coefficient by coefficient.
static_assert(z_axis != Line{});

} // namespace

int main() {}
```

```cpp rejected
// rejected: mv: give one value for each component, in the order the layout lists them
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct pga3 {
  static constexpr auto value = core::rigid<3>();
};
using Line =
    core::mv<pga3, core::blades<pga3, "e41 e42 e43 e23 e31 e12">, double>;

// Three values for a layout of six blades.
constexpr Line short_line{0, 0, 1};

} // namespace

double direction_z() { return short_line.coefficients[2]; }
```

Values of the same type compare with `==` and `!=`, coefficient by
coefficient and exactly; over a floating scalar that is IEEE equality, so
−0.0 equals +0.0 and a NaN makes two values unequal. Values of different
layouts, algebras, or scalar types have no `==`: convert one first. The
compiler then reports no match for `operator==` between two `mv<...>`
types printed in full, each with its layout spelled as a `blade_set` of
masks and signs; two types that differ only in that list are two layouts of
one algebra. A
multivector has no `<<`; see [printing](#printing).

## Reading and writing coefficients

`get<"name">(x)` is the coefficient of x on the blade of that name, as a
reference: `get<"e43">(line)` reads it and `get<"e43">(line) = 1.0` writes
it. The name is oriented, as the layout is. A layout that stores e41 = −e14
is read with `"e41"`, and `get<"e14">` is rejected with a message that says
the layout holds the other orientation: use the layout's name and negate the
value. A canonical layout, such as a product's result, names each blade by
its indices in increasing order. A name the layout lacks, an index above n,
a text that is not a name, and an algebra of more than 9 generators are
each rejected with a message of their own.

`get` is found by argument-dependent lookup, so `get<"e1">(x)` needs no
namespace. On a temporary it returns an rvalue reference, which must not
outlive the temporary. Above 9 generators, or by position anywhere, read
`x.coefficients[i]`, in the layout's order. `has_name<X, "name">` says,
without an error, whether `get` accepts a name.

```cpp
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct pga3 {
  static constexpr auto value = core::rigid<3>();
};
constexpr auto line_layout = core::blades<pga3, "e41 e42 e43 e23 e31 e12">;
using Line = core::mv<pga3, line_layout, int>;

// Written through get, read by name and by position.
constexpr Line made() {
  Line l{};
  core::get<"e43">(l) = 1;
  core::get<"e31">(l) = 5;
  return l;
}
constexpr Line l = made();
static_assert(core::get<"e43">(l) == 1 && l.coefficients[2] == 1);
static_assert(core::get<"e31">(l) == 5 && l.coefficients[4] == 5);

// The line stores e41 and e31, not e14 and e13.
static_assert(core::has_name<Line, "e41"> && !core::has_name<Line, "e14">);
static_assert(core::has_name<Line, "e31"> && !core::has_name<Line, "e13">);

} // namespace

int main() {}
```

## Conversions

A multivector converts to another layout of the same algebra and scalar
type that holds each of its blades, in either orientation:
`Motor{line}`, `Motor(line)`, or `static_cast<Motor>(line)`. Each
coefficient moves to its blade's place, negated where the two layouts give
the blade opposite orientations, so the value does not change, and the blades
the source lacks are zero. The conversion is explicit, so `Motor m = line;`
does not compile. A conversion to another algebra or scalar type is rejected
with a message.

A conversion to a layout that lacks some blades of the source would lose
their coefficients, so it has its own names:

- `truncate<Type>(x)`, or `truncate<layout>(x)`, keeps the blades the target
  holds and drops the rest. For an exact scalar type, such as an integer, it
  checks that what it drops is zero: always in a constant evaluation, which
  then fails to compile, and at run time in [checking mode](#checking-mode).
  For a rounding type such as `double` it checks nothing, since a coefficient
  that should be zero may be a rounding error away from it and truncate has
  no error bound to test against.
- `part<Type>(x)` takes the same components and never checks, for a part
  that is meant to be dropped, as the grade-2 part of a multivector drops its
  other grades.

Neither rounds a coefficient: they drop components, not digits.

```cpp
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct pga3 {
  static constexpr auto value = core::rigid<3>();
};
using Line =
    core::mv<pga3, core::blades<pga3, "e41 e42 e43 e23 e31 e12">, int>;
using Motor =
    core::mv<pga3, core::blades<pga3, "e41 e42 e43 e1234 e23 e31 e12 1">, int>;
using Bivector = core::mv<pga3, core::of_grade<4, 2>, int>;

constexpr Line z_axis{0, 0, 1, 0, 0, 0};

// A Motor holds every blade of a Line, so the conversion keeps the value.
constexpr Motor m{z_axis};
static_assert(core::get<"e43">(m) == 1 && core::get<"1">(m) == 0);

// The canonical bivector layout holds e34 = -e43, so the coefficient is
// negated in place and the value is unchanged.
constexpr Bivector b{z_axis};
static_assert(core::get<"e34">(b) == -1);

// truncate drops e1234 and 1, which are zero here, and checks that they are.
static_assert(core::truncate<Line>(m) == z_axis);

// A turn holds 1 on e1234; part drops it without a check.
constexpr Motor turn{0, 0, 1, 1, 0, 0, 0, 0};
static_assert(core::part<Line>(turn) == z_axis);

} // namespace

int main() {}
```

`truncate<Line>(turn)` over `int` would not compile: the dropped e1234 holds
1, and the check fails in the constant evaluation.

## Scalars

A scalar type is any `ring_scalar`: a regular type (default construction,
copies, assignment) with `+`, `-`, `*`, and unary `-` whose results convert
to it, constructible from the library's integer coefficients, and not
`bool`, whose sums are not a ring's. `T{}` must be zero, and the ring laws
must hold; neither can be checked. Equality is not required, so a type that
holds several lanes at once is accepted.

A scalar is `exact_scalar` when `std::numeric_limits` says its arithmetic is
exact: the integer types, and a type of your own whose `numeric_limits` sets
`is_exact`. Every other scalar is treated as rounding, `float` and `double`
among them, and so is any type without a `numeric_limits` specialization,
such as `std::complex<double>`. The difference matters to the checks: see
[conversions](#conversions) and [checking mode](#checking-mode).

The scalar of a multivector is fixed exactly. Two operands of a product or a
sum must have the same scalar type, and the scalar of `s * x`, `x * s`, or
`x / s` must be the multivector's own type: over `double`, `2 * x` is
rejected with a message that says to write `2.0`, and over `std::int64_t`,
`2.5 * x` is rejected rather than truncated. The structure constants of every
product are exact integers; a scalar type that cannot hold one of them, as
its `numeric_limits` tell, is rejected with a message.

`x / s` divides each coefficient with the scalar's own division, never by a
reciprocal, which would round twice: exact for an exact rational type,
correctly rounded per coefficient for `double`, and truncated toward zero
for a built-in integer type. An integer division by zero is checked in
checking mode.

```cpp
#include <complex>
#include <cstdint>

#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct e3 {
  static constexpr auto value = core::R<3, 0, 0>();
};
using Vector = core::mv<e3, core::blades<e3, "e1 e2 e3">, std::int64_t>;

static_assert(core::ring_scalar<double> && core::ring_scalar<std::int64_t>);
static_assert(!core::ring_scalar<bool>);
static_assert(core::exact_scalar<int> && !core::exact_scalar<double>);
static_assert(!core::exact_scalar<std::complex<double>>);

// The scalar has the vector's own type, and integer division truncates
// toward zero: 7 / 2 is 3 and -7 / 2 is -3.
constexpr Vector v{7, -7, 4};
static_assert(v / std::int64_t{2} == Vector{3, -3, 2});
static_assert(std::int64_t{3} * v == Vector{21, -21, 12});

} // namespace

int main() {}
```

## Products

The four products take two multivectors of one algebra and one scalar type.
Each is worked out at compile time, for each pair of layouts a program uses,
into a list of terms, so a product costs only the terms its layouts can
reach.

- The geometric product a ⟑ b, `geometric_product(a, b)` or `a * b`, is the
  associative product fixed by e_i ⟑ e_j + e_j ⟑ e_i = 2 g_ij, so a
  generator squares to its entry on the diagonal of the metric.
- The wedge a ∧ b, `wedge(a, b)` or `a ^ b`, is the exterior product:
  e_A ∧ e_B is the blade of A ∪ B, with the sign of the reordering, when A
  and B share no generator, and zero otherwise. It does not depend on the
  metric. In rigid(3) it is the join: two points give the line through them.
- The geometric antiproduct a ⟇ b, `geometric_antiproduct(a, b)` or
  `a % b`, is left(right(a) ⟑ right(b)), with the complements of
  [involutions and complements](#involutions-and-complements). Its identity
  is the antiscalar 𝟙. Motors and flectors act through it.
- The antiwedge a ∨ b, `antiwedge(a, b)` or `a & b`, is
  left(right(a) ∧ right(b)). In rigid(3) it is the meet: a line and a plane
  meet in a point, and a point and a plane in a scalar that is zero when the
  point lies on the plane.

The products and the antiproducts are associative, and the complements
carry one to the other: right(a ⟑ b) = right(a) ⟇ right(b) and
right(a ∧ b) = right(a) ∨ right(b). Each result is on the canonical layout
of the blades it can reach; see [result layouts](#result-layouts).

```cpp
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct e3 {
  static constexpr auto value = core::R<3, 0, 0>();
};
using Vector = core::mv<e3, core::blades<e3, "e1 e2 e3">, int>;
using Even = core::mv<e3, core::blades<e3, "1 e12 e13 e23">, int>;
using Bivector = core::mv<e3, core::blades<e3, "e12 e13 e23">, int>;

constexpr Vector a{1, 2, 0};
constexpr Vector b{3, 1, 0};

// a b = a . b + a ^ b: a . b = 1 * 3 + 2 * 1 = 5, and a ^ b has
// 1 * 1 - 2 * 3 = -5 on e12.
static_assert(Even{a * b} == Even{5, -5, 0, 0});
static_assert(Bivector{a ^ b} == Bivector{-5, 0, 0});

// The wedge is antisymmetric on vectors, and a vector wedged with a multiple
// of itself is zero.
static_assert(Bivector{a ^ b} == -Bivector{b ^ a});
static_assert(Bivector{a ^ (3 * a)} == Bivector{});

struct pga3 {
  static constexpr auto value = core::rigid<3>();
};
using Point = core::mv<pga3, core::blades<pga3, "e1 e2 e3 e4">, int>;
using Plane = core::mv<pga3, core::blades<pga3, "e423 e431 e412 e321">, int>;

// The plane z = 1 holds the points where z - w = 0: normal (0, 0, 1) and
// offset -1. The meet of a point and a plane is gx x + gy y + gz z + gw w on
// the scalar: zero for (2, 3, 1, 1), and 4 - 1 = 3 for (0, 0, 4, 1).
constexpr Plane g{0, 0, 1, -1};
static_assert(core::get<"1">(Point{2, 3, 1, 1} & g) == 0);
static_assert(core::get<"1">(Point{0, 0, 4, 1} & g) == 3);

// The antiscalar is the identity of the antiproduct.
using Antiscalar = core::mv<pga3, core::blades<pga3, "e1234">, int>;
constexpr Point p{1, 2, 3, 1};
static_assert(Point{Antiscalar{1} % p} == p);
static_assert(Point{p % Antiscalar{1}} == p);

} // namespace

int main() {}
```

Two operands of different algebras or scalar types are rejected with a
message that names the product as written, such as `a * b`. A number is not
an operand of a product; scale by a number with `s * x`, as in [sums and
scaling](#sums-and-scaling).

## Operators and their precedence

Named functions are the interface, and each operator is a one-line alias of
one of them:

| Operator | Meaning | Named function |
| --- | --- | --- |
| `a * b` | geometric product ⟑ | `geometric_product` |
| `a % b` | geometric antiproduct ⟇ | `geometric_antiproduct` |
| `a ^ b` | wedge ∧ | `wedge` |
| `a & b` | antiwedge ∨, the meet | `antiwedge` |
| `~a` | reverse | `reverse` |
| `!a` | right complement | `right_complement` |
| `a + b`, `a - b`, `-a` | sum, difference, negation | |
| `s * a`, `a * s`, `a / s` | scaling by a scalar s | |

The antireverse, the left complement, the duals, the dot and the antidot,
the projections, `transform`, and `compose` have no operator.

C++ fixes the precedence of each operator, and two of them surprise. `^` and
`&` bind more loosely than `==`, `+`, and `*`, so `p ^ q == l` means
`p ^ (q == l)` and `a ^ b + c` means `a ^ (b + c)`: parenthesize every join
and meet that stands beside another operator. `%` binds as `*` does, left to
right. `!x` is a multivector, not a `bool`, so `if (!x)` does not compile.
Like a product's, its result is on its own canonical layout, so compare it
with a named type after converting it, `Plane{!x} == y`, and with a value of
its own layout in parentheses, `(!x) == y`, since GCC warns that `!x == y`
may mean `!(x == y)`.

```cpp
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct e3 {
  static constexpr auto value = core::R<3, 0, 0>();
};
using Vector = core::mv<e3, core::blades<e3, "e1 e2 e3">, int>;
using Bivector = core::mv<e3, core::blades<e3, "e12 e13 e23">, int>;

constexpr Vector a{1, 0, 0};
constexpr Vector b{0, 1, 0};
constexpr Vector c{0, 0, 1};

// The wedge is associative: both groupings give e123.
static_assert(((a ^ b) ^ c) == (a ^ (b ^ c)));

// (a ^ b) + (b ^ c) is e12 + e23, in parentheses on both sides of +.
static_assert(Bivector{(a ^ b) + (b ^ c)} == Bivector{1, 0, 1});

// a % b * c is (a % b) * c, as for a * b * c.
static_assert((a % b * c) == ((a % b) * c));

} // namespace

int main() {}
```

## Sums and scaling

`x + y`, `x - y`, and `-x` work on two multivectors of one algebra and
scalar type. Two values of one type give that type, place by place, so the
sum of two lines is a line with the line's orientations. Values of two
different layouts give the canonical layout of the blades either holds;
convert it to a named type to read it under that type's orientations. A
multivector and a number have no sum: `x + 1.0` is rejected with a message.
To add a number, make it a multivector on the scalar blade,
`mv<Tag, blades<Tag, "1">, T>{s}`, and add that.

`s * x` multiplies each coefficient on the left, s x_i, and `x * s` on the
right, x_i s, so a scalar whose product does not commute keeps its order.
`x / s` is described in [scalars](#scalars). `-x` negates each coefficient
with the scalar's unary minus, so over IEEE 754 a +0.0 becomes −0.0. Each
coefficient is computed with one of the scalar's own operations, so these
round only as the scalar does.

```cpp
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct pga3 {
  static constexpr auto value = core::rigid<3>();
};
using Line =
    core::mv<pga3, core::blades<pga3, "e41 e42 e43 e23 e31 e12">, double>;
using Antiscalar = core::mv<pga3, core::blades<pga3, "e1234">, double>;

constexpr Line a{0, 0, 1, 0, 0, 0};
constexpr Line b{1, 0, 0, 0, 0, 0};

// A sum of two Lines is a Line.
constexpr Line c = a + b;
static_assert(c == Line{1, 0, 1, 0, 0, 0});
static_assert(2.0 * a == Line{0, 0, 2, 0, 0, 0} && a / 4.0 == a * 0.25);
static_assert(-a == Line{0, 0, -1, 0, 0, 0});

// A Line plus the antiscalar is on the canonical layout e12, e13, e14, e23,
// e24, e34, e1234, where a's 1 on e43 = -e34 is -1 on e34.
constexpr auto m = a + Antiscalar{1};
static_assert(core::get<"e34">(m) == -1.0 && core::get<"e1234">(m) == 1.0);

} // namespace

int main() {}
```

```cpp rejected
// rejected: a + b: a multivector and a number have no sum
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct e3 {
  static constexpr auto value = core::R<3, 0, 0>();
};
using Vector = core::mv<e3, core::blades<e3, "e1 e2 e3">, double>;

} // namespace

Vector shifted(const Vector& v) { return v + 1.0; }

int main() { return shifted(Vector{}) == Vector{} ? 0 : 1; }
```

## Involutions and complements

The involutions multiply each blade by a sign that depends only on its grade
or antigrade, so each keeps its input's type:

- the reverse, `reverse(x)` or `~x`, multiplies a blade of grade k by
  (−1)^(k(k−1)/2), which reverses the order of its generators;
- the antireverse, `antireverse(x)`, uses the antigrade m in place of the
  grade: (−1)^(m(m−1)/2);
- the grade involution, `grade_involution(x)`, multiplies a blade of grade k
  by (−1)^k.

The complements send each blade to the blade of the generators it lacks:

- the right complement, `right_complement(x)` or `!x`, is fixed on each
  basis blade u by u ∧ right(u) = 𝟙;
- the left complement, `left_complement(x)`, by left(u) ∧ u = 𝟙.

They differ by a sign: left(u) = (−1)^(gr(u) ag(u)) right(u), so they agree
when n is odd. Each undoes the other: left(right(x)) = right(left(x)) = x.
Applied twice, the right complement gives each blade times
(−1)^(gr(u) ag(u)), not x. The complements read only the number of
generators, never the metric, and their result is on the canonical layout of
the blades they reach.

```cpp
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct pga3 {
  static constexpr auto value = core::rigid<3>();
};
using Point = core::mv<pga3, core::blades<pga3, "e1 e2 e3 e4">, int>;
using Line =
    core::mv<pga3, core::blades<pga3, "e41 e42 e43 e23 e31 e12">, int>;
using Plane = core::mv<pga3, core::blades<pga3, "e423 e431 e412 e321">, int>;

constexpr Line l{1, 2, 3, 4, 5, 6};

// A bivector has grade 2 and, in four generators, antigrade 2:
// (-1)^(2 * 1 / 2) = -1 for both reverses, and (-1)^2 = 1 for the grade
// involution.
static_assert(~l == -l && core::antireverse(l) == -l);
static_assert(core::grade_involution(l) == l);

// u ^ right(u) = e1234 gives right(e1) = e234 = e423 and right(e4) =
// -e123 = e321, and likewise for e2 and e3: the right complement of a point
// is the plane with its four coefficients.
constexpr Point p{1, 2, 3, 1};
static_assert(Plane{!p} == Plane{1, 2, 3, 1});

// A vector of rigid(3) has gr * ag = 1 * 3, so left(u) = -right(u), and two
// right complements give -p. The left complement undoes the right one.
static_assert(Plane{core::left_complement(p)} == Plane{-1, -2, -3, -1});
static_assert(Point{!!p} == -p);
static_assert(Point{core::left_complement(!p)} == p);

} // namespace

int main() {}
```

## Bulk, weight, and their duals

The metric exomorphism G sends a blade e_A to the sum, over the blades e_B
of its grade, of det g[A, B] e_B, the determinant of the inner products of
A's generators with B's. In a diagonal metric it multiplies each blade by the
product of its generators' squares. Then:

| Function | Value |
| --- | --- |
| `bulk(x)` | G x |
| `weight(x)` | left(G right(x)), the antiexomorphism |
| `bulk_dual(x)` | right(G x) |
| `weight_dual(x)` | right(weight(x)), which is G right(x) |
| `left_bulk_dual(x)` | left(G x) |
| `left_weight_dual(x)` | left(weight(x)) |

In rigid(3), where e1, e2, e3 square to 1 and e4 to 0, the bulk is the part
of x without e4 and the weight the part with it, so bulk(x) + weight(x) is x,
and a ∧ bulk_dual(b) is the dot of a and b times 𝟙 for a and b of one grade.
In other metrics the two need not split x. With no null generator, as in
R(n, 0, 0), both keep every blade. In conformal(3), where e4 and e5 are null
with e4 · e5 = −1, both move blades: the bulk of e4 is −e5. With two or more
null generators, some blades are in neither. The duals built on the left
complement exist only under their own names. In a degenerate metric the
double dual is zero, so only the complements can be inverted.

None of these checks anything: the bulk and the weight drop blades by
definition, and the duals drop nothing.

```cpp
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct pga3 {
  static constexpr auto value = core::rigid<3>();
};
using Point = core::mv<pga3, core::blades<pga3, "e1 e2 e3 e4">, int>;
using Plane = core::mv<pga3, core::blades<pga3, "e423 e431 e412 e321">, int>;

constexpr Point p{1, 2, 3, 1};

// The bulk is the part without e4, the weight the part with it.
static_assert(core::get<"e1">(core::bulk(p)) == 1);
static_assert(core::get<"e3">(core::bulk(p)) == 3);
static_assert(core::get<"e4">(core::weight(p)) == 1);

// bulk_dual takes the complement of (1, 2, 3, 0): the plane (1, 2, 3, 0).
// weight_dual takes that of (0, 0, 0, 1): the plane (0, 0, 0, 1). The left
// duals negate both, since left(u) = -right(u) for a vector of rigid(3).
static_assert(Plane{core::bulk_dual(p)} == Plane{1, 2, 3, 0});
static_assert(Plane{core::weight_dual(p)} == Plane{0, 0, 0, 1});
static_assert(Plane{core::left_bulk_dual(p)} == Plane{-1, -2, -3, 0});
static_assert(Plane{core::left_weight_dual(p)} == Plane{0, 0, 0, -1});

// In conformal(3) the bulk of the origin e4 is -e5.
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
using Origin = core::mv<cga3, core::blades<cga3, "e4">, int>;
static_assert(core::get<"e5">(core::bulk(Origin{1})) == -1);

} // namespace

int main() {}
```

## Dot and antidot

`dot(a, b)` is a • b, the sum of a_A G_AB b_B over the blades of a and b,
on the scalar. Blades of different grades contribute nothing. It is
Lengyel's dot: in a Euclidean metric e12 • e12 is +1, where the scalar part
of e12 e12, which many libraries call the scalar or inner product, is −1.
`dot(x, x)` is the squared bulk norm of x. `antidot(a, b)` is a ∘ b, the
same with the antiexomorphism, times 𝟙; `antidot(x, x)` is the squared weight
norm times 𝟙, w² 𝟙 for a point of rigid(3). Both return multivectors, not
numbers: read them with `get<"1">` and `get<"e1234">`. When the metric pairs
no blade of one operand with any blade of the other, the result is on the
empty layout, and its type says that it is zero.

```cpp
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct pga3 {
  static constexpr auto value = core::rigid<3>();
};
using Point = core::mv<pga3, core::blades<pga3, "e1 e2 e3 e4">, int>;
using Line =
    core::mv<pga3, core::blades<pga3, "e41 e42 e43 e23 e31 e12">, int>;

constexpr Point p{1, 2, 3, 2};

// e4 squares to 0, so only x, y, z count: 1 + 4 + 9 = 14. The antidot
// counts only the weight: 2 * 2 = 4.
static_assert(core::get<"1">(core::dot(p, p)) == 14);
static_assert(core::get<"e1234">(core::antidot(p, p)) == 4);

// A point and a line share no grade, so their dot has no blade at all.
static_assert(decltype(core::dot(p, Line{}))::component_count == 0);

struct e3 {
  static constexpr auto value = core::R<3, 0, 0>();
};
using E12 = core::mv<e3, core::blades<e3, "e12">, int>;

// Lengyel's dot: e12 . e12 = +1, while e12 e12 = -1.
static_assert(core::get<"1">(core::dot(E12{1}, E12{1})) == 1);
static_assert(core::get<"1">(E12{1} * E12{1}) == -1);

} // namespace

int main() {}
```

## Projection and antiprojection

`project(a, b)` is b ∨ (a ∧ weight_dual(b)), the projection of the object a
onto b. In rigid(3) it moves a point onto a plane or a line along the
plane's or the line's normal direction, and a point projected onto a
unitized plane, one whose normal has length 1, keeps its weight.
`antiproject(a, b)` is b ∧ (a ∨ weight_dual(b)), which carries a through b:
a plane antiprojected onto a unitized point is the plane through the point
with the same normal, and a line, the line through the point with the same
direction. Onto a point of weight w, the result is w² times that. The object
comes first in both. Neither checks anything.

```cpp
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct pga3 {
  static constexpr auto value = core::rigid<3>();
};
using Point = core::mv<pga3, core::blades<pga3, "e1 e2 e3 e4">, int>;
using Plane = core::mv<pga3, core::blades<pga3, "e423 e431 e412 e321">, int>;

// The plane z = 2, with unit normal (0, 0, 1): z - 2 w = 0.
constexpr Plane g{0, 0, 1, -2};

// (1, 1, 7) projected onto it along the normal lands at (1, 1, 2), with its
// weight 1.
static_assert(Point{core::project(Point{1, 1, 7, 1}, g)} == Point{1, 1, 2, 1});

// The plane through (1, 2, 5) with g's normal is z - 5 = 0.
static_assert(Plane{core::antiproject(g, Point{1, 2, 5, 1})} ==
              Plane{0, 0, 1, -5});

} // namespace

int main() {}
```

## Transform and compose

A versor moves objects through the antiproduct. Its parity is the parity of
the antigrades n − k of its blades: a motor is even and a flector odd. In
rigid(3) grade and antigrade have the same parity; where n is odd they do
not. `transform(x, V)` applies V to x, with V̰ the antireverse of V:

- an even V gives V ⟇ x ⟇ V̰;
- an odd V gives −V ⟇ x̌ ⟇ V̰, where x̌, the antigrade involution, multiplies
  a blade of antigrade m by (−1)^m. When n is even x̌ is the grade
  involution, and when n is odd it is minus the grade involution. So in
  rigid(3) a flector reflects a point and keeps its weight positive, where
  the form −F ⟇ x ⟇ F̰ would negate it.

The result has x's type: `transform(line, V)` is a line. The part of the
antiproducts on blades x lacks is dropped; for a versor it is zero, and in
[checking mode](#checking-mode) `transform` checks that it is. A V with
blades of both parities is rejected at compile time.

For a versor with V ⟇ V̰ = λ𝟙 and λ ≠ 0, the transform T satisfies
λ^(k−1) T(a1 ∧ … ∧ ak) = T(a1) ∧ … ∧ T(ak), so lines and planes move as the
points that span them when λ = 1, as for a unitized motor or flector.
`transform` does not unitize: a versor with λ = 2 doubles the weight of a
point it moves, which is still the same point. There is no `unitize` in
this release.

`compose(Q2, Q1)` is Q2 ⟇ Q1, the versor that applies Q1 first and then Q2:
`transform(x, compose(Q2, Q1))` is `transform(transform(x, Q1), Q2)`. Each
operand must have one parity. Two motors or two flectors compose to a motor,
and a motor and a flector to a flector, since antigrades add modulo 2 under
the antiproduct. The result is on its own canonical layout; convert it,
`Motor{compose(q2, q1)}`.

```cpp
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct pga3 {
  static constexpr auto value = core::rigid<3>();
};
using Point = core::mv<pga3, core::blades<pga3, "e1 e2 e3 e4">, int>;
using Motor =
    core::mv<pga3, core::blades<pga3, "e41 e42 e43 e1234 e23 e31 e12 1">, int>;
using Flector = core::mv<
    pga3, core::blades<pga3, "e1 e2 e3 e4 e423 e431 e412 e321">, int>;

constexpr Point p{1, 0, 0, 1};

// Half of (2, 4, 6) on e23, e31, e12 and 1 on e1234: a move by (2, 4, 6).
constexpr Motor move{0, 0, 0, 1, 1, 2, 3, 0};
static_assert(core::transform(p, move) == Point{3, 4, 6, 1});

// sin 45 and cos 45 degrees on e43 and e1234, times the square root of 2:
// a quarter turn about the z axis with lambda = 2, so the weight doubles.
constexpr Motor turn{0, 0, 1, 1, 0, 0, 0, 0};
static_assert(core::transform(p, turn) == Point{0, 2, 0, 2});

// Turn first, then move: (0, 2, 0) at weight 2 moves by 2 (2, 4, 6).
constexpr Motor both{core::compose(move, turn)};
static_assert(core::transform(p, both) == Point{4, 10, 12, 2});
static_assert(core::transform(p, both) ==
              core::transform(core::transform(p, turn), move));

// The plane z = 0 as a flector reflects (1, 2, 3) to (1, 2, -3), keeping
// the weight +1. Two reflections in one plane compose to the identity motor.
constexpr Flector mirror{0, 0, 0, 0, 0, 0, 1, 0};
static_assert(core::transform(Point{1, 2, 3, 1}, mirror) == Point{1, 2, -3, 1});
static_assert(Motor{core::compose(mirror, mirror)} ==
              Motor{0, 0, 0, 1, 0, 0, 0, 0});

} // namespace

int main() {}
```

```cpp rejected
// rejected: transform: the versor holds blades of both parities, even and odd antigrade
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct pga3 {
  static constexpr auto value = core::rigid<3>();
};
using Point = core::mv<pga3, core::blades<pga3, "e1 e2 e3 e4">, double>;
// 1 has antigrade 4 and e1 antigrade 3.
using Mixed = core::mv<pga3, core::blades<pga3, "1 e1">, double>;

} // namespace

Point moved(const Point& p, const Mixed& v) { return core::transform(p, v); }

int main() { return moved(Point{}, Mixed{}) == Point{} ? 0 : 1; }
```

For the antiproduct of anything, without the parity rule, write
`geometric_antiproduct` or `%`.

## Checking mode

Some preconditions cannot be expressed in the types. Galoot checks them in
every constant evaluation, which then fails to compile, and at run time only
in checking mode, when the macro `GALOOT_CHECKING` is 1, as with
`-DGALOOT_CHECKING=1`. With no value it is 0, so a project that only adds the
headers gets an unchecked build. A failed check at run time writes
`galoot: assertion failed: <message> (<file>:<line>)` to the standard error
stream and calls `std::abort`; the core throws no exceptions. With
`GALOOT_CHECKING` 0 the conditions are not evaluated at run time, and a
violated precondition is undefined behavior or a wrong result, as each
function's comment says.

What is checked:

- `truncate`, for an exact scalar, checks that what it drops is zero. It
  never checks a rounding scalar such as `double`.
- `transform`, for an exact scalar, checks that what it drops is zero; the
  message is "transform: v does not act as a versor on x". For `float` and
  `double`, it checks that each dropped coefficient lies within the
  operation's forward error bound; the message is "transform: v is not a
  versor on x within rounding". [The limits page](limits.md#the-rounding-check)
  gives the scope of that check.
- An integer division `x / s` checks that s is not zero.
- The exact coefficients of a product are checked to fit
  `coefficient_type`, a 64-bit signed integer, as they are formed.
- The rga3 preset's `rotation` checks that the axis has a finite, nonzero
  length.

A checking build under `-ffast-math` or `-ffinite-math-only` does not
compile, because the rounding check needs IEEE 754 arithmetic with NaN and
infinity.

`GALOOT_ASSERT(condition, message)` is the macro the checks use, and code of
your own may use it too. `message` must be a string literal that states the
precondition, contains no quote characters, and starts on the line of
`GALOOT_ASSERT(`, because a failed constant evaluation shows the message only
in the source line the compiler echoes.

```cpp
#include <galoot/core/assert.hpp>

namespace {

// In a constant evaluation a failed check stops the compile; at run time it
// aborts in checking mode.
constexpr int half_of_even(int n) {
  GALOOT_ASSERT(n % 2 == 0, "half_of_even: n must be even");
  return n / 2;
}

static_assert(half_of_even(6) == 3);

} // namespace

int main() { return half_of_even(4) == 2 ? 0 : 1; }
```

## Forced inlining

The core marks its four products and their operators, `transform`,
`compose`, `project`, `antiproject` and a few others, and the functions they
call, to be inlined at every call, with GCC's and Clang's `always_inline`
attribute, so that an operation on the rga3 types compiles
at `-O2` to the arithmetic of its terms where it is called, as code written
by hand would. The cost is paid at compile time: every call carries the
operation's body, so a file with very many calls, such as a test that
checks identities over many algebras, can take much longer and much more
memory to compile, most of all with sanitizers or at `-O2` with Clang, and
at `-O0` an object grows.

The macro `GALOOT_FORCE_INLINE` turns this off. It is 1 by default; defined
as 0, as with `-DGALOOT_FORCE_INLINE=0`, it leaves every inlining decision
to the compiler, and an operation may then compile to a call.
`-DGALOOT_FORCE_INLINE` with no value defines it as 1. A word such as `ON`
or `true`, another number such as `2` or `01`, or an empty value
(`-DGALOOT_FORCE_INLINE=`) stops the compile with "GALOOT_FORCE_INLINE must
be 0 or 1"; a spelling such as `-1` stops it with an error of the
preprocessor's own.
Results over an exact scalar are the same in both modes, and so are results
over `float` or `double` when the compiler does not contract a multiply and
an add into one instruction (`-ffp-contract=off`); with contraction, which
products are fused can depend on what is inlined. Every translation unit of
a program must use the same mode, since the two modes give the core's
functions different definitions.

## Printing

`<galoot/core/format.hpp>` gives every multivector a `std::formatter`, so
`std::format`, `std::print`, and `std::println` write it. Neither
`<galoot/core.hpp>` nor a preset's header includes it, because `<format>`
costs more to compile than the rest of the core. A value prints as
`{name: value, ...}`: every component, zeros included, in the order of its
layout, each as its blade's name, a colon and a space, and its coefficient
as the scalar's own formatter writes it. An empty layout prints `{}`. A
format specification, such as `{:.3f}`, applies to each coefficient, and so
do width and fill; to pad the whole text, format it to a string first.

A blade prints by the name the algebra tag gives it, when the tag has a
member `names`, a constant text that lists oriented names as `blades` reads
them, and one of them has the orientation the layout stores. Otherwise it
prints by its canonical name, 1 or e and its indices in increasing order,
with a leading `-` when the layout stores it with orientation −1: a
coefficient on e41 = −e14 prints as `-e14`. A floating −0.0, which a join
often leaves, prints as `-0`. Names have one digit per index, so a
multivector of more than 9 generators does not print; format its
`coefficients` instead.

```cpp
#include <print>

#include <galoot/core.hpp>
#include <galoot/core/format.hpp> // IWYU pragma: keep

namespace core = galoot::core;

namespace {

// The same algebra twice, the second with names for its blades.
struct plain {
  static constexpr auto value = core::rigid<3>();
};
struct named {
  static constexpr auto value = core::rigid<3>();
  static constexpr auto names = "e41 e42 e43 e23 e31 e12";
};

template <class Tag>
using Line = core::mv<Tag, core::blades<Tag, "e41 e42 e43 e23 e31 e12">, int>;

} // namespace

int main() {
  std::println("{}", Line<plain>{0, 0, 1, 0, 0, 0});
  // prints: {-e14: 0, -e24: 0, -e34: 1, e23: 0, -e13: 0, e12: 0}
  std::println("{}", Line<named>{0, 0, 1, 0, 0, 0});
  // prints: {e41: 0, e42: 0, e43: 1, e23: 0, e31: 0, e12: 0}
  std::println("{:>2}", Line<named>{0, 0, 1, 0, 0, 0});
  // prints: {e41:  0, e42:  0, e43:  1, e23:  0, e31:  0, e12:  0}
}
```

A class derived from a multivector is not printable until it is given its
base's formatter, in one line:
`template <> struct std::formatter<Derived, char> : std::formatter<Base, char> {};`.

## Change of basis

Two algebras with the same number of generators are isomorphic when a change
of basis carries the inner products of one to those of the other, and
`change_basis` writes a multivector of the first in the generators of the
second. Include `<galoot/core/basis_change.hpp>` for it. A change is a tag,
like an algebra: it names its `source` and `target` algebra tags and holds a
`value` of type `basis_change<n>`. Row i of the change writes the source's
generator e_i in the target's generators e′1 to e′n, as integers over one
positive denominator per row, so no entry is ever rounded. `from_rows(rows,
denominators)` builds one, and the first use of the tag checks, exactly,
that the change has an inverse and keeps every inner product, each with a
message. `change_basis_back` applies the inverse, which the library computes
from the rows.

The change sends a blade e_A to the wedge of the images of its generators,
so it keeps grades, sends 1 to 1′, and sends 𝟙 to det(P) 𝟙′, where P is the
matrix of the change. A `basis_change` has no `==`, because two values for
one matrix can differ: a row scaled together with its denominator is the
same matrix. `same_rows(a, b)` compares two values as written.

Writing `a == b` or `a != b` for two changes does not compile. The compiler
reports that `==` is deleted, for `a != b` too, since the language rewrites
`a != b` through `==`, and in a note shows the line of the header that
deletes it, which ends with `// use same_rows(a, b)`.

Below, the two generators square to zero and have inner product 1, the
relations of the operators that create and destroy one fermion,
f f = 0 and f f† + f† f = 1, with the form doubled, since its entries are
integers. A change takes them to R(1, 1, 0):

```cpp
#include <galoot/core.hpp>
#include <galoot/core/basis_change.hpp>

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

// The first row doubled over a doubled denominator is the same matrix,
// written differently.
static_assert(core::same_rows(witt_to_r110::value,
                              core::from_rows({{1, 1}, {1, -1}}, {1, 2})));
static_assert(!core::same_rows(witt_to_r110::value,
                               core::from_rows({{2, 2}, {1, -1}}, {2, 2})));

} // namespace

int main() {}
```

The results are exact over `int` because the even layout's image needs no
division. A layout whose image does is rejected with a message over an
integer scalar, as `change_basis<witt_to_r110>(g)` is, since e2 maps to
(e′1 − e′2) / 2. A floating scalar type, or a rational type of your own
whose division is exact, takes every layout; over `double` each coefficient
rounds as one sum of products and one division.

`conformal_change<D>()` is the change from conformal(D), whose origin
e(D+1) and point at infinity e(D+2) square to zero with
e(D+1) · e(D+2) = −1, to its diagonal counterpart R(D+1, 1, 0):
e_i = e′_i for i up to D, the origin is (e′(D+2) − e′(D+1)) / 2, and the
point at infinity is e′(D+1) + e′(D+2), where e′(D+1) squares to +1 and
e′(D+2) to −1. Its determinant is −1. A value prints with the same blade
names in either algebra, so the comments say which one it is in:

```cpp
#include <print>

#include <galoot/core.hpp>
#include <galoot/core/basis_change.hpp>
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
the same result for them. With φ for the change and d = det(P), the
antiwedge and the bulk dual change by a factor: φ(a ∨ b) = φ(a) ∨ φ(b) / d
and φ(bulk_dual(x)) = d bulk_dual(φ(x)), and the same for the left bulk
dual. The antiproduct, the antidot, the weight duals, and `compose` keep the
factor 1/d, and `transform`, `project`, and `antiproject` the factor 1/d²,
exactly when the inverse transpose of P keeps the inner products as well;
that holds for every change between forms whose Gram matrices are their own
inverses, such as `conformal_change` and the change above. The complements,
the bulk, and the weight are defined on the coordinates of one basis and are
not preserved in general: in conformal(3) the bulk of the origin e4 is −e5,
while computed in R(4, 1, 0) and mapped back it is −e5 / 2. The image of a
layout can also hold more blades than the layout: the origin alone maps onto
e′4 and e′5, so its round trip comes back on e4 and e5, with zero on e5, and
`truncate<Type>(x)` reads it as its own type.

## Calling the functions

The core's functions are found by argument-dependent lookup, through the
multivector types of their arguments, so most calls need no namespace. Three
cases need one:

- Under `using namespace std::ranges` or `using namespace std::views`, an
  unqualified `transform(x, v)` or `reverse(x)` finds the standard library's
  function object, which turns off argument-dependent lookup, so the call
  does not compile. Write `galoot::core::transform(x, v)`, or `~x` for the
  reverse. Under `using namespace std` the calls work, since `std::transform`
  and `std::reverse` take iterators.
- A variable or a type named `part`, `bulk`, `weight`, or `dot` in scope
  stops an unqualified call of that function: write `galoot::core::bulk(x)`.
  Under `using namespace std::execution`, an unqualified `bulk(x)` finds the
  standard library's.
- A function whose arguments are plain numbers names no namespace of
  Galoot, so it is never found this way; the rga3 preset's `translation` is
  one.

```cpp
#include <algorithm>
#include <array>

#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct e3 {
  static constexpr auto value = core::R<3, 0, 0>();
};
using Bivector = core::mv<e3, core::blades<e3, "e12 e13 e23">, int>;

} // namespace

int main() {
  using namespace std::ranges;

  // Unqualified, reverse is std::ranges::reverse here: it reverses a range.
  std::array<int, 3> values{1, 2, 3};
  reverse(values);

  // So Galoot's is named in full. The reverse of a bivector is its negation.
  const Bivector b{1, 2, 3};
  const Bivector r = galoot::core::reverse(b);
  return (values[0] == 3 && r == -b && ~b == r) ? 0 : 1;
}
```

## Writing generic code

Galoot rejects a misuse with an overload of its own that holds a
`static_assert`, so the error is one line in the terms of the algebra. The
price falls on generic code: a requires-expression that asks whether such a
call is valid is told yes, because an overload exists. Ask a question that
does not compile the call:

- whether `get<"name">` accepts a name: `has_name<X, "name">`;
- whether two multivectors can be multiplied or added: whether they have the
  same `algebra_type` and `scalar_type`;
- whether `s * x` is valid: whether s has x's `scalar_type` exactly;
- whether a type is a multivector at all: the concept `multivector`.

`truncate<double>(x)` and `part<double>(x)` are worse: their return types
are deduced, so even asking compiles the message, and the program fails
there.

```cpp
#include <concepts>

#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct pga3 {
  static constexpr auto value = core::rigid<3>();
};
using Point = core::mv<pga3, core::blades<pga3, "e1 e2 e3 e4">, double>;

// A point has no e5, but a requires-expression cannot tell: get's rejecting
// overload matches. has_name answers.
static_assert(requires(Point p) { core::get<"e5">(p); });
static_assert(!core::has_name<Point, "e5"> && core::has_name<Point, "e4">);

// Ask about scalar types by comparing them.
template <class X, class S>
concept scales =
    core::multivector<X> && std::same_as<S, typename X::scalar_type>;
static_assert(scales<Point, double> && !scales<Point, int>);
static_assert(core::multivector<Point> && !core::multivector<double>);

} // namespace

int main() {}
```

## Result layouts

A layout that a user names, such as a line's, holds its blades in the order
and orientation the user wrote. The core's results do not know those names.
A product, a sum of two layouts, a complement, a dual, a projection, a
composition, and a change of basis each return a multivector on the
canonical layout of the blades the operation can reach: each blade once,
with orientation +1, by grade and then lexicographically by index list,
e12, e13, e14, e23, e24, e34. Its type is
`mv<Tag, wedge_layout<Tag, A, B>, T>` for a wedge, and likewise with the
layout named for each operation, as the [reference](#reference) lists. The
reverse, the antireverse, the grade involution, a sum or a scaling of one
type, and `transform` keep their input's layout.

So `auto j = p ^ q;` keeps the canonical layout, prints by canonical names,
and is read with names such as `"e24"`, while `Line{p ^ q}` is read and
printed under the line's own names, such as `"e42"`, with the opposite sign.
A layout holds only blades that can be nonzero, so the type of a result says
which parts are zero: a dot of a point and a line has the empty layout.

```cpp
#include <concepts>

#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct pga3 {
  static constexpr auto value = core::rigid<3>();
};
constexpr auto point_layout = core::blades<pga3, "e1 e2 e3 e4">;
using Point = core::mv<pga3, point_layout, int>;
using Join =
    core::mv<pga3, core::wedge_layout<pga3, point_layout, point_layout>, int>;

constexpr Point p{0, 0, 0, 1};
constexpr Point q{0, 1, 0, 1};

// The join of two points is on the six bivectors in canonical order.
static_assert(std::same_as<decltype(p ^ q), Join>);
static_assert(Join::layout == core::of_grade<4, 2>);

// The direction q - p = (0, 1, 0) is -1 on e24 = -e42.
static_assert(core::get<"e24">(p ^ q) == -1);

} // namespace

int main() {}
```

## Reference

One entry for each public name of the core, in the order of the headers.
Each says what it is and where this guide explains it. The header comment
before each declaration states its full requirements and guarantees.

### `GALOOT_CHECKING` and `GALOOT_ASSERT`

`<galoot/core/assert.hpp>`. `GALOOT_CHECKING` is 1 for a checking build and
0, the default, for an unchecked one. `GALOOT_ASSERT(condition, message)`
checks a precondition in every constant evaluation and, when
`GALOOT_CHECKING` is 1, at run time. See [checking mode](#checking-mode).

### `GALOOT_FORCE_INLINE`

`<galoot/core/assert.hpp>`. 1, the default, forces the core's operations
inline on GCC and Clang; 0 leaves inlining to the compiler. See
[forced inlining](#forced-inlining).

### `mask_t` and `mask_bits`

`<galoot/core/bits.hpp>`. `mask_t` is the unsigned 32-bit type of a blade
mask, in which bit i − 1 stands for e_i; `mask_bits`, 32, is the number of
generators a mask can hold. See [layouts](#layouts).

### `grade` and `antigrade`

`grade(a)` is the number of generators of the blade of the mask a, and
`antigrade(a, n)` is n minus it, for a blade of an algebra of n generators.
Both are `int`.

### `reorder_sign`

`reorder_sign(a, b)` is −1 when the number of pairs of a generator e_i of A
and e_j of B with i > j is odd, and +1 otherwise: for A and B with no common
generator, e_A ∧ e_B = `reorder_sign(a, b)` e_(A ∪ B).

### `complement_mask`, `right_complement_sign`, and `left_complement_sign`

`complement_mask(a, n)` is the mask of the generators of e1 to en that a
lacks. The two signs s give the complements s e_C of the blade of a, fixed by
u ∧ right(u) = 𝟙 and left(u) ∧ u = 𝟙. See [involutions and
complements](#involutions-and-complements).

### `reverse_sign`, `antireverse_sign`, and `grade_involution_sign`

The signs by which the reverse, the antireverse, and the grade involution
multiply the blade of a mask: (−1)^(k(k−1)/2) for grade k, the same with the
antigrade, and (−1)^k.

### `metric` and `metric_like`

`<galoot/core/metric.hpp>`. `metric<N>` holds the Gram matrix `gram` of N
generators as integers, with `generator_count`, `is_symmetric()`,
`entries_in_range()`, `is_diagonal()`, `is_null(i)`, and `==`.
`metric_like<Tag>` says whether a type is an algebra tag: a `static
constexpr` member `value` that is a valid metric. See [declaring an
algebra](#declaring-an-algebra).

### `R`

`R<P, Q, Z, MaxN = max_generators>()`: the diagonal signature R(p, q, z).
See [declaring an algebra](#declaring-an-algebra).

### `rigid`

`rigid<D, MaxN = max_generators>()`: R(D, 0, 1), with the null generator
e(D+1) last.

### `conformal`

`conformal<D, MaxN = max_generators>()`: e1 to eD Euclidean, the null origin
e(D+1) and point at infinity e(D+2), with e(D+1) · e(D+2) = −1.

### `from_gram`

`from_gram<N, MaxN = max_generators>({{...}, ...})`: the metric of a given
symmetric integer matrix.

### `max_generators`, `max_form_entry`, and `entry_type`

`max_generators`, 16, is the default bound on the number of generators.
`entry_type` is `int`, the type of a Gram matrix entry, and `max_form_entry`
is the largest magnitude an entry may have. See [the limits
page](limits.md#coefficient-range).

### `blade_set` and `oriented_blade`

`<galoot/core/blade_set.hpp>`. `oriented_blade` is a mask and a sign of +1
or −1; `blade_set<K>` is a list of K of them, with `size()` and `==`, usable
as a template argument. See [layouts](#layouts).

### `blades`

`blades<Tag, "names">`: the layout of the names written, in that order, with
their orientations. See [layouts](#layouts).

### `full` and `of_grade`

`full<N>`: every blade of N generators; `of_grade<N, K>`: the blades of
grade K. Both canonical, with sign +1.

### `merge`

`merge<A, B>`: the blades of two layouts, each once, in canonical order,
each with the sign it has in the layout that holds it.

### `index_of`

`index_of<S, Mask>`: the position in S of the blade with that mask,
counting from 0.

### `canonical_name`, `blade_name`, `canonical_less`, and `max_compact_generators`

`canonical_name(mask)` is the canonical name of a blade, `"1"` or e and its
indices in increasing order, as a `blade_name` whose `text()` gives it.
`canonical_less(a, b)` is the canonical order on masks.
`max_compact_generators`, 9, is the largest number of generators whose
blades have names. See [the limits page](limits.md#blade-names).

### `coefficient_type`

`<galoot/core/blade_product.hpp>`. `std::int64_t`, the type of the exact
coefficient of a generated term. A coefficient that leaves its range fails a
check where it is formed. See [the limits page](limits.md#coefficient-range).

### `mv`

`<galoot/core/mv.hpp>`. `mv<Tag, Layout, T>`, the multivector, with
`algebra_type`, `scalar_type`, `layout`, `component_count`, and the array
`coefficients`. See [multivectors](#multivectors) and
[conversions](#conversions).

### `multivector`

The concept that a type, with any cv-qualification and reference removed,
is an `mv`. It never causes an error.

### `ring_scalar` and `exact_scalar`

The concepts of a scalar type and of an exact one. See [scalars](#scalars).

### `truncate`

`truncate<Type>(x)` and `truncate<layout>(x)`: x on a smaller layout,
checking for an exact scalar that what it drops is zero. See
[conversions](#conversions).

### `part`

`part<Type>(x)` and `part<layout>(x)`: the same components as `truncate`,
never checked.

### `get`

`get<"name">(x)`: a reference to the coefficient on the blade of that name.
See [reading and writing
coefficients](#reading-and-writing-coefficients).

### `has_name`

`has_name<X, "name">`: whether `get` accepts the name for the type X. See
[writing generic code](#writing-generic-code).

### `geometric_product` and `*`

`<galoot/core/product.hpp>`. The geometric product. See
[products](#products).

### `wedge` and `^`

The wedge, the exterior product, which is the join in rigid(3).

### `geometric_antiproduct` and `%`

The geometric antiproduct, left(right(a) ⟑ right(b)).

### `antiwedge` and `&`

The antiwedge, left(right(a) ∧ right(b)), which is the meet in rigid(3).

### `geometric_product_layout`, `wedge_layout`, `geometric_antiproduct_layout`, and `antiwedge_layout`

`geometric_product_layout<Tag, A, B>` and the others: the layouts of the
four products' results, for operands on the layouts A and B. See [result
layouts](#result-layouts).

### `reverse` and `~`

`<galoot/core/unary.hpp>`. The reverse, on the input's layout. See
[involutions and complements](#involutions-and-complements).

### `antireverse`

The antireverse, on the input's layout.

### `grade_involution`

The grade involution, on the input's layout.

### `right_complement` and `!`

The right complement, u ∧ right(u) = 𝟙.

### `left_complement`

The left complement, left(u) ∧ u = 𝟙, which undoes the right complement.

### `right_complement_layout` and `left_complement_layout`

The layouts of the complements' results.

### `+`, `-`, and scaling by `*` and `/`

`<galoot/core/linear.hpp>`. Sums, differences, negation, and scaling by a
scalar of the multivector's own type. See [sums and
scaling](#sums-and-scaling).

### `sum_layout`

`sum_layout<Tag, A, B>`: the layout of a sum or a difference of values on A
and B, A itself when the two are equal.

### `bulk` and `weight`

`<galoot/core/dual.hpp>`. The metric exomorphism and the antiexomorphism.
See [bulk, weight, and their duals](#bulk-weight-and-their-duals).

### `bulk_dual` and `weight_dual`

The right complements of the bulk and of the weight.

### `left_bulk_dual` and `left_weight_dual`

The same with the left complement.

### `dot` and `antidot`

The dot a • b on the scalar and the antidot a ∘ b on the antiscalar. See
[dot and antidot](#dot-and-antidot).

### `project` and `antiproject`

The projection and the antiprojection of an object onto another. See
[projection and antiprojection](#projection-and-antiprojection).

### `bulk_layout` and the other layouts of the duals

`bulk_layout`, `weight_layout`, `bulk_dual_layout`, `weight_dual_layout`,
`left_bulk_dual_layout`, `left_weight_dual_layout`, `dot_layout`,
`antidot_layout`, `project_layout`, and `antiproject_layout`: the layouts of
these operations' results.

### `transform`

`<galoot/core/transform.hpp>`. `transform(x, v)`: the versor v applied to x,
on x's layout. See [transform and compose](#transform-and-compose).

### `compose` and `compose_layout`

`compose(q2, q1)`: q2 ⟇ q1, which applies q1 first, on
`compose_layout<Tag, A, B>`.

### `basis_change` and `from_rows`

`<galoot/core/basis_change.hpp>`. `basis_change<N>`: the integer `rows` and
the `denominators` of a change; `from_rows(rows, denominators)` builds one.
See [change of basis](#change-of-basis).

### `conformal_change`

`conformal_change<D>()`: the change from conformal(D) to R(D+1, 1, 0).

### `change_basis` and `change_basis_back`

`change_basis<C>(x)` writes x in the generators of the change tag C's
target, and `change_basis_back<C>(y)` brings y back.

### `same_rows`

`same_rows(a, b)`: whether two changes hold the same rows over the same
denominators, as written.

### `basis_change_like`

The concept of a change tag: `source` and `target` algebra tags and a
`value` that is a `basis_change`.

### `change_basis_layout` and `change_basis_back_layout`

The layouts of the results of `change_basis` and `change_basis_back`.

### `std::formatter` for `mv`

`<galoot/core/format.hpp>`. Prints a multivector by its blades' names. See
[printing](#printing).
