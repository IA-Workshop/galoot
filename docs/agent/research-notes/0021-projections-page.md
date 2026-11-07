# 0021 The rigid wiki's Projections page, read directly

Source:

- Rigid wiki, page Projections, raw wikitext: https://rigidgeometricalgebra.org/wiki/index.php?title=Projections&action=raw
- Revision read: revid 352 (parent 227), saved 2024-04-13T23:28:41Z by the user "Eric Lengyel", 7041 bytes, MediaWiki sha1 562bcc5ce564d2722fead980bc883546839d3bac, from https://rigidgeometricalgebra.org/wiki/api.php?action=query&prop=revisions&titles=Projections&rvprop=ids|timestamp|user|size|sha1&format=json. The server's Last-Modified header gave the same time. The 7041 bytes downloaded have SHA-256 b2c6dd0ba9f70a84c4416726c81c6291553c5062a2ffc130313a5129ecabaaf3.

Read by: Claude (agent, Opus 5.5, math-expert role), 2026-11-07, for WU13a. Not yet reviewed by the owner.

How the source was read. Both URLs were fetched with curl on 2026-11-07 at 11:32 UTC, with a generic User-Agent and no other identifying header, into a scratch directory outside the repository. The raw wikitext was then read in full, line by line, by the agent. No summarizing model stood between the page and the reader, so this is not the substitute reading of plan 0021 departure 6. Every formula below is transcribed from the page's LaTeX source. The page's five illustrations (SVG files) were not read. The page cites "Section 2.13.6" of Lengyel's book, which was not read. The page itself is not committed: only the short passages below are quoted, each marked as a quotation. No passage of the page addresses its reader with instructions.

Evidence labels, as in note 0005:

- Read: stated in the source at the cited place.
- Recomputed: checked exactly from Galoot's definitions by a committed script. `records/spikes/transform-parity/check.py` checks every formula as a polynomial identity in the coordinates in rigid(3), which proves it for all inputs (its "projections page" group). `test/reference/conventions_check.py` checks the antiprojection rows and the facts proposed for 4.3 on random rational inputs (`check_antiprojection`).
- Inference: my conclusion, not stated in the source.

## Summary

1. The page defines the antiprojection of a onto b as b ∧ (a ∨ b☆), the definition the plan proposed for 4.3, read from the page's own text. The object comes first, as in `antiproject(a, b)`. (A1)
2. All three antiprojection formulas in its rigid(3) table hold exactly with Galoot's wedge, antiwedge, and right weight dual. (A2 to A4)
3. The projection b ∨ (a ∧ b☆) of CONVENTIONS 4.3 is confirmed from the page itself, and its three table rows and the two origin rows hold exactly. (P1 to P6)
4. Erratum on the page: the row for the antiprojection of the horizon onto a point, which uses the bulk dual, is the negation of what its own formula gives. It equals the formula computed with the left bulk dual. The line row of the same table agrees. No Galoot convention uses that row. (H1, H2)
5. The earlier summary-based reading in plan 0021 departure 6 matches the page. (S1)
6. The page supports the 4.3 sentence proposed in plan 0021 O2 unchanged. (S2)

## Notation

The page writes ∧ (wedge), ∨ (antiwedge), and b^☆, coded as `\unicode["segoe ui symbol"]{x2606}`, U+2606 WHITE STAR. Its last table uses U+2605 BLACK STAR. In Lengyel's notation these are the weight dual and the bulk dual (note 0005, B2: the right duals u☆ = right(𝔾 u) and u★ = right(G u)). Galoot's notation is the same (CONVENTIONS 3.3), so the formulas carry over symbol for symbol. The page's coordinates are Galoot's (CONVENTIONS 5.1 to 5.3):

- point p = p_x e1 + p_y e2 + p_z e3 + p_w e4;
- line l = l_vx e41 + l_vy e42 + l_vz e43 + l_mx e23 + l_my e31 + l_mz e12, with direction l_v and moment l_m;
- plane g = g_x e423 + g_y e431 + g_z e412 + g_w e321.

The blade names are the oriented names of CONVENTIONS 1.6. Below, l_v × l_m is the cross product, and l_v · p = l_vx p_x + l_vy p_y + l_vz p_z, written out on the page.

## A. Antiprojection

A1. Read: "The orthogonal antiprojection of an object **a** onto an object **b** is given by the general formula **b** ∧ (**a** ∨ **b**☆) , where the operation in parentheses is the weight contraction of **a** with **b**." The table is headed "Antiprojections involving points, lines, and planes in the 4D rigid geometric algebra 𝒢₃,₀,₁". The page also says: "The formulas on this page are general and do not require the geometric objects to be unitized." So "a onto b" puts the object first, and b, the object onto which a is antiprojected, carries the dual. This is Galoot's `antiproject(a, b)` = `wedge(b, antiwedge(a, weight_dual(b)))`.

A2. Read, the row "Antiprojection of plane g onto point p":

    p ∧ (g ∨ p☆) = p_w² (g_x e423 + g_y e431 + g_z e412) − (g_x p_x + g_y p_y + g_z p_z) p_w e321

Recomputed: exact. With p unitized (p_w = 1), the result has the normal (g_x, g_y, g_z) of g and the offset −(g_x p_x + g_y p_y + g_z p_z). Recomputed: p ∨ result = 0, so it contains p. Inference: it is the plane through p parallel to g with the same normal, not only a parallel one. Recomputed: antiprojecting the result onto p again multiplies it by p_w², so it is unchanged when p is unitized.

A3. Read, the row "Antiprojection of line l onto point p":

    p ∧ (l ∨ p☆) = p_w² (l_vx e41 + l_vy e42 + l_vz e43)
                 + (p_y l_vz − p_z l_vy) p_w e23 + (p_z l_vx − p_x l_vz) p_w e31 + (p_x l_vy − p_y l_vx) p_w e12

Recomputed: exact. With p_w = 1 the direction is l_v and the moment is p × l_v. Recomputed: p ∧ result = 0, so it contains p. Inference: it is the line through p with the direction of l. By CONVENTIONS 5.2, the line through unitized points p and q has direction q − p and moment p × q = p × (q − p), which is the moment of a line through p with that direction.

A4. Read, the row "Antiprojection of plane g onto line l":

    l ∧ (g ∨ l☆) = (l_vx² + l_vy² + l_vz²)(g_x e423 + g_y e431 + g_z e412)
                 − (g_x l_vx + g_y l_vy + g_z l_vz)(l_vx e423 + l_vy e431 + l_vz e412)
                 + (g_x l_my l_vz − g_x l_mz l_vy + g_y l_mz l_vx − g_y l_mx l_vz + g_z l_mx l_vy − g_z l_my l_vx) e321

Recomputed: exact, for any six line coordinates. Recomputed: for a line p ∧ q of two symbolic points, l ∨ result = 0, so the result contains the line. Inference: its normal is |l_v|² g_n − (g_n · l_v) l_v, the part of g's normal orthogonal to l_v. So the result is the plane through l that is closest to parallel to g. The page gives only the formula and the illustration's caption.

## P. Projection

P1. Read: "The orthogonal projection of an object **a** onto an object **b** is given by the general formula **b** ∨ (**a** ∧ **b**☆) , where the operation in parentheses is the weight expansion of **a** into **b**." This confirms the definition in CONVENTIONS 4.3 from the page itself. Note 0005 did not read this page.

P2. Read, "Projection of point p onto plane g": g ∨ (p ∧ g☆) = (g_x² + g_y² + g_z²) p − (g_x p_x + g_y p_y + g_z p_z + g_w p_w)(g_x e1 + g_y e2 + g_z e3). Recomputed: exact. Its weight is (g_x² + g_y² + g_z²) p_w, which is +1 for a unitized point and plane, as 4.3 states.

P3. Read, "Projection of point p onto line l": l ∨ (p ∧ l☆) = (l_v · p)(l_vx e1 + l_vy e2 + l_vz e3) + (l_vy l_mz − l_vz l_my) p_w e1 + (l_vz l_mx − l_vx l_mz) p_w e2 + (l_vx l_my − l_vy l_mx) p_w e3 + |l_v|² p_w e4. Recomputed: exact.

P4. Read, "Projection of line l onto plane g": g ∨ (l ∧ g☆) = |g_n|² (l_v on e41, e42, e43) − (g_n · l_v)(g_n on e41, e42, e43) + (g_n · l_m)(g_x e23 + g_y e31 + g_z e12) − (g_y l_vz − g_z l_vy) g_w e23 − (g_z l_vx − g_x l_vz) g_w e31 − (g_x l_vy − g_y l_vx) g_w e12, where g_n = (g_x, g_y, g_z). Recomputed: exact.

P5. Read, "Projection of Origin", plane row: g ∨ (e4 ∧ g☆) = −g_x g_w e1 − g_y g_w e2 − g_z g_w e3 + (g_x² + g_y² + g_z²) e4. Recomputed: exact. It is P2 with p = e4.

P6. Read, line row: l ∨ (e4 ∧ l☆) = (l_vy l_mz − l_vz l_my) e1 + (l_vz l_mx − l_vx l_mz) e2 + (l_vx l_my − l_vy l_mx) e3 + |l_v|² e4. Recomputed: exact. It is P3 with p = e4.

## H. Antiprojection of the horizon, with the bulk dual

The page introduces this table with "the horizon **g** = **e**₃₂₁ (the plane at infinity) can be antiprojected onto a point or line using the bulk contraction". Its formulas use p★ and l★, the bulk dual, so they are not the antiprojection of A1. Galoot's 4.3 does not use them.

H1. Read, the point row: p ∧ (e321 ∨ p★) = p_x p_w e423 + p_y p_w e431 + p_z p_w e412 − (p_x² + p_y² + p_z²) e321. Recomputed: with Galoot's right bulk dual, p★ = p_x e423 + p_y e431 + p_z e412 (the Duals page row of note 0005, B3), the left side is −p_x p_w e423 − p_y p_w e431 − p_z p_w e412 + (p_x² + p_y² + p_z²) e321, the negation of the page's right side. With the left bulk dual left(G p) = −p★ (CONVENTIONS 3.3: the two differ by (−1)^(gr·ag), which is −1 for a vector in four dimensions), the left side equals the page's right side exactly. Both sides describe the same plane, p · q = |p|² for p_w = 1, with opposite orientations.

H2. Read, the line row: l ∧ (e321 ∨ l★) = (l_my l_vz − l_mz l_vy) e423 + (l_mz l_vx − l_mx l_vz) e431 + (l_mx l_vy − l_my l_vx) e412 + (l_mx² + l_my² + l_mz²) e321. Recomputed: exact. For a line (grade 2 in four dimensions) the left and right duals are equal, so this row cannot tell them apart.

Inference: H1 is a sign error on the page, or it was computed with a left bulk dual. The second reading matches the pattern of the "Dual sign" row of CONVENTIONS section 8, where the design study reports that Terathon's duals of points and planes are left duals. That is a guess about the page's origin, not a finding. The row is not used as a reference value. It bears on no Galoot convention, since 4.3 uses only the weight dual. The wiki's Errata page was not checked.

## S. Against plan 0021 and CONVENTIONS 4.3

S1. Plan 0021 departure 6 (the planner's `antiproject.py`, from a summarizing fetch) reads the page as giving antiprojection = b ∧ (a ∨ b☆), with formulas for a plane onto a point, a line onto a point, and a plane onto a line, and the projection of a point onto a plane. The direct reading agrees on every point. The definition is the page's own sentence (A1). The table has exactly those three antiprojection rows, under those names (A2 to A4). The projection row exists (P2). Recomputing each formula from the definitions agrees. The summary reading did not mention the horizon table (H) or its sign error, and that table does not bear on 4.3.

S2. The 4.3 sentence the page supports, with its label:

> 4.3 The projection of a onto b is b ∨ (a ∧ b☆), and the antiprojection of a onto b is b ∧ (a ∨ b☆), written `project(a, b)` and `antiproject(a, b)` with the object first. For a point projected onto a unitized plane, the result has weight +1. The antiprojection of a plane onto a unitized point is the plane through the point with the same normal, and that of a line onto a unitized point is the line through the point with the same direction. (Checked; Confirmed)

This is the text of plan 0021 O2, unchanged. "Confirmed" rests on A1 and P1 for the two definitions, on P2 for the weight, and on A2 and A3 for the two facts. For the facts, the page's formulas were read and recomputed. The words "the plane through the point with the same normal" and "the line through the point with the same direction" are my reading of those formulas at p_w = 1 (A2, A3, Inference). The page does not say them. "Checked" rests on `conventions_check.py`'s `check_objects` (the weight) and `check_antiprojection` (both facts, containment, repetition, and the page's three general rows on random rationals).

## Open questions

1. Should H1 be recorded with the Dual sign row of CONVENTIONS section 8, or only here? It is a second place where a Lengyel source and the right dual differ by the left-dual sign for a vector.
2. Plan 0021 R7 and A4 call the plane-onto-line result by its formula only. If a test or a comment describes it in words, A4's inference ("the plane through l closest to parallel to g") is mine and should be checked by whoever writes it.
3. Section 2.13.6 of Lengyel's book, which the page cites, was not read.
