# Blind review of plan 0036 (tools: pool L1, L2, L4; `tools/mutate` Clang guards and declared survivals)

The reviewer's record as it returned it. The reviewer has no Write tool, so the implementer, as the merging agent, committed it on `records/review-tools-pool-mutate`, branched from the reviewed revision `963ca6a`. Its findings, checks, and runs are as returned; the implementer shortened the "Files" section to the scratch directory and replaced the reviewer's own cost estimate with the harness's figures.

- **Reviewer:** blind-reviewer role (a general-purpose agent), Claude Opus 5.5, 2026-11-10.
- **Range:** `7715d5a..963ca6a` on `feature/tools-pool-mutate`, eight commits. Reviewed in the detached worktree `/home/brianm/projects/utils/math-core/galoot/.claude/worktrees/review-tools-pool-mutate` at `963ca6a`. The worktree was clean after every run (`git status` empty).
- **Verdict in the brief:** none. The brief listed the author's departures and readings (C20) and no conclusions.

## Outcome

**Block, with one finding to resolve first. The fix is small.**

- The plan's own mutant `mutate_notes_past_next_error` (`tools.toml`) **survives** the tip's suite. So the rule that notes stop at the next error, which decision 0229 item 1 states, is not judged by any test (B1).
- Everything else is non-blocking.

## Findings, most severe first

### B1. The stop of the notes at the next error is untested (blocks; confirmed)

- **Where:** `tools/mutate:369`, `test/tools/mutate_test.py` (`library_guard`).
- **Mutant:** `if END_OF_JOB.match(rest) or "error:" in rest:` becomes `if END_OF_JOB.match(rest):`. This is `tools.toml`'s `mutate_notes_past_next_error`, expected `killed`.
- **What I saw:** it passed `tools.mutate` twice. Once as my own copy against `test/tools/mutate_test.py` (exit 0). Once in my run of a scratch copy of `tools.toml`: "13 killed, 0 equivalent, 1 survived". So the author's pending `tools.toml` record will show this problem too.
- **Failure scenario:**
  - An object's first error is a constant-expression error not caused by a guard, for example an overflow ("constexpr variable 'a' must be initialized by a constant expression", with the note "value … is outside the range").
  - A later error in the same object is a failed guard.
  - With the mutant, the later error's note "non-constexpr function … cannot be used in a constant expression" is read as the first error's note. The object is classed `assert` and the mutant counted killed.
  - The fixture has no object with two such errors, so nothing tells the two apart.
- **Why it blocks:** W3 and AC4 count a mutant written for a new test only when that test kills it. This one is not killed.
- **Fix, either of:**
  - Add a fixture mutant whose one object fails with an overflow first and a guard second, expected `unjudged` / `invalid` on Clang.
  - Or declare the survivor in `tools.toml` with a reason and say so in the record.

### N1. Clang and GCC now disagree on a consteval guard reached from a header (non-blocking; mechanism confirmed, impact on Galoot plausible)

- **Where:** `tools/mutate:376-382`; decision 0229, "Options weighed" ("the two compilers agree").
- **Probe:** a guard inside a consteval function, called as an immediate invocation from a non-constant context in a header (a template function body). Files `$S/cx/h.hpp`, `d2.cpp`, `d3.cpp` (`$S` is my scratch directory, listed at the end).
  - Clang's first error is `h.hpp:4: error: call to consteval function 'cguard' is not a constant expression`, with the note "non-constexpr function 'reject' cannot be used in a constant expression". It is now class `assert`, so the mutant is killed.
  - GCC's first error is `h.hpp:4: error: call to consteval function 'cguard(9)' is not a constant expression`. The call to the non-constexpr function only comes as the second error. It stays class `invalid`, so the mutant is unjudged.
  - Before this change both compilers gave unjudged. The change creates a new disagreement, the reverse of N6.
- **Why it may matter here:** Galoot has about 44 consteval functions (`make_product_kernel`, `R()`, `conformal_change`, …).
- **Other contexts, where the compilers agree:** non-type template argument, a direct consteval call from a test, `static_assert`, `if constexpr`, an enum value, a case label, a `static constexpr` member, a constexpr variable in a header. In each the result is `assert` on both, or `test` vs `assert` (both kills). `constinit` is not `assert` on either.
- **Recommendation:** either give the GCC rule the counterpart (a first error "call to consteval function … is not a constant expression" followed, in the same object, by "call to non-constexpr function"), or record this case as a known limit in decision 0229.

### N2. Two conditions of the new classifier rule are untested (non-blocking, low; confirmed)

- **Conjunct dropped:** removing `CONSTANT_EXPRESSION.search(message) and` (`tools/mutate:380`), so any first error whose notes name a non-constexpr call counts as `assert`, survives `tools.mutate`.
- **Object-end stop dropped:** removing the `END_OF_JOB` stop of the inner notes loop (`tools/mutate:369`) also survives.
- Both conditions are defensive and I found no present failure from either. But the decision states them, and no test can tell them apart from their absence.

### N3. The `tools/timed` wait does nothing for a MAKEFLAGS path through a symbolic link (non-blocking, low; confirmed)

- **Where:** `tools/timed:68-72`.
- **Probe:** MAKEFLAGS named `link.fifo -> real.fifo`. I moved `real.fifo` to `real.fifo.stopping` and moved it back after 0.5 s. The step was refused at once (0.003 s) with "no shared job-slot pool", because `link.fifo.stopping` never exists.
- **Reach:** `tools/jobpool env` always prints the resolved path, so only a MAKEFLAGS written by hand is affected. A symlinked directory in the path is fine.
- **Odd characters:** a path with `*`, `'` and `[1]` waited and ran correctly.
- **Possible fix:** resolve the path with `realpath -m` before the wait, as `tools/jobpool` does.

### N4. The service also ends under live clients of a deleted FIFO (non-blocking, info; confirmed)

- **Where:** `tools/jobpool:180-187`; the message of `bb0ad20` ("missing at a poll with no client").
- **Cause:** a client's descriptor of an unlinked FIFO reads `<path> (deleted)`, so `count_clients` cannot see it.
- **Probe:** I started a private pool (unit `galoot-review-<pid>-delclient`, poll 1 s), had a client hold the FIFO and take a token, then unlinked the FIFO. Within 4 s the unit was `failed` while the client still held the old pipe. The journal shows "… is gone, so the pool stops". I stopped and reset that unit myself.
- **Consequence:** the same as the documented restart under running builds. The old clients keep the old pipe and `ensure` serves new builds from a new one, up to twice the slots.
- **Suggestion:** say "no client it can see" in the comment.

### N5. Loose or missing checks (non-blocking, low; confirmed by mutants)

- **Exit status not judged:** `exit 1` to `exit 0` in `tools/jobpool:186` survives `tools.jobpool`. Nothing checks the exit status or that the unit is `failed`.
- **Wait bound wide:** a 4 s wait (`{1..40}`) survives `timed_pool`, whose upper bound is 5.0 s. The plan says "about 2 s". A 1.4 s wait is killed.
- **Refusals judged by exit code only:** `refusals` checks only exit 2. `survived_without_reason` and `survived_with_killers` also pass against develop's tool, which refuses every `survived` spec. The contrast with `declared_survivals`, which accepts the same spec with a reason, does pin the cause. Checking the refusal text would be stronger.

### N6. `assert` now also covers a non-check on Clang, as it already did on GCC (info)

- A mutant that drops `constexpr` from a function, or calls a non-constexpr function on a compile-time path, is now `assert` (a kill) on both compilers. Probe c8: Clang "must be initialized by a constant expression" with the note naming `plain`; GCC "call to non-'constexpr' function".
- Under C++20 (not Galoot's C++23), Clang's default-error "constexpr function never produces a constant expression [-Winvalid-constexpr]" with the same note would also be `assert`, with no test involved. Under Galoot's C++23 flags it is not emitted (probed).
- This is consistent with decision 0214 item 5. Decision 0229 could name it.

### N7. Cosmetic

- The "rewrap" in `e08f3f4` moved the overlong docstring line rather than fixing it. `test/tools/jobpool_test.py:13` is now 84 characters.
- STYLE sets no Python column rule, so this is not a finding against a rule.

## Acceptance criteria of plan 0036

1. **Tests first:** met.
   - `tools.jobpool` against develop's `tools/jobpool` fails exactly the three checks its message names. `idle_full` passes there, as the message says.
   - `tools.measure_record` against develop's `tools/timed` fails the five checks named in `21b52f6`, plus the one of `3d6e68c`.
   - `tools.mutate` against develop's `tools/mutate` fails exactly the listed checks: `guard_kill`'s Clang class, the four `table_guard_kill` checks, guard problem lines, every `declared_survivals` check, and "expected survivor alone: exit".
   - `idle_full` watches across about 4 polls (4.5 s at 1 s) without opening the FIFO. My gate-dropped mutant gave 130 samples missing or aside.
   - `fifo_deleted` checks inactive within 10 s, `status` "not running" with exit 1, and `ensure` "started: 23 slots".
   - `timed_pool` covers all four cases. The fixture gains the header table, with `table_guard_kill` (`assert` on both compilers) and `table_overflow` (`invalid`, unjudged, on both).
   - The changed expected value of `guard_kill`'s Clang row is listed in `3c0ffe7` (W9).
   - Survivals: exit 0 alone; "expected to survive but killed" for a kill; refused without a reason or with killers (exit code only, N5).
2. **Fixes in their own commits after the tests:** met (`bb0ad20`, `11469a4`, `9c93209`).
3. **Documentation and decision:** met. The header comments and usage text of all three tools are updated, and decision 0229 is recorded (C18). No decision, plan, review, or provision citations in tools or tests: I scanned the added lines, and `tests.citations` passes.
4. **Mutants:** not met as it stands.
   - `tools.toml` from a scratch copy on gcc-debug: 13 of 14 killed; `mutate_notes_past_next_error` survived (B1).
   - Gap claims confirmed independently:
     - `jobpool_gate_removed` survives develop's `jobpool_test.py` and is killed by the tip's (`idle_full`).
     - `timed_missing_fifo_allowed` survives develop's `measure_record_test.py` and is killed by the tip's (`timed_pool`).
   - I did not run `gap.toml` itself (departure 7). Both `tools.toml` and `gap.toml` pass `--check`.
5. **Review:** this record.
6. **P9 at the final tip (all eight presets, lint, `runlog check`):** not checkable here and not asked of me. I ran the P8 selection only (below).

## Other checks

- **Existing specs:** all 31 existing specs under `docs/agent/records/mutations/` load to identical structures under develop's and the tip's `load_spec`. Only the new `gap.toml` differs: develop refuses it.
- **Timing and flakes:**
  - The lower bounds of `timed_pool` are deterministic: at least 20 sleeps of 0.1 s, and the FIFO comes back after a 0.5 s timer.
  - The upper bounds are wide (< 1 s refusal, < 5 s wait). Sleeping 0.1 s twenty times costs about 2.0–2.3 s even on a loaded or efficiency core.
  - `idle_full` cannot flake on a correct tool: nothing moves the FIFO of a full pool.
  - `fifo_deleted` allows 10 s for a 1 s poll. A client count over /proc took 0.016 s here, with about 600 processes.
  - The pool's real windows are about 0.25 s (repair) and about 1.05 s (idle check), well inside 2 s.
- **jobpool, self-race:** the `-p` check runs synchronously after the service's own moves, so the service cannot end itself during its own `mv`.
- **jobpool, idle stop with the timed wait:** the service removes `.stopping` and exits. The wait then ends early and the step is refused with "no shared job-slot pool", which is correct.
- **Process:**
  - W2/W9: three labelled failing test commits, then the fixes, then the late test commit `3d6e68c` (departure 6), which passes at its own revision.
  - Test commits touch only `test/`; fix commits touch only `tools/`.
  - W7: every message has "Agent work: Claude (Opus 5.5), implementer." and a `Co-Authored-By` line naming the model.
  - Cherry-picks carry `-x` lines.

## Not checked

- `gap.toml` runs, and the author's in-progress records (departure 7).
- Presets other than gcc-debug and clang-debug, lint, `tools/runlog check`, and the full suite.
- The run-log rows the author wrote with `--log` (departure 9).
- The renumbering risk of decision 0229 (departure 8).
- STYLE for shell, which has no explicit rule.
- Clang or GCC versions other than clang 23.1.4 and g++ 16.2.1.

## Runs

My checks at `963ca6a`:

| Check | gcc-debug | clang-debug |
| --- | --- | --- |
| P8 selection, `ctest -L '^tools\.(jobpool\|measure\|mutate)$'` | 4/4 passed, 91.1 s | 4/4 passed, 84.8 s |
| `ctest -R '^tests\.'` (includes `tests.citations`) | 6/6 passed | 6/6 passed |
| `format-check` | passed, 1.7 s | — |

- Trace gate exit 0 (0.25 s). It reports "items with no test: 6.1, 6.2, 6.3, 6.4"; I did not compare that line with develop.

All rows are in `$S/runs.tsv` (not committed):

```
utc_start step preset selection wall_s exit
10:31:05 configure gcc-debug - 0.308 0
10:31:06 configure clang-debug - 0.366 0
10:31:16 test gcc-debug tools.(jobpool|measure|mutate) 91.131 0
10:32:48 test clang-debug tools.(jobpool|measure|mutate) 84.798 0
10:34:50 test - probe-measure-vs-dev 1.763 1
10:34:50 test - probe-mutate-vs-dev 16.268 1
10:34:50 test - probe-jobpool-vs-dev 72.095 1
10:37:08 test - probe-T1-tip (wait 4 s) 6.247 0
10:37:14 test - probe-T2-tip (wait 1.4 s) 3.379 1
10:37:17 test - probe-T6-tip (gap) 3.884 1
10:37:21 test - probe-T6-dev (gap, old suite) 1.313 0
10:37:08 test - probe-M1 (no object-end stop) 18.939 0
10:37:08 test - probe-M2 (no "constant expression" conjunct) 18.867 0
10:37:08 test - probe-M4 (= mutate_notes_past_next_error) 19.192 0
10:37:08 test - probe-J1 (exit 0) 61.887 0
10:37:08 test - probe-J3-tip (gate dropped) 60.925 1
10:37:08 test - probe-J3-dev (gate dropped, old suite) 54.435 0
10:37:49 test gcc-debug tests. 1.002 0
10:37:50 test clang-debug tests. 1.006 0
10:37:51 build gcc-debug format-check 1.685 0
10:37:53 check - trace 0.248 0
10:38:52 check - spec-load-compare 0.085 0
10:38:52 check gcc-debug mutate-check-tools 0.087 0
10:38:53 check gcc-debug mutate-check-gap 0.071 0
10:39:08 build - oddchar-probe 0.053 0
10:40:54 mutate gcc-debug tools.toml-reviewer 376.672 1
  (+ 17 mutant:* and baseline rows written by the driver; exit 8 = killed for 13; mutate_notes_past_next_error exit 0)
  (+ 33 "probe cx-*" rows: single clang++/g++ -fsyntax-only diagnostic probes)
```

The symlink probe was refused before running, so it has no row.

## Shared pool status readings

All readings were `running … at /run/user/1000/jobserver.fifo, stops itself after 30 minutes`. I never started, stopped, ensured, or opened it. My only private pool was `galoot-review-<pid>-delclient`, which I stopped and reset myself.

| When | Slots free | Clients |
| --- | --- | --- |
| Before P8 | 23 of 23 | 1022553 (ctest) |
| After P8 | 23 of 23 | 1045122, 1048636 (ctest) |
| Before probes 1 | 23 of 23 | 1055968 (ctest), 1056242 (ninja) |
| After probes 1 | 23 of 23 | none |
| Before probes 2 | 16 of 23 | 1069004 (ninja), another session's build |
| After probes 2 | 23 of 23 | none |
| Before the mutate run | 23 of 23 | none |
| After the mutate run | 22 of 23 | 1156263 (ninja) |
| Final | 22 of 23 | 1166755 (ctest), 1175204 (ninja) |

## Files

`$S` is `/tmp/claude-1000/-home-brianm-projects-utils-math-core/fd84b452-4222-4a56-be70-661be7f37273/scratchpad/review` (scratch, not kept).

## Cost

- Token count: not visible to the reviewer; the harness reported 181,767 subagent tokens, 63 tool calls, and 1,081 s (18 min) to the implementer.
