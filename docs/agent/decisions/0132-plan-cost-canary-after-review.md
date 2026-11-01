# 0132 The canary of the plans' costs after its review: one test per unit, checked above on GCC, eight units

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), performance-engineer, fixing the W5 review of plan 0015 (`records/reviews/2026-11-01-plan-limits-certified.md`, findings F1 to F5, F8). Supersedes decision 0131 items 3, 5, and 6 where they differ; 0131 is not rewritten.

## Decisions

1. **On GCC a pass is confirmed above the threshold (F1).** GCC's pass or fail is not monotone in its limit, so a unit whose count rose can compile at exactly the recorded threshold; the reviewer showed it with the stand-in (`fake_compiler.py 1005 evades=1000`) and with a real one-statement mutant of the wedge rule on antiwedge `full<5>`. `test/tools/plan_cost_model.py` now requires a GCC unit to compile at its threshold, to fail one below with the limit's message, and to compile at T + 1, 2, 3, 7, 31, 127, T/1000, T/100, and T/10 (the reviewer's set; the harness's T/10000 point is left out, see item 4). Clang keeps two compiles: its count is one step per statement and stops at the limit, so its pass and fail are monotone. A failure one below with GCC's not-a-constant-expression message for a built-in's argument is named as such and searched (F4). Both cases have a stand-in test: `tools.plan_cost_model_evades_at_threshold` and `_builtin_below`.
2. **Registered where the counts hold (F2).** The tests and the units' object library exist only in builds whose `CMAKE_CXX_FLAGS` define `_GLIBCXX_ASSERTIONS`, and on GCC only without `-fsanitize`. That keeps them in the seven development presets except gcc-asan, and out of the four measure presets, which build at -O2 or -O3 without the assertions and where every unit counts more (the reviewer: Clang `gp_full5` 770,687 against 655,239; GCC 3,704,610 against 3,611,426, which I measured too).
3. **Eight reference units (F3).** Two units are added for paths that decide stated figures and that no unit took:

   | Unit | Path | Clang steps | GCC operations, cold |
   | --- | --- | --- | --- |
   | `plan_cost_antiwedge_full5.cpp`, antiwedge, `full<5>` twice, R(5,0,0) | the wedge rule of `pair_terms`, in the fill | 271,653 | 1,393,084 |
   | `plan_cost_gp_vectors_full6.cpp`, gp, the vectors times `full<6>`, R(6,0,0) | `collect_result`'s shift loop (252 shifts) | 474,959 | 2,415,778 |

   Each is a stated figure or the model's prediction in `figures-*.txt` (rows `antiwedge-r-5`, `gp-vectors-full-6`), and each passes the GCC check of item 1. The case for the shifts was chosen among plans of graded sets because its largest evaluation is `collect` on both compilers with a margin of 7.8 % over the next, it costs under a second a compile, and its first factor is not the scalar, so result blades arrive out of order.
4. **One CTest per unit (F5).** `core.plan_cost_model.<unit>`, labels `core;core.plan`; `ctest -R '^core\.plan_cost_model\.'` runs all eight. They run in parallel from the shared pool and a failure names its unit. Measured at `9b70dc8`, the eight tests together took 1.1 s on clang-debug and 4.7 s on gcc-debug (the GCC tests of the largest units, `gp_full5` and `gp_grades_3_7`, take 4.4 s to 4.7 s each with eleven compiles). The reviewer's timings of the single test of `26cc06e` were 3.98 s to 6.83 s over 27 runs, over 5 s in 8 (review section 4). No `slow` label: no single test exceeded 5 s in my runs, and the suite's critical path is now one GCC unit, not twelve compiles in sequence; under load a GCC test may still pass 5 s, which is reported, not bounded.
5. **The units are compiled as an ordinary library (F8).** `core_plan_cost_units`, an OBJECT library of the eight units linked to `galoot::core`, in every build that registers the tests. So the warning flags, `-Werror`, clang-tidy (lint), and the compile-cost launcher see them, and each compiles at the default limits. It is not built where the tests are not registered, because outside the calibrated configuration a unit is not known to fit the default limits.

## Consequences

- Decision 0131's consequences stand, with "the canary" read as these eight tests. A change to the antiproduct pair rule now also moves `plan_cost_antiwedge_full5.cpp`'s fill, so that threshold joins the list to regenerate.
- The mutation driver (`records/mutations/0015-plan-limits-certified/mutate.py`) runs `^core\.plan_cost_model\.` and has the reviewer's two surviving mutants, `wedge_branch_step` and `collect_shift_step`.
