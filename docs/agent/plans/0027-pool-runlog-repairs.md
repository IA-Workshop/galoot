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
