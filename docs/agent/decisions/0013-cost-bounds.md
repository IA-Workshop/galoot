# 0013 No run-cost or build-cost bounds in v1; track, don't assert

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner, accepting plan 0001 section 7 (D13). Recorded by Claude (agent).
- Owner amendment: agree; track, but don't assert.

## Context

The design study set a 10% target against Terathon and CI compile-time budgets.

## Decision

No cost bounds apply in v1 (C14). Build time, memory, test time, and kernel cost are recorded and compared with history (P7), never asserted or used to fail a check. The 10% target is replaced by plan 0001's DoD-24.

## Consequences

Reports state measurements and growth; the owner sets any bound later.
