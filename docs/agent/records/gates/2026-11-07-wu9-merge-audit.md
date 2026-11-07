# Audit report: the WU9 merge (`19273a6`), the M2 audit's item for WU9

- Merge audited: `19273a6` ("Merge feature/core-products: WU9, geometric_product, wedge, * and ^"), first parent `5db4c7e`, second parent `b5c0521`. Branch range `a75ce5e..b5c0521`: 12 commits and one merge of `develop` (`c6907c4`). Plan 0018, decision 0164, review record `records/reviews/2026-11-02-wu9-products.md`, mutation record `records/mutations/0018-products/README.md`. The governance applied is the text in force at `19273a6` (`git show 19273a6:docs/agent/governance/...`); where a later amendment bears on a finding, the finding says so.
- Auditor: Claude Opus 5.5 (`claude-opus-5-5`), `auditor` role file, 2026-11-07, on branch `records/audit-m2-wu9` in the worktree `.claude/worktrees/records-audit-m2-wu9`, from `develop` at `d68881f`. No source, test, or governance file was changed.
- Status: submitted to the owner. Not accepted.
- Verification status (V10): no build or test suite was run for this audit. The cheap gates ran on a `git archive` export of `08c70ba` (the last revision whose build inputs the merge carries; `08c70ba..19273a6` changes only plan 0018 and `runs.tsv`): the trace gate, `tools/test_references.py markers` and `golden`, and `conventions_check.py` all pass. The marker check also ran on exports of `a75ce5e`, `2604a53`, `eaa0074`, `4cd7229`, `664e1a9`, and `04dc275` (finding A3). `clang-format --dry-run --Werror` ran on `product.hpp`, `product_test.cpp`, and the eight compile-fail sources at `19273a6` and passed. Validation: not performed by this audit; WU9's products are verified against the golden tables, and no separate validation status is stated for them (carried minor 14).

Severity, as in the earlier audits: **blocking** blocks a release until the owner decides; **major** departs from a ratified provision or owner ruling without a declaration, or rests a record on a wrong fact; **minor** changes no result; **info** is an observation (C19). Counts: 0 blocking; 3 major (A1 against decision 0183, the W9 departure A2, and the post-WU10 audit's M2, still open); 5 new minor (A3 to A7) and 3 carried minor still open; 3 info.

## Departures and open items (C20)

Of this audit:

1. The brief's phrase "the M2 audit item for WU9" admits two readings: the WU9 part of the independent M2 audit that decision 0163 item 2 deferred until after WU10 (the post-WU10 audit), or that audit's finding M2 (WU9 ran ahead of the M2 gate). I followed both readings: this report audits the WU9 merge in full and gives the status of finding M2.
2. No build or test ran, to keep clear of the coordinator's preset runs. The P9 evidence below comes from the run log, and the full-suite claims are not reproduced.
3. My run rows (11 rows of the cheap gates and the format check, revision `d68881ff923c`, branch `records/audit-m2-wu9`) went first to a scratch log and are appended to `runs.tsv` in this branch's commit. Their selections name the audited revision after an `@`, so one of them is named `gate:trace@08c70ba`. It is not a merge's gate, but the compaction rule of P13 will keep it as one.
4. `tools/runlog check` was not run at `19273a6`. It needs a checkout, and I did not create a second worktree for it. It was run on this branch with my rows appended, and passed ("1 eras match the history; 1727 kept rows present once; no removed row present"); its own row is in the run log.
5. Git, grep, and file inspection did not go through `tools/timed` (they are not build or test steps).
6. The reviewer's own report could not be compared with the committed review record. The record reads as the reviewer's text (first person, own departures), and the coordinator committed it with the fix pass.

Found, most severe first:

1. **Major (A1), needs the owner:** decision 0183 says WU9's fix pass "changed only comments in `include/` after the review" and settles the post-WU10 audit's M1 on that basis. The fix pass changed code. The M1 question is still open.
2. **Major (A2):** the fix pass changed the tests and the pinned message texts that judge it, in the same commits as the code, with no delta review and no recorded review request listing them (W9). The post-WU10 audit's W9 row says "Met".
3. **Major (carried, post-WU10 audit M2):** WU9 ran ahead of the M2 gate (W11). No ruling exists (handoff of 2026-11-07, open items).

## What the earlier M2 audit covered, and what it did not

The M2 gate report (`records/gates/2026-11-02-M2.md`) was not an audit. A Sonnet coordinator wrote it from the records, with no auditor and no runs (its departure 1). Decision 0163 item 2 deferred the independent audit until after WU10, and that audit is `records/gates/2026-11-02-post-wu10-audit.md`. Its auditor was Opus 5.5, it ran at `7729c91`, and its scope was `a75ce5e..7729c91` (83 commits) plus M2's own record.

For WU9 it covered:

- M1: the fix pass merged without a delta review.
- M2: WU9 ran ahead of the M2 gate (W11).
- Minor 4: no before and after costs (W10, P1).
- Minor 6: failing commits `7fdd8d1` and `ba85930` (W2).
- Minor 13: no configure, trace-gate, or `tools/runlog check` rows behind the "new empty trees" claim.
- Minor 14: no separate V10 status.
- Info 5: the Sonnet implementer.
- The full suite on seven presets at `7729c91`, which includes WU9's code in its later form.
- The `product.hpp` header example at `7729c91`.

For WU9 it did not cover:

- Any check at the WU9 merge itself. Its runs were at `7729c91`.
- W9 in the fix pass. Its W9 row reads WU9's combined commit as "declared and listed" (A2).
- Four more failing commits on the branch (A3).
- The header-set gap: the style check and the header verification did not cover `product.hpp` at the merge. WU10a's review found the gap, but it was never attributed to WU9's merge (A4).
- The missing wall time in the merge message (A5).
- Commit messages that give no reason (A6).
- Whether the review record's resolution answers every finding (A7).
- The mutants, which it did not rerun.

The M3 gate audited `7729c91..b504aa7` and took WU9 "through that audit's findings". Its merge table calls WU9's merge message "complete" (see A5).

The window `9d82f30..a75ce5e` has had no independent audit. That is everything from the M0–M1 audit's revision to the M2 gate report: 423 commits and 19 merges into `develop`. It holds WU6a to WU8b, plans 0009 to 0017, the five `research/*` merges, and the job-pool merge `e1af2ac`. The one exception is plan 0009's merge `5393f05`, which the M0–M1 gate's addendum describes. The post-WU10 audit read M2 only through its gate record. So M2's units were never audited commit by commit, which was M2's purpose for an audit (decision 0163 item 2).

## Findings

### A1 (major, owner decision). Decision 0183 settles the WU9 delta-review question on a wrong fact

- Provisions: C18 ("A reader can answer 'why is it this way?' from the record"), C20, W5 as amended by decision 0183, decision 0094 item 3.
- Evidence: decision 0183, Context: "WU9's fix pass and WU11a's `f4d85f8` changed only comments in `include/` after the review". Consequences: "The WU9 fix pass (post-WU10 audit M1) and `f4d85f8` (M3 gate m3) are settled: each is listed in its review record." `git diff c6907c4 08c70ba -- include/`, with comment lines removed, shows these code changes:
  - In `product.hpp`, `reject_product` returns `std::remove_cvref_t<X>` instead of `void`, and eight `static_assert`s on `Spelling != ...` became an `if constexpr` chain that asserts `std::is_same_v<typename R::algebra_type, typename S::algebra_type>` and the same for `scalar_type`. `ProductSpelling` lost its `std::uint8_t` underlying type, and `<cstdint>` was dropped.
  - In `plan.hpp`, `BinaryOperation` and `UnaryOperation` changed from `std::uint8_t` to `int`.
  - In `mv.hpp`, the text of the ring-operations `static_assert` message changed.
- Why it matters: 0183's own rule says "A change that touches code as well as comments still gets a delta review". So by the amended W5, WU9's fix pass needed one, and the owner's ratification of 0183 was given on a premise the commits contradict. A decision record is not rewritten (W6). A later decision has to correct it.
- Mitigation (inferred, as the post-WU10 audit said): WU10a's blind and delta reviews read `product.hpp` in its later form.

### A2 (major). The fix pass changed the tests that judge it, in the code commits, with no listing (W9)

- Provisions: W9 ("A change to a test, expected value, or tolerance that judges the author's own change is committed separately and listed in the review request"), W5.
- Evidence: `f4eace5` changes `product.hpp`, `plan.hpp`, and `mv.hpp` together with the tests that judge them:
  - the expected message texts in `test/CMakeLists.txt` (`operator*` became `a * b`, `operator^` became `a ^ b`, and the `mv_scalar` text gained its fix clause);
  - all eight `product_mixed_*_fail.cpp` sources (`(void)(a * b);` became `const auto r = a * b;`);
  - `product_test.cpp`, where the `std::complex` and `std::bitset` checks were removed, the expected type was rewritten, and twelve `static_assert`s on the `detail` concepts were added.

  `ba85930` also removes one line of `product_test.cpp` along with header changes. No delta review exists, so no review request listed these changes. The original review request (the brief) is not in the repository, so the exception's condition ("the review request lists the tests and the expected values") cannot be checked for `2604a53` and `664e1a9` either. Plan 0018's departure covers only "commits 2 to 4".
- Why it matters: the expected texts changed with the code they pin, and no one other than the author saw them before the merge. No computed value changed; the oracle for the products, the golden tables, is untouched.

### A3 (minor). Six of the twelve branch commits fail a check; the before-commit selection left out the marker check (W2, P8)

- Provisions: W2 ("It leaves the project building, with its tests passing, at every recorded step"), P8.
- Evidence: my runs of `tools/test_references.py markers` on exports of each revision:

| Revision | Marker check | Run-log evidence |
| --- | --- | --- |
| `a75ce5e` (base) | pass, 141 cases | |
| `2604a53` | **fails**: three `TEST_CASE`s in `product_test.cpp` have no reference marker | `lint` build exit 1 at the clean revision (`2026-11-02T09:16:58Z`) |
| `eaa0074` | **fails** (three findings) | only `core_product_test` and `core.product` ran |
| `4cd7229` | **fails** (three findings, one a marker kind `definition` that the checker does not accept) | no rows |
| `664e1a9` | **fails** (three findings) | full suite exit 8 on all seven presets (`09:20:11Z` to `09:22:32Z`) |
| `04dc275` | pass, 147 cases | seven presets pass |

  The post-WU10 audit's minor 6 named `7fdd8d1` (tests fail on every preset, and the build fails on the Clang presets and lint) and `ba85930` (lint fails). With the four above, six recorded steps fail. Plan 0018 mentions "three earlier runs that failed" but lists no commit as a W2 departure. The selections run before the commits at `2604a53` to `4cd7229` were `core_product_test` and `core.product`. `tests.reference_markers` reads every test source, so a new test file affects it (P8: "each new or changed test"; reading this as covering the checks that read test sources is my reading).
- Why it matters: no result is affected, and the merged tree passes. The pattern of failing runs of commits recurs in the M0–M1 audit and lesson 0067.

### A4 (minor). The style check and header verification did not cover `product.hpp` at the merge (W5, S3)

- Provisions: W5 (the mechanical style checks), STYLE S3.
- Evidence: at `19273a6`, `CMakeLists.txt` lines 19 to 27 list the `galoot_core` header set without `include/galoot/core/product.hpp`. The `format-check` target formats `$<TARGET_PROPERTY:galoot_core,HEADER_SET>` and the test sources, and `VERIFY_INTERFACE_HEADER_SETS` compiles each listed header on its own. So the lint preset's passing run at `08c70ba` checked neither the format of `product.hpp` nor that it compiles alone. WU10a's review records the gap (`2026-11-02-wu10a-antiproducts.md`, "`product.hpp` was missing from `galoot_core`'s `FILE_SET` from WU9 until this branch"). The WU9 reviewer checked both by hand, but at `dd8a77e`, before the fix pass.
- Result now: `clang-format --dry-run --Werror` (23.1.4) on `product.hpp` at `19273a6` is clean, so no result changed. The standalone compile at `19273a6` was not rerun.

### A5 (minor). The merge message gives no wall time (P13)

- Provision: P13, in force at `19273a6` (applied by `c5f6742`, merged in `ab7af27` 21 minutes before WU9): "The message of each merge names the selection that ran, its configurations, its outcome, its wall time, and any check deferred".
- Evidence: the message of `19273a6` names the selection, configurations, outcome, and deferrals, but no wall time. Its selection is `all`, not `gate:all` (the post-WU10 audit's minor 1). The run rows give the time: the seven presets at `08c70ba` ran from `11:09:18Z` until the lint tests ended at about `11:11:45Z`, about 146 s end to end (the sum of the 14 rows' wall times is 145.4 s). The M3 gate's merge table marks this message "complete".

### A6 (minor). Seven of the twelve branch commits give no reason (W7)

- Provision: W7 ("a message that says what changed and why").
- Evidence: `eaa0074`, `4cd7229`, `04dc275`, `dd8a77e`, `ba85930`, `08c70ba`, and `b5c0521` have a title and the trailer only. The review's F10 named four such commits at `dd8a77e`, and the fix pass added three. Every commit carries `Co-Authored-By: Claude Sonnet 5.5`, so agent authorship is identified.

### A7 (minor). The review's resolution leaves one finding unanswered, and the reviewer's runs are not in the run log (W5, P13)

- Provisions: W5 ("lists each finding with its resolution"), P13.
- Evidence: the resolution names F1 to F14 but not F8e (the plan's `rga3`/`pga3` "over `int`" against rigid(3) and conformal(3) over `std::int64_t`, which the reviewer called "a quiet departure"). The reviewer logged its builds, tests, and mutant compiles to `scratchpad/runs.tsv` and wrote that "the merging agent can move those rows". `runs.tsv` has no row of the review: every `feature/core-products` row is the author's, 98 rows of steps `build` and `test`.

### Carried findings, status at `d68881f`

| Post-WU10 audit finding | Status |
| --- | --- |
| M1 (no delta review of the fix pass) | **Open.** Decision 0183 claims to settle it, on the wrong fact of A1 |
| M2 (WU9 ahead of the M2 gate, W11) | **Open, major.** No ruling; the 2026-11-07 handoff lists it for the owner. M3's acceptance answers the same question for WU13 only |
| M3 (agent cost) | WU9's reviewer backfilled (`agent-cost.tsv` row 89: 209,582 tokens, 61 tool calls, 793 s). The coordinator's own usage is not recorded |
| Minor 4 (W10, P1 before and after costs) | Open. The plan defers only the P2 figure. The post-WU10 audit's inferred share is about 2.9 M instructions (GCC) and 3.0 M (Clang) |
| Minor 6 (W2) | Extended by A3 |
| Minor 13 (no rows behind the trace gate and `tools/runlog check`) | Partly backed now: the trace gate passes at `08c70ba` (this audit, scratch export, row `gate:trace@08c70ba`). `tools/runlog check` at the merge and the "new empty trees" claim stay unbacked |
| Minor 14 (V10) | Open: plan 0018 states no separate verification and validation status |

### Info

1. **I1, plan 0018's trace criterion.** The definition of done says rows "for the wedge in CONVENTIONS 4.1 and the products in 5.4 move to `yes`". Item 5.4 is the composition of transforms (`geometric_antiproduct(Q2, Q1)`), which WU9 cannot pin; it stays `no`, and the outcome does not mention it. 4.1 and 7.1 are `yes` (trace gate at `08c70ba`: 7.1 at `product_test.cpp:156`, 4.1 gains `product_test.cpp:131`).
2. **I2, a reference label (V1, V2).** "the operators are the named functions" carried the marker kind `definition`, which the checker rejects (`4cd7229`). `04dc275` relabeled it `identity`. The claim is that an alias returns what its function returns, which is closer to the kind `contract` than to an identity the structure must satisfy. The test can fail, and the label overstates nothing that is cited as evidence.
3. **I3, the hijack probe.** Plan 0018 approach 4 asked for a probe that `*` and `^` do not hijack the operators of `std::complex` and `std::bitset`. The first version could not fail (review F8a), and the fix pass removed it, which V2 allows. No probe of that property exists since.

## Conformance by provision (WU9 merge)

| Provision | Status | Evidence |
| --- | --- | --- |
| W2 | Not met, undeclared | A3; the post-WU10 audit's minor 6 |
| W3 | Met | Plan 0018 claimed on `develop` (`189418d`) before work; closed with its outcome; status line records the merge |
| W5, review | Partly met | Independent reviewer (Opus 5.5, separate context; author Sonnet 5.5), record names reviewer and model, says what it checked and did not, applies the usability checklist (public interface), committed on the branch with the fixes (`f4eace5`). No delta review of a code fix pass (A1); F8e unanswered (A7) |
| W5, checks | Met for the suite and the trace gate; style check incomplete | Seven presets pass at `08c70ba`, which differs from the merged tree only in plan 0018 and `runs.tsv` (`git diff --stat`); trace gate passes there (this audit); format and header verification omitted `product.hpp` (A4) |
| W7 | Met, except messages | Branch from `develop` at `a75ce5e`; `develop` merged into it at its tip `5db4c7e` (`c6907c4`), and `develop` did not move before the merge (first parent `5db4c7e`); merged only into `develop`, one merge at a time (`ab7af27` 21 minutes earlier); the merged tree equals `b5c0521`'s; every commit carries an agent trailer. A6 |
| W9 | Not met for the fix pass | A2. The first pass's combined commits were declared (plan 0018 Outcome) |
| W10 | Partly met | Carried minor 4; reviewer's cost backfilled |
| W11 | Not met, unruled | Carried M2 |
| V1, V2, V7 | Met | Blade-pair tables against `golden::gp_tables` from the reference scripts (kind 3) over rigid(3) and conformal(3), the latter non-diagonal; associativity and v ∧ v = 0 on random integer multivectors (kind 2), seeded from doctest's `rand_seed` and printed (`product_test.cpp:203-205` at `19273a6`); 147 test cases with reference markers at `08c70ba`. I2, I3 |
| V6 | Met | 4.1 and 7.1 pinned (trace gate at `08c70ba`); I1 |
| V10 | Not met | Carried minor 14 |
| V11, V12 | Met | Exact comparison over `std::int64_t`; the `double` test compares two evaluations of one plan, so it is exact |
| P8 | Partly met | The rename (`5c5188f`) ran the full gcc-debug suite before its commit (`a75ce5e-dirty` rows, `09:13:52Z`); later selections left out the marker check (A3) |
| P9 (as in force at `19273a6`) | Met | Full suite on both compilers, gcc-asan and clang-asan, and lint, at `08c70ba`, all exit 0 (`11:09:18Z` to `11:11:39Z`); `clang-measure` was not then required (decision 0184 added it on 2026-11-07) |
| P13 | Partly met | A5, A7; carried minor 13 |

## Decisions for the owner, with recommendations

1. **A1, decision 0183 and WU9's fix pass.** Recommended: record a decision that supersedes 0183's statement about WU9 (0183 stays as written). The new decision should say that the WU9 fix pass changed code and so needed a delta review under the amended W5, and that the owner accepts it as covered by WU10a's blind and delta reviews of `product.hpp` in its later form. That was the post-WU10 audit's recommendation 2, which 0183 did not adopt for WU9. The alternative is a delta review of `c6907c4..08c70ba` now. It would judge code that WU10a has since changed, so it would add little.
2. **A2, W9 in fix passes.** Recommended: no amendment. Ask that a fix pass commit its test and expectation changes separately, and that its delta review request list them, as WU10a and WU11 did. Record WU9's fix pass as an accepted departure in the same decision as item 1.
3. **Carried M2, W11.** Recommended, as the post-WU10 audit said: accept WU9's start before M2's acceptance, and direct that no unit of a milestone is implemented before the gate it depends on is accepted. Plan 0021 already follows this.
4. **The unaudited window `9d82f30..a75ce5e`.** Recommended: say whether M2's units need the commit-level audit that decision 0163 item 2 intended. If so, scope it to the 19 merges listed above, with their review records, W2 and W9 at each merge, and the P9 rows. No re-run of the suite is needed, because `develop` has passed every merge preset since.
5. **A3 to A7.** No owner action. They are for the records of the next audit or a lesson.

## Not audited, and why (C20)

- No build, test suite, mutant, P1, P2, or assembly run (brief: avoid builds while the coordinator's preset runs are in progress). The P9 evidence is the run log's.
- `tools/runlog check` at `19273a6` and the standalone compile of `product.hpp` at the merge were not run.
- The mathematics was not derived again; the golden tables and the reviewer's checks were relied on.
- The review record was read in full; the reviewer's own report, which is not in the repository, could not be compared.
- WU10a's reviews were read only for the lines that bear on `product.hpp` (A1 mitigation, A4).

## Appendix: merged branches, and the 43 against 46 count

### The two counts

The M3 gate (minor m5, written at `b504aa7`) says "46 branches merged into `develop` remain". The handoff of 2026-11-07 lists 43 finished branches. Both counts are right for what they counted.

I rebuilt `git branch --merged develop` as it stood at 2026-11-04 11:30 −0500, when the M3 gate was being written, from each branch's reflog: its value at that time, kept only if the branch already existed, and tested for ancestry of `develop@{2026-11-04 11:30 -0500}` (`b504aa7`). The unfiltered command gives 46 lines: the 43 `feature/`, `research/`, and `review/` branches, plus `develop`, `master`, and `gate/m3`. `gate/m3` then pointed at `b504aa7`, since its report was committed only at 11:37. With the filter that the gate report's own command applies (`grep -E 'feature/|research/|review/'`), 43 remain. They are exactly the handoff's list (`diff` empty). So 43 is the number of finished, merged, deletable branches. The 46 counted `develop`, `master`, and the auditor's own new branch.

### At `develop` `d68881f` (2026-11-07)

`git branch --merged develop` prints 54 lines:

- The 43 of the handoff, all finished.
- Five more, finished and merged since: `records/m3-acceptance` (`ac76bbd`); `gate/m3` (merged into `records/m3-acceptance` by `29fa24e`, which then reached `develop`); `feature/test-split` (`51c18de`); `feature/core-transform--rounding` (`0fb3a57`); and `feature/core-transform`, whose tip `01835b7` reached `develop` only through `feature/core-transform--rounding`.
- Four `records/` branches at `develop`'s tip with no commits of their own, which only look merged: `records/audit-m2-wu9` (this audit), `records/o6-flat-plane-rounding`, `records/plan-wu15`, `records/plan-wu16`. They are in use and must not be deleted.
- `develop` and `master`.

Not merged into `develop`:

- The three test authors' branches, `feature/core-duals-tests`, `feature/core-transform--tests`, and `feature/core-transform--rounding--tests`, cherry-picked by design (decision 0185).
- `feature/core-transform--math`. Its four commits reached `develop` by cherry-pick (`960bdb4` → `48e090a`, `dcb182a` → `0fb27bd`, `8f82ba5` → `89dcf2d`, `db1a349` → `3126146`, matched by subject; inferred).
- `records/review-wu13a` and `records/review-wu13b`. Their review records reached `develop` by cherry-pick (`6c52567` says "cherry picked from commit caaefef").

So 48 branches are finished and merged today, not 43.

### Merges into `develop` without a review record

`develop`'s first-parent line has 36 merges. Each code, test, or tooling merge has a W5 record in `records/reviews/` except these three:

| Merge | Branch | What it changed | Review record |
| --- | --- | --- | --- |
| `e1af2ac` (2026-11-01) | `feature/jobpool-idle-shutdown` | `tools/jobpool` (+105, −15 lines), decision 0120 | **None**, and in the unaudited window above |
| `ab7af27` (2026-11-02) | `feature/runlog-0170` | `tools/runlog`, its test, `CMakePresets.json`, PERFORMANCE.md | **None** (post-WU10 audit minor 2) |
| `51c18de` (2026-11-07) | `feature/test-split` | Split `identities_test.cpp` into eight sources, `product_test.cpp` into two, two new shared check headers, `test/CMakeLists.txt`; the B1 fix by a raised step limit (decision 0193) | **None.** Decision 0194 lists "the agent-cost row for the test-split implementer and reviewer" as not done, so a reviewer may exist, but no record of its review is committed. The merge message has no P13 line |

Records-only merges with no review record: `b296391` (WU7a run rows), the five `research/*` merges (`1badbd3`, `3929bad`, `3bd9904`, `0e2f283`, `37a562d`; note 0015 says "Not yet reviewed by anyone else"), and `ac76bbd` (`records/m3-acceptance`, which also carries the governance text the owner ratified). Whether W5's independent review applies to a records-only merge is a reading the owner has not given. No audit has flagged these merges.

Merges with a review record but an incomplete W5: `19273a6` (WU9, A1). `535c91e` (WU11a, comment-only, now settled by decision 0183).

Also observed (W7):

- `0fb3a57` merges `feature/core-transform--rounding`, which was made from `feature/core-transform`, straight into `develop`, and `feature/core-transform` is never merged itself. Decision 0194 names the branch but does not declare this as a W7 departure.
- `gate/m3`, made from `develop`, was merged into `records/m3-acceptance` instead (declared in the handoff).
- Before decision 0188, no `review/*` branch was merged into `develop` directly. Seven were merged into the feature branch they reviewed: `4436186` into `feature/core-plan-maps`; `e541ab4`, `d203322`, and `d9e1521` into `feature/core-mv-a`; `aa867a8` and `9c9770b` into `feature/core-mv-b`; `bc14d7d` into `feature/metric-like-single-pass`. The other three (`review/plan-limits-certified`, `review/runlog-delta`, `review/runlog-distilled`) reached their features with no merge commit. Decision 0188's row ("branched from `develop` and merged into it") describes none of them.
- The merge messages of `51c18de`, `0fb3a57`, and `ac76bbd` carry no P13 line.

## Appendix: proposal for the owner, a `records/<name>` row in W7's branch table

W7's table (as amended by decision 0188) has no row for the `records/` prefix. That prefix is used in two ways today:

- Branched from `develop` and merged into it, holding records only: `records/m3-acceptance`, `records/plan-wu15`, `records/plan-wu16`, `records/o6-flat-plane-rounding`, and this branch.
- Branched from the head of the branch under review, never merged, its commits cherry-picked into `develop`: `records/review-wu13a`, made from `ff35cc9` on `feature/core-transform`, and `records/review-wu13b`, made from `43355bb` on `feature/core-transform--rounding`.

The `review/<name>` row, which 0188 added for review records, has not been used since 0188.

Proposed row, to follow the `research/<name>` row:

| Branch | Holds | Merged into it by |
| --- | --- | --- |
| `records/<name>` | Records only (plans, decisions, review and audit records, gate reports, handoffs, run rows), and the text of a governance change the owner has ratified; no change to `include/`, `test/`, `tools/`, `cmake/`, or the build files. Branched from `develop` and merged into it; a review record is branched from the revision it reviews, never merged, and its commits are cherry-picked into `develop` | Any contributor, agents included |

Proposed wording for the sentence after the table, so that the cherry-pick form does not read as a departure from "A branch merges only into the branch it was made from": "A test author's branch and the branch of a review record are not merged; their commits are cherry-picked with `-x`."

Option for the owner, with a recommendation: the same amendment could retire `review/<name>` and `gate/<milestone>` into `records/<name>`, which already holds both kinds of record, and leave one row for `research/<name>`. Recommended: do so (C12: one prefix for records), keeping the existing branches' names as decision 0188 did for its renamed forms.
