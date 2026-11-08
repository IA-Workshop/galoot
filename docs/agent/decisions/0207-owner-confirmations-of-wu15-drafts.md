# 0207 The owner confirms WU15's drafts and readings

- Status: recorded. Source: the owner's replies of 2026-11-08, "Confirmed the recommendations" and "All others approved as recommended", to the open items of plan 0022 after its parts 1 to 3 and records.
- Date: 2026-11-08
- Recorded by: Claude (agent, Opus 5.5), implementer of WU15
- Bears on: CONVENTIONS 7.2 and its version 1.14, CONVENTIONS section 8; STYLE S7; decisions 0204, 0205, and 0206; `records/mutations/0022-format/README.md`

## Confirmations

1. **CONVENTIONS 7.2**, the printed form, is confirmed in the wording drafted in decision 0204. Its version, 1.13 on this branch, became 1.14 when `develop`'s 1.13 (5.7's `rotation`, decision 0210) was merged in. The status line and the 1.14 row no longer call it a draft.
2. **STYLE S7**'s sentence extending the weaker-concept pattern to variable templates is confirmed in the wording drafted for decision 0206. The status line of STYLE no longer calls it a draft.
3. **The catalogue's scan** stays a record tool, as decision 0205 item 4 says, not a CTest or a merge gate.
4. **`blades` keeps two messages**, one for a tag that is not an algebra and one for a metric with no generators, as decision 0206 item 3 says, in place of the plan's one.
5. **The equivalent survivor** `other_source_widened_to_same_types` stays a survivor, and the rejecting constructor keeps its constraint `!same_as<OtherAlg, Alg> || !same_as<U, T>`, for the reason the mutants' record gives.
6. **CONVENTIONS section 8, the "Flector transform" row** (M4): the owner approved adding to its Test column the two C++ tests that tell the single-sign form apart, `test/core/transform_test.cpp`'s "a reflected point keeps a positive weight" and `test/rga3/objects_test.cpp`'s "the rga3 Flector e412 reflects (1, 2, 3, 1) to (1, 2, -3, 1) with weight +1". The edit is part of version 1.14 and is named in its row.

## Consequences

- Decisions 0204 and 0206 stand; their statements that the wording awaits the owner are closed by this decision.
