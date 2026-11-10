# 0027 Pool and run-log repairs: lost slots restored, `prune` guarded, the `runlog` label documented

- Status: claimed 2026-11-08 by Claude (agent, Opus 5.5), implementer with build-expert and cpp-expert judgement. Source: decision 0215 item 10, accepting the recommendations of the three-merge review (`records/reviews/2026-11-08-three-merges-review.md`, owner items 1 to 3) and decision 0130 item 7 (automatic repair). Merged into `develop` by `d0b84bc` on 2026-11-08, after the blind review and its delta review below (status corrected 2026-11-10).
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

Done on `feature/pool-runlog-repairs`, ready for the blind review (W5); not merged. (Corrected 2026-11-10: the review and delta review followed in the sections below, and the branch merged as `d0b84bc`.)

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

## State after the blind review (2026-11-08)

The blind review (`records/reviews/2026-11-08-pool-runlog-review.md`, cherry-picked with `-x` as `9d76bb9`) approved with findings M1, M2, L3 to L6, and I7. The coordinator's resolutions, done by the implementer (Claude, agent, Opus 5.5):

| Commit | What | Finding | Kind (W9) |
| --- | --- | --- | --- |
| `bd45823` | cherry-picked with `-x` from `df00fb4` on the tests branch (after merging this branch into it): `tools.jobpool` gains a pool on a FIFO path through a symlinked directory and a copy of the tool whose count of clients fails; the lost-slot check takes every token; label `slow`. Fails until `34d5af3`, as its message says | M1, M2, L4, L6 | test, before the code it judges |
| `8489cd2` | `no_clients`: only a count of exactly 0 is no client, at the poll and at the last count before an idle stop | M1 | code, needs a delta review |
| `34d5af3` | the FIFO path is resolved with `realpath -m` before any use | M2 | code, needs a delta review |
| `b99bc7d` | the top-up race stated in the comment of `balance`; the drain reports what it read; `status` reports the service's idle time and says when the FIFO is missing | L3, I7 | code |
| `d920335` | P9 and the PERFORMANCE status line reverted to `develop`'s text | L5 | governance text restored |

- **L5.** The message of `ba67851` calls the P9 wording "the owner's accepted recommendation". That overstates it: the owner accepted documenting the label, not this wording. The wording is reverted in `d920335`, and the label's command stays documented in the comment of `tools.runlog_check` and in `tools/runlog`'s usage text. History is not rewritten.
- **L3, the known race.** A client that opens the FIFO between the count of clients and the top-up, and takes tokens, leaves up to n surplus tokens once it returns them. They stay until the next poll with no client. No test covers it. The review's suggested fix, topping up only while the FIFO is moved aside, was not taken.
- **L4.** No test preset excludes `slow`; only `runlog` is excluded. So `tools.jobpool` still runs in every preset's full suite (P9). A P8 selection that leaves out `slow` skips it unless the change touches `tools/jobpool`, as P9's rule for slow tests says.
- **I7.** A client that starts during the one-second window while the FIFO is moved aside gets ENOENT, and `tools/timed` refuses with exit 2 in that window; this replaces the double pool. An `ensure` from an older checkout still starts the old code. Neither is changed.
- The tests before the fixes: four checks of the two new cases failed on the branch's tool (27.7 s), and the lost-slot check now fails on `develop`'s tool as well. After them, `tools.jobpool` passes (29.2 s). Every pool was a private unit. From this round on, the shared pool's status is read only with the main checkout's `tools/jobpool`.
- P9's checks for a merge into `develop`, rerun from new empty trees at `2e8aa68` with a clean tree (rows `2e8aa680dae1`, selection `gate:all`), all exit 0: gcc-debug 494 tests (64.3 s), gcc-release 494 (75.0 s), gcc-asan 486 (173.8 s), clang-debug 494 (58.7 s), clang-release 494 (78.2 s), clang-asan 494 (155.4 s), lint 494 (201.0 s), clang-measure 486 (77.3 s); `tools.jobpool` took 27.1 to 29.2 s in each. `format-check` (1.3 s), the trace gate (0.3 s), and `tools/runlog check` (2.7 s) exit 0. `develop` had not moved. The shared pool read 23 of 23 free before and after, read with the main checkout's tool. Next: the delta review of `8489cd2` and `34d5af3`.

## State after the delta review (2026-11-08)

The delta review (`records/reviews/2026-11-08-pool-runlog-delta-review.md`, cherry-picked with `-x`) approved with findings L1, I2 to I5.

- **L1.** `aa98f82`, cherry-picked with `-x` from `1ac84b7` on the tests branch, adds a case to `tools.jobpool`. It runs a copy of the tool whose count fails only for the FIFO moved aside, and checks that the pool keeps running with its FIFO back at its path. It passes on the tool (34.5 s). With the last count put back to the old `[[ $(count_clients …) -eq 0 ]]`, its two checks fail (39.5 s, a scratch copy of the tool; the mutant is not committed). The same commit rewrites the module docstring to name every case (I4).
- **Known, no code (I2, I3, I5).**
  - I2: resolving `JOBPOOL_FIFO` changes what an override touches. A link as the last component makes the service remove and recreate the link's target. A dangling link into a missing directory makes `start` fail. Without GNU `realpath`, the path would be empty. The default path is unaffected.
  - I3: `status`'s missing-FIFO message assumes the pool is stopping. A FIFO deleted by hand gets the same text and no hint to restart. `status` exit 1 now also covers a running pool without its FIFO.
  - I5: `stop` and `status` still find clients through the `find` pipeline in `clients()`, which fails open. A failed scan lets `stop` go ahead without `--force`.
- Checks at `e2984bf` from new empty trees with a clean tree (rows `e2984bfdde1d`), no CMake input changed since `2e8aa68`: gcc-debug and clang-debug full suites, 494 each (66.9 s, 63.6 s); `-L '^tools$'` 40 on each (35.3 s, 36.4 s); `format-check` (1.5 s), the trace gate (0.2 s), and `tools/runlog check` (2.6 s), all exit 0. The shared pool read 23 of 23 free, read with the main checkout's tool.
