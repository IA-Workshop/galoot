# What transform's rounding check costs: build and run

WU13b (plan 0021), measured by the WU13b implementer (Claude, agent, Opus 5.5). Toolchain: g++ 16.2.1 20260810 and clang++ 23.1.4 with libstdc++ 16, on the development machine.

## Method

`measure.py 01835b7 <out> 3 4 10000000` at `7912ea9`, after the blind review's fixes (the header is that of `e188522`; `bc06e9e` later changed only the compile-time comparison of the margin's hypothesis, which does not reach run time), with a clean tree, under the shared timing lock (`lock.sh take`), with the job pool idle (23 of 23 slots free) and no other build running. Every compile and run is pinned to CPU 4, a performance core, through `tools/timed measure` (rows `transform-rounding-cost:*` in `runs.tsv` at `7912ea97d908`), three repeats interleaved; the figures are the instructions retired on that core, mean of three; the spreads and every row are in `out/summary.txt` and `out/rows.tsv`. Then, under the same lock, each `cost` program ran once more under `perf stat -e cpu_core/assists.fp/,cpu_core/cycles/,cpu_core/instructions/`, pinned to CPU 4 (`out/assists/`; perf multiplexed these counters at about 90% of the run, so they are perf's scaled estimates). The first measurement, before the review, at `c6e1f3c`, is in the history of `out/`.

- Build: the gcc-debug and clang-debug test flags with `-c`, as WU13a's `transform-cost/measure.py`; "before" is `feature/core-transform` at `01835b7`, where WU13b branched. `transform_rounding_test.cpp` also compiled with `GALOOT_CHECKING=0`.
- Run: `records/spikes/transform-rounding/cost.cpp`, built at `-O2 -ffp-contract=off` with `GALOOT_CHECKING` 1 and 0, 10^7 iterations of a rigid(3) point moved by a motor and a conformal(3) vector reflected in a plane, inputs read at run time. Several of its inputs are zero, as for a motor without a translation or a point with z = 0.

## Build cost (W10, P1)

| Unit | GCC before | GCC after | Clang before | Clang after |
| --- | --- | --- | --- | --- |
| `smoke_test.cpp` | 1,161.0 M | 1,164.7 M (+0.3%) | 1,073.4 M | 1,078.9 M (+0.5%) |
| a unit that includes only `core.hpp` | 963.1 M | 967.1 M (+0.4%) | 974.9 M | 977.3 M (+0.2%) |
| `transform_test.cpp` | 125,263.4 M | 125,305.0 M (+0.0%) | 111,665.3 M | 111,791.7 M (+0.1%) |
| `transform_rounding_test.cpp`, checking build | | 49,718.0 M, 3.8 s, 544 MiB | | 34,264.7 M, 2.7 s, 375 MiB |
| the same, `GALOOT_CHECKING=0` | | 49,734.1 M | | 34,112.0 M |

The check adds about 4 M instructions (0.2% to 0.5%) to every includer of `core.hpp`. `transform_test.cpp`, which uses no rounding scalar, changes by less than the spread of its repeats. `transform_rounding_test.cpp` costs the same in both modes, since its constant evaluations always check and the checking path is compiled either way; it grew by about 4% with the review's new tests. It runs in under a second and is not labeled `slow`.

## Run cost of checking mode

| Program | GCC instructions | wall | FP assists | Clang instructions | wall | FP assists |
| --- | --- | --- | --- | --- | --- | --- |
| `cost`, `GALOOT_CHECKING=0` | 2,292.6 M | 0.14 s | 0 | 2,322.6 M | 0.13 s | 0 |
| `cost`, `GALOOT_CHECKING=1` | 24,472.6 M (10.7×) | 0.93 s (6.6×) | 0 | 21,882.6 M (9.4×) | 0.87 s (6.7×) | 0 |

Checking mode makes these transforms about ten times as many instructions and about 6.7 times as slow by wall time. The wall times include each run's fixed start-up, which weighs most on the short unchecked runs; by cycles the per-transform slowdown is about 9.5× (5.22 G against 0.54 G on GCC, 4.85 G against 0.51 G on Clang), which a reader comparing the cost per call should use. The delta review measured the same (9.7× and 9.4× by cycles). The instruction ratio is exact and the same as before the review. The first measurement found 26 to 28 times the wall time (3.43 s and 3.39 s), and attributed it, by inference, to the loop of the magnitude kernel and the run-time division of γ. That inference was wrong: the blind review measured 110,000,000 floating-point microcode assists in that run, 11 per iteration, from subnormal products of a padded magnitude near λ with an input below 1 where the inputs have zeros, about three quarters of its cycles. The wall-time ratio therefore depended on the inputs. With the padding λ/u, no padded product falls below λ for inputs of magnitude u or more, the assists are 0, and the wall time follows the instructions more closely (cycles 5.2 G and 4.9 G against 0.54 G and 0.51 G unchecked). Plan 0021 asks the cost to be measured and reported, not bounded, and no bound is set (C14). Computing the γ factors at compile time and unrolling the magnitude kernel as `run_product` does might cut the rest; not tried.
