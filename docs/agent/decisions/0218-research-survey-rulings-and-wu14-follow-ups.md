# 0218: The owner's rulings on the research survey and the WU14 gate items

Date: 2026-11-08. Status: recorded. Source: the owner's reply "Proceed as recommended and accept" to the open items after the WU14 merge (db27dd3).

## Rulings

1. A Theorem R structure-layer family is added to the identity suites, keeping the triple loop. It follows WU14 as a small follow-up unit with its own plan.
2. Two off-axis degenerate forms, `[[1,1,0],[1,0,1],[0,1,-1]]` and the all-ones rank-1 form, are appended to the form lists (`test/core/signatures.hpp`, the identity form lists and `small_forms`), so existing draws do not change.
3. A research note cites the math-tools proofs, after the `proof-harness` is rerun on current `develop`.
4. Sage Route A (with `lift_isometry`) is WU12's independent oracle; the WU12 plan says so.
5. Decision 0217 stands with its in-place dated corrections (the criterion 12 reading, item 6, accepted as "not met as written, replaced by the argument"). Later plans amend criterion 12 so a gap's mutants are judged by the suite the gap extends.
6. `tests.messages_pinned` also takes the `core.headers` label so header changes select it (about 0.1 s per run).
7. Merge `878e190`'s missing agent identification is noted and left; fixing it would rewrite history.
8. Decision 0217 is ratified.
