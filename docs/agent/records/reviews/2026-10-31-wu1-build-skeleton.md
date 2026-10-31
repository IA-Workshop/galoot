# W5 review: feature/build (WU1) into develop

- Reviewer: blind-reviewer, Opus 5.5 (`claude-opus-5-5`), 2026-10-31, in a detached worktree of its own.
- Reviewed: `develop...feature/build`, head `a00259cd7e49`, merge-base `aa8051c`. `develop` had moved to `9f17aba` (records only).
- Author: the WU1 build-expert implementer (agent). The commit trailers say Sonnet 5.5; the cost log says Opus. Decision 0032 item 4 applies from now on.
- Verdict: merge after fixes B1, B2, B3. N1 to N4 are recommended in the same pass. The rest go to plan 0003 or the M0 snippets review.

## Departures and gaps (C20)

- The reviewer wrote one log file outside its worktree by a relative-path slip, listed and deleted it.
- The reviewer saved its 44 `runs.tsv` rows to the session scratchpad (`review-wu1-runs.tsv`, 26 columns, branch `detached`) to append at the merge (P13).
- Not checked by author or reviewer: the doctest-expert review of the smoke test; `ninja -t deps` selections; token loss when a build is killed; a pool restart during builds.
- Not checked by the reviewer: RAPL unreadable and `perf_event_paranoid` above 2 (code read only); a real multiplexed pinned run; pinned runner compiles; CTest's own matching of escaped patterns (emulated); the standard-header list against N4950; the cited research notes; ASan under ptrace.
- The earlier merge `1a5be94` on the branch used git's default message, with no agent identification and none of the P13 fields, and its tree has a doubled `runs.tsv`. It cannot be rewritten; the final merge must not repeat it.

## Acceptance criteria

Criteria 1 to 9 and 11 to 14 met (clean workflows of seven correctness presets pass 9 of 9 tests; measurement presets configured and flags inspected only). Criterion 5 met but weak (N1). Criterion 10 not met on a clean build (B1). Criterion 15 is the merge.

Mutation checks: a never-matching zero-test regex fails `core.zero_cases`; an empty smoke test fails `core.smoke`; a compiling source fails `core.assert_consteval`; removing cycle detection or the guard check fails the include-scan rejects; adding `#include <stdio.h>` to `assert.hpp` fails `core.include_scan`. The recorder never writes 0 for an unavailable quantity, asserts no bound, and passes exit codes 3, 134, and 137 through.

## Findings

**Blocking**
- B1. `tools/measure:282-288`: `compile.tsv` header race. `append_row` tests `f.tell() == 0` after taking the lock, but the position was fixed at open, so two concurrent first compiles both write a header. Seen on a clean clang-debug build. Fix: test `os.fstat(f.fileno()).st_size == 0` after `flock`.
- B2. `include/galoot/core/assert.hpp:33,62,78,80`: the detail names are in `galoot::detail`, but S6 puts the core in `galoot::core`, with non-user names in a nested `detail`. `GALOOT_ASSERT` expands to the wrong namespace in user code. Fix: `galoot::core::detail`, and correct decision 0030 if needed.
- B3. Process: merge `develop` (now `9f17aba`) into the branch, run the P9 checks on the result (about 25 s, or argue P10 since only records changed), and write the merge message with the selection, configurations, outcome, wall time, and agent authorship.

**Non-blocking**
- N1. A compile-fail test can pass for the wrong reason: the message reaches the diagnostic only through the echoed source line, so an unrelated error on that line passes. Also require `assertion_failed`; correct the comment.
- N2. `core.zero_cases` copies the regex and does not check the rule is applied. Check `core.smoke`'s `FAIL_REGULAR_EXPRESSION` at configure time.
- N3. A `;` in MESSAGE splits into alternative regexes. `FATAL_ERROR` on `;`.
- N4. The pool guard keys on the command's base name, so `taskset`, `env`, or `nice` in front of `ctest` bypasses it. Treat step `test` as needing the pool.
- N5. `tools/measure:217-234`: possible silent re-run if perf ran the workload but no output line matches an event name. Decide from perf's status; never re-run once the workload started.
- N6. `git status` once per compile may collide with a commit; use `git --no-optional-locks`.
- N7. `GALOOT_CHECKING`, `GALOOT_PRESET`, `GALOOT_COMPILE_RECORD` are uninitialized cache entries with no help text; `GALOOT_PRESET` without `GALOOT_COMPILE_RECORD` fails every compile. Declare and check them (C8).
- N8. `LDFLAGS` from the environment leaks into non-asan builds; `format-check` lacks `CONFIGURE_DEPENDS`. Set `CMAKE_EXE_LINKER_FLAGS` in the base preset (C8).
- N9. Release without `-DNDEBUG` is recorded only in the plan's outcome; add it to decision 0029.
- N10. LLD, Ninja, and git appear in no decision; add them to decision 0027 (C13, C18).
- N11. Tests and their expected literals were committed with the code they judge, without listing them in a review request (W9). Undeclared.
- N12. The P1 baseline was recorded at `5134bb5-dirty`; re-record the four compiles on a clean revision.
- N13. The include scan misses `#include_next` and computed includes, and never flags non-`.hpp` files under `core/`.
- N14. `runs_tsv_merge.py`: copies the other side's header as a data row when the base lacks the file; its "last checked" line names no revision; run during `git merge --no-commit` it avoids a doubled merge commit; it conflicts with decision 0032 item 3.
- N15. Public `///` comments in `assert.hpp` cite plan 0002, a process record (W12).
- N16. Observation: perf's `<not counted>` is written as `unavailable`, so some shared rows lose the per-core instruction sum; a command that cannot start writes no row.
