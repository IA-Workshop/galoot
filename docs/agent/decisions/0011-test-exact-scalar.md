# 0011 Test-only exact rational scalar

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner, accepting plan 0001 section 7 (D11). Recorded by Claude (agent).
- Owner amendment: agree; if arbitrary precision becomes necessary, escalate and notify.

## Context

V11 needs an exact scalar for exact equality. Candidates: GMP, Boost, an owned type.

## Decision

An owned, test-only, overflow-trapping int64 rational. No GMP or Boost. If arbitrary precision becomes necessary, work escalates to the owner with a notice before any dependency or larger type is added.

## Consequences

Randomized exact tests bound their inputs; overflow fails the test rather than wrapping.
