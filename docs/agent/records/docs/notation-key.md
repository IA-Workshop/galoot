# The notation key of the user documentation

Written by: Claude (agent, Fable 5.2, math-expert and hci-expert roles), 2026-11-21, for unit U1 of plan 0049. Not yet reviewed.

Decision 0253 item 27 rules the notation: Unicode symbols as the conventions specification and today's guide write them, words for the accented operations, and derivations in `text` blocks. This record fixes, within that ruling, every symbol a page may use, how each is read aloud, its C++ spelling, where the reader first meets it, and the letters used for each kind of object. Section 2 is the key as the map shows it. Section 5 reports a font probe that bears on the ruling; it is open item 4 of research note 0028 and changes nothing here.

## 1 Rules

1. A page uses only the symbols of section 2 and the letters of section 3. A symbol appears with its C++ spelling at its first use on a page.
2. The accented operations of the literature are written as words (section 4). The literature's mark is shown once, at the operation's definition, for the reader who goes on to the sources.
3. Blades are written e1, e12, e423: the letter e and one digit per generator, in the order of the product, with no subscript. In running text they are plain; in code font only where the text means the C++ name, as in `get<"e41">`.
4. The minus sign of mathematics is − (U+2212); in code it is the ASCII hyphen. Multiplication of numbers is ×, as in 2 × 2; the same sign is the cross product, which the reader knows.
5. A power is written with a superscript digit where one exists (x², λ²). The tutorial writes any other power in words ("−1 to the power of the grade times the antigrade"), because the caret is the wedge in every program on the page. The conventions chapter, the concepts pages, and the reference may write (−1)^(k(k−1)/2), as the existing pages do, and say once that the caret there is a power.
6. The geometric product is written ⟑ where it is defined and wherever ⟇ stands in the same formula. Elsewhere it is written by juxtaposition, a b, as the texts a reader will meet write it and as school algebra writes a product.
7. The dot of two vectors is written a · b, as in school. The dot of blades of higher grade, which only the concepts pages, the conventions chapter, and the reference use, is written a • b; the page that introduces it says that on vectors the two are one operation.
8. Coordinates are written in parentheses in the order of the layout: a vector (x, y, z), a point (x, y, z, w), a line (vx, vy, vz, mx, my, mz), a plane (gx, gy, gz, gw), a motor by its eight coefficients in the order e41, e42, e43, e1234, e23, e31, e12, 1.

## 2 The key

The map shows this table without its last two columns.

| Symbol | Code point | Read aloud | Meaning | C++ | First met | In an ASCII-only text |
| --- | --- | --- | --- | --- | --- | --- |
| e1, e2, … | | "e one" | a generator | a name in a layout, `"e1"` | chapter 1 | the same |
| e12, e41, e423 | | "e one two" | a blade, by its generators in order | `"e12"` | chapter 2 | the same |
| 1 | | "one" | the scalar unit, the blade of no generators | `"1"` | chapter 1 | the same |
| a · b | U+00B7 | "a dot b" | the dot product of two vectors | `dot(a, b)` | chapter 1 | `dot(a, b)` |
| a ∧ b | U+2227 | "a wedge b" | the wedge | `wedge(a, b)`, `a ^ b` | chapter 2 | `a ^ b` |
| a b, a ⟑ b | U+27D1 | "a times b", "the geometric product" | the geometric product | `geometric_product(a, b)`, `a * b` | chapter 3 | `a * b` |
| 𝟙 | U+1D7D9 | "the antiscalar" | the blade of all the generators | `"e123"`, `"e1234"` | chapter 5 | `e12...n` |
| a ∨ b | U+2228 | "a antiwedge b", "a meet b" | the antiwedge | `antiwedge(a, b)`, `a & b` | chapter 5 | `a & b` |
| a ⟇ b | U+27C7 | "a antiproduct b" | the geometric antiproduct | `geometric_antiproduct(a, b)`, `a % b` | chapter 6 | `a % b` |
| λ | U+03BB | "lambda" | the factor a versor leaves: V ⟇ antireverse(V) = λ 𝟙 | | chapter 6 | `lambda` |
| θ | U+03B8 | "theta" | an angle of turn, in radians; a versor holds θ/2 | | chapter 4 | `theta` |
| R(p, q, r) | | "R p q r" | the algebra with p generators that square to +1, q to −1, and r to 0 | `R<P, Q, Z>()` | chapter 1, in full in chapter 7 | the same |
| rigid(3) | | | R(3, 0, 1), with e4 the generator that squares to 0 | `rigid<3>()` | chapter 7 | the same |
| a • b | U+2022 | "a dot b" | the dot of two blades of one grade | `dot(a, b)` | concepts | `dot(a, b)` |
| a ∘ b | U+2218 | "a antidot b" | the antidot | `antidot(a, b)` | concepts | `antidot(a, b)` |
| u★ | U+2605 | "the bulk dual of u" | the right complement of the bulk | `bulk_dual(u)` | concepts | `bulk_dual(u)` |
| u☆ | U+2606 | "the weight dual of u" | the right complement of the weight | `weight_dual(u)` | concepts | `weight_dual(u)` |
| n, gr(u), ag(u) | | "the grade of u", "the antigrade of u" | the number of generators; the grade; n − gr(u) | | conventions, reference | the same |
| conformal(d) | | | the conformal algebra of d dimensions | `conformal<D>()` | concepts | the same |

The note the map carries under the key:

> Two of these symbols, ⟑ and ⟇, are in few fonts. If they show as empty boxes, read them as the geometric product and the antiproduct, which are `*` and `%` in every program on these pages; the font Noto Sans Math has both.

## 3 Letters

One letter means one kind of thing on every page. Capital P and W followed by a digit are not used at all (rule W9 of `authoring-rules.md`).

| Letters | Kind |
| --- | --- |
| s, t | a scalar, a plain number |
| x, y, z, w | coordinates; w is the weight, and no vector is called w |
| u, v | vectors |
| a, b, c | mirror normals, which are vectors |
| r, r1, r2 | rotors, built with the geometric product |
| g, h | planes: a bivector of R(3, 0, 0), or a plane of rigid(3) |
| V, V1, V2 | versors in the form `transform` takes; W for a second one in the conventions chapter |
| p, q, and p1, p2, p3 when three are needed | points |
| l, k | lines |
| Q, Q1, Q2 | motors |
| F | a flector |
| x, y | a multivector of any kind, in a general statement only; never beside a coordinate x |
| θ | the angle of a turn; λ the factor of a versor |
| n, k, m | the number of generators, a grade, an antigrade |
| i, j | indices of generators |

The existing pages write the third point of a plane as r; the tutorial uses p1, p2, p3 there, so that r stays the rotor from chapter 4 to chapter 8.

## 4 Operations written as words

| In words on the pages | C++ | The literature's mark, shown once | Defined in |
| --- | --- | --- | --- |
| reverse(r) | `reverse(r)`, `~r` | a tilde above, r̃ | chapter 4 |
| right(u) | `right_complement(u)`, `!u` | a bar above, ū | chapter 5 |
| left(u) | `left_complement(u)` | a bar below, u̲ | chapter 5 |
| antireverse(V) | `antireverse(V)` | a tilde below, V̰ | chapter 6 |
| bulk(x), weight(x) | `bulk(x)`, `weight(x)` | | chapter 7 |
| the grade involution of x | `grade_involution(x)` | a hat, x̂ | concepts |
| the antigrade involution of x | none in this release | a caron, x̌ | concepts |
| the inverse of v | none in this release; v divided by v · v | v⁻¹ | chapter 3 |

So the sandwich of chapter 4 reads r v reverse(r) in a formula and `r * v * ~r` in a program, and the transform of chapter 6 reads V ⟇ x ⟇ antireverse(V).

## 5 A font probe, for the owner

`fc-list` on this machine, 2026-11-21, counting the installed font families that have each character:

| Character | Families | Of them monospaced |
| --- | --- | --- |
| ⟑ U+27D1 | 1 (Noto Sans Math) | 0 |
| ⟇ U+27C7 | 1 (Noto Sans Math) | 0 |
| 𝟙 U+1D7D9 | 7 | 1 |
| ∧ U+2227, ∨ U+2228 | 105 | 15 |
| • U+2022 | 149 | 17 |
| ★ U+2605, ☆ U+2606 | 90 | 14 |

It is one machine, and a viewer that falls back to another font still shows the two glyphs where Noto Sans Math is installed. What it shows is that the two symbols for the products the library is built on depend on one font, and that no monospaced font here has them, while the plan sets derivations in `text` blocks. The key above is written as ruled. Research note 0028, open item 4, gives the options; under its option (b), rule 6 would add "inside a `text` or code block these two are written `*` and `%`", and nothing else in this record would change.

## 6 A check worth adding

Plan 0049 criterion A6 leaves "the symbols glossary matches the pages in both directions" to review. A script can hold half of it: every character outside ASCII on a page is in section 2's list, one of the marks of section 4, a superscript digit, −, or ×. It is about twenty lines beside `tests.docs_terms`, and a suggestion for unit U2, not part of this unit.
