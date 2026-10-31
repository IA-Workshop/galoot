# Galoot conventions specification

Status: version 1.2, ratified by the owner on 2026-10-31. A convention here changes only by a change to this document that the owner ratifies (constitution C10).

This specification fixes the conventions that every part of Galoot shares: how generators and blades are numbered and named, the signs of complements and duals, the definitions of the products, the coordinates of the rigid and conformal objects, and the meaning of each operator. Each entry carries its evidence:

- Checked: verified exactly by [`test/reference/conventions_check.py`](../../../test/reference/conventions_check.py), which shares no code with the library.
- Confirmed: read in Lengyel's wikis or posters and found to agree, as recorded in research note [0005](../research-notes/0005-lengyel-sources.md) (VVUQ V9).
- Study: taken from the design study ([galoot-design-study.md](../../galoot-design-study.md)); the original source has not yet been read to confirm it (VVUQ V9).
- Proposed: a choice with no external source, made for consistency.

Sources rank as follows. The design study is the most canonical reference for Galoot. For mathematical conventions, Lengyel's wikis ([rigidgeometricalgebra.org](https://rigidgeometricalgebra.org), [conformalgeometricalgebra.org](https://conformalgeometricalgebra.org)) are the preferred source. Colapinto's thesis and the Versor library inform generic-programming technique, not conventions. A disagreement between ranked sources is recorded in section 8 (constitution C11), and the owner decides it.

Notation: n is the number of generators, gr(u) the grade and ag(u) = n − gr(u) the antigrade of a blade u, **1** the scalar unit, and 𝟙 the antiscalar.

## 1 Generators and blades

1.1 Generators are named e1 to en, counting from 1. A blade is stored as a bitmask in which bit i − 1 stands for e_i. The canonical blade of a mask lists its indices in increasing order, so the mask {1, 2, 4} is e124. (Proposed)

1.2 In the diagonal signature R(p, q, r), the first p generators square to +1, the next q to −1, and the last r to 0. This puts a null generator last, as in Lengyel's rigid algebra; some libraries and texts put it first, as e0. (Proposed)

1.3 `rigid(d)` is R(d, 0, 1): e1 to ed are Euclidean and e(d+1) is the null projective generator (e4 when d = 3). (Confirmed)

1.4 `conformal(d)` has e1 to ed Euclidean, e(d+1) the origin, and e(d+2) the point at infinity, with both of the last two null and e(d+1) · e(d+2) = −1. Its diagonal counterpart is R(d+1, 1, 0), in which e(d+1) squares to +1 and e(d+2) to −1, and the exact change of basis is origin = ½(e(d+2) − e(d+1)) and infinity = e(d+2) + e(d+1). (Checked)

1.5 The scalar unit is **1** and the antiscalar is 𝟙 = e12…n with sign +1. (Confirmed)

1.6 A named type stores its coefficients on a list of oriented blades, each a mask and a sign, so that coefficients match the published formulas with no hidden negation. In rigid(3) the oriented names are e23 = +e23, e31 = −e13, e12 = +e12, e41 = −e14, e42 = −e24, e43 = −e34, e423 = +e234, e431 = −e134, e412 = +e124, and e321 = −e123. (Checked; Confirmed)

1.7 The components of a generated result that has no declared type appear in order of grade and, within a grade, in lexicographic order of their index lists: e12, e13, e14, e23, e24, e34. (Proposed)

## 2 Grade operations

2.1 The reverse multiplies a blade of grade k by (−1)^(k(k−1)/2); the antireverse uses the antigrade in place of the grade; the grade involution multiplies by (−1)^k. (Confirmed for the reverse and antireverse, which every Checked transform below uses; the grade involution is Proposed, for 5.6)

## 3 Complements and duals

3.1 The right complement ū and the left complement u̲ of a basis blade u are fixed by u ∧ ū = 𝟙 and u̲ ∧ u = 𝟙, and extend linearly. They satisfy u̲ = (−1)^(gr·ag) ū, and the left complement of the right complement is the identity. In rigid(3) this gives the 16-entry table of the design study. (Checked)

3.2 The bulk of u applies the metric exomorphism, and the weight applies the antiexomorphism. In a diagonal metric, the bulk keeps the blades whose square is nonzero and the weight keeps the blades whose complement's square is nonzero; in rigid(3) the bulk is the part without e4 and the weight the part with it. (Confirmed, with the diagonal case Checked)

3.3 The bulk dual u★ is the right complement of the bulk, and the weight dual u☆ is the right complement of the weight. Then a ∧ b★ = (a • b)𝟙 for a and b of equal grade. Duals built on the left complement exist only under their own names, `left_bulk_dual` and `left_weight_dual`. (Checked; Confirmed)

3.4 In a degenerate metric the double dual u★★ is zero, so only the complements are invertible. (Checked for rigid(3); Confirmed)

## 4 Products

4.1 The geometric product is ⟑. The wedge a ∧ b keeps the terms of a ⟑ b whose blades share no generator. The dot a • b = aᵀGb is a scalar, and the antidot a ∘ b = aᵀ𝔾b multiplies 𝟙, where G and 𝔾 are the metric exomorphism and antiexomorphism. (Confirmed)

4.2 The geometric antiproduct is a ⟇ b = left(right(a) ⟑ right(b)), and the antiwedge is a ∨ b = left(right(a) ∧ right(b)). Both products and both antiproducts are associative, and the De Morgan laws hold with either complement: right(a ⟑ b) = right(a) ⟇ right(b), right(a ∧ b) = right(a) ∨ right(b), and the same with left. (Checked)

4.3 The projection of a onto b is b ∨ (a ∧ b☆). For a point projected onto a unitized plane, the result has weight +1. (Checked)

## 5 Rigid objects in rigid(3)

5.1 A point is x e1 + y e2 + z e3 + w e4, with weight w; it is unitized when w = 1. (Confirmed)

5.2 A line has its direction (vx, vy, vz) on e41, e42, e43 and its moment (mx, my, mz) on e23, e31, e12. The line p ∧ q through unitized points runs from p to q: its direction is q − p. (Checked)

5.3 A plane has its normal (gx, gy, gz) on e423, e431, e412 and its offset gw on e321, and a point p lies on it when p ∨ g = gx x + gy y + gz z + gw w is zero. The plane p ∧ q ∧ r through three points that run counterclockwise when seen from the tip of its normal has that normal. (Checked)

5.4 A motor Q transforms any object x as Q ⟇ x ⟇ Q̰, where Q̰ is the antireverse. This equals the outermorphism of Q's action on points, so lines and planes move as the points that span them. Applying Q1 and then Q2 is one transform by Q2 ⟇ Q1. (Checked; Confirmed except the order of composition, which Lengyel's pages do not state)

5.5 The translator t_x e23 + t_y e31 + t_z e12 + 𝟙 moves points by 2t. The rotor l sin φ + 𝟙 cos φ, for a unitized line l, turns by 2φ, counterclockwise when seen from the tip of the direction of l. (Checked, for l the z axis; Confirmed)

5.6 A flector F transforms any object x as −F ⟇ x̂ ⟇ F̰, where x̂ is the grade involution. This equals the outermorphism of F's action on points: a reflected point keeps a positive weight, and a reflected line or plane is the one spanned by the reflected points in the same order. (Checked, for a plane and for a plane composed with a motor) See the first disagreement in section 8.

5.7 Constructors take geometric quantities: `rotation(l, theta)` turns by theta and builds l sin(θ/2) + 𝟙 cos(θ/2), and `translation(d)` moves by d and builds ½d on e23, e31, e12 plus 𝟙. Raw motor coefficients hold half-angles and half-distances. (Proposed)

## 6 Conformal objects in conformal(3)

6.1 The round point with center p and squared radius r² is p + e4 + ½(p² + r²)e5. Its square P • P is −r², so a round point of zero radius is null and an imaginary round has r² < 0. A round point is not the dual of the sphere with its center and radius: that sphere is its container, and the bulk dual of the real sphere with center p and radius r is −(p + e4 + ½(p² − r²)e5). (Confirmed; the value of P • P also Checked)

6.2 Flat objects are the rigid objects wedged with e5, with their coefficients unchanged. (Study)

6.3 A conformal object splits into four parts: the round bulk has neither e4 nor e5, the round weight has e4 only, the flat bulk e5 only, and the flat weight both. (Confirmed)

6.4 The operations on rounds and flats (carrier, cocarrier, center, container, partner, attitude, radius) are specified with the conformal model in roadmap phase 3. (Study)

## 7 Operators

7.1 Named functions are the interface. These operators are aliases of them in every algebra:

| Operator | Meaning | Named function |
| --- | --- | --- |
| `a * b` | Geometric product ⟑ | `gp` |
| `a % b` | Geometric antiproduct ⟇ | `antigp` |
| `a ^ b` | Wedge ∧ | `wedge` |
| `a & b` | Antiwedge ∨ (meet) | `antiwedge` |
| `~a` | Reverse | `reverse` |
| `!a` | Right complement | `right_complement` |
| `a + b`, `a - b` | Sum and difference | |
| `s * a`, `a * s`, `a / s` | Scaling by a scalar s | |

The antireverse, left complement, duals, dot, antidot, and transforms have no operator. `compose(Q2, Q1)` is Q2 ⟇ Q1, and `transform(x, V)` applies 5.4 or 5.6. Because `^` and `&` bind more loosely than `==`, `+`, and `*`, expressions that mix them with other operators are fully parenthesized. (Study, with `compose` and `transform` Checked)

## 8 Recorded disagreements

| Topic | Disagreement | Choice | Test |
| --- | --- | --- | --- |
| Flector transform | The design study, Lengyel's Flector page, and his rigid poster give x′ = −F ⟇ x ⟇ F̰ at every grade. That form agrees with the outermorphism on lines, but on points and planes it gives the negation, so a reflected unitized point has weight −1. Lengyel's Reflection page writes F ⟇ x ⟇ F̰ for points, and the poster's flector matrix keeps the point weight positive; both agree with 5.6 on points. The Flector page's line row also has the sign of its moment formula reversed, so that row is not used as a reference value. | 5.6 | `check_versors` |
| Dual sign | The design study reports that Terathon's `BulkDual` and `WeightDual` for points and planes return left duals, which negate a ∧ b★ = (a • b)𝟙 at odd grades in four dimensions; the wiki uses right duals, which note 0005 confirms. The Terathon side has not been tested. | 3.3 | `check_identities` |
| Partner of a circle | The design study suspects missing parentheses in Terathon's `Partner(Circle3D)` relative to the wiki formula. The wiki's explicit formula equals its general definition par(u) = (−1)^(gr+1) con(u☆) ∨ car(u) on genuine circles but not on other inputs of the same type, so a test must use genuine circles. The Terathon side has not been tested. | Decided in phase 3 | To be written |
| Conformal duals | The Duals table of the conformal wiki, in its round point, flat point, plane, and sphere rows, is the negation of the wiki's own definition u★ = right(G u). The conformal poster agrees with the definition. | 3.3, the definition; confirmed by the owner in phase 3 | To be written |

## 9 Not yet specified

Graded signatures with phases other than ±1, their phase matrix and generator order, and the conventions of the extension modules are specified when their roadmap phases begin.

## Choices ratified in 1.0

These choices had no settled source and were ratified with this version:

1. A null generator comes last in R(p, q, r) (1.2).
2. Results without a declared type are ordered by grade, then lexicographically (1.7).
3. Duals use the right complement, and left duals exist only under their own names (3.3).
4. The flector transform carries the grade involution, so it agrees with the outermorphism at every grade (5.6).
5. Constructors take full angles and distances, and raw coefficients hold halves (5.7).
6. The operator table of 7.1.

## Revision history

| Version | Date | Change |
| --- | --- | --- |
| 0.1 | 2026-10-31 | First proposal |
| 1.0 | 2026-10-31 | Ratified by the owner, unchanged from 0.1 |
| 1.1 | 2026-10-31 | Ranking of sources added, as the owner directed |
| 1.2 | 2026-10-31 | Confirmed label added from note 0005; 6.1 clarified; 6.2 split into 6.2 to 6.4; flector, dual, and partner rows updated and a conformal-duals row added; no convention changed; ratified |
