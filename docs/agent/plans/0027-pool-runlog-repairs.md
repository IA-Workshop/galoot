# 0027 Pool and run-log repairs: lost slots restored, `prune` guarded, the `runlog` label documented

- Status: claimed 2026-11-08 by Claude (agent, Opus 5.5), implementer with build-expert and cpp-expert judgement. Source: decision 0215 item 10, accepting the recommendations of the three-merge review (`records/reviews/2026-11-08-three-merges-review.md`, owner items 1 to 3) and decision 0130 item 7 (automatic repair). Not merged by its author; a blind review follows (W5).
- Branches, from `develop` after this claim (W7):
  - `feature/pool-runlog-repairs`, worktree `.claude/worktrees/pool-runlog-repairs`: the unit.
  - `feature/pool-runlog-repairs--tests`, worktree `.claude/worktrees/pool-runlog-repairs--tests`: the tests, written first and cherry-picked with `-x`.
- Shared with plan 0026, which runs at the same time: `test/CMakeLists.txt` and `runs.tsv`. Whichever unit merges second merges its parent first.

## Goal and definition of done

1. **J1, lost slots.** `tools/jobpool`'s service, at each poll that finds no client, writes back the tokens missing from its total and drains any surplus, so lost slots come back without a restart. This also holds with `--idle 0`.
2. **R1, `prune` guard.** `tools/runlog prune` refuses, with exit 2 and every log unchanged, unless every era of every log it visits recomputes exactly as `eras.tsv` and `groups.tsv` record it.
3. **R2, the label.** P9, the CMake comment of `tools.runlog_check`, and the tool's usage text say that the check runs with `ctest --test-dir <tree> -L '^runlog$'`, since the test presets leave the label out.
4. **J2, a test.** A CTest test of `tools/jobpool` on a private unit and FIFO (never the shared pool): top-up, drain, no top-up while a client holds the FIFO, the argument refusals, and the idle stop (label `slow`).
5. **J3, the idle-stop race**, if small: before it exits, the service moves the FIFO aside, counts clients again, and puts it back if one appeared.
6. **J4, argument parsing**, if small: `ensure` takes no arguments; `start` refuses a second slot count or an unknown option.
7. Tests before code (W2, W9), P8 during work, then P9's checks for a merge into `develop` from new empty trees on the six build presets, lint, clang-measure, the format check, the trace gate, and `tools/runlog check`.

Anything found too large for this unit is listed in the outcome instead. Agent cost: one implementer session.

## Not in scope

The extraction of the pool (decision 0130), the slot total read from the unit's description text (note 0016 section 4 item 7), and any start, stop, or restart of the shared pool.

## State at the end of the implementer's session (2026-11-08)

Done on `feature/pool-runlog-repairs`, ready for the blind review (W5); not merged.

| Commit | What | Kind (W9) |
| --- | --- | --- |
| `5d11c2d` | cherry-picked with `-x` from `09145f1` on `feature/pool-runlog-repairs--tests`: case 14 of `tools.runlog`, `prune` refusals (a narrowed rule in a copy of the tool, a damaged era row of the second log, a damaged group row), every log unchanged; fails until `b902133`, as its message says | test, before the code it judges |
| `4c68562` | cherry-picked with `-x` from `17fd8ff`: `tools.jobpool` (`test/tools/jobpool_test.py`, registered in `test/CMakeLists.txt`), on private units with FIFOs in the build tree; fails until `05dffcb`, as its message says | test, before the code it judges |
| `b902133` | `tools/runlog`: `prune` runs the comparison `check` runs (shared `recompute()`), refuses on the first era that differs, and reads every log before it rewrites any; usage text names the `runlog` label's command | code (R1, R2) |
| `ba67851` | P9 sentence and the `tools.runlog_check` comment give `ctest --test-dir <tree> -L '^runlog$'` | governance wording accepted in decision 0215 item 10; comment (R2) |
| `05dffcb` | `tools/jobpool`: top-up and drain at each poll without a client, also with `--idle 0`; the idle stop moves the FIFO aside, waits 1 s, recounts, and puts it back if a client appeared; `ensure` refuses arguments, `start` refuses a second count and unknown options; `JOBPOOL_IDLE_SECONDS`; `status` says the pool restores slots itself when the running service sets `JOBPOOL_REPAIR=auto` | code (J1, J3, J4) |

Evidence. Before the code: `tools.runlog` failed only in case 14 (2.97 s) and `tools.jobpool` failed in 11 argument checks, both write-backs, and the idle case (74.7 s). After: P8's selection on gcc-debug, `tools.runlog` and `tools.jobpool` pass (17.8 s), `tests.*` 5 pass (0.5 s), `ctest --test-dir build/gcc-debug -L '^runlog$'` runs `tools.runlog_check` and passes (2.5 s), and `ctest --preset gcc-debug -N -L '^runlog$'` lists 0 tests. A hand check on a private unit with a 30 s poll showed `status` printing "no client holds the pool, and 2 of 3 slots are free; it brings them back to 3 at its next check" after a lost token and "4 of 3" after a surplus. The shared pool read 23 of 23 free before and after every run, and was never started, stopped, or opened by a test.

P9's checks for a merge into `develop`, from new empty trees at `05dffcb` (selection `gate:all`, `cmake --workflow`), all exit 0: gcc-debug 494 tests (50.1 s), gcc-release 494 (61.0 s), gcc-asan 486 (157.4 s), clang-debug 494 (47.0 s), clang-release 494 (65.2 s), clang-asan 494 (143.4 s), lint 494 (179.4 s), clang-measure 486 (67.0 s); `tools.jobpool` took 16.8 to 18.1 s in each. `format-check` (1.3 s), the trace gate (0.2 s; 6.1 to 6.4 without a test, as on `develop`), and `tools/runlog check` (2.6 s) exit 0. The rows from gcc-release on carry `05dffcbb0ac8-dirty`: this section was being written in the tracked plan file while they ran; no file that a build or test reads changed. `develop` had not moved since the claim, so no merge of the parent was needed.

Not done, listed: the slot total is still read from the unit's description text (note 0016 section 4 item 7; the review's J4 third point). The automatic tests do not reproduce the race between the count and the top-up, which leaves a surplus until a later poll by design.
