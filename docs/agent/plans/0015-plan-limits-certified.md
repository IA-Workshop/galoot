# 0015 Certified limits of plans and maps in place of measured ceilings

- Status: closed; the unit merged into `develop` by `39d87a8` on 2026-11-01 (status corrected 2026-11-11, from `git log --merges` on `develop`). Status text as written before that correction, kept as history: claimed 2026-11-01 (planner and implementer: Claude, agent, Opus 5.5, performance-engineer role, with the cpp-expert, doctest-expert, and build-expert role files for the comments, the tests, and their registration). Implemented on the branch, awaiting the W5 review (Outcome below).
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

## Outcome (2026-11-01, Claude, agent, Opus 5.5, performance-engineer, planner and implementer)

Status: done on `feature/plan-limits-certified`, awaiting the W5 blind review. The build inputs (headers, tests, build files) are final at `57f9d1b`; `1f7cf28` changes only `docs/agent/` after it (`git diff --name-only 57f9d1b 1f7cf28` lists nothing outside `docs/agent/`), and all seven presets ran from new, empty trees at `1f7cf28`. This outcome and the W10 after rows are committed after that run and change records only.

### Departures, skipped checks, and ambiguities (C20)

1. The W10 before commit `3013c63` says "139 of 139 and 141 of 141"; the trees held 140 and 142 tests (`ctest -N`), and all passed (exit 0). The count came from a grep of the CTest log, not from CTest. The message stays as history.
2. `22db6bc` (the canary) failed clang-tidy on two of its units (positional initializers, a missing trailing comma, `size() > 0`); its message did not know it. `db9b9ad` fixed them and says so. No preset lints these units, since only the canary compiles them; clang-tidy was run on them by hand at `db9b9ad`, with the lint tree's flags (0 findings).
3. Checks not run at every commit: `809a975` (a pure move of four scripts) and `e73ce89` (the snippet) change nothing a test or gate reads, so nothing ran at them. `cd7dcf2` and `1f7cf28` change only `docs/agent/`; nothing ran at `cd7dcf2` besides the self-check, and the seven presets ran at `1f7cf28`. At `22db6bc`: `all` and the full suite on gcc-debug (148 of 148) and clang-debug (150 of 150), `format-check`, the trace gate; clang-tidy found item 2. At `db9b9ad`: the canary, its tests, and the gates (`tests.*`, `reference.*`, `core.include_scan`) on both, 17 of 17 each; `format-check`, clang-tidy of the units, the trace gate. At `57f9d1b` (comment only): `all` and the header sets, and every test labeled `core`, `tests`, or `reference` on gcc-debug (123 of 123) and clang-debug (125 of 125); lint's `all`, header sets, and `format-check`; the trace gate. The full gcc-debug suite at the end of the test-and-code pair ran at `1f7cf28`, whose build inputs are `57f9d1b`'s.
4. Plan section 7 named the run-log selections `cost-model:*`; the searches used `harness.py` unchanged, so they are `ceiling-cert:*` as in note 0017, with `cost-model-verify:*` and `cost-model-config:*` for the new kinds.
5. Reference case choice (R5): `wedge_full5`, drafted first, was replaced by `gp_grades_3_7`, because on GCC a plan's typing is the largest evaluation of some inputs (decision 0131 item 3) and the wedge added only another `collect` case. `reverse_full7` was kept although `collect` is also pinned by `gp_full5` on Clang, because it is a certified example of `unary_map`'s comment and the only `collect` reference on GCC.
6. One probe of `train-gen` (`traingen47`, about 7 × 10⁷ steps on Clang) was dropped from the case set by a bound on the mirror's event count (`largest_events`), so that no probe costs minutes per compile; the case file is regenerated identically by `probe.py cases train-gen`. Clang's search of that probe ran to completion before the bound was added; its rows remain in `train-gen-clang-galoot-O0-warm.tsv` and are not used, since the case is not in the case file.
7. The canary's wall time: 4.26 s (gcc-debug) and 4.69 s (clang-debug) in CTest's report of the run before `22db6bc` was committed (the same files), and 4.83 s and 5.00 s inside the serial suite runs of the final trees at `1f7cf28`, on a shared machine. Criterion 7's bound is "exceeds 5 s", which no run did, so it has no `slow` label; it is the longest test of the suite, and the margin is small. Open item 3.
8. GCC builds with sanitizers do not run the canary (decision 0131 item 5); gcc-asan counts 2.2 % to 4.4 % more.
9. Steps outside `tools/timed` (P13): `clang-format -i` and `--dry-run` on touched files before the timed `format-check`, `cost.py fit`, `check`, `figures`, and `selfcheck` (they compile nothing), the mutation driver's own Python, `ctest -N` to count tests, and three `ctest --output-on-failure` reruns on gcc-asan to read the canary's messages (the first gcc-asan run is timed). Every compile of the refit and every build and CTest run went through `tools/timed`.
10. The job pool: 23 of 23 slots free before and after the canary mutation runs and the seven presets; never stopped or started. Memory: the largest GCC search compile peaked at 9.8 GB (`full<6>` dense, 5.7 × 10⁸ operations), with at most four GCC whole-unit searches at once; the machine showed at least 94 GB free when it was read.

### What the refit changed (the coordinator's condition)

Against note 0017's weights at `e1af2ac` (details in `records/performance/plan-cost-model/README.md`):

- Unchanged, event for event, on both compilers: `defects`, `collect`, `typed`, `typing`. Their code did not change, and the held-out probes confirm the weights.
- Changed: `count` and `fill` cost 10 more steps (Clang) and 44 more operations (GCC) per pair of a base product, from the two `factor_of` calls per pair; `capacity` costs 10 more steps and 51 more operations per pair, from the antigp test and the argument n. Four dependent weights of `count` and `fill` moved inside the fit's null space, which changes no prediction.
- New: the antiproducts' events (`factor.anti`, `factor.rs_bit`, `run.anti`, `run.rs_bit`) and the map phases (`map_fill`, `map_typed`, `map_defects`, and `collect` shared with plans). One event was missing from GCC's first map fit (`mapdef.second_test`, the short-circuit of `op == right || op == left`), found from its residual (largest 2.5 operations), and added.
- Consequence for the old figures: where `collect` is the largest phase, note 0017's counts stand (Clang `full<5>` and `full<6>`: 655,239 and 5,110,503); where `fill` is, they rose slightly (Clang dense `full<4>`: 680,927 to 681,823). The model also covers what note 0017 did not: the antiproducts and the maps.

### Done

| Commit | What |
| --- | --- |
| `4895a40` (develop) | Claim: this plan; decision 0125 marked accepted with the owner-ruling section |
| `3013c63` | W10 before rows (fresh gcc-debug and clang-debug trees at `4895a40`) |
| `809a975` | `git mv` of `mirror.py`, `probe.py`, `harness.py`, `cost.py` to `docs/agent/snippets/plan_cost_model/` |
| `e73ce89` | The snippet updated for `develop`: antiproducts, maps, `verify`, `figures`, `selfcheck` |
| `22db6bc`, `db9b9ad` | `core.plan_cost_model`, its six reference units and script, `tools.plan_cost_model_*` (7); the lint fix |
| `57f9d1b` | `product_plan`'s and `unary_map`'s comments: the cost law and counted examples |
| `cd7dcf2` | Refit records, decision 0131, lesson 0064, the self-check's exclusion of the metric-check units |
| `1f7cf28` | Mutation records; snippets index; `harness.py` header; note 0017's dated line |
| this commit | W10 after rows; this outcome |

Tests and expected values for the review (W9): `core.plan_cost_model` with the twelve thresholds in `test/CMakeLists.txt` (listed in `22db6bc`'s message); `tools.plan_cost_model_passes`, `_count_higher` (reports 1005), `_count_lower` (995), `_other_error`, `_passes_below`, `_evasion_above` (2000), `_never_compiles`. No existing test or expected value changed (`git diff 4895a40 57f9d1b -- test` adds only).

### Acceptance criteria

1. Met: `probe.py verify`, 1,254 cases per compiler, 0 differ (`out/verify-*`); 30 whole plans of more than 2,000 terms not written out.
2. Met: every phase exact in integers on both compilers (`weights-*.tsv`; the fit reports residual 0.000); the unchanged phases were refitted at `4895a40` like the others and came out equal to note 0017's weights, and predict the held-out probes exactly.
3. Met: held-out 204 of 204 per compiler, at least 15 per phase; whole plans 102 of 102 and maps 52 of 52 per compiler; wide 109 of 118, the other 9 being units whose largest evaluation is the metric's check (named by the compiler in `metric.hpp`). No GCC whole unit was predicted above 6 × 10⁸, so none was skipped. Every prediction lies in the span of the fit.
4. Met: every count in the two comments is in `figures-<compiler>.txt` (checked by script at `57f9d1b`; only the three limits are not); the commands are in the records README; the comments name Clang 23.1 (classic evaluator), GCC 16.2, libstdc++ 16, `_GLIBCXX_ASSERTIONS`, and that GCC's counts are an upper bound; `tests.citations` passes.
5. Met: `core.plan_cost_model` passes in all six presets that register it at `1f7cf28` (new trees).
6. Met: on clang-debug and gcc-debug, each of the 12 threshold mutants per compiler and each of the 6 generator mutants on a reference's largest evaluation fails the canary by a count that moved; none built with a warning. The 5 mutants in phases that are no reference's largest survive, as section 1 item 6 foresaw (`records/mutations/0015-plan-limits-certified/survivors.md`).
7. Met with departure 7: no run exceeded 5 s, so no `slow` label. CPU time: the run of the canary with its seven tool tests (`ctest -R plan_cost_model`, before the commit `22db6bc`, on the tree of `809a975` with the new files uncommitted) took 4.9 s (gcc-debug) and 5.3 s (clang-debug) of user CPU (`runs.tsv`); the tool tests take under 0.4 s each, so most of it is the canary. The canary alone was not timed for CPU.
8. Met: `cost.py selfcheck` passes for both compilers on the committed data (1,228 and 1,235 measured cases, 9 metric-check units not compared each), and each of its 14 mutants is killed (`selfcheck-cd7dcf2.tsv`).
9. Met with departures 2 and 3. Seven presets from new, empty trees at `1f7cf28`: gcc-debug 148 of 148, gcc-release 148 of 148, gcc-asan 147 of 147 (no canary), clang-debug, clang-release, clang-asan, and lint 150 of 150 each; header sets and `format-check` built on gcc-debug, clang-debug, and lint; lint's clang-tidy clean. Trace gate exit 0 at `22db6bc`, `db9b9ad`, and `57f9d1b`.
10. Met; shared mode, track-only. Fresh trees, built once, CTest serial (`ctest --test-dir`):

    | | gcc-debug before `4895a40` | after `1f7cf28` | clang-debug before | after |
    | --- | --- | --- | --- | --- |
    | Compiles; instructions | 85; 124.66 G | 93; 129.62 G | 87; 135.93 G | 95; 140.48 G |
    | `plan_test.cpp` | 31.67 G, 361 MiB | 31.67 G, 360 MiB | 34.81 G, 346 MiB | 34.80 G, 346 MiB |
    | Build of `all`, wall | 2.9 s | 2.9 s | 2.7 s | 2.8 s |
    | CTest, whole suite, wall | 140 tests, 16.1 s | 148 tests, 40.8 s | 142 tests, 17.8 s | 150 tests, 34.5 s |

    The eight more compiles after are the header-set checks (`all_verify_interface_header_sets`), which the after trees built and the before trees did not; without them the after trees have 85 and 87 compiles of 124.66 G and 135.94 G instructions, the same as before within 0.01 %. The canary's compiles do not go through the compile-cost launcher, so they are not in these rows. The comment change costs nothing measurable (`plan_test.cpp` within 0.01 %). The suite's serial wall time rose by 24.7 s and 16.7 s, of which the new tests account for 8.7 s and 8.8 s; the other tests also took longer than in the before run (31.9 s against 16.1 s on gcc-debug), which is the machine's load at the two times, not a change of theirs: the shared machine ran other workstreams' compiles during the after run, and its rows are not comparable (P14).

### Verification and validation

Verification: the model was checked against the compilers (fit, held-out probes, whole units, the canary) and against the C++ (`verify`) at the revisions named; the comments' figures are the model's output, and six are compiled by the canary in every run. Validation: not applicable (no mathematical content changed). Reproduction: `docs/agent/records/performance/plan-cost-model/README.md`; mutations: `python3 -I docs/agent/records/mutations/0015-plan-limits-certified/mutate.py canary <preset>` and `... selfcheck`.

### Open

1. For a later unit that changes the antiproduct pair rule (`research/antiproduct-growth`): refit `capacity`, `count`, and `fill`, update `mirror.py`'s `pair_capacity`, `factor_of`, `run_term`, and `generate_plan`, regenerate the antiproduct figures of `product_plan`'s comment and the threshold of `plan_cost_antigp_sparse16.cpp` (decision 0131, consequences).
2. Not calibrated: bare `-O2` (Clang and GCC), GCC's sanitizer builds, Clang's bytecode interpreter, GCC's call cache (plan section 13).
3. The canary is the suite's longest test (about 5 s). If that slows the edit cycle, it could be split into one CTest per reference unit, which CTest runs in parallel from the shared pool, at the cost of the name `core.plan_cost_model` naming six tests.
4. For small sets at n = 12 and 16 the largest evaluation of a unit is the metric's check (2,536 and 4,404 steps on Clang, 16,861 and 29,291 operations on GCC), which the model does not cover; the comments state the plans' own counts.
5. Knowledge base: `kb/constant-evaluation-canary` (worktree `knowledge-base-worktrees/constant-evaluation-canary`, from `kb/constant-evaluation-limits`), commit `3f2bcde`, adds a section on guarding published figures to `performance-engineering/constant-evaluation-cost-models.md`; `check_links.py` reports 0 problems. Not merged.

Agent cost: about 230 tool calls; token counts are not visible to me.

## W5 review fixes (2026-11-01, Claude, agent, Opus 5.5, performance-engineer)

The W5 review (`records/reviews/2026-11-01-plan-limits-certified.md`, blind-reviewer, Opus 5.5, head `26cc06e`, merged onto the branch as `d89806d`) accepted the unit after fixes. F1 to F11 are applied below. The outcome above is left as written; this section corrects it.

### Departures, skipped checks, and ambiguities (C20)

1. `539d5d6` (the two stand-in tests for F1 and F4) was committed before the fix it judges, `b6ed70c`, and both tests fail at `539d5d6` (checked on gcc-debug), as its message says.
2. `0aeb46b` changes comments of `include/galoot/core/plan.hpp` (F6, F7) and needs the delta review of a header comment; not done here.
3. F5 is resolved by the split the reviewer recommended, not by the `slow` label. The GCC tests of the two largest units take 4.4 s to 4.7 s each, close to the 5 s of criterion 7, because each now compiles eleven times (F1).
4. F2's registration rule is "`CMAKE_CXX_FLAGS` defines `_GLIBCXX_ASSERTIONS`". A build that gets the assertions only from libstdc++'s own default at -O0 is not registered; no preset is such a build.
5. The run log is not edited (the coordinator's follow-up unit).

### Corrections to the outcome

- Criterion 5: at `1f7cf28` ten presets registered the canary, not six: the four measure presets did too, and it fails there by design (F2). From `a3ce039` it is registered in six (gcc-debug, gcc-release, clang-debug, clang-release, clang-asan, lint).
- Criterion 6: true for the mutant class the plan defined; the wedge rule and `collect_result`'s shifts decided stated figures and no reference reached them (F3). Both are now pinned (decision 0132 item 3), and the reviewer's two mutants are killed (table below).
- Criterion 7: the reviewer measured the single test over 5 s in 8 of 27 runs (F5). Resolved by the split.
- Criterion 4's "checked by script" now has a committed script: `cost.py comments` (F10).

### The fixes

| Finding | Commit | What |
| --- | --- | --- |
| F1 | `539d5d6` (tests), `b6ed70c` | On GCC a pass needs compiles at nine limits from T + 1 to T + T/10 as well (reduced from ten in `a3ce039`); docstring corrected; `tools.plan_cost_model_evades_at_threshold` |
| F2 | `a3ce039` | Registered only where `CMAKE_CXX_FLAGS` defines `_GLIBCXX_ASSERTIONS` (on GCC also without `-fsanitize`); dated correction in decision 0131 item 5 |
| F3 | `b6cef4d` (figure), `9b70dc8` (tests) | `plan_cost_antiwedge_full5.cpp` (wedge rule) and `plan_cost_gp_vectors_full6.cpp` (shifts), thresholds 271,653 / 1,393,084 and 474,959 / 2,415,778; both reviewer mutants killed |
| F4 | `539d5d6` (test), `b6ed70c` | GCC's built-in message one below is named and searched; `tools.plan_cost_model_builtin_below` |
| F5 | `a3ce039` | One CTest per unit, `core.plan_cost_model.<unit>`; decision 0132 item 4 records the timings |
| F6, F7 | `0aeb46b` (comment only; delta review needed) | The plan's own evaluations versus the metric's check; GCC also counts more without assertions; libstdc++'s default at -O0; GCC's sanitizers |
| F8 | `a3ce039` | The OBJECT library `core_plan_cost_units`; lint ran clang-tidy on all eight units at `9b70dc8` (exit 0) |
| F9 | `b6cef4d`, this commit | `selfcheck` fails on a missing or failed measurement; dated correction in decision 0131 item 4 |
| F10 | `b6cef4d` | `cost.py comments`; passes on `plan.hpp` at `0aeb46b`, fails on a changed count (`comments-0aeb46b.tsv`) |
| F11 | `b6cef4d` | `cost.py` and `probe.py` executable |

Tests and expected values for the review (W9): `tools.plan_cost_model_evades_at_threshold` and `_builtin_below` (in `539d5d6`, outputs named there); `core.plan_cost_model.antiwedge_full5` and `.gp_vectors_full6` (in `9b70dc8`, thresholds above). No other threshold changed.

Checks per commit: `539d5d6`, the tools tests on gcc-debug (the two new ones fail, as intended). `b6ed70c`: the canary and its tools tests on gcc-debug (10 of 10) and clang-debug (an unreconfigured tree, 8 of 8). `a3ce039`: `all` on gcc-debug and clang-debug, the eight canary-related tests on both, lint `all` (the object library with clang-tidy); registration counted on gcc-debug, clang-debug, lint (6 each), gcc-asan, clang-measure, gcc-measure (0 each). `9b70dc8`: `all` and the canary on both (8 of 8 each), lint `all` and `format-check`. `0aeb46b`: `all` and the header sets, the `core`, `tests`, and `reference` labels on gcc-debug (130 of 130) and clang-debug (132 of 132), lint `all`, header sets, and `format-check`, the trace gate. `b6cef4d` and this commit change no build input; both self-checks ran at `b6cef4d` (0 differences).

Mutations at `0aeb46b` (`survivors.md`): the canary 24 of 29 killed on each debug preset, the reviewer's two survivors among the kills; the self-check 18 of 18; the comments check 2 of 2.

