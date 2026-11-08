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
