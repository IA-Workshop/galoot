# 0020 The source of the gamma_n forward error bound, read

- Date: 2026-11-04
- Author: Claude (agent, Opus 5.5), numerical-expert role, as implementer of the rounding tests of plan 0020 (WU11b).
- Branch: `feature/core-rounding`, from `develop` at `c86cb58`.
- Question: plan 0020 reading R15 bases the tolerances of the rounding tests on |d − e| ≤ γ_K M with γ_K = K u / (1 − K u), citing Higham, *Accuracy and Stability of Numerical Algorithms*, 2nd ed., Lemma 3.1, "to be confirmed by reading under V9 before it is cited in a test". Does the source state what the tests use, in that form, and what else from it does the margin of the check rest on?

## Source

N. J. Higham, *Accuracy and Stability of Numerical Algorithms*, second edition, SIAM, 2002. Read from the owner's local copy of the book (a PDF of the printed edition, 710 pages), at its printed pages 38 to 41 and 61 to 67 (PDF pages 66 to 69 and 90 to 96), on 2026-11-04. Pages 38 to 41 were read as text extracted with `pdftotext`; pages 62, 63, 66, and 67, which hold (3.1) to (3.5), Lemma 3.1, and Lemma 3.3, were also read as rendered page images, because the extraction garbles their symbols. This is the source itself, not a substitute.

## What the source states (V9)

| Claim the tests rely on | Where | Status |
| --- | --- | --- |
| The standard model: fl(x op y) = (x op y)(1 + δ), \|δ\| ≤ u, for op = +, −, ×, / | equation (2.4), p. 40 | Read. The text says IEEE 754 arithmetic, rounding to nearest, satisfies it ("With this default mode, the model (2.4) is obviously satisfied", p. 41). |
| Rounding a real x in the range of F: fl(x) = x(1 + δ), \|δ\| < u | Theorem 2.2, p. 38 | Read. The tests use it for the conversion of the exact value and of the magnitude to double. |
| If \|δ_i\| ≤ u, ρ_i = ±1, and n u < 1, then ∏(1 + δ_i)^ρ_i = 1 + θ_n with \|θ_n\| ≤ n u / (1 − n u) =: γ_n | Lemma 3.1, p. 63 | Read, as quoted (the proof is Problem 3.1 of the book, not read). |
| For an inner product in any order of evaluation, \|xᵀy − fl(xᵀy)\| ≤ γ_n Σ \|x_i y_i\| = γ_n \|x\|ᵀ\|y\| | equations (3.4) and (3.5), p. 63 | Read. The text says "It is easy to see that for any order of evaluation" this holds. |
| γ_k + u ≤ γ_(k+1) | Lemma 3.3, p. 67 | Read (proof: Problem 3.4, not read). The margin derivation uses it. |
| (1 + θ_k)(1 + θ_j) = 1 + θ_(k+j) | Lemma 3.3, p. 67 | Read. It is the rule the tracking scalar uses for a product. |

## How the tests use it (derived, not in the source)

The source proves (3.5) for an inner product. The tests apply the same argument to any straight-line program of +, −, and × over exact inputs, which is what a generated kernel is:

- Expand the computed result over the monomials it sums (products of inputs and integer constants). Each rounded operation multiplies every monomial below it by one factor (1 + δ). A sum adds one factor to each monomial of both operands, so a monomial's count is at most the larger operand count plus one; a product multiplies a monomial of one operand with one of the other, so its count is at most the sum of the two counts plus one. Negation and an exact integer constant add none. The tracking scalar computes exactly these counts, and K is their maximum.
- By Lemma 3.1, each monomial m_i appears as m_i (1 + θ_i) with \|θ_i\| ≤ γ_K, so \|d − e\| ≤ γ_K Σ\|m_i\| = γ_K M, where M is the same program run on absolute values with additions in place of subtractions. This is (3.5) with Σ\|x_i y_i\| replaced by M. [derived]
- The bound holds for every rounding that obeys (2.4), so it also holds when a compiler fuses a multiply and an add, which makes one rounding where the count has two. [derived]

The check's margin, the step from "d is within γ_K M of e" to a comparison computed in double that never rejects such a d, is derived in `test/support/tracked.hpp` beside the constant and in decision 0175. It uses Theorem 2.2, Lemma 3.3, and the monotonicity of rounding to nearest (x ≤ y implies fl(x) ≤ fl(y)), which the book uses throughout but which I did not find stated as a numbered result in the pages read; it follows from the definition of rounding to the nearest representable number. [derived]

## Checked numerically

Before the blind review, `core.rounding` compared 225,080 coefficients of double results with their exact values at seed 0 (and the same count at seeds 1, 2, 3, and 20261104), all within the bound. The largest observed ratio of \|d − e\| to γ_K M, per family, is in the WU11b records (`records/mutations/0020-rounding/README.md`); none exceeds 1. After the review it compares 262,920 per run and checks each family's largest computed ratio against 1 + 6u, a limit derived from Lemma 3.1 in decision 0175.

## Not verified

- The proofs of Lemmas 3.1 and 3.3 (Problems 3.1 and 3.4) were not read; the statements were.
- The second edition's errata were not checked for these pages.
