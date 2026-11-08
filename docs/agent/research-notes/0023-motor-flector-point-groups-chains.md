# 0023 Motor and Flector pages, point-group orders, and the joint-chain form

Read by: Claude (agent, Opus 5.5, math-expert role), 2026-11-07, for WU16 (plan 0023, step 1). Not yet reviewed by the owner. The plan calls this note 0022; that number was taken by the flat-plane chain rounding note in the meantime, so this is 0023 (plan 0023, section 1, "Record numbers").

## Sources

| Source | Revision | Read? |
| --- | --- | --- |
| Rigid wiki, page Motor, raw wikitext, https://rigidgeometricalgebra.org/wiki/index.php?title=Motor&action=raw | revid 494 (parent 397), saved 2026-07-19T04:31:45Z by the user "Eric Lengyel", 19432 bytes, MediaWiki sha1 9932ba30e5860029316e34c639edb37040d221f5; SHA-256 of the bytes read 568e69179a5aa6a7782b54fa9552407caa6318c3d74b144a5296cf6c57657e07 | yes, in full |
| Rigid wiki, page Flector, raw wikitext, same form of URL | revid 495 (parent 398), saved 2026-07-19T04:32:41Z by "Eric Lengyel", 10281 bytes, sha1 7565b5280b6fbf97a1d17cdc1cb54b25deafe3cc; SHA-256 e4422f44988ef2b7d326a95f69be7f8fa1700b5ee830c83cc79462bf080a2fe6 | yes, in full |
| International Tables for Crystallography, volume A (it.iucr.org) | none | no: the site answered with HTTP 403 and a JavaScript challenge page |
| Bilbao Crystallographic Server, point-group pages (cryst.ehu.es), tried as a second primary-quality table | none | no: TLS hostname mismatch on `www.`, and an HTTP/0.9 reply on the bare host |
| Wikipedia, "Crystallographic point group", raw wikitext, https://en.wikipedia.org/w/index.php?title=Crystallographic_point_group&action=raw | revid 1378079624, 2026-10-02T21:23:00Z, 35914 bytes, sha1 0b5feed31d4df6af7e9941dce9512675ab37424c | yes: the table of the 32 classes in section "In three dimensions"; a substitute (below) |
| J. Denavit and R. S. Hartenberg, "A kinematic notation for lower-pair mechanisms based on matrices", J. Appl. Mech. 22 (1955) 215–221, doi 10.1115/1.4011045 | none | no: the DOI resolves to the ASME site, which answered with HTTP 403 and a JavaScript challenge page |
| Wikipedia, "Denavit–Hartenberg parameters", raw wikitext | revid 1380662527, 2026-10-17T03:34:52Z, 22166 bytes, sha1 d819309b95f34cbc9434898c0ab8e4f77e75f71b | yes: sections "Denavit–Hartenberg convention" and "Denavit–Hartenberg matrix"; a secondary account, used only as below |

How the sources were read. Each URL was fetched once with curl on 2026-11-07 at 20:15 UTC, with a generic browser User-Agent and no other identifying header (no e-mail address, no From header, no cookie), into a scratch directory outside the repository. The revision metadata came from each wiki's `api.php` (`prop=revisions`, `rvprop=ids|timestamp|user|size|sha1`); for all four pages read, the SHA-1 of the bytes downloaded equals the revision's sha1, so the text read is that revision. The wikitext was read in full by the agent, line by line, with no summarizing model in between. The pages' illustrations (SVG and PNG files) and the sections of Lengyel's book that the pages cite (3.6 and 3.7) were not read. The pages are not committed; only short passages are quoted below, marked as quotations. No passage of any page addresses its reader with instructions.

Evidence labels, as in notes 0005 and 0021:

- Read: stated in the source at the cited place.
- Recomputed: checked exactly from Galoot's definitions by a committed script. `test/reference/conventions_check.py` (seed 20261031) checks the formulas on random rationals with the geometric-algebra route; `test/reference/golden.py` compares that route with `test/reference/rigid_motions.py` (linear algebra over fractions, seed 20261107) and stops if they differ. Revision: `feature/rga3--math` at the commit that adds this note; commands `python3 -I test/reference/conventions_check.py` and `python3 -I test/reference/golden.py --check test/reference/golden.hpp`.
- Inference: my conclusion, not stated in the source.

## Summary

1. The Motor page stores a motor as Q_vx e41 + Q_vy e42 + Q_vz e43 + Q_vw 𝟙 + Q_mx e23 + Q_my e31 + Q_mz e12 + Q_mw 1, and the Flector page a flector as F_px e1 + F_py e2 + F_pz e3 + F_pw e4 + F_gx e423 + F_gy e431 + F_gz e412 + F_gw e321. These are exactly the layouts of plan 0023 departure 4 and O5 (`e41 e42 e43 e1234 e23 e31 e12 1` and `e1 e2 e3 e4 e423 e431 e412 e321`). No row is needed in CONVENTIONS section 8. (M1, F1)
2. The constructors read agree with CONVENTIONS 5.5 and 5.7: the translator t_x e23 + t_y e31 + t_z e12 + 𝟙 moves by 2t, and the rotor l sin φ + 𝟙 cos φ turns by 2φ about l. The three "motors built from geometry" (two planes, two lines, two points) and the Motor page's transformation rows, factorization, and matrix all recompute exactly. (M2 to M6)
3. The Flector page is the revision note 0005 read (saved 2026-07-19), and its three known problems are still there: the single-sign transform −F ⟇ x ⟇ F̰, the point row with q′_w = −q_w, and the sign of the line row's moment. They are the existing first row of CONVENTIONS section 8, so nothing new is recorded. Its matrix, factorization, and "e4 is the inversion" recompute exactly with 5.6. (F2 to F5)
4. The International Tables could not be read. The substitute, Wikipedia's table of the 32 classes, gives orders that equal the closures computed both ways, and its abstract-group and point-symmetry columns equal the abelian, cyclic, centrosymmetric, enantiomorphic, and polar properties of the closures. (G1 to G4)
5. Denavit and Hartenberg's paper could not be read. The chain is the product Rz(θ) Tz(d) Tx(a) Rx(α), with no claim that it follows their paper. A secondary account gives the same product and its closed form, which recomputes. (J1, J2)
6. Every planning probe of plan 0023 departure 9 that concerns the mathematics is now a committed check, and each one passes. (P1 to P3)

## M. The Motor page (revid 494)

M1. Read: "In the 4D rigid geometric algebra 𝒢₃,₀,₁, a motor **Q** has the general form **Q** = Q_vx **e**41 + Q_vy **e**42 + Q_vz **e**43 + Q_vw 𝟙 + Q_mx **e**23 + Q_my **e**31 + Q_mz **e**12 + Q_mw **1**." (transcribed from the LaTeX; 𝟙 is `\unicode{x1d7d9}`). This is the order of departure 4's `motor_layout`. The page goes on to say: "the components of **Q** must satisfy the equation Q_vx Q_mx + Q_vy Q_my + Q_vz Q_mz + Q_vw Q_mw = 0". Recomputed: the motor layout is exactly the blades of even grade, and the flector layout those of odd grade. So the antiproduct of two motors or two flectors stays on the motor layout, and that of a motor and a flector, in either order, stays on the flector layout. This holds for any coefficients, not only for versors (`check_motor_flector_layouts`, 50 random pairs of each kind). Recomputed: the constraint holds for products of two random planes (`check_motor_and_flector_pages`).

M2. Read: "An element **x** is transformed by a motor **Q** through the operation **x**′ = **Q** ⟇ **x** ⟇ Q̰". This agrees with CONVENTIONS 5.4, as note 0005 F2 found.

M3. Read, "Simple Motors": "**T** = t_x **e**23 + t_y **e**31 + t_z **e**12 + 𝟙 , and this performs a translation by twice the displacement vector (t_x, t_y, t_z)", and "**R** = **l** sin φ + 𝟙 cos φ , and this performs a rotation by twice the angle φ about the line **l**". Recomputed: `check_objects` (existing), `check_translation`, and `check_rotation`. The direction of the turn (counterclockwise seen from the tip of the line's direction) is not stated on the page. CONVENTIONS 5.5 fixes it, and `check_rotation` checks it against the right-handed matrices for five rational half angles about x, y, and z and about a line through (1, 2, 0). `golden.py`'s `rigid_rotations` checks the same against Rodrigues' formula for seven lines.

M4. Read, "Motors Built from Geometry": h ⟇ g̰, l ⟇ k̰, and q ⟇ p̰, each with its coefficients. Recomputed: all three rows hold exactly. The page writes the two planes of the first row on **e**234, **e**314, **e**124, **e**321. Inference: these are the blades of CONVENTIONS 1.6's e423, e431, e412, each obtained by an even permutation, so the coefficients are the same. The recomputation used e423, e431, e412 and agrees, which supports this.

M5. Read, "Motor Transformations": the point, line, and plane rows, for a unitized motor, with **v** = (Q_vx, Q_vy, Q_vz) and **m** = (Q_mx, Q_my, Q_mz). Recomputed: all three rows hold exactly for unitized motors built from a translator and two rational rotors.

M6. Read, "Factorization" (**T** = **Q** ⟇ R̰ with its coefficients) and "Conversion from Motor to Matrix" (**M** = **A** + **B**). Recomputed: both hold exactly. The matrix acts on points as columns and keeps the weight. In a scratch run, the "Conversion from Matrix to Motor" formulas for Q_m also held on the same motors. Their branches with square roots were not checked. The norm, exponential, logarithm, and square-root sections were not recomputed; none bears on WU16 (plan 0001 section 3 leaves norms, `exp`, and `log` out of v1).

## F. The Flector page (revid 495)

F1. Read: "**F** = F_px **e**1 + F_py **e**2 + F_pz **e**3 + F_pw **e**4 + F_gx **e**423 + F_gy **e**431 + F_gz **e**412 + F_gw **e**321 , which is the sum of a point **p** and a plane **g**", with the constraint F_px F_gx + F_py F_gy + F_pz F_gz + F_pw F_gw = 0, which "means that the point **p** must lie in the plane **g**". This is the order of departure 4's `flector_layout`. Recomputed: the constraint holds for products of three random planes.

F2. Read: "An element **x** is transformed by a flector **F** through the operation **x**′ = −**F** ⟇ **x** ⟇ F̰". This is the single-sign form of the first row of CONVENTIONS section 8, unchanged since note 0005 A1 read the page (the same save date, 2026-07-19). The point row still has q′_w = −q_w. Recomputed in a scratch run: the point and plane rows equal −F ⟇ x ⟇ F̰ and are the negation of CONVENTIONS 5.6, and the line row's moment still has its sign reversed, as note 0005 A5 found. These rows were not added to the committed checks, because `check_versors` already pins the disagreement. Not a new finding.

F3. Read, "Conversion from Flector to Matrix": **A** has bottom row (0, 0, 0, 1). Recomputed: **M** = **A** + **B** equals CONVENTIONS 5.6's action on points, with the weight kept positive (`check_motor_and_flector_pages`). This agrees with note 0005 A4 (the poster's flector matrix). The page introduces the matrices with "Given a specific unitized flector **G**", but the formulas use F. Inference: G is a typo for F, and it bears on nothing.

F4. Read, "Factorization": **Q** = (1/√(1 − F_pw²)) (**F** ⟇ g̰), with its coefficients, and **F** = **Q** ⟇ **g**/√(1 − F_pw²). Recomputed without the square root, as **F** ⟇ g̰ = (−F_gx F_pw, −F_gy F_pw, −F_gz F_pw, 1 − F_pw², F_gy F_pz − F_gz F_py, …, 0) on the motor layout: exact. In a scratch run, the "Conversion from Matrix to Flector" formulas for F_p and F_gw also held.

F5. Read: "since **e**4 is a flector (representing inversion through the origin), any flector can be written as **Q** ⟇ **e**4". Recomputed: under 5.6, e4 sends (x, y, z, 1) to (−x, −y, −z, 1). The point-group generators use e4 as the inversion. The trigonometric form was not recomputed.

F6. Against plan 0023 O5. The proposed 5.8 sentence names the motor's halves (r_x, r_y, r_z, r_w) and (u_x, u_y, u_z, u_w), and the flector's (s_x, s_y, s_z, s_w) and (h_x, h_y, h_z, h_w). The pages name them Q_v, Q_vw, Q_m, Q_mw and F_p, F_pw, F_g, F_gw. The order is the same and only the letters differ. Recommended wording, for the implementer and the owner: use the pages' letters, so that a coefficient copies from a published formula by its name as well as its place. Label: "(Confirmed, research note 0023; closure under the antiproduct Checked)".

## G. Point-group orders

G1. Not read: the International Tables for Crystallography, volume A (see Sources). Also not read: the Bilbao server's tables.

G2. Read, the substitute: the table of section "In three dimensions" of Wikipedia's "Crystallographic point group" lists, for each of the 32 classes, the crystal family and system, a name, the Hermann–Mauguin symbol, the point symmetry ("enantiomorphic polar", "polar", "enantiomorphic", "centrosymmetric", "non-centrosymmetric"), the order, and the abstract group. The orders read are: 1: 1; −1: 2; 2, m: 2; 2/m, 222, mm2: 4; mmm: 8; 4, −4: 4; 4/m, 422, 4mm, −42m: 8; 4/mmm: 16; 3: 3; −3, 32, 3m: 6; −3m: 12; 6, −6: 6; 6/m, 622, 6mm, −6m2: 12; 6/mmm: 24; 23: 12; m−3, 432, −43m: 24; m−3m: 48. These are transcribed into `golden.py`'s `POINT_GROUPS` with the abstract group and the point symmetry.

How the substitute may differ from the Tables. It is a secondary account, edited by many people, and may hold transcription errors. It gives no generators, no matrices, and no setting, so it validates only the orders and the properties named below, not the elements. The Tables' own point-group tables give general positions in standard settings. The hexagonal and trigonal classes have a hexagonal basis there, with the principal axis along z, not along (1, 1, 1) as here (plan 0023 O8). The page cites the Tables' chapter on point groups (Hahn, Klapper, Müller, and Aroyo, doi 10.1107/97809553602060000930) behind a subscription, which was not read.

G3. Recomputed: each class's generators (`golden.py`, `POINT_GROUPS`) were closed twice. The first closure is of versors under the antiproduct, each normalized by its first nonzero coefficient (`conventions_check.versor_closure`, with `crystal_versor` building the rotors of half angle π/n as axis + r 𝟙 with r = |axis| cot(π/n)). The second is of matrices under multiplication (`rigid_motions.matrix_group`, by Rodrigues' formula with the full angle 2π/n, mirrors I − 2nnᵀ/|n|², and −I). For all 32 classes the two closures give the same set of matrices, every element fixes the origin, distinct normalized versors are distinct motions, and every versor has V ⟇ V̰ = λ𝟙 with λ > 0. The orders equal G2's. These properties of the matrix group also equal G2's columns: abelian and cyclic (the abstract groups Z_n are cyclic; V, V × Z2, Z4 × Z2, and Z6 × Z2 are abelian and not cyclic; the rest are not abelian), centrosymmetric (contains −I), enantiomorphic (every element has determinant +1), and polar (a nonzero direction is fixed by every element). So the substitute agrees on everything it states that can be computed.

G5. Read, a primary source: D. Hestenes, "Point Groups and Space Groups in Geometric Algebra" (knowledge base `references/crystallography/crystalsymmetry.pdf`, 23 pages; Table 2 generator code, Table 4 orders). Checked 2026-11-08 by `hestenes_check.py` and `props_check.py` (archived in the math-tools exercises, `ga/hestenes-point-groups/`): each of the 32 groups rebuilt from Table 2's symbols as exact Fraction matrices agrees with the library's class on order, on the multiset of (determinant, trace), and on the abelian, cyclic, centrosymmetric, enantiomorphic, and polar properties. 29 of 32 are identical as sets; 2/m, -42m, and 32 differ only by setting. Strong evidence of conjugacy, not proof [checked-self]. Notes: the paper uses rotary reflections, so -4 = S4, -3 = S6, -6 = C3h; its composition has the leftmost factor acting first (eqs. 56, 60-61), the library the rightmost, with no effect on the group as a set. Paper errors: Table 4 prints Schoenflies C6 for class 3 and C3 for class 6 swapped; Table 3 lists Cpv twice; the printed 22-bar = 2-bar-2 holds only up to relabeling; Fig. 7 draws "ac" for [2 2-bar] and [6 2-bar], read as {b, ac}. The paper prints no abstract-group or point-symmetry columns. The library's [111] setting is in no source, and the International Tables remain unread.

G4. The probe's orders (plan 0023 departure 9) equal G2's, class by class where the plan names the class and as a list of 32 otherwise.

## J. The joint-chain form

J1. Not read: Denavit and Hartenberg (1955). No claim is made that the chains follow their paper or the frame-attachment rules of any named convention. Each joint is the product Rz(θ) Tz(d) Tx(a) Rx(α) of 4 by 4 matrices acting on columns, so Rx(α) applies first, and a chain is the product of its joints with the first leftmost.

J2. Read, the secondary account: the "Denavit–Hartenberg matrix" section of Wikipedia's "Denavit–Hartenberg parameters" writes the link transformation as Trans_z(d) Rot_z(θ) Trans_x(r) Rot_x(α). It notes that Trans_z and Rot_z commute, giving Rot_z(θ) Trans_z(d) Trans_x(r) Rot_x(α), and gives the closed form [[cos θ, −sin θ cos α, sin θ sin α, r cos θ], [sin θ, cos θ cos α, −cos θ sin α, r sin θ], [0, sin α, cos α, d], [0, 0, 0, 1]]. Recomputed: `rigid_motions.joint` checks every joint's product against this closed form. Inference: the chains here have the form that account calls the classic DH matrix. That is a statement about the product, not about the paper. A mutant that swaps Rz and Tz in the geometric-algebra joint passes every check, because the two commute. It is equivalent, not a missed defect.

## P. The planning probes of plan 0023 departure 9, repeated

P1. `rga3_probe.py`, each fact with the committed check that now holds it:

| Probe fact | Committed check |
| --- | --- |
| antiproducts of motor and flector pairs stay on departure 4's layouts (50 pairs each) | `check_motor_flector_layouts`: the layouts are the even and odd blades (exhaustive), and 50 random pairs of each of the four kinds |
| ½d on e23, e31, e12 plus 𝟙 moves points by d and leaves a direction unchanged (20 cases) | `check_translation`: 50 cases, with a direction, a line's direction, and unitization; the probe's (2, 4, −6) case |
| s e4i + c 𝟙 turns as the right-handed matrix with cos θ = c² − s², sin θ = 2sc, for five (s, c) | `check_rotation`, the same five pairs about x, y, and z, and a line off the origin; `golden.py`'s `rigid_rotations` against Rodrigues |
| four-joint chains Rz Tz Tx Rx equal the 4 by 4 product on ten points, move lines as their points, stay unitized | `check_joint_chain` (10 seeded four-joint chains: unitized, lines and planes, order of application); `golden.py`'s `rigid_chains` (chains of 3, 4, 5, and 6 joints against the matrix product on ten points, one of weight 2 and one of weight 0) |
| the 32 classes close with the classical orders, every element an isometry fixing the origin with λ > 0 | `golden.py`'s `point_groups` (G3) |

P2. `incidence_probe.py`: for points of any weights, p ∧ q has direction w_p q − w_q p and moment p × q (`check_line_moment`, 200 cases). The plane of three unitized points is (n, −n · p) with n = (q − p) × (r − p). The line p ∧ q meets g in (m × g_n + g_w v, −v · g_n). Two planes meet in the line with direction h_n × g_n and moment h_w g_n − g_w h_n (the probe's "g2_n × g1_n" with g1 = g, g2 = h, for g ∨ h). These are `check_incidence`, 200 cases each. That check also has two facts the probe did not state: the three-plane meet (g ∨ h) ∨ k has coordinates det[g; h; k; e_j], and (p ∧ q) ∨ (a ∧ b) = det[p; q; a; b]. `golden.py`'s `rigid_incidences` compares each with the linear-algebra route, as does p ∧ q ∧ r, whose value at x is det[x; p; q; r].

P3. `model_probe.cpp`: its mathematical hand values are now in `conventions_check.py`: the translation (2, 4, −6) moves (1, 2, 3, 1) to (3, 6, −3, 1); 3 e43 + 4 𝟙 turns (1, 0, 0, 1) into (7, 24, 0, 25); and the mirror e412 sends (1, 2, 3, 1) to (1, 2, −3, 1), which `check_odd_sign` already held. Its C++ facts (lookup, `sizeof`, conversions, the model's types) belong to the test author (plan 0023, section 1, "The W2 question") and were not repeated here.

Every probe fact checked agrees with the committed check. None disagrees with departure 4, so CONVENTIONS section 8 gains no row.

## Golden values written

`golden.py` writes seven new namespaces, each with its `// Fact:` line and a trace row (Implemented `no` until a test states it). Each value is computed by both routes and written only where they agree exactly:

| Namespace | Item | Values |
| --- | --- | --- |
| `rigid_lines` | 5.2 | 8 lines p ∧ q of seeded points of weights 1, 2, −1, ½, 3 |
| `rigid_planes` | 5.3 | 8 planes p ∧ q ∧ r |
| `rigid_meets` | 5.3 | 8 each of line–plane, plane–plane, and three-plane meets |
| `rigid_incidence` | 5.3 | 8 each of p ∨ g, p ∧ (a ∧ b), and (a ∧ b) ∨ (c ∧ d), every other one incident by construction |
| `rigid_rotations` | 5.5 | 21 rotors: three half angles about seven lines, four of them off the origin |
| `joint_chains` | 5.4 | 10 points and chains of 3, 4, 5, and 6 joints with their matrices and images |
| `point_groups` | 5.6 | the 32 classes: symbol, order, generators as versors, and 332 element matrices |

The chains are drawn again, with the same seed, when the product of their half angles' squared denominators reaches 2³¹, so that every value written, and every coefficient of every partial product of their motors, stays below 2³¹. Then a test over the test rational (two `std::int64_t`) can form a product of two of them. The largest value written for the chains is 826173739. Inference: the library's own intermediate sums may still be larger than any value written, so the test author should watch for the rational's overflow stop in the six-joint chain.

## Not verified, and open

1. The International Tables (G1) and Denavit and Hartenberg (J1) were not read. The orders are read in Hestenes' Table 4 (G5), a primary source in the geometric-algebra literature, and the groups rebuilt from his Table 2 match the library's [checked-self]. The abstract-group and property columns still rest on Wikipedia (G2) plus computation, the [111] setting is in no source, and the chain form has no primary source. If the owner can supply the Tables or Denavit and Hartenberg as a PDF (the knowledge base's `references/`), those can be checked against it.
2. Lengyel's book, sections 3.6 and 3.7, which both pages cite, was not read.
3. The Flector page's three known problems (F2) are unchanged. Nothing new needs the owner. Note 0021 open question 1 (whether page errata go into section 8) still applies to the line-row sign.
4. Recommended wording of 5.8 (F6) for the owner's confirmation with step 2.
5. The meets of `rigid_meets` are confirmed by two routes only up to sign. In `rigid_motions.py`, the overall signs of `line_meets_plane`, `planes_meet`, and `three_planes_meet` were chosen to match the library's convention (the geometric route of `conventions_check.py`), not derived independently, and `planes_meet` checks itself only up to scale (its rank test against the line through two common points). A sign error shared by both routes would therefore pass `golden.py`. The incidence zeros of `rigid_incidence` do not depend on these signs. The plane's orientation is the exception: `self_check` pins `plane_through` against CONVENTIONS 5.3 (counterclockwise points about +z give the normal +z). Found by the WU16 blind review (finding 2); recorded, not changed.
