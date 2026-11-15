# 0194 WU13 merges to develop; the owner confirms the 4.4 and 2.1 passages; four rounding-check points stay open for the owner

- Status: accepted (coordinator record of the merge and of the owner's reply of 2026-11-07, "agree. proceed as recommended"). The four open points below are the coordinator's defaults, not owner rulings.
- Date: 2026-11-07
- Recorded by: Claude (agent, Sonnet 5.5), coordinator
- Bears on: plan 0021; decisions 0176 to 0180; CONVENTIONS 2.1, 4.3, 4.4, 4.5; WORKFLOW W5, W7, W9; CONSTITUTION C10, C18
- Branches: `feature/core-transform--rounding` at `c21417d`, which contains `feature/core-transform` (WU13a) and the WU13b work; merged into `develop` as `0fb3a57`

## What merged

WU13a (`compose`, `transform`, `antiproject`, CONVENTIONS 1.10) and WU13b (the rounding check of `transform`, decision 0180), after blind review and delta review of each part. The review records are `records/reviews/2026-11-07-wu13a-review.md`, `-wu13a-delta-review.md`, `-wu13b-review.md` and `-wu13b-delta-review.md`. All four verdicts ended at "approve with non-blocking findings"; WU13b's first review was a rejection on one blocking finding (`-ffinite-math-only` defeated the check), fixed and delta-reviewed.

The last full runs from empty trees, before the merge: gcc-debug 409, clang-debug 409, clang-measure 401. The release, asan and lint presets last ran at a revision with the same code (comment and test changes followed); they come due again at the next merge.

## Owner confirmation

The owner's reply covers the items put to them with the WU13a review: 4.4's sentence that the transform is λ^(k−1) times an outermorphism (the ratified text, "the outermorphism of V's action on vectors", is false unless V ⟇ V̰ = 𝟙), 2.1's two clauses on the antigrade involution, and the evidence labels "Checked for n = 3, 4, and 5" (4.4) and "Checked; Confirmed" (4.3). CONVENTIONS 3 and the 1.10 revision row now say so. Plan 0021's R10 claim is replaced by the theorem with hypothesis (H) in decision 0177.

## Open for the owner (not covered by the reply above)

1. O6, the chain limit: composing many rounded flat-plane motors in conformal(3) trips the check (0.2% of cases at 2 compositions, 2.2% at 8, 11.4% at 64, 22.7% at 128); correctly rounded versors never did. The reviewer judged this a sound limit, not a defect. The code stays as decision 0180 describes.
2. Overflow: a padded magnitude that overflows fails the check (decision 0180).
3. Scope of the `-ffast-math` and `-ffinite-math-only` errors: they stop all of `core.hpp` in a checking build. A narrower `static_assert` on the rounding branch is possible.
4. Coarse IEEE types (for example `std::bfloat16_t`) are left unchecked in every mode where the margin's hypothesis fails. This is narrower than the sentence in CONVENTIONS 4.5, which names every round-to-nearest IEEE binary type. 4.5 is unchanged here because a convention changes only by the owner's ratification (C10).

Also for the owner: decision 0193's route for B1 (a raised step limit on two sources) departs from decision 0184's grade subsets; and decision 0180's second addendum proposes recording, as standing practice beside decision 0185, that a test author's failing commit may be combined with its fix before anyone has seen it.

## Not done at this merge

The compile-record copy of decision 0080 (it needs trees rebuilt at the final revision) and the agent-cost row for the test-split implementer and reviewer.
