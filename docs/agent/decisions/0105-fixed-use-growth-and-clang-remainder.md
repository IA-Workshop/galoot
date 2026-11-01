# 0105 The growth of a fixed use after the set-bit loops, and the cause of what remains on Clang

- Status: accepted (recorded by the author; the owner may reverse it, and the classification under PERFORMANCE P2 in item 3 is the owner's to confirm)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), implementing plan 0012 in the role of performance-engineer.

## Measurements

All in `docs/agent/records/performance/product-cost-growth/`. GCC 16.2.1 and Clang 23.1.4 with libstdc++ 16, Galoot's test flags at -O0 (`-D_GLIBCXX_ASSERTIONS -DGALOOT_CHECKING=1`), `-c`. Every compile of `measure_growth.py`, `trace_remainder.py`, and `remainder_probe.py` ran as `taskset -c 10 tools/timed measure ...`, so its row in `runs.tsv` is in pinned mode on one performance core; the sibling thread (CPU 11) and other processes on CPU 10 were busy at most 0.01 s during any compile. The machine was otherwise quiet: the Python jobs of other sessions seen earlier (on CPUs 4 to 6) had ended, and every performance core was below 5% busy when the runs started at 12:12:58Z. Three repeats, interleaved, so that drift affects both header sets alike. Figures are retired instructions on the pinned core, means of the repeats.

1. Reproduction (`out/reproduce-review-p2-0f1039a.tsv`): the review's `p2_cause.py`, run unchanged at `0f1039a` (pinned by `taskset` inside the command, so the rows are in shared mode, as the review's were; the other sessions' jobs were running then), gives +27% (Clang) and +16% (GCC) for the header as it was, +9% and +1% for the review's copy with set-bit loops in `blade_product.hpp`, and +6% and +0% with `reorder_sign` also changed. The review reported +27%, +16%, +9%, +0%, +7%, +0%.
2. Before and after (`out/growth-77ed567/`): the four typed pairs' plans, the unit minus a baseline with the same algebra and sets and no plan, headers of `0f1039a` and of `c97fac5` (the run at `77ed567`, `out/growth-77ed567/`, agrees within the spread).

| n | GCC before | GCC after | Clang before | Clang after |
| --- | --- | --- | --- | --- |
| 4 | 0.186 G | 0.180 G | 0.347 G | 0.342 G |
| 8 | 0.196 G (+5.2%) | 0.180 G (+0.2%) | 0.373 G (+7.7%) | 0.348 G (+1.5%) |
| 12 | 0.205 G (+10.2%) | 0.180 G (+0.2%) | 0.406 G (+17.0%) | 0.355 G (+3.6%) |
| 16 | 0.215 G (+15.7%) | 0.180 G (+0.2%) | 0.439 G (+26.5%) | 0.365 G (+6.5%) |

   Spread (largest minus smallest of the three) at most 0.8 M on GCC and 3.1 M on Clang. Other work on the pinned CPU or its sibling: at most 0.02 s per compile.
3. Header and test units (criterion 6, same file): `core.hpp` alone 0.7571 → 0.7568 G on GCC (−0.04%) and 0.7376 → 0.7381 G on Clang (+0.5 M, +0.07%, against spreads of 1.6 and 2.8 M); `blade_product_test.cpp` 8.2471 → 8.2229 G (−0.29%) and 5.1300 → 5.1273 G (−0.05%). Beyond the criterion: `bits.hpp` alone +0.4 M on GCC (+0.16%, spread under 0.1 M) and +0.9 M on Clang (+0.44%, spread 0.2 M), the longer comment and the new loop body; `bits_test.cpp` +4.8 M on GCC (+0.14%, spread 2.3 M) and +17.2 M on Clang (+0.58%, spread 5.2 M), whose `static_assert`s run `reorder_sign` over every pair of masks of up to four generators, where the new loop does more work per step than the old one did on such small masks.

## The Clang remainder

After the change, Clang's plans still cost 23 M instructions more at n = 16 than at n = 4. What was measured, at `77ed567` and again at `c97fac5` (`out/trace-*`, `out/probe-*`; figures below from `77ed567`, the later run agreeing within the spread):

- `trace_remainder.py` (`out/trace-77ed567/`): `-ftime-trace -ftime-trace-granularity=0` of the plans unit and its baseline at n = 4, 8, 12, 16. The number of every kind of event is the same at every n: 123 evaluations of initializers, 214 constant expressions, 65 function and 29 class instantiations, 4 code-generated functions. So the remainder is not more work items but dearer ones. The events whose time grows with n are the constant-expression evaluations (plans minus baseline: 184, 480, 790, 1,271 µs) and the constraint-satisfaction checks that contain them (6.4 to 8.4 ms). The initializer evaluations, which generate the plans, take the same 12.5 ms at every n.
- The constant expressions that grow are `metric.hpp:276`, `:277`, and `:278`: the three conditions of `metric_like` that read every one of the n² Gram entries (`entries_in_range()` inside `std::bool_constant`, `is_symmetric()`, and `entries_in_range()` again). Counted in traces of n = 4 and 16: the baseline evaluates each once, in its `static_assert(core::metric_like<alg>)`; a unit with one plan evaluates each twice, and a unit with four plans also twice. At n = 16 they take 1,231 µs in the baseline and about 2,450 µs with one or four plans; at n = 4, 111 and 214 µs.
- `remainder_probe.py` (`out/probe-77ed567/`), pinned instruction counts. With a probe copy of the headers in which `metric_like` keeps only its type test, the growth from n = 4 to 16 of four plans over the baseline falls from 22.6 M to 2.6 M instructions, and of one plan from 23.7 M to 1.3 M; the baseline itself stops growing (Clang 748.7 → 770.5 M as committed, 745.3 → 745.2 M in the probe; GCC 806.9 → 825.3 M and 803.4 → 804.3 M). As committed, one plan and four plans grow by the same amount (23.7 M and 22.6 M), so the cost does not scale with the number of uses. On GCC the plans do not grow in either copy.

What is inferred, not measured: why Clang evaluates the concept a second time. The baseline's evaluation is of the expression `core::metric_like<alg>` in a `static_assert`; the second is the satisfaction check of the first constrained template that names `alg`, after which later plans reuse it (four plans still give two evaluations). That Clang does not share its satisfaction cache with the value of a concept-id expression fits these counts, but it was not read in Clang's source. GCC shows no second evaluation in instructions; how it shares the result was not traced either.

The remaining 1 to 3 M instructions of growth in the probe are within twice the spread of the Clang figures (up to 4.1 M) and are not attributed.

## Decision

1. The loop change of decision 0104 stands as the fix of finding F8: the growth of a fixed use is gone on GCC (+15.4% to +0.1%) and falls on Clang from +27.1% to +6.2%.
2. The Clang remainder is traced to the evaluation of `metric_like`, which reads the n² entries of the signature's Gram matrix. It is a cost per algebra and translation unit, paid by the first constrained use and once more by a separate evaluation of the concept, not a cost per plan.
3. Under PERFORMANCE P2 this growth follows from the algebra's own signature data (checking every entry of an n × n form), so it is recorded and measured at four sizes, as P2 asks, and is not counted as a defect of a fixed use. This reading is for the owner to confirm.
4. Nothing in `metric.hpp` changes in this unit; the concept and what it checks are outside plan 0012's scope.

## Consequences

- An option for the owner, not taken: if the per-algebra check matters, `metric_like` could test a variable template that holds the result of the three conditions for each tag, so that a second evaluation reads a stored `bool`; or the two `entries_in_range()` calls could become one. Either changes a public concept and needs its own measurement and review.
- The P2 figure of "plans minus baseline" counts one evaluation of `metric_like` on Clang whenever the baseline unit also evaluates the concept; a measurement that wants the per-use cost alone should keep that in mind.
