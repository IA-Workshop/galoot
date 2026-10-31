# 0005 Lengyel's sources against CONVENTIONS.md

Sources:

- Rigid wiki, https://rigidgeometricalgebra.org/wiki/index.php?title=PAGE, pages Flector (last edited 2026-07-19), Reflection (last edited 2025-04-15), Motor, Complements, Duals, Bulk_and_weight, Reverses, Antiscalars.
- Conformal wiki, https://conformalgeometricalgebra.org/wiki/index.php?title=PAGE, pages Metrics, Round_point, Dot_products, Duals, Sphere, Circle, Carriers, Containers, Partners, Join_and_meet.
- Lengyel's reference posters, https://projectivegeometricalgebra.org/projgeomalg.pdf (created 2024-03-14) and https://projectivegeometricalgebra.org/confgeomalg.pdf (file dated 2025-10-21). Both are marked copyright 2024 on the face.

Read by: an agent (Claude), 2026-10-31. Not yet reviewed by the owner.

How the sources were read. The wiki was fetched as HTML that a summarizing model converted to text. Every formula used below was requested as LaTeX source. The two pages with discrepancies (Flector, conformal Duals) were also fetched as raw wikitext. The posters were rendered at 250 dpi and read from the images. The basis tables on the rigid Complements, Duals, and Reverses pages are SVG files whose glyphs are paths, so they were not read. The Terathon source was not read, because no wiki page was ambiguous on the points checked (C16).

Owner's direction: the wiki is the authority for conventions. Where the wiki and the design study disagree, both are reported for a recorded disagreement.

Evidence labels:

- Read: stated in the source at the cited place.
- Recomputed: checked by exact rational arithmetic in scripts written for this note: `ga.py`, `lengyel_check.py` (106 checks, all pass), and `poster_flector.py`. They live in this session's scratchpad (`scratchpad/checks/`), not in the repository. They implement the definitions from scratch, including a geometric product for the non-diagonal conformal metric.
- Inference: my conclusion, not stated in the source.

## Summary

1. Lengyel states the flector transform with a single sign, −F ⟇ x ⟇ F̰, for every grade. The Flector page and the PGA poster both say so. The section 8 analysis in CONVENTIONS.md is confirmed: this form negates points and planes and agrees with the outermorphism on lines. (A1, A2)
2. Lengyel's own sources do not agree on that sign. The Reflection page writes F ⟇ x ⟇ F̰ with no minus sign. The poster's flector matrix keeps the point weight positive. Both match Galoot 5.6 on points. (A3, A4)
3. The Flector page's line row has a sign error in its moment formula. Do not use it as a reference value. (A5)
4. Right-complement duals are confirmed for the rigid algebra by the wiki and the poster. Left duals exist only under their own name, as 3.3 says. (B1 to B6)
5. The round point and P • P = −r² are confirmed. A round point is tied to its sphere by the container, not by the dual. (C2, C3)
6. New disagreement: the conformal wiki's Duals table is the negation of its own definition u★ = right(G u). The CGA poster matches the definition. (C5)
7. The wiki's explicit Partner(circle) formula agrees exactly with the general definition on genuine circles. The Terathon half of that disagreement is still untested. (E2)

## A. Flector transform (5.6, first disagreement)

A1. Read: the Flector page says "An element x is transformed by a flector F through the operation x′ = −F ⟇ x ⟇ F̰." No grade-dependent sign appears. Its point row gives q′_w = −q_w. Source: Flector page, section "Flector Transformations", and its raw wikitext. Recomputed: the point and plane rows equal −F ⟇ x ⟇ F̰ for six flectors (three planes, and each composed with a motor).

A2. Read: the PGA poster's Flector box says "−F ⟇ u ⟇ F̰ rotates object u through angle 2φ … and reflects across plane g", "−g ⟇ u ⟇ g̰ reflects object u across plane g", and "−p ⟇ u ⟇ p̰ inverts object u through point p". One sign covers every object.

A3. Read: the Reflection page writes the sandwich as F ⟇ p ⟇ F̰ with no minus sign. Its point row ends in "+ p_w e4". Recomputed: its point, line, and plane rows equal +F ⟇ x ⟇ F̰ exactly. So the two wiki pages differ by an overall sign. Galoot 5.6 equals the Reflection page on points and planes and is its negation on lines.

A4. Read: the poster's matrix conversion M_F = A_F + B_F has bottom row (0, 0, 0, 1). Recomputed: M_F applied to points equals Galoot 5.6 on four flectors and three points each. It does not equal −F ⟇ p ⟇ F̰.

A5. Recomputed: on the Flector page's line row, the direction formula k′_v matches −F ⟇ l ⟇ F̰. The moment formula k′_m = 2[F_gw a + F_pw(c − b) + g × (c − b) + p × a] − k_m does not. The exact moment is the negation of that formula, k_m − 2[…]. This holds for all six flectors, including a pure reflection in x = 0, where the formula maps the moment (m_x, m_y, m_z) to (−m_x, m_y, m_z). The true image is (m_x, −m_y, −m_z). Inference: a typo on the wiki.

Conclusion. The single-sign form is Lengyel's stated sandwich. CONVENTIONS 5.6 departs from it at odd grades, by design. Both forms give the same projective objects. They differ only in the sign of the weight, so the departure changes orientation, not geometry. Lengyel's own point action (the poster matrix, the Reflection page) agrees with 5.6.

## B. Complements and duals (3.1 to 3.4, second disagreement)

B1. Read: u ∧ ū = 𝟙, u̲ ∧ u = 𝟙, and u̲ = (−1)^(gr(u) ag(u)) ū. Source: Complements page; PGA poster, Unary Operations table. The 16-entry table is an image and was not read. CONVENTIONS 3.1 is already Checked.

B2. Read: u★ = right complement of G u, u☆ = right complement of 𝔾 u, labeled right duals. Left duals use the left complement and are written with subscript stars. Source: Duals page; poster Unary Operations table, which lists "Right bulk dual", "Right weight dual", "Left bulk dual", and "Left weight dual" separately.

B3. Read and recomputed: the Duals page's geometry table gives p★ = p_x e423 + p_y e431 + p_z e412, p☆ = p_w e321, g★ = −g_w e4, g☆ = −g_x e1 − g_y e2 − g_z e3, and lines with leading minus signs. All six rows equal Galoot's right duals exactly. The left duals negate the point and plane rows.

B4. Read: a ∧ b★ = (a • b)𝟙 when a and b have the same grade (Duals page). The poster gives a ∨ b★ = a • b and a ∧ b☆ = a ∘ b. Recomputed: the wiki identities u★ = ũ ⟑ 𝟙 and u☆ = ṵ ⟇ **1** hold with the right complement and fail with the left.

B5. Read: u★★ = u☆☆ = (−1)^(gr ag) det(𝔤) u, so a double dual is zero in R(3,0,1) (Duals page). This confirms 3.4. Recomputed: in conformal(3), where det 𝔤 = −1, u★★ = −u.

B6. Read: the bulk u● = G u holds the components without e4, and the weight u○ = 𝔾 u holds those with it (Bulk and weight page). The poster's Metric box gives 𝔾u = left(G right(u)) = right(G left(u)) and G𝔾 = det(𝔤) I. This confirms both parts of 3.2.

The wiki half of the second disagreement is confirmed. The Terathon half (that `BulkDual` and `WeightDual` return left duals) was not checked.

## C. Conformal model (1.4, 6.1, 6.2)

C1. Read: the Metrics page gives the 5×5 metric with 1, 1, 1 on the diagonal, 0 at (4,4) and (5,5), and −1 at (4,5) and (5,4). It says the antimetric 𝔾 is the negation of G. Recomputed: left(G right(u)) = −G u for all 32 blades. The pages read do not state the relation to e₊ and e₋, so 1.4 stays Checked on the mathematics alone.

C2. Read: a = p_x e1 + p_y e2 + p_z e3 + e4 + ½(p² + r²) e5 (Round point page; CGA poster). The poster's legend labels r² > 0 real and r² < 0 imaginary. The Dot products page gives a1 • a2 = −½(v² + r1² + r2²), so a • a = −r². The radius norm is √((2 a_w a_u − a_x² − a_y² − a_z²)/a_w²). Recomputed: all three. CONVENTIONS 6.1 is confirmed.

C3. Read: "The container of a round point a is the sphere having the same center and radius as a" (Round point page). The Sphere page gives s = −e1234 + p_x e4235 + p_y e4315 + p_z e4125 − ½(p² − r²) e3215. Recomputed: con(a) = s for a unitized round point. The bulk dual of that real sphere is −(p + e4 + ½(p² − r²) e5), a round point with r² negated. This is the observation behind recommendation 5 of note 0003. It agrees with the wiki and is not a disagreement. Lengyel relates a round point to its sphere through the container. The phase 3 specification should say so.

C4. Read: the CGA poster gives the four-part partition for each object. For a round point: round bulk a_x e1 + a_y e2 + a_z e3, round weight a_w e4, flat bulk a_u e5, flat weight 0. For a sphere: round bulk 0, round weight s_u e1234, flat bulk s_w e3215, flat weight s_x e4235 + s_y e4315 + s_z e4125. This matches the design study's masks: neither e4 nor e5, e4 only, e5 only, both. The partition in 6.2 is confirmed. The operation definitions are confirmed in E1.

C5. New disagreement. Read (raw wikitext): the conformal Duals page defines u★ = right(G u) and u☆ = right(𝔾 u). Its table gives a★ = a_w e1234 − a_x e4235 − a_y e4315 − a_z e4125 + a_u e3215 and s★ = s_x e1 + s_y e2 + s_z e3 − s_u e4 − s_w e5. Its flat point and plane rows follow the same pattern. Recomputed: all four rows equal −right(G u), not right(G u). The complement orientation is not the cause: the wiki's meet formulas for s ∨ t and s ∨ g agree exactly with Galoot's antiwedge. Read: the CGA poster gives a★ = −a_w e1234 + a_x e4235 + a_y e4315 + a_z e4125 − a_u e3215 and s★ = −s_x e1 − s_y e2 − s_z e3 + s_u e4 + s_w e5. Recomputed: the poster's dual rows for round point, flat point, flat line, plane, and sphere equal right(G u). The wiki's explicit container, cocarrier, center, and partner formulas all agree with u☆ = right(𝔾 u). Inference: the wiki table has the opposite sign, perhaps computed with 𝔾. The design study reports that Terathon's `Dual(RoundPoint3D)` is negated. That may be this same sign, but it was not checked. In five dimensions the left and right complements coincide, so this question is G against 𝔾, not left against right.

## D. Grade operations and the antiscalar (1.5, 2.1)

D1. Read: ũ = (−1)^(gr(gr−1)/2) u, the antireverse uses the antigrade, and the antireverse equals (−1)^(gr ag) (−1)^(n(n−1)/2) times the reverse. Source: Reverses page; poster Unary Operations table. Recomputed for n = 4 and n = 5.

D2. Read: the poster's basis table gives 𝟙 = e1 ∧ e2 ∧ e3 ∧ e4. The Antiscalars page calls it the wedge product of all basis vectors and the identity of the geometric antiproduct. Recomputed: e12…n is that identity for n = 4 and n = 5.

D3. The grade involution appears on none of the pages read. It is Galoot's addition for 5.6.

## E. Partner (6.2, third disagreement)

E1. Read: par(u) = (−1)^(gr(u)+1) con(u☆) ∨ car(u), with u • par(u) = 0 (Partners page). con(u) = u ∧ car(u)☆ (Containers page), car(u) = u ∧ e5, and ccr(u) = u☆ ∧ e5 (Carriers page).

E2. Recomputed: the wiki's explicit Partner(circle) formula equals the general definition for a circle a ∧ b ∧ c of three round points with random radii and weights, and c • par(c) = 0. The explicit formulas for the round point, dipole, and sphere also match, as do the explicit container and carrier of a circle. For a 10-component element that is not a circle, the explicit formula and the general definition differ, and c • par(c) ≠ 0. Inference: the explicit formula uses the constraints that a circle satisfies. Two implementations can therefore agree on every circle and differ on other inputs, and a regrouping that looks like missing parentheses may be an equivalent form. A comparison with Terathon must use genuine circles.

## F. Rigid objects and products (1.3, 1.6, 4.1, 5.1 to 5.5)

F1. Read: the poster's metric is diag(1, 1, 1, 0), with e4 = e_n. Its basis table defines e41 = e4 ∧ e1, e31 = e3 ∧ e1, e423 = e4 ∧ e2 ∧ e3, and e321 = e3 ∧ e2 ∧ e1. Points, lines, and planes have the coordinates of 5.1 to 5.3 (Bulk and weight page, Reflection page, poster). This confirms 1.3, the names in 1.6, and 5.1.

F2. Read: the Motor page gives x′ = Q ⟇ x ⟇ Q̰, the point row p′_w = p_w, R = l sin φ + 𝟙 cos φ for "a rotation by twice the angle φ", and T = t_x e23 + t_y e31 + t_z e12 + 𝟙 for "a translation by twice the displacement vector". This confirms 5.4 and 5.5. The page gives no general composition rule beyond the factorization Q = T ⟇ R.

F3. Read: the poster gives a • b = (aᵀGb)**1**, a ∘ b = (aᵀ𝔾b)𝟙, and the De Morgan laws with the right complement. This confirms 4.1.

## Open questions

1. Should section 8 record the Reflection page and the poster matrix as Lengyel sources that agree with 5.6 on points? They strengthen the choice, but Lengyel's stated sandwich is still the single-sign form.
2. For the conformal dual sign (C5), adopt right(G u), which agrees with the definition, the poster, and a ∧ b★ = (a • b)𝟙? The wiki table would then be the recorded alternative.
3. The Terathon halves of the second and third disagreements remain open. Phase 3 can settle them by black-box comparison of outputs (V1, kind 4).
4. The basis tables on the rigid Complements, Duals, and Reverses pages are images and were not read.
5. The A5 and C5 findings should get a human look at the live pages before anyone reports them to Lengyel. The wiki's Errata page was not checked.
6. The scratch scripts could become part of `test/reference/` if the owner wants these checks kept.
