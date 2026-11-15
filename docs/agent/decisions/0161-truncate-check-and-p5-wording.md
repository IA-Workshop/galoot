# 0161 `truncate` checks exact scalars only; P5 says "trivially copyable"

- Status: accepted by the owner, 2026-11-01 ("proceed as recommended")
- Date: 2026-11-01
- Decided by: the owner; recorded by Claude (agent, Sonnet 5.5)
- Resolves: open questions O1 and O2 of plan 0016

## Decision

1. CONVENTIONS 4.5 (now version 1.7): a conversion that drops blades verifies in checking mode that what it drops is zero when the scalar is exact. An operation that drops part of its own result, such as `transform`, verifies on a rounding scalar that the dropped part is within that operation's forward error bound (decision 0010). A standalone `truncate` has no plan and so no error bound to check against; promising a check there would be dishonest.
2. PERFORMANCE P5: "Values are trivially copyable (STYLE S8)", where it said "trivially copyable aggregates". The amended S8 makes multivectors non-aggregates; P5 only needs the copyability.

## Consequences

- WU8b implements the exact-scalar check of `truncate` and the rounding-scalar behavior of criterion 16 as ruled; it no longer waits on wording (plan 0016, R1).
- The convention trace and the tests that cite 4.5 are checked at the next merge (the trace gate ran at this commit).
