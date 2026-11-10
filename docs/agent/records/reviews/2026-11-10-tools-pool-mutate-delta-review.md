# Delta review of plan 0036: `963ca6a..6f373e7`

The reviewer's record as it returned it. The reviewer has no Write tool, so the implementer, as the merging agent, committed it on `records/review-tools-pool-mutate-delta`, branched from the reviewed revision `6f373e7`. Its findings, checks, and runs are as returned; the implementer added the harness's cost figures.

**Outcome: approve, with non-blocking findings.** B1 is resolved. The restructured classifier behaves exactly like the old one. Dropping the conjunct is acceptable, but the reason recorded for dropping it is false (D1). Half of N2 is still open (D2).

- **Reviewer:** blind-reviewer role, run by a general-purpose agent, Claude Opus 5.5, 2026-11-10. Range `git diff 963ca6a 6f373e7`, 5 commits. Worktree `.claude/worktrees/review-tools-pool-mutate-delta` at 6f373e7, left clean (`git status` empty).
- **Did the brief carry a verdict?** No. It carried the author's departures and readings (C20) and the first review's findings, and said nothing on whether the delta is correct.

## Findings, most severe first (none blocks)

**D1. The recorded reason for dropping the "constant expression" conjunct is false.** Confirmed by probe.
- **Where:** `docs/agent/decisions/0229-mutate-clang-guards-and-expected-survivals.md:6` and `:24`; behavior at `tools/mutate:377-378`. Commit 35754cb claims the same.
- **The claim:** the conjunct "could not change a class" and "no test could tell it from its absence".
- **Counter-examples, Clang 23.1.4:** two errors carry the guard note but do not contain "constant expression":
  - constinit: "variable does not have a constant initializer"
  - alignas: "'aligned' attribute requires integer constant"
  - Both were non-assert at 963ca6a and are `assert` at 6f373e7.
- **GCC 16.2.1, alignas:** the first error is "call to non-'constexpr' function", so the drop brings the compilers into agreement here.
- **GCC 16.2.1, constinit in a header:** the first error is "'constinit' variable 'c' does not have a constant initializer" and the guard is a second error. GCC stays `invalid` (unjudged) while Clang is now `assert` (killed). Before the drop, both were `invalid`. This is a new disagreement with the same shape as N1's consteval case, and it is not listed among the known limits.
- **In a test file:** both compilers kill (`test` on GCC, `assert` on Clang), so verdicts agree there.
- **Present impact:** none. There is no `constinit` in `include/` or `test/`, and the only `alignas` (`test/core/mv_test.cpp:151`) takes a type.
- **Other contexts probed:** case, enum, bit-field, if constexpr, explicit, noexcept, requires, consteval, SFINAE, VLA. Each either contains "constant expression" or has no guard note after the error, so the class does not change there.
- **Fix:** correct the "Corrected" line and the "untestable" sentence in 0229, and add constinit as a known limit beside consteval. Optionally, add a constinit fixture.

**D2. The end-of-job stop at `tools/mutate:362` is still not judged by any test.** Confirmed.
- **The test:** I removed the `END_OF_JOB` break (mutant m2). It survives `test/tools/mutate_test.py` at 6f373e7, and also at 963ca6a with the old code's outer break removed, so the gap predates the delta.
- **Why it matters:** N2's second part was answered as "disappeared with the restructuring". The stop did not disappear. It moved, and it is now the only thing that cuts both the first error and its notes.
- **Scenario:** a failed job whose output has no `error:` line, followed by another failed job. The first object then takes the next object's first error as its own. If that error is in `test/`, the kill is credited to the wrong object.
- **Likelihood:** low, since nearly every failure prints `error:` (compiler, collect2, lld).
- **Fix:** add a fixture job that fails without an error line, or record this as a known gap.

**D3. The bound "up to its next error" is not pinned beyond two errors.** Confirmed, minor.
- At `tools/mutate:369`, changing `errors[1]` to `errors[-1]` (mutant m3) survives, because no fixture object has three or more errors.

**D4. Nit: `tools/mutate:360` rebinds the parameter `output` inside the loop.**
- It is harmless today because `lines` is taken at `:350`. A later edit that uses `output` after the loop would silently read only the last object's lines. Renaming it (for example `job`) avoids that.

**D5. Process: recorded step 57d4319 contains the racy version of the "unit failed" check.**
- That version can fail while the unit is still deactivating (`ExecStopPost` runs then). cd01fcb fixes it, and the author saw the failure once.
- This is a W2 blemish only. History must not be rewritten; noting it is enough.

**D6. Observation: one mutant in plan 0036 DoD 4 has been replaced.**
- The plan names "the rule widened to every constant-expression error". With the conjunct gone, the spec has `mutate_every_error_with_notes_a_guard` in its place. That is reasonable, and the plan's outcome should say so.

## Status of each first-review finding

| Finding | Status | Evidence |
| --- | --- | --- |
| B1 | Resolved | See below. |
| N1 | Accepted as a recorded limit | consteval probe on both compilers matches the 0229 text exactly. |
| N2 | Half resolved | Conjunct: dropped; the behavior is acceptable but the reason is wrong (D1). End-of-job stop: not resolved (D2). |
| N3 | Not resolved | `tools/timed` is unchanged in the delta; the author will report it to the owner as an open item. |
| N4 | Resolved | Comment-only change in `tools/jobpool`, and the comment is accurate. |
| N5 | Resolved | See below. |
| N6 | Accepted as a recorded limit | Dropping `constexpr` gives `assert` on both compilers, as 0229 says. |
| N7 | Resolved | No line added in the delta is over 79 characters. |

**B1 detail.**
- The notes-past-next-error mutant (m1) of the new code is killed at 6f373e7. Four checks fail on `guard clang-debug overflow_then_guard`: verdict killed instead of unjudged, the killer, the class, and the problem lines.
- The same mutant survives the 963ca6a tests (exit 0).
- The kill rests on the Clang row only. On GCC the notes play no part, which is expected.
- **Equivalence of the restructure:** I ran a differential of `build_failures` at 963ca6a against 6f373e7 on 600k random Ninja-like outputs. These mixed several failing objects, objects with no error line, "In file included from" lines, progress lines and source echoes.
  - With the old conjunct also removed, there were 0 differences.
  - With the conjunct kept, the only differences were errors lacking "constant expression".
  - On real output, all `tools.mutate` fixture cases pass on both compilers.

**N5 detail.**
- The `is-failed` wait is correct: the unit is started without `--collect`, so the failed state persists until `reset-failed`.
- The three refusal strings exist in `tools/mutate:185-193`.
- I did not run the spec's `jobpool_missing_fifo_exit_zero` and `timed_wait_long` mutants myself. By reading the code, both are killed (exit 0 leaves the unit inactive, so the 10 s wait times out; 4 s is over the 3.5 s bound).

## Can the new tests flake?

Low risk.
- **Runs:** `tools.jobpool` passed 5 of 5 times (2 under ctest, 1 at cd01fcb with the 963ca6a tools, 2 repeats). `tools.measure_record` passed 6 of 6 times, one of them under `taskset -c 31` (an efficiency core) while the shared pool was busy.
- **The `is-failed` wait:** 10 s, against a 1 s poll.
- **The timed wait:** 20 × `sleep 0.1` plus fork overhead against the 3.5 s bound, which leaves about 1.5 s of headroom. A plausible risk only under extreme oversubscription.

## Process checks

- **W9:** the test commits 57d4319 and cd01fcb are cherry-picked with `-x` and come before the fixes. Verified: the tests at cd01fcb pass against the 963ca6a tools (`tools.mutate`, `tools.measure_record`, `tools.jobpool` all exit 0), as their messages claim.
- **W7:** all five commits carry "Agent work: Claude (Opus 5.5), implementer." and a Co-Authored-By line naming Claude Opus 5.5.
- **Decision 0034:** no citations in the changed tool comments, usage text, test docstrings and comments, or fixture comments. The spec in `records/` cites plan 0036, which records may do.
- **C18 (decision corrected in place before its merge):** acceptable. It follows the precedent of 0213, 0214 and 0217, it is still on the feature branch, and its "Corrected" line says what changed. Only its reason is wrong (D1).

## What I did not check

- Did not run `records/mutations/0036-tools-pool-mutate/tools.toml` (the author is running it), the eight-preset full suite, lint, or `tools/runlog check`; those belong to the author's P9 merge step.
- The usability checklist does not apply: no public header changed.

## Run rows

TIMED_LOG is `/tmp/claude-1000/-home-brianm-projects-utils-math-core/fd84b452-4222-4a56-be70-661be7f37273/scratchpad/delta-review/runs.tsv` (scratch, not committed). All rows have revision 6f373e7918cb and cpus 0-31 (`timed` records its own affinity, so the taskset row also shows 0-31).

| utc_start | step | preset | selection | wall s | exit |
| --- | --- | --- | --- | --- | --- |
| 10:57:01 | configure | gcc-debug | all | 0.385 | 0 |
| 10:57:01 | configure | clang-debug | all | 0.458 | 0 |
| 10:57:02 | test | gcc-debug | `^tools\.(jobpool\|measure\|mutate)$` (4/4 passed) | 63.643 | 0 |
| 10:58:05 | test | clang-debug | `^tools\.(jobpool\|measure\|mutate)$` (4/4 passed) | 62.873 | 0 |
| 11:00:03 | mutant | - | `tools.mutate:m1_notes_past_next` (killed) | 17.772 | 1 |
| 11:00:21 | mutant | - | `m2_no_end_of_job_break` (survived) | 18.164 | 0 |
| 11:00:39 | mutant | - | `m3_last_error_bound` (survived) | 17.588 | 0 |
| 11:00:57 | mutant | - | `m4_notes_include_error` (equivalent) | 18.005 | 0 |
| 11:01:15 | mutant | - | `m5_first_is_last_error` (killed) | 17.875 | 1 |
| 11:01:33 | mutant | - | `m6_guard_note_any_line` (killed) | 23.533 | 1 |
| 11:01:57 | mutant | - | `tools.mutate@963ca6a:m1_notes_past_next` (survived) | 18.758 | 0 |
| 11:02:06 | build | gcc-debug | `format-check` (pass) | 1.823 | 0 |
| 11:02:08 | trace | - | `convention-trace` (pass) | 0.261 | 0 |
| 11:02:39 | test | - | `w2:mutate_test@cd01fcb,tools@963ca6a` | 17.184 | 0 |
| 11:02:56 | test | - | `w2:measure_record_test@cd01fcb,tools@963ca6a` | 3.840 | 0 |
| 11:03:00 | test | - | `w2:jobpool_test@cd01fcb,tools@963ca6a` | 62.623 | 0 |
| 11:04:02 | test | - | `flake:jobpool_test@6f373e7#1` | 62.896 | 0 |
| 11:05:02 | mutant | - | `tools.mutate@963ca6a:m2_no_end_of_job_break(old code)` (survived) | 17.256 | 0 |
| 11:05:05 | test | - | `flake:jobpool_test@6f373e7#2` | 63.044 | 0 |
| 11:06:08 | test | - | `flake:measure_record_test@6f373e7#1` | 3.824 | 0 |
| 11:06:12 | test | - | `flake:measure_record_test@6f373e7#2` | 3.926 | 0 |
| 11:06:16 | test | - | `flake:measure_record_test@6f373e7#3` | 3.828 | 0 |
| 11:06:21 | test | - | `flake:measure_record_test@6f373e7,taskset-31` | 4.093 | 0 |

## Pool readings

I only read `tools/jobpool status`; I never started, stopped or opened the shared pool. The only private units were the ones the repository's own `jobpool_test.py` makes, and it stopped and reset them itself; none were left over.

| When | Slots free | Clients |
| --- | --- | --- |
| Before configure | 21 of 23 | ctest ×2, ninja |
| Before the mutant runs | 22 of 23 | ninja, ctest |
| Before the W2 and flake runs | 20 of 23 | ctest ×2, one unnamed |
| Mid-run | 20 of 23 | ctest |
| After all runs | 0 of 23 | one ninja client, not mine |

## Cost

- The harness reported 155,804 subagent tokens, 83 tool calls, and 747 s (12.5 min) to the implementer.
