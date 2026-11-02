# 0131 The canary of the plans' documented costs, and the refit it rests on

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), performance-engineer, implementing plan 0015 under decision 0125 as the owner ruled it.

## Context

Decision 0125 (owner ruling) asks for the cost law and certified examples in the comments of `product_plan` and the map and antiproduct generators, a canary CTest `core.plan_cost_model`, and the predictor in `docs/agent/snippets/`. Plan 0015 left to the implementer the reference cases, the presets the canary runs in, its label, and how the refit is made. The records are in `docs/agent/records/performance/plan-cost-model/`.

## Decisions

1. **The refit.** The model was refitted on the headers of `4895a40` (the claim) for every phase of both generators, not only the generation phases, so that all weights come from one revision; the weights of the phases whose code did not change since `e1af2ac` are a check of that claim. `mirror.py` gained the antiproducts (`factor_of`, `run_term`, `pair_capacity` with n) and the unary maps. A map's result set is collected by `collect_result`, the plan's function, so it shares the `collect` phase and its weights; the probes of both generators train it together.
2. **Mirror fidelity is checked, not only inferred from the fit.** `probe.py verify` compiles each training, held-out, and whole case once with a `static_assert` that the C++ result equals the mirror's.
3. **Reference cases** (each the smallest of its kind that makes the intended phase the unit's largest evaluation on both compilers, or on one where noted):

   | Unit | Largest evaluation, Clang / GCC | Clang steps | GCC operations, cold |
   | --- | --- | --- | --- |
   | `plan_cost_gp_full5.cpp`, gp, `full<5>` twice, R(5,0,0) | collect / fill | 655,239 | 3,611,426 |
   | `plan_cost_gp_dense4.cpp`, gp, `full<4>` twice, 2 on the diagonal and 1 elsewhere | fill / fill | 681,823 | 3,645,825 |
   | `plan_cost_gp_grades_3_7.cpp`, gp, trivectors times 7-vectors of R(8,0,0) | typed / typed | 601,471 | 4,235,775 |
   | `plan_cost_antigp_sparse16.cpp`, antigp, the sparse sets of R(15,0,1) | fill / fill | 77,513 | 443,211 |
   | `plan_cost_reverse_full7.cpp`, the reverse of `full<7>` | collect / collect | 313,635 | 1,577,666 |
   | `plan_cost_right_complement_full8.cpp`, the right complement of `full<8>` | map typing / map typing | 813,090 | 5,717,129 |

   Together they make the generation of a product's and of an antiproduct's terms, the collection of the result blades, the typing of a plan, and the typing of a map each the largest evaluation of some unit on each compiler (on Clang, fill of the geometric product only in the dense case). The grade case exists because on GCC a plan's typing is the largest evaluation of some inputs (31 of the 485 distinct inputs measured, up to 2.2 × 10⁷ operations); on Clang it is the largest only for inputs below 4 × 10⁴ steps.
4. **Phases no reference pins.** Over all 485 distinct inputs of the refit, the largest evaluation was fill, collect, typed, or the map's typing, except on Clang one map whose largest was `map_defects` (671 steps) and one whose largest was `map_fill` (500 steps). `count` runs the same code as `fill` without the writes, so it is never above it; `capacity`, `defects`, and `typing` are single passes with less work per pair or term than `fill` or `typed`. A change to one of these phases changes no figure stated in the comments and is outside the canary's reach; the weights' self-check (item 7) still sees it at the next refit.
5. **Presets.** The canary compiles with `galoot_compile_command`, the flags each preset's tests use. Its thresholds were met exactly on clang-debug, clang-release, clang-asan, lint, gcc-debug, and gcc-release (all at `22db6bc`), so `-O` level, `GALOOT_CHECKING`, and Clang's sanitizers do not change the counts. gcc-asan counts more: 3,740,515, 3,808,941, 4,334,561, 451,792, 1,643,207, and 5,849,995 for the six units (+2.2 % to +4.4 %), because GCC's sanitizer instrumentation is also evaluated. The canary is therefore registered on Clang in every preset and on GCC in every build without `-fsanitize`. Recording a second set of GCC thresholds for the sanitizer build was rejected: the comments certify the configuration without sanitizers, and a second set would guard figures that no comment states.
6. **No `slow` label.** The canary compiles twelve units in sequence; its wall time was 4.3 s (gcc-debug) and 4.7 s (clang-debug) at `22db6bc` on a loaded machine, under plan 0015's criterion of 5 s. It compiles outside the compile-cost launcher, because each unit is compiled twice and the copy step of the compile record refuses a source compiled twice.
7. **The snippet's self-check.** `cost.py selfcheck` refits from the committed training files and compares every weight and training row; predicts every committed measurement and compares it exactly; and compares a fresh `figures` run with the committed `figures-<compiler>.txt`. The canary's thresholds are not read from these files: they are ported values in `test/CMakeLists.txt`, as decision 0125 item 2 requires.
8. **The canary's script decides a failure at the threshold by compiling under the largest limit.** GCC does not always name the limit when its count runs out: inside the argument of a built-in function it reports that argument as not a constant expression (seen on gcc-asan). So a unit that fails at its threshold and compiles under the largest limit is reported with its measured threshold, and one that compiles under no limit is reported as failing for another reason.

## Consequences

- A compiler, standard-library, or generator change that moves a reference's largest phase fails `core.plan_cost_model` with the new threshold; the maintainer then refits (the commands are in the records' README), regenerates `figures-*.txt` and the comments' figures, and updates the thresholds.
- A change to the antiproduct pair rule (the work on `research/antiproduct-growth`) needs a refit of `capacity`, `count`, and `fill` and of `mirror.py`'s `pair_capacity`, `factor_of`, `run_term`, and `generate_plan`, and new antiproduct figures and the `plan_cost_antigp_sparse16.cpp` threshold; the typing and map phases are unaffected unless their code changes.

## Correction, 2026-11-01 (plan 0015 outcome)

Item 6's wall times, 4.3 s and 4.7 s, were measured just before `22db6bc` was committed, on the same files (run-log revision `809a975-dirty`), not at `22db6bc`. In the serial suite runs of the final trees at `1f7cf28` the canary took 4.83 s (gcc-debug) and 5.00 s (clang-debug); none exceeded the criterion of 5 s. The text above is left as recorded.

## Correction, 2026-11-01 (W5 review of plan 0015, findings F2 and F9)

- Item 4, last sentence: the self-check compiles nothing, so it cannot see a change to a generator. Such a change is seen only when the phase probes are measured again, as a refit does, and the weights then fail to predict them. The self-check does now fail when a measured threshold of a committed case file is missing or marked failed.
- Item 5: as built at `26cc06e`, the canary was also registered in the four measure presets (`clang-measure`, `clang-measure-o3`, `gcc-measure`, `gcc-measure-o3`), which build without `_GLIBCXX_ASSERTIONS` and where every unit counts more, so it failed there. Decision 0132 item 2 registers it only in builds that define `_GLIBCXX_ASSERTIONS`.
- Items 3 and 6 are superseded by decision 0132 (eight units, one test per unit).

The text above is left as recorded.
