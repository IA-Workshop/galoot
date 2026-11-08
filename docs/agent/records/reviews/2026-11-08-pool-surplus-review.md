# Blind review of the pool surplus fix (plan 0028)

- Reviewer: blind-reviewer role (Claude, agent, Opus 5.5). Date: 2026-11-08.
- Reviewed: `git diff develop...feature/pool-surplus-fix`, `develop` at `b738e6e`, tip `a83b066` (commits `33910af`, `328a77b`, `6bc4674`, `4777e22`, `a83b066`).
- Record branch `records/review-pool-surplus`, worktree `.claude/worktrees/review-pool-surplus`, from `a83b066`. Only this file is committed.
- The brief carried no verdict. It gave the authors' departures, the design and its rejected options, the invariant, the known costs, and the checks to make.

## Departures, skipped checks, ambiguities (C20)

- The role file gives this reviewer no Write tool and says the merging agent commits the record. The brief asked me to commit it, so I did.
- Run rows went to a scratch log, not to `runs.tsv`. They are listed at the end.
- Not run: the six other presets, lint, and `tools/runlog check`. For those I read the authors' rows at `4777e22`. `4777e22..a83b066` changes only the plan file and `runs.tsv`.
- Not available on this machine: the sources of GNU Make, Ninja, and CMake. I read their installed manuals (`make.info` 4.4.1, Ninja's `manual.asciidoc` 1.13.2, `ctest.1.rst` 4.4.5). For what the manuals leave open, I traced each binary with `strace` on a private FIFO.
- Shared pool: never started, stopped, ensured, or opened by a probe. I read its status before and after (23 of 23 free at the end) and read its journal. My first status read used the tip's tool. It reads the same way as `develop`'s.
- Authors' departures, judged:
  - The regression clients are Python. Acceptable: I ran real Ninja and CTest against the fix (see the load runs below).
  - The race is forced by a test copy that marks each count and waits 0.5 s. Acceptable: the anchor check guards the substitution, and the marker of the recount (`.stopping.counted`) shows that the rounds reach the new path.
  - All eight presets ran. That is what P9 requires after a CMake change.

## What I checked

### 1. The invariant

Measured with `strace -f -y` on a private FIFO:

- **GNU Make 4.4.1** opens the path at startup, `O_RDONLY` and then `O_WRONLY`, sets `FD_CLOEXEC` on both, keeps its tokens in memory, and closes only when it exits. Recipe children hold no descriptor of the FIFO: I counted 0 in each child. A recursive sub-make opens the path again by name, and its parent holds a token for it.
- **Ninja 1.13.2** opens `O_RDONLY|O_CLOEXEC` and `O_WRONLY|O_CLOEXEC` at startup. It writes back every token before it closes both descriptors at the end.
- **CTest 4.4.5** opens the FIFO once, `O_RDWR` with no close-on-exec, and closes it at the end. Its test processes still do not inherit the descriptor; I listed their fds to check.
  - It writes back `.` for the `+` it read. The manual of GNU Make asks clients to write back the same character. This is harmless here, because the pool counts bytes.
  - A test whose program is missing costs one token (7 reads, 6 writes). This is a known loss.

So for these three clients, a live process that holds tokens holds the FIFO open, as the invariant assumes. Steps 1 to 5 of the plan's argument hold given that assumption, and I found no path by which `balance` writes back a token that a client the count can see is about to return. Where the assumption fails, the client is invisible to the count. These cases remain:

- A holder whose `/proc/<pid>/fd` cannot be read. This is pre-existing (L3).
- A descriptor passed over a socket, or opened through a hard link or another mount namespace's path.
- An open that resolved the old path and is still inside the kernel after the 0.2 s wait (I5).

None of these applies to the clients in use here.

### 2. The window

During the 0.2 s window, the FIFO's path does not exist. Measured with the path missing:

- **`tools/timed`** exits 2 with `timed: refused: no shared job-slot pool (MAKEFLAGS='…').` and the `jobpool status` hint.
- **A bare `ninja`** prints `ninja: error: Could not initialize jobserver: Error opening fifo for reading: No such file or directory`. It still runs, at its own default parallelism, without the pool, and exits 0: 8 edges of 0.3 s each finished in 0.31 s.
- **A bare `ctest -j`** runs without the pool and prints no message at all.
- **GNU Make**, whether it is the top level or a sub-make that opens the path again, prints `cannot open jobserver …` and `jobserver unavailable: using -j1`, then runs serially.

A build already running is not affected, for two reasons. Ninja and CTest never open the path again. And the window opens only after a count found no client, so a holder that was already running prevents it. Only a client that opens in the milliseconds between that count and the move can be affected, and then only through a nested client (a sub-make, or a nested `ninja` or `ctest`).

How often the window opens: the shared pool's journal has 5 repair messages today under `develop`'s tool, so about a few windows per hour. I judge the risk acceptable against the surplus it removes. Before this fix the surplus was up to n, and 3n was seen below.

### 3. What the fix keeps, and whether the test can fail

Each run below used the tip's `jobpool_test.py` against a scratch copy of the tool, on private units:

| Tool | Result |
| --- | --- |
| `develop` | 7 failures: every round, and the load |
| skip the recount | 6 failures: the rounds |
| count the recount on the wrong path | 10 failures |
| no move back after the fix | 4 failures |
| no wait before the recount | **passes** (I5) |
| no `free != n` gate | **passes** (L1) |
| tip, 5 runs | 5 passes, 55.5 to 57.4 s |

The 5 tip runs ran concurrently with the 6 mutant runs. The two ctest runs below also passed it, so the tip passed 7 of 7.

On the fixed tool, the test cannot fail because of timing. A test delayed by more than 0.5 s only makes a round pass vacuously on a broken tool, and there are three rounds.

M1, M2, and the idle stop are kept: `count_fails`, `last_count_fails`, `symlinked`, and `idle_stop` pass, and the wrong-path mutant fails them.

### 4. Real clients on private pools

Each pool had 4 slots and a 1 s poll, and ran for 240 s. Its clients ran in loops with random gaps: one bare `ninja`, one `ninja` through `tools/timed`, and one `ctest -j` through `tools/timed`. A sampler read the free count every 50 ms.

| Tool | Client runs | Most free | Repairs | Path missing | Effects of the window |
| --- | --- | --- | --- | --- | --- |
| tip | 467 | 4 | none | 0 samples | none |
| tip with the slow count | 420 | 4 | none | 30 samples | 3 `timed` refusals with exit 2; 1 bare `ninja` without the pool |
| `develop` | 496 | **8** | 3 × "wrote back 4 of 4", then drained | 0 | |
| `develop` with the slow count | 593 | **13** | 7 write-backs, 8 drains | 0 | |

- Every pool ended with 4 of 4 free.
- Real losses on the tip were repaired within 3 s: a missing test program gave "wrote back 1 of 4", and a `ninja` killed with SIGKILL gave "wrote back 4 of 4".
- The `sleep` that `status` showed as a client of the slow copy is the test copy's own added `sleep`. The tip's tool does not run it.

### 5. Other behavior

`start`, `env`, and `stop` print what they printed before. Of `status`, only the text of the missing-FIFO line changed, and its callers (the math-tools scripts) only log it. The idle stop now also returns the FIFO to its path when its count fails, as before.

### Process

- W9: `328a77b` has the patch-id of `950fad1` on the tests branch and comes before the code it judges, and its message says that it fails until the fix.
- Every commit carries the agent trailer.
- No code comment cites a plan.

### Runs

Fresh trees at `a83b066`, each run through `tools/timed` after `eval "$(tools/jobpool env)"`:

- Configure and build: exit 0 on gcc-debug and on clang-debug.
- `ctest -R '^(tools|tests)\.'`: 45 of 45 pass on each preset, `tools.jobpool` in 56.9 s.
- `format-check` exits 0 on both presets.
- The trace gate exits 0.

## Findings, most severe first

No finding blocks the merge.

- **L1, low, confirmed: the gate that keeps the window to real repairs is untested** (`tools/jobpool:182`).
  - **Mutant.** Dropping `((free != n)) &&` leaves `tools.jobpool` passing.
  - **Failure scenario.** A regression on that line moves the FIFO aside for 0.2 s at every poll that finds no client, that is, every 30 s while the machine is idle. Each window can refuse a `tools/timed` build, or run a bare `ninja` without the pool.
  - **Fix.** Add a test that watches the path of an idle, full pool across three polls and checks that it never disappears.
- **L2, low, confirmed: the service no longer stops itself after its FIFO is deleted by hand** (`tools/jobpool:181`).
  - **Cause.** `free=$(free_slots 3>&-) || continue` skips the idle check.
  - **Behavior on `develop`.** The service exited at the idle check: its `mv` failed under `set -e`.
  - **Probe** on private pools with a 3 s idle time:
    - `develop`'s service had exited (`failed`) within 8 s.
    - The tip's service stays `active` and logs a Python traceback at every poll.
    - `status` keeps saying "the pool is stopping".
    - `ensure` reports "already running".
  - **Fix.** End the service when the FIFO is missing at a poll that finds no client, for example `[[ -p $fifo ]] || exit 1`.
- **L3, low, confirmed, pre-existing: a client whose `/proc/<pid>/fd` cannot be read is not counted** (`tools/jobpool:124`).
  - **Probe.** A same-uid holder that had made itself non-dumpable (`prctl(PR_SET_DUMPABLE, 0)`) counted 0; a dumpable holder counted 1.
  - **Failure scenario.** Such a client's tokens would be written back, which is a surplus.
  - **Why the count cannot fail closed here.** The user's `systemd` is itself unreadable in this way, so treating unreadable processes as clients would leave the pool never repairing itself or stopping.
  - **Fix.** State the assumption in the invariant, which today lists only the fork and close-after-read cases.
- **L4, low, design: the window's effects fall on clients the pool cannot protect.**
  - `tools/timed`'s message, "no shared job-slot pool", is misleading for a 0.2 s gap.
  - Between `tools/timed`'s `-p` check and Ninja's open, a build can still start without the pool.
  - **Change I would make**, in a later unit: `tools/timed` waits up to about 2 s while `<fifo>.stopping` exists, before it refuses.
- **I5, info: the 0.2 s wait is a bound on scheduling, not a proof.**
  - An open that resolves the path before the move and is delayed past the wait could still take tokens during `balance`.
  - The no-wait mutant survives, and no deterministic test seems possible.
  - The idle stop uses 1 s for the same argument. The difference is defensible, because a repair window costs more than an idle one, but it is not stated.
- **I6, info.** Cosmetic issues:
  - A short wrapped line in the header comment of `tools/jobpool:19`.
  - A docstring line over the width at `test/tools/jobpool_test.py:10`.
- **I7, info.** The spike copy `docs/agent/records/spikes/jobpool-generalization/jobpool` (note 0016) keeps its own count-then-balance design, which I did not review. The extraction should check it against this race.

## Outcome

Approve with non-blocking findings.

## Open for the owner

1. **Restart the shared pool after the merge**, so that it runs the fixed tool. Its journal shows the race today ("wrote back 1 of 23" at 12:17:03, drained at 12:17:33). When no agent is building, run: `cd /home/brianm/projects/utils/math-core/galoot && tools/jobpool status && tools/jobpool stop && tools/jobpool start 23`. **Recommended.** The alternative, waiting for an idle stop, may not come on a busy machine.
2. **L1, L2, and L4.**
   - (a) A small follow-up tools unit: the gate test, an exit when the FIFO is missing, and a short wait in `tools/timed`. **Recommended:** each is a few lines and none blocks.
   - (b) Fix L1 and L2 here before the merge, with a delta review. This costs another review session.
3. **L3.**
   - (a) Accept it, and add it to the invariant's list of what is not covered. **Recommended:** no client in use is non-dumpable.
   - (b) Match clients by device and inode through another interface. None exists that avoids the same permission check.
4. **The cost of `tools.jobpool`, about 57 s per preset.** I agree with the authors' recommendation (a): keep it. Its rounds are the only guard of the invariant. Their load phase did not catch the skip-recount mutant in my run, so it alone could be shortened.

## Run rows (scratch log, not committed)

| utc_start | revision | step | preset | selection | wall_s | exit |
| --- | --- | --- | --- | --- | --- | --- |
| 2026-11-08T17:45:24Z | a83b0665d686 | configure | gcc-debug | review:all | 0.307 | 0 |
| 2026-11-08T17:45:25Z | a83b0665d686 | build | gcc-debug | review:all | 31.682 | 0 |
| 2026-11-08T17:45:57Z | a83b0665d686 | test | gcc-debug | review:tools\|tests | 56.924 | 0 |
| 2026-11-08T17:46:54Z | a83b0665d686 | check | gcc-debug | review:format-check | 1.395 | 0 |
| 2026-11-08T17:46:55Z | a83b0665d686 | configure | clang-debug | review:all | 0.364 | 0 |
| 2026-11-08T17:46:55Z | a83b0665d686 | build | clang-debug | review:all | 30.245 | 0 |
| 2026-11-08T17:47:26Z | a83b0665d686 | test | clang-debug | review:tools\|tests | 58.096 | 0 |
| 2026-11-08T17:48:24Z | a83b0665d686 | check | clang-debug | review:format-check | 1.328 | 0 |
| 2026-11-08T17:48:25Z | a83b0665d686 | trace | - | review:trace-gate | 0.222 | 0 |

These ran untimed in the scratch directory: the mutant and repeat runs of `jobpool_test.py` (55 to 100 s each), the four 240 s load runs (whose client runs were timed into per-pool scratch logs), and the `strace` probes.
