# Blind review of three merges into develop that had no review record

- Reviewer: blind-reviewer role (Claude, agent, Opus 5.5). Date: 2026-11-08.
- Scope, by owner ruling (decision 0202 item 6):
  - `e1af2ac`, the merge of `feature/jobpool-idle-shutdown` (commit `12cf393`): `git diff e1af2ac^1 e1af2ac`.
  - `ab7af27`, the merge of `feature/runlog-0170` (commits `c5f6742`, `2311598`): `git diff ab7af27^1 ab7af27`.
  - `51c18de`, the merge of `feature/test-split` (commits `cac4bf1`, `4353c6d`, `784b55e`, `c28fa89`, `4efe4d0`, `1b8ed47`, `5399fe6`): `git diff 51c18de^1 51c18de`.
- Record branch `records/review-three-merges`, worktree `.claude/worktrees/review-three-merges`, from `develop` at `26c8a82`.
- The brief carried no verdict on the changes. It did relay that the pool lost slots twice after failed builds. I took that as a report to investigate, not as a finding.

## Departures, skipped checks, ambiguities (C20)

- The role file gives this reviewer no Write tool, but the brief asks for this record to be committed. Only this file is committed.
- The branch starts at `develop`'s tip, not at each reviewed revision as W7 describes for a review branch. The brief asked for that, and all three merges are ancestors of the tip.
- All three changes are already merged, so the review cannot block anything. Each finding below says whether it would have blocked.
- Run rows went to a scratch log (16 rows, listed at the end), not to `runs.tsv`, so that this branch commits only this record.
- Not run: the full suite on any preset; gcc-release, clang-release, gcc-asan, clang-asan, and lint; the mutation drivers; clang-tidy on the new test headers; the build-cost measurements of decision 0193; the external-linkage probe; a private-unit test of the idle stop or of the race in J3. Not reproduced: the lost-slot incidents themselves. Whatever on record ran at these revisions (decisions 0120 and 0193, the merge message of `ab7af27`) was read and not rerun (P10).
- Ambiguity: "lost slots twice after failed builds". The records show losses of 9 of 23 slots (post-WU10 audit, 2026-11-02) and 2 of 23 (M3 gate, 2026-11-04), and the owner's restart on 2026-11-07 (decision 0195 item 7). None of them links a loss to a build. The one mechanism on record that is tied to failing builds needs a test program to be missing (lesson 0063). I could not tell which two incidents the brief means.
- The usability checklist does not apply, because no public header changed. The `tools/jobpool` command line is an agent interface, and its points are under J4.

## Merge 1: `feature/jobpool-idle-shutdown` (`e1af2ac`)

**What changed and why.** Decision 0120 is the owner's request that a lost session should not leave the pool running forever. `tools/jobpool` now runs its own `_serve` subcommand as the service body. Every `JOBPOOL_POLL_SECONDS` (30 s) the service counts the processes that hold the FIFO open, leaving out itself and its descendants, and it exits after `--idle` minutes (default 30; 0 means never) without a client. `ensure` starts the pool only when it is not running. `status` prints the idle time. The unit name, FIFO path, and poll interval can be overridden.

**What I checked.**
- I read the whole script at the tip. It is unchanged since `12cf393`.
- The service and the tool never take tokens. `free_slots` opens the FIFO and only does `FIONREAD`. `count_clients` and `clients` only read `/proc/*/fd` links. Nothing in `tools/jobpool` or `tools/timed` reads from or writes to the FIFO after the initial fill. So the tooling cannot lose a slot itself. Losses come from clients that take a token and never return it.
- Whether the watcher counts itself as a client. Its `sleep` and its `count_clients` run with descriptor 3 closed. I tested the command-substitution case in bash 5.3.20 on a scratch FIFO: inside `$(f 3>&-)` the subshell holds no descriptor to the FIFO. So neither `status` nor the watcher mistakes the service's children for clients.
- The pool read "running: 21 of 23 slots free ... clients: 1940735 (ctest)" at the start of this review, and "23 of 23" before and after my builds. I did not start, stop, or restart the shared pool.

**Findings.**

- **J1, medium, would not have blocked: no recovery of lost slots short of a restart.** The causes on record are both clients. CTest keeps the slot of a test whose program is missing (lesson 0063, reproduced there). Ninja killed with SIGKILL returns nothing (plan 0003, reproduced there). A Ninja build that merely fails lost no slots (lesson 0063). The idle stop recovers lost slots only after 30 minutes with no client at all, which seldom happens while several agents work. That fits the 9-slot and 2-slot losses persisting across audits. Neither `tools/timed` nor the mutation drivers stop CTest from running after a build that left a program missing. The fix lives in the pool, not in each client. When a poll finds no client, the service can write back `n − free` tokens, and drain the surplus if `free > n`. The owner already accepted this in decision 0130 item 7 (automatic repair), but only as part of the extraction lane, which has not started. A recovery is warranted. See owner item 1.
  - A caveat on the repair. A client that opens the FIFO and takes a token between the count and the top-up leaves one surplus token until the next quiet poll. That is a window of milliseconds, and the drain covers it.
- **J2, low, would have blocked under W5 at the time: no committed test and no recorded checks.** The only evidence of the idle behavior is the hand run described in decision 0120, on a private unit with a 1-minute idle time. No CTest test covers `tools/jobpool`. The merge message names no selection, configuration, outcome, or wall time (P13), and no review was recorded before the merge (W5).
- **J3, low, plausible (inferred, not tried): the idle-stop race.** Suppose a client opens the FIFO after the last count and before `exit 0`. It keeps the old FIFO and its tokens, `ExecStopPost` removes the path, and the next `ensure` makes a second pool. For a while, up to twice the slots are in use. Research note 0016 §4 item 5 lists the same race. The window is the run time of one `count_clients`.
- **J4, low: the command line.**
  - `ensure` accepts the same arguments as `start`, although the usage line gives it none.
  - `start 23 24` silently takes the last number.
  - The slot total is parsed from the unit's description text (0016 §4 item 7).
  - None of these changes a build's result.

## Merge 2: `feature/runlog-0170` (`ab7af27`)

**What changed and why.** This merge applies the owner's ratification of decision 0170 (decision 0162 item 3):
- P7 now holds the distilled history.
- P13 now covers campaign logs, the `gate:<selection>` marker, and the compaction rule.
- P8 and P9 now run the once-per-merge `tools/runlog check`.
- DoD-25 is reworded.
- `tools/runlog`'s `stays()` keeps every row whose selection starts with `gate:`.
- The test presets leave out the label `runlog`.
- One rule test covers the marker.

**What I checked.**
- I read the whole of `tools/runlog` at the tip. It is unchanged since `c5f6742`.
- I compared `stays()` with P13 clause by clause, and they match:
  - rows named `gate:` are kept at any step and revision;
  - workflow runs of a named preset (any preset but `-`) are kept at any revision;
  - at committed revisions (12 hex digits), configure, build, and test runs of a named preset are kept, as are the `lint` and `gate` steps and the `check` and `trace` steps of the ten cheap gates;
  - the anchors are kept.
- Under the current rule, compaction drops no row that P13 says to keep.
- `tools/runlog check` at `26c8a82` exits 0: one era matches the history, its 1,727 kept rows are each present once and in order, and no removed row is back.
- `runs.tsv` at the tip:
  - one header line and 12,312 rows;
  - 12,057 rows have 28 cells and 255 have 26 (older rows without the last two columns, which `fields()` pads);
  - every revision is 12 hex digits, with or without `-dirty`.
- `git show ebdd06d09289:docs/agent/records/performance/runs.tsv` holds no row whose selection starts with `gate:`. So widening the rule under the existing era changed nothing in that era.
- The CTest filter. With `-N` at the tip, `ctest --preset gcc-debug -L '^runlog$'` lists 0 tests, and `ctest --test-dir build/gcc-debug -L '^runlog$'` lists 1.

**Findings.**

- **R1, medium, latent, would not have blocked: `prune` can delete rows that an era kept.** `replay()` recomputes every past era with today's `stays()` and anchors. Only `check` compares the result with `eras.tsv`. `prune` acts on the recomputed sets without that comparison (`tools/runlog`, `def prune`).
  - **Failure scenario.** A later change narrows `stays()`. Then `check` reports the newly excluded rows as "a row an era removed is back", next to an era mismatch. P9 tells the merging agent to run `prune` "when the check reports removed rows". `prune` then deletes rows that the era kept and that P13 protected when the era was made.
  - **The converse.** A widening makes `check` fail with no repair in the tool.
  - **This merge.** It made exactly such a rule change. It was safe only because the one era had no `gate:` rows.
  - **Fix.** `prune` should refuse unless every era recomputes exactly. Alternatively, record a rule version per era. See owner item 2.
- **R2, low: `tools.runlog_check` cannot be reached through any test preset.** `test-base` excludes `^runlog$`, so `-L '^runlog$'` with a preset selects nothing, and with `noTestsAction: error` a real run fails. P9's merge check is met anyway, because merges run `tools/runlog check` directly (rows `gate:runlog`). P9's sentence that the CTest test "carries the label `runlog`" therefore describes a test that only `--test-dir` can run.
- **R3, low, process.**
  - `c5f6742` puts the rule change, its test, the governance text, and a preset change in one commit, with no review request listing the test (W9).
  - The merge message names no wall time.
  - It does not say that the trace gate or the style checks ran before a merge into `develop` (W5).
  - Its own check runs are named `all`, not `gate:all`. They are kept anyway, since they are at a committed revision with a named preset.
- **R4, info.**
  - Committed revisions are recognized as exactly 12 hex digits. `tools/measure` uses `git rev-parse --short=12`, which writes a longer prefix when 12 digits are ambiguous, and such a row would be dropped as uncommitted. The odds are negligible at this repository's size.
  - The `check - runlog` rows from before the marker (for example `a8bbe979cbc4`) are not kept at the next compaction, because `runlog` is not a cheap gate. P13 allows that: they are counted in the history.

## Merge 3: `feature/test-split` (`51c18de`)

**What changed and why.** Decision 0189: `identities_test.cpp` was the critical path of every build. Decision 0184 (B1): clang-measure could not build the conformal(3) `full<5>` antiproduct.
- The identities checks moved to `test/core/identities_checks.hpp`.
- Seven group sources each run one group of signatures. The three test cases stay in `identities_test.cpp` and loop over the groups in order from one seed.
- The product test's two conformal(3) cases moved to `product_conformal3_test.cpp`, its helpers to `product_checks.hpp`.
- The two conformal(3) sources build on Clang with `-fconstexpr-steps=1200000`.
- `4efe4d0` gave the shared checks internal linkage again.

**What I checked.**
- **`TEST_CASE` names.** `git grep` at `ac76bbd` (`51c18de^1`) and at `51c18de` finds the same 280 names in `test/`, 255 of them in `*_test.cpp`, which matches decision 0193.
- **CTest listing.** I configured both revisions in new detached trees and compared `ctest --preset <p> -N --show-only=json-v1` by name and labels. They are identical: 335 tests on gcc-debug and 337 on clang-debug. `core.identities` keeps `core, core.identities, core.product, core.unary, slow`.
- **doctest, gcc-debug.** I built `core_identities_test` and `core_product_test` at both revisions and ran them. `core_identities_test` has 3 cases and 2,208 assertions before and after; `core_product_test` has 17 cases and 243 assertions before and after. Each program lists the same `-ltc` names at both revisions. All pass.
- **No weakened test.**
  - The three check function bodies, `value_on`, `difference`, `random_integers`, `antiscalar`, the sparse sets, and `samples = 3` are textually identical apart from `inline`.
  - Each group checks the same forms on the same layouts per identity as the old file. Full layouts are used for the 22 forms up to three generators, the 12 of four, R(4,1,0), R(4,0,1), and conformal(3). The dense fives use bivectors, and vectors for the antiproduct laws. The sixes use vectors × bivectors, and the large forms the sparse sets.
  - The group order equals the old fold order, so one seed draws the same multivectors.
  - The product split keeps every code line. Only includes, namespaces, `using` lines, and indentation differ.
- **B1.** At `51c18de` under clang-measure, both programs build. The two sources carry the option (2 lines in `build.ninja`), and the 24 `core.identities`/`core.product*` tests pass.
- **Trace gate.** It exits 0 at `51c18de` (items 4.4, 5.x, and 6.x have no test, as on the tip).

**Findings.**

- **T1, low, process, would have blocked under W5 at the time.** No review record was committed, although decision 0194 mentions a reviewer. The merge message names no selection, configurations, outcome, or wall time (P13). The evidence exists only in decision 0193.
- **T2, low.** The mutation driver (`0019-antiproducts/mutate.py`, used unchanged) runs CTest after a failed `ninja -k 0` build.
  - The mutation record says so: stale programs run, and they decide the attribution of the `left_complement_sign_flipped_for_e12` kill.
  - Every mutant is still killed, so this weakens nothing.
  - It is the client pattern of lesson 0063, but programs from the previous build existed at these revisions. So it should not lose slots, and I found no loss tied to these runs (rows of 2026-11-07, 11:33Z to 12:05Z).
- **T3, info.**
  - STYLE S14 (one file per component) is departed from by the owner's ruling, and decision 0193 says so.
  - The two new headers fall outside the lint preset's header filter, as the older test headers do. They were tidied by hand, which I did not rerun.
  - The B1 route departs from decision 0184. The owner kept it (handoff 2026-11-07-second, decision 0195).

## Outcome

- `feature/jobpool-idle-shutdown`: approve with non-blocking findings (J1 to J4). J2 was a W5 lapse at merge time.
- `feature/runlog-0170`: approve with non-blocking findings (R1 to R4). R1 is the one worth a small unit.
- `feature/test-split`: approve with non-blocking findings (T1 to T3). Every test case, assertion count, CTest name, and label is preserved, and no test is weaker.

## Open for the owner

1. **Lost-slot repair (J1).**
   - Options:
     - (a) Add the automatic top-up and drain to `tools/jobpool`'s poll loop now, as a small Galoot tools unit. **Recommended.** It is about 15 lines, it ends the stop-and-start recovery, and it carries over unchanged to the extraction of decision 0130.
     - (b) Wait for the extraction lane, which has not started. The restarts continue until then.
     - (c) Keep restarts only.
   - Tradeoffs:
     - (a) adds code to a script that is about to move, and it needs a private-unit test.
     - (b) and (c) cost an owner restart per incident.
2. **`prune` guard (R1).**
   - Options:
     - (a) `prune` refuses unless every era recomputes exactly, and a rule change is checked against every era before it merges. **Recommended.** It is a few lines plus a test.
     - (b) Version the rule per era. That is more code, but it lets the rule change freely.
     - (c) Leave it as it is and rely on readers of `check`'s output.
3. **The `runlog` label (R2).**
   - Options:
     - (a) Reword P9 to say the merge check is `tools/runlog check` run directly, and drop the CTest test.
     - (b) Keep the test and document `ctest --test-dir <tree> -L '^runlog$'`. **Recommended:** this costs nothing.
   - A change to P9's wording is the owner's to ratify (C10).

## Run rows (scratch log, not committed)

| utc_start | revision | step | preset | selection | wall_s | exit |
| --- | --- | --- | --- | --- | --- | --- |
| 2026-11-08T14:40:57Z | 26c8a820f59b | check | - | review:runlog-check | 2.638 | 0 |
| 2026-11-08T14:42:12Z | ac76bbdcc0e4 | configure | gcc-debug | review:test-split-before | 0.381 | 0 |
| 2026-11-08T14:42:13Z | ac76bbdcc0e4 | configure | clang-debug | review:test-split-before | 0.319 | 0 |
| 2026-11-08T14:42:13Z | 51c18deabe49 | configure | gcc-debug | review:test-split-after | 0.367 | 0 |
| 2026-11-08T14:42:14Z | 51c18deabe49 | configure | clang-debug | review:test-split-after | 0.315 | 0 |
| 2026-11-08T14:42:31Z | ac76bbdcc0e4 | build | gcc-debug | review:core_identities_test+core_product_test | 18.912 | 0 |
| 2026-11-08T14:42:50Z | 51c18deabe49 | build | gcc-debug | review:core_identities_test+core_product_test | 6.128 | 0 |
| 2026-11-08T14:43:07Z | ac76bbdcc0e4 | test | gcc-debug | review:core_identities_test-before | 0.117 | 0 |
| 2026-11-08T14:43:07Z | ac76bbdcc0e4 | test | gcc-debug | review:core_product_test-before | 0.311 | 0 |
| 2026-11-08T14:43:07Z | 51c18deabe49 | test | gcc-debug | review:core_identities_test-after | 0.118 | 0 |
| 2026-11-08T14:43:08Z | 51c18deabe49 | test | gcc-debug | review:core_product_test-after | 0.354 | 0 |
| 2026-11-08T14:45:21Z | 51c18deabe49 | check | - | review:trace-gate | 0.404 | 0 |
| 2026-11-08T14:45:22Z | 51c18deabe49 | configure | clang-measure | review:test-split-after | 1.171 | 0 |
| 2026-11-08T14:45:23Z | 51c18deabe49 | build | clang-measure | review:core_identities_test+core_product_test | 7.908 | 0 |
| 2026-11-08T14:45:31Z | 51c18deabe49 | test | clang-measure | review:core.identities+core.product | 0.597 | 0 |
| 2026-11-08T14:46:31Z | 26c8a820f59b | configure | gcc-debug | review:runlog-label | 0.632 | 0 |

The build wall times ran on a shared machine with the 23-slot pool. They are noted, not compared (P7).
