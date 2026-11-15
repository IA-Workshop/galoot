# Blind review of the pool and run-log repairs (plan 0027)

- Reviewer: blind-reviewer role (Claude, agent, Opus 5.5). Date: 2026-11-08.
- Reviewed: `git diff develop...feature/pool-runlog-repairs`, `d15b4f8..f9d02f6` (commits `5d11c2d`, `4c68562`, `b902133`, `ba67851`, `05dffcb`, `f9d02f6`).
- Record branch `records/review-pool-runlog`, worktree `.claude/worktrees/review-pool-runlog`, from `f9d02f6`. Only this file is committed.
- The brief carried no verdict on the change. It gave the authors' departures and asked for specific checks.

## Departures, skipped checks, ambiguities (C20)

- The role file gives this reviewer no Write tool and says the merging agent commits the record. The brief asked me to commit it, so I did.
- Run rows went to a scratch log, not to `runs.tsv`, so that this branch commits only this record. They are listed at the end.
- Not run: gcc-release, gcc-asan, clang-asan, clang-release, lint, clang-measure, and the full suite on any preset. For those I read the authors' rows. They were made at `05dffcb`, and the rows from gcc-release on are marked `-dirty`. I confirmed that `05dffcb..f9d02f6` changes only the plan file and `runs.tsv`, so for those configurations the authors' rows are the only evidence.
- Not tried: a private pool with real Ninja or CTest clients; the `status` message for a pool that repairs itself (the authors checked it by hand, and no test covers it); flakiness under a full-suite load.
- Ambiguity: decision 0215 item 10 accepts "documenting the `runlog` test label". The owner did not see the P9 wording that was added. See L5.

## What I checked

- **Commit order (W9).** The cherry-picks `5d11c2d` and `4c68562` have the same patch-ids as `09145f1` and `17fd8ff`. Test commits and code commits are separate.
- **The new tests fail on the old tools.** I ran them in a scratch copy against `develop`'s `tools/runlog` and `tools/jobpool`, with the pools on private units only.
  - `tools.runlog`: 11 checks of case 14 fail.
  - `tools.jobpool`: 13 checks fail, and the idle case times out (75 s).
  - At the tip, both tests pass.
- **No test is weaker.** The diff only adds a case, a file, and two CMake registrations.
- **Callers.** The format of `start`, `env`, the first line of `status`, and `stop` is unchanged. `tools/timed` parses only `MAKEFLAGS` and checks `-p`. The math-tools scripts read `status | head -n1`.
- **R1.** `prune` compares eras and groups through `recompute()`, which `check` also uses. It collects every log before it writes any. The damaged-era case does discriminate, because the run log is visited first.
- **Top-up logic.** I reasoned it through and probed it on private pools of my own.
  - The service tops up only when it counts 0 clients, so tokens held by a visible client are never added back.
  - The drain cannot take the total below n, even while a new client is taking tokens.
  - A client that holds all 3 tokens through 4 polls leaves 0 free.
- **J3.** The service moves the FIFO aside, waits, counts again, and moves it back if a client appeared. `ExecStopPost` removes both paths.
- **Runs.** Each was a fresh tree in this worktree at `f9d02f6`, run through `tools/timed` after `eval "$(tools/jobpool env)"`.
  - `ctest -R '^(tools|tests)\.'`: 45 of 45 pass on gcc-debug and 45 of 45 on clang-debug, `tools.jobpool` among them, not skipped.
  - The trace gate exits 0.
  - `format-check` exits 0 on both presets.
  - `tools/runlog check` on the real `runs.tsv` exits 0.
  - `ctest --test-dir build/gcc-debug -L '^runlog$'` runs one test, which passes. `ctest --preset gcc-debug -N -L '^runlog$'` lists 0.
- **Shared pool.** It read 23 of 23 free before and after every run. I did not start, stop, or open it with the changed code, and no test or probe unit was left behind.

## Findings, most severe first

- **M1, medium, plausible, non-blocking: client counting fails open** (`tools/jobpool:145`, `:155`).
  - `count_clients` failing with no output reads as 0 clients. In bash, `[[ "" -gt 0 ]]` is false and `[[ "" -eq 0 ]]` is true, as I checked.
  - **Failure scenario.** A transient fork or exec failure of `python3` during a poll makes `balance` write back the tokens that live clients hold, up to n extra slots. During the recount before an idle stop, it removes the pool under a client.
  - **Fix.** Treat output that is not a number as "clients present".
- **M2, medium, confirmed, latent, non-blocking: clients are matched by the path string** (`tools/jobpool:113`, `:77`).
  - **Probe.** A private pool on a FIFO path through a symlinked directory, with a client holding all 3 tokens, was back to 3 free within 4 s. Every poll then adds what the clients hold, so the capacity grows without bound.
  - **Scope.** The default `/run/user/1000/jobserver.fifo` is canonical, so only an override of `JOBPOOL_FIFO` or `XDG_RUNTIME_DIR` reaches this.
  - **Fix.** Resolve the FIFO path with `realpath`, or match clients by device and inode.
- **L3, low: the race between the count and the top-up is acknowledged and untested** (`tools/jobpool:50-53`).
  - A surplus of up to n tokens persists until the next poll with no client, which is rare on a busy machine.
  - **Fix.** Top up only while the FIFO is moved aside, and only when the change is nonzero. The service already has the means.
- **L4, low: an unlisted departure from the plan.** Plan 0027 definition of done item 4 says the test carries the label `slow`. `tools.jobpool` does not (`test/CMakeLists.txt:865`). It runs for 17 to 18 s holding a shared CTest slot, in each of the eight presets, although its result does not depend on the configuration.
- **L5, low, governance: the P9 edit.**
  - **Meaning.** In my judgment it does not change P9's meaning. It adds no check, drops none, and changes no scope. It states a consequence and a command. So it is documentation.
  - **Ratification.** It is still a textual change to a ratified document (Constitution, Amendment). The owner ratified "documenting" the label, not this wording. The status line and the message of `ba67851` ("the owner's accepted recommendation") overstate that.
  - **Owner item.** See item 1 below.
- **L6, low: one check passes on the old tool.** In my run against `develop`'s `tools/jobpool`, "lost slots written back" passed, because the surplus that was not drained masked the 2 taken tokens (`test/tools/jobpool_test.py:190-194`). The suite still catches a missing top-up through the check on the killed client.
- **I7, info.**
  - `status` reports `--idle` minutes, not `JOBPOOL_IDLE_SECONDS`.
  - During the 1 s aside window:
    - `status` prints an empty count.
    - `tools/timed` refuses with exit 2.
    - A client started then gets ENOENT. This failure mode replaces the double pool.
  - The drain message prints the number requested, not the number read.
  - The shared pool runs the new code only after a restart. An `ensure` from an older worktree still starts the old code.

## Outcome

Approve with non-blocking findings. M1 and M2 are a few lines each in the logic this unit adds. If they are fixed here, the fix needs a delta review.

## Open for the owner

1. **P9 wording (L5).**
   - (a) Ratify it as worded. **Recommended:** the meaning is unchanged, and decision 0215 item 2 is a precedent.
   - (b) Revert the P9 hunk. The CMake comment and the usage text already document the command.
2. **M1 and M2.**
   - (a) Fix them in this unit before the merge, with a delta review. **Recommended:** both are small, and they guard against the over-subscription the top-up can cause.
   - (b) Merge now and fix them in the next tools unit. That leaves them latent meanwhile.
3. **The `slow` label (L4).**
   - (a) Add the label, as the plan says. **Recommended.**
   - (b) Amend the plan instead.
4. **Restart of the shared pool once this merges.** When no client holds the pool, run: `cd /home/brianm/projects/utils/math-core/galoot && tools/jobpool status && tools/jobpool stop && tools/jobpool start 23`. **Recommended:** otherwise the old code stays until an idle stop, and the next `ensure` may come from an older checkout.
5. **The slot total read from the unit's description** (J4, third point) stays with the extraction (decision 0130). No action is needed.

## Run rows (scratch log, not committed)

| utc_start | revision | step | preset | selection | wall_s | exit |
| --- | --- | --- | --- | --- | --- | --- |
| 2026-11-08T15:36:45Z | f9d02f6e4ce6 | configure | gcc-debug | review:all | 0.329 | 0 |
| 2026-11-08T15:36:46Z | f9d02f6e4ce6 | test | gcc-debug | review:tools\|tests | 17.931 | 0 |
| 2026-11-08T15:37:04Z | f9d02f6e4ce6 | configure | clang-debug | review:all | 0.332 | 0 |
| 2026-11-08T15:37:04Z | f9d02f6e4ce6 | test | clang-debug | review:tools\|tests | 16.806 | 0 |
| 2026-11-08T15:37:36Z | f9d02f6e4ce6 | trace | - | review:trace-gate | 0.243 | 0 |
| 2026-11-08T15:37:36Z | f9d02f6e4ce6 | check | gcc-debug | review:format-check | 1.292 | 0 |
| 2026-11-08T15:37:38Z | f9d02f6e4ce6 | check | clang-debug | review:format-check | 1.258 | 0 |
| 2026-11-08T15:37:39Z | f9d02f6e4ce6 | check | - | review:runlog | 2.541 | 0 |
| 2026-11-08T15:37:41Z | f9d02f6e4ce6 | test | gcc-debug | review:runlog-label | 2.443 | 0 |

The runs against the old tools (runlog 10.6 s, exit 1; jobpool 75.0 s, exit 1) and the private-pool probes ran untimed in the scratch directory.
