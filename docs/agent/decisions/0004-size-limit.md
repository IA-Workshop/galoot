# 0004 Size limit: n up to 16, flexible template parameter

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner, accepting plan 0001 section 7 (D4). Recorded by Claude (agent).
- Owner amendment: agree, but leave it flexible as a template parameter.

## Context

The design study is inconsistent: a 16x16 metric matrix but masks up to 32.

## Decision

v1 accepts 1 <= n <= 16, tested exhaustively to n = 6 and sparsely at 8, 12, 16. The limit is a template parameter, not a hard-coded constant, so it can be raised without an interface change. n = 0 is rejected. The mask type is std::uint32_t unless Spike A shows otherwise.

## Consequences

No bound is asserted beyond what Spike A measures and the documentation states.
