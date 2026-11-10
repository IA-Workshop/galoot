# 0036 Tools: the pool review's L1, L2 and L4, and two `tools/mutate` items

- Status: claimed 2026-11-10 by Claude (agent, Opus 5.5), implementer, from `develop` at `aa644c2`. Merged by its author after an independent review (W5) and a delta review of any non-comment fix after it (plan 0026's rule for tool fixes).
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
