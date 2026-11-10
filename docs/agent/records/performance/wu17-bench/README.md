# WU17 E2: the rga3 benchmarks, Galoot against its straight-line reference form, reference inline

Plan 0038 section 4 item 6 (DoD-24's benchmark; P6, P14, V15), rerun under decision 0237 items 2 and 4 at `2de0d36`, where the reference kernels and both forms' lambdas are `gnu::always_inline` (`part-d-inline/README.md`). Run by Claude (agent, Opus 5.5, performance-engineer) on 2026-11-10 in the worktree `.claude/worktrees/wu17-e23`, branch `records/cost-evidence-measurements`. The first run, at `18849b5` with the reference kernels out of line, is `first-run-out-of-line/`. Recorded, never asserted (decision 0013).

## The protocol, fixed before the run

Written and committed before the first timed process of this run.

- Revision `2de0d36` merged into this branch (`052bda1`); `include/` is unchanged from `18849b5`, and `test/` differs only in `test/bench/` and `test/reference/reference_kernels.py`.
- The same driver as the first run with three changes: the busy rule below, the selections named `wu17-bench-inline:...`, and two more diagnostic events (scalar and packed double arithmetic per operation). The same passes (`PASSES`, from the first run's calibration), seed 20261110, 128 operand sets, three environment sizes (0, 1352, 2768 characters of `BENCH_ENV_PAD` in an otherwise empty environment), eleven interleaved rounds per size, pairs of processes at 1/8 and all of the passes, CPU 4 pinned with its sibling CPU 5 left idle, the lock taken and released per batch with `lock.sh`, pool and load read before and after.
- **Busy rule, in ticks (decision 0237 item 4).** `/proc/stat` counts busy time in ticks of 10 ms. A pair (its two processes, together about 1.0 s of wall time at the first run's passes: about 0.85 s and 0.15 s) is repeated once, both kept, when its sibling thread's busy time, or its CPU's busy time beyond the pair's own, summed over the two processes, is more than 2 ticks (0.02 s, about 2% of the pair's wall time). Each process's figure is the difference of two tick counts, so it can be one tick off with nothing running and the pair's sum two; a third tick is then work that ran. A pair busy again is marked and used.
- The criterion, unchanged: a difference is called when the medians differ by more than twice the larger of the two ranges, with the same form faster, at every environment size (strict) or at the smallest with the same order at all three (weaker). Both are reported.
- Clang's throughput and latency loops are read separately (review F1 of the inlining unit): with the reference inline, Clang vectorizes the reference's throughput loop across iterations, two operations per iteration, which the library's call prevents; the latency loop cannot be vectorized that way. The diagnostic counts packed and scalar double arithmetic per operation in each timed loop to show it.

RESULTS_PLACEHOLDER
