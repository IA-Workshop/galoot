# 0186 C8 states the reading of decision 0168

- Status: accepted and ratified (owner answer 2026-11-07, relayed by coordinator: "proceed with recommendations", M3 gate report owner decision 5). CONSTITUTION C8 amended by this record.
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), records clerk role
- Bears on: constitution C8; decision 0168 (confirmed the reading, did not amend C8); decision 0174 item 2 (the C8 amendment left out of CONVENTIONS 1.9); CONVENTIONS 4.5; plan 0021 R8 and R9

## Context

C8 said "A checking mode verifies that whatever an operation discarded was zero". Read literally, it forbids every operation that drops nonzero components by design, such as the bulk, the weight, `part`, and grade parts. Decision 0168 confirmed the reading that the check covers what an operation assumes is zero, and left C8's text unchanged. WU13's `transform` will be the first operation to rely on C8's check for rounding scalars.

## Options weighed

1. Amend C8 with one clause stating decision 0168's reading (recommended).
2. Keep the text and rely on decision 0168.

## Decision

Option 1. C8's sentence becomes: "A checking mode verifies that whatever an operation assumed to be zero, and discarded, was zero."

## Consequences

- The constitution's text now agrees with decision 0168 and CONVENTIONS 4.5. No code or test changes.
