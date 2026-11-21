# The concept inventory of the user documentation, and its order

Written by: Claude (agent, Fable 5.2, math-expert and hci-expert roles), 2026-11-21, for unit U1 of plan 0049. Checked by an independent math review on 2026-11-21 and corrected the same day by Claude (agent, Opus 5.5); `checks/README.md` lists each correction. Research note [0029](../../research-notes/0029-docs-design-transform-and-the-classical-sandwich.md) gives the reasons for the order; this record is the order.

Every term, symbol, notation, named result, and technique that the documentation introduces is a row here, with what it rests on. The tutorial is written in the order of part 2: a row may use only the prerequisites of part 1 and rows above it. The glossary (unit U3) is built from this record, one row per term, and `tests.docs_terms` then holds the pages to the same order.

How to read a row: **ID**, the **concept** as the reader will meet it, its **kind** (term, symbol, C++ name, rule, technique, result, or warning), what it **rests on** (IDs), and the **public names** of the library it brings in. A concept taught first in a simple form and refined later has two rows, and the second rests on the first.

`checks/inventory_check.py` reads this file and fails if an ID repeats, a row rests on an ID that does not exist, or a row of part 2 rests on a row below it. That shows the order has no cycle and no forward reference by ID. It also fails if a tutorial row's text uses a term above the row that defines it, for the terms it can read: the headword before the first colon of a row of kind term or symbol. So a row that defines a term names it first, followed by a colon, and a term met first in a simple form names the first form there ("Blade, first form:"). A term named later in a cell is not seen. The script cannot show that no concept is missing; that is the independent reviewer's question.

## 1 Prerequisites

Assumed of the tutorial's reader and never explained. `learner-contracts.md` gives the self-check questions.

| ID | Assumed | Kind | Rests on | Public names |
| --- | --- | --- | --- | --- |
| P01 | A vector as an arrow and as a list of coordinates; adding two and scaling one | term | | |
| P02 | The length of a vector, by Pythagoras; a unit vector | term | P01 | |
| P03 | Angle; perpendicular and parallel; clockwise and counterclockwise | term | | |
| P04 | Sine and cosine of an angle; the radian | term | P03 | |
| P05 | The dot product of two vectors, by coordinates and as the lengths times the cosine | term | P01, P02, P04 | |
| P06 | The cross product of two vectors in space, as a definition seen once | term | P01, P03 | |
| P07 | The area of a parallelogram and the volume of a box | term | | |
| P08 | A line and a plane in space; the origin; the coordinate axes | term | P01 | |
| P09 | A matrix as a table of numbers | term | | |
| Q01 | Compiling and running a C++23 program from the command line, with an include path | technique | | |
| Q02 | `#include`, a namespace, a namespace alias, a qualified name | C++ | | |
| Q03 | A `struct` with a `static constexpr` member | C++ | | |
| Q04 | Using a template by writing its arguments in angle brackets; a `using` alias; `auto` | C++ | | |
| Q05 | `constexpr` and `static_assert` | C++ | | |
| Q06 | Brace initialization, and that it rejects a narrowing conversion | C++ | | |
| Q07 | That operators have precedence and that a library can define them for its types | C++ | | |
| Q08 | `int` and `double`; that integer division truncates and that `double` arithmetic rounds | C++ | | |
| Q09 | `std::println` with `{}` and with a specification such as `{:.3f}` | C++ | | |
| Q10 | Finding the first error in a compiler's output | technique | | |

## 2 The tutorial, in order

### Chapter 1: A first program: vectors and dot products

R(2, 0, 0), then R(3, 0, 0), over `int`. No new mathematics: Galoot's C++ on what the reader knows.

| ID | Concept | Kind | Rests on | Public names |
| --- | --- | --- | --- | --- |
| K101 | Generator: one of the unit directions e1, e2, … from which everything in the tutorial is built | term | P01, P02, P03 | |
| K102 | Algebra, first form: a choice of generators, declared as a tag, a `struct` whose `value` holds the choice | term | K101, Q03 | |
| K103 | `R<P, Q, Z>()`, first form: P perpendicular unit generators; the other two counts are zero, and chapter 7 says what they count | C++ name | K102 | `R` |
| K104 | Layout: the list of things a type stores, written `blades<Tag, "e1 e2">` | term | K101, K102, Q04 | `blades` |
| K105 | Multivector, first form: a value that holds one number for each entry of its layout, `mv<Tag, Layout, T>` | term | K104 | `mv` |
| K106 | Coefficient; scalar, a plain number; scalar type | term | K105, Q08 | |
| K107 | Construction with braces: one value for each entry, in the layout's order | rule | K105, Q06 | |
| K108 | Two values of one type compare with `==`; `static_assert` as the check of every example | technique | K105, Q05 | |
| K109 | Sum, difference, negation, and scaling by a scalar of exactly the multivector's scalar type | rule | K105, K106 | |
| K110 | Reading a coefficient by name, `get<"e1">(v)`, and by position, `v.coefficients[i]` | C++ name | K105 | `get` |
| K111 | Printing: `<galoot/core/format.hpp>`, and the printed form `{e1: 3, e2: 4}` | technique | K105, Q09 | |
| K112 | `dot(a, b)` for two vectors | C++ name | K105, P05 | `dot` |
| K113 | A result has its own layout, first form: the dot of two vectors holds one coefficient, on the scalar `1`, read with `get<"1">` | rule | K112, K104, K110 | |
| K114 | Metric: the table of the dot products of the generators; in R(3, 0, 0) it says that they are unit and perpendicular | term | K112, K101, P09 | |
| K115 | Squared length as `dot(v, v)`; comparing squares to stay in integers | technique | K112, K113, P02 | |
| K116 | Exact, first form: integer arithmetic has no rounding, and the compiler evaluates a constant expression and checks it | term | K108, K106 | |

Likely mistake shown: a wrong number of values in the braces, and its message.

### Chapter 2: The wedge

R(2, 0, 0) for the first picture, then R(3, 0, 0).

| ID | Concept | Kind | Rests on | Public names |
| --- | --- | --- | --- | --- |
| K201 | Bivector: an oriented area, a piece of a plane with a sense of turning | term | P07, P01, P03 | |
| K202 | The wedge a ∧ b of two vectors, the bivector they span; `wedge(a, b)` and `a ^ b` | symbol | K201, K105 | `wedge` |
| K203 | Antisymmetry: b ∧ a = −(a ∧ b), and a ∧ a = 0 | result | K202 | |
| K204 | Blade, first form: a wedge of distinct generators, also called a basis blade, such as e1 and the basis bivectors e12, e13, e23; a blade's name lists its generators | term | K202, K101 | |
| K205 | Oriented and canonical names: e21 = −e12; the canonical name lists indices in increasing order | rule | K204, K203 | |
| K206 | The coefficients of a ∧ b, each a 2 × 2 difference of products; in the plane, the signed area | result | K204, K203, P07 | |
| K207 | Result layouts: a wedge is on the canonical layout of the blades it can reach, and `auto` keeps it there | rule | K113, K205 | |
| K208 | Conversion with braces to a type of the reader's own, `Bivector{a ^ b}`; conversions are explicit | rule | K207, K107 | |
| K209 | `^` binds more loosely than `==`, `+`, and `*`: parentheses around every wedge beside another operator | rule | K202, Q07 | |
| K210 | Blade, refined: a wedge of any vectors, not only of generators; grade: how many vectors; the scalar has grade 0, a vector 1, a bivector 2 | term | K202, K204 | |
| K211 | Trivector: an oriented volume, e123; a ∧ b ∧ c | term | K210, P07 | |
| K212 | The wedge of three vectors is the determinant of their coordinates, on e123 | result | K211, K206 | |
| K213 | The wedge and the cross product: the same three numbers on e23, e31, e12, a different object | result | K206, K205, P06 | |

Likely mistake shown: `a ^ b == c`, which the compiler reads as `a ^ (b == c)`.

### Chapter 3: The geometric product

R(2, 0, 0), then R(3, 0, 0).

| ID | Concept | Kind | Rests on | Public names |
| --- | --- | --- | --- | --- |
| K301 | The geometric product of generators: e1 e1 = 1, e1 e2 = e12, e2 e1 = −e12 | rule | K114, K204, K205 | |
| K302 | The geometric product ⟑; `geometric_product(a, b)` and `a * b`, which is not the scaling `s * a` | symbol | K301, K109 | `geometric_product` |
| K303 | Multiplying out by the rules: distribute, reorder with a sign, square the generators | technique | K301, K203 | |
| K304 | For two vectors, u v = u · v + u ∧ v | result | K303, K112, K202 | |
| K305 | Multivector, refined: a sum of blades of several grades, on a layout such as `1 e12 e13 e23` | term | K304, K210, K105 | |
| K306 | The product is associative and does not commute | result | K303 | |
| K307 | Taking a part: `part<Type>(x)` drops the rest without a check; `truncate<Type>(x)`, over an exact scalar such as `int`, checks that what it drops is zero in a constant expression; chapter 9 adds the same check at run time | C++ name | K305, K208 | `part`, `truncate` |
| K308 | A vector squares to its squared length; the bivector e12 squares to −1 | result | K304, K301, K115 | |
| K309 | The inverse of a vector, v divided by v · v, written by hand; in integers, keep the factor instead of dividing | technique | K308, K109 | |

Likely mistake shown: `Vector{a * b}`, a conversion that would drop the scalar and the bivector.

### Chapter 4: Reflections and rotors

R(3, 0, 0), with the first picture in the plane.

| ID | Concept | Kind | Rests on | Public names |
| --- | --- | --- | --- | --- |
| K401 | Reflection of v in the mirror with normal a: −a v a⁻¹; in integers −a v a, which is the reflection times a · a | result | K309, K306, P03 | |
| K402 | Two reflections turn by twice the angle between the mirrors, from the first toward the second | result | K401, P04 | |
| K403 | Rotor: the product b a of two mirror normals | term | K402, K302 | |
| K404 | Reverse: the factors in the opposite order; `reverse(r)` and `~r`; a bivector changes sign | term | K403, K205 | `reverse` |
| K405 | The sandwich r v r̃, written `truncate<Vector>(r * v * ~r)`, in a helper of the reader's own | technique | K403, K404, K307 | |
| K406 | The factor a rotor leaves, r r̃; a unit rotor; an exact rotor in integers, 2 − e12 with factor 5 | result | K405, K308 | |
| K407 | Half angles: the rotor of a turn by θ holds cos(θ/2) and sin(θ/2) | result | K402, K406, P04 | |
| K408 | Composing turns by multiplying rotors: r2 r1 applies r1 first | rule | K405, K306 | |
| K409 | A rotor's layout: the scalar and the bivectors | rule | K403, K305 | |
| K410 | Galoot's `transform` is not this sandwich: a signpost, with the vector (0, 0, 1) on which they differ | warning | K405 | |

Likely mistake shown: `transform(v, r)`, which compiles and gives another vector; the failed `static_assert` is what the reader sees.

### Chapter 5: Complements and the meet

R(3, 0, 0), and R(2, 0, 0) for the one contrast that three generators cannot show.

| ID | Concept | Kind | Rests on | Public names |
| --- | --- | --- | --- | --- |
| K501 | Antiscalar 𝟙: the blade of all the generators, e123 in three dimensions | symbol | K211 | |
| K502 | Antigrade: the number of generators a blade lacks, n minus its grade | term | K210, K501 | |
| K503 | Right complement: on each basis blade u, the blade right(u) with u ∧ right(u) = 𝟙; `right_complement(x)` and `!x` | term | K202, K501 | `right_complement` |
| K504 | Left complement, with left(u) ∧ u = 𝟙; `left_complement(x)`; each undoes the other | term | K503 | `left_complement` |
| K505 | Two right complements multiply a blade by (−1) to the grade times the antigrade: x in three dimensions, −x for a vector of two or four | result | K503, K502 | |
| K506 | A plane through the origin is a bivector, and the complement of its normal | result | K503, K201, P08 | |
| K507 | The antiwedge a ∨ b = left(right(a) ∧ right(b)), the meet; `antiwedge(a, b)` and `a & b` | symbol | K503, K504, K202 | `antiwedge` |
| K508 | Two planes through the origin meet in the line along the cross product of their normals | result | K507, K506, P06 | |
| K509 | `!x` is a multivector, not a truth value: `(!x) == y` in parentheses, and no `if (!x)` | rule | K503, Q07 | |

Likely mistake shown: `!x == y` and `if (!x)`.

### Chapter 6: Mirrors as planes: the antiproduct

R(3, 0, 0). The same reflections and turns as chapter 4, with each mirror given by its plane.

| ID | Concept | Kind | Rests on | Public names |
| --- | --- | --- | --- | --- |
| K601 | A mirror given by its plane g = right(a) in place of its normal a | technique | K506, K401 | |
| K602 | The geometric antiproduct a ⟇ b = left(right(a) ⟑ right(b)); `geometric_antiproduct(a, b)` and `a % b`; its identity is 𝟙 | symbol | K503, K504, K302, K501 | `geometric_antiproduct` |
| K603 | The complement carries one product to the other: right(b a) = right(b) ⟇ right(a) | result | K602 | |
| K604 | Antireverse: the reverse with the antigrade in place of the grade; `antireverse(x)` | term | K404, K502 | `antireverse` |
| K605 | Versor: an antiproduct of mirror planes; even or odd, by how many | term | K602, K601 | |
| K606 | `transform(x, V)`: for an even V, V ⟇ x ⟇ antireverse(V); for an odd V, such as a mirror, the same with a sign set by the antigrade of each part of x, −1 where it is even and +1 where it is odd, so −V ⟇ v ⟇ antireverse(V) for a vector v of R(3, 0, 0) | C++ name | K605, K604, K502, K405 | `transform` |
| K607 | The bridge: `transform(v, !r)` is `r * v * ~r`, and `transform(v, !a)` is −a v a | result | K606, K603, K405, K401 | |
| K608 | `compose(V2, V1)` is V2 ⟇ V1 and applies V1 first, as r2 r1 does | C++ name | K602, K408 | `compose` |
| K609 | `transform` gives x's own type and does not divide: the factor V ⟇ antireverse(V) = λ 𝟙 stays, the factor r r̃ of chapter 4 | rule | K606, K406 | |

Likely mistake shown: `transform(v, r)` again, now explained: the versor was not complemented.

### Chapter 7: Points, lines, and planes

rigid(3), through the rga3 preset, over `int`.

| ID | Concept | Kind | Rests on | Public names |
| --- | --- | --- | --- | --- |
| K701 | Weight, first form: a fourth coordinate w, with (x, y, z, w) standing for the position (x/w, y/w, z/w) | term | P01, K106 | |
| K702 | A generator that squares to zero, e4; rigid(3) is R(3, 0, 1); `R<P, Q, Z>` counts P generators that square to 1, Q that square to −1, and Z that square to 0, and the tutorial's Q is always 0 | term | K114, K103, K701 | `rigid` |
| K703 | Point: x e1 + y e2 + z e3 + w e4; unitized when w = 1; (2, 4, 6, 2) and (1, 2, 3, 1) are one point and two values | term | K701, K702 | |
| K704 | The rga3 preset: `<galoot/presets/rga3.hpp>`, its tag `rga3::algebra`, and `rga3::Point<T>` | C++ name | K703, K104 | `algebra`, `Point` |
| K705 | Join: p ∧ q is the line through p and q, from p to q | result | K703, K202 | |
| K706 | Line: its direction on e41, e42, e43 and its moment on e23, e31, e12; `rga3::Line<T>` | term | K705, K205, K206 | `Line` |
| K707 | The oriented names of rigid(3): e41 = −e14, e31, e423, e431, e412, e321 | rule | K205, K706 | |
| K708 | Plane: p ∧ q ∧ r; its normal on e423, e431, e412 and its offset on e321; `rga3::Plane<T>` | term | K705, K211, K707 | `Plane` |
| K709 | In four generators the antiscalar is e1234, and the complement of a point is a plane | result | K503, K501, K708 | |
| K710 | Meet in rigid(3): a line and a plane meet in a point, two planes in a line | result | K507, K706, K708 | |
| K711 | Incidence: a point lies on a plane exactly when their meet, a scalar, is zero | result | K710, K113 | |
| K712 | Converting to the named types, `Line{p ^ q}` and `Point{l & g}`; a join kept with `auto` reads −1 on e24 where the `Line` reads 1 on e42 | rule | K208, K707 | |
| K713 | Bulk and weight, refined: the part without e4 and the part with it, so that a point's weight is now w e4 and not the number w; `bulk(x)` and `weight(x)` | term | K702, K703 | `bulk`, `weight` |
| K714 | Parallel planes meet too, in a line with zero direction | result | K710, K713 | |
| K715 | The sign and size of a meet carry the orientations of what met; the point is the same | result | K710, K703 | |

Likely mistake shown: `get<"e14">` on a `Line`, and its message.

### Chapter 8: Motions

rigid(3), over `int` and then `double`.

| ID | Concept | Kind | Rests on | Public names |
| --- | --- | --- | --- | --- |
| K801 | A plane as a mirror: `rga3::Flector<T>`, and `transform(p, F)`; a reflected point keeps a positive weight | term | K606, K708 | `Flector` |
| K802 | The mirrors of chapter 6, as planes through the origin, give the same reflections of points | result | K801, K607, K709 | |
| K803 | Two parallel mirrors move by twice their distance, from the first toward the second | result | K801, K608, K708 | |
| K804 | Two crossing mirrors turn by twice their angle about their common line, their meet | result | K801, K608, K710, K402 | |
| K805 | Motor: the antiproduct of two mirrors; `rga3::Motor<T>` and its eight coefficients | term | K803, K804, K605 | `Motor` |
| K806 | The rotor of chapter 4, complemented in rigid(3), is the motor of the same turn about a line through the origin | result | K805, K607 | |
| K807 | `transform(x, Q)` moves lines and planes as the points that span them | result | K606, K805, K705 | |
| K808 | `compose(Q2, Q1)` for motors applies Q1 first; its result converts with braces | rule | K608, K805, K712 | |
| K809 | `rga3::rotation(l, theta)` and `rga3::translation(dx, dy, dz)`: full angles and distances in, halves stored | C++ name | K805, K407, K706, P04 | `rotation`, `translation` |
| K810 | The weight factor λ, with Q ⟇ antireverse(Q) = λ 𝟙; a unitized motor has λ = 1 | term | K609, K805, K703 | |
| K811 | A turn about a line that misses the origin | technique | K809, K808, K804 | |
| K812 | The other convention, in two sentences: e0 for the generator that squares to zero, put first; planes as the vectors; the geometric product; the complement carries one to the other | warning | K702, K805, K503 | |

Likely mistake shown: `const Motor m = compose(b, a);`, an implicit conversion, and the order of `compose`.

### Chapter 9: Exact and rounded

rigid(3).

| ID | Concept | Kind | Rests on | Public names |
| --- | --- | --- | --- | --- |
| K901 | Exact structure: the signs and coefficients of every product are exact integers, and only the scalar type rounds | term | K116, K302 | |
| K902 | Motions in integers at compile time, with the factor λ carried | technique | K810, K116 | |
| K903 | The same motions in `double` at run time: results round | result | K809, Q08 | |
| K904 | Comparing two rounded values within a tolerance, in a loop over `coefficients` written by the reader | technique | K903, K110 | |
| K905 | Checking mode: `GALOOT_CHECKING`, and what `truncate` and `transform` verify in it | term | K307, K606, K903 | |
| K906 | An integer that overflows is the reader's to prevent; exact does not mean unbounded | warning | K902 | |
| K907 | What a product costs to compile: each pair of layouts is worked out once, at compile time | result | K207, K302 | |

Likely mistake shown: `==` between two `double` results that differ in the last bit.

### Chapter 10: A robot arm

rigid(3). Nothing new: three joints composed with K808 and K809, checked exactly with K902, run with K903.

## 3 Terms defined outside the tutorial

These rows belong to the conventions chapter, the concepts pages, and the first-things page. Their readers are not bound to the tutorial's order, but each page defines its terms before using them and may rest on any row of part 2.

| ID | Concept | Kind | Rests on | Defined on |
| --- | --- | --- | --- | --- |
| X01 | A blade as a mask of generators; the canonical order, by grade and then by index list | term | K210, K205 | concepts: blades and grades |
| X02 | `full`, `of_grade`, `merge`, and a layout written as a `blade_set` | C++ name | X01, K104 | concepts: blades and grades |
| X03 | Signature R(p, q, r); null generator; degenerate metric | term | K702, K114 | concepts: metric and signature |
| X04 | Gram matrix; a metric that is not diagonal, `from_gram`; a blade there is a wedge, not a product | term | X03, K202 | concepts: metric and signature |
| X05 | conformal(d) and its two null generators | term | X04 | concepts: metric and signature |
| X06 | The dot of two blades of any one grade, a • b; why e12 • e12 is +1 while e12 e12 is −1 | symbol | K112, K308 | concepts: the products |
| X07 | The antidot a ∘ b | symbol | X06, K602 | concepts: the products |
| X08 | The De Morgan laws of the four products | result | K603, K507 | concepts: the products |
| X09 | The metric exomorphism G; the bulk and the weight in any metric | symbol | K713, X03 | concepts: complements and duals |
| X10 | Bulk dual and weight dual, and their left forms; a dual is not a complement | term | X09, K503 | concepts: complements and duals |
| X11 | Projection and antiprojection | term | K507, X10 | concepts: the products |
| X12 | Grade involution; antigrade involution | term | K210, K502 | concepts: versors and transforms |
| X13 | The parity of a versor, by antigrade; the odd form of `transform` | rule | K606, X12 | concepts: versors and transforms |
| X14 | `transform` as λ times an outermorphism | result | K807, K810 | concepts: versors and transforms |
| X15 | `transform` and the classical sandwich, in every Euclidean algebra and with the bulk dual in the others | result | K607, X10, X13 | concepts: versors and transforms; conventions |
| X16 | Ring scalar and exact scalar | term | K106, K901 | concepts: exactness and rounding |
| X17 | Unit roundoff; forward error bound; the rounding check of `transform` | term | K905, X16 | concepts: exactness and rounding |
| X18 | Plan, term, and kernel: how a product becomes straight-line code | term | K907 | concepts: how generation works |
| X19 | Constant-evaluation limits and how a program meets them | term | X18 | concepts: compile-time cost |
| X20 | Change of basis between two algebras | term | X04 | concepts: metric and signature |

## 4 What the order was checked against

- No cycle, no forward reference by ID, and no headword used above its defining row: `checks/inventory_check.py`, run on this file; `checks/inventory_mutants.py` shows that it fails on three broken copies.
- Each concept of plan 0049 section 4.2's table has a row. Three of its entries moved: "why `!!x` is not always x" needs two or four generators, so K505 uses R(2, 0, 0) for it; the antiproduct, the antireverse, `transform`, and `compose` moved from the plan's chapter 8 to the new chapter 6; and the plan's chapters 1 and 2 are one chapter.
- The public names the tutorial brings in are counted by the same script. The reference documents the rest.
- Not checked: that a reader at the declared level can take each step. That is what the cold reads of plan section 9.1 test.
