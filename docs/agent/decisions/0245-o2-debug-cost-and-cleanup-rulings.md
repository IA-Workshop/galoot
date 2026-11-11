# 0245 Debug compile cost, cleanup, and release placeholders

Date: 2026-11-11. Source: the owner's acceptance of the recommendations in the core pass report.

1. **Superseded the same day by the gate 2 result; the knob question is open again.** The ruling as first written: no inlining knob for 1.0. The forced-inline unit raises debug-build compile cost (GCC `product_test` +80.5% instructions, 2.18 s to 4.32 s, 418 to 830 MiB; `transform_test` +35.7%, 9.19 s to 13.22 s, 972 to 1,432 MiB). The cost is recorded as known friction in the release record and the limits page, and no configuration macro is added. Revisit only if a user reports the cost as a problem.
2. **Cleanup approved, after the merge.** Once the core pass is on `develop`, the stale gate worktrees and merged feature, review and merge branches may be removed. Resolve each path to absolute and check that the branch is an ancestor of `develop` before deleting.
3. **Release placeholders.** The release record's must-fill list (FetchContent URL and tag, CMake VERSION 0.1.0, byline, known friction) is filled at the release commit, not before.

Gate 2 for the core pass (matrix at a310a2f) found the forced public operations make the identity test units very expensive under the sanitizer presets and Clang optimized builds (gcc-asan 5,368 s with one process at 22 GB; clang-asan stopped at 8 to 16 GB per compile; clang-release 665 s against 118 s). The first ruling rested on debug-build evidence only. A new ruling from the owner is needed before any change under `include/`.
