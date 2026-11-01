# 0015 Certified limits of plans and maps in place of measured ceilings

- Status: claimed 2026-11-01 (planner and implementer: Claude, agent, Opus 5.5, performance-engineer role, with the cpp-expert, doctest-expert, and build-expert role files for the comments, the tests, and their registration).
- Branch `feature/plan-limits-certified`, worktree `.claude/worktrees/plan-limits-certified`, from `develop` at the claim commit. Decision numbers from 0131 (0114 to 0116, 0121, 0122, 0125, and 0130 are in use).
- Owner ruling behind the unit: decision 0125, accepted as proposed on 2026-11-01 (its owner-ruling section, added in the claim commit), with the coordinator's condition that the model is first refitted against the current `develop`. Evidence and method: research note 0017 and `docs/agent/records/performance/constexpr-certification/`.
- Goal: the `///` comments of `product_plan` (which covers the four binary operations, the antiproducts included) and `unary_map` state the cost law and examples certified by the exact cost model on the named compilers, in place of the ceilings found by building each n; a canary CTest, `core.plan_cost_model`, compiles reference plans at their committed thresholds and one below, so that a compiler, library, or generator change that invalidates the figures fails a test; the predictor moves to `docs/agent/snippets/` with a self-check that can fail.

## 1. Departures, ambiguities, and owner questions (C20)

Each item states the reading this plan follows.

1. The refit first. Note 0017 calibrated the headers of `e1af2ac`. Since then WU7b changed `plan_capacity`, `pair_capacity`, and `generate_plan` (the antiproducts' `factor_of` and `run_term`, and the argument n) and added the unary maps; plan 0014 changed `metric_like`. `collect_result`, `type_terms`, `typed`, `defects_of`, `typed_defects_of`, `blade_product.hpp`, `bits.hpp`, and the metric factories are unchanged in `git diff e1af2ac develop` (read while planning). So the generation phases are refitted for all four operations, the map phases are fitted for the first time, and the unchanged phases are checked against new held-out probes with their old weights and refitted only if a probe disagrees. The outcome says, phase by phase, what changed.
2. "The map/antiproduct generators' comments" is read as two comments: `product_plan`'s (the antiproducts are operations of it) and `unary_map`'s. The measured limits in `blade_set.hpp` (`full`, `merge`) are out of scope.
3. `product_plan`'s comment also states, from instruction counts, how the antiproduct's cost grows with n at fixed use. That is not a ceiling; it is restated in the model's exact counts for the same sets at n = 4 and 16, so the comment uses one kind of figure. The instruction measurements stay in decision 0116.
4. The certified configuration. The model is fitted on Clang 23.1.4 (classic evaluator) and GCC 16.2.1 with libstdc++ 16 and the flags of Galoot's debug presets, which define `_GLIBCXX_ASSERTIONS`; GCC is fitted with its call cache off (`-fconstexpr-cache-depth=1`), the cold bound the owner accepted with no margin (item 6). The two Clang ceilings that depend on `_GLIBCXX_ASSERTIONS` (decision 0116) are replaced by a statement that the figures assume it, with the reason, which note 0017 section 2.4 derived from libstdc++'s source and one compile at the claim revision rechecks. A bare `-O2` calibration is deferred (section 13).
5. Which presets run the canary is decided by measurement: the reference thresholds are measured with the compile command of each of the seven presets. By reading, they should be equal within each compiler (`GALOOT_ASSERT` has the same text whatever `GALOOT_CHECKING` says, and with `_GLIBCXX_ASSERTIONS` defined libstdc++'s assertion has one form at every `-O`), but release and sanitizer flags were never measured. The canary is registered in every preset whose thresholds equal the debug preset's; any other is left out, with the reason in decision 0131.
6. "A mutant that changes a weight must be killed by the canary" is read as: a change to the generator's code on a path of the largest evaluation of a reference unit, and a change to a committed threshold. A phase that is never the largest evaluation of any unit cannot change a certified figure; for each such phase (expected: `capacity`, `count`, `defects`, `typing`, and the map's checks) the outcome gives the argument from the fitted weights, and a mutant there is recorded as outside the canary's reach. If the argument fails for a phase, the canary gets a reference unit for it. A changed weight of the predictor itself is killed by the predictor's self-check, not by the canary.
7. "Move the cost.py tool": `cost.py` imports `mirror.py` and `probe.py`, and refitting needs `harness.py`, so the four scripts move together with `git mv` into `docs/agent/snippets/plan_cost_model/`, in a commit that only moves them, and are then changed for `develop`. Note 0017's data (`out/`) stays where the note cites it, and the note gets a dated line saying where the scripts went and that its figures came from them at `3bd9904`. The new fit's data goes to `docs/agent/records/performance/plan-cost-model/`.
8. The owner's item 4 (checked searches for future GCC limit work) is recorded where a future agent looks: the snippet's header, the snippets index, and a lessons-learned entry.
9. The canary reports the measured threshold on a mismatch, as decision 0125 item 2 says, by a short search with the confirmation above the result that GCC needs. The search runs only when a reference fails.
10. W10 applies because the unit changes a core header, though only in comments; the costs are recorded before and after as usual.

## 2. Definition of done

- `mirror.py` copies the generator of the claim revision: `plan_capacity` and `pair_capacity` with n, `generate_plan` with `factor_of` and `run_term` for the four binary operations, and the unary maps (`unary_term`, `generate_map`, `typed_map_of`, `map_defects_of`, `closed_form_right_sign`). A check compiles, for every training and held-out input, the phase's C++ result against the mirror's and finds them equal.
- Weights fitted exactly, in integers, for Clang (Galoot's debug flags) and GCC (the same, cold), for every phase of `make_product_plan` and `make_unary_map` that the model covers; every held-out phase probe and every whole plan and map measured is predicted exactly.
- `product_plan`'s and `unary_map`'s `///` comments state the cost law and certified examples with the compilers, library, and configuration they hold for, and no ceiling found by trial. No other header text changes.
- `core.plan_cost_model` and the tests of its script exist and pass in the presets decision 0131 names.
- The four scripts are in `docs/agent/snippets/plan_cost_model/`, indexed in the snippets README, with a header comment each and a self-check (`cost.py selfcheck`) that fails on a changed weight, event, or recorded threshold, shown by mutation.
- Mutation records under `docs/agent/records/mutations/0015-plan-limits-certified/`; decision 0131 (and later numbers if needed); a lessons-learned entry for checked searches; note 0017's dated line; this plan's outcome.
- W5 blind review requested by the coordinator, who merges.

## 3. Acceptance criteria

1. Mirror fidelity: for every training and held-out input, the C++ phase result equals the mirror's (count, plan, result set, typed plan, map, defects), on Clang and GCC.
2. Exact fit: every fitted phase has zero residual with integer weights on both compilers; the old weights of each unchanged phase predict its new held-out probes exactly, or that phase is refitted and the outcome says so.
3. Prediction: every held-out phase probe (at least 15 per phase and compiler) and every whole unit measured (the `full<n>` grid of each operation, form, and map up to the first n over the default limit, and small sets at n = 4 to 16) is predicted exactly by a checked search, with each prediction inside the span of the fit. On GCC, whole units whose predicted cold count exceeds 6 × 10⁸ are not measured (memory, note 0017 section 3.2), and the outcome lists them.
4. Comments: every figure in the two comments is the predictor's output at a recorded revision, reproduced by a committed command; the comments name the compilers, the library, and the configuration, say that GCC's figures are an upper bound, and cite no project record (STYLE S11).
5. Canary, positive: `core.plan_cost_model` passes in every preset decision 0131 names, at the final revision.
6. Canary, negative: each committed threshold changed by +1 and by −1 makes the canary fail on that compiler; each generator mutant on the largest evaluation of a reference unit makes it fail; it is never killed by a warning alone (section 6).
7. Canary cost: its wall time and CPU time on gcc-debug and clang-debug are recorded; if its wall time exceeds 5 s in either, it carries the `slow` label (P9).
8. Snippet: `cost.py selfcheck` passes on the committed data and fails on each of its mutants (section 6).
9. Checks: every commit passes, at that commit on a clean tree, the tests it can affect, clang-format and clang-tidy of each touched C++ file, and the cheap gates (`tests.reference_markers`, `tests.golden_facts`, `tests.citations`, `reference.golden`, `core.include_scan`, and the trace gate); the full gcc-debug suite runs at the end of the test-and-code pair; all seven presets build and pass from new, empty build trees at the final revision of the build inputs, with the header sets and `format-check` on gcc-debug, clang-debug, and lint.
10. W10: build time, build memory, and test time of the whole suite on fresh gcc-debug and clang-debug trees at the claim commit and at the final code revision.

## 4. Approach

### Refit (records and snippet only)

1. Move the four scripts (`git mv`, no edit), then update `mirror.py` for the claim revision. Events stay named `<function>.<path>`; paths that differ by operation are named by operation (the per-pair and per-term paths of `generate_plan`, the branches of `pair_capacity` and `unary_term`), so one fit per phase serves all operations and shares the events of `blade_product`, the wedge rule, and `reorder_sign`. New phases: `map_fill` (`filled_map`), `map_typed` (`typed_map_of`), `map_defects`; a map's `found` is the `collect` phase, the same function, checked rather than assumed.
2. `probe.py`: units for the new phases and operations, case sets for them (seeds 20261102 and on), and a `verify` command that compiles each case once with a `static_assert` that the C++ phase result equals the mirror's literal (criterion 1). `harness.py` keeps its checked search unchanged.
3. Measure on the shared pool: training probes for `capacity`, `count`, `fill` (four operations) and the map phases; held-out probes for every phase; whole units. Clang first (cheap), then GCC cold. Fit with `cost.py fit`, check with `cost.py check`.
4. If a fit is not exact, look for the missing event as note 0017 did (one more event, found from the residual). Two distinct attempts without new understanding halt the unit (W8).
5. `cost.py figures`: prints the figures the comments state and the canary's reference thresholds from the weights, with the revision; its output is committed in records. `cost.py selfcheck <records>`: refits from the committed training rows and compares with the committed weights, predicts every committed measurement and compares exactly, and checks the committed figures file against a fresh `figures` run; it exits nonzero on any difference.

### The comments

`product_plan`, replacing the measured paragraph: the plan is a constant evaluation, so the compiler's limits bound the sets; generating costs a fixed amount per pair plus the pair's product, which for the geometric product grows with the grades of the two blades (for an antiproduct, of their complements); typing costs in proportion to the number of terms times the number of result blades and dominates for large sets; GCC also refuses a loop of 262,144 iterations, so a plan of that many terms. Then the certified counts against the default limits for Clang 23.1 (classic evaluator) and GCC 16.2 (an upper bound: the count with its cache of constant calls off), with libstdc++ 16 and `_GLIBCXX_ASSERTIONS` defined: `full<n>` × `full<n>` at the largest n under each limit and the next n, per operation, in R(n,0,0) and the dense form where they differ; the four-by-three sets at n = 4 and 16 through the geometric product and antiproduct. A sentence says that another compiler version, evaluator, or standard library counts differently, and that without `_GLIBCXX_ASSERTIONS` libstdc++ checks subscripts through a call that costs Clang more steps. `unary_map` the same, shorter: typing costs in proportion to the square of the size of S, with the counts of `full<n>` maps.

### The canary

- `test/core/plan_cost_<case>.cpp`: one small unit per reference case, each reading one `product_plan` or `unary_map` in a `static_assert`, with a comment that says what it pins in plain words.
- `test/tools/plan_cost_model.py`: given the compile command, the compiler kind, and pairs of source and threshold, compiles each source with `-fsyntax-only` at the threshold (must compile) and at one below (must fail with the compiler's limit message: Clang "constexpr evaluation hit maximum step limit", GCC "constexpr operation count exceeds limit"); on GCC with `-fconstexpr-cache-depth=1`. On a mismatch it searches for the measured threshold (bracket, bisection, and the confirmation above the result) and prints it beside the committed one.
- Registered in `test/CMakeLists.txt` with `galoot_compile_command`, so the canary compiles as the library's tests do, with labels `core;core.plan`; the thresholds are written in the registration, one per compiler, with a comment on what they are and how they are regenerated, in plain words.
- `tools.plan_cost_model_*`: the script against a stand-in compiler (`test/tools/plan_cost_model/fake_compiler.py`) whose count is fixed by its arguments: passes at the true threshold; fails, reporting the measured threshold, when the committed one is one too high or one too low; fails when the compile one below fails for another reason; fails when the compile one below passes although one at the threshold also passes (GCC's evasion).

Reference cases are chosen after the refit by these rules: (R1) the unit's largest evaluation is a plan or map phase, above every other evaluation of the unit by a baseline unit without the plan; (R2) together the cases make the heavy phases each the largest at least once: the fill of the geometric product, the fill of an antiproduct, `collect`, `typed`, and the map's typing; (R3) on GCC, the compile one below fails with the operation-count message; (R4) each compile takes under 2 s and 1 GB on the slower compiler; (R5) at most six cases, preferring the certified examples of the comments.

### Commits

1. Claim (on `develop`): this plan and decision 0125's owner-ruling section.
2. Branch: W10 before rows; `git mv` of the scripts; mirror and probe updates; refit data and weights (records only).
3. Tests before the comment: the canary's script, its tests, the reference units, and their registration with the thresholds of the refit (W9: listed with their expected values). They judge the generator, which this unit does not change, so they pass at once; that they fail on the threshold mutants is shown in step 5.
4. The comment change in `plan.hpp`.
5. Mutation driver and results; snippet index, self-check, lessons entry, note 0017's line, decision 0131; seven presets; W10 after rows; outcome.

## 5. Tests and expected values

| Test | Expected | Reference |
| --- | --- | --- |
| `core.plan_cost_model`, Clang | each reference compiles at its committed threshold and fails one below with the step-limit message | the cost model's prediction (mirror counts times fitted weights), confirmed when committed by a checked search on clang-debug; independent of the library's own output, since it is the compiler's count |
| `core.plan_cost_model`, GCC | the same with the operation limit, cold | the same, on gcc-debug |
| `tools.plan_cost_model_passes` | exit 0 | the stand-in's fixed count |
| `tools.plan_cost_model_count_higher`, `_count_lower` | exit 1, output names the stand-in's count as measured | the stand-in's fixed count |
| `tools.plan_cost_model_other_error` | exit 1, output says the compile one below failed for another reason | the stand-in's message |
| `tools.plan_cost_model_passes_below` | exit 1, output says the unit compiles one below its threshold | the stand-in's evasion mode |

The thresholds are committed values ported with their derivation (the `figures` output and its revision in records), never a call of the predictor from the test. All tests and expected values are listed in the review request (W9).

## 6. Mutations

Driver `docs/agent/records/mutations/0015-plan-limits-certified/mutate.py`, modeled on plan 0014's: each anchor must occur once (lesson 0061); a build failure is read before any test, and CTest is not run after a failed build (lessons 0060, 0063); a failure only by a `-Werror` diagnostic counts as "warning", not a kill; the job pool's status is read before and after.

| Mutant | Change | Expected killer |
| --- | --- | --- |
| `threshold_up_<case>`, `threshold_down_<case>` | a committed threshold +1 or −1, per case and compiler | `core.plan_cost_model` |
| `typed_scan_step` | one more statement in `type_terms`' search loop | the canary, on the cases whose largest phase is `typed` or the map's typing |
| `collect_scan_step` | one more statement in `collect_result`'s scan | the canary, on a case whose largest phase is `collect` |
| `product_term_step` | one more statement per term in `add_generator_product` | the canary, on the geometric-product fill case |
| `factor_step`, `run_term_step` | one more statement in the antiproduct branch of `factor_of` or `run_term` | the canary, on the antiproduct fill case |
| `map_copy_step` | one more statement in `typed_map_of`'s copy loop | the canary, on the map case |
| `defects_step`, `typing_step`, `capacity_step`, `map_defects_step` | one more statement in a phase that is never the largest | none expected (item 6 of section 1), recorded with the argument |
| `weight_<phase>`, `event_<phase>`, `figure_<case>` | a committed weight changed by one; an event dropped from the mirror; a figure of the committed figures file changed | `cost.py selfcheck` |

The "one more statement" is a plain statement with no effect on the result (for example a local counter incremented), so that only the count changes; each is checked to compile without a warning before it is counted.

## 7. Evidence and records

- `docs/agent/records/performance/plan-cost-model/`: case files, thresholds, weights, check outputs, `figures-<rev>.txt`, and a README of the commands; every compile through `tools/timed measure`, rows `cost-model:*` in `runs.tsv`.
- The comparison of the new weights with note 0017's, event by event, in the outcome.

## 8. W10 costs

Fresh gcc-debug and clang-debug trees at the claim commit and at the final code revision, built once, the suite run once, rows copied by `copy_compile_rows`. Reported: build wall time, total compile instructions, the canary's own wall and CPU time, and the suite's wall time. Shared mode, track only (C14).

## 9. Review

W5 blind review of the comment change, the canary and its tests, the registration, and the snippet, with this plan's criteria and the outcome's departures; the reviewer gets note 0017 and decision 0125, not the author's view that the change is correct. The coordinator requests it and merges.

## 10. Cost estimate (for information, decision 0019)

Agent: about 300 to 450 tool calls and 2 to 4 M tokens (the refit is most of it). Machine, tracked only: about 1,500 probe units at 15 to 30 compiles each on the shared pool (under 1 s each), about 40 to 90 minutes in all; GCC whole units up to 6 × 10⁸ cold operations, about 9 GB each, run a few at a time; the mutation runs are about 30 mutants, each an incremental build of the canary's references; four fresh builds for W10 and seven for the final presets.

## 11. Risks

- A missing event on the new paths (as GCC's unwritten Gram entries were). Found from a residual; if it cannot be found in two attempts, W8 halts the unit.
- GCC's evasion of its limit at a reference's one-below compile. Rule R3 avoids such cases; a later generator change can create one, and the canary then fails, which is the intended signal to refit.
- Memory of GCC cold searches (35 GB for one search of note 0017). Whole units above 6 × 10⁸ predicted operations are not measured, and heavy GCC searches run at low concurrency.
- Another workstream (`research/antiproduct-growth`) may later change the antiproduct pair rule. That unit then refits the `capacity`, `count`, and `fill` phases (the antiproduct events), regenerates the antiproduct figures of `product_plan`'s comment and the antiproduct reference's threshold, and updates `mirror.py`'s `generate_plan`, `factor_of`, `run_term`, and `pair_capacity`. The typing and map phases are unaffected unless their code changes. The outcome repeats this list.

## 12. What would need a refit, in general

A change of compiler version, of Clang's default evaluator, of libstdc++, or of the flags that change libstdc++'s assertions; a change to any function on a phase's path (`plan.hpp`, `blade_product.hpp`, `bits.hpp`, `blade_set.hpp`'s `canonical_less`); a change to how the metric factories write the Gram matrix (GCC). The canary catches each of these that moves a reference's largest phase.

## 13. Deferred

- A bare `-O2` calibration (Clang and GCC), and GCC's other configurations.
- A model of GCC's call cache (note 0017 open question 2).
- Clang's bytecode interpreter.
- Cheaper typing (owner item 5: not now).
- The limits stated in `blade_set.hpp`'s `full` and `merge` comments.
