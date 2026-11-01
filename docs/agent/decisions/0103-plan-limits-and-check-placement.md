# 0103 The measured limits of gp_plan, the first P2 record, and where the exhaustive checks run

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), implementing WU6b under plan 0011 (criteria 14 to 16).

## Context

Plan 0011 section 7 keeps every exhaustive proof at run time, with the same `constexpr` generator, and the compile-time checks to tables of n ≤ 4 and the typed and sparse pairs (note 0008, recommendation 1; C12). Criterion 14 asks for the default limits of `gp_plan` on both compilers in three configurations, stated in its `///` comment as `full` and `merge` state theirs; criterion 16 asks for the first P2 record; criterion 15 for one evaluation per argument set.

## Measurements

All by `docs/agent/records/performance/plan-growth/measure_plan.py`, each compile through `tools/timed` pinned to CPU 6 (a performance core), rows in `runs.tsv` with selections `plan-<mode>:...`, summaries in `out/`. Header at `bb36c7e` throughout; script at `e379a4b` for limits, growth, and locals, and `92062e3` for evals. GCC 16.2.1, Clang 23.1.4, libstdc++ 16. Instructions are both kinds of core added; shared machine (load from other work), so wall times are indicative only.

### Default limits (criterion 14)

Three configurations: Galoot's test flags (`-Wall ... -Werror -ffp-contract=off -D_GLIBCXX_ASSERTIONS`) at -O0 with checking and at -O2 without, and a bare `-std=c++23 -pedantic-errors`. Each unit compiled with `-c`. `out/limits.tsv`.

| Unit | Clang, all three | GCC, all three |
| --- | --- | --- |
| Typed rigid(3) pairs: point × point, line × plane, motor × point, point × motor, line × line | builds | builds |
| conformal(3) vectors × vectors (25 terms) | builds | builds |
| Sparse {1, e1, e_n, e1n} × {e1, e2, e_n} in R(n−1,0,1), n = 8, 12, 16 | builds | builds |
| `full<n>` × `full<n>`, R(n,0,0), R(n−1,0,1), conformal(n−2) | n ≤ 5; n = 6 hits the step limit | n ≤ 7; n = 8 exceeds `-fconstexpr-ops-limit` (2^25) |
| `full<n>` × `full<n>`, dense form (2 on the diagonal, 1 elsewhere) | n ≤ 4; n = 5 hits the step limit | n ≤ 5; n = 6 exceeds the ops limit |

The three configurations agree at every point. Note 0011 finding 5 found Clang -O2 failing an n = 5 full plan that -O0 passed, because libstdc++ adds a call per checked access when `__OPTIMIZE__` is defined; here n = 5 passes at -O2 too. Inferred cause, not traced: this generator's scratch is `plan_capacity`, at most 2^(n−1), where Spike D's was 2^n, which leaves Clang more room under 2^20 steps. Galoot's own test flags define `_GLIBCXX_ASSERTIONS` at every level, and libstdc++ 16 defines it by default without `__OPTIMIZE__`, so the bare -O0 configuration has the assertions as well (note 0011); the bare figure is the one a user's default build faces.

### One evaluation per argument set (criterion 15)

Clang `-ftime-trace -ftime-trace-granularity=0` (`out/evals.tsv`): `test/core/plan_test.cpp`, which uses `gp_plan<rga3, point, point>` in two functions and in a `static_assert` among 18 distinct argument sets, shows 18 instantiations of `make_gp_plan`, 18 evaluations of `gp_plan`'s initializer, and 18 fills; a unit with two uses of one plan in two functions and one other plan shows 2, 2, 2. The fill is counted only within an instantiation of `make_gp_plan`, because the test has variables of its own named `plan`. With the plain `constexpr` locals of the first version, each evaluation of `gp_plan`'s initializer generated the plan again (decision 0102 item 7).

### First P2 record (criterion 16)

Galoot's test flags at -O0, `-c`, three repeats (instruction spread under 0.35%), `out/growth.tsv`. Each "plans" figure is the unit minus a baseline unit with the same algebra, sets, and includes and no plan.

Fixed use as n grows: point × point, line × plane, motor × point, and point × motor of rigid(3) embedded in R(n−1,0,1), e4 becoming e_n (86 terms at every n).

| n | Clang baseline | Clang plans | GCC baseline | GCC plans |
| --- | --- | --- | --- | --- |
| 4 | 0.748 G | 0.345 G | 0.807 G | 0.186 G |
| 8 | 0.752 G | 0.375 G (+9%) | 0.811 G | 0.195 G (+5%) |
| 16 | 0.771 G | 0.438 G (+27%) | 0.826 G | 0.215 G (+16%) |

Peak memory: Clang 111 MiB baseline, 117 MiB with the plans, at every n; GCC 70 to 72 MiB baseline, 76 MiB with them.

Distinct plans at n = 4, of the point, line, plane, and motor sets: 1 plan (point × point) 0.127 G on Clang and 0.045 G on GCC; 4 plans (the four pairs above) 0.347 G and 0.186 G; 16 plans (every pair of the four sets: 22 × 22 pairs of blades less the 11 × 11 that share e4, 363 terms) 1.389 G and 0.656 G. Peak memory rises from 111 to 118 MiB on Clang and from 70 to 83 MiB on GCC.

Growth at fixed use. The cost of the same 86 terms rises by 27% on Clang and 16% on GCC from n = 4 to 16. PERFORMANCE P2 records growth with n at fixed use as a defect unless it follows from the algebra's own structure. The likely cause is the generator, not the algebra (inferred, not measured): `add_generator_product` and the correction in `add_blade_product` (`blade_product.hpp`) loop over all N generators for each term and skip the ones the blade lacks, so each term costs O(N) steps where O(grade) would do; the contraction needs g(e_i, e_j) only for the j in the blade. It is recorded here as a defect for the owner and blocks nothing (decision 0005). It is not fixed in this unit, because the loops are WU6a's reviewed code and a change there needs its own measurement, mutation rerun, and review; the check that would confirm the cause is a copy of the header whose loops visit only set bits, measured at n = 4 and 16.

## Decision

1. The `///` comment of `gp_plan` states the measured limits: `full<n>` with itself builds up to n = 5 on Clang 23 and n = 7 on GCC 16 in R(n,0,0), R(n−1,0,1), and conformal(n−2), up to 4 and 5 in a form with every entry nonzero, and sets of a few blades build at n = 16; alike at -O0 and -O2 and with or without `_GLIBCXX_ASSERTIONS`. Like `full` and `merge`, the figures name the compiler versions measured.
2. Where the checks run. At run time, with the same generator: the plan of every blade times every blade of the 42 signatures of plan 0011 section 5 (n ≤ 6), against `blade_product` of each pair, with +1 signs and with two sign patterns, and the V4 properties of those plans (the `core.plan` test cases). At compile time, in `static_assert`s: the typed rigid(3) pairs (n = 4), the sparse pairs at n = 8, 12, and 16, `full<4>` × `full<4>` in rigid(3), R(4,0,0), and R(1,3,0) against a direct `generate_gp` call, and the post-generation checks of every plan any unit uses. Every compile-time check is under the measured limits on both compilers with room: the largest, `full<4>` × `full<4>`, is one generator below the largest n that Clang builds for diagonal forms.
3. No preset raises a constant-evaluation limit.

## Consequences

- The multivector unit (WU8) and the result typing (WU7) can rely on plans of named types at n up to 16; a program that asks for `full<6>` × `full<6>` or a dense `full<5>` table at compile time hits the Clang limit and has to narrow its sets.
- The fixed-use growth above is open for the owner: fix the loops in `blade_product.hpp` by a decision with the confirming measurement, or record it as accepted.
- The P2 grid of DoD-23 (more sizes and operations) stays with WU17; this is its first point.
