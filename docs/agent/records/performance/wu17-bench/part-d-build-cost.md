# Build cost of the benchmark targets, and part D's departures (plan 0038, part D)

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

## Departures of part D (C20)

Recorded after the blind review `records/reviews/2026-11-10-wu17-part-d-blind.md` (branch `records/wu17-part-d-review`), findings D1, D2 and H1.

1. **One agent wrote the tests and the code (D1).** Plan sections 5 and 9 give part D's tests to the doctest-expert and the code to the performance-engineer with the build-expert. One agent (Claude, Opus 5.5) did all three in turn, as its brief directed. Decision 0185's form was kept (tests committed on `feature/cost-evidence--bench--tests` and cherry-picked with `-x` before their code, the failing commit labeled), but not its separation: the generator, whose output was the scratch stand-in the tests were compiled against, existed while the tests were written. The blind review checked the tests' power independently (its M1 and T1) and found them sound; whether that suffices as the independent check is the coordinator's call.
2. **The double tolerance was fixed in the test commit, not in the plan (D2).** Plan item 4 admits bitwise equality "where the term order is the same and the plan says so before the run"; the plan was not amended. The tolerance (zero: equal values for p ^ q, p ^ q ^ r, line v plane and compose, equal bits for the three transforms) and its derivation (the same products added in the same order; an orientation's sign moved from a sum into its terms changes only a zero's sign under rounding to nearest; `-ffp-contract=off` in every preset) are in the header comment of `test/bench/reference_kernels_test.cpp` and the message of `78a4458`. That commit came 28 s after the last stand-in run, so the record cannot show that the tolerance was fixed before a result was seen (V12); the claim rests on the author's statement. The review's probe on coefficients in {-1, 0, 1} confirms the derivation on other evidence, and the double cases now draw such coefficients too (`ff13308`).
3. **The harness is larger than decision 0014's "about 100 lines" (H1).** `test/bench/harness.hpp` has about 150 lines that are neither blank nor comment. The rest of the size is the `check` mode (the stale-benchmark guard run in every preset), the drawing of unitized motors (which keeps the latency loop bounded), the two loops, and argument parsing. Trimming the parsing would save perhaps 15 lines; it was not done.
4. **Run rows lost.** The run log rows of the stand-in compiles and runs for `ff13308`'s test change, about 13 rows in the tests worktree's uncommitted `runs.tsv` (step `compile` and `test`, preset `-`, selections `standin:*`, `standin-o2:*`, `mutant-sign-on-sum:*`, `old-test-mutant:g++`, 2026-11-10 about 14:03 to 14:04 UTC), were discarded by a `git checkout` of that file before they were copied here. They recorded scratch compiles only, not a gate or a committed revision.
