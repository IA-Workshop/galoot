# 0004 Romero et al. 2017: hollow-sphere Voronoi diagrams in CGA

Source: Netz Romero, Ricardo Barrón-Fernández, Salvador Godoy-Calderón, "Constructing Voronoi Diagrams from Hollow Spheres Using Conformal Geometric Algebra", *Advances in Applied Clifford Algebras* 27 (2017), 1997–2017, doi:10.1007/s00006-017-0787-x. Local copy in `references/`. Page numbers are the journal's.

Read by: an agent (Claude), 2026-10-31, from a text extraction. Every equation, table, and listing cited below was checked against the PDF page image (pages 2002–2003, 2006–2007, 2010). Not yet reviewed by the owner.

Owner's direction: Lengyel's wiki is the preferred source for conventions. This note treats Romero's conventions as a translation into Galoot's, never as a candidate convention.

Evidence labels:

- Read: stated in the paper at the cited place.
- Recomputed: checked by exact rational arithmetic in scripts written for this note: `ga.py`, `romero_check.py` (18 checks, all pass), and `insphere3d.py`. They live in this session's scratchpad (`scratchpad/checks/`), not in the repository. They implement Galoot's conformal(2) and conformal(3) from CONVENTIONS 1.4, 3.1, 3.3, and 4.2. Their sphere, dual, and antidual signs follow note 0005.
- Sampled: a random experiment, reported with its seed and size (V9).
- Inference: my conclusion, not stated in the paper.

## Summary

1. Romero's algorithm is incremental Bowyer–Watson in 2D. It stores circumcircles ("hollow spheres") instead of triangles and tests inside or outside with the inner product P · S. The design study's description is confirmed.
2. Its conformal basis is Galoot's with e0 for e4 and e∞ for e5. Its spheres, planes, and intersections translate into Galoot through the antidual, s☆ = right(𝔾 s). (Section 1)
3. The paper does not address robustness. It evaluates in doubles, classifies with exact "= 0", and has no tie-breaking. Its text and its code use opposite sign rules, and the code's rule depends on the orientation in which each circle was written. (Section 3)
4. The predicates the mesh module needs are single wedges with fixed constants. In conformal(2), a ∧ b ∧ c ∧ d = −½ incircle(a, b, c, d) 𝟙, and the e123 part of a ∧ b ∧ c is orient2d(a, b, c). In conformal(3), a ∧ b ∧ c ∧ d ∧ e = −½ insphere 𝟙, and the e1234 part of a ∧ b ∧ c ∧ d is orient3d. Here incircle, insphere, orient2d, and orient3d are Shewchuk's determinants. (Section 4)
5. The paper's worked example has an exactly checkable answer. It and five other cases are proposed as V8 validation cases. (Section 5)

## 1 Conventions, translated

1.1 Basis. Read (p. 2002): e1, e2, e3, e₊, e₋ with e₊² = 1 and e₋² = −1, e0 = ½(e₋ − e₊), e∞ = e₋ + e₊, both null. Recomputed: e0 · e∞ = −1. This is CONVENTIONS 1.4 exactly, with e0 = e4 (origin) and e∞ = e5 (infinity). The paper works in the 5D algebra with points at z = 0 (CLUCalc's N3 space, Listing 1, p. 2007). Inference: Galoot's natural model for it is conformal(2), with 16 components instead of 32.

1.2 Point. Read (Table 1, p. 2003): P = x + ½x² e∞ + e0. This is Galoot's round point (6.1) with r = 0, with the same coefficients.

1.3 Sphere. Read (Table 1): the IPNS sphere is S = P − ½r² e∞. Recomputed: S = s☆ = −s★, where s is the wiki's sphere (s_u = −1, note 0005 C3). As a vector, S is Galoot's round point with r² replaced by −r², so S · S = +r², whereas a round point of radius r has P • P = −r². Lengyel would call S imaginary. This is a translation hazard, not a disagreement: S is the dual of a real sphere, while Lengyel's round point is tied to its sphere by the container.

1.4 Inside test. Read (eq. 5.2, p. 2006): P · S = −½(s − p)² + ½r². The three bullets after eq. 5.3 say P is inside when P · S > 0, on the sphere when it is 0, and outside when it is negative. Recomputed: eq. 5.2 holds. In Galoot, P ∧ s = ½(r² − d²) 𝟙 for a unitized P and the wiki's sphere, which is positive inside, the same sign as Romero's. For other weights the value scales by a_w · (−s_u). Equivalently, P • s☆ = P · S.

1.5 Plane. Read (Table 1): π = n + d e∞. Recomputed: π = g☆ for Galoot's plane g, with n = (g_x, g_y, g_z) and d = −g_w.

1.6 Joins. Read (Table 1): the OPNS forms are S* = P1 ∧ P2 ∧ P3 ∧ P4, Z* = P1 ∧ P2 ∧ P3, and Pp* = P1 ∧ P2. These are Galoot's joins with no change. Notation trap: Table 1 uses a star to mark the OPNS form. The CLUCalc listings use `*` as the dual, which they apply to the OPNS wedge. Galoot's ★ is the bulk dual.

1.7 Meet. Read (Table 1): the IPNS circle is Z = S1 ∧ S2, the point pair Pp = S1 ∧ S2 ∧ S3, and the line L = π1 ∧ π2. Recomputed: Z = (s1 ∨ s2)☆ and Pp = (s1 ∨ s2 ∨ s3)☆. This follows from the poster identity (b ∨ c)☆ = b☆ ∧ c☆ (note 0005 B4). Romero meets by wedging dual vectors. Galoot meets with the antiwedge on the objects themselves.

## 2 Algorithm and the Galoot operations it needs

Read (sections 3.1 and 5, pp. 2000–2013):

- A hollow sphere passes through d + 1 sites and contains none (p. 2000). Its center is a Voronoi vertex. The segment joining the centers of two "border spheres", which share d sites, is a Voronoi edge.
- Initialization uses three auxiliary points whose triangle must contain every site (5.2, p. 2005).
- Inserting Q finds the circles that contain Q, deletes them, keeps their sites, sorts them, and builds the fan of circles P[i] ∧ P[i+1] ∧ Q (Algorithm 1, Listing 3, p. 2008).
- Circles are stored in a hash table that maps each circle to its up to three border circles (Table 2, p. 2004).
- Point location walks from circle to circle, comparing S · Q for the two border circles not yet visited (Algorithm 2, p. 2011). A KD-tree finds the nearest site, and the walk starts from a circle through that site (pp. 2011–2012).
- Complexity claims are empirical. Figures 13 and 17 average 30 runs per size. There is no proof, and no code is published.

Galoot operations needed: round points in conformal(2), wedge for joins and predicates, antiwedge and the antidual for meets, dot, carrier, cocarrier, center, and the radius norm. Recomputed: cen(c) = ccr(c) ∨ c for the circle through P1, P2, P5 of the paper gives the circumcenter (−141/260, −227/260) with radius squared 7141/6760, exactly. Voronoi vertices are therefore rational. A square root is needed only to output a radius.

## 3 Numerical and degenerate cases

3.1 Read: the paper uses doubles (Java, 5.5, p. 2013) and CLUCalc. It classifies "P · S = 0" as on the sphere with no tolerance (p. 2006), but Algorithm 2 uses only "< 0" and "<", so in its code a point on a circle counts as unaffected. It requires non-collinear triples ("as long as the points are not collinear", p. 2005) and sites inside the auxiliary triangle. It says nothing about rounding, cocircular sites, or duplicate sites.

3.2 Read: the text says P · S > 0 means inside (p. 2006). Listing 2 marks a circle as affected when `r(4) < 0`, and Algorithm 2 treats `dmn < 0` as inside (pp. 2007, 2011). The text and the code use opposite signs. Recomputed: all five circles in Listing 2 and all seven in Listing 4 are written clockwise, while the five new circles in Listing 3 are counterclockwise. Inference: the dual of P_i ∧ P_j ∧ P_k is not normalized, so its sign flips with the orientation of the triple. A fixed sign threshold is right only if every circle has the same orientation or is divided by its weight. In conformal(2) that weight is the e123 part, which equals orient2d (recomputed).

3.3 Recomputed: Algorithm 2 compares raw values of different circles. In Listing 4, Q = (0.5, −0.2) lies outside both border circles. Its exact powers d² − r² are 817/1150 for the circle through P7, P2, P1 and 227/350 for the circle through P1, P3, P4. The normalized power picks P1P3P4. Figure 14 (p. 2010) shows the paper picked P7P2P1. Inference: if CLUCalc's value is ½ |orient2d| times the power, the raw values are 0.4085 and 0.454, and the paper's choice follows from the unnormalized weight. CLUCalc's dual and inner-product signs were not checked.

3.4 Recomputed: the eight lattice points (±3, ±4), (±4, ±3), … on x² + y² = 25 give a ∧ b ∧ c ∧ d = 0 exactly, and so does the unit square. Their Voronoi vertex at the origin has degree 8. This contradicts the paper's claim that exactly d + 1 edges meet at each vertex (p. 2000), which holds only in general position. For a collinear triple, the e123 weight of a ∧ b ∧ c is zero: the circle is a flat line.

3.5 Sampled (seed 99, 2000 trials): four points at random angles on the circle of radius 0.5 centered at (0.25, 0.125), each rounded to doubles. A naive double evaluation of the 4×4 cocircularity determinant gave the wrong sign in 601 trials against exact evaluation of the same rounded inputs. None was exactly zero. This supports V13; it proves nothing.

## 4 Agreement with the design study

4.1 Confirmed: "2D Bowyer–Watson on empty circles ('hollow spheres'), using P·S for inside/outside" (design study, Existing CGA Voronoi work).

4.2 Confirmed: robustness is unaddressed, with doubles and exact "= 0" classification. Correction: the paper itself contains no least-squares circle fits. The design study took those from two precursors, which I did not read. Its reference list marks this paper "abstract only". The full text has now been read.

4.3 Recomputed: the design study's predicates are single wedges, with constants. In conformal(3), the coefficient of 𝟙 in a ∧ b ∧ c ∧ d ∧ e is −½ insphere(a, b, c, d, e), and the e1234 coefficient of a ∧ b ∧ c ∧ d is orient3d(a, b, c, d). In conformal(2) the constants are −½ for incircle and 1 for orient2d. Each was checked on 100 or 200 random rational inputs. All four are polynomial identities, so the checks also confirm the derivation.

4.4 Recomputed: the design study's VoroCrust vector S_i = P_i − ½r_i² e∞ is s_i☆ in Galoot, and S_i • S_j = −(s_i • s_j) = ½(r_i² + r_j² − d²). This uses the wiki's s • t formula (note 0005). The design study's "Lengyel's sphere is the complement of this vector … up to a fixed factor" is, precisely, S = right(𝔾 s) = −s★. For B = S1 ∧ … ∧ Sk, Galoot's dot gives B • B = det G_k directly, with no sign. The design study's B² = (−1)^(k(k−1)/2) det G_k also holds. Both were checked for k = 2, 3, 4.

## 5 Validation cases for the mesh module (V8)

Each has an answer known independently of Galoot.

V-1. The paper's example. Sites P1 to P7 from Listing 1 (p. 2007), as exact rationals. Recomputed: the Delaunay triangulation is exactly the seven circles of Listing 4. P7 lies strictly inside three of Listing 2's five circles, the ones through P2 P1 P5, P5 P1 P6, and P1 P4 P6, which matches Figure 9. The Voronoi vertex of P1 P2 P5 is (−141/260, −227/260).

V-2. Predicate identities against Shewchuk's determinants, with the constants of 4.3, on random and near-degenerate exact inputs.

V-3. Cocircular lattice ring: the 12 integer points on x² + y² = 25 have a single Voronoi vertex, of degree 12, at the origin. Every in-circle test is exactly zero.

V-4. An n × n integer grid. Its Voronoi cells are unit squares with (n − 1)² vertices of degree 4. Its Delaunay triangulation is not unique, but its Voronoi diagram is, so the case tests tie-breaking.

V-5. Random sites in exact arithmetic. A triangulation of n sites with h on the hull has 2n − 2 − h triangles. Every circumcircle is empty by brute force, and every Voronoi vertex is equidistant from its sites. Clipped cell areas sum to the box area.

V-6. 3D: the eight corners of a cube are cospherical (insphere zero), and a regular tetrahedron has a known circumcenter and radius.

## 6 Recommendations for the mesh module plan

R1. Write incircle and insphere as Galoot wedges with the constants of 4.3, and test them against Shewchuk's determinants.

R2. Never take the sign of a dual or wedge of an unnormalized circle or sphere alone. Multiply by the sign of its weight (orient2d or orient3d), or unitize.

R3. Evaluate those predicates under V13: filtered, with an exact fallback. Section 3.5 shows why.

R4. Break cocircular and cospherical ties by symbolic perturbation, as the design study plans for `mesh/delaunay`. Romero offers no tie rule.

R5. Do not port Algorithm 2's comparison of raw values. Walk with orientation predicates (a Lawson walk), or compare normalized powers.

R6. Keep Voronoi vertices exact. cen(c) is rational, and only output radii need a square root.

R7. Use conformal(2) for 2D diagrams, not conformal(3) restricted to z = 0.

R8. For VoroCrust, compute Gram entries as −(s_i • s_j) on spheres or as s_i☆ • s_j☆, and use B • B = det G_k.

R9. Inference: prefer a symbolic point at infinity, or exact handling of the hull, to a finite auxiliary triangle. Romero requires a finite triangle that contains all sites.

## Open questions

1. CLUCalc's definitions of `*` and `.` were not checked, so the sign explanation in 3.2 and 3.3 is inferred.
2. The two precursor papers the design study cites were not read for this note.
3. The KD-tree start relies on a circle through the nearest site containing Q. Inference: this holds, because the nearest site becomes a Delaunay neighbor of Q, but the paper does not prove it.
4. The paper leaves 3D as future work (p. 2015). There is no 3D example to validate against.
