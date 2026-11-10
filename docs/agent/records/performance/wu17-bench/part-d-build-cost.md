# Build cost of the benchmark targets (plan 0038, part D)

Recorded by Claude (agent, Opus 5.5), performance-engineer role, 2026-11-10, on `feature/cost-evidence--bench` at `a28504a` (the mutation-records commit after `e490570`, which added the harness and programs; its code is that of `e490570`). Toolchain GNU 16.2.1 and Clang 23.1.4, machine i9-14900K. P1 record of the new translation units, not a W10 record: nothing under `include/` changed.

## What was built and how

The ten new translation units under `test/bench/`, in gcc-debug, clang-debug, gcc-measure and clang-measure: the smoke test `reference_kernels_test.cpp`, the seven `*_bench.cpp` programs, and the header checks of `reference_kernels.hpp` and `harness.hpp`. Their objects were deleted and rebuilt through `tools/timed build <preset> bench-targets-cost -- cmake --build build/<preset> --target <the ten targets>`, from the shared job pool (23 of 23 slots free before, load average 3.9), so each row's mode is `shared`: several compiles ran at once and the times are not pinned (not comparable with pinned rows). The rows are `part-d-compile.tsv`, copied from each tree's `compile.tsv` (the launcher `tools/measure tu`), filtered to the revision above and to `test/bench/`.

## Figures

| Preset | TUs | Sum of compile wall time (s) | Largest TU (s) | Largest peak memory (MiB) |
| --- | ---: | ---: | ---: | ---: |
| gcc-debug | 10 | 10.49 | 1.52 (`compose_motors_bench.cpp`, `reference_kernels_test.cpp`) | 327 |
| clang-debug | 10 | 9.92 | 1.42 (`wedge_point_point_point_bench.cpp`) | 274 |
| gcc-measure | 10 | 16.99 | 2.63 (`wedge_point_point_bench.cpp`) | 324 |
| clang-measure | 10 | 12.90 | 2.00 (`transform_plane_bench.cpp`) | 270 |

Each benchmark program instantiates one or two rga3 operations over double and the harness; the header check of `reference_kernels.hpp` costs about 0.05 s and 46 to 91 MiB. Against the about 207 s of compile time per debug compiler of the whole suite at `c799821` (plan 0038 section 11, O5), the ten units add roughly 5 % (inferred: that figure is from an earlier revision and from other runs of the shared pool).

Every preset builds these targets (decision 0233, O8 option a), so every configure and full build of a preset pays them; the eight-preset matrix pays about 8 times 10 to 17 s of compile time, spread over the pool.
