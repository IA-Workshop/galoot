# Galoot conventions specification

Status: version 1.17, in which 1.4 names the generators of R(d+1, 1, 0) e′1 to e′(d+2) and states the blade map and the antiscalar's sign, 1.8 states what a change of basis preserves and with which factor, and section 8's "Conformal duals" row records the owner's confirmation, ratified by the owner on 2026-11-09 ("Accept all recommendations", decisions 0219 items 4 and 5 and 0223 item 9). Version 1.16, in which section 8's "Dual sign" and "Conformal duals" rows name in their Test column the C++ tests that tell each alternative apart, ruled by the owner on 2026-11-08 ("Accept and proceed", decision 0216 item 5), and the "Flector transform" row adds the plane case that the owner folded into WU14 (decision 0216 item 4). Version 1.15, in which section 8's flector row gives the new path of the rga3 preset's objects test, confirmed by the owner on 2026-11-08 ("Continue as recommended", decision 0213). Version 1.14, with 7.2's printed form, ruled by the owner on 2026-11-07 ("accept all recommendations", decision 0196 item 5); its wording confirmed by the owner on 2026-11-08 ("Confirmed the recommendations", decision 0207), as was the addition of two C++ tests to section 8's flector row. Version 1.13, in which 5.7's `rotation` divides its axis by the length of the axis's direction, ratified by the owner on 2026-11-08 ("Accept all recommendations", decision 0202 item 1); the exact wording of 5.7 confirmed by the owner on 2026-11-08 ("confirmed the recommendations", decision 0210). Version 1.12, with 5.8's motor and flector order, 5.7's `translation(dx, dy, dz)`, and 5.2's moment, ratified by the owner on 2026-11-07 ("accept all recommendations", decision 0196); the exact wording of 5.8, which takes the coefficient letters of Lengyel's pages, was confirmed by the owner on 2026-11-08 (decisions 0197 and 0202). Version 1.11, with 4.5's clause on coarse rounding scalars, confirmed by the owner on 2026-11-07 ("accept all recommendations", decision 0196). Version 1.10, ratified by the owner on 2026-11-07, including 4.4's sentence on the outermorphism with the factor λ^(k−1) and 2.1's two clauses relating the antigrade involution to the grade involution and the complement, which the owner confirmed ("agree. proceed as recommended", decision 0194). (Version 1.10: "proceed with recommendations", decision 0192; version 1.9, 2026-11-04: "take all recommendations", decision 0174; version 1.8: "agreed", decisions 0167 and 0168; version 1.7: "proceed as recommended", decision 0161; version 1.6: "proceed as recommended", decisions 0155 and 0159). A convention here changes only by a change to this document that the owner ratifies (constitution C10).

This specification fixes the conventions that every part of Galoot shares: how generators and blades are numbered and named, the signs of complements and duals, the definitions of the products, the coordinates of the rigid and conformal objects, and the meaning of each operator. Each entry carries its evidence:

- Checked: verified exactly by [`test/reference/conventions_check.py`](../../../test/reference/conventions_check.py), which shares no code with the library.
- Confirmed: read in Lengyel's wikis or posters and found to agree, as recorded in research note [0005](../research-notes/0005-lengyel-sources.md) (VVUQ V9).
- Study: taken from the design study ([0000-design-study.md](../research-notes/0000-design-study.md)); the original source has not yet been read to confirm it (VVUQ V9).
- Proposed: a choice with no external source, made for consistency.

Sources rank as follows. The design study is the most canonical reference for Galoot. For mathematical conventions, Lengyel's wikis ([rigidgeometricalgebra.org](https://rigidgeometricalgebra.org), [conformalgeometricalgebra.org](https://conformalgeometricalgebra.org)) are the preferred source. Colapinto's thesis and the Versor library inform generic-programming technique, not conventions. A disagreement between ranked sources is recorded in section 8 (constitution C11), and the owner decides it.

Each section carries a component marker: *core* for what the core implements and *model* for what an extension implements on the core (constitution C2). Sections 1 to 4 and 7 are core; section 5 belongs to the rga3 model and section 6 to the cga3 model.

Notation: n is the number of generators, gr(u) the grade and ag(u) = n − gr(u) the antigrade of a blade u, **1** the scalar unit, and 𝟙 the antiscalar.

## 1 Generators and blades

Component: core.

1.1 Generators are named e1 to en, counting from 1. A blade is stored as a bitmask in which bit i − 1 stands for e_i. The canonical blade of a mask lists its indices in increasing order, so the mask {1, 2, 4} is e124. In a metric that is not diagonal, the blade of a mask is the wedge product e_a1 ∧ … ∧ e_ak of its generators, not their geometric product, so that e45 = e4 ∧ e5 in conformal(3) while e4 ⟑ e5 = −1 + e45. (Proposed)

1.2 In the diagonal signature R(p, q, r), the first p generators square to +1, the next q to −1, and the last r to 0. This puts a null generator last, as in Lengyel's rigid algebra; some libraries and texts put it first, as e0. (Proposed)

1.3 `rigid(d)` is R(d, 0, 1): e1 to ed are Euclidean and e(d+1) is the null projective generator (e4 when d = 3). (Confirmed)

1.4 `conformal(d)` has e1 to ed Euclidean, e(d+1) the origin, and e(d+2) the point at infinity, with both of the last two null and e(d+1) · e(d+2) = −1. Its diagonal counterpart is R(d+1, 1, 0), whose generators are written e′1 to e′(d+2): e′(d+1) squares to +1 and e′(d+2) to −1. The change of basis from conformal(d) to R(d+1, 1, 0) (1.8) keeps e_i = e′_i for i ≤ d and writes the origin as ½(e′(d+2) − e′(d+1)) and the point at infinity as e′(d+2) + e′(d+1). Its determinant is −1, so e(d+1)(d+2) maps to −e′(d+1)(d+2) and 𝟙 to −𝟙′. (Checked; Confirmed for d = 3, research note 0026)

1.5 The scalar unit is **1** and the antiscalar is 𝟙 = e12…n with sign +1. (Confirmed)

1.6 A named type stores its coefficients on a list of oriented blades, each a mask and a sign, so that coefficients match the published formulas with no hidden negation. In rigid(3) the oriented names are e23 = +e23, e31 = −e13, e12 = +e12, e41 = −e14, e42 = −e24, e43 = −e34, e423 = +e234, e431 = −e134, e412 = +e124, and e321 = −e123. (Checked; Confirmed)

1.7 The components of a generated result that has no declared type appear in order of grade and, within a grade, in lexicographic order of their index lists: e12, e13, e14, e23, e24, e34. (Proposed)

1.8 A change of basis from an algebra with Gram matrix G to one with Gram matrix G′ on the same number n of generators is an invertible rational matrix P whose row i writes e_i in the generators e′1 to e′n, with P G′ Pᵀ = G. It sends e_A to Σ_B det P[A, B] e′_B over the blades B of A's grade, so it keeps grades and sends 1 to 1′ and 𝟙 to det(P) 𝟙′, and its inverse is the change by P⁻¹. Write φ for it and d = det P. It preserves the geometric product, the wedge, the dot, the reverse, the antireverse, and the grade and antigrade involutions; φ(a ∨ b) = φ(a) ∨ φ(b) / d, and φ(u★) = d φ(u)★, as for the left bulk dual. The geometric antiproduct, the antidot, the weight duals, and compose are preserved exactly when P⁻ᵀ G′ P⁻¹ = G, each with the factor 1/d, and transform, project, and antiproject then with 1/d²; for a nondegenerate form this condition makes d = ±1, and it holds for every change between forms whose Gram matrices are their own inverses, such as the change of 1.4 and that of a Witt form to R(m, m, 0). The complements are preserved, with the factor d, exactly when P is orthogonal; the bulk exactly when P Pᵀ G = G, and the weight exactly when G′ Pᵀ P = G′, each with the factor 1, which for a nondegenerate form also mean that P is orthogonal. So the complements, the bulk, and the weight depend on the basis: in conformal(3) the bulk of e4 is −e5, while by way of R(4, 1, 0) it is −½ e5. (Checked on eleven changes, the general statements proved in research note 0026)

## 2 Grade operations

2.1 The reverse multiplies a blade of grade k by (−1)^(k(k−1)/2); the antireverse uses the antigrade in place of the grade; the grade involution multiplies by (−1)^k; the antigrade involution multiplies a blade of antigrade m by (−1)^m, so it is (−1)^n times the grade involution, and the right complement takes it to the grade involution: right(x̌) is the grade involution of right(x). (Confirmed for the reverse and antireverse, which every Checked transform below uses; the grade involution is Proposed, for 5.6; the antigrade involution is Proposed, for 4.4, and its relations Checked)

## 3 Complements and duals

3.1 The right complement ū and the left complement u̲ of a basis blade u are fixed by u ∧ ū = 𝟙 and u̲ ∧ u = 𝟙, and extend linearly. They satisfy u̲ = (−1)^(gr·ag) ū, and the left complement of the right complement is the identity. In rigid(3) this gives the 16-entry table of the design study. (Checked)

3.2 The bulk of u applies the metric exomorphism, and the weight applies the antiexomorphism. In a diagonal metric, the bulk keeps the blades whose square is nonzero, each times the product of the squares of its generators, and the weight keeps the blades whose complement's square is nonzero, each times the product of the squares of the generators it lacks. In rigid(d) these factors are 1, the bulk is the part without e(d+1), and the weight is the part with it; with two or more null generators some blades are in neither. (Confirmed, with the diagonal case Checked)

3.3 The bulk dual u★ is the right complement of the bulk, and the weight dual u☆ is the right complement of the weight. Then a ∧ b★ = (a • b)𝟙 for a and b of equal grade. Duals built on the left complement exist only under their own names, `left_bulk_dual` and `left_weight_dual`. (Checked; Confirmed)

3.4 In a degenerate metric the double dual u★★ is zero, so only the complements are invertible. (Checked for rigid(3); Confirmed)

## 4 Products

Component: core.

4.1 The geometric product is ⟑. It is the associative product fixed by the defining relation e_i ⟑ e_j + e_j ⟑ e_i = 2 g_ij on the generators, so that e_i ⟑ e_i = g_ii, where g is the metric. The wedge a ∧ b is the exterior product, defined on blades by e_A ∧ e_B = ± e_{A∪B} when A and B share no generator and 0 otherwise, with the sign of the reordering, and extended bilinearly; it does not depend on the metric. It equals the part of a ⟑ b of grade gr(a) + gr(b), which in a diagonal metric is the terms of a ⟑ b whose blades share no generator. The dot a • b = aᵀGb is a scalar, and the antidot a ∘ b = aᵀ𝔾b multiplies 𝟙, where G and 𝔾 are the metric exomorphism and antiexomorphism. (Confirmed; the metric-independent wording is Checked, note 0009 B5)

4.2 The geometric antiproduct is a ⟇ b = left(right(a) ⟑ right(b)), and the antiwedge is a ∨ b = left(right(a) ∧ right(b)). Both products and both antiproducts are associative, and the De Morgan laws hold with either complement: right(a ⟑ b) = right(a) ⟇ right(b), right(a ∧ b) = right(a) ∨ right(b), and the same with left. (Checked)

4.3 The projection of a onto b is b ∨ (a ∧ b☆), and the antiprojection of a onto b is b ∧ (a ∨ b☆), written `project(a, b)` and `antiproject(a, b)` with the object first. For a point projected onto a unitized plane, the result has weight +1. The antiprojection of a plane onto a unitized point is the plane through the point with the same normal, and that of a line onto a unitized point is the line through the point with the same direction. (Checked; Confirmed, research note [0021](../research-notes/0021-projections-page.md))

4.4 `transform(x, V)` applies a versor V to x by the parity of the antigrades of V's blades. An even V gives V ⟇ x ⟇ V̰, as in 5.4. An odd V gives −V ⟇ x̌ ⟇ V̰, where x̌ is the antigrade involution (2.1); when n is even x̌ is the grade involution x̂, as in 5.6, and when n is odd x̌ = −x̂, so an odd V gives V ⟇ x̂ ⟇ V̰. Either way, for a versor V, an antiproduct of hyperplanes, with V ⟇ V̰ = λ𝟙 and λ ≠ 0, the transform T satisfies λ^(k−1) T(a1 ∧ … ∧ ak) = T(a1) ∧ … ∧ T(ak), so it is the outermorphism of V's action on vectors when λ = 1, as for a unitized motor or flector, and a single reflection maps each vector of its mirror to itself times λ. A V with blades of both parities is rejected at compile time. The result has the blade set of x, and in checking mode every discarded part is verified to be zero, exactly for an exact scalar and within the operation's forward error bound for a rounding scalar (4.5). `compose(Q2, Q1)` is Q2 ⟇ Q1 for versors Q1 and Q2 each of one parity, and applies Q1 first: transform(x, compose(Q2, Q1)) = transform(transform(x, Q1), Q2). (Proposed; the odd sign Checked for n = 3, 4, and 5)

4.5 A conversion to a blade set that omits blades of the input is lossy, is named `truncate<Blades>(x)`, or `truncate<Type>(x)` when the target is named by its type, and in checking mode verifies that what it drops is zero when the scalar is exact. An operation that drops part of its own result, such as `transform`, verifies on a rounding scalar that the dropped part is within that operation's forward error bound (decision 0010). The rounding scalars it checks are the IEEE 754 binary floating-point types that round to nearest, those whose `std::numeric_limits` give `is_iec559` and `round_to_nearest`: `float` and `double` are tested, and `long double` is neither tested nor claimed. A type of this kind whose precision is too coarse for the operation's error bound to hold, such as `bfloat16`, is not checked either. Other inexact scalars, such as `std::complex<double>`, are not checked, and the operation's interface says so. It is never implicit. `part<Blades>(x)` and `part<Type>(x)` keep the same components and never check: what they drop may be nonzero, as in a grade part. (Proposed)

## 5 Rigid objects in rigid(3)

Component: model (rga3). Sections 5.4 and 5.6 are instances of 4.4.

5.1 A point is x e1 + y e2 + z e3 + w e4, with weight w; it is unitized when w = 1. (Confirmed)

5.2 A line has its direction (vx, vy, vz) on e41, e42, e43 and its moment (mx, my, mz) on e23, e31, e12. The line p ∧ q through unitized points runs from p to q: its direction is q − p. Its moment is p × q for points of any weight. (Checked)

5.3 A plane has its normal (gx, gy, gz) on e423, e431, e412 and its offset gw on e321, and a point p lies on it when p ∨ g = gx x + gy y + gz z + gw w is zero. The plane p ∧ q ∧ r through three points that run counterclockwise when seen from the tip of its normal has that normal. (Checked)

5.4 A motor Q transforms any object x as Q ⟇ x ⟇ Q̰, where Q̰ is the antireverse. This equals the outermorphism of Q's action on points, so lines and planes move as the points that span them. Applying Q1 and then Q2 is one transform by Q2 ⟇ Q1. (Checked; Confirmed except the order of composition, which Lengyel's pages do not state)

5.5 The translator t_x e23 + t_y e31 + t_z e12 + 𝟙 moves points by 2t. The rotor l sin φ + 𝟙 cos φ, for a unitized line l, turns by 2φ, counterclockwise when seen from the tip of the direction of l. (Checked, for l the z axis; Confirmed)

5.6 A flector F transforms any object x as −F ⟇ x̂ ⟇ F̰, where x̂ is the grade involution. This equals the outermorphism of F's action on points: a reflected point keeps a positive weight, and a reflected line or plane is the one spanned by the reflected points in the same order. (Checked, for a plane and for a plane composed with a motor) See the first disagreement in section 8.

5.7 Constructors take geometric quantities: `rotation(l, theta)` turns by theta about a line l whose direction v has any positive length, and builds (l / |v|) sin(θ/2) + 𝟙 cos(θ/2), dividing all six coefficients of l by |v|; l must still be a line, v · m = 0. `translation(dx, dy, dz)` moves by d = (dx, dy, dz) and builds ½d on e23, e31, e12 plus 𝟙. Raw motor coefficients hold half-angles and half-distances. (Proposed, with both actions Checked from the coefficients they build for a unitized l)

5.8 A motor stores (Q_vx, Q_vy, Q_vz, Q_vw) on e41, e42, e43, 𝟙 and then (Q_mx, Q_my, Q_mz, Q_mw) on e23, e31, e12, **1**, and a flector stores (F_px, F_py, F_pz, F_pw) on e1, e2, e3, e4 and then (F_gx, F_gy, F_gz, F_gw) on e423, e431, e412, e321, with the coefficient names of Lengyel's Motor and Flector pages. The motor's blades are those of even grade and the flector's those of odd grade, so the antiproduct of two motors or of two flectors is a motor, and that of a motor and a flector, in either order, is a flector. (Confirmed, research note [0023](../research-notes/0023-motor-flector-point-groups-chains.md); closure under the antiproduct Checked)

## 6 Conformal objects in conformal(3)

Component: model (cga3).

6.1 The round point with center p and squared radius r² is p + e4 + ½(p² + r²)e5. Its square P • P is −r², so a round point of zero radius is null and an imaginary round has r² < 0. A round point is not the dual of the sphere with its center and radius: that sphere is its container, and the bulk dual of the real sphere with center p and radius r is −(p + e4 + ½(p² − r²)e5). (Confirmed; the value of P • P also Checked)

6.2 Flat objects are the rigid objects wedged with e5, with their coefficients unchanged. (Study)

6.3 A conformal object splits into four parts: the round bulk has neither e4 nor e5, the round weight has e4 only, the flat bulk e5 only, and the flat weight both. (Confirmed)

6.4 The operations on rounds and flats (carrier, cocarrier, center, container, partner, attitude, radius) are specified with the conformal model in roadmap phase 3. (Study)

## 7 Operators

7.1 Named functions are the interface. These operators are aliases of them in every algebra:

| Operator | Meaning | Named function |
| --- | --- | --- |
| `a * b` | Geometric product ⟑ | `geometric_product` |
| `a % b` | Geometric antiproduct ⟇ | `geometric_antiproduct` |
| `a ^ b` | Wedge ∧ | `wedge` |
| `a & b` | Antiwedge ∨ (meet) | `antiwedge` |
| `~a` | Reverse | `reverse` |
| `!a` | Right complement | `right_complement` |
| `a + b`, `a - b` | Sum and difference | |
| `-a` | Negation | |
| `s * a`, `a * s`, `a / s` | Scaling by a scalar s | |

The antireverse, left complement, duals, dot, antidot, projections, and transforms have no operator. `compose(Q2, Q1)` is Q2 ⟇ Q1, and `transform(x, V)` applies 4.4. Because `^` and `&` bind more loosely than `==`, `+`, and `*`, expressions that mix them with other operators are fully parenthesized. (Study, with `compose` and `transform` Checked)

7.2 With `<galoot/core/format.hpp>` included, `std::format("{}", x)` writes `{name: value, ...}`: every component of x, zero or not, in the order of x's layout, each as its blade's name, a colon and a space, and its coefficient as the scalar type's own formatter writes it; an empty layout writes `{}`. A blade is named as the algebra tag names it when the tag declares a name with the layout's orientation, and otherwise by its canonical name (1.1; the scalar is 1, 1.5), preceded by `-` when the layout stores it with orientation −1, as e41 = −e14 is written `-e14`. A format specification, as in `{:.3f}`, applies to each coefficient. (Proposed)

## 8 Recorded disagreements

| Topic | Disagreement | Choice | Test |
| --- | --- | --- | --- |
| Flector transform | The design study, Lengyel's Flector page, and his rigid poster give x′ = −F ⟇ x ⟇ F̰ at every grade. That form agrees with the outermorphism on lines, but on points and planes it gives the negation, so a reflected unitized point has weight −1. Lengyel's Reflection page writes F ⟇ x ⟇ F̰ for points, and the poster's flector matrix keeps the point weight positive; both agree with 5.6 on points. The Flector page's line row also has the sign of its moment formula reversed, so that row is not used as a reference value. | 5.6 | `check_versors`; "a reflected point keeps a positive weight" in `test/core/transform_test.cpp` and "the rga3 Flector e412 reflects (1, 2, 3, 1) to (1, 2, -3, 1) with weight +1" in `test/presets/rga3/objects_test.cpp`, each of which also checks that the single-sign form gives the weight −1; "the rga3 Flector e412 reflects the plane (0, 0, 1, -1) to (0, 0, 1, 1)" in `test/presets/rga3/objects_test.cpp`, which also checks that the single-sign form gives the negated plane |
| Dual sign | The design study reports that Terathon's `BulkDual` and `WeightDual` for points and planes return left duals, which negate a ∧ b★ = (a • b)𝟙 at odd grades in four dimensions; the wiki uses right duals, which note 0005 confirms. The Terathon side has not been tested. | 3.3 | `check_identities`; "the duals of a point are planes with the dual-sign convention" in `test/core/dual_test.cpp`, which also checks that the left duals give the negation |
| Partner of a circle | The design study suspects missing parentheses in Terathon's `Partner(Circle3D)` relative to the wiki formula. The wiki's explicit formula equals its general definition par(u) = (−1)^(gr+1) con(u☆) ∨ car(u) on genuine circles but not on other inputs of the same type, so a test must use genuine circles. The Terathon side has not been tested. | Decided in phase 3 | To be written |
| Conformal duals | The Duals table of the conformal wiki, in its round point, flat point, plane, and sphere rows, is the negation of the wiki's own definition u★ = right(G u). The conformal poster agrees with the definition. | 3.3, the definition; confirmed by the owner on 2026-11-09 ("Accept all recommendations", decision 0219 item 5) | "the bulk dual of a conformal sphere is a round point" in `test/core/dual_test.cpp`, which the Duals table's sphere row fails |

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
| 1.3 | 2026-10-31 | Ratified by the owner: component markers; non-diagonal blade definition in 1.1; 4.4 and 4.5 added (decision 0009) |
| 1.4 | 2026-10-31 | 4.1 defines the wedge as the exterior product, independent of the metric (decision 0023); ratified |
| 1.5 | 2026-11-01 | 4.1 states the defining relation of the geometric product; ratified (decision 0098) |
| 1.6 | 2026-11-01 | 4.5 also accepts the target's type, `truncate<Type>(x)` (decision 0155); 7.1 names the geometric product and antiproduct `geometric_product` and `geometric_antiproduct` (decision 0159); ratified by the owner: proceed as recommended |
| 1.7 | 2026-11-01 | 4.5: a conversion that drops blades verifies the dropped part is zero when the scalar is exact; the rounding-scalar check belongs to operations that drop part of their own result (decision 0161); ratified by the owner: proceed as recommended |
| 1.8 | 2026-11-02 | 4.5 names `part<Blades>(x)` and `part<Type>(x)`, which keep the same components as `truncate` and never check (decision 0168); 7.1 adds negation `-a` (decision 0167); ratified by the owner: agreed |
| 1.9 | 2026-11-04 | 3.2 states the factor by which the bulk and the weight multiply each blade they keep in a diagonal metric, and that with two or more null generators some blades are in neither; 4.5 says that what `part` drops may be nonzero, in place of "is meant to be nonzero" (decision 0174); ratified by the owner: take all recommendations |
| 1.10 | 2026-11-07 | 2.1 defines the antigrade involution; 4.3 defines the antiprojection, `antiproject(a, b)`, after a direct reading of the rigid wiki's Projections page (research note 0021); 4.4 uses the antireverse, the parity of the antigrades, and the odd sign −V ⟇ x̌ ⟇ V̰, states the transform's relation to the outermorphism, and restricts `compose` to versors of one parity; 4.5 says which rounding scalars are checked; 7.1 cites 4.4 and lists the projections; 3.2's diagonal case is now checked by `conventions_check.py` on several diagonal forms, with its label unchanged (decisions 0176, 0177, 0190, 0192); ratified by the owner: proceed with recommendations; 4.4's sentence on the outermorphism and 2.1's clauses after the definition of the antigrade involution confirmed by the owner: agree, proceed as recommended (decisions 0176 and 0194) |
| 1.11 | 2026-11-07 | 4.5 adds that a rounding scalar too coarse for the operation's error bound to hold, such as `bfloat16`, is not checked; the owner chose this amendment over a compile error (decision 0195) and confirmed the wording: accept all recommendations (decision 0196) |
| 1.12 | 2026-11-07 | 5.8 fixes the motor and flector component order, which equals Lengyel's Motor and Flector pages as read directly (research note 0023), and uses the pages' coefficient letters; 5.7 writes `translation(dx, dy, dz)` and labels both constructors' actions Checked; 5.2 adds the moment p × q for points of any weight (decision 0197); ratified by the owner: accept all recommendations (decision 0196); the exact wording of 5.8 was confirmed by the owner on 2026-11-08 (decision 0202) |
| 1.13 | 2026-11-08 | 5.7: `rotation(l, theta)` divides l by the length of its direction, so its direction may have any positive length, and states that l must be a line; its label says the actions are Checked for a unitized l (decision 0203); ratified by the owner: accept all recommendations (decision 0202 item 1); the exact wording confirmed by the owner: confirmed the recommendations (decision 0210) |
| 1.14 | 2026-11-08 | 7.2 states the printed form of a multivector, `{name: value, ...}` in the order of its layout, with the names the algebra tag declares, otherwise canonical names with `-` for orientation −1; the owner chose the form and the names (decision 0196 item 5); the wording confirmed by the owner: confirmed the recommendations (decision 0207); section 8's flector row names in its Test column the two C++ tests that tell the single-sign form apart, approved by the owner (decision 0207 item 6); 7.2 cites 1.5 beside 1.1 for the scalar's name 1, which 1.1 does not give (a wording edit from the WU15 hci reading, C7) |
| 1.15 | 2026-11-08 | Section 8's flector row names `test/presets/rga3/objects_test.cpp`, where the move of rga3 into the presets layer put the test, in place of `test/rga3/objects_test.cpp`; no convention changes; confirmed by the owner: continue as recommended (decision 0213) |
| 1.16 | 2026-11-08 | Section 8's "Dual sign" row names "the duals of a point are planes with the dual-sign convention" in `test/core/dual_test.cpp`, which fails under left duals, and the "Conformal duals" row names "the bulk dual of a conformal sphere is a round point" there, which fails under the Duals table's sphere row, as research note 0024 (D1, D2) shows; the "Flector transform" row adds "the rga3 Flector e412 reflects the plane (0, 0, 1, -1) to (0, 0, 1, 1)" in `test/presets/rga3/objects_test.cpp`, the plane hand value of decision 0211 item 5, which the single-sign form fails; no convention changes; ruled by the owner: accept and proceed (decision 0216 items 4 and 5) |
| 1.17 | 2026-11-09 | 1.4 names the generators of R(d+1, 1, 0) e′1 to e′(d+2), states the blade map and that 𝟙 maps to −𝟙′, and is confirmed against the conformal wiki's Main Page (research note 0026); 1.8 states what a change of basis preserves and with which factor; section 8's conformal-duals row records the owner's confirmation (decision 0219 items 4 and 5); ratified by the owner: accept all recommendations, committed at the unit's merge after the Sage cross-check (decisions 0219 item 4 and 0223 item 9) |
