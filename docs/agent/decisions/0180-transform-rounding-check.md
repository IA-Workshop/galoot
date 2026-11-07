# 0180 The rounding check of transform: a padded magnitude, a margin of one rounding, and what it covers

- Status: recorded (plan 0021 reading R9, questions O5 to O7, answered by the owner "proceed with recommendations", decision 0192)
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), implementer of WU13b, numerical-expert role
- Bears on: CONVENTIONS 4.4 and 4.5 (1.10); constitution C8; VVUQ V12, V13; STYLE S9, S15; decisions 0010, 0161, 0175, 0178, 0181; plan 0021 (R9, criteria of WU13b, section 6); research note 0020 and its addendum of 2026-11-07

## Context

CONVENTIONS 4.5 says that an operation that drops part of its own result, such as `transform`, verifies on a rounding scalar that the dropped part is within that operation's forward error bound (decision 0010), for the IEEE 754 binary types that round to nearest. Decision 0178 item 5 left this to WU13b: in checking mode `transform` already computes every dropped coefficient d_j, through the whole first stage and the dropped terms of the second (decision 0178 item 4), and checks d_j = 0 for an exact scalar. This record fixes the bound each d_j is compared with on a rounding scalar, how the comparison is computed so that it never rejects a d_j within the bound, and what the check covers. It was written before any rounding test of `transform` ran (plan 0021 section 4, WU13b step 1).

Plan 0021 R9 sketched the design: the bound γ_K M of decision 0175, with K the count of roundings on the coefficient's path and M the same computation on absolute values; K computed at compile time by the rules of `test_support::tracked`; M computed in checking mode from the absolute values of V and x, not from the computed intermediate; a margin for the check's own arithmetic, at most γ_(2K+2) by the planner's sketch; an absolute term for underflow "of the order of K times denorm_min"; a NaN or infinite dropped part fails (O7).

## Terms

T is the scalar, p its precision, u = 2^−p its unit roundoff (`numeric_limits<T>::epsilon() / 2`; 2^−53 for double, 2^−24 for float), and λ = `numeric_limits<T>::min()`, its smallest positive normal number (2^−1022, 2^−126). γ_k = k u / (1 − k u). fl(z) is z rounded to nearest. For a dropped coefficient j: e_j its exact value from the inputs as given, d_j the value the kernel computes, and K_j its count of roundings.

## The computation that is bounded

A dropped coefficient is computed by two kernels of `kernel_term`s (decision 0178 item 4): the whole first stage, middle_k = Σ c_t v_a x_b over the terms t that reach middle blade k, and the dropped terms of the second stage, d_j = Σ c_t middle_a v_b. Each term runs through `accumulate` in `mv.hpp`: the product of the two operands, rounded; then, when |c| ≠ 1, the product with c, rounded; then assigned, for the first term of its output, or added to or subtracted from it, rounded. A coefficient of ±1 makes no product. So K follows `tracked`'s rules exactly: an input 0; a product the sum of its operands' counts plus 1; a sum or a difference the larger count plus 1; a negation or a copy no change. Per term of the first stage, K = 1, or 2 when |c| ≠ 1; per term of the second, K = K(middle_a) + 1, or + 2 when |c| ≠ 1; and per output, the first term's count, then the larger of the running count and the term's count plus 1 for each later term, in the kernel's order. The core computes K_j this way at compile time.

## Options weighed

**The bound.**
1. γ_K M per dropped coefficient, M from the absolute values of V and x through the same kernels (chosen, as R9). It is the bound the test oracle `tracked` computes (decision 0175), so the tests can check the core's K and its tolerance coefficient by coefficient. Its cost is one more run of the two kernels in checking mode only.
2. A running error bound, u times the sum of the computed values that are rounded (knowledge-base note `running-error-bounds.md`). Often several times tighter, but for a two-stage computation it must carry the first stage's bound through the second, which needs a kernel of its own, and no test oracle computes it. Not chosen: tightness is not what a check that guards against false aborts needs most, and the oracle decides.
3. A norm bound, such as γ_K times the largest |v|² |x| times the number of terms. Cheaper and looser; a dropped part well above rounding could pass. Not chosen.

**Underflow.** With gradual underflow, a product can underflow: fl(a b) = a b (1 + δ) + η with |η| ≤ λu and δη = 0, while a sum or a difference that lands below λ is exact (Higham (2.8), p. 56–57; research note 0020 addendum). The γ_K M bound misses η.
1. An additive term K_j times the smallest subnormal (R9's sketch). Unsound: an η of a first-stage product is multiplied by |v_b| in the second stage, so its effect on d_j scales with V and can exceed any fixed multiple of the smallest subnormal. A probe in exact rationals (two products near 2^−1080 that nearly cancel, then times 2^500) gave an error of 2.3 × 10^−187 against 2.5 × 10^−190 for γ_3 M + 3 · 2^−1074.
2. **A padded magnitude** (chosen): in the computation of M, every rounded product, of two operands or of a coefficient and an operand, adds λ. Each η is then charged to the λ of its own product node, and later factors multiply that λ exactly as they multiply η. Lemma A below shows the bound becomes γ_K P with P the padded magnitude, with no separate absolute term. For inputs of ordinary size the padding is far below the rounding of M itself: P and M agree to about λ / M relative.
3. Leave underflow out and state "no underflow" as a precondition. The check would then abort correct programs whose values are tiny, which is a false abort, the failure decision 0010 warns of.

**The margin.** The comparison is computed in T, so the tolerance it computes must not fall below the true bound γ_K P.
1. γ_(2K+2), the planner's sketch: sound, looser than needed.
2. **γ_(K+1)** (chosen): the smallest integer margin that the derivation admits (Lemma C), given that the computed padded magnitude is at most 2K roundings below the exact one, each on a sum of nonnegative numbers.
3. γ_K: fails, since the computed magnitude and γ can each round down.

**A dropped coefficient that is not finite** (O7, ruled): fails the check. A checking build cannot verify a NaN or an infinity.

**A padded magnitude that overflows.** The bound assumes no overflow. When the computed padded magnitude is infinite, the tolerance is infinite and the bound is not known.
1. **Fail** (chosen), as O7 rules for what a checking build cannot verify. The cost: a program whose values are near the overflow threshold, so that the products of the sandwich overflow on absolute values, aborts in checking mode even where cancellation keeps its true values finite.
2. Pass, since an infinite tolerance admits every finite d_j. That accepts a coefficient the check has not verified.

**Scalars** (O5, ruled; CONVENTIONS 4.5 since 1.10): those whose `numeric_limits` give `is_iec559` and `round_style == round_to_nearest`. `float` and `double` are tested; `long double` meets the condition and gets the check, and is neither tested nor claimed. Any other inexact scalar (`std::complex<double>`, a user type without a `numeric_limits` specialization, the test-only `tracked`) is not checked, and the comment of `transform` says so.

**Options that relax IEEE 754** (STYLE S15). The derivation assumes each operation is rounded once to T, to nearest, with gradual underflow. `-ffast-math` reassociates and, through the startup code it links, flushes subnormals to zero, so the check could then reject a correct result. S15 asks a header whose correctness depends on IEEE 754 to stop compilation when the compiler reports such an option. Options weighed: stop whenever `__FAST_MATH__` is defined, which makes all of `core.hpp` unusable under fast math, though nothing else in it depends on IEEE 754 for correctness; stop only when `__FAST_MATH__` is defined and `GALOOT_CHECKING` is 1, the build in which the check runs at run time (chosen); or only document it. Constant evaluation, where the check always runs, is evaluated by the compiler in IEEE 754 arithmetic whatever the options, and flushing to zero is a run-time mode, so an unchecked build under fast math is left alone. A flush-to-zero mode set by other means, which no macro reports, is outside the model, and the comment says so.

## Derivation

Assumptions: T is an IEEE 754 binary type with rounding to nearest; each operation of the kernels is rounded once to T (no wider intermediate format); underflow is gradual; nothing overflows; V and x are exact as given (the bound covers this operation's roundings only). Sources: Higham, *Accuracy and Stability of Numerical Algorithms*, 2nd ed.: the model (2.8) with |η| ≤ λu, δη = 0, and η = 0 for + and −, pp. 56–57; Lemma 3.1 (γ_k) and Lemma 3.3 ((1 + θ_k)(1 + θ_j) = 1 + θ_(k+j), γ_k + u ≤ γ_(k+1)), pp. 63, 67; monotonicity of IEEE arithmetic, p. 56 (research note 0020 and its addendum).

Every node y of the computation (an input, a coefficient, a product, a sum or difference, a negation, a copy) has a value y, a computed value ŷ, a count K_y, and a padded magnitude P_y, defined exactly, over the reals: an input or a coefficient has P = |y| and K = 0; a negation or a copy keeps both; a sum or difference has P = P_a + P_b and K = max(K_a, K_b) + 1; a product has P = P_a P_b + λ and K = K_a + K_b + 1. Since P ≥ M ≥ |y|, where M is the unpadded magnitude, P_y bounds the value.

**Lemma A.** For every node, |ŷ − y| ≤ γ_(K_y) P_y.

By induction over the nodes. Inputs and coefficients are exact; negation and copying are exact. Write â = a + ε_a with |ε_a| ≤ γ_(K_a) P_a, and the same for b.
- Sum or difference: ŷ = (â ± b̂)(1 + δ) with |δ| ≤ u (no η, since a sum that lands below λ is exact). So |ŷ − y| ≤ u(|a| + |b|) + (1 + u)(|ε_a| + |ε_b|) ≤ (u + (1 + u) γ_(K−1)) (P_a + P_b) ≤ γ_K P_y, since u + (1 + u) γ_(K−1) ≤ γ_K (Lemma 3.3).
- Product: ŷ = â b̂ (1 + δ) + η. Then |â b̂ − a b| ≤ (γ_(K_a) + γ_(K_b) + γ_(K_a) γ_(K_b)) P_a P_b ≤ γ_(K_a+K_b) P_a P_b (Lemma 3.3), so |ŷ − y| ≤ ((1 + u) γ_(K_a+K_b) + u) P_a P_b + λu ≤ γ_K P_a P_b + λu. Since K ≥ 1, γ_K λ ≥ γ_1 λ = λu / (1 − u) ≥ λu, so |ŷ − y| ≤ γ_K (P_a P_b + λ) = γ_K P_y.

For a dropped coefficient of a versor, e_j = 0, so |d_j| ≤ γ_(K_j) P_j.

**Lemma B.** The core computes P̂_j by running the same two kernels on |v| and |x| with |c| for each coefficient, every rounded product followed by an addition of λ: a term computes p = fl(fl(|v_a| |x_b|) + λ), then, when |c| ≠ 1, p = fl(fl(|c| p) + λ), and accumulates p by fl(out + p) (the second stage reads P̂ of the middle blades in place of |middle_a|). Let K'_y count the roundings of this computation on the path to y: two per product, one per sum, so K'_y ≤ 2 K_y. Then P̂_y ≥ (1 − u)^(K'_y) P_y ≥ (1 − u)^(2K_y) P_y.

Every value here is nonnegative. A sum: fl(s) = s(1 + δ) ≥ (1 − u) s. A padded product of z = P̂_a P̂_b (or |c| p̂): fl(z) ≥ min(z(1 − u), z − λu) ≥ (1 − u) z − λu by (2.8) with δη = 0, and then fl(fl(z) + λ) ≥ (1 − u)((1 − u) z − λu + λ) = (1 − u)² (z + λ). By induction, with P̂_a P̂_b ≥ (1 − u)^(K'_a + K'_b) P_a P_b, the padded product is at least (1 − u)^(K'_a + K'_b + 2) (P_a P_b + λ). Every computed padded value is at least λ(1 − u)², a normal number, so none underflows.

**Lemma C.** For k = K + 1, if (K + 1)(2K + 1) u < 1, then (1 − u)^(2K+1) γ_(K+1) ≥ γ_K.

Since (1 − u)^(2K+1) ≥ 1 − (2K + 1) u, it suffices that (1 − (2K + 1)u)(K + 1)(1 − K u) ≥ K (1 − (K + 1) u), after multiplying by the positive denominators. The difference of the two sides is 1 − (K + 1)(2K + 1) u + K (K + 1)(2K + 1) u² = 1 − (K + 1)(2K + 1) u (1 − K u), which is positive under the hypothesis. With k = K, the factor (1 − u)^(2K+1) < 1 makes the inequality fail, so one rounding is the smallest integer margin.

**The computed comparison.** The tolerance is tol_j = fl(g_j P̂_j) with g_j = fl(k u / (1 − k u)), k = K_j + 1. Under the hypothesis of Lemma C, k u is k times a power of two and 1 − k u a multiple of u in [1/2, 1), both exact in T, so only the division rounds, without underflow, and g_j ≥ (1 − u) γ_k. Then g_j P̂_j ≥ (1 − u)^(2K+1) γ_(K+1) P_j ≥ γ_K P_j ≥ |d_j| as exact reals (Lemmas A to C). |d_j| is a value of T, and rounding to nearest is monotone, also below λ, so |d_j| ≤ fl(g_j P̂_j) = tol_j. g_j < 1, so tol_j is finite when P̂_j is. So the check

    |d_j| ≤ tol_j and tol_j ≤ numeric_limits<T>::max()

passes for every dropped coefficient of a versor whose computation does not overflow, and fails for a NaN or an infinite d_j (a comparison with NaN is false; an infinite d_j exceeds any finite tol_j) and for an infinite P̂_j.

The hypothesis of Lemma C holds for K up to 2,895 in float and 67,108,863 in double. The largest K of any transform that builds is far below it: K is at most the number of terms of a middle coefficient plus that of a dropped one, plus two; in a diagonal form each is at most 2^n, so K is at most about 130 at n = 6, the largest transform that builds on GCC (decision 0178, the measured limits), and a non-diagonal form, with more terms per output, stays far below the limit too (inferred, not counted). A `static_assert` states the hypothesis for every dropped coefficient.

**From above.** For inputs of ordinary size, where λ is negligible beside M, every rounding of P̂ and g can also raise them by at most a factor 1 + u, so tol_j ≤ (1 + u)^(2K+2) γ_(K+1) M_j (1 + λW/M) < γ_(K+2) M_j, with W the padding's weight, since (1 + u)^(2K+2) − 1 ≈ (2K + 2) u is far below (γ_(K+2) − γ_(K+1)) / γ_(K+1) ≈ 1/(K + 1). So for such inputs γ_K M_j ≤ tol_j < γ_(K+2) M_j: the tests pin the margin from both sides with that pair of inequalities, against `tracked`'s K and M.

**Contraction.** A fused multiply-add rounds once where the count has two, and its absolute error below λ is at most λu, which the padding of its product covers; on the magnitude side every value is at least λ, so a fused operation there neither underflows nor rounds more than the count allows. So the check also holds for a contracted build (the test build sets `-ffp-contract=off`).

## Decision

1. In checking mode (every constant evaluation; run time with `GALOOT_CHECKING` 1), for a scalar T with `numeric_limits<T>::is_iec559` and `round_style == round_to_nearest`, `transform` computes each dropped coefficient d_j as for an exact scalar (decision 0178 item 4), its count K_j at compile time by the rules above, and the padded magnitude P̂_j by the absolute-value kernels of Lemma B, and checks |d_j| ≤ fl(fl(γ_(K_j+1)) P̂_j) ≤ `numeric_limits<T>::max()` by `GALOOT_ASSERT`, once per dropped coefficient, with a message of its own. The kept coefficients are bitwise those of the unchecked path, as for an exact scalar.
2. Constants, named in `transform.hpp` with their derivation beside them (S15): the margin of one rounding, and the padding λ = `numeric_limits<T>::min()` per rounded product.
3. `detail` names the tests read: `rounding_checked_scalar<T>` (the scalars of item 1), `transform_dropped_roundings<Alg, V, X>` (the K_j, in the order of the dropped blades, which is the order of the second plan's result without X's blades), `transform_dropped_tolerances<Alg, V, X>(v, x)` (the tol_j), and `check_transform_rounded(dropped, tolerances)`.
4. A `static_assert` states Lemma C's hypothesis for every dropped coefficient.
5. `transform.hpp` stops compilation with `#error` when `__FAST_MATH__` is defined and `GALOOT_CHECKING` is 1.
6. Not checked, and stated in the comment: a scalar that is neither exact nor of item 1; and what V's own rounding error leaves, below.

## What the check cannot tell (O6, ruled: keep D10 as ruled for version 1, characterize, revisit in M6)

The bound takes V and x as exact. A V that carries rounding error from its own construction is only nearly a versor. Where a dropped part is zero by a polynomial identity in V for every V of one parity, as for the point, line, plane, motor, and flector layouts of rigid(3) (decision 0177), V's error cannot make it nonzero, so no false abort can come from it there. Where a dropped part is zero only because V is a versor, as for the antiscalar part of a conformal(3) vector, a rounded V leaves a dropped part of the order of its own error times |V|²|x|, which can exceed this operation's bound. WU13b measures how often a rounded conformal(3) motor trips the check (plan 0021 criterion 5); the result is appended to this record.

## Consequences

- The tests (`transform_rounding_test.cpp`) require the core's K_j to equal `tracked`'s K and γ_K M ≤ tol_j < γ_(K+2) M for every dropped coefficient on exact dyadic inputs of ordinary size, show the check firing on a non-versor and at ten times the tolerance, not firing at a tenth, not firing for inputs whose products underflow, and firing for a non-finite dropped coefficient.
- Checking mode costs one more run of the first stage and of the dropped terms, on absolute values with one more addition per rounded product. Unchecked builds are unchanged.
- Decision 0175's inventory gains `transform`, `compose`, and `antiproject` (its addendum).

## Record of the process

This record, with the derivation, was committed before the test author's rounding tests were written or run, and before the check was implemented. The probe of the additive underflow term above is a scratch computation in exact rationals (Python `fractions`), not reproducible as committed (V10); its role is to show that option 1 fails, which the argument above shows independently.

## Addendum, 2026-11-07: what the check does with rounded versors (plan 0021 criterion 5)

Measured after the check was implemented (`records/spikes/transform-rounding/`, at the headers of `c2af6a8`, seed 20261107, 10,000 cases per family, identical on GCC 16 and Clang 23): for conformal(3) vectors moved by versors composed in double from random flat planes, the check fires in 11 cases of 10,000 for two planes and 15 for three, with dropped parts at most 3.76 times their tolerance. It never fired for versors of general hyperplanes, for a single exact plane, for conformal(3) bivectors, or for the rigid(3) controls, whose largest ratios stayed below 0.4. On exact versors the tests' largest ratios of |d| to its tolerance were 0.089 over double and 0.151 over float (`core.transform_rounding`, seed 0). This is the information O6 asked for; the ruling stands (keep the check for version 1, revisit in M6). A sampled claim (V9), not a bound.
