# What transform's rounding check costs: build and run

WU13b (plan 0021), measured by the WU13b implementer (Claude, agent, Opus 5.5). Toolchain: g++ 16.2.1 20260810 and clang++ 23.1.4 with libstdc++ 16, on the development machine.

## Method

`measure.py 01835b7 <out> 3 4 10000000` at `c6e1f3c` (the final revision of the build inputs is `cb04901`; later commits change records only), with a clean tree, under the shared timing lock (`lock.sh take`), with the job pool idle (23 of 23 slots free) and no other build running. Every compile and run is pinned to CPU 4, a performance core, through `tools/timed measure` (rows `transform-rounding-cost:*` in `runs.tsv`, 64 rows at `c6e1f3ce3120`), three repeats interleaved; the figures are the instructions retired on that core, mean of three; the spreads and every row are in `out/summary.txt` and `out/rows.tsv`. The sibling thread of CPU 4 was idle throughout (`sibling_busy_s` 0.00 in every row).

- Build: the gcc-debug and clang-debug test flags with `-c`, as WU13a's `transform-cost/measure.py`; "before" is `feature/core-transform` at `01835b7`, where WU13b branched. `transform_rounding_test.cpp` also compiled with `GALOOT_CHECKING=0`.
- Run: `records/spikes/transform-rounding/cost.cpp`, built at `-O2 -ffp-contract=off` with `GALOOT_CHECKING` 1 and 0, 10^7 iterations of a rigid(3) point moved by a motor and a conformal(3) vector reflected in a plane, inputs read at run time.

## Build cost (W10, P1)

| Unit | GCC before | GCC after | Clang before | Clang after |
| --- | --- | --- | --- | --- |
| `smoke_test.cpp` | 1,160.9 M | 1,164.4 M (+0.3%) | 1,073.2 M | 1,077.8 M (+0.4%) |
| a unit that includes only `core.hpp` | 962.9 M | 966.7 M (+0.4%) | 974.2 M | 975.9 M (+0.2%) |
| `transform_test.cpp` | 125,284.9 M | 125,063.7 M (−0.2%) | 111,827.9 M | 111,692.7 M (−0.1%) |
| `transform_rounding_test.cpp`, checking build | | 47,760.7 M, 3.5 s, 526 MiB | | 33,454.5 M, 2.5 s, 365 MiB |
| the same, `GALOOT_CHECKING=0` | | 47,764.2 M | | 33,385.7 M |

The check adds about 4 M instructions (0.3%) to every includer of `core.hpp`. `transform_test.cpp`, which uses no rounding scalar, changes by less than the spread of its repeats (range 211 M and 352 M before). `transform_rounding_test.cpp` costs the same in both modes, since its constant evaluations always check and the checking path is compiled either way. It is far from the suite's largest unit (`transform_identities_test.cpp`, 402 G on GCC), runs in under a second, and is not labeled `slow`.

## Run cost of checking mode

| Program | GCC instructions | wall | Clang instructions | wall |
| --- | --- | --- | --- | --- |
| `cost`, `GALOOT_CHECKING=0` | 2,292.6 M | 0.13 s | 2,322.6 M | 0.12 s |
| `cost`, `GALOOT_CHECKING=1` | 24,472.6 M (10.7×) | 3.43 s (26×) | 21,882.6 M (9.4×) | 3.39 s (28×) |

Checking mode makes these transforms about ten times as many instructions and about 27 times as slow. Plan 0021 asks the cost to be measured and reported, not bounded, and no bound is set (C14). The cause was not profiled. Inferred from the code, not measured: the padded magnitude runs as a loop over the kernel's terms, with its branches, where the value kernels are unrolled straight-line code, and each dropped coefficient's γ is a division at run time, though K is known at compile time; computing the γ factors at compile time and unrolling the magnitude kernel as `run_product` does would remove most of that, if the cost is found to matter.
