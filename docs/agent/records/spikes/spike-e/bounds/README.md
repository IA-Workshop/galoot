# Spike E, part F: forward error bounds of forms A, B, C, and drifted motors

Written by Claude (agent, Opus 5.5) in the numerical-expert role, 2026-11-10, on `research/spike-e--bounds` from `develop` at `d0bbe49`. Only this directory was written; nothing under `include/` or `test/` changed. The forms are those of part B (`../forms.py`, `../README.md`): A the restricted composition the kernels compute, B the collected form, C the constraint form. The benchmarks and the collection prototype of part F are the performance-engineer's and are not here.

## Departures, skipped checks, and ambiguities

1. **Two counts of K.** The core's rule (`kernel_roundings` in `transform.hpp`, and `tracked.hpp`) counts a product by a coefficient other than ±1 as a rounding. A doubling is exact in binary floating point unless it overflows, so it can be counted 0. Tables give both, as "core/exact"; the empirical check uses the exact count, which is the tighter claim. For form A they agree (no coefficient other than ±1).
2. **K depends on the summation order of the program**, not only on the form. The K of B and C are those of the programs in `../forms.py` (left folds in the order of the terms). Another order of the same form, such as pairwise sums, has another K. No order was searched.
3. **Form C's two arrangements** (the motor doubled first, or the factor 2 applied to results) give the same bits for every input, since a doubling commutes with rounding; `drift.py` checks this on every case and the tables show C once. They differ only in the core rule's K.
4. **The sampler of the chains.** The first full run drew step motors from rationals with small numerators and denominators. Their correctly rounded coefficients round the same way in most draws, so λ of the rounded steps drifts systematically (down in double, up in float), and that drift dominated the chains' λ − 1. The main run (`drift.md`) uses 21-bit dyadic stereographic parameters instead; the first sampler is kept as `drift-small.md` (`--small-steps`), since form C's error grows with λ − 1 and an application's step motors can have such a bias too.
5. **Conformal(3) is inferred, not computed.** What a collected plan does to the check's K for the conformal(3) flat motor (section 6) is derived from research note 0022's identities F2 and F3 and the shape of the collected program, not from a program run here.
6. **Sampled, not worst case.** The bounds are proofs; the observed ratios are from seeded random inputs (V9), with no adversarial search for inputs that approach a bound. The largest M_C / M_A values are maxima over 200 sampled unit motors, not suprema.
7. **The emulation is checked against the library only for form A and `compose`.** `replay.py` replays 300 cases per object and scalar through a probe built from the headers and finds every coefficient equal bit for bit, zeros' signs included. Forms B and C have no library implementation; their programs are their definitions.
8. **One probe build** (`probe.cpp`, one `g++` compile of 0.7 s) ran under `tools/timed` with its row in this directory's `runs.tsv`, not the shared log. The pool was not running and was not started; a single compiler call is not a pool step. No timing measurement was made, so the timing lock was not taken.
9. **No knowledge-base note was added**, since the brief limits writes to this directory; the role file asks for one. Two candidates are listed at the end.
10. **Read in part only:** CONVENTIONS (sections 1 and 5 in full, the rest by heading), STYLE (S11), WORKFLOW and PERFORMANCE (not read; this work has no build, merge, or timing). Never read: the Terathon source and the book (C15, C16). No web request was made.

## Model of arithmetic and terms

The model is that of decision 0180: each operation rounds once to nearest in the scalar (u = 2^−53 for double, 2^−24 for float), fl(a op b) = (a op b)(1 + δ) + η with |δ| ≤ u, |η| ≤ λ_min u for a product and η = 0 for a sum; no fused multiply-add (the test builds use `-ffp-contract=off`); no overflow; v and x exact as stored. γ_k = k u / (1 − k u). With underflow, the padded magnitude P of decision 0180 replaces M below; all inputs here are of ordinary size, where P and M agree to about 2^−900 relative.

The motor Q has weight r = (v, s) = (Q_vx, Q_vy, Q_vz, Q_vw) and bulk b = (m, t) = (Q_mx, Q_my, Q_mz, Q_mw) (CONVENTIONS 5.8). Then

- λ = |v|² + s², the unit constraint λ = 1;
- h = v · m + s t, the geometric constraint h = 0;
- T = 2(s m − t v + v × m), which for a unit versor is the translation;
- R_λ u = (s² − |v|²) u + 2 (v · u) v + 2 s v × u, and R̂ = R_λ / λ, a rotation for every λ > 0 (`derive_bounds.py` checks R_λ u · R_λ w = λ² u · w).

For one output coefficient y of a form, K is the largest number of roundings on a path from an input to y, and M is the same program run on the absolute values of the inputs with every subtraction an addition. Lemma A of decision 0180 is proved for every node of a straight-line program, so it bounds every kept output as well as the dropped ones:

    |ŷ − y| ≤ γ_K M.

The rounding check of `transform` uses this only for the coefficients it drops, with tolerance fl(fl(γ_(K+1)) P). Nothing checks the kept outputs at run time; their bound is what a test's tolerance comes from (V12).

## 1. K of each form

From `derive_bounds.py` section 1. A form-A count is the same under both rules.

| Object | Output | A | B (core/exact) | C (core/exact) |
| --- | --- | ---: | ---: | ---: |
| point | x, y | 12 | 8/8 | 9/8 |
| point | z | 12 | 7/7 | 9/8 |
| point | w | 5 | 5/5 | 0/0 |
| line | vx, vy | 7 | 7/7 | 7/6 |
| line | vz | 7 | 6/6 | 7/6 |
| line | mx, my, mz | 14 | 11/10 | 14/12 |
| plane | gx, gy | 7 | 7/7 | 7/6 |
| plane | gz | 7 | 6/6 | 7/6 |
| plane | gw | 12 | 9/8 | 15/13 |

Collection lowers K where the restricted composition sums long chains: the middle coefficients of stage 1 are sums, and stage 2 adds a chain of their products, so a path holds both chains. B sums each matrix entry (at most 4 monomials) and then the row (at most 4 products), and its paths are shorter. On the weight, the directions, and the normals, whose entries are short in both, K hardly changes. Form C's point weight is the input itself (K = 0, exact); its plane offset has the longest path of any form (15/13), through T, the rotated normal, and their dot product.

The checking-mode coefficients that form A drops have K = (12, 5, 5, 5) for the point, (14, 7) for the line, and (12, 12, 12, 7) for the plane, in the order of the dropped blades. `replay.py` reads these from the library (`transform_dropped_roundings`) and they equal the script's.

## 2. M: collection changes no magnitude

**Lemma 1.** For every kept output of a motor's transform of a point, a line, or a plane, M_A = M_B as polynomials in |Q| and |x|.

Proof. M_A is the restricted composition run on absolute values. Its products and sums of nonnegative values distribute, so M_A = Σ over the raw paths (a stage-1 term and a stage-2 term that reads it) of the product of the three absolute factors. Group the paths by their monomial Q_a Q_b x_j: the polynomial y has the coefficient c = Σ (signs of the group), and B evaluates y with that c, so M_B = Σ |c| |Q_a Q_b x_j| ≤ M_A, with equality exactly when no monomial gathers paths of both signs. `derive_bounds.py` checks that the number of raw paths of each output equals the sum of the |c| of its monomials (for example, 20 paths and 12 monomials for a point coordinate; 36 and 20 for a line's moment; 28 and 16 for the plane's offset), and that M_A − M_B is the zero polynomial for all 14 outputs. ∎

So in rigid(3) the like terms that collection combines in a kept output always have the same sign (Q_a Q_b + Q_b Q_a = 2 Q_a Q_b): **collection removes no cancellation and adds none**. The only paths with opposite signs are those onto the dropped blades, which cancel completely: there every dropped polynomial is 0 while M_A > 0 (section 6).

**Lemma 2 (norm form of M).** Each M is Σ_j c_j(|Q|) |x_j|, and each c_j is one of: λ; 2(|a||b| + |c||d|) with {a, b, c, d} the four weight coefficients; or 2 Σ_i |b_σ(i)| |r_i| with σ a bijection from the weight's coefficients to the bulk's (`derive_bounds.py` section 2 checks this for every coefficient of every M). The first two are at most λ (2|a||b| ≤ a² + b²), the third at most 2 √λ |b| (Cauchy-Schwarz). With |T|² = 4(λ |b|² − h²), checked, 2 √λ |b| = (|T|² + 4h²)^(1/2), which is |T| for h = 0. Hence, with ‖·‖₁ the sum of absolute values of the object's coefficients named:

| Object | Output | M ≤ |
| --- | --- | --- |
| point (u, w) | position, each | λ ‖u‖₁ + 2 √λ \|b\| \|w\| |
| point | weight | λ \|w\| (equality) |
| line (d, k) | direction, each | λ ‖d‖₁ |
| line | moment, each | λ ‖k‖₁ + 2 √λ \|b\| ‖d‖₁ |
| plane (n, g_w) | normal, each | λ ‖n‖₁ |
| plane | offset | λ \|g_w\| + 2 √λ \|b\| ‖n‖₁ |

For a unit versor 2 √λ |b| = |T| is the length of the translation, so a point coordinate's bound is γ_K (‖u‖₁ + |T| |w|).

**The bounds of A and B.** For each output, |ŷ − y| ≤ γ_K M with the K of section 1 and the M of Lemma 2, the same M for both. B's bound is A's times γ_(K_B)/γ_(K_A): about 2/3 on a point's coordinates, 0.71 to 0.79 on a line's moment, 0.67 to 0.75 on a plane's offset, and 1 or 6/7 elsewhere. The bound for a batch under one motor is the same as per call, since the arithmetic is the same and only shared.

**Shared quadratic terms.** In B a rounded motor product, and each rounded matrix entry, is read by several outputs and, for a batch, by every object. This does not change any componentwise bound (each output's K counts the shared node's roundings once on its path), but it correlates the errors: B computes (M(Q) + ΔM) x plus a per-object rounding of the final dot products, with |ΔM_ij| ≤ γ_(k_ij) |M|_ij for the entry's own count k_ij, so a whole batch moves by one perturbed linear map. A's errors are separate per object. Derived, not measured: no test here looks at relations between objects of a batch.

## 3. Form C's own rounding

C is a different program, of degree 4 in Q for the line's moment and the plane's offset (it multiplies T, quadratic, by a rotated vector, quadratic), so its magnitude is not A's. At exact unit motors (200 samples, `derive_bounds.py` section 5) the largest M_C / M_A per output was 1.7 to 2.1 for a point's position (1 for the weight), 2.1 to 3.1 for a line, and 2.3 to 2.6 for a plane's normal and 3.96 for its offset. So C's own bound γ_(K_C) M_C exceeds A's by up to about 4.3 on the plane's offset (γ_13 · 3.96 against γ_12) and is at most about 1.4 times A's on a point coordinate (γ_8 · 2.13 against γ_12). This bound holds for any Q; it is the bound on C's distance from C's own exact value, not from the sandwich.

## 4. The sandwich off the constraints (forms A and B)

All of these are polynomial identities checked by `derive_bounds.py` section 3, for every Q:

- **D1. Points and planes do not see h.** Replacing the bulk b by b + α r changes h by α λ and leaves T and the point and plane sandwiches unchanged. The sandwich reads the bulk only through T, and T(r, r) = 0. So a motor with h ≠ 0 moves points and planes exactly as its renormalization b ← b − (h/λ) r does, research note 0022's helper.
- **D2. λ only scales.** S(point) = (R_λ u + w T, λ w), and λ S_w(plane) = λ² g_w − R_λ(n) · T. Since R̂ is a rotation for every λ > 0, a motor with nonzero weight moves every point and plane to its image under one rigid motion (rotation R̂, translation T/λ), scaled by λ, which `transform`'s comment already states. So on a drifted motor, A and B make no error beyond their rounding for points and planes, against the motion the motor represents.
- **D3. Lines see h.** S_d = R_λ d and λ S_k = λ R_λ k + T × R_λ d + 2 h R_λ d. Replacing b by b + α r adds 2 α R_λ d to the moment. The image's Plücker product is S_d · S_k = λ² (d · k) + 2 h λ |d|², so a line moves to a screw of pitch 2h/λ, research note 0022's additive defect τ. Against the renormalized motor, the moment is off by 2 h R̂ d, of norm 2 |h| |d|.

So for forms A and B the only extra error on a drifted motor is the line moment's 2 h R̂ d. A and B share it, since they compute the same polynomial.

## 5. Form C off the constraints

`derive_bounds.py` section 4 checks, as polynomial identities (the plane's offset and the line's moment multiplied through by λ):

| Object | S − C |
| --- | --- |
| point | (λ − 1) p, every coefficient |
| line, direction | (λ − 1) d |
| line, moment | (λ − 1)(k + T × d − T × R̂ d) + 2 h R̂ d |
| plane, normal | (λ − 1) n |
| plane, offset | (λ − 1)(g_w − n · T + R̂ n · T) |

Readings:

- **C does not see h at all.** S_k − 2 h R̂ d is the moment under the renormalized motor (D3), so C_k is that moment minus a term in λ − 1 alone, and C's point and plane outputs are S minus terms in λ − 1 alone, where S does not see h (D1). C behaves like A on the renormalized motor, plus an error first order in λ − 1. On a line under a drifted motor that is an advantage: C's image is a true line when λ = 1, whatever h is.
- **C's sensitivity is to λ − 1, first order.** Per unit of λ − 1 the output moves by p (point), d and k + T × d − T × R̂ d (line), and n and g_w − n · T + R̂ n · T (plane). Norm bounds, from |R̂ w| = |w|: |S − C| = |λ − 1| |p| for a point; |S_k − C_k| ≤ |λ − 1| (|k| + 2 |T| |d|) + 2 |h| |d|; |S_w − C_w| ≤ |λ − 1| (|g_w| + 2 |T| |n|). On every case of `drift.py` the observed |S − C| was at most 0.99 of these norm bounds (`drift.md`, last table).
- **Euclidean reading for a point.** Let x̄ = u/w and x̄' = R̂ x̄ + T/λ, the image under the motion the motor represents. C's point is (λ R̂ u + w T − (λ − 1) u, w), whose Euclidean point minus x̄' is λ R̂ x̄ + T − (λ − 1) x̄ − R̂ x̄ − T/λ = (λ − 1)(R̂ x̄ + T/λ − x̄) = **(λ − 1)(x̄' − x̄)**: C misplaces a point by λ − 1 times the distance the motion moves it. Points the motion moves far, at the end of a long lever arm, are misplaced most.
- **C cannot replace `transform`.** Off λ = 1, C is not the sandwich, so every exact test with a non-unit motor (the rga3 tests' Motor{1, ..., 8}, and `transform`'s documented "a versor with λ = 2 doubles the weight of a point") would change value. C is a different operation with a named assumption (C8).

The total error of C against the sandwich of the stored motor is then

    |ŷ_C − S| ≤ γ_(K_C) M_C + |C − S|,

the first term C's own rounding (section 3) and the second the table above. For a correctly rounded unit motor |λ − 1| ≤ about 2u and |h| about u, so the second term is a few u times |x|; for a composed chain it grows with the chain (section 7).

A remark for the performance side, not bounded or measured here: since S − C = (λ − 1) p exactly for a point, C plus the correction (λ − 1) p is an exact form of the point's sandwich for any motor, at C's 22 multiplications plus 8 (λ and the correction), against B's 35 and A's 48 (counted by hand, inferred). The same holds for a line's direction and a plane's normal; the moment and the offset have no such cheap correction.

## 6. What a collected plan would do to the rounding check

- **rigid(3), a motor on a point, a line, or a plane.** Every coefficient `transform` drops is the zero polynomial (`derive_bounds.py` section 6; part B's R10 recheck). An exact collection over both stages therefore leaves no term on any dropped blade: nothing to drop, no K, no tolerance, no check. Checking mode's extra work in rigid(3), 68, 84, 76 multiplications against 48, 72, 48 (`../counts.md`), would go, and the guarantee that nothing is lost would be structural (an exact identity the generator proves) rather than numerical. Verified: the polynomials are zero. Inferred: that a collection pass emits no terms for them, which depends on its design (it must collect across the two stages, as form B does). The flector layouts were not checked here.
- **Where the dropped part is not identically zero** (conformal(3), research note 0022's flat motor, dropped antiscalar 2h x4): the magnitude is unchanged, since note 0022's F3 gives A's magnitude as |x4| T and the collected 2h x4 has the same |x4| T (no opposite-sign like terms). K falls: note 0022 gives K = 9 for A, and a collected entry 2h (four products, a chain of four, a doubling) times x4 has K = 6 under the core's rule and 5 with exact doubling. The tolerance would fall from γ_10 to γ_7 (or γ_6), so the check would pass only when |2h| ≲ 7u T instead of about 10u T. **A collected plan makes the check stricter on drifted conformal motors**, so note 0022's firing rates would rise. Inferred from the structure, not computed.
- **The machinery.** `transform_dropped_roundings`, `transform_dropped_tolerances`, and `padded_magnitude` run over the current kernels and would have to run over the collected program instead. Lemmas A to C of decision 0180 are stated for any straight-line program and still hold. A coefficient of 2 would be charged a rounding by the core's rule, which is sound and loose by one rounding; a doubling, exact also below the normal range, could be charged none.

## 7. Empirical results

`drift.py`, seed 20261110, 21-bit dyadic stereographic step motors with translations up to 2 per coordinate; N = 1 is 1,000 correctly rounded unit motors, N = 10, 100, 1000 are 300, 100, and 30 chains, each step correctly rounded and composed in the scalar by the library's `compose` (bit for bit). Each motor moves one point, one line, and one plane. Every comparison is in exact rationals (python-flint 0.9.0 `fmpq`; normalized images in 200-bit mpmath). Full tables: `drift.md`; with the small-denominator sampler: `drift-small.md`.

**Drift of the motors** (median / max over the chains; T here is 2 Σ|r_i||b_i|, research note 0022's componentwise scale):

| Scalar, N | \|λ − 1\| / u | the steps' Π λ_k − 1, / u | \|2h\| / (u T) |
| --- | ---: | ---: | ---: |
| double, 1 | 0.40 / 1.3 | 0.40 / 1.3 | 0.23 / 1.0 |
| double, 10 | 2.6 / 11.6 | 1.1 / 5.6 | 1.6 / 17.9 |
| double, 100 | 6.8 / 31 | 4.0 / 13.4 | 5.8 / 37.6 |
| double, 1000 | 13.8 / 70.5 | 10.9 / 42 | 16.6 / 127 |
| float, 1 | 0.39 / 1.3 | 0.39 / 1.3 | 0.25 / 1.1 |
| float, 10 | 2.2 / 9.4 | 1.2 / 5.3 | 2.0 / 24.3 |
| float, 100 | 6.0 / 29.2 | 2.5 / 15.7 | 5.0 / 73.6 |
| float, 1000 | 19 / 69.8 | 9.9 / 43.2 | 11.9 / 38.4 |

With the small-denominator sampler the steps' own λ drifts one way: the median |λ − 1| at N = 1000 is 36u in double and 128u in float, of which the steps' Π λ_k accounts for 33u and 123u (`drift-small.md`).

**The bounds hold.** Over 20,020 output coefficients per form and scalar in the main run, and as many in the second sampler's, no rounding error exceeded its bound γ_K M (exact-doubling K). The largest ratio of error to bound was 0.50 for A and B in the main run (0.62 in the second sampler's), and 0.39 (0.38) for C. A run exits 1 if any ratio exceeds 1.

**Against the sandwich of the stored motor** (Galoot's definition of the result), in units of form A's bound γ_(K_A) M_A, max (median):

| Scalar, N | point A | point B | point C | line A | line B | line C | plane A | plane B | plane C |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| double, 1 | 0.50 (0.029) | 0.39 (0.027) | 0.30 (0.032) | 0.27 (0.025) | 0.32 (0.026) | 0.46 (0.035) | 0.31 (0.035) | 0.30 (0.037) | 0.58 (0.047) |
| double, 10 | 0.41 (0.034) | 0.40 (0.033) | 2.3 (0.055) | 0.27 (0.026) | 0.29 (0.029) | 1.3 (0.090) | 0.30 (0.037) | 0.25 (0.037) | 1.1 (0.12) |
| double, 100 | 0.38 (0.039) | 0.50 (0.039) | 6.2 (0.071) | 0.27 (0.027) | 0.21 (0.030) | 2.6 (0.30) | 0.24 (0.037) | 0.22 (0.037) | 2.6 (0.35) |
| double, 1000 | 0.40 (0.044) | 0.33 (0.045) | 14.1 (0.070) | 0.22 (0.027) | 0.24 (0.037) | 8.4 (0.65) | 0.25 (0.041) | 0.25 (0.040) | 7.1 (0.81) |
| float, 1000 | 0.27 (0.052) | 0.38 (0.045) | 14.0 (0.076) | 0.26 (0.030) | 0.38 (0.030) | 8.9 (0.83) | 0.24 (0.046) | 0.17 (0.035) | 8.9 (1.1) |

(float at N = 1, 10, 100 in `drift.md`; they follow double's pattern.) A and B are their own rounding only; B's median is 0.77 to 1.39 times A's, a little larger more often than not, although B's bound is smaller. C adds |C − S|, which passes A's whole bound from N = 10 on and reaches 14 times it by N = 1000; its median on lines and planes reaches 0.65 to 1.1 of A's bound, against 0.03 to 0.04 for A. With the second sampler's biased λ, C reaches 16.8 (double) and 38.2 (float) times A's bound at N = 1000. The point's median stays small because the position coordinates' M is large beside (λ − 1) |u_i|; the largest values are on the weight, where C's error (λ − 1) w meets A's small bound γ_5 λ |w|.

**Against the intended motion** (the exact product of the unrounded steps, an exact unit versor), geometric error: each image scaled to its unit (a point to weight 1, a line to a unit direction, a plane to a unit normal), relative to the image's norm, in units of u, median (max):

| Scalar, N | point A | point B | point C | line A | line B | line C | plane A | plane B | plane C |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| double, 1 | 1.1 (4.2) | 1.0 (5.4) | 1.1 (6.4) | 1.2 (3.9) | 1.3 (4.4) | 1.4 (8.0) | 0.92 (3.6) | 1.0 (6.0) | 1.1 (7.3) |
| double, 10 | 3.0 (11) | 3.2 (12) | 3.9 (20) | 4.0 (29) | 4.0 (23) | 5.0 (35) | 3.1 (27) | 3.0 (30) | 3.8 (37) |
| double, 100 | 9.1 (37) | 9.0 (37) | 11.2 (36) | 12.7 (57) | 12.5 (59) | 17.2 (73) | 11.1 (241) | 10.5 (243) | 15.0 (216) |
| double, 1000 | 26 (94) | 26 (94) | 32 (96) | 39 (164) | 39 (166) | 44 (190) | 29 (361) | 29 (361) | 45 (396) |
| float, 1000 | 28 (70) | 27 (68) | 37 (62) | 37 (335) | 38 (333) | 49 (285) | 34 (715) | 33 (720) | 55 (1,640) |

Here the chain's own error, from rounding the steps and composing them, dominates every form: A and B are indistinguishable, and C's median is 1.1 to 1.6 times theirs at N = 1000. With the second sampler's biased λ, C's median at N = 1000 is 1.5 to 2.8 times A's in double and 3.3 to 6.3 times in float (`drift-small.md`). The largest plane errors are offsets of planes nearly through the origin of a long translation, where the normal's error is multiplied by the lever arm, for every form alike.

**Other observations.**

- A and B give different values in 57% to 73% of output coefficients (57% to 77% in the second sampler's run): a collected `transform` would not reproduce the current bits.
- The line moment's change under A and B if h were removed, 2h R̂ d, reached 2.6 times A's bound at N = 1000 in double (median 0.25), 1.3 in float; with the second sampler 5.9 and 5.1.
- Form C's |C − S| stayed within its norm bound of section 5 in every case (largest ratio 0.999).
- λ − 1 grows along a chain more slowly than linearly with the generic sampler, and nearly linearly with the biased one; no law is derived.

## 8. What a collected plan does to tests

Verified means I read the test; inferred means from its description or its use of a name.

| Test | What changes | Status |
| --- | --- | --- |
| `test/bench/reference_kernels_test.cpp` (on `feature/cost-evidence`) | It requires equal bits between `transform` and the form-A reference kernels over double. A collected `transform` differs from form A in 57% to 77% of coefficients (section 7), so the reference kernels would have to be regenerated from B. | verified (read); share measured on the emulation |
| `rga3.op_counts_checking`, `rga3.op_counts_unchecked` (`test/presets/rga3/op_counts_test.cpp`, `golden.py`, same branch) | Counts of form A (48, 72, 48 multiplications and the checking-on 68, 84, 76) become B's (35, 53, 35, and no checking-mode extra, section 6). | verified (header read); new counts from `../counts.md` |
| `test/core/transform_test.cpp`, "the transform's kernels keep the terms that reach x's blades" | Pins the sizes of `first_kernel` and `second_kernel`, kept and dropped. | verified (read) |
| `test/core/transform_rounding_test.cpp` | Calls `detail::transform_middle`, `transform_outer`, `transform_dropped_roundings`, and `transform_dropped_tolerances` and compares K and the tolerance with `tracked`'s run of the same kernels. The rigid(3) families (motor and flector on point, line, plane) would have nothing dropped; the conformal(3) and other families' K would change with the program, and `tracked` would follow if the test is ported to the new internals. | verified (read); consequences inferred |
| `test/core/identities_rounding_test.cpp` | Its tolerances come from `tracked` running the same computation, so they follow a new program; no expected value is a literal. | inferred: no change of expectations |
| rga3 preset tests (`objects`, `motions`, `chains`, `point_groups`) | Exact inputs (the test rational, or dyadic doubles whose products do not round): B is an exact identity, so values are unchanged; a zero's sign may differ, which `==` ignores. Bit comparisons in `motions_test.cpp` are of motors built by `rotation`, not of transforms. | verified for the bit comparisons (read); the rest inferred |

A form C as `transform` itself would change the exact values of every test with a non-unit motor (section 5); it can only be a separately named operation.

## 9. Facts for research note 0027

1. In rigid(3), collection (form B) changes no magnitude of a kept output of transform(point, line, plane) by a motor: the like terms it combines always have the same sign (Lemma 1, verified for all 14 outputs). It lowers K from 12 to 7 or 8 on a point's coordinates, 14 to 11 (10 with exact doubling) on a line's moment, and 12 to 9 (8) on a plane's offset, and leaves the others nearly unchanged; its bound is uniformly at most A's.
2. In practice the accuracy of A and B is the same: on 1,000 rounded motors and 430 chains in each of double and float, under two samplers, both stayed within 0.62 of their bounds, B's median error against the sandwich is 0.77 to 1.39 times A's (the smaller K does not make B's typical error smaller), and against the intended motion they are indistinguishable (section 7). Collection buys operations, not accuracy.
3. A collected `transform` gives different values from the current one in 57% to 77% of coefficients, so any test that pins bits of a transform over double changes (section 8).
4. In rigid(3) a collected plan has nothing to check: every dropped coefficient is the zero polynomial. Checking mode's cost there would vanish. In conformal(3) the check gets stricter (K from 9 to about 6 on the flat motor's antiscalar, inferred), so drifted chains would trip it more often.
5. On a drifted motor, A and B are exact images under the motion the motor represents for points and planes (they do not see h, and λ only scales), and put a line's moment off by 2 h R̂ d (pitch 2h/λ).
6. Form C's error against the sandwich is first order in λ − 1 and independent of h: S − C = (λ − 1) p for a point, and the table of section 5 for lines and planes. A point is misplaced by (λ − 1) times its displacement.
7. On composed chains λ − 1 grows with the chain (section 7), so form C's extra error against the sandwich grows from below A's rounding bound at N = 1 to 2.3 times it at N = 10 and 14 times at N = 1000 (38 times in float with steps whose rounding is biased). Against the intended motion, where the chain's own error dominates, C's median geometric error at N = 1000 is 1.1 to 1.6 times A's with unbiased steps and up to 6.3 times in float with biased ones. A form C needs either λ = 1 restored (a unitization) or the correction of section 5 for points, directions, and normals, and the assumption named in its interface (C8).
8. Form C's own rounding bound is up to about 4 times A's (the plane's offset; section 3), from a larger magnitude, while its observed rounding error is similar to A's.

## Reproduction

From a checkout root, with the math-tools Python environment (`<env>` = `/home/brianm/projects/utils/math-core/math-tools/toolset/env/py/.venv`; sympy 1.14.0, python-flint 0.9.0, mpmath as sympy's):

    <env>/bin/python -B docs/agent/records/spikes/spike-e/bounds/derive_bounds.py    # about 1 s
    <env>/bin/python -B docs/agent/records/spikes/spike-e/bounds/drift.py            # about 3 min
    <env>/bin/python -B docs/agent/records/spikes/spike-e/bounds/drift.py --small-steps
    tools/timed --log docs/agent/records/spikes/spike-e/bounds/runs.tsv probe - spike-e-bounds:probe-build -- \
      g++ -std=c++23 -O2 -ffp-contract=off -DGALOOT_CHECKING=0 -Iinclude \
      docs/agent/records/spikes/spike-e/bounds/probe.cpp -o <scratch>/probe
    <env>/bin/python -B docs/agent/records/spikes/spike-e/bounds/replay.py <scratch>/probe

Each exits 1 on a failed check. Outputs: `derive_bounds.txt`, `drift.md`, `drift.txt`, `drift.json`, the same with `-small`, `replay.txt`. The probe was built with GCC 16.2.1. None of the Python runs went through `tools/timed`; they are seconds to minutes of single-threaded Python.

| File | What |
| --- | --- |
| `fp.py` | the forms as programs, K and M, evaluation in exact, double, and emulated float arithmetic, `compose` as the library accumulates it |
| `derive_bounds.py` | K and M of each form, Lemmas 1 and 2, the identities D1 to D3 and of form C, the dropped coefficients |
| `drift.py` | rounded motors and chains, observed errors against the bounds and against the sandwich and the intended motion |
| `replay.py`, `probe.cpp` | the emulation of form A and `compose` against the library, bit for bit, and the library's dropped K |

## What remains unverified

- The conformal(3) consequences of section 6 (inferred).
- The batch correlation of B's errors (derived, not measured).
- Worst cases: no adversarial inputs; the ratios are sampled (V9). The M_C / M_A maxima are over 200 samples.
- The growth laws of λ − 1 and h along a chain are not derived; the tables give samples. With the small-denominator sampler, λ − 1 is dominated by a systematic rounding bias of the steps (`drift-small.md`), as in the knowledge base's note on rotor drift.
- The flector layouts and odd versors.
- The cost of the corrected form C of section 5 (counted by hand only).

## Candidate knowledge-base notes (not written; the brief limits writes to this directory)

- For a two-stage sandwich in a degenerate algebra, the expanded (collected) form can have the same magnitude as the two-stage form, so collection changes only the count of roundings; this updates the `[untested]` remark in `numerical-analysis/error-analysis/multivector-product-error.md`.
- The unit-constraint form of a rigid motion differs from the sandwich by (λ − 1) times the object (and, for the moment, terms in λ − 1 and h), so its error on drifted motors is first order in the norm defect and misplaces a point by (λ − 1) times its displacement; a rationals-based sampler whose values round the same way induces a systematic norm drift in long chains.
