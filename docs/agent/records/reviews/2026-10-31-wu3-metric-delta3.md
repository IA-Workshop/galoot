# W5 delta review: feature/core-metric, 02e7093 to a01add5 at head 12d346d (WU3, plan 0005)

- Reviewer: blind-reviewer, Claude Opus 5.5, 2026-10-31, in a detached worktree of its own at 12d346d (removed after use; its 18 run rows are in the session scratchpad under `wu3d/review-wu3d-runs.tsv`).
- Reviewed: `git diff 02e7093 a01add5 -- include cmake test .clang-tidy CMakeLists.txt` (10 files), the merge 02e7093, and the record commit 12d346d.
- Verdict: merge. No finding blocks and none needs a re-review.
- Not run: gcc-release, gcc-asan, clang-debug, clang-release, the cost measurement (the plan records 57 of 57 on all seven presets at a01add5; 12d346d changes only records); the 282-assert derivation and the earlier header-mutant set (the delta changes nothing metric.hpp computes; the earlier edge matrix gave identical diagnostics). The edge matrix, FORBID checks, and golden-drop compiles ran as scratch commands; the FORBID checks called `cmake -P` directly. `develop` moved by two records-only commits (584d9f1, 5ab3527) after the merge base.

## Checks

- Clean builds at 12d346d: gcc-debug, clang-asan, and lint each pass 57 of 57; format-check and the header sets pass.
- Gates from the repository root: the trace gate passes (1.2 at lines 218 and 239, 1.3 at 265 and 301, 1.4 at 288, 301, and 332; 1.1, 1.5, 2.1, 3.1 from the bits tests); the marker check finds 28 test cases all marked; the golden check passes.
- Merge resolution: all seven trace rows are yes; `core.hpp` lists bits, then metric; the header set lists assert, bits, metric; `test/CMakeLists.txt` keeps both blocks.
- `metric.hpp` compiles standalone, with `bits.hpp` in either order, and through `core.hpp`, on both compilers; no `mask_t` declaration or `<cstdint>` is left. The edge matrix: `metric<32>` and every factory at MaxN 32 compile; `metric<33>` and MaxN 33 are rejected with the true message; so are `R<17,0,0>`, `R<0,0,0>`, `R<SIZE_MAX,0,1>`, `rigid<32,32>`, `conformal<31,32>`.
- FORBID: a present literal fails and names it; an absent one passes; regex metacharacters match literally; several FORBID literals work and a required literal still applies; callers without FORBID behave as before (only `core.metric_count_wrap_zero` and the two tool tests carry it).
- Mutations: removing R's `if constexpr` fails `core.metric_count_wrap_zero`; removing R's count `static_assert` fails both wrap tests. Dropping the last row and its size from a scratch golden copy fails the matching size assert for every n on both compilers.
- Intermediate commit d8cf3a8 (tests ahead of the header change) builds and passes 57 of 57 on gcc-debug and lint, so W2 holds there.
- The six changed markers are true of their tests. No added line cites a provision or agent record; the touched CMake files keep older citations that belong to the cleanup unit. Tests and code are in separate commits and every commit carries agent attribution.

## Findings (all non-blocking)

- R1 (low). The same gap as before remains in `conformal`: without the `if constexpr` at `metric.hpp:220`, `conformal<SIZE_MAX - 1>()` (where D + 2 wraps to 0) prints the false "at least one generator" message on both compilers, and a plain block still passes all 11 `core.metric` tests. Add a FORBID twin with `conformal<SIZE_MAX - 1>()` or record the gap.
- R2 (nit). `tools.expect_output_forbid` (WILL_FAIL) passes on any failure of the script; its twin covers the realistic mutants.
- R3 (nit). A `;` in a FORBID literal splits it and the helper does not say so; it errs strict. Document it or reject `;` as MESSAGE does.
- R4 (nit). The row count does not prove the rows are distinct; the golden header's byte-equality check still guards this.
- R5 (nit). Two comments in `metric.hpp` wrap short after the edit (about lines 53 and 168).
