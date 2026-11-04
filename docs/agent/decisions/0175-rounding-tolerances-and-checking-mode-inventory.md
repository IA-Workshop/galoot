# 0175 Rounding tests over double: a tracked forward bound with a margin of two roundings, and the M3 checking-mode inventory

- Status: accepted (plan 0020 question O3, readings R14 and R15; the owner answered O3 "yes", accepting both readings as recommended, on 2026-11-04, relayed by the coordinator)
- Date: 2026-11-04
- Recorded by: Claude (agent, Opus 5.5), implementer of WU11b, numerical-expert role
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

The derivation is also written beside the constant `rounding_margin` in `test/support/tracked.hpp` (STYLE S15).

## Decision

1. `test/support/tracked.hpp` holds `test_support::tracked` and the check `within_rounding_bound(d, e, K, M)`: exact equality when K = 0, and fl(\|d − fl(e)\|) ≤ fl(fl(γ_(K+2)) fl(M)) otherwise. The margin constant is `rounding_margin = 2`. The original proposal, γ_(K+4), is kept in plan 0020 R15.
2. `test/core/rounding_test.cpp` (`core.rounding`) runs every M3 operation on `develop` over double and over `tracked`, on seeded inputs p / 2^27 with \|p\| < 2^27 (wide inputs p / 2^(53+s), \|p\| < 2^53, s from 0 to 6, for sums, whose sums would otherwise be exact), and compares each coefficient with the exact value composed from the golden tables. The tracked value must equal the golden value exactly. Operations that make no rounding (the grade maps, the complements, negation, `part`, `truncate`) must have K = 0 and give the exact value. The scalings and division are compared for equality with the one IEEE operation per coefficient, written in the test.
3. Inputs are sized so that every exact value and magnitude fits the test rational: the largest sum of coefficient magnitudes on one output is 111 (the random form; 32 in conformal(3)), so magnitudes stay below 2^7 with denominators of at most 2^54.
4. Each family reports its largest ratio of error to γ_K M, over all rounded coefficients and over those with K ≥ 2. A ratio above 1 is a defect; ratios near 1 are expected for one rounding, whose relative error can approach u.

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
- The bound is tight at one rounding: observed ratios reach 0.9999 over five seeds. The margin of two roundings is what keeps the check from rejecting such results because of its own rounding.
- `tracked` needs no `numeric_limits` specialization: the core treats it as a rounding scalar, so `truncate` does not check it, and its coefficient check is skipped (a type without limits must hold the coefficients exactly, which `tracked` does).
- Float is not covered: M3 asks for double, and float would need its own input scale (noted for WU17 in R15).
