# 0189 `identities_test.cpp` is split before WU13 adds its identities

- Status: accepted (owner answer 2026-11-07, relayed by coordinator: "proceed with recommendations", M3 gate report owner decision 7). The split is a separate unit.
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), records clerk role
- Bears on: the post-WU10 audit's owner item 9; the M3 gate's info 1; PERFORMANCE P1, P8, W10; plan 0021 criterion 4

## Context

`test/core/identities_test.cpp` is the critical path of every build: at `b504aa7`, 295 G instructions and 19.2 s of a 19.4 s gcc-debug build, and 111.3 s of a 111.9 s gcc-asan build at 3.5 GB. Any change to the products or the unary operations selects it (P8).

## Options weighed

1. Split it now, as a small build-lane unit with before and after costs, by form family, before WU13 adds identities (recommended).
2. Leave it, and put WU13's identities in their own unit only.

## Decision

Option 1. The split is its own unit, with W10's before and after costs recorded, and merges before WU13 does.

## Consequences

- The unit changes only test files and the test build lists; no test is weakened or removed (W9), and each case keeps its labels.
- WU13's identities go in their own unit, as plan 0021 already allows (`transform_identities_test.cpp`).
