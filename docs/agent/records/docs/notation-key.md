# The notation key of the user documentation

Written by: Claude (agent, Fable 5.2, math-expert and hci-expert roles), 2026-11-21, for unit U1 of plan 0049. Brought in step with the owner's ruling on the two product symbols (decision 0258 section 4 item 9) on 2026-11-21 by Claude (agent, Opus 5.5, math-expert and hci-expert roles): rules 6 and 9, the key's block column and its note, the last paragraph of section 4, and sections 5 and 6. Checked by an independent check of those changes on 2026-11-21; rule 9's interim choice for figures is listed as an open item in section 7 the same day by Claude (agent, Opus 5.5).

Decision 0253 item 27 rules the notation: Unicode symbols as the conventions specification and today's guide write them, words for the accented operations, and derivations in `text` blocks. Decision 0258 section 4 item 9 rules where two of those symbols, ⟑ and ⟇, give way to their C++ operators (rule 9). This record fixes, within those rulings, every symbol a page may use, how each is read aloud, its C++ spelling, how it is written inside a block, where the reader first meets it, and the letters used for each kind of object. Section 2 is the key as the map shows it. Section 5 reports the font probe on which item 9 was ruled.

## 1 Rules

1. A page uses only the symbols of section 2 and the letters of section 3. A symbol appears with its C++ spelling at its first use on a page.
2. The accented operations of the literature are written as words (section 4). The literature's mark is shown once, at the operation's definition, for the reader who goes on to the sources.
3. Blades are written e1, e12, e423: the letter e and one digit per generator, in the order of the product, with no subscript. In running text they are plain; in code font only where the text means the C++ name, as in `get<"e41">`.
4. The minus sign of mathematics is − (U+2212); in code it is the ASCII hyphen. Multiplication of numbers is ×, as in 2 × 2; the same sign is the cross product, which the reader knows.
5. A power is written with a superscript digit where one exists (x², λ²). The tutorial writes any other power in words ("−1 to the power of the grade times the antigrade"), because the caret is the wedge in every program on the page. The conventions chapter, the concepts pages, and the reference may write (−1)^(k(k−1)/2), as the existing pages do, and say once that the caret there is a power.
6. The geometric product is written as a sign where it is defined and wherever the antiproduct stands in the same formula: ⟑ in running text, `*` inside a block (rule 9). Elsewhere it is written by juxtaposition, a b, as the texts a reader will meet write it and as school algebra writes a product, in running text and in blocks alike.
7. The dot of two vectors is written a · b, as in school. The dot of blades of higher grade, which only the concepts pages, the conventions chapter, and the reference use, is written a • b; the page that introduces it says that on vectors the two are one operation.
8. Coordinates are written in parentheses in the order of the layout: a vector (x, y, z), a point (x, y, z, w), a line (vx, vy, vz, mx, my, mz), a plane (gx, gy, gz, gw), a motor by its eight coefficients in the order e41, e42, e43, e1234, e23, e31, e12, 1.
9. The geometric product and the antiproduct are written ⟑ and ⟇ in running text, and `*` and `%`, their C++ operators, inside a block. Running text is what a reader sees in the page's proportional font: paragraphs, headings, lists, tables (the map's key and the conventions card among them), and captions. A figure's labels and its alternative text are neither running text nor a block, and the ruling does not name them; until it does, they write the two products in words, "the antiproduct of g and h", which reads in any font and aloud. That interim choice is not ruled or settled, and section 7 lists it as an open item. A block is a `text` block, a `cpp` block of any kind, and the comments inside a `cpp` block. Inside a block, `*` stands wherever running text would write ⟑ and `%` wherever it would write ⟇, and juxtaposition stays where rule 6 allows it. Every other symbol of section 2 is written the same in both places, and an inline code span is C++ and needs no rule. The reason: no monospaced font on the machine probed has either glyph (section 5), and blocks are monospaced. One line of each kind, from chapter 6:
   - In running text: the two mirror planes multiply to the complemented rotor, (e23 + e31) ⟇ e23 = e123 − e3.
   - In a `text` block, the same rotor turning e3, with antireverse(e123 − e3) = e123 + e3; where both products stand, as in the definition on the second line, `*` is written as well:

     ```text
     (e123 − e3) % e3 % (e123 + e3) = 2 e3
     a % b = left(right(a) * right(b))
     ```

## 2 The key

The map shows this table without its last two columns.

| Symbol | Code point | Read aloud | Meaning | C++ | In a block | First met | In an ASCII-only text |
| --- | --- | --- | --- | --- | --- | --- | --- |
| e1, e2, … | | "e one" | a generator | a name in a layout, `"e1"` | the same | chapter 1 | the same |
| e12, e41, e423 | | "e one two" | a blade, by its generators in order | `"e12"` | the same | chapter 2 | the same |
| 1 | | "one" | the scalar unit, the blade of no generators | `"1"` | the same | chapter 1 | the same |
| a · b | U+00B7 | "a dot b" | the dot product of two vectors | `dot(a, b)` | the same | chapter 1 | `dot(a, b)` |
| a ∧ b | U+2227 | "a wedge b" | the wedge | `wedge(a, b)`, `a ^ b` | the same | chapter 2 | `a ^ b` |
| a b, a ⟑ b | U+27D1 | "a times b", "the geometric product" | the geometric product | `geometric_product(a, b)`, `a * b` | a b; `*` where rule 6 asks for the sign | chapter 3 | `a * b` |
| 𝟙 | U+1D7D9 | "the antiscalar" | the blade of all the generators | `"e123"`, `"e1234"` | the same | chapter 5 | `e12...n` |
| a ∨ b | U+2228 | "a antiwedge b", "a meet b" | the antiwedge | `antiwedge(a, b)`, `a & b` | the same | chapter 5 | `a & b` |
| a ⟇ b | U+27C7 | "a antiproduct b" | the geometric antiproduct | `geometric_antiproduct(a, b)`, `a % b` | `%` | chapter 6 | `a % b` |
| λ | U+03BB | "lambda" | the factor a versor leaves: V ⟇ antireverse(V) = λ 𝟙 | | the same | chapter 6 | `lambda` |
| θ | U+03B8 | "theta" | an angle of turn, in radians; a versor holds θ/2 | | the same | chapter 4 | `theta` |
| R(p, q, r) | | "R p q r" | the algebra with p generators that square to +1, q to −1, and r to 0 | `R<P, Q, Z>()` | the same | chapter 1, in full in chapter 7 | the same |
| rigid(3) | | | R(3, 0, 1), with e4 the generator that squares to 0 | `rigid<3>()` | the same | chapter 7 | the same |
| a • b | U+2022 | "a dot b" | the dot of two blades of one grade | `dot(a, b)` | the same | concepts | `dot(a, b)` |
| a ∘ b | U+2218 | "a antidot b" | the antidot | `antidot(a, b)` | the same | concepts | `antidot(a, b)` |
| u★ | U+2605 | "the bulk dual of u" | the right complement of the bulk | `bulk_dual(u)` | the same | concepts | `bulk_dual(u)` |
| u☆ | U+2606 | "the weight dual of u" | the right complement of the weight | `weight_dual(u)` | the same | concepts | `weight_dual(u)` |
| n, gr(u), ag(u) | | "the grade of u", "the antigrade of u" | the number of generators; the grade; n − gr(u) | | the same | conventions, reference | the same |
| conformal(d) | | | the conformal algebra of d dimensions | `conformal<D>()` | the same | concepts | the same |

The note the map carries under the key:

> Two of these symbols, ⟑ and ⟇, are in few fonts. In derivations and programs they are written `*` and `%`, as in C++, and only the text around them uses the symbols. If they show there as empty boxes, read them as the geometric product and the antiproduct; the font Noto Sans Math has both.

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

So the sandwich of chapter 4 reads r v reverse(r) in a formula, in running text and in a `text` block, and `r * v * ~r` in a program. The transform of chapter 6 reads V ⟇ x ⟇ antireverse(V) in running text, `V % x % antireverse(V)` in a `text` block (rule 9), and `core::transform(x, V)` in a program.

## 5 The font probe behind rule 9

`fc-list` on this machine, 2026-11-21, counting the installed font families that have each character:

| Character | Families | Of them monospaced |
| --- | --- | --- |
| ⟑ U+27D1 | 1 (Noto Sans Math) | 0 |
| ⟇ U+27C7 | 1 (Noto Sans Math) | 0 |
| 𝟙 U+1D7D9 | 7 | 1 |
| ∧ U+2227, ∨ U+2228 | 105 | 15 |
| • U+2022 | 149 | 17 |
| ★ U+2605, ☆ U+2606 | 90 | 14 |

It is one machine, and a viewer that falls back to another font still shows the two glyphs where Noto Sans Math is installed. What it shows is that the two symbols for the products the library is built on depend on one font, and that no monospaced font here has them, while the plan sets derivations in `text` blocks. Research note 0029, open item 4, gave the options. The owner ruled its option (b) (decision 0258 section 4 item 9): the symbols in running text, and `*` and `%` inside code and text blocks. Rules 6 and 9, the key's block column, and the map's note carry the ruling. The antiscalar 𝟙 is in one monospaced family here and stays a symbol in blocks, as the ruling names only the two products.

## 6 A check worth adding

Plan 0049 criterion A6 leaves "the symbols glossary matches the pages in both directions" to review. A script can hold half of it: every character outside ASCII on a page is in section 2's list, one of the marks of section 4, a superscript digit, −, or ×. With rule 9 the same script can also report a ⟑ or a ⟇ inside a block. It is about twenty lines beside `tests.docs_terms`, and a suggestion for unit U2, not part of this unit.

## 7 Open item

1. **The two products in a figure's labels and alternative text.** Decision 0258 section 4 item 9 rules running text and blocks only. Rule 9 counts a caption as running text, so a caption writes ⟑ and ⟇, while its figure's labels and alternative text write the two products in words; a caption and its alternative text can then describe one product in two ways. The figures are SVG files drawn by one script, whose labels show in whatever font the viewer has, and alternative text is read aloud by a screen reader, which may announce ⟇ by its Unicode name or not at all.
   - (a) Words in labels and alternative text, as rule 9 has it now. Readable in any font and aloud; the cost is the mismatch with a caption that uses the symbol.
   - (b) The symbols in labels and alternative text, as in running text. One spelling for one figure; a viewer without Noto Sans Math sees empty boxes in a label, and a screen reader may read the symbol badly.
   - (c) The symbols in labels, words in alternative text, and the caption in words wherever its figure's labels show a product. Matches what each reader meets; it adds a third rule an author must remember.
   - Recommendation: (a), recorded by the coordinator as a reading of item 9 before U2 writes the figure script, since no figure exists yet and the choice changes only labels.
