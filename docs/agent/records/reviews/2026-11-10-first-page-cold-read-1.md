# First-page cold read 1 (2026-11-10)

Reader: a fresh agent (Sonnet 5.5) that read only README.md and
docs/guide/README.md (no link beyond them was needed), plus a few lines of
compiler output. Compilers: g++ and clang++ with `-std=c++23 -Wall
-I<worktree>/include`. Programs are in the scratchpad `.../scratchpad/fp/`
(t1a..t5a first attempts, t1..t5 final). No repo tests were used.

Result: all five tasks pass; every fix came from the page. Both compilers
give the same output.

## Task 1: join two points, compare with a given line

First attempt: `p ^ q == l` with `Point p{0,0,0,1}, q{0,0,2,1}; Line l{...}`.

Error (g++, abridged, the type dumps are very long):
`error: no match for 'operator==' (operand types are 'Point' {aka
'galoot::core::mv<...blade_set<4>{...}, double>'} and 'Line' {aka
'galoot::core::mv<...blade_set<6>{...}, double>'})`

Fix: guide section 4, "`p ^ q == l` means `p ^ (q == l)`" and "a join is
compared with a `Line` after it is converted: `Line{p ^ q} == l`".
The error text itself does not hint at precedence; only the page does.

Final: `std::println("{}", Line{p ^ q} == l);` with l = {0,0,2,0,0,0}
prints `true`.

## Task 2: quarter turn about z, then a move

First attempt: `Motor m = core::compose(mv, turn);` and
`translation(1,2,3)`.

Errors:
- `error: conversion from 'mv<[...],[...],int>' to non-scalar type
  'mv<[...],[...],double>' requested` (the `translation(1,2,3)` line)
- `error: conversion from 'mv<...blade_set<8>...>' to non-scalar type
  'mv<...blade_set<8>...>' requested` (the compose line)
- `static assertion failed: translation: halving d would truncate in an
  integer scalar type; use a floating or exact rational scalar type`

Fixes: README example comment ("converts to a Motor with braces, so
`const Motor motion = core::compose(move, turn);` would not compile") and
guide section 1 for the braces; guide section 2 ("`translation(1.0, 0, 2)`
... rejected, `translation(1.0, 0.0, 2.0)` accepted") for the doubles.

Final: `Motor m{core::compose(mv, turn)}` with
`rotation(z_axis, pi/2)` and `translation(1.0, 2.0, 3.0)`; transform of
(1,0,0,1) prints `1.000 3.000 3.000 w1.000`.

## Task 3: double a line over double

First attempt: `Line d = 2 * l;`.

Error: `static assertion failed: s * a: the scalar is not of the
multivector's scalar type, the second of the two types shown; convert it to
that type first, as 2.0 for 2 over double or std::int64_t{2} over
std::int64_t`

Fix: guide section 2 (`2 * line` is rejected, write `2.0`). The error text
alone was also enough. Final `2.0 * l` prints
`{e41: 0, e42: 0, e43: 2, e23: 0, e31: 0, e12: 0}`.

## Task 4: read a line's direction by name

First attempt: `core::get<"e14">(l)` (guessing the natural x-direction
name from the math).

Error: `static assertion failed: get: the type stores this blade with the
other orientation; use the name its layout gives it and negate the value`

Fix: guide section 5 (a `Line` stores the direction on e41, e42, e43).
Final: `get<"e41">, get<"e42">, get<"e43">` of {1,2,3,0,0,0} print `1 2 3`.

## Task 5: is the right complement of a point equal to a plane

First attempt: `!p == g` with `Plane g{1,2,3,1}`.

Error: `error: no match for 'operator==' (operand types are
'mv<...blade_set<4>{... {7,1},{11,1},{13,1},{14,1}}...>' and 'Plane'
{aka 'mv<...{14,1},{13,-1},{11,1},{7,-1}...>'})` (two different layouts of
four blades; the complement is on canonical blades, the Plane on oriented).

Fix: guide section 3 ("the right complement of the point (x, y, z, w) is
the plane with the same four coefficients", with the example
`Plane{!p} == Plane{1, 2, 3, 1}`) together with section 1 (convert with
braces). Final `Plane{!p} == g` is `true`, against `Plane{1,2,3,2}` is
`false`.

## What I learned from the page

- Products (`^`, `&`, `%`, `compose`, `!`) return results on unnamed
  canonical layouts; named types are got with braces, and `==` and
  assignment need the same layout.
- The scalar type must match exactly, in operators and in `translation` and
  `rotation`; braces accept integer literals.
- `get<"...">` is orientation-checked; a Line is read on e41..e43.
- `^` and `&` bind weakly, so parenthesize.
- Printing needs `<galoot/core/format.hpp>`.

## Missing or misleading

1. The guide never says, in the sentence about `==`, that the complement `!x`
   is also on a canonical layout; section 3 shows `Plane{!p}` only inside a code
   example, without saying why. A reader comparing `!p == plane` gets the
   long layout error. Add to section 3: "`!x` also has its own layout, so
   convert it, `Plane{!p}`, before comparing."
2. Section 3 says to write `(!x) == y` ("Parenthesize a complement in a
   comparison"), which, read alone, suggests that is sufficient. It is not
   sufficient: `(!p) == plane` still fails on layouts. Change that sentence
   to `Plane{!x} == y` or add the conversion remark.
3. Section 2 says `translation(1.0, 0, 2)` is "rejected, with a message that
   says to write `2.0`". The compiler gives a conversion error for `int`
   arguments mixed with a double (`translation(1,2,3)` here gave "halving d
   would truncate"), not a message naming `2.0`. Say "is rejected" without
   the message claim, or reword.
4. The error messages for layout mismatch print the whole blade_set as a
   type; the page does not warn that this is what "no conversion exists"
   looks like. A sentence in section 1: "the compiler prints both layouts in
   full; look for two `mv<...>` types differing in the blade list."
5. The README's first example uses `rotation(...)` qualified and the guide
   section 2 uses it unqualified; both work (ADL) but the README says to
   qualify. Harmless, but pick one in the guide's first example.
6. README says the first library page is the guide, but the "nine facts"
   page never lists the preset's object names (Point, Line, Plane, Motor,
   Flector) with their coefficient orders; a newcomer must guess the order
   of `Line{...}` (direction, then moment) from a comment in the README.
   One line in the guide's intro would help.

## Sentences I would change

- Guide section 3, "Parenthesize a complement in a comparison, `(!x) == y`;
  GCC warns ..." -> "Convert a complement and parenthesize it,
  `Plane{!x} == y`; unparenthesized, GCC warns ...".
- Guide section 2, "rejected, with a message that says to write `2.0`" ->
  state it only for `2 * line`, where it is true.
- Guide section 1, add that the complement `!x` and the meet `&` are
  results on their own layouts too.
