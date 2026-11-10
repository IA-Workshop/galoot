# 0036 Tools: the pool review's L1, L2 and L4, and two `tools/mutate` items

- Status: done 2026-11-10 (outcome below). Claimed 2026-11-10 by Claude (agent, Opus 5.5), implementer, from `develop` at `aa644c2`. Merged by its author after an independent review (W5) and a delta review of any non-comment fix after it (plan 0026's rule for tool fixes).
- Source of scope: decision 0228 item 9 (L1, L2 and L4 of `records/reviews/2026-11-08-pool-surplus-review.md`), decisions 0223 item 5 and 0225 item 5 (`tools/mutate`), the M5 audit's table of ruled work still to do.
- Branches, from `develop` after this claim (W7): `feature/tools-pool-mutate`, worktree `.claude/worktrees/tools-pool-mutate`; the test commits on `feature/tools-pool-mutate--tests`, worktree `.claude/worktrees/tools-pool-mutate--tests`, cherry-picked with `-x`.
- Record location: `records/mutations/0036-tools-pool-mutate/`, free on every branch at this commit.
- The shared pool is read only (status before and after heavy runs) and is never stopped or restarted. Every pool a test starts is private, under its own unit name with its FIFO in the build tree, as `tools.jobpool` already does.

## Goal

1. **L1.** A test that fails when the `free != n` gate of the service's repair is dropped: an idle, full pool must never move its FIFO aside.
2. **L2.** The service ends, so that `status` says "not running" and `ensure` starts a new pool, when its FIFO is missing at a poll that finds no client (deleted by hand). Today it stays active and logs a traceback at every poll.
3. **L4.** `tools/timed` waits up to about 2 s while `<fifo>.stopping` exists before it refuses, so that the pool's 0.2 s repair window and 1 s idle check do not refuse a build.
4. **`tools/mutate`, Clang guards.** Clang reports a guard that stops constant evaluation inside a `static constexpr` variable of a header as "constexpr variable '…' must be initialized by a constant expression", with a note "non-constexpr function '…' cannot be used in a constant expression". The driver classes that `invalid`, so the mutant is unjudged where GCC calls it killed (WU12 blind review N6). Such an error is classed `assert`, as GCC's "call to non-constexpr function" already is (decision 0214 item 5). A Clang constant-expression error whose notes name no such call (an overflow, say) stays as it is.
5. **`tools/mutate`, expected survivals.** A mutant may declare `expect = "survived"` with a `reason`: a gap mutant run against the suite before its tests (W3), or a form one compiler accepts. Its survival is then no problem, and a kill is. Existing specs (`killed`, `equivalent`) keep their meaning and their outputs.

## Definition of done and acceptance criteria

1. Tests first (W2, W9): on the tests branch, one commit per tool, each failing against `develop`'s tool with the checks it names in its message, and passing after the fix.
   - `tools.jobpool`: case `idle_full` watches the FIFO path of a private, idle, full pool across at least three polls and fails if the path is ever missing or `.stopping` ever exists; case `fifo_deleted` deletes a private pool's FIFO and expects the unit inactive within a few polls, `status` to say "not running", and `ensure` to start a new pool.
   - `tools.measure_record` (which already copies and tests `tools/timed`): with MAKEFLAGS naming a FIFO, a build step runs when the FIFO exists; is refused at once (well under 1 s) when neither the FIFO nor `<fifo>.stopping` exists; runs when `<fifo>.stopping` is moved back within the wait; and is refused after about 2 s when `<fifo>.stopping` stays.
   - `tools.mutate`: the fixture gains a header guard reached only through a `static constexpr` variable of the header, and a mutant that trips it (killed, class `assert`, on both compilers) and one that overflows there (unjudged, not `assert`, on both); the Clang row of the existing `guard_kill` changes from class `test` to `assert` (a changed expected value, listed for the review, W9); specs with `expect = "survived"` give exit 0 for a survivor, a problem for a kill, a refusal without a reason or with `killers`.
2. The fixes, each in its own commit after the tests.
3. Documentation in the tools' own header comments and usage text; decision record of the two `tools/mutate` rule changes (C18).
4. Mutants, `records/mutations/0036-tools-pool-mutate/tools.toml`, judged by the tools' own CTests on gcc-debug with selections that build nothing (the pattern of `records/mutations/0026-tools/`). The new tests close a gap in `tools.jobpool`; a mutant counts for the gap when the suite without the new tests lets it survive and the suite with them kills it, and a mutant written for a new test counts when that test is among those it fails. The gap mutant is the dropped `free != n` gate, run at the tip with `test/tools/jobpool_test.py` as at the branch point (expected: survived, which the new `expect = "survived"` states) and as at the tip (expected: killed). The other mutants (the missing-FIFO exit dropped, the wait dropped, the wait made unconditional, the Clang note rule dropped, the rule widened to every constant-expression error, an expected survival reported as a problem, a kill of an expected survivor not reported) are written for new tests and run at the tip only.
5. Review: a blind review by a separate Opus agent in the blind-reviewer role, with no verdict in its brief; its findings resolved; a delta review after any non-comment fix.
6. P9 at the final tip from empty trees: all eight presets (each its full suite; the CMake comments of the tests change), lint, `format-check`, the trace gate, `tools/runlog check`, each through `tools/timed` as `gate:<selection>`. Merged into `develop` with a P13 line.

Not in scope: L3 (accepted, decision 0228 item 9), I5 to I7 of the pool review, per-preset expectations in a mutation spec.

Agent cost estimate: one implementer session (about 300k to 500k tokens) and two review sessions (about 150k each).

## Outcome (2026-11-10)

Done. All criteria met, with the departures below.

### Commits

| Commit | What | Kind (W9) |
| --- | --- | --- |
| `e08f3f4` | `tools.jobpool`: `idle_full` (an idle, full pool never moves its FIFO) and `fifo_deleted`; fails until `bb0ad20`, as its message says | test, before the code |
| `21b52f6` | `tools.measure_record`: `timed_pool`, the pool check and wait of `tools/timed`; fails until `11469a4` | test, before the code |
| `3c0ffe7` | `tools.mutate`: the header table of the fixture, `table_guard_kill`, `table_overflow`, `declared_survivals`, three refusals; `guard_kill`'s Clang class `test` becomes `assert` (a changed expected value, listed in the message); fails until `9c93209` | test, before the code |
| `bb0ad20` | `tools/jobpool`: the service exits 1 when its FIFO is missing at a poll with no client (L2) | code |
| `11469a4` | `tools/timed`: waits up to 2 s while `<fifo>.stopping` exists, with its own refusal after (L4) | code |
| `9c93209` | `tools/mutate`: Clang's guard note makes a first error `assert`; `expect = "survived"` | code |
| `3d6e68c` | `tools.measure_record`: the refusal names the FIFO left aside | test, after the code (departure 3) |
| `963ca6a` | decision (now 0231) and the mutation specs | record |
| `57d4319`, `cd01fcb` | the review's B1 and N5: `overflow_then_guard`, the unit's failed state (then waited for, after a race seen once), a 3.5 s bound, refusal texts, a docstring wrap | test |
| `053179d` | `tools/jobpool`: comment, clients of a removed FIFO are not seen (N4) | comment |
| `35754cb` | `tools/mutate`: the first error and its notes read from the object's own output; the "constant expression" condition dropped (B1, N2) | code |
| `6f373e7`, `aa8ba63` | the decision corrected twice, the spec and the mutation records | record |
| `5bae5db`, `004fcbc` | the blind and delta review records, cherry-picked with `-x` from `records/review-tools-pool-mutate` (`9a4b6b8`) and `records/review-tools-pool-mutate-delta` | record |
| `53b2ae3` and the next | the decision renumbered 0230, since 0229 on `develop` is the owner's M5 follow-up rulings, then 0231, since 0230 on `develop` is the snippets cleanup rulings | record |

### Evidence

- **Tests first.** Each of the three first test commits failed against `develop`'s tool with exactly the checks its message names (runs `tests-first:*`; the blind review reproduced each), and passes after its fix.
- **Mutants** (`records/mutations/0036-tools-pool-mutate/`): `tools.toml` at `6f373e7`, 16 of 16 killed. The gap run, `gap.toml` in a detached commit with the two tests as at `7715d5a`: `jobpool_gate_removed` and `timed_missing_fifo_allowed` both survive, as declared, so both count for the gap. The earlier run at `963ca6a` had 13 of 14 killed; its survivor was the review's B1.
- **Reviews.** Blind review (Claude Opus 5.5, blind-reviewer role) at `963ca6a`: block on B1, `records/reviews/2026-11-10-tools-pool-mutate-blind-review.md`. Delta review (Claude Opus 5.5) at `6f373e7`: approve with non-blocking findings, `records/reviews/2026-11-10-tools-pool-mutate-delta-review.md`. The fixes after the delta review are records only (the decision's text), so no further delta review.
- **P9**, from empty trees, every run through `tools/timed` as `gate:<selection>`, all exit 0:

| Check | at `a592dcd` (before merging `develop`) | at `b8ccbda` (merged) |
| --- | --- | --- |
| gcc-debug | 540 tests, 100.6 s | 544 tests, 100.0 s |
| gcc-release | 540, 123.7 s | 544, 113.5 s |
| gcc-asan | 532, 291.4 s | 536, 221.1 s |
| clang-debug | 540, 95.7 s | 544, 97.1 s |
| clang-release | 540, 115.3 s | 544, 122.0 s |
| clang-asan | 540, 200.5 s | 544, 202.3 s |
| lint | 540, 255.2 s | 544, 252.4 s |
| clang-measure | 532, 114.5 s | 536, 117.1 s |
| format-check, trace gate, `tools/runlog check` | 1.6 s, 0.3 s, 2.6 s | 1.8 s, 0.4 s, 2.7 s |

The four more tests at `b8ccbda` came with `develop`'s plan 0035. `develop` then gained plan 0037's merge (README and snippets), so the matrix ran a third time on the merged tree at `3d64842`, all exit 0: 544 tests on six presets and 536 on gcc-asan and clang-measure (gcc-debug 98.1 s, gcc-release 112.1 s, gcc-asan 219.4 s, clang-debug 93.7 s, clang-release 114.4 s, clang-asan 195.1 s, lint 258.5 s, clang-measure 116.0 s; format-check 1.7 s, trace gate 0.3 s, `tools/runlog check` 2.5 s). After it, `develop` gained only decision 0230, a records file; on that last merge the `tests.*` selection (with the citation check), the trace gate, and `tools/runlog check` ran. The shared pool read 23 of 23 free before and after both matrices. It was not running when the unit started; it was started once with `tools/jobpool ensure` after `status` said "not running", and never stopped.

### Resolution of the review findings

| Finding | Resolution |
| --- | --- |
| B1 (blocks): the stop of the notes at the next error untested | Fixed: `overflow_then_guard` (`57d4319`) kills the mutant on clang-debug; confirmed by the delta review and by `tools.toml` |
| N1: a guard in a `consteval` function from a header is `assert` on Clang, unjudged on GCC | Accepted as a known limit in decision 0231; open item 2 |
| N2: the "constant expression" condition and the inner end-of-job stop untested | The condition dropped (`35754cb`); the stop now cuts the whole object's output, still untested (D2) |
| N3: no wait for a MAKEFLAGS path through a symbolic link | Not fixed; open item 1 |
| N4: the service ends under clients of a removed FIFO | Comment (`053179d`) |
| N5: exit status, wait bound, refusal texts not judged | Fixed (`57d4319`, `cd01fcb`); mutants `jobpool_missing_fifo_exit_zero` and `timed_wait_long` killed |
| N6: a non-check call is `assert` on both compilers | Stated in decision 0231 as a known limit shared with decision 0214 |
| N7: docstring line over the width | Fixed (`57d4319`) |
| D1: the reason given for dropping the condition is false (`constinit`, `alignas`) | Decision 0231 corrected (`aa8ba63`); `constinit` added as a known limit. The message of `35754cb` keeps the false reason (history is not rewritten) |
| D2: the end-of-job cut untested (predates the unit) | Recorded in the mutation README; open item 3 |
| D3: "the next error" not pinned beyond two errors | Recorded in the mutation README; open item 3 |
| D4: `output` rebound inside the loop of `build_failures` | Not changed: a rename of code after the delta review would need a third review for no change in behavior; open item 3 |
| D5: `57d4319` holds the racy failed-state check | Noted here; fixed by `cd01fcb` |
| D6: the plan's mutant "the rule widened to every constant-expression error" replaced | `mutate_every_error_with_notes_a_guard` takes its place, since the condition is gone |

### Departures (C20)

1. The test of the `tools/timed` wait is a case of `tools.measure_record`, which already runs `tools/timed` in a scratch repository, not of `tools.jobpool`; it uses a FIFO in its scratch directory, no pool service.
2. `tools/timed` gives its own refusal when `<fifo>.stopping` still exists after the wait, beyond the ruled wait.
3. Test commits came after code commits three times (`3d6e68c`, `57d4319`, `cd01fcb`). Each passes at its own revision and judges mutants that earlier tests let survive; none changes an expected value.
4. The gap run used a detached commit (`46e6c55`), on no branch and not kept; its rows are in the run log.
5. The "constant expression" condition of the plan's goal 4 was dropped after the review; the class now follows the note alone.
6. The decision was numbered 0229 on the branch and renumbered 0230 and then 0231 at the merge, as `develop` took both numbers first; the review records and the earlier messages say 0229.
7. Test-first runs in the tests worktree wrote their rows to the feature worktree's run log with `tools/timed --log`.
8. The shared pool's service keeps running the tool it was started with (`develop`'s, before this unit) until it next stops; the missing-FIFO stop reaches it only then. No restart, as decision 0228 item 9 rules.

### Open items for the owner

1. **N3, a MAKEFLAGS FIFO path through a symbolic link.** `tools/timed` looks for `<path>.stopping` by the path as written, so through a link it never waits. Only a hand-written MAKEFLAGS is affected; `tools/jobpool env` prints the resolved path. Options: (a) leave it (recommended: no present user); (b) resolve the path with `realpath -m` first, with a test case, in a later tools unit.
2. **N1 and the `constinit` case: GCC unjudged where Clang kills**, for a guard reached through a `consteval` call or a `constinit` variable in a header, where GCC names the guard only in a second error. Options: (a) leave it as the recorded limit and read the Clang row when a spec meets it (recommended until a spec does); (b) extend the GCC rule to read the second error, in a later tools unit with its own fixture cases.
3. **D2 to D4, `tools/mutate`'s untested cuts and a variable name.** Options: (a) fold into the next unit that touches `tools/mutate`: a fixture job that fails without an `error:` line, an object with three errors, and the rename (recommended); (b) a unit of their own now.
