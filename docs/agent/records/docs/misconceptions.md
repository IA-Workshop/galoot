# The misconceptions the documentation refutes

Written by: Claude (agent, Fable 5.2, math-expert and hci-expert roles), 2026-11-21, for unit U1 of plan 0049. Not yet reviewed.

A misconception is a wrong idea a reader brings to a page. The pedagogical skill asks that each be refuted where the reader has the words for the correct idea: the page states the wrong idea, says plainly that it is wrong, gives the correct idea, and says why the wrong one is tempting. A refutation with a number the reader can run is stronger than one in words, so each entry here has a value that tells the two ideas apart.

Plan 0049 section 4.2 lists fourteen. This record keeps them as fifteen entries (the plan's "`!x` is a truth value and `!!x` is x" is two ideas, M6 and M22), sorts them by who holds them, places each, and adds sixteen found while writing the chapters' first programs and deriving research note 0028. Three kinds of reader hold them: the newcomer (N), who generalizes from school vectors; the C++ programmer (C), who reads Galoot's operators by their built-in meaning; and the arriver (A), who knows geometric algebra from another source. A tutorial chapter refutes the first two kinds. The third is refuted on the first-things page and in the conventions chapter, because the newcomer does not hold those ideas and the words for them would break the tutorial's order.

"Checked by" says where the value was computed: **probe** is `checks/design_values_probe.cpp` or `checks/transform_sandwich_probe.cpp` of this unit, **guide** is a compiled example of the 1.0.0 README or guide, and **derived** means derived here and not compiled.

## 1 Held by the newcomer

| ID | The wrong idea | Why it is tempting | The correct idea, and a value that shows it | Refuted in | Row | Checked by |
| --- | --- | --- | --- | --- | --- | --- |
| M1 | The wedge is the cross product | In space both give three numbers, and they are the same three: (1, 2, 3) and (4, 5, 6) give (−3, 6, −3) either way | The wedge is an area with a sense of turning, on e23, e31, e12; it exists in the plane, where it has one number and no vector is perpendicular to it; and reflecting everything through the origin leaves it unchanged, where it negates every vector | chapter 2 | K213 | probe |
| M2 | e12 and e21 are one blade | The same two generators | e21 = −e12: `get<"e12">` of e2 ∧ e1 is −1 | chapter 2 | K205 | probe |
| M3 | The product of two vectors is a vector, or does not depend on their order | School products of numbers commute, and the dot and cross products each give one kind of thing | (1, 2, 0)(3, 1, 0) = 5 − 5 e12 and (3, 1, 0)(1, 2, 0) = 5 + 5 e12: a scalar plus a bivector, and the bivector changes sign with the order | chapter 3 | K304, K306 | probe |
| M4 | A rotor holds the angle of its turn | Its coefficients are a cosine and a sine | It holds half: mirrors 45 degrees apart make 1 − e12, which turns by 90 | chapter 4 | K407 | probe |
| M5 | A turn needs a rotor of unit size, or the answer is wrong | Texts normalize first | The sandwich of any rotor is the turn times r r̃: 1 − e12 sends (1, 0, 0) to (0, 2, 0), the right direction at twice the length | chapter 4 | K406 | probe |
| M6 | Two complements give back what one started with | "Complement" suggests a pair of opposites | They give x times (−1) to the grade times the antigrade: x in three dimensions, but `!!e1` is −e1 in two, and `!!p` is −p for a point of rigid(3) | chapter 5, again in 7 | K505 | probe, guide |
| M7 | e4 is a fourth axis like the others | It is written like them | It squares to zero: the dot of (1, 2, 3, 2) with itself is 14, with no 4 from the weight | chapter 7 | K702 | probe |
| M8 | A generator that squares to zero is zero, or is useless | Only zero squares to zero among numbers | The origin is (0, 0, 0, 1), all of it on e4, and it is not the zero value | chapter 7 | K702 | derived |
| M9 | (2, 4, 6, 2) and (1, 2, 3, 1) are different points; or, being one point, they compare equal | Different numbers; and then, one point | One point, two values: `==` compares coefficients and says they differ. Compare after dividing by the weights, or by cross-multiplying. The same holds for a line, a plane, or a motor and its multiple | chapter 7 | K703 | probe |
| M10 | The line from p to q is the line from q to p | The same set of points | q ∧ p = −(p ∧ q): the same line with the opposite direction, and a different value | chapter 7 | K705 | derived |
| M11 | Parallel planes do not meet | In school geometry they do not | Their meet is a line with zero direction: z = 0 and z = 1 give (0, 0, 0, 0, 0, −1) | chapter 7 | K714 | probe |
| M12 | A meet gives the point; its sign and size mean nothing, or mean an error | One expects (1, 1, 0) and reads (−2, −2, 0, −2) | The same point, with a weight that carries the orientations of the line and the plane | chapter 7 | K715 | guide |
| M13 | `compose(a, b)` applies a first | Reading left to right | It applies b first, as r2 r1 applies r1 first: `transform(x, compose(Q2, Q1))` is `transform(transform(x, Q1), Q2)` | chapter 6, again in 8 | K608 | guide |
| M14 | `transform` returns a unitized result | "Transform" sounds complete | A motor with λ = 2 doubles the weight: (1, 0, 0, 1) goes to (0, 2, 0, 2) | chapter 6, again in 8 | K609, K810 | guide |
| M15 | Galoot's `transform(v, r)` is the sandwich r v r̃ | The name, and a first try with a quarter turn agrees | It is the opposite turn of the negated vector; the sandwich is `transform(v, !r)`. For r = 1 − e12 on (0, 0, 1): the sandwich gives (0, 0, 2) and `transform(v, r)` gives (0, 0, −2). For 2 − e12 on (1, 0, 0): (3, 4, 0) against (−3, 4, 0) | chapter 4 as a warning, chapter 6 in full | K410, K607 | probe |
| M16 | A check that passes over `double` shows the result is exact | It passed | Over `double` every operation rounds, and a comparison with `==` passes or fails by the last bit. Exact means an exact scalar type, checked in a constant expression | chapter 9 | K903 | derived |
| M17 | Integer arithmetic at compile time cannot go wrong | It is exact | It can overflow. The library checks its own coefficients; the reader's values are the reader's | chapter 9 | K906 | derived |

## 2 Held by the C++ programmer

| ID | The wrong idea | Why it is tempting | The correct idea, and a value that shows it | Refuted in | Row | Checked by |
| --- | --- | --- | --- | --- | --- | --- |
| M18 | `dot(a, b)` is a number | It is one in school | It is a multivector with one coefficient, on the scalar: read it with `get<"1">` | chapter 1 | K113 | probe |
| M19 | `2 * v` works whatever the scalar type | It does for built-in types | The scalar must have the multivector's own type: over `double`, write `2.0 * v` | chapter 1, in a sentence; the first-things page | K109 | guide |
| M20 | `a ^ b == c` compares the wedge with c | `^` looks tighter than `==` | C++ reads `a ^ (b == c)`: every wedge and meet beside another operator goes in parentheses | chapter 2 | K209 | guide |
| M21 | A product returns the named type: `p ^ q` is a `Line`, `compose` gives a `Motor` | The mathematics says a line | A result is on the canonical layout of the blades it reaches, and converts to a named type with braces: `Line{p ^ q}` | chapter 2, again in 7 and 8 | K207, K712 | guide |
| M22 | `!x` is a truth value | It is for every built-in type | It is the right complement, a multivector: `if (!x)` does not compile, and `!x == y` needs parentheses | chapter 5 | K509 | guide |
| M23 | A name such as e14 reads the blade whichever way the type stores it | A blade is a blade | Names are oriented: a `Line` stores e41 = −e14, and `get<"e14">` is rejected with a message that says so | chapter 7 | K707 | guide |
| M24 | `truncate` and `part` round | The names | They drop blades, never digits; `truncate` checks that what it drops is zero and `part` does not | chapter 3 | K307 | guide |

## 3 Held by the arriver

These are refuted on the first-things page and in the conventions chapter, each row of whose card carries the value.

| ID | The wrong idea | Why it is tempting | The correct idea, and a value that shows it | Checked by |
| --- | --- | --- | --- | --- |
| M15 | As above: `transform(v, r)` is r v r̃ | Every other source's sandwich is | `transform(v, !r)` is, in a Euclidean algebra; with `bulk_dual` in place of `!` in any nondegenerate one | probe; note 0028 |
| M25 | One vector a reflects by `transform(v, a)` | −a v a is the reflection | In Galoot the mirrors are the antivectors: `transform(v, a)` is a v a, which sends (1, 2, 3) to (1, −2, −3) for a = (1, 0, 0); the reflection is `transform(v, !a)`, (−1, 2, 3) | probe |
| M26 | The dot is the scalar part of the geometric product | Many libraries define it so | Galoot's dot is Lengyel's: e12 • e12 = +1, while e12 e12 = −1 | guide |
| M27 | The dual and the complement are the same | With a pseudoscalar that squares to ±1 they differ by a sign at most | In rigid(3) the complement of the origin e4 is the plane e321, and its bulk dual is zero. A complement can be undone; a dual cannot | probe |
| M28 | The null generator is e0, first; planes are the vectors; motions use the geometric product | The description many readers learned first | e4, last; points are the vectors; motions use the antiproduct. The right complement carries one description to the other (note 0028, Theorem C) | Checked in the note's script |
| M29 | A Euclidean rotor's coefficients copy into a motor as they are | Same letters | a + b1 e23 + b2 e31 + b3 e12 becomes the motor (−b1, −b2, −b3, a, 0, 0, 0, 0): the complement sends 1 to e1234 and e23, e31, e12 to −e41, −e42, −e43. So 1 − e12 becomes (0, 0, 1, 1, 0, 0, 0, 0). Copied with the same signs, the motor turns the other way | probe |
| M30 | A motor's coefficients are a quaternion's, scalar first | Four numbers and half angles | The order is e41, e42, e43, e1234, then e23, e31, e12, 1: the "scalar" of the rotation part is fourth, on e1234 | guide |

Derivation for M29, since the knowledge base has it only as self-checked: in four generators, e23 ∧ e14 = e2 e3 e1 e4, and moving e1 past e3 and e2 costs two swaps, so right(e23) = e14 = −e41. e13 ∧ e24 = e1 e3 e2 e4 = −e1234, so right(e13) = −e24 and right(e31) = e24 = −e42. e12 ∧ e34 = e1234, so right(e12) = e34 = −e43. And right(1) = e1234.

## 4 Held by an author

One more, which this unit's plan fell into, goes into the authoring rules and not onto a page.

| ID | The wrong idea | The correct idea |
| --- | --- | --- |
| M31 | Two ways of turning a vector agree, because they agree for a quarter turn of a vector in the rotor's plane | A quarter turn in its own plane is the one case in which a turn and the opposite turn of the negated vector agree. A test of a rotation uses an angle that is not a multiple of a quarter turn and a vector off the plane (rule A2) |

## 5 The tenth fact of the first-things page

The page lists the facts that most often cost a new user an hour. M15 belongs there before the tutorial exists, since the arriver reads that page first. A draft of its text, for the unit that moves the page (U3) or writes the conventions chapter (U4); the example is `checks/transform_sandwich_probe.cpp` cut to one rotor.

> **`transform` is not the sandwich r v r̃.** `transform(x, V)` acts through the antiproduct, and it takes its versor in the form in which a mirror is a plane, not a normal. A rotor built with the geometric product, as r = b a from two mirror normals, is not in that form, and `transform(v, r)` compiles and gives a different vector: the opposite turn of −v. Write the sandwich itself, `r * v * ~r`, or give `transform` the complemented rotor, `transform(v, !r)`, which is the same turn in every R(n, 0, 0). A test with a quarter turn of a vector in the rotor's plane does not show the difference; a vector off the plane does.

## 6 What was not found

No source was read in this unit for how often readers hold each idea; the list comes from the plan, from the knowledge base's notes on sign conventions and on teaching geometric algebra, and from writing each chapter's first program before reading the headers. The cold reads of plan section 9.1 are the test: a wrong belief a reader states that is not here is added, with its value, in the unit that finds it.
