# Cost model of the plan and map generators, refitted for plan 0015

The calibration behind the figures in the `///` comments of `product_plan` and `unary_map` and the thresholds of the canary `core.plan_cost_model`. Method: research note 0017. Tools: `docs/agent/snippets/plan_cost_model/` (`mirror.py`, `probe.py`, `harness.py`, `cost.py`).

- Headers measured: `include/` of `develop` at `4895a40` (the claim of plan 0015), unchanged on the branch until the comment commit, which changes comments only.
- Compilers: Clang 23.1.4 (classic constant evaluator, its default) and GCC 16.2.1 20260810, with libstdc++ 16, on the development machine.
- Configuration `galoot-O0`: `-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -D_GLIBCXX_ASSERTIONS -O0 -DGALOOT_CHECKING=1`, `-fsyntax-only`. GCC with `-fconstexpr-cache-depth=1` (cold: no constant call is cached), Clang as it is (`warm` in the file names; Clang has no such cache).
- Every compile ran through `tools/timed measure` holding a slot of the shared pool; the selections are `ceiling-cert:<case>:<cache>:<compiler>:<config>:L<limit>` (the prefix `harness.py` has used since note 0017) and `cost-model-verify:<case>:<compiler>`.

## Files in `out/`

| File | Holds |
| --- | --- |
| `<set>-cases.tsv` | The cases of each set, written by `probe.py cases <set>` (seeds 20261102 and on) |
| `verify-<set>-<compiler>.tsv` | `probe.py verify`: each case compiled once with a `static_assert` that the C++ result equals the mirror's |
| `<set>-<compiler>-galoot-O0-<cache>.tsv`, `.log` | Thresholds by the checked search of `harness.py` |
| `weights-<compiler>-galoot-O0.tsv` | `cost.py fit` of the training sets (`train`, `train-gen`, `train-map`) |
| `check-<set>-<compiler>.txt` | `cost.py check` of every set against the weights |
| `figures-<compiler>.txt` | `cost.py figures`: the figures of the comments and the canary's thresholds |

## Commands

From the worktree root, with the pool exported (`eval "$(tools/jobpool env)"`), `S=docs/agent/snippets/plan_cost_model`, `R=docs/agent/records/performance/plan-cost-model/out`:

```bash
python3 -I $S/probe.py cases train $R/train-cases.tsv      # and train-gen, train-map, test, plans, maps, wide
python3 -I $S/probe.py verify $R/train-cases.tsv $R/verify-train-clang.tsv clang 16
python3 -I $S/probe.py run $R/train-cases.tsv $R/train-clang-galoot-O0-warm.tsv clang galoot-O0 warm 20
python3 -I $S/probe.py run $R/train-cases.tsv $R/train-gcc-galoot-O0-cold.tsv gcc galoot-O0 cold 20
python3 -I $S/cost.py fit $R/train-cases.tsv:$R/train-clang-galoot-O0-warm.tsv $R/train-gen-cases.tsv:$R/train-gen-clang-galoot-O0-warm.tsv $R/train-map-cases.tsv:$R/train-map-clang-galoot-O0-warm.tsv $R/weights-clang-galoot-O0.tsv
python3 -I $S/cost.py check $R/test-cases.tsv $R/test-clang-galoot-O0-warm.tsv $R/weights-clang-galoot-O0.tsv
python3 -I $S/cost.py figures $R/weights-clang-galoot-O0.tsv > $R/figures-clang.txt
```

The check that every count in `plan.hpp`'s comments is a figures row or a default limit:

- `python3 -I docs/agent/snippets/plan_cost_model/cost.py comments include/galoot/core/plan.hpp docs/agent/records/performance/plan-cost-model/out/figures-clang.txt docs/agent/records/performance/plan-cost-model/out/figures-gcc.txt`

The self-check, one line per compiler (about 80 s each; it exits 1 on any difference, including a case whose measured threshold is missing or failed):

- selfcheck-clang: `python3 -I docs/agent/snippets/plan_cost_model/cost.py selfcheck docs/agent/records/performance/plan-cost-model/out/weights-clang-galoot-O0.tsv docs/agent/records/performance/plan-cost-model/out/figures-clang.txt --train docs/agent/records/performance/plan-cost-model/out/train-cases.tsv:docs/agent/records/performance/plan-cost-model/out/train-clang-galoot-O0-warm.tsv docs/agent/records/performance/plan-cost-model/out/train-gen-cases.tsv:docs/agent/records/performance/plan-cost-model/out/train-gen-clang-galoot-O0-warm.tsv docs/agent/records/performance/plan-cost-model/out/train-map-cases.tsv:docs/agent/records/performance/plan-cost-model/out/train-map-clang-galoot-O0-warm.tsv --check docs/agent/records/performance/plan-cost-model/out/test-cases.tsv:docs/agent/records/performance/plan-cost-model/out/test-clang-galoot-O0-warm.tsv docs/agent/records/performance/plan-cost-model/out/plans-cases.tsv:docs/agent/records/performance/plan-cost-model/out/plans-clang-galoot-O0-warm.tsv docs/agent/records/performance/plan-cost-model/out/maps-cases.tsv:docs/agent/records/performance/plan-cost-model/out/maps-clang-galoot-O0-warm.tsv docs/agent/records/performance/plan-cost-model/out/wide-cases.tsv:docs/agent/records/performance/plan-cost-model/out/wide-clang-galoot-O0-warm.tsv`
- selfcheck-gcc: `python3 -I docs/agent/snippets/plan_cost_model/cost.py selfcheck docs/agent/records/performance/plan-cost-model/out/weights-gcc-galoot-O0.tsv docs/agent/records/performance/plan-cost-model/out/figures-gcc.txt --train docs/agent/records/performance/plan-cost-model/out/train-cases.tsv:docs/agent/records/performance/plan-cost-model/out/train-gcc-galoot-O0-cold.tsv docs/agent/records/performance/plan-cost-model/out/train-gen-cases.tsv:docs/agent/records/performance/plan-cost-model/out/train-gen-gcc-galoot-O0-cold.tsv docs/agent/records/performance/plan-cost-model/out/train-map-cases.tsv:docs/agent/records/performance/plan-cost-model/out/train-map-gcc-galoot-O0-cold.tsv --check docs/agent/records/performance/plan-cost-model/out/test-cases.tsv:docs/agent/records/performance/plan-cost-model/out/test-gcc-galoot-O0-cold.tsv docs/agent/records/performance/plan-cost-model/out/plans-cases.tsv:docs/agent/records/performance/plan-cost-model/out/plans-gcc-galoot-O0-cold.tsv docs/agent/records/performance/plan-cost-model/out/maps-cases.tsv:docs/agent/records/performance/plan-cost-model/out/maps-gcc-galoot-O0-cold.tsv docs/agent/records/performance/plan-cost-model/out/wide-cases.tsv:docs/agent/records/performance/plan-cost-model/out/wide-gcc-galoot-O0-cold.tsv`

## Results at `4895a40`

Every number is from the files above; "exact" means the predicted threshold equals the measured one.

| | Clang 23.1.4 | GCC 16.2.1, cold |
| --- | --- | --- |
| Mirror against C++ (`verify-*`) | 1,254 cases, 0 differ (30 whole plans of more than 2,000 terms not written out) | the same |
| Fit (`train`, `train-gen`, `train-map`) | every phase exact in integers | every phase exact in integers, after the event `mapdef.second_test` |
| Held-out probes (`test`) | 204 of 204 exact (15 or more per phase) | 204 of 204 exact |
| Whole plans (`plans`): `full<n>` × `full<n>` for n = 3 to 7, four products, four forms, and random sets | 102 of 102 exact | 102 of 102 exact, up to 5.85 × 10⁸ operations (peak 9.8 GB per compile) |
| Whole maps (`maps`): five maps of `full<n>`, n = 3 to 10, and random sets | 52 of 52 exact | 52 of 52 exact |
| Sets programs write, n = 4 to 16 (`wide`) | 109 of 118 exact; in the other 9 the unit's largest evaluation is the metric's check (`metric.hpp`), which the model does not cover, at 2,536 steps (n = 12) and 4,404 (n = 16) | 109 of 118 exact; the same 9, at 16,861 and 29,291 operations |

Against note 0017's weights (`e1af2ac`, `records/performance/constexpr-certification/out/weights-*.tsv`): `defects`, `collect`, `typed`, and `typing` are unchanged event for event on both compilers, as their code is. In `count` and `fill` every pair of a base product costs 10 more steps on Clang and 44 more operations on GCC (`gen.pair`: `factor_of` is now called twice per pair), and in `capacity` every pair costs 10 more steps and 51 more operations (`capacity.pair`: the test for the geometric antiproduct and the argument n), with the per-path weights of `pair_capacity` regrouped; the antiproducts add their own events (`factor.anti`, `factor.rs_bit`, `run.anti`, `run.rs_bit`: on Clang 46, 13, 49, and 13 steps; on GCC 182, 72, 226, and 72 operations). The full plans' counts of note 0017 are unchanged where `collect` is the largest phase (on Clang `full<5>` and `full<6>` of the geometric product: 655,239 and 5,110,503; the wedge), and higher where `fill` is (the dense form on Clang: 681,823 against 680,927 at n = 4). Four weights of `count` and `fill` (`add.call`, `add.cl_equal`, `add.cancel`, `agp.term`) moved by equal and opposite amounts: the fit is rank-deficient there and the solver picks one basic solution, so the change lies in its null space and changes no prediction whose events lie in the span of the training rows, which `cost.py` checks for each prediction and found true for every one made here.

The canary's thresholds are the `figures` rows `gp-r-5`, `gp-dense-4`, `gp-grades-3-7`, `sparse-antigp-16`, `reverse-7`, and `right_complement-8`, confirmed by `core.plan_cost_model` on clang-debug and gcc-debug at `22db6bc`. After the W5 review (decision 0132) two more references were added, from the rows `antiwedge-r-5` and `gp-vectors-full-6` (the second a row added for it), confirmed by `core.plan_cost_model.<unit>` on both debug presets at `9b70dc8`, on GCC with the check above the threshold.

Configurations checked with the canary's six units (decision 0131): clang-release, clang-asan, lint, and gcc-release count exactly as the debug presets; gcc-asan counts 2.2 % to 4.4 % more. Clang at bare `-std=c++23 -pedantic-errors -O2`, without `_GLIBCXX_ASSERTIONS`: `full<5>` × `full<5>` in R(5,0,0) costs 770,687 steps against 655,239 (selection `cost-model-config:*`).

## Rerun at `6a3cf13`

Headers of `develop` at `692d661`, with the compilers above. `probe.py` first had its product names repaired (`gp` and `antigp` are now written as `geometric_product` and `geometric_antiproduct`, the enumerators `plan.hpp` has had since `5c5188f`); before that, every plan probe and every `capacity`, `count`, and `fill` probe of those two products failed to compile. Each run is a row of `runs.tsv` with the selection named here. The `verify` runs were made on the working tree that was then committed as `6a3cf13`, so their rows carry `f5a2698-dirty`; the blind review of plan 0037 repeated them at `753a334` (selections `review:snippets:*`).

| Run | Selection | Outcome |
| --- | --- | --- |
| `probe.py verify` of `test` (204 cases), both compilers | `snippets:plan_cost_model:verify-test:<compiler>` | 0 differ; the output equals `verify-test-<compiler>.tsv` row for row |
| `probe.py verify` of `maps` (52 cases), both compilers | `snippets:plan_cost_model:verify-maps:<compiler>` | 0 differ; equals `verify-maps-<compiler>.tsv` |
| `probe.py verify` of all seven sets, both compilers, by the reviewer at `753a334` | `review:snippets:verify-<set>:<compiler>` | 0 differ in each. Each output equals the committed file except `verify-train-gen-*.tsv`, which hold two rows (`traingen47-count`, `traingen47-fill`) for cases that `train-gen-cases.tsv` does not have. Both files came in one commit (`cd7dcf2`), so the cause is not traced; the fit reads the case file, not the verify file, so the two extra rows touch no weight |
| `cost.py selfcheck`, Clang data | `snippets:plan_cost_model:selfcheck-clang` | "1228 measured cases predicted; 9 whole units whose largest evaluation is the metric's check, not compared; 0 differences", exit 0, 88.7 s |
| `cost.py selfcheck`, GCC data | `snippets:plan_cost_model:selfcheck-gcc` | "1235 measured cases predicted; 9 whole units ... not compared; 0 differences", exit 0, 91.7 s |
| `cost.py comments include/galoot/core/plan.hpp` with both figures files | `snippets:plan_cost_model:comments` | Exit 1: "4 counts in the comments are not in the figures", all in `product_plan`'s comment |

The four counts `comments` reports, with their sources. None is a cost the model predicts, so none contradicts it; each came into the comment from `records/research/2026-11-09-scalar-full10-gcc.md` (`70f91fa`, `16b4d56`):

| Line | Count | What it is |
| --- | --- | --- |
| 619 (twice) | 1,024 | The terms and the result blades of the scalar times `full<10>`: sizes, not costs |
| 620 | 4,096 | The terms of `full<6>` times `full<6>`: a size |
| 624 | 134217728 | The value of `-fconstexpr-ops-limit` under which the scalar times `full<10>` builds (2^27), a flag value from the research record's runs |

The comment's "about 89 million operations" is not matched by the checker's pattern, which takes only digits; its source is the same record (89,102,056, the typing phase, measured with the cache on). `selfcheck` compares the model with the committed measurements; it does not measure the current headers. The `verify` runs show that the mirror still gives the C++ generators' results phase by phase, and the canary `core.plan_cost_model` checks the counts of its eight reference units in each preset without sanitizers; it passed in the P9 matrix at `fe4fb3b` (`gate:all`), whose headers are those of `692d661`. So the model's figures stand. The thresholds were not remeasured (no `probe.py run`).
