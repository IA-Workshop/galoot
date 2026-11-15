# Plan closures for DoD-28 (draft)

- Status: applied 2026-11-11 to every plan except 0001, 0038 and 0042 (plan 0042 unit U5): status lines corrected from merge commits, and outcomes added to plans 0024, 0027 and 0028. Plans 0040 and 0041, written after this draft, got the same status correction. The table below is the draft as written and is kept as history. Open items that no record resolves stay open in the plans' outcomes (plans 0027 and 0028). Updated 2026-11-11 by plan 0042 unit U6, from the M7 gate audit's m9: plans 0018, 0036 and 0039 name their merges (`19273a6`, `80f4c6e`, `7b85b33`), and the outcomes of plans 0027 and 0028 now cite decision 0228 item 9 (the restart and the cost of `tools.jobpool`) and decision 0248 item 8 (their I-findings accepted as known limits), so neither plan has an open item. Plan 0038's outcome was corrected for the audit's m2. Updated 2026-11-12 by plan 0042 unit U11: plan 0001 has an outcome (M8's acceptance is the owner's), plan 0042's status line and outcome are finished except the coordinator's merge facts, and plan 0019's status line is corrected after tracing section C's check (the post-WU10 audit's owner items, all but two traced to decisions 0182 to 0192; the two are named there).
- Written: 2026-11-10 by Claude (agent, Sonnet 5.5) at `develop` `1ed0e6a`, from the status line, headings, and end of each of the 39 plans in `docs/agent/plans/`, and `git log --merges` on `develop` for merge state.
- DoD-28 asks for "plans closed with outcomes". W3 wants each plan to end with an outcome.

## Departures and unknowns, first (C20)

1. I read each plan's status line, section headings, and tail, not every body. "Outcome present" means a heading containing "Outcome". I did not judge whether an outcome is complete against its plan's criteria.
2. Merge state comes from branch names and merge commit subjects (`git branch --merged develop`, `git log --merges`). Plans 0002, 0003, 0004, 0005, 0006, 0007, 0008, 0009, 0010, 0012, 0014, 0015, 0017 have merged branches in that list, but I did not match every plan to its merge commit; where I say "merged" for those I mean the named feature branch is merged into `develop`.
3. Whether an owner decision closed a plan outside the plan's own text (for example gate acceptance) was not searched beyond the gate addenda in plan 0001.

## A. Plans with no outcome section

| Plan | State | What is missing |
| --- | --- | --- |
| 0001 core v1 roadmap | Addenda record M0 to M5 gates. No outcome section. | W11 says owner acceptance is recorded as the outcome here at each gate. Missing: M6 (deferred past v1 by decision 0211: say so), M7 (cost evidence; D8 ruled by decision 0239; DoD-22 to DoD-25 not yet shown), M8 (release). D7 and D8 rows still read as open; section 11 "Next step" is stale (the addendum for 2026-11-01 already says so). A closing outcome with the DoD-1 to DoD-29 result, written at release. |
| 0024 rotation-normalize | Status "claimed 2026-11-08", "Not merged by its author; a delta review follows". Two "State" sections. Merged by `a92c551`. | An Outcome section; a status line that says merged, with the delta review's record and the revision. The decision (0203) and the mutant record exist and could be cited. |
| 0027 pool-runlog-repairs | Status says merged by `d0b84bc` (corrected 2026-11-10). "State" sections for the session, the blind review, and the delta review. | An Outcome heading summarizing the final state; open minor findings from the delta review (I2, I3, I5) are "known, no code" and need an accept or fix ruling stated. |
| 0028 pool-surplus-fix | Status says merged by `2adc87f`. "Resolution of the review's findings" table. | An Outcome heading. Review findings L1, L2, L4 were resolved by plan 0036; L3 accepted (decision 0228 item 9); I5, I6, I7 and owner items 1 and 4 have no recorded resolution (the plan says so itself). |
| 0038 WU17 cost evidence | Status "claimed ... No implementation has started"; Section 13 "Next step" says the coordinator will launch parts A and B. In fact parts A to E (counting, benchmarks, E1, E2 with a rerun, E3, the inline reference kernels) merged: `43cc6da`, `1e573ad`, `5444671` and others. | An outcome per part (A to H) with results and records pointers; which parts remain (F, G, H, and the pre-release inlining pass of decision 0240 item 1, with E1, E2, G, H rerun); a corrected status line; the answers to open owner questions O2 to O10 (decisions 0233, 0237, 0238, 0239 hold them). |

## B. Outcome present, but the status line is stale or contradicts it

The outcome exists; the status line still reads as in-progress or "pending review". These need a one-line status correction, not new work. Merge state is stated where I could see it.

| Plan | Status line says | Evidence it is done | What is missing |
| --- | --- | --- | --- |
| 0002 wu1 build skeleton | "implemented on `feature/build`; criterion 15 is the merge" | `feature/build` merged | Status line to say merged |
| 0003 recorder followups | "awaiting the W5 review and the merge" | `feature/recorder` merged; review records `2026-10-31-recorder*.md` exist | Status line |
| 0004 wu2 bits | "pending W5 review and merge" | `feature/core-bits` merged; reviews `wu2-bits*.md` | Status line |
| 0005 wu3 metric | "W5 review and the merge are the coordinator's" | `feature/core-metric` merged; reviews exist | Status line |
| 0006 m0 closure | "closed (outcome below)" | `feature/m0-scripts` merged | None beyond confirming |
| 0007 cleanup-1 | "awaiting the W5 review and the merge" | `feature/cleanup-1` merged; reviews exist | Status line |
| 0008 wu4 blade sets | "pending W5 review and the merge" | `feature/core-blade-set` merged | Status line |
| 0009 blade-set merge fix | "pending W5 review and the merge" | `feature/blade-set-merge-fix` merged | Status line |
| 0010 compile-fail lane | "claimed 2026-11-01" | `feature/compile-fail-lane` merged; outcome present | Status line |
| 0011 wu6 blade product | "Pending: the delta review of the header's comment and attribute changes, and the merge" | `feature/core-gp-plan` merged; delta review not confirmed by me | Status line; whether the delta review happened (unknown) |
| 0012 blade-product cost growth | "awaiting the W5 review" | `feature/product-cost-growth` merged | Status line |
| 0013 wu7 plan results | WU7b "W5 review accepted it after fixes" | `feature/core-plan-results` and `core-plan-maps` merged | Status line to say both merged |
| 0014 metric-like single pass | "awaiting the W5 review" | `feature/metric-like-single-pass` merged | Status line |
| 0015 plan limits certified | "awaiting the W5 review" | `feature/plan-limits-certified` merged | Status line |
| 0016 wu8 multivector | "Not started" (revised for rulings) | Outcome WU8a and WU8b present; `feature/core-mv-a`, `core-mv-b` merged | Status line |
| 0017 runlog distilled | "Not merged" | `feature/runlog-distilled` merged | Status line |
| 0020 wu11 duals | "planned 2026-11-02 ... Closed 2026-11-07: WU11b ..." | Merges `535c91e`, `b504aa7` | Read the full status line: partly updated; confirm it names the merges |
| 0021 wu13 transform | "planned 2026-11-04 ... not started" | Outcome section 11; merged before the M4 gate (plan 0001 addendum) | Status line |
| 0022 wu15 format catch-alls | "planned 2026-11-07" | Outcome section 14; M4 gate addendum | Status line |
| 0023 wu16 rga3 model | "planned 2026-11-07" | Outcome section 13; merged `9097500` | Status line |
| 0025 presets-rga3 | "Not merged by its author" | Merged `26c8a82`; two Outcome sections | Status line |
| 0026 evidence tools | "planned 2026-11-08" | Outcome section 18; merged `c829b02` | Status line |
| 0029 wu14 identities | "planned 2026-11-08" | Outcome section 18; merged `db27dd3` | Status line |
| 0030 wu12 change of basis | "planned ...; Not merged by its author" | Outcome section 18; merged `998df67` | Status line |
| 0031 wu14 follow-ups | "claimed 2026-11-08 ... Implemented 2026-11-09" | Outcome section 11; merged `ba05a36` | Status line |
| 0032 core kernel init | "Not merged by its author; stops for a blind review" | Outcome section 11; merged `5edf616` | Status line |
| 0033 mv test reference kernels | "claimed 2026-11-10" | Outcome present; merged `a972fa0` | Status line |
| 0034 placement cost comments | "claimed 2026-11-10" | Outcome present; merged `c612f57` | Status line |
| 0035 m5 docs and messages | "claimed 2026-11-10" | Outcome present; merged `fdb22f4` (the M5 gate report says closed) | Status line |
| 0037 snippets cleanup | "claimed 2026-11-10" | Outcome present; merged `72f3c58` | Status line |

## C. Plans that look closed

0018 (wu9 products: "merged to `develop` 2026-11-02", Outcome section 10), 0019 (wu10: both parts merged, closed except the post-WU10 audit's owner items; check those items are resolved in later decisions), 0036 (done, outcome), 0039 (done 2026-11-10, Outcome section 13).

## Open outcome content to check across plans

- Open findings written into outcomes that no later record resolves (found: plan 0027 I2, I3, I5; plan 0028 I5, I6, I7 and owner items 1 and 4). Each needs "accepted as a known limit" or a fix, recorded.
- Plan 0019: "open owner items in the post-WU10 audit"; not traced here whether decisions 0182 to 0192 settled them all.
- Plans 0003 to 0017, 0020 to 0023: Outcome sections exist, but V10 status (verification and validation stated separately) is absent in several, per the M4 and M5 audit findings m8 and m9 (plans 0022, 0024, 0025, 0027, 0028, 0033, 0034 lack it). Whether to add it is the owner's.
