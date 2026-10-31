# 0010 Checking mode on rounding scalars

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner, accepting plan 0001 section 7 (D10). Recorded by Claude (agent).

## Context

C8 requires a checking mode that verifies discarded components were zero. With rounding scalars exact zero is the wrong test (V13).

## Decision

Exact scalars check for exact zero. Rounding scalars check that the discarded magnitude is within the plan's forward error bound.

## Consequences

Error-bound logic enters the core; a wrong bound gives false aborts. Tests cover both directions.
