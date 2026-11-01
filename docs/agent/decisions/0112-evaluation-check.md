# 0112 The check that a plan is generated once: trace durations, on Clang only

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), implementing WU7a of plan 0013 in the role of cpp-expert. The owner said to proceed with the plan's defaults; this record holds owner question 5 (departure 7, the form of the check and Clang only), answered with the plan's default.

## Context

`product_plan`'s `///` comment guarantees that a plan is evaluated once in each translation unit for each argument set. Decision 0102 item 7 made every local of the generator `static constexpr`, because Clang evaluates a plain `constexpr` local's initializer again each time the call is evaluated, generating the plan twice. Decision 0103's note (review F3 of WU6b) showed that evaluation counts cannot detect a second generation, because it happens inside the one `EvaluateAsInitializer` of the variable template. Plan 0011 section 12 deferred a CTest to WU7 and named two forms; plan 0013 departure 7 a third.

## Options weighed

1. (a) Durations in a Clang `-ftime-trace`: the median duration of the variable template's `EvaluateAsInitializer` events against the median duration of the generator's `InstantiateFunction` events. One compile, no copy of the header. Chosen.
2. (b) The unit's retired instructions against a copy of the header whose initializer generates twice. Needs `perf` and two compiles.
3. (c) A lowered Clang step limit that one generation fits under and two do not. Deterministic, but the margin is about a factor of two and the threshold must be recalibrated whenever Clang counts steps differently.

For the compilers: GCC writes no per-evaluation trace, and its cache of constant-evaluated calls serves a repeated call from the first (note 0011, finding 3; decision 0102 item 7 measured no cost from plain locals on GCC), so a GCC form would have nothing to detect.

## Decision

1. `core.plan_evaluated_once`, registered in `test/CMakeLists.txt` only when `CMAKE_CXX_COMPILER_ID` is Clang, with the reason in its comment. It runs `test/tools/evaluated_once.py`, which runs the library's compile command (`galoot_compile_command`, through the build's launcher) on `test/core/plan_evaluated_once.cpp` with `-ftime-trace=<file> -ftime-trace-granularity=0 -c`, then reads the trace.
2. The unit reads three plans of rigid(3), motor ⟑ motor (48 terms), line ⟑ plane (15), and point ∧ line (12), each in a `static_assert` and in two functions.
3. The check passes when the median duration of the `EvaluateAsInitializer` events whose detail is `galoot::core::detail::product_plan` is below a tenth of the median duration of the `InstantiateFunction` events whose detail starts with `galoot::core::detail::make_product_plan<`. Medians, because the trace names the variable template without its arguments, so events cannot be paired, and because a median of three absorbs one interval stretched by other work. It fails, rather than passing, when either kind of event occurs fewer than three times.
4. `tools.evaluated_once_passes`, `_rejects_twice`, and `_rejects_few` run the script on three hand-written traces (copied plans pass; plans generated again fail; two plans fail as too few), so the script's own rules have tests.

## Evidence

- At `11ad0aa`, on clang-debug, clang-release, clang-asan, and lint, `core.plan_evaluated_once` passes (each preset 16 of 16 for `core.plan` and `tools.evaluated_once`). On clang-debug before the commit, on a tree that differed only in the layout of the unit's `static_assert`s: median evaluation 6 µs, median instantiation 7,265 µs, ratio 0.0008. A copy of the headers with every local of `make_product_plan` plain `constexpr`: ratios 0.5619 and 0.5612 in two compiles; it fails.
- The mutants of criterion 9 (every local plain; plain locals with an initializer that generates twice; an initializer that calls the generator twice with `static constexpr` locals, which must pass) are in `records/mutations/0013-plan-results/`, with their results.

## Consequences

- WU7b adds one `unary_map` and one conjugated plan to the unit and the script's names, or a second test for the map's variable template and generator.
- If the owner chooses plain locals for smaller GCC debug objects (decision 0102 note, open), this test fails on Clang by design; a different remedy for Clang's second evaluation would be needed first.
- A Clang change in what the trace records, or in its event names, fails the test with the counts it found, not silently.
- The check rests on a ratio of durations on a shared machine. A failure that does not reproduce is a defect of the test to trace (P10), never a rerun.
