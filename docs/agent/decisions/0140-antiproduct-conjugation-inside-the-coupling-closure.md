# 0140 The geometric antiproduct is conjugated inside the coupling closure of its use

- Status: proposed, 2026-11-01. Takes effect only if the owner accepts it; nothing is implemented.
- Date: 2026-11-01
- Proposed by: Claude (agent, Opus 5.5), math-expert in the researcher role, from research note 0018 (branch `research/antiproduct-growth`).
- Bears on: decision 0115 (the antiproducts as conjugated plans), decision 0116 item 2 and its consequences (the growth at fixed use, "the antimetric pair rule is the first candidate, with its own decision under C4"), decision 0005 (the reading of P2), constitution C4 and C6.

## Context

WU7b generates the geometric antiproduct of two blades as the left complement of the geometric product of their right complements, over all n generators (CONVENTIONS 4.2, decision 0115). For low-grade blades the complements have grade near n, so the work per pair grows as n² and the term lists get capacity 2^(n−1). Decision 0116 measured a fixed use, the four typed rigid(3) pairs embedded in R(n−1,0,1), at +268% (GCC) and +463% (Clang) from n = 4 to 16, and left to the owner whether that growth follows from the algebra (P2's exception, decision 0005) or is a defect against C6.

Note 0018 decides it with mathematics:

- Theorem 1: the antiproduct is the geometric product of the same form on the antivectors (Lengyel's own statement); WU7b's conjugation is exactly that formulation, so "the antimetric route" in the sense of Gram data on complements is the construction already built.
- Theorem 2: call generators coupled when the form pairs them (g_ij ≠ 0, i ≠ j), and let C be the union of the coupling components that the use's blades touch, Q the rest. For blades on C, e_A ⟇ e_B = γ (−1)^(|Q|(|C| + |A|)) τ(C, Q) Σ_E a_E τ(E, Q) e_(E ∪ Q), where Σ a_E e_E is the antiproduct inside the algebra on C and γ = e_Q e_Q is a scalar.
- Theorem 4: any correct method must read the form outside C (the value depends on it), but by Theorem 2 only once, as γ; per term nothing beyond the product inside C is needed. Where C itself grows with n (a dense form), the result grows too, and that growth is the algebra's own.
- Multiplying the original blades with the antimetric on vectors is not the antiproduct (it gives 0 where the antiproduct is −𝟙 in rigid(3)), so it is not an option.

All of this was checked exactly against the definition on 113 forms (168,764 pairs, no difference), and a prototype on WU7b's generator equals WU7b's plans on every case checked (note 0018 sections 3 and 5).

## Options weighed

| Option | Growth of decision 0116's use, n = 4 to 16 | Forms covered | New machinery | Notes |
| --- | --- | --- | --- | --- |
| A. Keep the conjugation over all n; record the growth as P2's exception | as measured: +268% / +463% | all | none | The growth is not the algebra's (Theorem 4), so the exception would admit a defect |
| B. Conjugate inside the coupling closure C of the use (Theorem 2) | +9.0% / +10.8% with C per plan; the plans alone +0.7% / +4.3% with the coupling data per algebra, which itself grows by 30.6 / 44.1 M (note 0018 section 6) | all | none: blade_product unchanged, on complements inside C, with per-basis masks and signs and one coefficient per plan | Recommended |
| C. A closed-form pair rule for diagonal forms (note 0018 Theorem 3: a sign and a mask from the antimetric diagonal) | flat in loop count (inferred) | diagonal only; non-diagonal forms keep A's growth | a second pair rule beside blade_product (C3 replacement) | Kept as a check, not as the construction |
| D. The original masks with the antimetric on vectors | — | none (wrong) | — | Rejected: not the antiproduct |
| E. Cache conjugated plans per algebra | unchanged | all | a cache | Does not remove the work; moves it |

## What composition shows (C4)

C4 admits new core machinery only where composing the one mechanism with exact per-basis data cannot express the capability. B adds no machinery; it is that composition:

1. The mechanism is `blade_product(g, X, Y)`, unchanged, called with X = C ∖ A and Y = C ∖ B. It reads only entries of g inside C, because its recursion touches only the generators of X and Y.
2. The per-basis data are exact masks and signs: for an input blade A, the mask C ∖ A with sign r_C(A) (−1)^(|Q||A|), where r_C is the right-complement sign inside C (`reorder_sign(A, C ∖ A)`); for an output term on D, the mask (C ∖ D) ∪ Q with sign l_C(D) τ(C ∖ D, Q) (`reorder_sign(C ∖ D, D)` and `reorder_sign(C ∖ D, Q)`); for the plan, one coefficient γ (−1)^(|Q||C|) τ(C, Q).
3. γ is the product, over the components K of Q, of (−1)^(|K|(|K|−1)/2) e_K e_K, times (−1)^(|Q|(|Q|−1)/2); each e_K e_K is `blade_product(g, K, K)`. So even the scalar comes from the one mechanism.
4. The only data read that WU7b does not read is the coupling graph, the pattern of nonzero off-diagonal entries of g, which is part of the signature.
5. WU7b's conjugation is the case C = all generators (Q empty, γ = 1, every added sign +1), so the present construction remains the reference against which B is verified (C12) by calling the same functions with C = all generators.

## Proposal

Option B, built after WU7b merges, by a unit planned for it (or folded into WU10, as the owner prefers):

1. `plan.hpp`: the antiproduct's `factor_of` and `run_term` take the closure C and its complement Q (with γ) instead of n alone; `generate_plan<antigp>` applies the signs of item 2 above; `pair_capacity` and `plan_capacity` for the antiproduct use `product_capacity(C ∖ A, C ∖ B)`; `make_product_plan` computes C, Q, and γ as `static constexpr` locals before the count. The antiwedge's plan is empty unless Q is empty (its "form" is zero, so γ = 0), which the same step states.
2. Where the components come from: once per algebra (recommended): a per-tag variable template holding each generator's component mask and each component's determinant, O(n²) once, then one step per used generator and per outside component in each plan; measured, the plans then grow by +0.7% (GCC) and +4.3% (Clang) from n = 4 to 16, and the data by 30.6 and 44.1 M, which could share `metric_like`'s single pass. Or per plan: O(n²) per distinct set of used generators. The owner chooses.
3. Tests: the reduced plan against the conjugation with C = all generators, at run time, for every pair of blades of every form of plan 0001 section 5, with Witt and conformal pairs placed outside C and with n − |C| odd (the cases that the component-determinant sign and the (−1)^(|Q||A|) sign alone affect); WU7b's golden composition (its D5) unchanged; compile time equal to run time for the typed pairs; the four sign mutants of note 0018 as mutation cases.
4. Records: the P2 rerun of decision 0116's use, and the limits rerun (`full<n>` sets have C = all generators and should not change).
5. Optional: Theorem 3's closed diagonal sign, τ(complement of B, complement of A), as a generation-time second route for diagonal forms, in the way `closed_form_right_sign` checks the complements.
6. CONVENTIONS is unchanged: 4.2 still defines the antiproduct; Theorem 2 is a property of that definition.

## For the owner

1. Is the growth of decision 0116 a defect to fix (note 0018: it is an artifact, so P2 records it as one), and when: after WU7b's merge as its own unit, or within WU10?
2. Does the showing above satisfy C4?
3. Components per plan or per algebra (item 2)?

## Consequences if accepted

- The antiproduct of a fixed use costs what the product of that use costs in the algebra of its coupling closure, at any n. What still grows with n is one pass over the form, of the order of the signature data that decision 0005 admits.
- A use whose blades touch a large coupling component (a dense form) still grows with that component: the result itself grows (note 0018 Theorem 4(c)), which P2's exception covers.
- Decision 0116's consequence "the antimetric pair rule is the first candidate" is answered: the candidate that works is conjugation inside the closure, not a rule on the original blades with the antimetric.

## Owner ruling, 2026-11-01

The owner deferred this remedy (decision 0141): the conjugation stays, the growth is a recorded exception to C6 for twelve months, and this decision stays proposed until the exception ends or an issue is found in production.
