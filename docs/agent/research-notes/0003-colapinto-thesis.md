# 0003 Colapinto's thesis: lessons for Galoot

Source: Pablo Colapinto, *Articulating Space: Geometric Algebra for Parametric Design – Symmetry, Kinematics, and Curvature*, PhD dissertation, UC Santa Barbara, 2016 (223 pages; local copy in `references/`).

Read by: an agent (Claude), 2026-10-31, from a `pdftotext -layout` extraction. The extraction drops minus signs, so every equation cited below with a sign was checked against the PDF page image. Page numbers are the printed ones.

Owner's direction for this note: Lengyel's wiki is the preferred source for conventions, and the design study is the canonical reference. The thesis matters for its generic-programming lessons, the operations Galoot will need, and its applications as validation cases. This note recommends no Colapinto convention. Its conventions appear only as a translation table for Versor users.

Evidence labels:

- Read: stated in the thesis at the cited place.
- Recomputed: checked by an independent exact-rational script written for this note (`scratchpad/scripts/cga_check.py`, `meet_more.py`, `dual_rel.py` in this session's scratchpad, not in the repository). It implements Galoot's `conformal(3)` from CONVENTIONS 1.4, 3.1, 3.3 and 4.2. Sampled results give the seed and number of trials (V9).
- Inference: my conclusion, not stated in the thesis.

## Summary of recommendations

1. Plan `exp`, `log`, `sqrt`, `inverse`, ratio and interpolation as named core operations. Simple-bivector closed forms come with the RGA3 motors (phase 2) and the CGA3 model (phase 3). The general bivector split is a later item. (Findings 2.1 to 2.6)
2. When they are specified, give exp and log their own CONVENTIONS entries. Each entry fixes the half-angle rule (already 5.5 and 5.7), the principal branch of the log, the alternative branch under its own name, and the results at angle 0 and angle π. (2.2, 2.7)
3. Add a second scalar concept for the transcendental operations (sqrt, atan2, asinh, acosh, cos, sin), separate from the minimal ring concept the products need. (2.8)
4. Every degenerate case the thesis meets returns a failure value (STYLE S9), not a silent result. The cases are zero norm, 1 + R² = 0, the log of a pure translation, and orthogonal circles. Branch selection on rounded scalars follows V13. (2.7)
5. Open a C11 entry and a test for the sign of r² in the round point of CONVENTIONS 6.1. Under Galoot's own bulk dual, the vector whose dual is a real sphere of radius R carries −½R² on e5, not +½R². (1.2)
6. Publish the Versor-to-Galoot translation table of Finding 1.1 in the user documentation, including the grade-dependent meet and dual signs and the different component order. (1.1)
7. Provide the two motor interpolations under distinct names, because they give different results. (2.5)
8. Use the thesis's applications as V8 validation cases. The strongest are finite point groups and their orders, wallpaper groups, Bennett-linkage loop closure, Denavit–Hartenberg forward kinematics, exp/log round trips, and the line-to-circle boost. (Finding 3)
9. Never take a thesis equation as a golden value. Recompute from the primary sources it cites, chiefly Dorst–Valkenburg 2011 [46] and Wareham–Cameron–Lasenby 2004 [134]. The thesis has no proofs and contains several errata. (5.1, 5.2)
10. Keep Versor's introspection idea: printable plans that name blades and list terms. (4.6)
11. Appendix A has no measurements, so Galoot must build its own build-cost and run-cost baselines (P1, P2). Any Versor comparison is a fresh black-box measurement. (4.3)
12. Validate space groups in RGA3 in phase 2, before CGA exists, because they need only rigid motions. (3.1)

## 1 Conventions: translation table and traps only

### 1.1 Translation table for a Versor user

Thesis notation is from §2.5 (pp. 56–57) unless noted. Galoot entries cite CONVENTIONS.md.

| Thesis / Versor | Galoot | Status |
| --- | --- | --- |
| Null basis n_o, n∞ with n_o · n∞ = −1 (eq. 2.39, p. 39; Table 2.4, p. 40) | e4 = origin, e5 = infinity, e4 · e5 = −1 (1.4) | Read. Same metric. |
| Point p = n_o + x + ½x²n∞ (eq. 2.44, p. 43) | x + e4 + ½x² e5 | Read. Coefficients carry over unchanged. |
| Diagonal basis: eq. 2.39 and §2.5 give n_o = ½(e₋ + e₊), n∞ = e₋ − e₊. The Table 2.6 caption (p. 64) gives n_o = ½(e₋ − e₊), n∞ = e₋ + e₊. | origin = ½(e₋ − e₊), infinity = e₋ + e₊ (1.4) | Read. The thesis contradicts itself. Galoot matches the Table 2.6 caption. |
| E = n_o ∧ n∞ = e₊e₋ (eq. 2.42, p. 41, under eq. 2.39) | e4 ∧ e5 = −e₊ ∧ e₋ | Recomputed. The sign flips when converting diagonal-basis code. |
| Dual X* = X I⁻¹, I = e123 n_o n∞ (eqs. 2.17, p. 27; Table 2.5, p. 42) | X I⁻¹ = −bulk_dual(reverse(X)) | Recomputed: 5 random blades per grade 0–5, seed 3, exact. |
| Undual X I (eq. 2.20, p. 27) | bulk_dual(reverse(X)), that is (−1)^(k(k−1)/2) bulk_dual(X) | Recomputed, same sample. |
| Meet (A* ∧ B*)I (eq. 2.25, p. 28) | (−1)^(ag(A)·ag(B)) A ∨ B | Recomputed: 95 random integer blade pairs, all grade pairs with gr A + gr B ≥ 5, seed 11, exact, no mismatch. A sampled identity, not a proof. |
| Dot A · B = lowest-grade part (eq. 2.71, p. 57); Versor `<=` tests a left contraction (Alg. A.4, p. 180) | dot is the scalar aᵀGb (4.1); contractions are compositions such as a ∨ b★ | Read. Not interchangeable. The thesis's text and its code also disagree. |
| Reverse (−1)^(k(k−1)) X (eq. 2.22, p. 28) | (−1)^(k(k−1)/2) (2.1) | Read. The thesis omits the /2. Conjugation, eq. 2.24, has the same slip. |
| Dual sphere σ = p − ½r²n∞, σ² = r² (eqs. 2.52–2.53, p. 47) | Same coefficients form the vector whose bulk dual is the sphere | Recomputed. See 1.2. |
| Rotor R = e^(−θB/2) = cos(θ/2) − sin(θ/2)B, X ↦ RXR̃ (Table 2.6, p. 64; eq. 2.65, p. 53) | RGA motor l sinφ + 𝟙 cosφ applied as Q ⟇ x ⟇ Q̰; `rotation(l, θ)` stores half-angles (5.5, 5.7) | Read. Both store half-angles. The product, the sign and the blades differ. Galoot's CGA versor conventions wait for phase 3. |
| Translator T = 1 − ½t n∞ (§2.4.4, p. 53) | `translation(d)` stores ½d on e23, e31, e12 plus 𝟙 (5.7) | Read. §5.2.2 (p. 120) writes T = 1 + v n∞/2, so the thesis mixes signs. |
| Odd versor A ↦ X Â X⁻¹ (eq. 2.28, p. 31) | Flector −F ⟇ x̂ ⟇ F̰ (5.6) | Read. Both put the grade involution on the argument. This is consistent with ratified choice 4. |
| R₂R₁ applies R₁ first (p. 36) | `compose(Q2, Q1)` = Q2 ⟇ Q1 | Read. Same order. |
| Components ordered by grade, then by mask value: e12, e13, e23, e14, e24, e34 (Alg. A.6, p. 181; printed output, p. 177) | By grade, then lexicographic: e12, e13, e14, e23, e24, e34 (1.7) | Read. Coefficient arrays cannot be copied across. |
| Operators `^` wedge, `<=` inner, `*` geometric, `/` division by a versor, `.dual()`, `.undual()` (p. 63, p. 176) | `^`, `&`, `*`, `%`; no versor division (7.1) | Read. |
| Signed norm: +√(X·X̃) if X² ≥ 0, −√(−X·X̃) if X² < 0 (eq. 2.21, p. 28) | Bulk and weight norms (3.x) | Inference: as printed, the rule takes the square root of a negative number for grades 2 and 3, where the reverse is negative (for example X = e12). |

### 1.2 Trap: the sign of r² in a round point

Read: the thesis's real dual sphere is p − ½r²n∞, with square +r² and imaginary spheres at +½r² (pp. 47, 51).

Recomputed: in Galoot's `conformal(3)`, I wedged four points on a sphere with center (1, 2, −1) and radius 3. The vector v with bulk_dual(v) proportional to that sphere (CONVENTIONS 3.3) is c + e4 + ½(c² − R²) e5, and v • v = +R². CONVENTIONS 6.1 writes the round point as p + e4 + ½(p² + r²) e5 with P • P = −r². For r² = R², a point q on the real sphere gives q • P = −R², not zero. So under Galoot's own bulk dual, 6.1's formula with r² > 0 describes an imaginary sphere. 6.1 is marked "Study" (unconfirmed). Inference: Lengyel's wiki may define the round point or its dual differently. The other notes on Lengyel's sources should settle the sign. Either way, Galoot needs a C11 record and a test that bulk_dual(round_point(c, r)) contains the points at distance r from c.

## 2 Operations Galoot will need

### 2.1 Exponential of a 2-blade

Read: e^(−θB/2) is cosh − sinh B for B² > 0, cos − sin B for B² < 0, and 1 − (θ/2)B for B² = 0 (eq. 2.62, p. 52). The generators listed are rotation, rotation about a dual line, translation, dilation about the origin and about a point, transversion, and the point-pair boost (p. 53; Table 2.6, p. 64).

Inference: the formula branches on the exact sign of B². With rounded scalars, a near-null bivector can fall into either trigonometric branch. Galoot should use a form continuous at B² = 0, such as cos(√−B²) with sinc, and their hyperbolic versions.

### 2.2 Logarithm of a simple rotor

Read: log R = atanh2(⟨R⟩₂, ⟨R⟩) (eq. 2.68, p. 55). For s = ⟨R⟩₂ and c = ⟨R⟩: asinh(√s²)/√s² · s if s² > 0; s if s² = 0; atan2(√−s², c)/√−s² · s if −1 ≤ s² < 0 (eq. 2.69, p. 56). A variant −(π − atan2(√−s², c))/√−s² · s turns the other way around the orbit (eq. 2.70, p. 56). It is chosen when the starting corner has a negative dot product with the far coordinate sphere (eq. 6.21, p. 152), a rule found "through experiment on a computer".

Inference, hazards: the s² > 0 case ignores c, so it fails for a rotor with a negative scalar part. The s² = 0 case returns s whatever c is, which assumes a unit rotor. The cases split on exact equality. The direction rule is empirical.

### 2.3 Motor exponential and logarithm

Read: for λ = θB + d n∞, e^λ = (cos θ + sin θ B)(1 + d⊥ n∞) + sinc(θ) d∥ n∞ (eq. 5.1, p. 120, after [134]). The log of a normalized motor (eq. 5.2, pp. 120–121) uses ‖ab‖ = cos⁻¹⟨M⟩, s = sinc‖ab‖, and sq = ‖ab‖² s, and divides by s and by sq. The motor must be normalized first with ‖M‖ = ±√(MM̃) (p. 120). The printed formula writes ⟨R⟩₂ and ⟨R⟩₄ where the motor M is meant.

Inference, hazards: sq = 0 for a pure translation, so the printed log is undefined there. cos⁻¹ loses accuracy near ⟨M⟩ = ±1, which means small rotations. The ± in the norm leaves the sign of a normalized motor open. Galoot needs the translation limit and an atan2 form.

Read: eq. 5.1 uses e^λ with no half and no minus sign, while Table 2.6 uses e^(−θλ/2). This confirms the design study's "mixed half-angle conventions" in the text itself.

### 2.4 Square roots and ratios of versors

Read: the rotor taking a to b is (1 + ba)/√|2(1 + ⟨ab⟩)| (eq. 2.38, p. 35), and in general √R² = (1 + R²)/‖1 + R²‖ (p. 55), valid for simple rotors. Twice the transformation taking blade A to blade B is (B/A)/‖B/A‖ (eq. 2.66, p. 54). The relative rotor between R₁ and R₂ is R₂R₁⁻¹ (§2.4.5.2, p. 56). The versor inverse is Ã/(AÃ) (eq. 2.18, p. 27). The general square root in R(4,1) is eq. 5.11 (p. 134, from [46] eq. 5.4). Its denominator (1 + ⟨C⟩)² − ⟨C⟩₄² vanishes for the ratio of two orthogonal circles. There the root is not unique, and eq. 5.16 (p. 135) needs a chosen 2-blade B with B² = −1 that commutes with ⟨C⟩₄. Algorithm 5.1 (p. 136) builds B from a chosen tangent direction and tests the condition (1 + C₀)² = C₄² by exact equality.

Inference, hazards: 1 + R² = 0 for a half-turn, so eq. 2.38 divides by zero. The orthogonal-circle case is the same degeneracy in R(4,1). Ã/(AÃ) needs AÃ to be a scalar, which a checking build can verify (C8). The inverse of a scalar plus a quadvector, needed for the split, is (⟨X⟩ − ⟨X⟩₄)/(⟨X⟩² − ⟨X⟩₄²) (p. 135). It fails when ⟨X⟩² = ⟨X⟩₄².

### 2.5 Interpolation

Read: R_t = e^(−(t/2) log R²) (eq. 2.67, p. 55), with R² = e^(−2θλ) stated just before it. For motors there are two methods, e^(t log(M₂M₁⁻¹)) M₁ and e^((1−t)λ₁ + tλ₂), and "these methods give different results" (eq. 5.3, p. 121). The second extends to trilinear blends of eight frames (Fig. 5.3, p. 123).

Inference: with log R² = −2θλ, eq. 2.67 as printed runs backward (e^(+tθλ)), so its leading minus is probably an erratum. A test should pin the endpoint R_1 = R. Under C10, the two motor interpolations are two concepts and need two names.

### 2.6 General bivector split

Read: a general bivector splits into commuting, orthogonal 2-blades with e^B = e^(B₊) e^(B₋) (p. 56; eqs. 5.5–5.6, p. 128). The split is eqs. 5.12–5.15 (pp. 134–135, from [46]). As printed, eq. 5.14 reduces to ⁴√(F⁴), so brackets were probably lost. Eq. 5.15 branches on S∓² = 0.

Inference: implement the split from [46] or from the invariant decomposition the design study cites, never from the thesis's transcription.

### 2.7 Round operations and projections

Read: projection (a · B)B⁻¹ and rejection (a ∧ B)B⁻¹ (eqs. 2.29–2.30, pp. 31–32). Point-pair split p∓ = (κ ± √|κ²|)/(−n∞ · κ) (eq. 2.57, p. 50). The line–sphere meet is dualized, not undualized, to keep the orientation of p₋ ∧ p₊ (eq. 2.58, p. 50). Round from a sphere and a direction: σ ∧ (σ · (−Â n∞)) (eq. 2.59). Surround sphere κ/(−n∞ · κ) (eq. 2.60, p. 51) and κ/(κ ∧ n∞) (eq. 2.61). Tangent extraction p⌋Ŝ, and construction of rounds from a point and a tangent (eqs. 6.7–6.10, p. 145). The plunge p ∧ σ₀ ∧ σ₁ (Alg. 6.1, p. 154).

Inference, hazards: √|κ²| hides imaginary pairs, so reality must be reported as the sign of κ², a V13 predicate. The denominator −n∞ · κ is zero for a flat point pair.

### 2.8 Scalar requirements

Inference: everything in 2.1 to 2.7 needs sqrt, division, atan2, acos or asinh, and cos and sin. The core scalar concept in the design study has only +, −, ×, 0 and 1. A second concept, satisfied by float, double, dual numbers and intervals but not by expansions or exact rationals, keeps the products exact and generic.

## 3 Applications as validation cases

### 3.1 Space groups (chapter 3)

Read: roots a, b, c of unit length satisfy (ab)^p = (bc)^q = (ca)² = −1 (eq. 3.1, p. 69). b is found as the meet of two rotated planes (eq. 3.8, p. 70). Generators are reflections a, b, c, rotations ab, bc, ca, and the rotoreflection abc (p. 72). Algorithm 3.1 (p. 74) closes the group by applying generators to a random seed vector until no new result appears. Stated group sizes: 8, 4, 4 and 2 for the four p = q = 2 groups (Table 3.3, p. 75), and 12 for 3̄3̄ in general position (Fig. 3.8, p. 85). Lattices come from Table 3.4 (p. 78), glides and screws from translators (§3.3.2). Appendix B shows the 17 plane groups.

Requirements (inference): Euclidean vectors as odd versors, rotors, translators, and composition. All of these are rigid motions, so RGA3 flectors and motors suffice. Root coordinates are cos(π/p) for p ∈ {2, 3, 4, 6}, which are quadratic irrationals.

V8 cases (inference): the orders of all 32 crystallographic point groups and the 17 plane groups, whose referent is the International Tables. Group closure under compose. The "is a new result" test of Algorithm 3.1 needs a stated tolerance or exact arithmetic.

### 3.2 Linkages and inverse kinematics (chapter 4)

Read: Denavit–Hartenberg parameters become motors, with the link M_L = T(a e2 n∞ + r e3 n∞) R_α in the e13 plane and the joint R_θ in the e12 plane (eq. 4.1, p. 94). Frames compose as M_i = M_{i−1} M_{L,i−1} M_{J,i} (eq. 4.2). FABRIK uses sphere, plane and circle meets and the point-pair split (Algs. 4.1–4.6, pp. 98–106), and "there is no guarantee of convergence" (p. 102). The Bennett linkage obeys sin α₂ = −sin α₁ · a₂/a₁, with alternate links equal (eq. 4.4, p. 108). It is solved in closed form by intersecting two spheres and a plane, and the solver must "pick one" of the two points (Alg. 4.7, p. 110).

Requirements (inference): motors, CGA spheres, circles, point pairs, meets, projection onto planes, and a branch choice in the point-pair split.

V8 cases (inference): the product of the four Bennett link and joint motors equals the identity, exactly with exact scalars. The joint angles match the classical Bennett relation between θ₁ and θ₂ from the kinematics literature (not stated in the thesis, to be confirmed from a primary source). Motor forward kinematics matches an independent 4×4 Denavit–Hartenberg matrix product. FABRIK is not a V8 case, since it has no exact answer, but it is a good stress test.

### 3.3 Motors, boosts and orbits (chapter 5)

Read: a null point pair κ bends a line into a circle of radius 1/c under C = 1 − (c/2)κ (eq. 5.4, p. 126). A real-circle orbit under e^(tκ/2) closes with period 2π (Fig. 5.8, p. 127). The Hopf and "modular" results (pp. 129–137) are stated as found by experiment, without proof.

V8 cases (inference): exp(log M) = M and log(exp λ) = λ for |θ| < π, against Chasles screw parameters computed separately. Images of a line under eq. 5.4 lie on the analytic circle of radius 1/c. The orbit period. The knot and Hopf claims are hypotheses under V9, not referents.

### 3.4 Cyclidic nets (chapter 6)

Read: patch rotors are half-logs of normalized sphere ratios, and the two generators must commute (eqs. 6.1–6.5, pp. 143–144). Frames propagate around four concircular points by eq. 6.17 (p. 149).

V8 cases (inference): commutation, concircular corners and circular edges are checkable identities. Interior points need a closed-form Dupin cyclide from the literature ([17]) as referent. This is the weakest case.

### 3.5 Precision and performance

Read: the thesis targets real-time graphics on embedded devices (p. 172) with `float` values (p. 181), and reports no accuracy study or timings.

## 4 Appendix A: implementation lessons

4.1 Goals. Read: one stage, pure C++ with no DSL, header-only at about 200 kB, portable down to a Raspberry Pi (pp. 172, 174–175). Galoot's C13 and STYLE S6 share these goals.

4.2 Stated limits. Read: template generation is slow. Pre-instantiating types cuts compile time. Template-depth limits obstruct algebras "for dimensions higher than 10". The code needs experts to read (pp. 175–176). The blade type is `short` because "we do not plan to go above 16 dimensional representations" (p. 178). Inference: the design study's practical cap of about 9 dimensions comes from issue #24, not from the thesis, and the two are consistent.

4.3 No measurements. Read: Appendix A gives no compile times, memory use or run-time figures. Its claim that Gaalet's expression templates add "significantly to compile time" (p. 174) is unmeasured. Inference: Galoot has no quantitative Versor baseline from this source.

4.4 Mechanism. Read: types and combinatorics are fixed at compile time, and weights are computed at run time (p. 184). Each term is an `Inst` carrying a sign flag, two blades and two indices (p. 185). The return type is the deduplicated, sorted set of all product blades (Alg. A.10, p. 187). Instructions are grouped per output blade and summed recursively (Algs. A.11–A.12, pp. 189–190). The conformal metric split is only "present in the online code base" (p. 191). Inference: the documented term type can hold only ±1 coefficients, and the result type keeps structural zeros only, which confirms two pain points in the design study.

4.5 Cost structure. Read: insert-sort, membership and reduction are recursive head-and-tail type instantiations, called "a rather naive implementation", with early exits added (p. 184). Inference: every step instantiates a type, which is exactly what STYLE S7 replaces with loops over values.

4.6 Introspection. Read: any result prints its blades with bit patterns, and any product prints its generated instruction list, for example `a[0] * b[1] /*e12*/ -a[1] * b[0] /*e12*/` (pp. 176–177). Inference: worth keeping as a plan formatter in blade names. It serves debugging, documentation, and the design study's plans-as-data export.

4.7 Errata in Appendix A. Read: Algorithm A.4's pseudocode returns the inverse of its code (p. 180), and Algorithm A.6 compares `a == b` where grades are meant (p. 181).

## 5 Process and planning

5.1 Read: "no mathematical proofs of the formulas developed are provided"; images serve as "proofs-of-geometric-concept" (§1.5, p. 12). Several results are reported as found by computer experiment (pp. 129, 137, 152). Inference: these are hypotheses under V9.

5.2 Errata found while reading: eqs. 2.21, 2.22, 2.24, 2.39 against Table 2.6, 2.67, and 5.14; the ⟨R⟩/⟨M⟩ slip in §5.2.2; and Algorithms A.4 and A.6. Inference: thesis equations are pointers to [46], [134], Dorst–Fontijne–Mann [45] and Hestenes–Holt [73], never expected values (C15).

5.3 Read: the chapters follow a deliberate order, from symmetry (reflections) to kinematics (motors) to curvature (conformal rotors), each step reusing the last (§1.4, p. 11). Inference: this order maps onto the roadmap. Space groups and Bennett need phase 2 (RGA3). FABRIK, boosts and cyclides need phase 3 (CGA3) plus exp, log and split. The constructions belong in `geom/form`.

5.4 Read: the thesis names Lie sphere geometry in R(4,2) as its next step (§6.5, p. 160). Inference: this shares a signature with the design study's twistor track and needs no core change.

## Open questions

1. Which sign does Lengyel's wiki use for r² in the round point, and does CONVENTIONS 6.1 match Galoot's own bulk dual? (1.2)
2. Which principal branch should `log` use, and what should it return at angle π, where the square root is not unique? (2.2, 2.4)
3. Should the meet-sign and dual-sign relations of 1.1 become proved identities in `test/reference`, rather than sampled ones?
4. Does the general split go in the core, as a Lie layer per the design study, or in a model extension? It needs the second scalar concept either way.
5. Is a closed-form Dupin cyclide referent available for the cyclidic-net validation, or should that case be limited to identities?
