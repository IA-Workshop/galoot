# Tolerances for the floating change of basis of WU12

- Date: 2026-11-09
- Author: Claude (agent, Opus 5.5), numerical-expert role, part N of plan 0030 (criterion 8; decision 0219).
- Branch `records/wu12-part-n`, from `develop` at `a6307ba`. Nothing in `include/` or `test/` is changed.
- Written before any floating test of the change of basis exists or has run (V12). `change_basis` is not yet written (part C), so no double, float, or complex result of it has been computed. The only runs are the exact scripts below, over `fractions.Fraction` and sympy.
- Files, in `wu12-change-basis-tolerances/`:
  - `changes.py`, `changes.out`: the denominators, term counts, and coefficient sums of 1.4's change and of the LDLᵀ change of each seeded random form, exact.
  - `polynomials.py`, `polynomials.out`: the sympy and exact checks of the polynomials and constants below (sympy 1.14.0, CPython 3.14.9, the math-tools interpreter).
  - `model.py`, `model.out`: a model of the assumed operation form in Python floats, run only after the rule was committed (section "Model check").
  - Run each from the repository root with `python3 -I <path>`. They are single runs, not build or test steps, so they need no job pool. Here they ran under `tools/timed` with a log in the session scratchpad: 0.13 s, 0.46 s, and 2.6 s.

## The computation

Criterion 8 asks that `change_basis` work over `double`, `float`, and `std::complex<double>`, with a tolerance fixed by this record for `double`. For a dyadic change, 1.4's, on inputs that are small integers times a power of two, the bound is zero and the comparison is `==`. For an LDLᵀ change whose denominators are not powers of two, the tolerance is "the forward bound of one sum of m products and one division, as `test/support/tracked.hpp` counts it".

The rule assumes the operation form of plan 0030's section 6 and R4, which part C has not yet written:

- For each output blade B, y_B = (Σ_A c_AB x_A) / q_B. The c_AB are integers, and q_B > 0 is the least common multiple of the reduced denominators of the minors det P[A, B] over the source blades A of B's grade.
- The integer combination is evaluated by `accumulate` in `mv.hpp`, as for every other map: the terms in the kernel's order, left to right; the first term assigned; a coefficient of +1 or −1 as an addition or a subtraction; any other coefficient as `static_cast<T>(C) * x`.
- Then one division by `static_cast<T>(q_B)` when q_B ≠ 1.

If part C writes it another way, regime C's count follows automatically, because `tracked` runs the same templates. The claims of regime E below do not follow. Three forms would change them: a multiplication by a reciprocal, a division applied to each term, and a denominator that is not reduced. Decision 0221 then says which form was chosen, and this rule is checked again before the floating tests run.

## Sources (V9)

- `test/support/tracked.hpp` at `a6307ba`: the counting rules, the bound |d − e| ≤ γ_K M, `within_rounding_bound`, `bound_ratio` with `ratio_limit` = 1 + 4u and its derivation from five roundings, `nearest_double` (power-of-two denominators only), and `exact_rational` (power of two from −62 to 8).
- `include/galoot/core/mv.hpp` at `a6307ba`: `accumulate` and `run_map`, the form a map's kernel takes.
- `docs/agent/records/numerical/wu14-identities-tolerances.md`, as the model of method.
- Higham, *Accuracy and Stability of Numerical Algorithms*, 2nd ed.: the standard model (2.4), fl(x op y) = (x op y)(1 + δ) with |δ| ≤ u for op in {+, −, ×, /}, Lemma 3.1, and Lemma 3.3. These are cited through `tracked.hpp`'s comments and the WU14 record. They were not re-read for this record. That (2.4) covers division is recalled [background]. So is IEEE 754's correct rounding of division, which the standard model rests on.

## Model and assumptions

- T is double: p = 53, u = 2⁻⁵³, γ_k = k u / (1 − k u). For float, p = 24 and u = 2⁻²⁴, used only in regime E. Every operation rounds once, to nearest, ties to even (`FLT_EVAL_METHOD == 0`, which `tracked.hpp` asserts). The test build sets `-ffp-contract=off`. A contracted build also satisfies every claim below: a fused multiply-add rounds once where the count has two, and in regime E its product and sum are exact either way.
- No overflow. No underflow: the input classes below keep every nonzero value above 2⁻⁶⁰.
- Every input is exact in both runs. The double input is the tracked input's value.
- The double run and the tracked run perform the same operation tree, as for WU14.

## What `tracked` gains: a division by an exact divisor

`tracked` has no division today, and the change of basis divides. The rule for `a / b`:

- **Requires** that b is exact (K_b = 0) and nonzero. Otherwise it stops, as the rational stops on overflow. An exact value is an input, an integer constant, or a negation of one, so its magnitude is |b|.
- **The value** is a / b.
- **The magnitude** is M_a / |b|.
- **The count** is K_a + 1. If |b| is a power of two, it is K_a.

**Derivation.** As `tracked.hpp`'s comment shows, the double result of a is Σ m_i (1 + δ_i1) … (1 + δ_ik), with at most K_a factors per term and Σ |m_i| = M_a. By (2.4), fl(â / b) = (â / b)(1 + δ) with |δ| ≤ u, so the result is Σ (m_i / b)(1 + δ_i1) … (1 + δ_ik)(1 + δ). That is at most K_a + 1 factors per term, and the terms' magnitudes sum to M_a / |b|. By Lemma 3.1, |d − a/b| ≤ γ_(K_a+1) M_a / |b|. A divisor that is a power of two scales exactly when nothing underflows, so its δ is 0 and the count is unchanged. The bound needs b exact. A rounded divisor, or a divisor that is not a constant, makes the result a ratio of two expansions, which the rule does not cover. The check that K_b = 0 enforces this.

`polynomials.py` (check 1) confirms Lemma 3.1's two extremes, (1 + u)^k − 1 ≤ γ_k and 1 − (1 − u)^k ≤ γ_k, exactly for k = 1 to 64 at both values of u.

**The count of one output.** Each term t_j has K = 0 for a coefficient of ±1 (the input itself, or its negation) and K = 1 otherwise (a product with an exact integer). Along the left-to-right chain, K(s_1) = K(t_1) and K(s_j) = max(K(s_(j−1)), K(t_j)) + 1. So a sum of m terms has K = m if t_1 or t_2 is a product, and K = m − 1 otherwise. The division adds one when q_B is not a power of two. The largest count is m + 1, which is γ_(m+1), Higham's γ_m for an inner product of m terms plus one rounding for the division: "one sum of m products and one division". The magnitude is M = Σ_A |det P[A, B]| |x_A|. It does not depend on how the integers and the denominator are split.

## The rule

The tests use two classes of inputs, and the class fixes the check for each coefficient. In each check, d is the double (or float, or complex) coefficient. e, K, and M are the tracked run's value, count, and magnitude for the same coefficient. A blade that one result has and the other does not counts as 0, with K = 0 and M = 0.

### Regime E: small dyadic inputs, so that only the division can round

**Inputs.** Each input is x = n · 2⁻ᵇ with |n| ≤ N_E = 2²⁰ and 0 ≤ b ≤ 20. Until `exact_rational`'s range check is corrected (see "A defect in `exact_rational`"), the test also needs C · N_E · 2⁻ᵇ < 2⁹, so that no exact result is a multiple of 2⁹. For 1.4's change and its round trip, b ≥ 15 is enough.

**Condition.** Let C = max_B Σ_A |c_AB| over the outputs of the layout. The condition is C · N_E ≤ 2ᵖ: 2⁵³ for double and 2²⁴ for float. For 1.4's change, C is 3 forward and 2 back. A round trip, back after forward, needs 12 N_E ≤ 2ᵖ. In units of 2^−(b+1), the forward results have integer numerators of at most 6 N_E, and the back change sums at most 2 of them per output. For the LDLᵀ changes of `changes.out`, C is at most 18,612, so N_E = 2²⁰ meets the condition for double (2³⁵ < 2⁵³) on every change listed.

**Lemma E.** Under the condition, every term and every partial sum before the division is exact, and the sum is the exact S_B = q_B e. Every intermediate is an integer multiple of 2⁻ᵇ: n is an integer, c_AB is an integer, and sums keep the grid. Its magnitude is at most Σ_A |c_AB| |x_A| ≤ C N_E 2⁻ᵇ ≤ 2^(p−b). An integer of at most p bits times 2⁻ᵇ is a floating-point number, so each operation's exact result is representable, and rounding to nearest returns it. The integer constants, at most C in magnitude, are exact too.

**The check.**

- **q_B a power of two (including 1):** d = S_B / q_B is exact, so `exact_rational(d) == e`. The bound is zero. This covers all of 1.4's change, forward, back, and round trip, over double and float. It also covers the LDLᵀ changes of random forms 0, 1, 5, and 6, whose denominators are all powers of two in `changes.out`.
- **q_B not a power of two (double only):** d = fl(S_B / q_B), the correctly rounded quotient of two exact operands. The test requires that d equals the correctly rounded value of e, which it computes from e alone. Let w be the odd part of e's denominator. Then w e is dyadic. Its numerator divides S_B's, so w e is a double, and `nearest_double(w e)` returns it exactly. fl(`nearest_double(w e)` / w) is then a correctly rounded quotient of two exact operands. Correct rounding depends only on the real number w e / w = e, so this value equals d for a correct implementation. The helper checks that `exact_rational(nearest_double(w e)) == w e`, and stops otherwise. It is the K = 1 case of `within_rounding_bound` ("the double nearest the exact value"), extended to an exact value whose denominator is not a power of two.

The derivation is the comment beside the constant N_E, in words such as: "Every input is an integer of at most 20 bits times 2^-b, and every integer coefficient sum of the change is at most C, so each term and partial sum is an integer multiple of 2^-b below 2^(53-b) in magnitude, which double holds exactly. Only the division by a denominator that is not a power of two rounds, once, so the result is the correctly rounded quotient."

### Regime C: wide integer inputs, so that products and sums round

**Inputs.** Each input is an integer n with |n| ≤ N_R, where N_R is the largest power of two with N_R ≤ 2⁵³ and C · N_R ≤ 2⁶⁰. For random 7 → diagonal (C = 867), N_R = 2⁵⁰. For random 2 → diagonal (C = 42), N_R = 2⁵³. For 1.4's change (C = 3), N_R = 2⁵³. Then many products and most sums of several terms need more than 53 bits, so double rounds, while every exact value and magnitude stays within the oracle's range (below).

**The check, by tracked's count K, which includes the division:**

- **K = 0:** `exact_rational(d) == e`.
- **K = 1:** one operation rounds, its operands are exact, and whatever follows it (a negation, or a division by a power of two) is exact. So d equals the correctly rounded value of e. The test uses the same check as regime E's second case.
- **K ≥ 2:** the scaled ratio check below, which requires ratio ≤ `ratio_limit` = 1 + 4u.

**The scaled ratio.** `within_rounding_bound` and `bound_ratio` convert e and M with `nearest_double`, which requires a power-of-two denominator. Here e = S_B / q_B is not dyadic. Scale both sides by w, the least common multiple of the odd parts of the denominators of e and M. Both odd parts divide q_B's odd part, so w ≤ q_B. Then w d, w e, and w M are all dyadic, and

  |d − e| ≤ γ_K M  if and only if  |w d − w e| ≤ γ_K w M.

The test forms the exact difference D = `exact_rational(d)` · w − w e as a rational (dyadic) and computes

  ratio = fl( fl(|D|) / ( fl(γ_K) · fl(w M) ) ),

with fl(|D|) = `nearest_double(D)` in absolute value, fl(γ_K) = (K u) / (1 − K u) in double, and fl(w M) = `nearest_double(w M)`. If D = 0, the ratio is 0.

**Proof that a result within the bound passes, and how far beyond it one can pass.** This is `ratio_limit`'s derivation unchanged, since the scaled computation rounds the same five times. These are the conversion of D, the division that forms γ_K (K u and 1 − K u are exact), the conversion of w M, the product, and the final division. Each multiplies or divides the exact ratio r = |w d − w e| / (γ_K w M) = |d − e| / (γ_K M) by one factor 1 + δ, so the computed ratio is r(1 + θ_5) with |θ_5| ≤ γ_5. Check 2 of `polynomials.py` gives 6u − γ_5 = u(1 − 30u) / (1 − 5u) > 0, so for r ≤ 1 the computed ratio is a double at most 1 + γ_5 < 1 + 6u, hence at most 1 + 4u. The check therefore never rejects a result within the bound. It rejects every result with r > (1 + 4u) / (1 − γ_5), which is 1 + 9.000000u for double (check 2).

**The margin, and why.** The count K has no margin: it is `tracked`'s count, including the division's rounding, and the bound γ_K M is the bound itself. The only slack is `ratio_limit`'s 4u above an exact ratio of 1. That slack is forced by the five roundings of the check's own arithmetic, and by this argument no smaller limit is safe. Regimes E and C with K ≤ 1 have no slack at all: they are equalities.

**Why a ratio and not a tolerance.** A tolerance in the form of `rounding_tolerance` needs fl(e), the double nearest a rational whose denominator is not a power of two. The oracle's int64 rational cannot produce it in one rounding when e's numerator exceeds 2⁵³. An identity-style tolerance on the scaled difference, fl(|D|) ≤ fl(fl(γ_(K+1)) fl(w M)), is also sound with a margin of one rounding. Check 3 of `polynomials.py` gives the cleared form (1 − u)²γ_(K+1) − γ_K ∝ 1 − 2(K + 1)u + (K + 1)(2K + 1)u² − K(K + 1)u³, which is at least 1 − 2(K + 1)u > 0. Margin 1 holds exactly for K = 1 to 10,000, and margin 0 fails. But that tolerance admits an error up to γ_(K+1)/γ_K ≈ 1 + 1/K times the bound, against 1 + 9u for the ratio. V12 asks for the tightest tolerance, so the rule is the ratio. The margin-one tolerance is recorded only as the fallback if the ratio's helper proves awkward, and choosing it would itself be a change of tolerance (C18).

### The oracle's range in regime C

The int64 rational stops rather than wraps. `exact_rational` documents that it accepts a double whose denominator is at most 2⁶² and whose magnitude is below 2⁶². Once its range check matches that contract (next section), integer inputs keep regime C inside both:

- Every computed partial sum is an integer, because a rounded integer double is an integer. So a nonzero result has |d| ≥ (1 − u) / q_B > 2⁻¹⁰ for q_B < 2⁹, and its denominator is at most 2⁶². With C N_R ≤ 2⁶⁰, every exact value, magnitude, and |d| is below 2⁶¹.
- w d, w e, and w M are at most about q_B · 2⁶⁰ / q_B = 2⁶⁰ in magnitude. `exact_rational(d)` · w has a numerator of at most 2⁵³ · w, which fits for w < 2⁹.

So regime C needs every output denominator below 2⁹ = 512. In `changes.out` this holds for every change except random 3 → diagonal (denominators up to 15,510). Random 7 → diagonal (up to 72, m up to 10, so K up to 11) and random 2 → diagonal (up to 30, m up to 5) qualify. So does the diagonal → random 7 direction (up to 20). Inputs on a grid 2⁻ᵇ instead of integers can make |d| small enough after cancellation that `exact_rational` stops, so regime C's inputs are integers.

### A defect in `exact_rational`

`exact_rational` stops when the power of two left after removing the significand's trailing zeros is above 8, whatever the significand. So it stops on 1024.0 = 1 · 2¹⁰, which its comment says it accepts: the denominator is 1 and the magnitude is far below 2⁶². This was confirmed by a one-off program built against the headers at `a6307ba` (`c++ -std=c++23 -O1 -I include -I test`, in the session scratchpad, not committed). `exact_rational(511.0)` returned 511/1, and `exact_rational(1024.0)` aborted with "exact_rational: the double does not fit the rational". The check is conservative, it fails loudly, and it is never silent. The WU14 tests' inputs, multiples of 2⁻²⁷ below 1, never reach it. But regime C's results are integers between 2⁵³ and 2⁶⁰, and a quarter of those between 2⁵⁹ and 2⁶⁰, or any smaller integer result that is a multiple of 2⁹, would stop the test.

The correction is part B's, in `tracked.hpp` with a test of its own (W9). For power ≥ 0, stop only when |significand| > (2⁶² − 1) >> power, or power > 62. It only accepts values the current code rejects, so no existing test changes. Without it, regime C cannot use integer inputs. It would need inputs on a fine grid, which risks a stop after cancellation, as the WU14 record accepted.

### Denominators of part B's changes

The denominators in `changes.out` come from this record's own LDLᵀ: the reference's `diagonalize` with R5's squarefree scaling (`changes.py`). Part B's `diagonalize.hpp` may pivot or scale differently, and then gives other denominators. Part B checks C and the denominators of its own changes against the conditions above before choosing inputs.

### Float and complex

- **float:** regime E only, with p = 24. 1.4's change meets C · N_E = 3 · 2²⁰ ≤ 2²⁴, and so does its round trip, 12 · 2²⁰ = 12,582,912 ≤ 16,777,216 (check 4). `tracked`'s helpers are written for double. No rounding tolerance for float is claimed.
- **std::complex<double>:** regime E with power-of-two denominators only, componentwise `==`. A product with the integer coefficient, (a + bi)(c + 0i), computes ac − b·0 and a·0 + bc, which are exact under Lemma E. Division by (q + 0i) with q a power of two is exact under each method I know: componentwise division, Smith's method (ratio 0, denominator q), scaling by powers of two followed by the textbook formula, and the textbook formula (a q + b · 0)/(q² + 0). The last needs one more factor q in Lemma E's condition, which 1.4's change meets with q ≤ 2 (6 · 2²⁰ ≤ 2⁵³). That the run-time library's complex division is one of these methods is recalled, not read [background]. If the complex test fails, the method is investigated, not tolerated.

## What each floating case of criterion 8 should be

| Case | Change | Inputs | Check |
| --- | --- | --- | --- |
| double, dyadic | 1.4 for d = 1 to 4, forward, back, round trip | regime E | `exact_rational(d) == e` on every blade |
| float, dyadic | 1.4, forward, back, round trip | regime E | the float result converted to double, then as above |
| complex<double>, dyadic | 1.4, forward and back | regime E, real and imaginary parts drawn separately | each part as above |
| double, one division | an LDLᵀ change with a denominator that is not a power of two (random 7 or random 2 → diagonal) | regime E | dyadic q_B: exact; otherwise d equals the correctly rounded e |
| double, sum and division | the same change | regime C | K = 0: exact; K = 1: correctly rounded; K ≥ 2: scaled ratio ≤ 1 + 4u |

Each case also checks that the tracked run's value equals the exact rational run's result on the same inputs, so the oracle and the library agree before the doubles are compared. Each also reports the largest ratio of regime C, as `rounding_test.cpp` does.

## What part B adds to `tracked.hpp`

Each addition has a present caller, the floating cases of criterion 8. Each comment states its derivation in its own words and cites no record (S11).

1. `operator/(tracked, tracked)`, the division rule above. It stops when the divisor is not exact or is zero, and it keeps the count when |divisor| is a power of two.
2. The correctly rounded value of a rational x whose denominator need not be a power of two. If x is dyadic (w = 1), it is `nearest_double(x)`, which rounds the numerator once and scales exactly. Otherwise it is fl(`nearest_double(w x)` / w), with w the odd part of the denominator, and it stops unless w x is exactly a double. The first commit of this record gave only the second form, which stops on a dyadic x that is not a double, such as (x_a + x_b)/2 with a 54-bit sum. That case is K = 1 in regime C, where the one rounding is the addition, and the model check below found it (correction 1).
3. The scaled ratio: `bound_ratio` with both sides multiplied by w, the least common multiple of the odd parts of the denominators of e and M. When w = 1 it is `bound_ratio`, so generalizing `bound_ratio` in place is also an option.
4. The check for one coefficient, by count: K = 0, exact; K = 1, correctly rounded (2); K ≥ 2, scaled ratio (3) at most `ratio_limit`. A regime-E test calls (2), or checks exactness, directly. Its premise is Lemma E's condition on the inputs, not the count.
5. The correction of `exact_rational`'s range check.

Each gets a test in `tracked_test.cpp`, including the stop on a rounded divisor and a value of 1024.0.

## `rounding.toml`: predictions, written before any run

| Mutant | Regime E, power-of-two q_B | Regime E, other q_B | Regime C |
| --- | --- | --- | --- |
| the division applied to each term before the sum | survives: x / 2ᵏ is exact either way | killed: each term's quotient rounds, then the sum rounds, so the result is not in general the correctly rounded quotient | survives: each term then has K = 2, the chain still gives K = m + 1 at most, and `tracked`, running the mutated templates, counts the same bound |
| the tolerance's division term dropped (K − 1 in the ratio; exactness expected in place of correct rounding) | not applicable: no rounding is counted | killed: e is not a double when q_B does not divide S_B | expected to survive: a worst-case bound is rarely reached; the numerical pilot saw medians of 0.03 to 0.08 and maxima up to 0.69 for such bounds (the first commit misquoted this as "worst ratios of 0.03 to 0.7") |
| every floating result times 1 + 2⁻⁴⁸ | killed: every nonzero result moves | killed | killed on coefficients where 32|e| exceeds the bound's γ_K M by enough, that is, with little cancellation; not on every coefficient |

So the regime-E case with a denominator that is not a power of two is the one that kills the first two mutants. The γ-bound case of regime C cannot kill them, by construction. Each regime-C prediction says "expected", because it depends on values that have not been seen.

## Model check, run after the rule's commit

The rule above was committed on this branch (`4414fe6`) before this check ran. Nothing in the rule was changed to fit the check's results. `model.py` models the assumed operation form in Python floats, which are IEEE double with round to nearest: the terms in ascending order of source blade, ±1 as an addition or a subtraction, `float(c) * x` otherwise, and then one division. It also models `tracked` with the division rule, and applies the rule's checks as stated, with the ratio computed in double as the helper would compute it. It is a model, not the library, which does not exist yet. So it shows that the rule is consistent with its own assumptions, and it shows what the mutants do to the model. It is not evidence about part C's code. It used seed 20261109, 400 draws per change and regime, and 200 round trips per d, and took 2.6 s under `tools/timed` (`model.out`).

| Change | Regime | Draws rejected: correct, per-term division, division term dropped, ×(1 + 2⁻⁴⁸) | Worst exact ratio, K ≥ 2 (median) |
| --- | --- | --- | --- |
| conformal(3) → R(4,1,0) | E | 0, 0, 0, 400 | — |
| conformal(3) → R(4,1,0) | C | 0, 0, 0, 400 | 0.5 (0) |
| R(4,1,0) → conformal(3) | E | 0, 0, 0, 400 | — |
| R(4,1,0) → conformal(3) | C | 0, 0, 359, 400 | no K ≥ 2 |
| random 2 → diagonal | E | 0, 386, 400, 400 | — |
| random 2 → diagonal | C | 0, 0, 400, 400 | 0.932 (0.075) |
| random 4 → diagonal | E | 0, 84, 363, 400 | — |
| random 4 → diagonal | C | 0, 0, 363, 400 | 0.638 (0.131) |
| random 7 → diagonal | E | 0, 400, 400, 400 | — |
| random 7 → diagonal | C | 0, 0, 18, 400 | 0.638 (0.051) |
| random 8 → diagonal | E | 0, 127, 338, 400 | — |
| random 8 → diagonal | C | 0, 0, 346, 400 | 0.609 (0.115) |
| random 9 → diagonal | E | 0, 330, 396, 400 | — |
| random 9 → diagonal | C | 0, 0, 1, 400 | 0.712 (0.109) |
| diagonal → random 7 | E | 0, 400, 400, 400 | — |
| diagonal → random 7 | C | 0, 0, 43, 400 | 0.742 (0.068) |

The round trip of 1.4's change in regime E was exact in all 200 draws for each of d = 1 to 4.

Findings:

- The rule never rejected the unmutated model. The worst exact ratio was 0.932, for random 2 in regime C, so the bound is close to attained there and leaves little room for an undeclared change of order. Because `tracked` follows the library's own tree, such a change is counted, not hidden.
- The per-term division was rejected only in regime E with a denominator that is not a power of two, as predicted.
- ×(1 + 2⁻⁴⁸) was rejected in every draw of every case. The model counts rejecting draws, not coefficients, so the prediction that regime C does not reject it on every coefficient was not checked.
- **Correction 1, to the correctly rounded helper** (item 2 of "What part B adds"): its first form stopped on a dyadic exact value that is not a double. This made the model's correct runs fail for 1.4's inverse in regime C. The corrected helper uses `nearest_double` when w = 1. This completes the recipe for the check the rule already states ("K = 1: correctly rounded"). It loosens nothing.
- **Correction 2, to the prediction for the dropped division term in regime C.** The table above said "expected to survive". In the model it is rejected when an output with K = 2 or K = 1 has a result that K − 1 does not admit. K − 1 then asks for correct rounding or exactness, which a sum that rounded and then divided does not give in general. It is rejected in most draws of the small changes and of 1.4's inverse, and in few draws of random 7 (18 and 43) and random 9 (1), whose outputs have larger K. The prediction holds only for coefficients with K ≥ 3. This is a correction of a prediction, not of the rule.

## Not covered

- Composition in regime C (a round trip through two divisions that are not by powers of two): w can then exceed 2⁹. Regime E covers the round trip for the dyadic change.
- Underflow, overflow, subnormal inputs, `-ffast-math`, flush-to-zero, and arithmetic that is not IEEE round-to-nearest.
- An implementation form other than the one assumed above (section "The computation").
- Random 3 → diagonal in regime C, which is outside the oracle's range.
- The tightness of the library's results, which do not exist yet. The model's ratios are in the section above.
