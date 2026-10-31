# W5 delta review: feature/core-bits (WU2 fixes), 7c3f5f0 to c9d9cad

- Reviewer: blind-reviewer, Claude Opus 5.5, 2026-10-31, in a detached worktree of its own (removed with `--force` because `tools/timed` had added rows there; the 79 rows, 42.8 s, are saved in the session scratchpad as `review-wu2b-runs.tsv` for appending at the merge).
- Verdict: merge. All six findings (N1, N2, N4, N5, N8, N9) are resolved; four low findings remain and none blocks.
- Skipped: gcc-release, gcc-asan, clang-debug, clang-release, measurement presets; the author's report that lint failed at the intermediate commits 4292517 to d04ca79 was not reproduced; the hand comparison with the design study table was not rechecked; unchanged parts of the diff (records, other reviews) were skimmed. `develop` moved from 1143ea3 to 16ae555 during the review; the branch head stayed at c9d9cad.

## Checks

- The count is `std::size_t` in every function and `mask_bits` is `std::size_t`; grades stay `int` with one exact cast in `antigrade`; no silent narrowing; clean under `-Wconversion -Werror`; clang-tidy clean. The metric branch counts with `std::size_t`.
- clang-asan passes the n = 32 acceptance tests and every rejection test; with the upper bound widened, UBSan reports a shift exponent of 18446744073709551615, so the guard prevents the undefined behavior and the rejection tests catch the widening.
- No expected value changed: every CHECK, REQUIRE, `static_assert` and message line was diffed; additions are the new case and `static_assert(mask_bits == mask_width)`; ranges are unchanged; exactly two test names changed; the CTest expectations are unchanged.
- The tests use `numeric_limits<mask_t>::digits`, `mask_bits + 1`, `mask_bits`, and `~mask_t{0}`; no bound literal is left.
- The new diagonal-product case catches four mutations (3,096; 3,072; 1,532; 3,128 of 16,384 failures); a fifth (drop bit 30) is caught only by the existing n = 32 sample case, as expected for a six-generator test. Its reference is an independent route.
- The snippet catches 11 of 11 unchanged mutations in both modes, refuses a dirty header or test (exit 2) and leaves the edit intact, and restores from the text it read.
- Clean builds and 32 of 32 tests: gcc-debug 2.91 s, clang-asan 4.22 s, lint 2.34 s; 13 cases and 96,789 assertions.
- No provision or agent citation in the added lines; no line over 80 columns.

## Findings (low)

- L1. Commit 4292517 also carries renames, literal replacements, and message changes that would build with `int`; the W2 reason covers the type change only. No effect on expected values. Record it as a departure.
- L2. Commits 4292517, 106069e, and d04ca79 fail the lint preset by the author's record (fixed in 8ac9d93). History is not rewritten.
- L3. In the diagonal case, `CAPTURE(squares.at(1))` identifies the metric weakly, and the six-generator size appears three times; capture the metric or its index and name the size once.
- L4. The snippet lacks a mutation for accepting n = `mask_bits` + 1; a third argument other than `--runtime-only` is silently treated as full mode; there is no argument-count check; its "last checked" line is stale (the reviewer ran it at c9d9cad).
