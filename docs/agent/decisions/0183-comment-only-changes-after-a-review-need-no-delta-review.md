# 0183 A comment-only change after a review needs no delta review if the review record lists it; W5 amended

- Status: accepted and ratified (owner answer 2026-11-07, relayed by coordinator: "proceed with recommendations", M3 gate report owner decision 2). WORKFLOW W5 amended by this record.
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), records clerk role
- Bears on: decision 0094 item 3 (a delta review after any fix pass on core code); the post-WU10 audit's M1 and owner item 2; the M3 gate's m3 (`f4d85f8`, a sentence of `dual.hpp`'s comment rewritten after the WU11a review, merged with no delta review); WORKFLOW W5

## Context

Decision 0094 item 3 requires a delta review after any fix pass on core code. WU9's fix pass and WU11a's `f4d85f8` changed only comments in `include/` after the review and merged with no delta review. The audits asked whether a comment-only change needs one. WORKFLOW did not state the delta-review rule itself; it lived only in decision 0094.

## Options weighed

1. A comment-only change to `include/` after a review needs no delta review if the review record lists it with its resolution, as WU11a's record does for F6 (recommended). Cheaper than a delta review and no less visible.
2. Every change to core code after a review, comments included, gets a delta review.

## Decision

Option 1. W5 gains one sentence: "A fix to core code made after its review gets a delta review of the fix before the merge (decision 0094), except a change only to comments, which the review record lists with its resolution."

## Consequences

- The WU9 fix pass (post-WU10 audit M1) and `f4d85f8` (M3 gate m3) are settled: each is listed in its review record.
- A change that touches code as well as comments still gets a delta review.
