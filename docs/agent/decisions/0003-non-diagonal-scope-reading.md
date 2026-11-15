# 0003 Non-diagonal forms are in scope through exact change of basis

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner, accepting plan 0001 section 7 (D3). Recorded by Claude (agent).

## Context

C2 says basis elements multiply to a single basis element times a coefficient. In conformal(3) the stored-basis product e4 e5 = -1 + e45 has two terms.

## Decision

C2 covers non-diagonal forms because they are exact changes of basis from a monomial (diagonal) algebra, which C2 names. Blades in a non-orthogonal metric are wedge monomials (e45 = e4 ^ e5). C2's text is unchanged. CONVENTIONS 1.1 gets matching wording (D9).

## Consequences

The core interface carries a form plus an exact change of basis. Plan coefficients are integers; rationals appear in the change of basis and form entries.
