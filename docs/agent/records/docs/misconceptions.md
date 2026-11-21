# The misconceptions the documentation refutes

Written by: Claude (agent, Fable 5.2, math-expert and hci-expert roles), 2026-11-21, for unit U1 of plan 0049. Checked by an independent math review on 2026-11-21 and corrected the same day by Claude (agent, Opus 5.5); `checks/README.md` lists each correction. Section 5 brought in step with the owner's rulings on `transform` and the first-things page (decision 0258 section 4 items 1 and 11) on 2026-11-21 by Claude (agent, Opus 5.5, math-expert and hci-expert roles); section 5 and row M15 corrected after the independent check of those changes the same day, as `checks/README.md` lists.

A misconception is a wrong idea a reader brings to a page. The pedagogical skill asks that each be refuted where the reader has the words for the correct idea: the page states the wrong idea, says plainly that it is wrong, gives the correct idea, and says why the wrong one is tempting. A refutation with a number the reader can run is stronger than one in words, so each entry here has a value that tells the two ideas apart.

Plan 0049 section 4.2 lists fourteen. This record keeps them as fifteen entries (the plan's "`!x` is a truth value and `!!x` is x" is two ideas, M6 and M22), sorts them by who holds them, places each, and adds sixteen found while writing the chapters' first programs and deriving research note 0029. Three kinds of reader hold them: the newcomer (N), who generalizes from school vectors; the C++ programmer (C), who reads Galoot's operators by their built-in meaning; and the arriver (A), who knows geometric algebra from another source. A tutorial chapter refutes the first two kinds. The third is refuted on the first-things page and in the conventions chapter, because the newcomer does not hold those ideas and the words for them would break the tutorial's order.

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
| M15 | Galoot's `transform(v, r)` is the sandwich r v r̃ | The name, and a first try with a quarter turn agrees | It is the opposite turn of the negated vector; the sandwich is `transform(v, !r)`. For r = 1 − e12 on (0, 0, 1): the sandwich gives (0, 0, 2) and `transform(v, r)` gives (0, 0, −2). For 2 − e12 on (1, 0, 0): (3, 4, 0) against (−3, 4, 0) | chapter 4 as a warning, chapter 6 in full; for the arriver, the first-things page and the conventions card (section 5) | K410, K607 | probe |
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
| M24 | `truncate` and `part` round | The names | They drop blades, never digits; over an exact scalar, `truncate` checks that what it drops is zero in a constant expression, and at run time only in checking mode, and `part` never checks | chapter 3 | K307 | guide |

## 3 Held by the arriver

These are refuted on the first-things page and in the conventions chapter, each row of whose card carries the value.

| ID | The wrong idea | Why it is tempting | The correct idea, and a value that shows it | Checked by |
| --- | --- | --- | --- | --- |
| M15 | As above: `transform(v, r)` is r v r̃ | Every other source's sandwich is | `transform(v, !r)` is, in a Euclidean algebra; with `bulk_dual` in place of `!` in any R(p, q, 0), where every generator squares to 1 or −1, and not in general in a form with other squares | probe; note 0029 |
| M25 | One vector a reflects by `transform(v, a)` | −a v a is the reflection | In Galoot the mirrors are the antivectors: `transform(v, a)` is a v a, which sends (1, 2, 3) to (1, −2, −3) for a = (1, 0, 0); the reflection is `transform(v, !a)`, (−1, 2, 3) | probe |
| M26 | The dot is the scalar part of the geometric product | Many libraries define it so | Galoot's dot is Lengyel's: e12 • e12 = +1, while e12 e12 = −1 | guide |
| M27 | The dual and the complement are the same | With a pseudoscalar that squares to ±1 they differ by a sign at most | In rigid(3) the complement of the origin e4 is the plane e321, and its bulk dual is zero. A complement can be undone; a dual cannot | probe |
| M28 | The null generator is e0, first; planes are the vectors; motions use the geometric product | The description many readers learned first | e4, last; points are the vectors; motions use the antiproduct. The right complement carries one description to the other (note 0029, Theorem C) | Checked in the note's script |
| M29 | A Euclidean rotor's coefficients copy into a motor as they are | Same letters | a + b1 e23 + b2 e31 + b3 e12 becomes the motor (−b1, −b2, −b3, a, 0, 0, 0, 0): the complement sends 1 to e1234 and e23, e31, e12 to −e41, −e42, −e43. So 1 − e12 becomes (0, 0, 1, 1, 0, 0, 0, 0). Copied with the same signs, the motor turns the other way | probe |
| M30 | A motor's coefficients are a quaternion's, scalar first | Four numbers and half angles | The order is e41, e42, e43, e1234, then e23, e31, e12, 1: the "scalar" of the rotation part is fourth, on e1234 | guide |

Derivation for M29, since the knowledge base has it only as self-checked: in four generators, e23 ∧ e14 = e2 e3 e1 e4, and moving e1 past e3 and e2 costs two swaps, so right(e23) = e14 = −e41. e13 ∧ e24 = e1 e3 e2 e4 = −e1234, so right(e13) = −e24 and right(e31) = e24 = −e42. e12 ∧ e34 = e1234, so right(e12) = e34 = −e43. And right(1) = e1234.

## 4 Held by an author

One more, which this unit's plan fell into, goes into the authoring rules and not onto a page.

| ID | The wrong idea | The correct idea |
| --- | --- | --- |
| M31 | Two ways of turning a vector agree, because they agree for a quarter turn of a vector in the rotor's plane | A quarter turn in its own plane is the one case in which a turn and the opposite turn of the negated vector agree. A test of a rotation uses an angle that is not a multiple of a quarter turn and a vector off the plane (rule A2) |

## 5 The `transform` trap on the pages: the four pieces the owner ruled

M15, with its one-mirror form M25, is the misconception here that gives a reader who knows the subject a wrong number with no message. The owner ruled that it is taught in the documentation now, in four pieces: the warning in chapter 4, the bridge in chapter 6, the first-things fact, and the conventions card's row (decision 0258 section 4 item 1), and that the first-things page gains it as a tenth fact (item 11). Nothing in the library changes for it. Two further parts of the ruling are not pages: a sentence in `transform`'s `///` comment when that comment is next changed, and a named classical operation as the first candidate of the backlog for a later minor release. No page says that a later release will have that operation (rule C2 of `authoring-rules.md`).

### 5.1 The values that tell the two actions apart

All in R(3, 0, 0), over `int`. The rotors are r = b a for the mirrors a = (1, 0, 0) and b = (1, 1, 0), and s = c a for c = (2, 1, 0). Each row's values are derived in research note 0029 section 2.8, except T3, derived below.

| ID | Versor V and vector v | The classical sandwich: `V * v * ~V` for a rotor, −a v a for a mirror | `transform(v, V)` | `transform(v, !V)` | What it shows |
| --- | --- | --- | --- | --- | --- |
| T1 | r = 1 − e12, v = (0, 0, 1) | (0, 0, 2) | (0, 0, −2) | (0, 0, 2) | the difference, with numbers a newcomer can check by hand |
| T2 | r = 1 − e12, v = (1, 0, 0) | (0, 2, 0) | (0, 2, 0) | (0, 2, 0) | the coincidence of a quarter turn in its own plane; never a test on its own (M31) |
| T3 | s = 2 − e12, v = (2, −3, 5) | (18, −1, 25) | (6, 17, −25) | (18, −1, 25) | the difference, and the agreement of the complemented form, with a turn that is not a multiple of a quarter turn and a vector off its plane (rule A2) |
| T4 | s = 2 − e12, v = (1, 0, 0) | (3, 4, 0) | (−3, 4, 0) | (3, 4, 0) | that a vector in the rotor's plane shows the difference too once the turn is not a quarter turn |
| T5 | the mirror a = (1, 0, 0), v = (1, 2, 3) | (−1, 2, 3) | (1, −2, −3) | (−1, 2, 3) | the same for one mirror (M25) |

In T3, !s = 2 e123 − e3. Derivation of T3: s e1 s̃ = 3 e1 + 4 e2 (note 0029 section 2.8), and in the same way s e2 s̃ = −4 e1 + 3 e2 and s e3 s̃ = 5 e3, since e3 commutes with e12 and s s̃ = 5. So s (2 e1 − 3 e2 + 5 e3) s̃ = 2 (3, 4, 0) − 3 (−4, 3, 0) + 5 (0, 0, 5) = (18, −1, 25). By the note's Theorem A, `transform(v, s)` is −s̃ v s, the opposite turn of −v. With s̃ = 2 + e12: s̃ e1 s = (2 e1 − e2)(2 − e12) = 4 e1 − 2 e2 − 2 e2 − e1 = 3 e1 − 4 e2, s̃ e2 s = 4 e1 + 3 e2, and s̃ e3 s = 5 e3, so s̃ v s = 2 (3, −4, 0) − 3 (4, 3, 0) + 5 (0, 0, 5) = (−6, −17, 25), and `transform(v, s)` = (6, 17, −25). Checked: T1, T2, T4, and T5 by `checks/transform_sandwich_probe.cpp` and row W of `checks/transform_sandwich_check.py`; T3, and all three values of T1 and of T2 again, by `checks/rulings_values_probe.cpp` and `checks/rulings_values_check.py`.

### 5.2 Where each piece goes and what it must say

| Piece | Page and place | Written in | Must say | Values | Must not |
| --- | --- | --- | --- | --- | --- |
| The warning | `tutorial/` chapter 4, the signpost of inventory row K410, and the chapter's "A mistake you will make" (template T1 item 5) | U5, milestone D2 | In chapter 4's words, with no complement, antiproduct, or versor: this chapter turns a vector with the sandwich `r * v * ~r` in the reader's own helper; the library's function named `transform` is a different operation, which chapter 6 teaches; `transform(v, r)` with this chapter's rotor compiles and gives another vector. The mistake is a `cpp` block whose assertion states the wrong value as what the program gives. It says that the two agree on (1, 0, 0), which is why a test with a quarter turn of a vector in the rotor's plane passes. This is the chapter's objective "say why this chapter does not use the function named `transform`" | T1 for the mistake, T2 for the coincidence; T4 if the chapter shows the difference in the plane | explain why, which needs chapters 5 and 6; call `transform` wrong; use `transform` anywhere else in the chapter (rule A8) |
| The bridge | `tutorial/` chapter 6, rows K601 to K609, with K607 the statement and the chapter's "A mistake you will make" | U6, milestone D2 | That with each mirror given by its plane, `transform(v, !a)` is the reflection −a v a and `transform(v, !r)` is r v r̃, in R(3, 0, 0), and that the complement carries one product to the other (K603); what went wrong in chapter 4: the versor was not complemented, and `transform(v, r)` turned −v the opposite way; that `transform` does not divide by the factor (K609) | T5 and T2, the numbers chapter 4 computed; T3 for the agreement (rule A2); T1 for chapter 4's mistake, explained | use `bulk_dual` or any algebra but R(3, 0, 0); state the agreement with T2 alone |
| The first-things fact | `howto/first-things.md`, the tenth fact, after the nine that move unchanged | U3 when it moves the page, or U4 if U3 does not; milestone D1, so that the arriver has it before the tutorial exists | The text of section 5.3, with its example: one compiled program on s = 2 − e12 and (2, −3, 5) that asserts the three values of T3, and one sentence that r = 1 − e12 agrees on (1, 0, 0). It links the card's row | T3, with T2 named | link a page of milestone D3 |
| The card's row | `conventions` chapter, the row "`transform` against the classical sandwich" of template T5, and its section | U4, milestone D1 | Row: the choice; the rule in a clause, "`transform(x, V)` is not the sandwich; in R(n, 0, 0) the sandwich is `transform(x, !V)`"; the one-line expression `core::transform(v, s)` with s = 2 − e12 and v = (2, −3, 5); its value (6, 17, −25), beside `transform(v, !s)` = (18, −1, 25). Section: the rule; a compiled example on T3 and T5; that in R(p, q, 0) for any p and q, where every generator squares to 1 or −1, the sandwich is `transform(x, bulk_dual(V))`, and that `!` in place of `bulk_dual` fails there when p and q are both nonzero (note 0029 section 2.4, whose Theorem B is proved for those forms only); that in a form with other squares, such as one built with `core::from_gram` with a generator that squares to 2, the bulk dual does not give the sandwich in general; that a Euclidean versor complemented in rigid(d) is the motion that does to points what the versor does to vectors, with the same factor on the weight (note section 2.6); that a quarter turn in its own plane cannot tell the two apart (T2). The evidence in words: derived by hand, recomputed by a script that shares no code with the library, and confirmed by the library | T3 on the card; T1 to T5 in the section | name another library's or author's convention beyond what someone has read (rule A10); state the bulk-dual form for conformal(d) or any other form outside R(p, q, 0), for which nothing is proved (rule A3) |

The second language-model cold reader of rule R5 of `authoring-rules.md` is the only test of this documentation that the settlement names (decision 0255 part A item 8); the chapter cold reads of U5 and U6 meet the warning and the bridge as newcomers do. A human cold reader is still optional and unnamed (decision 0258 section 4 item 10).

### 5.3 The tenth fact of the first-things page: ruled

Ruled: the page gains this fact (decision 0258 section 4 item 11, which accepts decision 0255 part B item 11). Plan 0049 section 4.1 has the page "unchanged in substance"; the ruling adds this one fact, and the nine others move unchanged. The text below was drafted in unit U1; the page's author may change its words under the authoring rules and not its substance. Its example is in section 5.2.

> **`transform` is not the sandwich r v r̃.** `transform(x, V)` acts through the antiproduct, and it takes its versor in the form in which a mirror is a plane, not a normal. A rotor built with the geometric product, as r = b a from two mirror normals, is not in that form, and `transform(v, r)` compiles and gives a different vector: the opposite turn of −v. Write the sandwich itself, `r * v * ~r`, or give `transform` the complemented rotor, `transform(v, !r)`, which is the same turn in every R(n, 0, 0). A test with a quarter turn of a vector in the rotor's plane does not show the difference; a vector off the plane does.

## 6 What was not found

No source was read in this unit for how often readers hold each idea; the list comes from the plan, from the knowledge base's notes on sign conventions and on teaching geometric algebra, and from writing each chapter's first program before reading the headers. The cold reads of plan section 9.1 are the test: a wrong belief a reader states that is not here is added, with its value, in the unit that finds it.
