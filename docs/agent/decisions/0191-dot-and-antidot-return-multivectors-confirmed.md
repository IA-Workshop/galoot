# 0191 The return types of `dot` and `antidot` are confirmed as multivectors

- Status: accepted (owner answer 2026-11-07, relayed by coordinator: "proceed with recommendations", M3 gate report owner decision 12)
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), records clerk role
- Bears on: decision 0174 item 4 (plan 0020 O4); plan 0020 R6; plan 0001 M3 ("API names")

## Context

Decision 0174 ruled that `dot` and `antidot` return multivectors, on {1} and {𝟙} or an empty layout. Plan 0020 R6 asked for that to be confirmed at the M3 gate, with the operation inventory and the API names.

## Decision

Confirmed with the gate report's inventory: `dot(a, b)` and `antidot(a, b)` return multivectors on {1} and {𝟙}, or an empty layout when the result is zero by layout. The number is read with `get<"1">(dot(a, b))`.

## Consequences

None beyond decision 0174; no code or test changes.
