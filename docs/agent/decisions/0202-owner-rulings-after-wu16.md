# 0202: The owner's rulings on the open items after WU16

Date: 2026-11-08. Status: recorded. Source: the owner's reply "Accept all recommendations" to the list of open items after the merge of WU16 (`9097500`).

## Rulings

1. S1 (hci reading, WU16): `rotation(l, theta)` normalizes the direction of its axis, so a non-unit axis no longer turns by the wrong angle. This is a behavior change to a public function and is made as its own small unit: tests first, then the code, a delta review, and CONVENTIONS 5.7 and the `rotation` comment reworded to match. The cost is one square root per call and a possible change in the last bit.
2. S2: `compose` of two operands of one layout returns that layout, as core work in a later unit before release.
3. S3: `transform(motor, point)` is documented now (two lines in `transform.hpp`'s comment and the README if it fits); a core catch-all that rejects a swapped argument order is part of the same later core unit as S2.
4. CONVENTIONS 5.8's coefficient letters (Q_v, Q_m, F_p, F_g) and 5.7's label as committed in 1.12 are confirmed.
5. The Wikipedia substitutes for the International Tables and the Denavit-Hartenberg paper are accepted for M4; the outcome of plan 0023 states the gap.
6. Handoff items 5, 6 and 10: one blind review of the three unreviewed merges (`feature/jobpool-idle-shutdown`, `feature/runlog-0170`, `feature/test-split`) and one auditor pass over 9d82f30 to a75ce5e at the M4 gate; the smaller items wait until after WU15.
7. Cleanup of the merged WU16 worktrees and branches is approved, limited to branches git reports merged into `develop`.
