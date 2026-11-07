# 0190 `conventions_check.py` checks the bulk and the weight of other diagonal forms, so CONVENTIONS 3.2's label is true

- Status: accepted (owner answer 2026-11-07, relayed by coordinator: "proceed with recommendations", M3 gate report owner decision 8). The change is made in WU13a's step 1.
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), records clerk role
- Bears on: CONVENTIONS 3.2 ("(Confirmed, with the diagonal case Checked)"); WU11a review finding F4; plan 0020 outcome; the M3 gate's m11; plan 0021 section 4 step 1

## Context

3.2's label says the diagonal case is Checked, but `test/reference/conventions_check.py` checks only rigid(3)'s masks. The factor by which version 1.9's sentence multiplies each kept blade in other diagonal metrics is checked by the C++ tests against golden values, not by the script.

## Options weighed

1. Extend the script to diagonal forms with squares other than 0 and 1: G from the squares, and the factor on each kept blade (recommended). WU13a's step 1 already changes the script.
2. Relabel 3.2 in CONVENTIONS 1.10.

## Decision

Option 1, in WU13a's step 1, by the math-expert, with the script's own forms and no code shared with the library. If the check is not added, 3.2 is relabeled in 1.10 instead.

## Consequences

- Plan 0021 step 1 gains this check.
