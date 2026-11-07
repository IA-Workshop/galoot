# 0196: The owner's rulings on the open items of the second handoff of 2026-11-07

Date: 2026-11-07. Status: recorded. Source: the owner's reply "Accept all recommendations" to the list of open items in `records/handoff-2026-11-07-second.md` (items 1 to 4, 7, 8 and 9).

## Rulings

1. CONVENTIONS 1.11 (4.5, the coarse-type clause) is confirmed as worded and no longer a draft.
2. O6 (research note 0022): add a renormalization helper, m minus (v·m/|v|²) v, in the M6 rigid and conformal models; apply the note's section 9 comment text to `transform`; no change to the bound and no componentwise margin. The comment text on `transform` is made as a comment-only change, listed for the next review.
3. WU9 audit finding A1: this decision supersedes decision 0183's statement that the WU9 fix pass changed only comments. The fix pass changed code in `product.hpp` and `plan.hpp` and so needed a delta review under the amended W5. The owner accepts it as covered by WU10a's later blind and delta reviews of `product.hpp`, an inferred mitigation. A2 (code and its judging tests changed together, W9) and the carried M2 (W11: WU9 began before the M2 gate was accepted) are past departures with no further action. Decision 0183 stays as written.
4. W7: the `records/<name>` row and the sentence on cherry-picked branches are ratified and added to WORKFLOW.md. The merge of `feature/core-transform--rounding` straight into `develop` (`0fb3a57`) is a past departure. `review/` and `gate/` are not retired.
5. Plan 0022 (WU15): O1 to O5 as recommended (oriented names declared by the algebra tag, otherwise the canonical name with a leading `-`; printed form `{e41: 1, e42: 0}` as a new CONVENTIONS 7.2; the misuse catalogue; a library message for `blades<Alg,...>` on a tag with no metric, with the STYLE S7 amendment; three new catch-alls); O6 to O9 take the plan's defaults. The CONVENTIONS and STYLE edits are made inside WU15 and need the owner's confirmation of their wording then.
6. Plan 0023 (WU16): O1 run WU16 before WU15 and before the scalar-conversion unit (M4 still waits for WU15); O2 alias templates; O3 no default scalar; O4 the public names of R2; O5 the motor and flector order of plan 0023 as a CONVENTIONS 5.8 labeled Confirmed only after the math-expert reads Lengyel's pages directly; O6 `translation(dx, dy, dz)`; O7 `rotation` not constexpr; O8 all 32 crystal classes with axes along [111], amending DoD-17's reading; O9 a short README compiled by a CTest.
7. Cleanup of merged worktrees and branches is approved, limited to branches git reports merged into `develop`; unmerged branches (test-author and review branches that are cherry-picked) stay until their commits are confirmed in `develop`.

## Not covered

Items 5, 6 and 10 of the handoff stay as recommended there (one blind review of the three unreviewed merges and one audit of 9d82f30 to a75ce5e at the M4 gate; the smaller items after WU16).
