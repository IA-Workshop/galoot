# 0228 M5 gate rulings

Date: 2026-11-10. The owner replied "Accept all and proceed" to the report on the M5 audit (`records/gates/2026-11-10-M5-audit.md`) and on the open state of the plan 0028 review. Each item below quotes the options and the recommendation the owner accepted.

1. **Accept M5.** Options: accept once MA1's cherry-pick is on `develop` and MA2 is settled by item 2 (recommended); or accept now and carry MA1. Accepted as recommended. Every exit criterion of plan 0001's M5 row is met at `1aa1e40`; the suite passes on all eight presets from new trees and the 50-seed campaign passes (300 of 300). MA1 is closed (`02c66ab`). The acceptance takes effect when item 2's unit has merged; the coordinator records that in the roadmap addendum then.
2. **MA2.** Options: (a) before acceptance, one small unit writes the README section on a user's own algebra and a change of basis, compiled as a test (W12), does the N6 message pass (a test commit of its own for the changed pinned literal, a short blind review with the usability checklist), and writes the M5 snippets review; (b) accept now and make these the first units after the gate. Accepted: (a).
3. **m1.** Options: (a) record plans 0027, 0032, 0033 and 0034 as past departures and keep W5 and P9 as written; (b) amend W5 and P9 to allow narrower checks for comment-only and test-only merges. Accepted: (a). The auditor's eight-preset run at `1aa1e40` covers the four merges.
4. **m10.** Ratified: WORKFLOW's status line gains the sentences naming W2's widening (decision 0216 item 8) and W3's gap-mutant sentence (decision 0219 item 10).
5. **DoD-16 referents.** Options: (a) sufficient for M5 with the limits stated; (b) read a primary text for the matrix representations first. Accepted: (a). The Pauli, Dirac, quaternion and split-quaternion matrices rest on a secondary source; `golden.py` recomputes the defining relations and faithfulness from the matrices themselves, so a wrong matrix would fail.
6. **Decision records.** Options: (a) a later ruling goes into a new record, never into an existing one, and an owner-ruling record quotes the options and the recommendation it accepts; (b) allow appended, dated items. Accepted: (a), with no amendment. Decision 0226 item 5 stays as written.
7. **Agent cost.** No new rule. The coordinator adds the missing rows ("not visible" where unknown) and corrects the two "replaces" notes.
8. **Lessons.** Two: the seed campaign as the detector of test-arithmetic limits, and target-limited builds and `-L` selections.
9. **Pool review (plan 0028).**
   - L1, L2 and L4: a small tools unit adds the gate test, makes the service exit when its FIFO is missing, and makes `tools/timed` wait up to about 2 s while `<fifo>.stopping` exists; bundled with the `tools/mutate` items. Accepted.
   - L3: accepted as a known limit; "an unreadable fd directory" is added to the invariant's "not covered" list.
   - No restart of the shared pool. Accepted.
   - The roughly 57 s per preset cost of `tools.jobpool` is kept. Accepted.
