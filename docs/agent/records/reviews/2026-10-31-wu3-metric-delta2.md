# W5 delta review: feature/core-metric, header 0e2d3ef..10aeff3 and tests 911b4e8..717294f (WU3, plan 0005)

- Reviewer: blind-reviewer, Claude Opus 5.5, 2026-10-31, in a detached worktree of its own at 717294f (removed after use; its 94 run rows are in the session scratchpad under `wu3c/review-wu3c-runs.tsv`).
- Reviewed: `git diff 0e2d3ef 10aeff3 -- include .clang-tidy` and `git diff 911b4e8 717294f -- test docs/agent/records/convention-trace.md`, covering commits 22983f6, d9de465, 36f4d9f, 64dc0ac, and 10aeff3.
- Verdict: merge. Nothing blocks; the findings are non-blocking and none needs a re-review. `develop` moved (9132b01 to 259c552) during the review, so merging it in and rerunning P9 on the merged head (W7) is still to do.
- Not run: gcc-release, gcc-asan, clang-debug, clang-release (the author's plan records 31 of 31 on each at 10aeff3, and 717294f changes only docs); the cost measurement (not done by the author); the `mask_t` redeclaration against core-bits (unchanged since the last review). The first two scratch compile matrices ran directly, not through `tools/timed`.

## Checks

- Clean builds at 717294f: gcc-debug, clang-asan, and lint each pass 31 of 31; format-check and the header sets are clean. The trace gate passes (1.2 at lines 213 and 226, 1.3 at 252 and 288, 1.4 at 275, 288, and 319). Rows 1.2 to 1.4 are earned; 1.1 staying no is defensible because the mask mapping belongs to core-bits.
- All 11 markers read; their words are true of their tests.
- Independent derivation from the conventions text (282 generated `static_assert`s): every split of R(p,q,r) for n up to 6 (83), rigid(0..5), conformal(0..4), the diagonal squares of R(d+1,1,0), and every `in_diagonal` entry as rationals, compared with both the golden header and the library on both compilers: 0 mismatches. Golden lists every split exactly once. BᵀDB = G holds exactly for d = 0 to 4. A two-flip control fails on both compilers.
- Golden mutations in scratch copies each fail the matching test case, except that a dropped last row with the array size fixed passes (N1).
- Header mutants on gcc-debug and clang-asan: killed on both compilers apart from dropping the new `if constexpr` in R (N2).
- The row-by-row coverage diff against the removed test-side sweeps found nothing lost; the R(d+1,1,0) check is stronger than the old `is_diagonal()` check.
- The guard returns false on both compilers for six forms at n = 1 and n = 2 (static const defined, declared-only external linkage, non-static member, static inline const, private constexpr, non-const static); the old header hard-errors on four of them at n = 1. The `NOLINTNEXTLINE` for the declared-only tag is needed.
- The `if constexpr` change across 18 wrap and bound cases removes the misleading second message everywhere and loses nothing for in-range counts, with one exception (N3).
- The conformal wrap test passes against the old header and fails on both compilers when the check is removed or the pre-fix form is restored.
- `.clang-tidy` changes only the two bounds exclusions plus their reason; every NOLINT has a reason.
- Building the intermediate commits against the old header: 36f4d9f and 64dc0ac fail on both compilers; 22983f6 compiles but fails clang-tidy (N4); d9de465 is clean.
- Added lines cite no provision, record, or agent; tests and code are in separate commits; every commit carries agent attribution.

## Findings (all non-blocking)

- N1 (low). The R sweep test is named "for every split with n from 1 to 6" but trusts the golden row list; a dropped row with its array size adjusted passes, and only the golden byte-equality check guards it. Add one `static_assert(Table.size() == (n + 1) * (n + 2) / 2)` per n.
- N2 (low). The "no misleading second message" behavior has no test: removing the `if constexpr` in R passes all ten `core.metric` tests. The compile-fail helper can only require text, not forbid it; add a forbid option or record the gap.
- N3 (nit). When a count exceeds MaxN and MaxN is wider than a mask, only the count message appears; the second true message is suppressed (`conformal<SIZE_MAX, 33>()`). Accept, or check MaxN outside the `if constexpr`.
- N4 (low, process). The lint build fails at 22983f6; the W2 departure is not in the plan, which names only 36f4d9f though 64dc0ac does not build either; the 36f4d9f message says the next commit fixes it but the fix is 10aeff3.
- N5 (nits). The `.clang-tidy` reason says "the test builds" check array accesses, but the measure presets do not set the library assertions; say "the correctness presets". Marker kinds for written-out values differ; the markers at 359 and 374 are incomplete; the name at 319 says "for d from 0 to 4" though the differing-signature clause is checked directly only at d = 3; the 64dc0ac message calls a warning-as-error a rejection.

Earlier findings D1 to D5 resolved apart from N2 and N3 here; the `mask_t` redeclaration and the three-size cost measurement remain open.
