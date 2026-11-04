# 0175 Rounding tests over double: a tracked forward bound with a margin of two roundings, and the M3 checking-mode inventory

- Status: accepted (plan 0020 question O3, readings R14 and R15; the owner answered O3 "yes", accepting both readings as recommended, on 2026-11-04, relayed by the coordinator)
- Date: 2026-11-04
- Recorded by: Claude (agent, Opus 5.5), implementer of WU11b, numerical-expert role
- Amended: 2026-11-04, after the blind review (`records/reviews/2026-11-04-wu11b-review.md`, F1, F2, F4): the check of a result with one rounding is equality, and a result with two or more roundings must also pass a limit on the ratio of its exact error to its bound. The margin, its derivation, and the inventory are unchanged.
- Bears on: VVUQ V12 (tolerances), V2 (tests that can fail), V7 (seeded tests), STYLE S15 (named tolerances), constitution C7 and C8, CONVENTIONS 4.5, decisions 0010, 0161, 0168, plan 0020 (R14, R15, section 6), research note 0020

## Context

M3's gate asks for "V12 double tests with forward bounds" and for the "checking mode". Plan 0020 reading R15 designs the tests: run each operation over double and over a test-only scalar that tracks, for each result, its exact value, the magnitude M of the same computation on absolute values, and the depth K of roundings, and accept a double result within γ_K M of the exact golden value, plus a margin for the check's own arithmetic. R15 proposed γ_(K+4) M̂, counting four roundings in the check, and left the margin's derivation to the numerical-expert. Reading R14 says what M3's "checking mode" covers. The owner accepted R14 and R15 as recommended (O3, 2026-11-04).

## Options weighed

The oracle for the bound:
1. A tracking scalar run through the library's own kernels (chosen, R15). The bound then has exactly the roundings the kernel makes, including those of composed operations, and needs no per-operation formula. Its exact value doubles as a check that the tracked computation is right.
2. A closed-form bound per operation from the plan, γ_n Σ\|c\|\|a\|\|b\| with n the number of terms on each output. It needs the plan's term counts in the test, which ties the test to the plan's layout, and it does not cover compositions.
3. A higher-precision reference: x87 is out of scope, `__float128` is a compiler extension that STYLE S2 disables, and double-double arithmetic is more machinery than an exact rational with sized inputs (C12).

The margin, with K the tracked depth, e the exact value, M the magnitude, and d the double result:
1. γ_(K+4) M̂, as R15 proposed: four roundings for fl(e), fl(M), the subtraction, and the product that forms the tolerance. Sound, and looser than needed.
2. γ_(K+2) M̂ (chosen): the smallest integer margin the derivation below admits. VVUQ V12 asks for the tightest tolerance that admits every known source of difference.
3. γ_(K+1) M̂: the derivation does not go through (below), and no case that rejects a valid result was constructed to settle it either way, so it is not used.

## Derivation of the margin

Model: IEEE 754 double, rounding to nearest, no underflow or overflow, each operation rounded once to double (`tracked.hpp` asserts `FLT_EVAL_METHOD == 0`, `is_iec559`, 53 digits, and round to nearest). Source: Higham, *Accuracy and Stability of Numerical Algorithms*, 2nd ed., read in research note 0020.

1. The kernel is a straight-line program of +, −, × over exact inputs. Expanded over its monomials m_i, its result is d = Σ m_i (1 + θ_i), each θ_i a product of at most K factors (1 + δ) with \|δ\| ≤ u, so \|θ_i\| ≤ γ_K (Lemma 3.1), and \|d − e\| ≤ γ_K Σ\|m_i\| = γ_K M (as (3.5)). The tracking scalar's rules compute K and M: an input or integer constant is depth 0; negation keeps the depth; a sum or difference is one more than the larger depth; a product is one more than the sum of the depths. A fused multiply-add rounds once where the rules count two, so the bound also holds for a contracted build (the test build sets `-ffp-contract=off` anyway).
2. The check computes e' = fl(e) and M' = fl(M), each a correctly rounded conversion of a rational with a power-of-two denominator (the numerator converted to double, then divided by the power of two exactly). By Theorem 2.2, \|e' − e\| ≤ u\|e\| ≤ u M (since \|e\| ≤ M) and M' ≥ (1 − u) M.
3. So \|d − e'\| ≤ (γ_K + u) M ≤ γ_(K+1) M, by Lemma 3.3 (γ_k + u ≤ γ_(k+1)).
4. The tolerance factor g = fl(k u / (1 − k u)) with k = K + 2: k u is a small integer times 2^−53 and 1 − k u a multiple of 2^−53 in [1/2, 1), both exact, so only the division rounds and g ≥ (1 − u) γ_k. Then g M' ≥ (1 − u)² γ_(K+2) M.
5. (1 − u)² γ_(j+1) ≥ γ_j for j = K + 1: multiplying out, (j + 1)(1 − u)²(1 − j u) − j(1 − (j + 1) u) = 1 − 2(j + 1) u + (j + 1)(2j + 1) u² − j(j + 1) u³, which is positive whenever 2(j + 1) u < 1. So \|d − e'\| ≤ g M' as exact reals.
6. Rounding to nearest is monotone, so fl(\|d − e'\|) ≤ fl(g M'): the computed check passes for every d within the true bound.

With margin 1, step 4 gives (1 − u)² γ_(K+1) M, which is less than γ_(K+1) M, and the argument fails. When K = 0 the operation made no rounding, and the check requires d to equal e exactly, as a rational, with no tolerance.

## After the review: one rounding by equality, and the ratio limit

The blind review (F1) found the margin check too loose as the only check: it measures from fl(e), so it admits \|d − e\| up to about γ_(K+3) M, and a kernel that adds one ulp after every sum passed it, with a reported ratio of 2.997. The check now has three parts:

- K = 0: d equals e exactly.
- K = 1: d equals fl(e), the double nearest e, ties to even. A value of depth 0 is an input, an integer constant, a copy, or a negation of one, so it is an exact double; a result of depth 1 is one +, −, or × of exact doubles, which IEEE 754 rounds correctly, possibly followed by a copy or a negation, which keep it. So the double result is fl(e) itself, and anything else is a defect. The margin plays no part here.
- K ≥ 2: the margin check above, and also `bound_ratio(d, e, K, M)` ≤ `ratio_limit` = 1 + 6u. The ratio is formed from the exact difference d − e (both exact rationals) and five roundings: the difference converted to double, the division that forms γ_K, M converted to double, the product γ_K M, and the final division. Each multiplies or divides the exact ratio r by one factor (1 + δ), so the computed ratio is r(1 + θ_5) with \|θ_5\| ≤ γ_5 (Lemma 3.1). For d within the bound, r ≤ 1, so the computed ratio is at most 1 + γ_5; γ_5 = 5u + 25u²/(1 − 5u) < 6u, and the doubles just above 1 are 1 + 2u, 1 + 4u, 1 + 6u, so 1 + 6u is the smallest double at or above 1 + γ_5. A result whose exact ratio exceeds (1 + 6u)/(1 − γ_5), about 1 + 11u, is rejected. The reviewer suggested about 8u; 6u is the tightest double the derivation admits.

Each family of `core.rounding` also checks its largest computed ratio against `ratio_limit`, so the rule "a ratio above 1 is a defect" is a test, not a report. The margin check stays for K ≥ 2 as asked by the review; the ratio check is the tighter of the two.

The derivation is also written beside the constant `rounding_margin` in `test/support/tracked.hpp` (STYLE S15).

## Decision

1. `test/support/tracked.hpp` holds `test_support::tracked` and the check `within_rounding_bound(d, e, K, M)`: exact equality when K = 0; d == fl(e) when K = 1; and, when K ≥ 2, both fl(\|d − fl(e)\|) ≤ fl(fl(γ_(K+2)) fl(M)) and the computed ratio \|d − e\| / (γ_K M) ≤ 1 + 6u. The constants are `rounding_margin = 2` and `ratio_limit`. The original proposal, γ_(K+4), is kept in plan 0020 R15.
2. `test/core/rounding_test.cpp` (`core.rounding`) runs every M3 operation on `develop` over double and over `tracked`, on seeded inputs p / 2^27 with \|p\| < 2^27 (wide inputs p / 2^(53+s), \|p\| < 2^53, s from 0 to 6, for sums, whose sums would otherwise be exact), and compares each coefficient with the exact value composed from the golden tables. The tracked value must equal the golden value exactly. Operations that make no rounding (the grade maps, the complements, negation, `part`, `truncate`) must have K = 0 and give the exact value. The scalings and division are compared for equality with the one IEEE operation per coefficient, written in the test.
3. Inputs are sized so that every exact value and magnitude fits the test rational: the largest sum of coefficient magnitudes on one output is 111 (the random form; 32 in conformal(3)), so magnitudes stay below 2^7 with denominators of at most 2^54.
4. Each family reports its largest ratio of error to γ_K M, over all rounded coefficients and over those with K ≥ 2, and checks it against `ratio_limit`. Ratios near 1 are expected for one rounding, whose relative error can approach u.

## Checking-mode inventory at M3 (reading R14, accepted)

What each M3 operation on `develop` at `c86cb58` checks when `GALOOT_CHECKING` is 1 (and in every constant evaluation, where a failed `GALOOT_ASSERT` ends the evaluation):

| Operation | Drops part of its value? | Check in checking mode |
| --- | --- | --- |
| `geometric_product`, `wedge`, `geometric_antiproduct`, `antiwedge`, `* ^ % &` | No: the result layout holds every blade the plan reaches | None at run time. At compile time, a `static_assert` that the scalar holds every exact coefficient of the plan (not a checking-mode check) |
| `reverse`, `antireverse`, `grade_involution`, `right_complement`, `left_complement`, `~`, `!` | No: sign maps and permutations | None |
| `a + b`, `a - b` | No: `sum_layout` holds every blade of both | None |
| `-a`, `s * a`, `a * s` | No | None |
| `a / s` | No | For an integer scalar, `s != 0` (`GALOOT_ASSERT`), a precondition rather than a check of discarded data; floating scalars are not checked |
| lossless conversion `Type{x}` | No | None; a lossy conversion is a compile-time error |
| `truncate<Blades>(x)`, `truncate<Type>(x)` | Yes, the blades the target lacks | For an exact scalar with `==`, every dropped coefficient is zero (`GALOOT_ASSERT`); for a rounding scalar, none (CONVENTIONS 4.5, decision 0161) |
| `part<Blades>(x)`, `part<Type>(x)` | Yes, by purpose | Never (decision 0168) |
| `get`, `has_name` | No | Compile time only |

The other `GALOOT_ASSERT`s of the core guard preconditions of the generator (blade masks within e_n, n from 1 to 32, product coefficients fitting `coefficient_type`, a symmetric form within range), not discarded data.

Evidence that exists: `core.mv_truncate_nonzero_<scalar>` and the `mv_part_checking` program (both modes, decision 0030); `core.linear_divide_*`. Pending: WU11a's operations (`bulk`, `weight`, the four duals, `dot`, `antidot`, `project`), which reading R14 says never check; the last group of WU11b adds them to this table and to `core.rounding` after WU11a merges. The rounding-scalar check of decision 0010, a dropped part within the operation's forward bound, belongs to `transform` (WU13), which will check its core-side bound against this test-side oracle.

## Consequences

- The tests can fail (V2): mutants of the kernel (products rounded through float), of division (multiplication by the reciprocal), of the negation (a multiplication by −1, which adds a rounding the exact operations must not have), and of the tracking scalar and the check are killed (records `mutations/0020-rounding/`).
- The bound is tight at one rounding: observed ratios reach 0.99997 over five seeds. Those results are now checked by equality with fl(e), so the margin plays no part for them. The margin of two roundings protects results with two or more roundings, whose check measures from fl(e) and is computed in double, from being rejected because of the check's own rounding; the ratio limit then keeps that check from admitting more than the bound.
- `tracked` needs no `numeric_limits` specialization: the core treats it as a rounding scalar, so `truncate` does not check it, and its coefficient check is skipped (a type without limits must hold the coefficients exactly, which `tracked` does).
- Float is not covered: M3 asks for double, and float would need its own input scale (noted for WU17 in R15).

## Record of the process

- The margin and its derivation were written into `test/support/tracked.hpp` before the first build and run of the tests (the first `core.rounding` run is the run-log row of 2026-11-04 12:55:23 UTC); the derivation rested on the text of Higham's pages read before that. The page images of Lemma 3.1 and Lemma 3.3 were read after the first runs, while writing research note 0020, and agree with the text read before. So the margin was fixed before any result was seen.
- The K = 1 equality and the ratio limit were added after the blind review, when results had been seen. Neither rests on an observed ratio: both are derived above, and the limit is tighter than what was observed allowed for, not looser.
- History of the branch was rewritten once while it was unshared: three commits (`9a6ca29`, `3aa9079`, `38ba916`) were folded into `c8f4c02` and `e36a905` with `git reset --soft`, to remove two commits on which `tests.reference_markers` failed. The final tree was the same. The run log holds 54 rows naming `3aa9079` (22 clean, 30 `-dirty`) and 2 naming `38ba916`; those commits are no longer on the branch, so those rows cannot be reproduced from its history (they survive only in the implementer's reflog). `38ba916` had the same tree as `e36a905`. No history was rewritten after the branch was reported for review.
