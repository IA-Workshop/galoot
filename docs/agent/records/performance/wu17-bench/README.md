# WU17 E2: the rga3 benchmarks, Galoot against its straight-line reference form, reference inline

Plan 0038 section 4 item 6 (DoD-24's benchmark; P6, P14, V15), rerun under decision 0237 items 2 and 4 at `2de0d36`, where the reference kernels and both forms' lambdas are `gnu::always_inline` (`part-d-inline/README.md`). Run by Claude (agent, Opus 5.5, performance-engineer) on 2026-11-10 in the worktree `.claude/worktrees/wu17-e23`, branch `records/cost-evidence-measurements`. The first run, at `18849b5` with the reference kernels out of line, is `first-run-out-of-line/`. Recorded, never asserted (decision 0013).

## The protocol, fixed before the run

Written and committed before the first timed process of this run.

- Revision `2de0d36` merged into this branch (`052bda1`); `include/` is unchanged from `18849b5`, and `test/` differs only in `test/bench/` and `test/reference/reference_kernels.py`.
- The same driver as the first run with three changes: the busy rule below, the selections named `wu17-bench-inline:...`, and two more diagnostic events (scalar and packed double arithmetic per operation). The same passes (`PASSES`, from the first run's calibration), seed 20261110, 128 operand sets, three environment sizes (0, 1352, 2768 characters of `BENCH_ENV_PAD` in an otherwise empty environment), eleven interleaved rounds per size, pairs of processes at 1/8 and all of the passes, CPU 4 pinned with its sibling CPU 5 left idle, the lock taken and released per batch with `lock.sh`, pool and load read before and after.
- **Busy rule, in ticks (decision 0237 item 4).** `/proc/stat` counts busy time in ticks of 10 ms. A pair (its two processes, together about 1.0 s of wall time at the first run's passes: about 0.85 s and 0.15 s) is repeated once, both kept, when its sibling thread's busy time, or its CPU's busy time beyond the pair's own, summed over the two processes, is more than 2 ticks (0.02 s, about 2% of the pair's wall time). Each process's figure is the difference of two tick counts, so it can be one tick off with nothing running and the pair's sum two; a third tick is then work that ran. A pair busy again is marked and used.
- The criterion, unchanged: a difference is called when the medians differ by more than twice the larger of the two ranges, with the same form faster, at every environment size (strict) or at the smallest with the same order at all three (weaker). Both are reported.
- Clang's throughput and latency loops are read separately (review F1 of the inlining unit): with the reference inline, Clang vectorizes the reference's throughput loop across iterations, two operations per iteration, which the library's call prevents; the latency loop cannot be vectorized that way. The diagnostic counts packed and scalar double arithmetic per operation in each timed loop to show it.

## Departures from the protocol above, found during the run

1. **Pair length.** The protocol says a pair takes about 1.0 s. The passes were chosen for the slower compiler of each loop, so the faster forms run shorter: the pairs took 0.32 to 1.85 s, median 0.87 s. The busy threshold of 2 ticks is therefore 1% to 6% of a pair's wall time. It was applied as fixed.
2. **Readings of the size-0 batch.** `batch.sh` wrote its readings to `out/protocol.log` before `out/` existed, so the size-0 batch's "before" and "lock taken" readings were lost. The lock was taken just before the first row of `out/runs.tsv` (18:34:51 UTC), and a reading during the batch is in the log. A `mkdir` was then added to `batch.sh` while that batch was running. Bash reads a script as it runs, so it misread the line after `drive.py`, and the release line reads exit 127. `drive.py` had finished all 11 rounds, and the script's trap released the lock. Both are noted in the log.

## What ran

- Toolchain, machine, and options as in the first run: g++ 16.2.1 and clang++ 23.1.4 with libstdc++ 16, on CPU 4 of the i9-14900K, with the `gcc-measure` and `clang-measure` flags (`-O2 -ffp-contract=off`, checking off).
- Rebuilt in this worktree at `3662d37`, the merge of `2de0d36` plus records only, through `tools/timed` (`runs.tsv` here). `ctest -R '^rga3\.benchmark_'` passed 14 of 14 on both presets before any timing. That covers each program's `check` mode and the `_inline` symbol-table tests.
- Every benchmark process ran as `taskset -c 4 tools/timed --log out/runs.tsv bench <preset> wu17-bench-inline:<...> -- env -i [BENCH_ENV_PAD=...] <program> <form> <loop> <passes> 20261110`. The forms' checksums agree in every pair.
- Busy: 14 of the 1,452 pairs were repeated (5, 4, and 5 at the three sizes), and none was busy again. The largest busy time of any process was 0.02 s, on the sibling and on CPU 4 alike.

Commands, from the worktree's root:

```bash
docs/agent/records/performance/wu17-bench/batch.sh 0     # each takes and releases the lock itself
docs/agent/records/performance/wu17-bench/batch.sh 1352
docs/agent/records/performance/wu17-bench/batch.sh 2768
python3 -I docs/agent/records/performance/wu17-bench/drive.py stalls docs/agent/records/performance/wu17-bench/out --cpu 4   # under the lock
python3 -I docs/agent/records/performance/wu17-bench/drive.py summarize docs/agent/records/performance/wu17-bench/out
```

## Lock, pool, and load

| Batch | Lock held (UTC) | Pool before / after | Load average (1 min) before / after |
| --- | --- | --- | --- |
| size 0 | 18:34:51 to 18:41:59 | lost (departure 2) / 23 of 23 free | lost; 2.11 during / 2.10 |
| size 1352 | 18:42:35 to 18:49:41 | 23 of 23 free / 23 of 23 free | 1.61 / 2.04 |
| size 2768 | 18:49:41 to 18:56:49 | 23 of 23 free / 23 of 23 free | 2.04 / 2.05 |
| diagnostic | 18:57:10 to 18:57:45 | 23 of 23 free / 23 of 23 free | 1.75 / 1.86 |

The load never fell below 1, so no energy figure is attributed.

## Results

The columns:

- **Galoot ns, Reference ns:** nanoseconds per operation, median [range] over the 11 rounds at environment sizes 0 / 1352 / 2768.
- **Instructions, Cycles:** medians per operation at size 0 (the instruction ranges are 0 everywhere).
- **Calls, Packed / scalar double, Store-forward blocks:** per operation in each timed loop, Galoot / reference, from `out/stalls.tsv`:
  - retired near calls;
  - retired packed and scalar double arithmetic instructions (`fp_arith_inst_retired.128b_packed_double`, `.scalar_double`; one packed instruction does two lanes);
  - loads that a store could not forward to.
- **Difference?:** "yes" under the strict reading of the criterion; "smallest size only" under the weaker one.
- **First run, reference ns:** the reference out of line, at size 0, from `first-run-out-of-line/`.

### GCC 16

| Operation | Loop | Galoot ns | Reference ns | Instructions | Cycles | Calls | Packed / scalar double | Store-forward blocks | Difference? | First run, reference ns |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| p ∧ q | throughput | 4.76 [0.17] / 4.76 [0.18] / 4.76 [0.17] | 1.53 [0.01] / 1.53 [0.01] / 1.53 [0.01] | 56.0 / 42.0 | 28.3 / 9.0 | 0.0 / 0.0 | 3.0, 12.0 / 9.0, 0.0 | 2.0 / 0.0 | yes, reference faster | 1.53 |
| p ∧ q ∧ r | throughput | 2.84 [0.01] / 2.84 [0.01] / 2.84 [0.01] | 5.53 [0.10] / 5.53 [0.24] / 5.53 [0.01] | 81.1 / 82.1 | 16.7 / 32.8 | 0.0 / 0.0 | 0.0, 38.0 / 0.0, 38.0 | 0.0 / 2.0 | yes, galoot faster | 5.53 |
| l ∨ g | throughput | 4.90 [0.21] / 4.90 [0.22] / 4.90 [0.02] | 5.13 [0.23] / 5.13 [0.02] / 5.13 [0.01] | 56.0 / 62.0 | 29.1 / 30.5 | 0.0 / 0.0 | 0.0, 20.0 / 7.0, 10.0 | 2.0 / 1.0 | no | 5.14 |
| transform(p, Q) | throughput | 7.71 [0.04] / 7.71 [0.04] / 7.70 [0.04] | 7.67 [0.02] / 7.66 [0.03] / 7.67 [0.03] | 180.1 / 164.1 | 45.7 / 45.5 | 0.0 / 0.0 | 19.0, 54.0 / 19.0, 54.0 | 0.0 / 1.0 | no | 7.89 |
| transform(p, Q) | latency | 7.41 [0.09] / 7.43 [0.12] / 7.39 [0.12] | 9.88 [0.07] / 9.88 [0.11] / 9.90 [0.16] | 146.0 / 151.1 | 44.0 / 58.7 | 0.0 / 0.0 | 0.0, 84.0 / 0.0, 84.0 | 1.0 / 1.0 | yes, galoot faster | 13.15 |
| transform(l, Q) | throughput | 9.47 [0.14] / 9.46 [0.04] / 9.46 [0.07] | 9.23 [0.06] / 9.24 [0.03] / 9.24 [0.03] | 246.0 / 243.1 | 56.1 / 54.8 | 0.0 / 0.0 | 0.0, 130.0 / 0.0, 130.0 | 3.0 / 3.0 | no | 9.20 |
| transform(l, Q) | latency | 12.51 [0.09] / 12.49 [0.05] / 12.48 [0.12] | 12.31 [0.06] / 12.32 [0.08] / 12.30 [0.07] | 229.1 / 225.0 | 74.1 / 73.0 | 0.0 / 0.0 | 0.0, 130.0 / 0.0, 130.0 | 3.0 / 3.0 | smallest size only (reference) | 15.17 |
| transform(g, Q) | throughput | 7.36 [0.04] / 7.37 [0.07] / 7.37 [0.03] | 7.41 [0.12] / 7.42 [0.03] / 7.41 [0.03] | 160.0 / 160.1 | 43.7 / 44.0 | 0.0 / 0.0 | 0.0, 84.0 / 0.0, 84.0 | 2.0 / 2.0 | no | 7.63 |
| transform(g, Q) | latency | 7.31 [0.02] / 7.31 [0.03] / 7.32 [0.02] | 9.65 [0.04] / 9.66 [0.03] / 9.65 [0.04] | 146.0 / 147.1 | 43.4 / 57.3 | 0.0 / 0.0 | 0.0, 84.0 / 0.0, 84.0 | 1.0 / 2.0 | yes, galoot faster | 11.73 |
| compose(Q2, Q1) | throughput | 9.79 [0.36] / 9.94 [0.32] / 9.95 [0.59] | 7.64 [0.02] / 7.64 [0.02] / 7.64 [0.33] | 211.0 / 190.0 | 58.0 / 45.3 | 1.0 / 0.0 | 0.0, 88.0 / 0.0, 88.0 | 3.0 / 4.0 | yes, reference faster | 8.81 |
| compose(Q2, Q1) | latency | 10.52 [0.18] / 10.52 [0.17] / 10.53 [0.19] | 7.78 [0.24] / 7.87 [0.21] / 7.78 [0.22] | 207.0 / 184.1 | 62.4 / 46.0 | 1.0 / 0.0 | 0.0, 88.0 / 0.0, 88.0 | 3.5 / 4.0 | yes, reference faster | 9.90 |

### Clang 23

| Operation | Loop | Galoot ns | Reference ns | Instructions | Cycles | Calls | Packed / scalar double | Store-forward blocks | Difference? | First run, reference ns |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| p ∧ q | throughput | 1.38 [0.01] / 1.38 [0.00] / 1.38 [0.01] | 1.44 [0.00] / 1.44 [0.00] / 1.44 [0.01] | 41.0 / 41.0 | 8.1 / 8.4 | 0.0 / 0.0 | 9.0, 0.0 / 9.0, 0.0 | 0.0 / 0.0 | yes, galoot faster | 1.44 |
| p ∧ q ∧ r | throughput | 2.65 [0.02] / 2.64 [0.02] / 2.64 [0.01] | 2.89 [0.01] / 2.89 [0.01] / 2.89 [0.01] | 75.8 / 73.8 | 15.6 / 17.1 | 0.0 / 0.0 | 0.1, 37.7 / 21.0, 0.0 | 0.0 / 0.0 | yes, galoot faster | 2.89 |
| l ∨ g | throughput | 1.47 [0.01] / 1.47 [0.01] / 1.47 [0.01] | 1.62 [0.01] / 1.62 [0.01] / 1.62 [0.01] | 45.0 / 48.9 | 8.6 / 9.5 | 0.0 / 0.0 | 6.0, 7.9 / 0.1, 19.8 | 0.0 / 0.0 | yes, galoot faster | 1.62 |
| transform(p, Q) | throughput | 7.72 [0.02] / 7.72 [0.03] / 7.72 [0.03] | 5.64 [0.02] / 5.63 [0.01] / 5.63 [0.02] | 166.1 / 161.5 | 45.8 / 33.1 | 1.0 / 0.0 | 0.0, 84.0 / 10.2, 67.5 | 2.0 / 0.0 | yes, reference faster | 7.51 |
| transform(p, Q) | latency | 11.09 [0.03] / 11.08 [0.03] / 11.09 [0.03] | 6.66 [0.03] / 6.65 [0.03] / 6.64 [0.03] | 165.0 / 147.0 | 65.8 / 39.4 | 1.0 / 0.0 | 0.0, 84.0 / 0.0, 84.0 | 1.7 / 0.0 | yes, reference faster | 11.91 |
| transform(l, Q) | throughput | 10.19 [0.04] / 10.19 [0.05] / 10.19 [0.03] | 8.85 [0.05] / 8.84 [0.02] / 8.84 [0.01] | 272.1 / 258.1 | 60.0 / 51.8 | 2.0 / 0.0 | 10.0, 112.0 / 42.2, 55.6 | 3.0 / 0.0 | yes, reference faster | 9.37 |
| transform(l, Q) | latency | 16.15 [0.05] / 16.14 [0.05] / 16.14 [0.05] | 11.38 [0.04] / 11.38 [0.04] / 11.39 [0.04] | 271.0 / 243.0 | 95.8 / 67.5 | 2.0 / 0.0 | 10.0, 112.0 / 43.0, 53.0 | 2.8 / 0.0 | yes, reference faster | 15.62 |
| transform(g, Q) | throughput | 5.74 [0.03] / 5.73 [0.01] / 5.73 [0.02] | 5.46 [0.02] / 5.46 [0.01] / 5.46 [0.07] | 167.1 / 151.6 | 33.5 / 32.1 | 1.0 / 0.0 | 21.0, 48.0 / 21.2, 47.6 | 0.0 / 0.0 | yes, reference faster | 5.73 |
| transform(g, Q) | latency | 9.41 [0.03] / 9.41 [0.03] / 9.41 [0.06] | 6.53 [0.03] / 6.54 [0.02] / 6.53 [0.04] | 166.0 / 143.0 | 55.9 / 38.7 | 1.0 / 0.0 | 21.0, 48.0 / 0.0, 84.0 | 0.0 / 0.0 | yes, reference faster | 9.38 |
| compose(Q2, Q1) | throughput | 8.90 [0.17] / 9.00 [0.18] / 8.87 [0.20] | 5.51 [0.01] / 5.51 [0.01] / 5.51 [0.04] | 196.1 / 191.3 | 52.6 / 32.2 | 1.0 / 0.0 | 13.0, 64.0 / 21.2, 45.6 | 1.0 / 0.0 | yes, reference faster | 6.00 |
| compose(Q2, Q1) | latency | 12.52 [0.37] / 12.53 [0.20] / 12.53 [3.60] | 6.93 [0.03] / 6.92 [0.03] / 6.92 [0.02] | 200.0 / 170.0 | 74.3 / 40.9 | 1.0 / 0.0 | 13.0, 64.0 / 29.0, 32.0 | 3.0 / 0.0 | smallest size only (reference) | 8.68 |

### The criterion

Strict reading: 16 of the 22 loops show a difference at every environment size. Weaker reading: 18, which adds Clang compose latency (its range at size 2768 is 3.60 ns, from one slow run) and GCC transform(l, Q) latency (0.20 ns, beyond the criterion at sizes 0 and 1352 only). Under the strict reading, Galoot is slower in 10 loops and faster in 6. No order reversed between sizes. A median moved by at most 0.15 ns across the sizes.

### Clang: throughput and latency read apart (review F1 of the inlining unit)

The static loops show the inlined reference's throughput loop vectorized across iterations (`part-d-inline/README.md`: 48, 72, 48, 48 `mulpd`). The retired counts show how much of that runs.

- **transform(p, Q).** The reference retires 10.2 packed and 67.5 scalar per operation in the throughput loop, against 0 and 84 in its latency loop. So about a quarter of its throughput work runs in the cross-iteration vector loop (inferred from the counts, assuming 42 packed per operation there and 84 scalar in the rest). Its throughput gap (2.08 ns) therefore includes vectorization. Its latency gap (4.43 ns, 26 cycles) does not: both forms retire 84 scalar and no packed instructions there.
- **transform(l, Q), transform(g, Q), compose.** The reference's throughput loop retires no more packed instructions than its latency loop: line 42.2 against 43.0, compose 21.2 against 29.0. For the plane, the throughput loop retires 21.2 packed against the library's 21.0. So the cross-iteration vector loop does not carry these operations at run time. Their packing is within one operation (straight-line SLP), as the library's is. Why the vector loop does not run was not traced.
- **Where the gap comes from.** On Clang, then, the gap is mostly the library's call, not the reference's vectorization:
  - Each Clang transform and compose makes 1 call per operation (2 for the line, whose `transform` calls its first stage's `run_product`). The reference makes none.
  - The result comes back through memory: 1.7 to 3.0 store-forward blocks per operation in Galoot's latency loops, against 0 for the reference.
  - Latency gaps: 4.43 ns for the point, 4.77 for the line, 2.88 for the plane, 5.60 for compose. That is 67%, 42%, 44%, and 81% of the reference's time.
  - Throughput gaps: 2.08, 1.35, 0.27, and 3.39 ns (37%, 15%, 5%, 62%).
  - E1's table (`wu17-asm/README.md` at `c2f181d`, "Calls found" and "Calls in the benchmark programs") gives the instructions behind the calls:
    - for the point and the plane, a tail jump to an out-of-line `galoot::core::transform` of 148 and 149 instructions;
    - for the line, `transform` calling `run_product<first_kernel>` through a 72-byte frame with the stack protector's check: 251 instructions against 236 inlined;
    - for compose, a call of `run_product<product_kernel>`, with its eight results through an 80-byte frame and four sign flips: 199 instructions against 166 inlined, plus one padding lane of the SLP vectorizer.

### GCC: like for like on the transforms

GCC inlines the library's transforms. The two forms retire the same double arithmetic in every transform loop (54 scalar and 19 packed, 130 scalar, or 84 scalar per operation), and neither makes a call.

### Each Galoot-slower row and its instruction-level cause

| Row | Gap | Cause |
| --- | --- | --- |
| GCC p ∧ q, throughput | 4.76 against 1.53 ns | Galoot's loop has 2 store-forward blocks and 14 more instructions per operation. It retires 3 packed and 12 scalar against the reference's 9 packed: the same 18 arithmetic lanes. E1's probe of the operation alone (46 instructions, rule K's 12 multiplications and 6 subtractions, 4 negations) shows no extra arithmetic, so the extra instructions and stalls come from how the timed loop handles the result. Inferred from the first run's disassembly: the conversion `Line{p ^ q}` permutes and negates the result through the stack in 8- and 16-byte stores, which the checksum reloads in 16-byte loads. |
| GCC compose, throughput and latency | 9.79 against 7.64 ns; 10.52 against 7.78 ns | One call per operation: GCC keeps `run_product<product_kernel>` out of line in the benchmark program (E1, "Calls in the benchmark programs": 159 instructions), though it inlines it in E1's probe. The double arithmetic is the same (88 scalar per operation); Galoot retires 21 to 23 more instructions, and the cycles rise by 13 to 16. |
| GCC transform(l, Q), latency (smallest size only) | 12.51 against 12.31 ns | Same arithmetic and store-forward counts; 4 more instructions and 1.1 more cycles per operation. Not traced. |
| Clang transform(p, Q), (l, Q), (g, Q), compose, both loops | above | The library's call (above). For transform(p, Q) throughput, partly also the reference's cross-iteration vectorization. |

### Galoot faster

- **GCC transform(p, Q) and transform(g, Q), latency** (7.41 against 9.88 ns, 7.31 against 9.65 ns). Both forms are inline with the same arithmetic. The reference takes 14 to 15 more cycles per operation for 1 to 5 more instructions. Inferred, not traced: the reference kernel's term order makes a longer dependence chain from one result to the next.
- **GCC p ∧ q ∧ r** (2.84 against 5.53 ns): the reference has the 2 store-forward blocks here.
- **Clang p ∧ q, p ∧ q ∧ r, l ∨ g** (0.06 to 0.24 ns): fewer cycles per operation.

### Against the first run

Inlining the reference changed the reference's times only:

- Clang transforms and compose: 0.3 to 5.3 ns faster.
- GCC compose: 1.2 to 2.1 ns faster.
- GCC transforms: about the same in throughput, 2.1 to 3.3 ns faster in latency.

Galoot's medians moved by at most 0.2 ns, as expected, since none of its code changed (review of the inlining unit).

## For decision 0237 item 3 (O3)

With the reference inline, the library's out-of-line calls cost beyond the criterion at every environment size:

- **Clang:** all three transforms and compose, in both loops (compose latency at two of the three sizes);
- **GCC:** compose.

The arithmetic each side retires is the same or close, so the cost is the call and the memory round trip of its result. On the strict reading this meets the condition of decision 0233 item 3 for a core inlining unit, which decision 0237 item 3 puts to the owner.

## Files

- `drive.py`, `batch.sh`: the rerun's driver and batch script (the protocol section gives the changes from the first run's).
- `out/rows.tsv`: every process. `out/runs.tsv`: the campaign log. `out/protocol.log`: the readings.
- `out/results.tsv`, `out/comparison.tsv`, `out/verdict.tsv`, `out/stalls.tsv`: the summaries.
- `runs.tsv`: the configure, build, and check runs.
- `first-run-out-of-line/`: the first run.
- `part-d-inline/`: the inlining unit's record.

Verification: the two forms' checksums agree in every pair. The reference kernels are verified by part D's smoke tests, and their inlining by `rga3.benchmark_<operation>_inline`. Validation: none applies.
