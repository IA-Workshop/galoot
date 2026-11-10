# 0234 WU17 counting rule and part B outcome

Date: 2026-11-10. Coordinator rulings within plan 0038's scope, after part B (`research/spike-e`, checked independently in `records/reviews/2026-11-10-wu17-part-b-check.md`). The owner may reverse any of them.

1. **Counting rule.** The written rule in `records/spikes/spike-e/COUNTING-RULE.md` (rule K for the kernels) is the rule parts C and D follow.
2. **Comparison unit.** Multiply-adds are for comparison with published counts only, reported for every arrangement, secondary and never gated (decision 0013). The primary units are multiplications, additions and subtractions, negations, and doublings.
3. **Checking mode.** `golden.py` carries both the checking-off counts (48, 72, 48; compose 48) and the checking-on counts (68, 84, 76 multiplications plus compares). The test asserts the set matching the build's `GALOOT_CHECKING`; the counting scalar declares itself exact so the checking path runs.
4. **Negations.** Pinned with multiplications and additions/subtractions (the checker found them deterministic under the term order the plan enforces; a reordering needs the usual W9 update). This replaces the "informational only" proposal in the report.
5. **Default constructions.** The `T{}` per result place in each kernel call is counted separately from constructions from an integer and is not part of the pinned totals.
6. **Note 0027.** Presents the collected form as a per-batch feature (cheaper per object from 2 points, against the constraint form from about 5), names a batch-under-one-motor interface as the real D8 decision, and does not cite the assembly add/sub claim (withdrawn).
7. **Errata naming.** Lengyel's errata and CONVENTIONS 5.8 disagree on which of Q_vw and Q_mw is the scalar coefficient. Recorded in note 0027; no change to CONVENTIONS 5.8 (governance, owner only). Raised to the owner at the M7 gate.
8. **Model departure.** Part B's author ran as Sonnet by an error of the coordinator's launch; the independent check ran as Opus and agreed with every count.
