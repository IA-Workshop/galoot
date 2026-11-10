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

## State at the end of the implementer's session (2026-11-08)

| Commit | What | Kind (W9) |
| --- | --- | --- |
| `33910af` | the evidence-tools review record, cherry-picked with `-x` from `dc2d1d9` (F1 is the source of this unit) | record |
| `328a77b` | cherry-picked with `-x` from `950fad1` on the tests branch: `tools.jobpool` case `surplus_race`, and the CMake comment of the test. Fails until `6bc4674`, as its message says | test, before the code it judges |
| `6bc4674` | `tools/jobpool`: the tokens are fixed only while the FIFO is moved aside and a second count finds no client | code |

**The test.** A copy of the tool touches `<fifo>.counted` after each count of clients and then waits 0.5 s. In that window the test opens the FIFO and takes 2 of 4 tokens. It checks that nothing is written back while it holds them and that exactly 4 are free once it returns them, in three rounds. Then three Python clients open, take 1 or 2, hold, return, and close, for 10 s, while the test samples the free count every 20 ms. On `develop`'s tool every round fails (4 free while 2 are held, then 6 of 4) and the load sees a surplus (57.8 s, exit 1). With the fix the test passes (59.5 s). A mutant that moves the FIFO aside but skips the second count fails rounds and load (57.8 s). Without the marker and the half-second delay, the load alone did not reproduce the race in 20 s on `develop`'s tool, so the deterministic rounds are the proof; the load is a realism check. Real `ninja` and `ctest` were not run against the fix; the review's reproduction with them was on the old tool.

**The designs weighed.**

- *Fix only with the FIFO moved aside, after a second count* (chosen). With the FIFO aside, no process can open it by path. After the wait, which lets any open that resolved the old path finish, a count of 0 means no process holds it. Every token is then in the FIFO, and setting it to n is exact. The cost is an ENOENT window of 0.2 s, but only at polls with no client whose free count is off, that is, at a real loss or surplus or the race itself. `tools/timed` refuses with exit 2 in that window; a bare `ninja` would run without the pool.
- *The same deficit on two consecutive polls with no client.* This only makes the race less likely. Continuous churn can repeat it, and real repairs wait a poll longer. Rejected: it proves nothing.
- *Never write back while any client exists, with a count that cannot miss a just-opened FIFO.* No such count exists without moving the FIFO. A `/proc` scan is not atomic, and a FIFO has no open-count interface. Reading the free count before and after the count narrows the window, but a client that returns tokens while another takes the same number defeats it. Rejected.

**The invariant.** Capacity is the free tokens plus those held by live clients. Assume, as for Ninja, CTest, and GNU Make clients, that a live process holding tokens holds the FIFO open.

1. The service reads or writes tokens only inside `balance`.
2. `balance` runs only while the FIFO is moved aside and a count after the wait has found no holder. Then no live process holds a token, and none can open the FIFO before it goes back.
3. So `balance` sees the whole capacity, and leaves exactly n.
4. Clients take and return only their own tokens, so outside `balance` capacity never grows.
5. Hence capacity never exceeds n, and `balance` never reclaims a token a live client holds.

Not covered:

- A holder that forks a child that inherits the descriptor, then exits during the scan, after the child's entry was read. This is contrived, and the FIFO is opened with no inheritance by the clients in use here.
- A process that keeps tokens after closing the FIFO.

**Kept.** A failed count is a client (the existing `count_fails` and `last_count_fails` cases pass). The FIFO path is resolved (`symlinked` passes). The idle stop moves the FIFO aside with its 1 s wait (`idle_stop` passes).

**Checks** at `4777e22`, from new empty trees with a clean tree (rows `4777e2206186`, selection `gate:all`). The CMake comment of `tools.jobpool` changed, so all eight presets ran, each its full suite, which holds `tools.*` and `tests.*`. All exit 0:

| Preset | Tests | Wall time |
| --- | --- | --- |
| gcc-debug | 494 | 91.1 s |
| gcc-release | 494 | 101.4 s |
| gcc-asan | 486 | 210.8 s |
| clang-debug | 494 | 87.3 s |
| clang-release | 494 | 108.2 s |
| clang-asan | 494 | 221.2 s |
| lint | 494 | 236.6 s |
| clang-measure | 486 | 112.5 s |

`tools.jobpool` now takes 56.6 to 57.1 s in each preset. `format-check` (1.3 s), the trace gate (0.2 s), and `tools/runlog check` (2.6 s) exit 0. The shared pool was read only, with the main checkout's tool.

Open for the owner:

1. **The shared pool runs `develop`'s tool, with the race, until it restarts.** Once this merges, restart it when no agent builds: `cd /home/brianm/projects/utils/math-core/galoot && tools/jobpool status && tools/jobpool stop && tools/jobpool start 23`. Recommended. The alternative is waiting for an idle stop, which on a busy machine may not come.
2. **`tools.jobpool` costs about a minute of mostly sleeping per preset run.** Options:
   - (a) Keep it. Recommended: the race case is the only guard of the invariant.
   - (b) Shorten the load phase, or drop it. The deterministic rounds are the proof.

## Resolution of the review's findings (recorded 2026-11-10, from the repository)

The blind review of this unit is `records/reviews/2026-11-08-pool-surplus-review.md`, made at `a83b066` and merged into `develop` by `2adc87f`. The review record reached `develop` only at this entry, cherry-picked with `-x` from `1af8dc0` (finding MA1 of the M5 audit). Its verdict was to approve with non-blocking findings. The sources of each resolution below are the files at `develop` `2fc19c9`: `tools/jobpool` and `test/tools/jobpool_test.py` have not changed since `6bc4674`, the fix itself (`git diff 6bc4674 2fc19c9 -- tools/jobpool test/tools/jobpool_test.py` is empty), and no later plan, decision, or record names the findings.

| Finding | Resolution | Evidence |
| --- | --- | --- |
| L1: the `free != n` gate is untested | **Open.** No recorded resolution. No fix and no acceptance. | `tools/jobpool:182` still has the gate. No test in `test/tools/jobpool_test.py` watches the path of an idle, full pool across polls |
| L2: the service no longer stops itself after its FIFO is deleted by hand | **Open.** No recorded resolution. Not fixed, not accepted. | `tools/jobpool:181` is still `free=$(free_slots 3>&-) \|\| continue`, and the service has no `[[ -p $fifo ]]` exit |
| L3: an unreadable `/proc/<pid>/fd` hides a client (pre-existing) | **Open.** No recorded resolution. The review recommended accepting it and stating it in the invariant; that statement was not made. | The "Not covered" list of the invariant above names only the forked child and the process that keeps tokens after closing the FIFO. `count_clients` still skips an entry it cannot read (`tools/jobpool:124` to `127`) |

The review's other findings have no recorded resolution either: L4 (`tools/timed` says "no shared job-slot pool" in the 0.2 s window and could wait while `<fifo>.stopping` exists), I5 (the 0.2 s wait bounds scheduling and proves nothing), I6 (a wrapped header comment and a long docstring line), and I7 (the spike copy of the pool, note 0016, keeps its own count-then-balance design). Its "Open for the owner" items 1 (restart the shared pool after the merge) and 4 (the cost of `tools.jobpool`) also have no resolution in the repository. The owner has not ruled on the follow-up unit it recommended (item 2, option (a)) or on accepting L3 (item 3, option (a)).
