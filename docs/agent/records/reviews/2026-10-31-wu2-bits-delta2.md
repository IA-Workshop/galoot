# W5 delta review: feature/core-bits, c9d9cad to 66122dd (markers and golden comparisons)

- Reviewer: blind-reviewer, Claude Opus 5.5, 2026-10-31, in a detached worktree of its own (removed after use; its 136 run rows, 62.8 s, are saved in the session scratchpad as `review-wu2c-runs.tsv` for appending at the merge).
- Reviewed: commits 8a5f890, dbd2309, 2ef0ee6, 90ebe25, 66122dd; the two merges (1f77713, 4961277) were re-run with `--remerge-diff` and resolved only the snippets README. `develop` moved from 0c1890a to 9d04f25 (records only) during the review.
- Verdict: merge after fixes. Nothing blocks. L1, L2, L5, and L6 are small and best fixed now.
- Skipped: gcc-release, gcc-asan, clang-debug, clang-release, and the measurement presets (the author's P9 run on 2ef0ee6 covers them under P10; the code tree at 66122dd differs only in records). Not rechecked: the header semantics and abort tests covered by the two earlier reviews. The reviewer made two trial merge commits with `git commit-tree` in the shared object store, with no ref pointing to them.

## Checks

- Clean builds and 43 of 43 tests on gcc-debug, clang-asan, and lint; `core.bits` runs 16 cases and 97,239 assertions, matching 96,789 + 498 new − 48 removed. The trace gate, the marker check, the golden-facts check, and `golden.py --check` pass.
- All eight golden checks fail under a targeted header mutation (run in three modes: full, with the `all_agree` asserts removed, and with all asserts removed and each golden case run alone). Mutating `golden.hpp` itself fails the matching case, and `reference.golden` catches the stale copy.
- The removal of the hand-worked rigid(3) table lost nothing: the 16 removed rows equal `golden::complements::n4`, and the reviewer derived all 16 rows plus n = 5 again with the permutation sign as an integer determinant, a different route from counting inversions. Every other CHECK and `static_assert` was diffed; only the diagonal case changed, by the readability refactor.
- Blade-name parsing works for n up to 5. All 16 cases and 4 marked asserts were read; the markers are true of their bodies apart from L2 to L4.
- Trace rows 1.5, 2.1, and 3.1 are earned; 1.1 is weakly earned (L1); 1.7 correctly stays no.
- The snippet catches 12 of 12 in both modes, rejects wrong argument counts, and refuses a dirty header.
- No citation of provisions, records, or agents in the added lines; no line over 80 columns.
- Build cost of the bits test (gcc-debug, instruction counts, three runs, spread within 0.3%): 2.284 G at c9d9cad, 2.99 G at the head. Adding `<span>` and `<string_view>` costs 0.132 G, `golden.hpp` 0.085 G, the four run-time golden cases 0.487 G, the four golden `static_assert`s 0.138 G; removing the rigid(3) table saves 0.134 G. The main cause of the growth is the doctest expansion of the four run-time cases.

## Findings (all low)

- L1. The 1.1 trace row is earned weakly: the two cases credited to it check the header only through `grade` and `complement_mask`, which give the same answer whatever order the bits are numbered in. With `reorder_sign` counting in the opposite order both still pass. Add a check of the bit order, for example `reorder_sign(0b010U, 0b001U) == -1`, or state the 1.1 fact in the reorder case's marker.
- L2. Case at line 371 is marked `proof` but checks one hand-worked mask; it is an independent value.
- L3. Loose marker wording at lines 416, 506, and 384 (blades that share a generator; the complements do not depend on the metric; the marker omits the `antiscalar(4)` check).
- L4. The name parser checks less than its comment says: it accepts "e21" or a name without a leading `e`. Narrow the comment or check increasing indices.
- L5. The snippet reports success when nothing could build: an unknown preset prints "caught (build failed)" twelve times and exits 0, and it never checks that a build failure is the intended assertion. Build and test the unmutated tree first and exit 2 if that fails.
- L6. The snippet's docstring line "Last checked at ... all 12 mutations caught" is agent-process status; move it to the snippets index.
- L7. Merge friction with the metric branch in the trace table, `CMakeLists.txt`, and `core.hpp`; keep every yes; the branch that merges second reruns the gate.
- L8. The plan's cost cause is incomplete; record the measured split above.
