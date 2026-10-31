# 0023 Wording of the wedge in CONVENTIONS 4.1

- Status: accepted; ratified by the owner on 2026-10-31 ("proceed with recommendations on the 4.1 ruling"), and CONVENTIONS is now version 1.4
- Date: 2026-10-31
- Proposed by: Claude (agent), from the blind review of 2026-10-31 (finding 6)

## Context

CONVENTIONS 4.1 says the wedge a ∧ b keeps the terms of a ⟑ b whose blades share no generator. That is the diagonal-metric rule. In conformal(3), e4 and e5 share no generator, so the rule would give e4 ∧ e5 = e4 ⟑ e5 = −1 + e45, but 1.1 defines e45 = e4 ∧ e5.

## Options

1. Keep the wording and add "in a diagonal metric". It leaves the non-diagonal wedge undefined.
2. Define the wedge as the grade |A| + |B| part of e_A ⟑ e_B for wedge-monomial blades (grade projection).
3. Define it as the exterior product on the blade basis, extended bilinearly, independent of the metric.

## Decision proposed

Option 3, with option 2 stated as a theorem: "The wedge a ∧ b is the exterior product, defined on blades by e_A ∧ e_B = ± e_{A∪B} when A and B are disjoint and 0 otherwise, with the sign of the reordering. It equals the grade |A|+|B| part of a ⟑ b."

## Consequences

CONVENTIONS is version 1.4. No code depends on it yet.
