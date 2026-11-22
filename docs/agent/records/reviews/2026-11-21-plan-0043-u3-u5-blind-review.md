# Blind review: plan 0043 units U3, U4 and U5 (tools)

- Reviewer: blind-reviewer role, run by Claude (agent, Sonnet 5.5), 2026-11-21.
- Reviewed: branch `feature/cost-cleanup-1-0-1--tools`, tip `8f48b32df15bd8f991c29d16d8e93a9c157a9fa9` (merged into `feature/cost-cleanup-1-0-1` as `0ad599e982b16904f23cfbb498d26424ed87fe2f`), range `01dce7f..8f48b32` (12 commits), restricted to `tools/`, `test/tools/` and the run-log rows the units wrote. The review branch `review/cost-cleanup-1-0-1-tools` starts at `8f48b32`.
- Criteria: plan 0043 section 2 criteria 3 to 5, section 4, section 6 rows U3 to U5, decision 0252 item 4.
- Did the brief carry a verdict? No. It carried the author's list of departures only.
- Verdict: approve. No blocking or major finding stands; two minor findings and four notes.

## Criteria

| Criterion | Result | Evidence |
| --- | --- | --- |
| 3. `tools/measure` marks a run dirty if its tree changed at any point between start and end, with a test; the staged case pinned (0252 item 4) | Met | `tools/measure` reads `revision_state()` before and after the command; the row is `-dirty` if either read is dirty or HEAD moved, and carries the start revision. The new case `dirty_mark` covers: clean, staged, edit before, edit during, edit before and undone during, change before and committed during, empty commit during, an edit under the performance records (not a mark), and the compile launcher. I ran `measure_record_test.py` on the new tool (pass) and on the tool of `01dce7f` (fails 5 checks: `git status calls` got 1 expected 2; undone-during, committed-during, empty-commit-during and compile-started-dirty read clean or with the wrong revision). The staged case passes on both, as section 1 item 3 predicts: it pins existing behavior. |
| 4. `tools/mutate` runs CTest on the shared pool, with a test and a before-and-after run | Met for code and test; the before-and-after run exists but shows no gain (finding F2) | `--parallel` with no number is added to the one `ctest --tests-from-file` call, which serves the baseline and each mutant. `ctest_in_parallel` wraps `ctest`, logs its arguments and requires `--parallel` with no number on both runs; I ran `mutate_test.py` against the old `tools/mutate`: 2 checks fail, on the new: pass. |
| 5. Cheap findings of section 7 closed | Met for I3 and I6 (the part of section 7 these units own) | `tools/jobpool status` without its FIFO now names a deletion by hand and says to start a new pool with `tools/jobpool ensure` once status says "not running"; `status_without_fifo` fails 2 checks on the old tool and passes on the new. I6: the review record names a short wrapped header line (`tools/jobpool:19`) and an over-wide docstring line in `jobpool_test.py:10`; the header is reflowed, and no line of the test file or of the file at `01dce7f` exceeds 79 columns, so the author's "did not reproduce" holds. |

## What I checked, and how

- Read the full diff of `tools/measure`, `tools/mutate`, `tools/jobpool` and the three test files, and the code around each change (`git`, `main` and `row` in `tools/measure`; `test` and the listing call in `tools/mutate`; the `serve` loop in `tools/jobpool`).
- Ran, each through `tools/timed` on the reported revision with the pool exported: `tools.measure_record` (pass), `tools.measure_header` (pass), `tools.mutate` (pass, 30 s), `tools.jobpool` (pass, 63 s). Ran each new case against the old tool (copies in the scratchpad) and saw it fail as the author said, then pass on the new tool.
- Ran the citations check on `tools/` and `test/tools/` (fixture directory excluded as in CMake): 100 files, none citing a record. Searched the six changed files for plan, decision and rule citations: none (STYLE S11).
- `tools/runlog check`: passes.
- Checked that CTest's `--parallel` draws on a make-style jobserver: in a scratch project of four 2 s tests, `ctest --parallel` under `make -j2` ran in 4.0 s, under `make -j3` in 4.0 s (three at once, then one), and under `make -j4` in 2.0 s, so the number of tests at once follows the slots granted, as the comment in `tools/mutate` says. `tools/timed` refuses a test step with no FIFO jobserver in `MAKEFLAGS`, so `ctest` there always inherits one.
- Timed the status call that `tools/measure` now makes twice per row: about 12 ms each in this tree.
- Checked the jobpool message against the `serve` loop: a poll that finds no client and no FIFO exits with an error, so "ends at its next check" and "start a new one when status says not running" are correct; clients that hold the old FIFO are not seen, as the loop's comment says.
- Process: test commits precede code commits for U3, U4, U5 (W2); the expected value changed in `7fcd913` is in a commit of its own and listed (W9); each commit names its model; the 4 test-first commits that leave a check failing are labelled.
- Interface checklist: not applicable; no public header changed.

## What I did not check

- The convention trace gate: no `test/` C++ or `include/` change. The tools tests under other presets, clang, or a full P9 matrix: not needed for these units (plan section 8.1: one configuration).
- The U3 timing claim ("about 10 to 50 ms, measured in its test log") beyond my own 12 ms figure; no log of the author's measurement is in the diff.
- The before-and-after mutation outputs: not committed (departure 8); I read only their run-log rows. I did not repeat the mutation run (40 minutes under the present load).
- Reading of departure 4 against the original author's brief: see note N1.

## Findings

### F1, minor: the rewrapped docstring of `tools/mutate` has a 96-column line and detaches a parenthesis

`tools/mutate:39-42`. The edit rewrote "CTest runs the selected tests whose programs now exist (so no test runs a program left from an earlier build)" as "...exist, in parallel on the pool (so no test runs a program left from an earlier build)". The parenthesis explains the "programs now exist" clause, and now reads as if it explains the parallel run. The following line was not reflowed, so line 42 is 96 columns long ("byte for byte; at the end the targets are rebuilt. Every build and test goes through tools/timed"); at `01dce7f` the file's longest docstring lines were 80. Not blocking: it is text in a usage docstring. Suggested fix: put "in parallel on the pool" after the parenthesis, and reflow the paragraph.

### F2, minor: the before-and-after spec run does not show the effect that criterion 4 expects

Rows `db3de04` (before) and `c645e69` (after), same spec, gcc-debug, with the six mutants of `faults_identities.toml`. The baseline `test` row: 23.5 s wall, 23.4 s user before; 34.6 s wall, 34.5 s user after. The other test rows that run the selection (last_term_dropped, one_coefficient_doubled, squares_doubled): wall about equal to user before (22.3 to 24.2 s) and after (36.0 to 38.2 s). In every row wall is about equal to user, so there was no overlap of tests, and the after run was slower. The build rows of the after run are also 3 to 8 times slower than before (for example 708 s against 81 s), and `tools/jobpool status` reads `0 of 23 slots free` at the start and end of this review, so other sessions held the pool: wall times here say nothing about the change (P14), and a saturated pool grants a client its one implicit slot only. The plan's expectation ("the selection's tests at once, bounded by the pool") is therefore neither confirmed nor refuted by these rows. The test I ran with a private jobserver (above) shows the mechanism works. The U6 record and the coordinator's report must state this as found: the code and the test are right; the saving is unmeasured. A quiet-machine repetition, taking the timing lock, would settle it; I do not recommend holding the merge for it. The pool reads before and after that section 6 asks for are not in the committed rows and I could not check them.

### N1, note: departure 4 (the listing call) is a reasonable reading

`tools/mutate:304` is `ctest --show-only=json-v1 -L ...`, which runs no test. "Both CTest calls" read as the baseline and mutant uses of `test` leaves nothing that could run in parallel. Agreed.

### N2, note: departure 5 (start revision, no new column) is a reasonable reading

The row's revision is the start revision, `-dirty` when either read is dirty or HEAD moved. `tools/runlog` keeps rows of committed revisions only, so a run during which HEAD moved is conservatively treated as not committed. No column, reader or compaction change is needed. One consequence to know about: a row whose command itself commits, such as `tools/mutate`'s own edits to tracked files, reads `-dirty` by design (departure 9).

### N3, note: the `git_attribution` change of expected value is correct

`git status` now runs before and after the command, so 2 is the right count; the assertion that it takes no optional locks is unchanged and still applies to both calls. The change is in its own commit and listed.

### N4, note: `status_without_fifo` depends on a poll interval of 30 s for determinism

The case sets `JOBPOOL_POLL_SECONDS=30` for a pool of its own and removes the FIFO at once, so `status` always runs before the service's next poll. It is deterministic unless the case itself is slower than 30 s, and it never touches the shared pool. The run of the whole file took 63 s, as before.

## Open items for the owner

None. F1 and F2 are for the author or coordinator; neither needs an owner decision.

## Outcome

Approve. F1 and F2 are non-blocking; F2 must be reported as found in the plan's outcome.
