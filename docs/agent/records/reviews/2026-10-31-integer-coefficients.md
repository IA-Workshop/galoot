# Review: integer coefficients of the wedge-basis product (note 0009, B3)

- Reviewer: Claude Fable 5.1 (agent), blind-reviewer procedure, 2026-10-31. Revision read and run: `7d9505e487f8`.
- Subject: the claim that every coefficient of e_A e_B in the wedge-monomial basis is an integer for every symmetric integer form, and `test/reference/clifford_reference.py`.
- Departures: no `tools/timed` (it writes a tracked file); no diff to review, since the subject is a mathematical claim; the reviewer ran on Fable, not Opus; the reviewer's scripts were in a scratchpad and are now copied to `docs/agent/records/spikes/spike-b-review/`. Not checked: `fault_injection.py`, `diff_reference.py`, term-count claims, commit hygiene.
- Ambiguity: "blades as wedge monomials" admits a recursive definition (used by the note) and an antisymmetrized one (CONVENTIONS 1.1). The reviewer proved they coincide.

## Verdict

Approved. The claim holds, and is sharp. Findings:

1. The note's derivation takes the identity e_i x = e_i ⌟ x + e_i ∧ x as given. The load-bearing step, that the two operators satisfy the defining relation with g, was omitted. Supplied by the reviewer. Non-blocking.
2. The note does not say that the recursive and antisymmetrized blades are the same element. Supplied by the reviewer. Non-blocking.
3. Strong induction should be stated. The stronger result, that each coefficient is a homogeneous integer polynomial in the g_ij of degree (|A|+|B|−|C|)/2, answers the half-integer question.
4. The hypothesis that the largest coefficient equals |det g| is false. Counterexample g = [[−3,−1,−2],[−1,0,0],[−2,0,−1]]: det g = 1, but e_1 e_12 has coefficient −3. 61 of about 300 seeded forms violate it.
5. One "Consequence" bullet labeled Exhaustive covers a Derivation.
6. CONVENTIONS 4.1's wedge wording is the diagonal-metric rule and contradicts 1.1 in conformal(3).

Resolutions: findings 1 to 5 are applied in note 0009 (B5). Finding 6 is decision 0023.

## What the reviewer ran

An independent Chevalley-route check over 40 seeded integer forms (n = 2..5, 8 forced degenerate) and 24 half-integer forms; the script's output matched the note's table line for line; tables compared on the 14 note signatures and 9 half-integer forms with no mismatches. Commands are in the scripts' headers. Times are tracked, not asserted.
