# Delta review of the pool and run-log repairs (plan 0027)

- Reviewer: blind-reviewer role (Claude, agent, Opus 5.5). Date: 2026-11-08.
- Reviewed: `git diff f9d02f6..8d9e0c4` on `feature/pool-runlog-repairs` (commits `9d76bb9`, `bd45823`, `8489cd2`, `34d5af3`, `b99bc7d`, `d920335`, `2e8aa68`, `8d9e0c4`), against the findings of `2026-11-08-pool-runlog-review.md`.
- Record branch `records/review-pool-delta`, worktree `.claude/worktrees/review-pool-delta`, from `8d9e0c4`. Only this file is committed.
- The brief carried no verdict. It gave the authors' commit list, their departures, and the checks to make.

## Departures, skipped checks, ambiguities (C20)

- The role file gives this reviewer no Write tool and says the merging agent commits the record. The brief asked me to commit it, so I did.
- Authors' departures, judged:
  - The failing-count test runs a copy of the tool whose `count_clients` returns 1. Acceptable: the test first checks that the anchor occurs exactly once, so a rename makes it fail, not pass vacuously.
  - The race fix of L3 (top up only while the FIFO is moved aside) was not taken; the race is stated in the comment of `balance`. Acceptable as a known, bounded surplus. It stays open (owner item 2).
  - Only `runlog` is excluded from the presets. Confirmed: `ctest --preset gcc-debug -N -L '^slow$'` lists `tools.jobpool` and `core.identities`, so no preset drops the test.
- Run rows went to a scratch log, not to `runs.tsv`. They are listed at the end.
- Not run: gcc-release, gcc-asan, clang-release, clang-asan, lint, clang-measure. For those I read the authors' rows at `2e8aa68`; `2e8aa68..8d9e0c4` changes only the plan file and `runs.tsv`, so they hold for the tip.
- Not tried: real Ninja or CTest clients on a private pool; a reproduction of the L3 race; the tool on a system without GNU `realpath`.

## What I checked

- **Commit order (W9).** `bd45823` has the patch-id of `df00fb4` and `9d76bb9` that of `55e2d48`. The test commit precedes the code it judges. Every commit names the agent and model.
- **The new tests discriminate.** I ran the tip's `jobpool_test.py` against scratch copies of the tool, each on private units and FIFOs only.
  - At `f9d02f6`: 4 checks fail (2 symlinked, 2 count-fails), exit 1. This matches the message of `bd45823`.
  - At `8489cd2`: the 2 symlinked checks still fail. At `34d5af3` and at the tip, all pass (28.9 s).
  - At `develop`: "lost slots written back" now fails, so L6 is fixed.
- **M1 fails closed.** `no_clients` declares `count` before assigning it, so the status of the count is not masked, and it accepts only the string `0`.
  - Poll: a failed count resets the idle clock and skips `balance`. The top-up and the drain are both inside `balance`, so both are skipped.
  - Recount before the idle stop: a failed count moves the FIFO back.
  - A pool that is really idle still tops up and stops itself: the `repair` and `idle_stop` cases pass at the tip.
- **M2.** I ran `realpath -m` on these paths, and each gives the physical absolute path: a path in a missing directory, a path through a symlinked directory, `link/sub/../x`, `link//x`, a relative path, and a dangling link.
  - A private pool started with the relative `JOBPOOL_FIFO=rel.fifo` from inside a symlinked directory created the FIFO at `real/rel.fifo`. `env`, `status`, and `stop` all agreed on that path.
- **I7.**
  - On private pools, `status` prints "90 seconds" for 90 and "2 minutes" for 120. It prints no idle line for `--idle 0`. With the FIFO moved aside, it prints the missing-FIFO line and exits 1.
  - The tip's `status` on the live shared pool, which old code started, printed the same two lines as `develop`'s tool. That was a read only.
  - Callers: `tools/timed` does not call `status`; it checks `-p` and parses `MAKEFLAGS`. `tools/measure` does not use the pool. The math-tools scripts only log `status` text, either the whole output or `head -n1`. No caller parses the changed lines.
- **L5.** `git diff develop 8d9e0c4 -- docs/agent/governance/PERFORMANCE.md` is empty.
- **Runs**, from fresh trees at `8d9e0c4`, each through `tools/timed` after `eval "$(tools/jobpool env)"`:
  - Configure and build: exit 0 on both presets.
  - `ctest --preset <p> -R '^(tools|tests)\.'`: 45 of 45 pass on gcc-debug and 45 of 45 on clang-debug, with `tools.jobpool` passing in 28 s.
  - The trace gate exits 0. `format-check` exits 0 on both presets. `tools/runlog check` exits 0.
- **Shared pool.** It read 23 of 23 free before and after; mid-run, 22 of 23 were free while another agent's `ctest` was a client. I never started, stopped, or ensured it. No test or probe unit was left behind.

## Findings, most severe first

- **L1, low, confirmed, non-blocking: the fail-closed recount is untested** (`tools/jobpool:171`).
  - **Mutant.** I put line 171 back to the old `[[ $(count_clients …) -eq 0 ]]`. `tools.jobpool` still passes (28.8 s).
  - **Why.** A count that always fails resets the idle clock at every poll, so the run never reaches the recount.
  - **Failure scenario.** A regression on that line alone would remove the FIFO under a late client whenever its count fails, and no test would notice.
  - **Fix.** Add a copy of the tool whose count fails only for a path ending in `.stopping`. Check that the pool stays active and that the FIFO is back at its path.
- **I2, info: resolving the path changes what an override touches** (`tools/jobpool:43`).
  - **A link as the last component.** When the last component of `JOBPOOL_FIFO` is itself a symlink, the service now runs `rm -f` and `mkfifo` on the link's target, not on the link.
  - **A dangling link.** A link whose target lies in a missing directory now makes `start` fail.
  - **Missing realpath.** Without GNU `realpath`, `fifo` would be empty, and the tool would give no clear error.
  - **Scope.** All three need an override or a non-GNU system; the default path is unaffected.
- **I3, info: the missing-FIFO message assumes the pool is stopping** (`tools/jobpool:242`). A FIFO that was deleted by hand gets the same text and no hint to restart. `status` exit 1 now also means "running".
- **I4, info.** The module docstring of `test/tools/jobpool_test.py:1-2` does not mention the two new cases. The CMake comment does.
- **I5, info, outside the delta.** `stop` and `status` still find clients with the `find` pipeline in `clients()`, which fails open. A failed scan lets `stop` proceed without `--force`.

## Outcome

Approve with non-blocking findings. M1, M2, L4, L5, L6, and I7 are resolved, and L3 is documented as the authors say.

## Open for the owner

1. **L1, test of the recount.**
   - (a) Add the `.stopping`-only failing copy now, as a test commit before the merge (about 15 minutes, plus 5 s of test time). **Recommended:** the line it guards is the one that can remove the pool under a client.
   - (b) Leave it for the next tools unit. That leaves the line guarded by reading only.
2. **The L3 race** stays documented only.
   - (a) Accept it. **Recommended for now:** the surplus is bounded by n and is drained at the next idle poll.
   - (b) Take the review's fix (top up while the FIFO is aside) in the extraction (note 0016).
3. **Restart of the shared pool after the merge**, so that it runs the fixed code. When no agent is building, run: `cd /home/brianm/projects/utils/math-core/galoot && tools/jobpool status && tools/jobpool stop && tools/jobpool start 23`. **Recommended.** Otherwise the old code runs until an idle stop.

## Run rows (scratch log, not committed)

| utc_start | revision | step | preset | selection | wall_s | exit |
| --- | --- | --- | --- | --- | --- | --- |
| 2026-11-08T16:08:53Z | 8d9e0c48297c | configure | gcc-debug | review:all | 0.624 | 0 |
| 2026-11-08T16:08:53Z | 8d9e0c48297c | build | gcc-debug | review:all | 33.854 | 0 |
| 2026-11-08T16:09:27Z | 8d9e0c48297c | configure | clang-debug | review:all | 0.417 | 0 |
| 2026-11-08T16:09:28Z | 8d9e0c48297c | build | clang-debug | review:all | 29.353 | 0 |
| 2026-11-08T16:10:05Z | 8d9e0c48297c | test | gcc-debug | review:tools\|tests | 28.388 | 0 |
| 2026-11-08T16:10:33Z | 8d9e0c48297c | test | clang-debug | review:tools\|tests | 28.091 | 0 |
| 2026-11-08T16:11:01Z | 8d9e0c48297c | trace | - | review:trace-gate | 0.200 | 0 |
| 2026-11-08T16:11:02Z | 8d9e0c48297c | check | gcc-debug | review:format-check | 1.326 | 0 |
| 2026-11-08T16:11:03Z | 8d9e0c48297c | check | clang-debug | review:format-check | 1.291 | 0 |

`tools/runlog check` took 2.6 s and exited 0. The runs of the tip's test against the six scratch copies of the tool were untimed, each about 28 s; the run against `develop`'s tool took 126 s.
