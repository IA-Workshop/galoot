# 0028 Job pool: the automatic top-up never makes surplus slots

- Status: claimed 2026-11-08 by Claude (agent, Opus 5.5), implementer with build-expert and cpp-expert judgement. Source: finding F1 of the evidence-tools review (`records/reviews/2026-11-08-evidence-tools-review.md`, branch `records/review-evidence-tools` at `dc2d1d9`) on the top-up plan 0027 merged in `d0b84bc`. On the coordinator's instruction. Not merged by its author; a delta review follows (W5).
- Branches, from `develop` after this claim (W7): `feature/pool-surplus-fix`, worktree `.claude/worktrees/pool-surplus-fix`; tests on `feature/pool-surplus-fix--tests`, worktree `.claude/worktrees/pool-surplus-fix--tests`, cherry-picked with `-x`.

## Goal and definition of done

1. **The defect.** `tools/jobpool`'s service counts clients, then reads and fixes the FIFO in a second step. A client that opens the FIFO and takes tokens between the two steps has its tokens written back, and they become surplus once it returns them: up to the whole pool. Reproduced by the review with real `ninja` and `ctest`, and seen on the shared pool ("wrote back 9 of 23").
2. **The invariant.** Capacity, the free tokens plus those held by live clients, never exceeds the slot total. Tokens held by a live client are never reclaimed.
3. **Tests first.** A regression test on a private pool with several concurrent clients opening, taking, returning, and closing while the service polls. It uses a copy of the tool with a delay between the count and what follows, so that the race shows within seconds. It fails on `develop`'s tool and passes after the fix.
4. **The fix.** The design is chosen and argued in the outcome. Kept: a failed count counts as a client, the resolved FIFO path, and the idle stop.
5. **Checks.** `tools.*` and `tests.*` on gcc-debug and clang-debug, `format-check`, the trace gate, and `tools/runlog check`. The six build presets, lint, and clang-measure run only if a build input changes.

The shared pool is never started, stopped, ensured, or opened by this unit. Agent cost: one implementer session.
