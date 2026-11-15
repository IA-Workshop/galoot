# 0008 Collected plans for repeated inputs, decided on evidence at M7

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner, accepting plan 0001 section 7 (D8). Recorded by Claude (agent).

## Context

C6 requires results to hold only components that can be nonzero. Composed sandwiches keep structurally cancelling components. Collected plans are an exact post-pass on composed term lists, not a second generator (C4).

## Decision

Collected plans are admitted under C4 as composition plus exact collection. Whether they are required in 1.0 is decided at M7 on the results of Spike E and the P2 run.

## Consequences

Until M7 the models use direct-composition transforms with the declared-output check of C8.
