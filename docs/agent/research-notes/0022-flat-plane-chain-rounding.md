# 0022 Why composed flat-plane motors trip transform's rounding check

- Date: 2026-11-07
- Author: Claude (agent, Opus 5.5), in the role of numerical-expert (researcher), at the coordinator's request after decision 0195 item 1 (question O6: find the cause before M5; documenting the limit is the fallback; the bound is not loosened until it is understood).
- Branch: `records/o6-flat-plane-rounding`, from `develop` at `d68881f`. The headers are read, not changed; no test is changed.
- Probes and outputs: `docs/agent/records/spikes/flat-plane-chain-rounding/` (its README gives every command). Every number below comes from a file in its `out/` directory.
- Question: chains of conformal(3) motors, each the composition of two random flat planes, composed in double, trip `transform`'s rounding check (decision 0180) in 0.2% of cases at 2 motors, 2.2% at 8, 11.4% at 64, and 22.7% at 128. Correctly rounded versors and chains of general hyperplanes never fire. Is the bound wrong, loose in the wrong way, or is the input outside the bound's hypothesis?

## Answer

The input is outside the hypothesis, and the bound is right. The composed motor is no longer a versor: its exact V̰ ⟇ V has a nonzero e5 part, and that part, times x's e4 coefficient, is exactly the dropped antiscalar the check sees. The transform's own rounding stays within its bound with room to spare.

1. **No defect of the bound.** In all 14,000 chains of the review's experiment, recomputed exactly by an algebra that shares no code with the library, the transform's own rounding |d − e| is at most 0.295 of the tolerance (flat planes) and 0.071 (general hyperplanes). In all 465 cases that fire, the exact dropped part e is at least 0.957 of the tolerance (section 2).
2. **What the check measures on a flat motor.** For any V on the layout of these chains, V̰ ⟇ V = λ𝟙 + 2h e5, and the dropped part of V ⟇ x ⟇ V̰ for a vector x is 2h x4 on 𝟙 and zero on every trivector, as a polynomial identity. The library's tolerance is γ_10 |x4| T, with T the same expression on absolute values, to within 2 ulp. So the check does not depend on x: it passes when |2h| ≤ γ_10 T, about 10u T. In the oriented coordinates of a rigid motor (weight v, bulk m), h = v · m, the motor constraint, and T = 2 Σ|v_i||m_i|. The check asks that v · m be zero to about ten roundings of its own condition (section 3).
3. **Why correctly rounded versors pass.** A versor whose coefficients each carry a relative error of at most u has |2h| ≤ (2u + u²) T. That is about 0.2 of the tolerance (section 4).
4. **Why composed chains fail, more often as they grow.** First, compose rounds each coefficient relative to the terms it sums, not relative to the coefficient. Its error is normwise in V, and the check is componentwise. Second, the defect τ = 2h/λ is additive under composition, exactly: τ(m ⟇ V) = τ(m) + τ(V). Old defects are never forgotten. Each composition adds about u |A|/|B|, where |A|/|B| is the ratio of the chain's translation part to its rotation part. That ratio random-walks, growing like √N. So the RMS of τ grows linearly in N, from 1.8u at 2 motors to 93.6u at 128. The tolerance in the same units, T/λ, grows only like √N, from 1.35 to 14. The median of |2h|/(uT) rises from 0.68 to 4.45, and its tail crosses the threshold of about 10 more and more often (section 5).
5. **Why general hyperplane chains never fire.** The magnitude of their dropped antiscalar is dense: 40 terms over five components of x. Its tolerance is about 13u |V|²|x|₁ (median), while their defect stays near 1.3u |V|². And the chains forget their defects: an old defect shrinks relative to |V|² by a factor of about 4 per composition, because random products of spheres and planes grow hyperbolically. The check's silence there says nothing about accuracy, though. By 32 to 64 motors, λ/|V|² is at the rounding level, and λ's sign is a coin toss (section 6).
6. **Recommendation: a renormalization helper** for flat conformal(3) motors and rigid(3) motors (the same formula), m ← m − (v · m / |v|²) v. It costs 12 multiplications, 10 additions, and one division. Applied once to the finished chain, or after every composition, it left 0 firings in 7,000 chains of the review's draws and 0 in 7,000 other chains. The largest ratio left, 0.295, is the transform's own rounding. The exact residual |2h'|/(uT') is at most 1.56, against about 10 at the threshold. Keep the bound as it is. Until the helper exists, sharpen the comment of `transform` (section 9). Section 7 weighs the other options.

## Terms

u = 2^−53 is the unit roundoff of double, and γ_k = ku/(1 − ku). Blades are written by their generators in increasing order, each with orientation +1 (e125 = e1 ∧ e2 ∧ e5), as `compose_layout` stores them; e4 is the origin and e5 the point at infinity, both null (CONVENTIONS 1.4). The versor product is the geometric antiproduct ⟇, and V̰ is the antireverse. For a versor, V̰ ⟇ V = λ𝟙 with λ ≠ 0. The defect of V is the rest of V̰ ⟇ V.

The chains of the review probe (`drift.cpp`) are on the layout `compose` gives, e5, e125, e135, e145, e235, e245, e345, e12345. Below, their coefficients are q, a1, a2, a3, b1, b2, b3, s in the order e5, e125, e135, e235, e145, e245, e345, e12345. This is a rigid(3) motor wedged with e5 (CONVENTIONS 6.2). In Lengyel's oriented names, its weight v (the rotation) is on e415, e425, e435, e12345, and its bulk m (the translation) is on e235, e315, e125, e5. Hence b1 = −v_x, b2 = −v_y, b3 = −v_z, s = v_w, a3 = m_x, a2 = −m_y, a1 = m_z, q = m_w. Write A = (a1, a2, a3, q) and B = (b1, b2, b3, s), so that |A|/|B| compares translation with rotation.

For one case: d is the dropped coefficient the library computes, e its exact value from the stored V and x, and tol its tolerance. The check fires when |d| > tol.

## 1. The experiment, reproduced

`chains.cpp chains 20261107` draws exactly as `drift.cpp` does, built with GCC. It reproduces the review's counts per 1,000 at the current headers: 2, 4, 22, 36, 60, 114, and 227 at N = 2, 4, 8, 16, 32, 64, and 128 motors for flat planes, and 0 at every N for general hyperplanes (`out/analyze.txt`). The Clang build evaluates the function arguments in the other order. It therefore draws other chains, which serve as a second sample: 2, 7, 20, 29, 63, 121, and 205 (`out/renorm-clang.tsv`).

Layouts and counts, from the library (`chains.cpp`): the dropped blades of a vector under the flat chain are the ten trivectors and 𝟙, and the antiscalar's count of roundings is K = 9, so its tolerance is fl(fl(γ_10) P̂). Under the 16-blade chain of general hyperplanes, K = 21 for every dropped blade.

## 2. Is the bound wrong? No (part ii)

`analyze.py` reads every stored V and x as exact dyadic rationals. It recomputes V ⟇ x ⟇ V̰ with `cga.py`, which takes geometric products from `test/reference/clifford_reference.py` (a diagonalization over fractions) and builds the antiproduct from the complements, a ⟇ b = left(right(a) ⟑ right(b)) (CONVENTIONS 4.2). The library takes neither route.

| Family | Cases | Fired | |e| > tol | Largest |d|/tol | Largest |e|/tol | Largest |d − e|/tol |
| --- | --- | --- | --- | --- | --- | --- |
| flat planes, N = 2 to 128 | 7,000 | 465 | 465 | 14.6 | 14.6 | 0.295 |
| general hyperplanes, N = 2 to 128 | 7,000 | 0 | 0 | 0.475 | 0.511 | 0.071 |

(Per N in `out/analyze.txt`.) Lemma A of decision 0180 bounds |d − e| by γ_9 P. That holds in every case, and the bound is used to at most 0.295 of the tolerance. Every case that fires has |e|/tol ≥ 0.957, and every one fires on the antiscalar e12345. The firing and the drift tallies differ in 20 cases, all with |e|/tol between 0.957 and 1.04. There the transform's rounding, at most 0.243 of the tolerance in those cases, tips a defect lying at the edge. So the firing is caused by V and not by this operation.

## 3. What the check measures on a flat motor (part i)

`identity.py` proves the following as polynomial identities in the coefficients, with SymPy over `cga.py` (`out/identity.txt`):

- **F1.** V̰ ⟇ V = V ⟇ V̰ = λ𝟙 + 2h e5, with λ = b1² + b2² + b3² + s² and h = −a1 b3 + a2 b2 − a3 b1 + q s. V is a versor exactly when h = 0.
- **F2.** For x = x1 e1 + … + x5 e5, the part of V ⟇ x ⟇ V̰ off the vectors is 2h x4 on 𝟙. Every trivector coefficient is identically zero.
- **F3.** Summed over the transform's two stages, with absolute values of coefficients and factors, the magnitude of the antiscalar is |x4| T, where T = 2(|a1 b3| + |a2 b2| + |a3 b1| + |q s|).
- **F6.** In the oriented coordinates, h = v_x m_x + v_y m_y + v_z m_z + v_w m_w = v · m and λ = |v|², so T = 2 Σ|v_i||m_i|.

`analyze.py` also checks the library against these identities. In all 7,000 flat cases, e equals 2h x4 exactly, every trivector e is exactly zero, and the library's tolerance of the antiscalar divided by γ_10 |x4| T lies between 1 − 4.4 × 10⁻¹⁶ and 1 + 2.2 × 10⁻¹⁶. The padding is negligible there.

So for every x with x4 ≠ 0, the check on a flat motor is the test

    |2h| ≲ γ_10 T,  that is,  |v · m| ≲ 10u Σ|v_i||m_i|,

and nothing else. Σ|v_i||m_i| is the condition-weighted size of the dot product v · m. The check therefore asks the motor constraint to hold to about ten roundings, relative to its own terms: a componentwise test of V. The review's guess in `records/spikes/transform-rounding/README.md` was partly right. The defect does live on the null blade e5, but it is not hidden from the sandwich: it reaches the antiscalar through x's e4 coefficient, the origin part that every round point has.

## 4. Correctly rounded versors

Let V* be an exact flat motor, h(V*) = 0, and V = V*(1 + η_i) coefficient by coefficient, with |η_i| ≤ η. Then 2h(V) is a sum of the four products of 2h(V*), each multiplied by (1 + η_i)(1 + η_j) − 1, so

    |2h(V)| ≤ (2η + η²) T(V*) ≤ (2η + η²) / (1 − η)² · T(V).

For η = u, this is about 2u T, or 0.2 of γ_10 T. That is why the review's 60,000 versors, computed exactly and rounded once per coefficient, never fired, with a largest ratio of 0.30, which includes the transform's own rounding. For the motor of two planes, K = 7, so the share is 2/8. The same argument holds for any layout and any dropped coefficient: a versor with componentwise relative error η leaves |e| ≤ (2η + η²)(1 − η)⁻² M, with M the unpadded magnitude.

This is not a worst-case guarantee. The check's margin is one rounding: tol is at least γ_9 P, but not provably more than γ_9 P + 2u M. So a correctly rounded versor whose transform rounds in the worst way could fire. In every sample so far, the transform's rounding has used less than 0.3 of the tolerance. The comment's "never fired" is a sampled claim (V9), and it should stay worded as one.

## 5. Composed chains: why the defect builds up

Compose computes each coefficient as a sum of products and rounds it with an error of about u times the sum of the absolute values of its terms. Relative to the coefficient itself, that error is unbounded where the sum cancels. The renormalization below shows this: it changes some coefficients by up to 2.4 × 10⁶ u of their own size, but never by more than 30u of the largest coefficient (`out/renorm-check.txt`). A composed V is thus close to a versor in norm, not coefficient by coefficient. Section 4 does not apply to it.

The defect then accumulates, because it is additive:

- **F4.** V̰ ⟇ e5 ⟇ V = λ e5 for every V on the layout. The layout is closed under ⟇, and W(m ⟇ V) = λ_m λ_V 𝟙 + (λ_m 2h_V + λ_V 2h_m) e5. Hence τ = 2h/λ satisfies τ(m ⟇ V) = τ(m) + τ(V) for the exact product.

So for a chain, τ(V_N) = Σ τ(m_k) + Σ ℓ_k exactly. Here m_k is the stored motor of step k, whose own defect comes from composing two exact planes in double. ℓ_k = τ(V_k) − τ(V_(k−1)) − τ(m_k) is what rounding the k-th composition adds, and scaling by a power of two changes nothing. `steps.py` checks the sum exactly at every step of 200 chains of 128 motors (`out/steps.txt`):

| Motors | RMS τ / u | RMS Σ τ(m_k) / u | RMS Σ ℓ_k / u | RMS ℓ_N / u | median \|A\|/\|B\| | median T/λ | median \|2h\|/(uT) | 99th pct | share > γ_10 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 2 | 1.81 | 1.06 | 1.49 | 1.49 | 1.24 | 1.35 | 0.68 | 11.1 | 1.0% |
| 8 | 6.35 | 2.11 | 6.24 | 4.01 | 2.47 | 2.99 | 1.20 | 9.1 | 0.5% |
| 32 | 26.0 | 4.19 | 25.5 | 6.23 | 5.02 | 6.34 | 2.66 | 41.8 | 8.5% |
| 128 | 93.6 | 8.49 | 93.3 | 13.5 | 11.1 | 14.0 | 4.45 | 47 | 17.5% |

Reading it:

- The motors' own defects add up like a random walk (√N), but the rounding of the compositions dominates, and its RMS grows linearly in N.
- Each step adds ℓ_k of about u |A_k|/|B_k| (the RMS of ℓ_N tracks the median |A|/|B|). Rounding perturbs A and B by about u|m||V|, and 2h is bilinear in A and B, so ℓ = Δ(2h)/λ is about u|A||B|/|B|² (inferred, and consistent with the table). The chain's translation random-walks while its rotation stays bounded, so |A|/|B| grows like √N. The sum of N steps of random sign and size about u√k then has RMS about u N/√2.
- The check compares |τ| with γ_10 T/λ, and T/λ is about |A|/|B| times an alignment factor of order 1. That factor, T/(2|A||B|), stays stationary, with median 0.65 to 0.67 at every N (`out/analyze.txt`), so T/λ grows only like √N.
- The ratio the check sees therefore grows like √N. Its tail beyond 10 is the firing rate. The 200-chain sample gives 17.5% at 128 against the 22.7% of the review's 1,000 chains.
- The defect is carried forward, not damped. Continuing 20 chains exactly from their stored V_63 with exact motors, so that no new defect enters, leaves |2h|/T at a median of 0.51 of its starting value after 64 more motors. The factor comes from T growing, while τ stays exactly unchanged.

A single motor of two planes fires too (0.11% in the WU13b spike), with no accumulation. There the normwise rounding of one composition meets an unlucky alignment: the median alignment factor of the 2 cases that fire at N = 2 is 0.10, against 0.67 overall. As N grows, the cases that fire have ordinary alignment (median 0.44 at 128) and a large defect instead.

## 6. General hyperplane chains: why they never fire

- **H1** (`identity.py`). For every V on all 16 blades of odd grade, V̰ ⟇ V = λ𝟙 + w with w a vector. The trivector part of V ⟇ x ⟇ V̰ is zero, and its antiscalar is ⟨x ⟇ (V̰ ⟇ V)⟩_𝟙, linear in w and in all five components of x. The check again tests the defect, but now against a dense magnitude with 8 products per component of x.
- The dense magnitude leaves room. The tolerance of the antiscalar, over |V|²|x|₁, has a median of 12.2u to 13u and a 1st percentile of 3.6u at 2 motors and 4.5u to 5.3u from 4 on. The defect, as its largest coefficient over |V|², has a median of 0.66u to 0.94u and a 99th percentile of at most 5u (`out/analyze.txt`). As the Euclidean |w|/|V|² it has a median of 0.9u to 1.3u and a 99th percentile below 9u (`out/steps.txt`). For flat chains the same tolerance has a median of 0.96u at 2 motors, falling to 0.21u at 128, and a 1st percentile of 0.004u to 0.023u. The largest ratio over the 7,000 hyperplane chains is 0.475.
- The defect does not accumulate. W(m ⟇ V) = λ_m W(V) + V̰ ⟇ w_m ⟇ V. A product of random spheres and planes is a random walk in a group whose products grow hyperbolically: |V|²/|λ| grows exponentially. Relative to |V|², an old defect is multiplied by about |λ_m|/|m|² per step (inferred; the continuation below measures it). Continued exactly, the relative defect of V_63 falls to 0.27 after one step, 0.012 after 4, and 4 × 10⁻³² after 64. Only the last step or two of rounding matter.
- The check's silence is not accuracy. Under that same growth, |λ|/|V|² falls by about 2^−1.6 per motor, from 2^−3 at 2 motors to 2^−52 at 32. From there it sits at the rounding level, near 2^−54. At 64 and 128 motors, the stored V's λ has the sign of the exact product of its motors' λ in only 44% and 47% of chains. The versor's norm has lost every digit, and the check, which bounds only what `transform` drops, cannot see it and is not meant to. What this does to the kept part of a transform was not measured.

So the flat case is not the exception because its arithmetic is worse. Its check is componentwise and sharp, and its group keeps every defect. The hyperplane case has a loose check and a group that forgets its defects.

## 7. Options weighed

**(a) Document the limit, nothing else.** The comment of `transform` already states the limit, the rates, and the remedy "rebuild or renormalize v". It does not say how to renormalize, and a user has no tool to do it. With the cause now known, the comment can say exactly what the check tests on a motor and how to restore the versor (section 9). This is necessary in any case, but not sufficient for users who compose long chains in checking builds.

**(b) A different bound for composed inputs.** The check cannot know how V was made. Two forms are possible:

- *Componentwise input error.* If V is within relative η = cu of a versor coefficient by coefficient, section 4 gives |e| ≤ ρ u M with ρ = (2c + c²u)/(1 − cu)². The check then needs (1 − u)^(2K+1) γ_(K+m) ≥ γ_K + ρu. Since γ_(K+m) − γ_K ≥ mu, and the (1 − u)^(2K+1) factor costs O(K² u²), a margin of m = ⌈ρ⌉ + 1 suffices while K²u is far below 1. The exact statement needs a lemma like decision 0180's Lemma C, not written here. For correctly rounded versors, ρ is about 2 and m = 3. This would make section 4's "never fires" a theorem, and it would loosen the tolerance by (K + 3)/(K + 1), 1.2 for K = 9. It does nothing for composed chains, whose componentwise error is unbounded (section 5).
- *Normwise input error.* A term covering a normwise error of V would need to grow with the chain without limit. The ratio |2h|/(uT) reaches 47 at the 99th percentile at 128 motors and 146 at most over the 7,000 chains, and it grows like √N. A margin of 48 instead of 1 would loosen the check about fivefold, still fire for about 1% of chains of 128, and fire more often beyond that. Not recommended.

**(c) A renormalization helper** (recommended). By F5, V ⟇ (𝟙 + α e5) keeps λ and has defect 2(h + αλ), since e5 ⟇ e5 = 0. With α = −h/λ the defect vanishes. Written out (F5, F6), this is q += αs, a1 −= αb3, a2 += αb2, a3 −= αb1, with b and s unchanged; in the oriented coordinates, m ← m − (v · m / |v|²) v. It subtracts from the bulk its component along the weight, which is the dual part of the dual-quaternion normalization without the scaling. `transform` does not need λ = 1. As `renormalize()` in `chains.cpp`, it computes h and λ as four-term sums, one division, and four multiply-add updates, unfused: 12 multiplications, 10 additions, one division.

Measured: applied once to the finished chain, or after every composition, it leaves 0 firings in the 7,000 flat chains of the review's draws (`out/renorm-gcc.tsv`) and 0 in Clang's 7,000 other chains (`out/renorm-clang.tsv`). On the review's draws, the largest ratio at each N, 0.217 to 0.295, equals the largest |d − e|/tol of the raw chains at that N: what is left is the transform's own rounding. On Clang's draws it is 0.21 to 0.28. Exactly, the residual |2h'|/(uT') is at most 1.56 and 0.28 at the median, against a threshold of about 10, while the raw chains reach 146 (`out/renorm-check.txt`).

Its worst-case residual, in the model of decision 0180: the computed ĥ has |ĥ − h| ≤ γ_4 T/2 (a dot product of four terms), λ̂ = λ(1 + θ_4), α̂ = −ĥ(1 + θ_5)/λ, and each updated coefficient is (c_i + σ_i α̂ b_i(1 + δ))(1 + δ'). Summing as in h gives

    |2h'| ≤ [γ_4 (1 + γ_5)(1 + γ_2) + u] T + [γ_5 + γ_2 (1 + γ_5)] |2h|  ≈  5u T + 7u |2h|,

with T' = T(1 + O(u + |2h|/T)). For any V with |2h| up to a few hundred u times T, the residual is about 5u T, about half the threshold. That bound is not a worst-case guarantee against firing: as in section 4, the margin of one rounding leaves no worst-case room for any input defect. With option (b)'s componentwise margin at ρ ≈ 5, that is m = 6, it would be one. The measured residual, at most 1.56u T', is under a third of that bound.

Tradeoffs: it applies to one family of layouts, the rigid(3) motor and its conformal(3) flat counterpart, and it needs λ > 0 (a motor with a nonzero weight). It cannot tell drift from a wrong V: it makes any V on the layout a versor, so it should be called by code that composes motors, not by the check. General conformal(3) versors, products of spheres, have closed-form normalizations for even elements of fewer than six dimensions (De Keninck and Roelfs, arXiv:2206.07496, from the knowledge base's note on rotations, not re-read here). They do not trip the check, so they are not needed for it.

**(d) None.** Leaves checking builds unusable for long motor chains in conformal(3), with no tool for the remedy the message names.

## 8. Recommendation

(c), with (a) now. Keep the bound and its margin unchanged: the bound is sound and tight (largest |d − e|/tol 0.295), and what fires is a true defect of V. Add the projection m ← m − (v · m/|v|²) v as a helper of the motor in the rigid and conformal models (M6), where motors are named types and the oriented coordinates are fixed. One formula serves rigid(3) motors, whose drift `transform` ignores (their dropped parts are zero for every V) but which drift all the same, and conformal(3) flat motors, where the check needs it. A core helper on raw layouts is possible but would have to name the layout's blades. Until the helper exists, update the comment (section 9). Option (b)'s componentwise margin is a separate choice for the owner, if section 4's sampled claim should become a guarantee. It costs 20% of the check's sharpness at K = 9, and it is not needed for the chains.

## 9. Proposed text for the comment of transform (not applied)

In the paragraph "The bound takes v and x as exact", after "the check aborts.", and in the matching cause at `check_transform_rounded`:

> For a conformal(3) motor, a rigid(3) motor wedged with e5, with weight v on e415, e425, e435, e12345 and bulk m on e235, e315, e125, e5, the dropped antiscalar is 2(v · m) times x's coefficient on e4. The check passes when |v · m| is below about 10u times |v_x m_x| + |v_y m_y| + |v_z m_z| + |v_w m_w|, whatever x is. Composing motors in floating point adds to v · m / |v|² at every step, by about u times the ratio of translation to rotation, so the defect grows with the chain: [the rates]. m ← m − (v · m / |v|²) v removes it and changes the motor by no more than its defect.

## What remains unverified

- The rates are sampled (V9): 14,000 chains of one seed, 7,000 more from Clang's draws, and 200 traced chains of 128 motors. The √N and linear growth laws are fitted by eye to RMS values and medians, not derived as limits.
- The worst-case residual of the helper is derived by hand and not machine-checked. Option (b)'s margin needs a lemma before adoption.
- `renorm_check.py` repeats the helper on Python doubles and was not compared bit for bit with the C++ run. The C++ runs measure the check directly.
- Float chains, chains of flectors (three or more planes), and the effect of the hyperplane chains' lost λ on the kept part of a transform were not measured.
- F3 rebuilds the transform's terms from `cga.py` and not from the library's kernels. The library's tolerance agrees with it to 2 ulp on 7,000 cases.

## Reproduction

See `docs/agent/records/spikes/flat-plane-chain-rounding/README.md`. Builds and runs are rows `probe:o6:*` in `records/performance/runs.tsv`. The two dumps are regenerated bit for bit (SHA-256 in `out/dumps.sha256`), and each analysis takes under 20 s.
