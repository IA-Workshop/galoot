# WU17 E2, first run: the reference kernels out of line

**This is the first E2 run, kept as the out-of-line-reference run (decision 0237 item 2).** At `18849b5` both compilers called the reference kernels of the three transforms and of compose out of line, so for those four operations this run compares Galoot with a straight-line kernel behind a call (caveat 1). The rerun with the reference kernels inline, at `2de0d36`, is `../README.md`. The files were moved here unchanged after the run; the paths in this README's commands and in `batch.sh` and `drive.py` name `wu17-bench/`, where they ran, and read `wu17-bench/first-run-out-of-line/` now.

Plan 0038 section 4 item 6 (DoD-24's benchmark; P6, P14, V15). Run by Claude (agent, Opus 5.5, performance-engineer) on 2026-11-10 in the worktree `.claude/worktrees/wu17-e23`, branch `records/cost-evidence-measurements`, at `18849b5` (the tip of `feature/cost-evidence`, which holds parts C and D). Recorded, never asserted (decision 0013): nothing here bounds anything or fails a check.

## Caveats, read first

1. **The reference form makes a call on every transform and compose.** A counter of retired calls (below) shows that both compilers emit the reference kernels of the three transforms and of compose out of line and call them once per operation. The benchmark of those four operations therefore compares Galoot with a straight-line kernel behind a call, not with inline straight-line code. On GCC, Galoot's transforms are inline (no call), so GCC's "Galoot faster" results for the transforms' latency loops include the reference's call. On Clang both forms of every transform and of compose make at least one call.
2. **Store-forwarding stalls decide the wedge results on GCC.** The large differences of `p ∧ q` and `p ∧ q ∧ r` on GCC follow the loads that a store could not forward to (2 per operation in the slower form, 0 in the faster), which depend on how each form's result reaches the checksum, not on its arithmetic: the two forms retire about the same instructions for `p ∧ q ∧ r`. They are code-generation effects of the benchmark loop as compiled, and E1's assembly read is the place where their instructions are named.
3. **The busy threshold sits at the counter's resolution.** The 1% threshold fixed in plan 0038 section 6 is, for runs of about 1 s, one tick of `/proc/stat` (10 ms). 615 of the 1,452 pairs were repeated and 245 were busy again, nearly all on one or two ticks (the largest busy time of any process was 0.02 s on the sibling and 0.03 s on CPU 4). Over the 615 repeated pairs the first attempt's nanoseconds per operation differ from the second's by a median of 0.0% (−4.1% to +3.8%). The owner's editor language server ran at one core's load throughout and moves between CPUs; it is outside the lock.
4. **The range criterion is set by the worst run.** With 11 runs a single slow run sets the range: GCC compose's throughput range at size 0 is 3.82 ns against 0.06 ns for its reference, so the criterion cannot call a difference there that sizes 1352 and 2768 show. Both readings of the criterion over the sizes are given below.
5. **Nanoseconds per operation include the checksum** (a move and an addition per coefficient, the same in both forms), and the instructions and cycles per operation are differences of two processes with 1/8 and all of the passes, so the process's start, the drawing of operands, and the printing are left out (review finding H2 of part D).

## What ran

- Toolchain: g++ 16.2.1 20260810 and clang++ 23.1.4, libstdc++ 16; Linux 7.3.1-1-cachyos; perf 7.3.1; ASLR on (`randomize_va_space` 2). Machine: Intel Core i9-14900K, CPU 4, a performance core (48 KiB L1d), its sibling CPU 5 left idle by this work; governor `powersave` (read, not changed).
- Options: the `gcc-measure` and `clang-measure` presets, `-O2 -ffp-contract=off -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror`, `GALOOT_CHECKING` off. Built in this worktree with `cmake --build build/<preset> --target bench_<operation>...` through `tools/timed` (rows in `runs.tsv` here); `ctest -L rga3.benchmarks` passed on both presets before any timing (each program's `check` mode: both forms give the same checksum).
- Workload: `test/bench/<operation>_bench.cpp` at `18849b5`, seed 20261110, 128 operand sets per run, the passes of `drive.py`'s `PASSES` (from `out/calibration.txt`, chosen for long runs of about 0.8 s). Eleven rounds at each of three environment sizes; in each round every (compiler, operation, loop, form) runs once as a pair of processes, in an order that reverses the compilers and the forms every other round. Each process runs with an empty environment (`env -i`) but for `BENCH_ENV_PAD` of 0, 1352, or 2768 characters (environment strings of 0, 1,367, and 2,783 bytes), which moves the start of the stack.
- The forms' checksums agree in every pair of runs with the same passes and seed (checked by `drive.py summarize`).

Commands, from the worktree's root, with the pool's environment exported:

```bash
python3 -I docs/agent/records/performance/wu17-bench/drive.py calibrate docs/agent/records/performance/wu17-bench/out --cpu 4   # under the lock
docs/agent/records/performance/wu17-bench/batch.sh 0      # each takes and releases the lock itself
docs/agent/records/performance/wu17-bench/batch.sh 1352
docs/agent/records/performance/wu17-bench/batch.sh 2768
python3 -I docs/agent/records/performance/wu17-bench/drive.py summarize docs/agent/records/performance/wu17-bench/out
python3 -I docs/agent/records/performance/wu17-bench/drive.py stalls docs/agent/records/performance/wu17-bench/out --cpu 4   # under the lock
```

`drive.py` is the driver the part D review asked for (finding I2); its docstring gives each step. Every benchmark process ran as `taskset -c 4 tools/timed --log out/runs.tsv bench <preset> <selection> -- env -i [BENCH_ENV_PAD=...] <program> <form> <loop> <passes> 20261110`, so each row of `out/runs.tsv` is pinned to CPU 4 with its instructions, cycles, cache events, sibling busy time and other busy time on CPU 4. `out/rows.tsv` is the per-run table: one row per process (4,134), with the round, the attempt, the busy flag, and the program's own output. The diagnostic `stalls` runs `perf stat` itself, not inside `tools/timed`, whose eight counters would multiplex its two (a first attempt inside `tools/timed` stopped at once on a multiplexed count, exit 1 below); its runs are not in a run log.

## Lock, pool, and load

Taken and released with `lock.sh` for each batch; read from `out/protocol.log`:

| Batch | Lock held (UTC) | Pool before / after | Load average (1 min) before / after |
| --- | --- | --- | --- |
| calibrate | 17:13:29 to 17:13:33 | 23 of 23 free / 23 of 23 free | 1.04 / 1.12 |
| size 0 | 17:14:51 to 17:26:16 | 23 of 23 free / 23 of 23 free | 1.10 / 2.05 |
| size 1352 | 17:26:56 to 17:37:26 | 23 of 23 free / not running | 1.73 / 2.00 |
| size 2768 | 17:37:42 to 17:48:04 | not running / not running | 1.78 / 1.06 |
| stalls, first attempt | 17:49:09 to 17:49:09 (exit 1) | not running | 1.44 / 1.44 |
| stalls | 17:49:32 to 17:50:09 | not running | 1.31 / 1.65 |
| stalls with calls | 17:52:59 to 17:53:36 | not running | 1.39 / 1.78 |

"Not running" is the pool having stopped itself after 30 minutes without a client; it was not started or stopped by this work. The load average never fell below 1, so no energy figure is attributed (plan 0038 section 6); `energy_pkg_j` stays in the rows unread.

## Results

Nanoseconds per operation: median [range] over the 11 rounds at environment sizes 0 / 1352 / 2768. Instructions and cycles per operation: medians at size 0 (the instruction counts have range 0 at every size; cycles vary by at most 26 per operation, in the one slow GCC compose run). Store-forward blocks and calls: loads per operation that a store could not forward to (`ld_blocks.store_forward`) and retired near calls per operation (`br_inst_retired.near_call`), Galoot / reference, from `out/stalls.tsv`. "Difference?" applies the criterion: medians apart by more than twice the larger of the two ranges, with the same form faster, at every size ("yes"), or at the smallest size only.

### GCC 16

| Operation | Loop | Galoot ns | Reference ns | Instructions | Cycles | Store-forward blocks | Calls | Difference? |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| p ∧ q | throughput | 4.77 [0.18] / 4.93 [0.39] / 4.77 [0.43] | 1.53 [0.01] / 1.53 [0.06] / 1.53 [0.08] | 56.0 / 42.0 | 28.3 / 9.0 | 2 / 0 | 0 / 0 | yes, reference faster |
| p ∧ q ∧ r | throughput | 2.83 [0.02] / 2.83 [0.01] / 2.83 [0.05] | 5.53 [0.02] / 5.53 [0.02] / 5.52 [0.06] | 81.1 / 82.1 | 16.6 / 32.8 | 0 / 2 | 0 / 0 | yes, Galoot faster |
| l ∨ g | throughput | 4.91 [0.18] / 4.90 [0.03] / 4.90 [0.05] | 5.14 [0.16] / 5.14 [0.03] / 5.14 [0.06] | 56.0 / 62.0 | 29.1 / 30.5 | 2 / 1 | 0 / 0 | no |
| transform(p, Q) | throughput | 7.72 [0.04] / 7.71 [0.20] / 7.72 [0.28] | 7.89 [0.20] / 7.89 [0.35] / 7.88 [0.08] | 181.0 / 180.0 | 45.8 / 46.8 | 0 / 1 | 0 / 1 | no |
| transform(p, Q) | latency | 7.51 [0.42] / 7.46 [0.49] / 7.44 [0.20] | 13.15 [0.53] / 13.13 [0.09] / 13.14 [0.14] | 146.0 / 178.0 | 44.1 / 77.9 | 1 / 1 | 0 / 1 | yes, Galoot faster |
| transform(l, Q) | throughput | 9.46 [0.38] / 9.45 [0.09] / 9.44 [0.15] | 9.20 [0.41] / 9.18 [0.27] / 9.19 [0.12] | 246.0 / 245.1 | 55.9 / 54.4 | 3 / 3 | 0 / 1 | no |
| transform(l, Q) | latency | 12.67 [0.45] / 12.67 [0.09] / 12.64 [0.20] | 15.17 [0.45] / 15.18 [0.11] / 15.14 [0.18] | 229.1 / 243.1 | 75.2 / 89.9 | 3 / 0.3 | 0 / 1 | yes, Galoot faster |
| transform(g, Q) | throughput | 7.37 [0.06] / 7.37 [0.03] / 7.38 [0.15] | 7.63 [0.08] / 7.63 [0.06] / 7.62 [0.08] | 160.1 / 168.0 | 43.7 / 45.3 | 2 / 2 | 0 / 1 | smallest size only (Galoot) |
| transform(g, Q) | latency | 7.32 [0.05] / 7.31 [0.07] / 7.30 [0.08] | 11.73 [0.07] / 11.72 [0.10] / 11.72 [0.12] | 146.0 / 166.0 | 43.4 / 69.6 | 1 / 0.1 | 0 / 1 | yes, Galoot faster |
| compose(Q2, Q1) | throughput | 9.99 [3.82] / 9.90 [0.33] / 9.78 [0.31] | 8.81 [0.06] / 8.81 [0.04] / 8.79 [0.10] | 210.0 / 202.1 | 59.3 / 52.2 | 3 / 4 | 1 / 1 | no |
| compose(Q2, Q1) | latency | 10.58 [0.20] / 10.58 [0.64] / 10.56 [0.25] | 9.90 [0.43] / 9.89 [0.45] / 9.88 [0.15] | 206.0 / 202.1 | 62.7 / 58.5 | 3.5 / 3.0 | 1 / 1 | no |

### Clang 23

| Operation | Loop | Galoot ns | Reference ns | Instructions | Cycles | Store-forward blocks | Calls | Difference? |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| p ∧ q | throughput | 1.38 [0.04] / 1.38 [0.01] / 1.38 [0.05] | 1.44 [0.04] / 1.44 [0.01] / 1.44 [0.03] | 41.0 / 41.0 | 8.1 / 8.4 | 0 / 0 | 0 / 0 | no |
| p ∧ q ∧ r | throughput | 2.65 [0.10] / 2.65 [0.01] / 2.64 [0.07] | 2.89 [0.11] / 2.89 [0.01] / 2.89 [0.04] | 75.8 / 73.8 | 15.6 / 17.1 | 0 / 0 | 0 / 0 | yes, Galoot faster |
| l ∨ g | throughput | 1.47 [0.04] / 1.47 [0.01] / 1.47 [0.02] | 1.62 [0.05] / 1.62 [0.02] / 1.62 [0.03] | 45.0 / 48.9 | 8.6 / 9.5 | 0 / 0 | 0 / 0 | yes, Galoot faster |
| transform(p, Q) | throughput | 7.73 [0.33] / 7.73 [0.04] / 7.72 [0.10] | 7.51 [0.32] / 7.50 [0.04] / 7.50 [0.08] | 166.0 / 174.0 | 45.8 / 44.5 | 2 / 2 | 1 / 1 | no |
| transform(p, Q) | latency | 11.11 [0.50] / 11.10 [0.05] / 11.09 [0.14] | 11.91 [0.51] / 11.90 [0.05] / 11.88 [0.13] | 165.0 / 173.0 | 65.8 / 70.6 | 1.7 / 1.0 | 1 / 1 | no |
| transform(l, Q) | throughput | 10.28 [0.43] / 10.28 [0.20] / 10.26 [0.18] | 9.37 [0.34] / 9.37 [0.05] / 9.36 [0.19] | 272.0 / 263.0 | 60.4 / 55.2 | 3 / 3 | 2 / 1 | yes, reference faster |
| transform(l, Q) | latency | 16.12 [0.61] / 16.13 [0.07] / 16.11 [0.14] | 15.62 [0.65] / 15.62 [0.06] / 15.61 [0.17] | 271.0 / 262.0 | 95.7 / 92.6 | 2.9 / 0.3 | 2 / 1 | no |
| transform(g, Q) | throughput | 5.74 [0.02] / 5.74 [0.02] / 5.73 [0.10] | 5.73 [0.11] / 5.73 [0.01] / 5.72 [0.11] | 167.0 / 166.0 | 33.6 / 33.5 | 0 / 0 | 1 / 1 | no |
| transform(g, Q) | latency | 9.44 [0.40] / 9.43 [0.05] / 9.40 [0.11] | 9.38 [0.42] / 9.36 [0.04] / 9.36 [0.09] | 166.0 / 165.0 | 55.9 / 55.6 | 0 / 0 | 1 / 1 | no |
| compose(Q2, Q1) | throughput | 8.88 [0.58] / 9.05 [0.19] / 9.04 [0.27] | 6.00 [0.20] / 6.00 [0.02] / 5.98 [0.12] | 196.0 / 159.0 | 52.6 / 35.2 | 1 / 0 | 1 / 1 | yes, reference faster |
| compose(Q2, Q1) | latency | 12.54 [0.60] / 12.55 [0.18] / 12.59 [3.54] | 8.68 [0.37] / 8.70 [0.31] / 8.66 [0.38] | 200.0 / 160.0 | 74.2 / 51.4 | 3.0 / 1.0 | 1 / 1 | smallest size only (reference) |

### Reading

Under the criterion at every environment size, nine differences are called; under the weaker reading (beyond at the smallest size, same order at all three) eleven. The order of the two forms never changed between sizes for any (compiler, operation, loop), so the layout-bias check found no reversal; the sizes moved a median by at most 0.21 ns (GCC compose, Galoot, throughput).

- **Galoot slower, beyond the criterion.** GCC `p ∧ q` (4.77 against 1.53 ns): Galoot's loop retires 14 more instructions per operation and has 2 store-forward blocks per operation against none; inferred, not traced instruction by instruction here: the conversion of the wedge's canonical layout to the `Line` layout (`Line{p ^ q}`), which negates and permutes, sends the result through the stack in stores of 8 and 16 bytes that the checksum reloads in 16-byte loads. Clang `compose` (8.9 to 9.0 against 6.0 ns throughput; 12.5 against 8.7 ns latency at the smallest size): both forms make one call per operation, so the call does not explain it; Galoot retires 37 more instructions per operation (196 against 159), the work E1's read should name. Clang `transform(l, Q)` (10.28 against 9.37 ns throughput, about 9%): Galoot makes two calls per operation against the reference's one and retires 9 more instructions, which fits WU13's record of Clang keeping the line transform's first stage out of line; the extra call's share of the 0.9 ns is not separated here.
- **Galoot faster, beyond the criterion.** GCC `p ∧ q ∧ r` (the reference has the 2 store-forward blocks), the three GCC transform latency loops (the reference makes a call per operation, and for the point and plane retires 32 and 20 more instructions), and Clang `l ∨ g` (4 fewer instructions, 0.9 fewer cycles per operation) and `p ∧ q ∧ r` (2 more instructions in 1.5 fewer cycles).
- **No difference** in the other eleven loops under either reading; their medians differ by under 0.3 ns in seven, and by at most 1.2 ns (GCC compose throughput, whose range at size 0 is 3.82 ns).

Decision 0233 item 3 (O3) asked for a core unit forcing inlining only if a benchmark shows a cost beyond the criterion. Clang's `transform(l, Q)` is beyond it with one more call per operation than its reference; Clang's `compose` is beyond it with no more calls than its reference. The cost of the call itself is not isolated, because the reference kernel is itself called out of line (caveat 1).

## Files

- `drive.py`: the driver (calibrate, run, summarize, stalls). The run step is the one that ran every batch; `summarize`'s `verdict.tsv` and the `stalls` step were added after the size-0 batch and do not change the runs.
- `batch.sh`: one locked batch at one environment size, with the protocol readings.
- `out/calibration.txt`: the calibration's figures; `out/protocol.log`: the readings above.
- `out/rows.tsv`: every process; `out/runs.tsv`: the campaign log (`TIMED_LOG`) of every benchmark process; `runs.tsv`: the campaign log of the configure, build, and check runs.
- `out/results.tsv`, `out/comparison.tsv`, `out/verdict.tsv`, `out/stalls.tsv`: the summaries.
- `part-d-build-cost.md`, `part-d-compile.tsv`: part D's own build-cost record, unchanged.

Verification: the checksums of the two forms agree in every pair (the reference kernels themselves are verified by part D's smoke tests, `rga3.reference_kernels`). Validation: none applies; cost is measured, not validated (plan 0038 section 8).
