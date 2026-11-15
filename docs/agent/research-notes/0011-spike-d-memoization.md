# 0011 Spike D: the form that memoizes a plan

Date: 2026-10-31. Checked by: a spike subagent (Claude Opus 5.5) working for the owner. Scope: plan 0001, section 9, Spike D. It measures which of three forms evaluates a generated table once per argument set per translation unit, and at what cost, on GCC 16.2.1 and Clang 23.1.4 (decision 0006): an inline `constexpr` variable template, a class-static `constexpr` data member, or a function-local `static constexpr`. Each is compared with 1, 4, and 16 uses of the same plan, against an unmemoized control and a calibration. Nothing here is library code.

Revision and provenance. The final runs were made on the files as kept, at revisions `aa8051c0be1d` to `63e2014f8bdc` (`develop`). `runs.tsv` marks them `-dirty` only because `tools/timed` appends to the tracked `runs.tsv`. An earlier trace run at `96eb5fc7` failed on Clang -O2 (see "Limits") and is superseded. The owner's commits during the spike (`783ff86` to `63e2014`) change governance, decisions, plans, and records only, none of which this spike reads. Sources and results are in `docs/agent/records/spikes/spike-cdf/`, not yet committed, so the figures are labeled not reproducible until they are (V10).

How each claim was checked:

- **Local**: measured on this machine with the scripts named.
- **Docs**: read in the installed libstdc++ 16 headers.
- **Inferred**: my reasoning, not checked.

The paper number given for `static constexpr` variables in `constexpr` functions (P2647) is from my own knowledge, not read in this session (V9); that the form compiles in C++23 mode is checked locally.

## Summary

1. All three forms evaluate the plan once per argument set per translation unit, on both compilers, at -O0 and -O2, for 1, 4, and 16 uses. Clang's `-ftime-trace` shows one fill evaluation at 1 and at 16 uses. Instruction counts stay flat on Clang (6.9e9 to 7.4e9) and grow only by the cost of the use functions on GCC (1.89e9 to 2.05e9 at -O0, to 2.32e9 at -O2). Each object holds one copy of the 24 KiB table. A second plan beside the first gives one table of each (Local).
2. The three forms cost the same. At 16 uses, their instruction counts lie within 1.5% of each other on each compiler and level. Peak memory agrees within 0.4 MiB, and wall times are within run-to-run noise (Local). In an earlier full sample on the same files, which differed only in comments, one Clang -O2 sample of the function-local form ran 4% high, and its repeat agreed with the others.
3. The unmemoized control, in which each use holds its own `static constexpr` table initialized by a direct call, is evaluated 16 times by Clang: 70.7e9 to 73.3e9 instructions and 3.3 s at -O0, against 7.0e9 and 0.40 s memoized. GCC evaluates it once: 2.59e9 against 2.04e9, where 16 distinct evaluations cost 5.51e9. GCC still emits 16 copies of the table (Local). Inferred cause: GCC's constexpr call cache reuses the result of the identical call `make_plan<r500, all5, all5>()`. So a memoization defect is invisible on GCC and costs 10× on Clang, and only a Clang build can detect it.
4. `consteval` and `constexpr` initializers cost the same on both compilers at both levels. The GCC -O2 double evaluation of note 0008 does not occur when the initializer calls a function template rather than a lambda (Local).
5. At -O1 and above, Clang's default step limit (it behaves as 2²⁰ here, as in note 0008; Clang's documentation was not read) stops the n = 5 full plan when it is written as a `consteval` function template; at -O0 the same code passes. The cause is libstdc++ 16's `bits/c++config.h`. It enables `_GLIBCXX_ASSERTIONS` only when `__OPTIMIZE__` is undefined; otherwise `__glibcxx_assert` becomes `if (std::__is_constant_evaluated() && !bool(cond))`, a function call on every checked access. So optimized builds take more constant-evaluation steps. Checked: Docs, and Local with `run_memo.py limits` (`out/memo/limits.tsv`): `-O0 -D__OPTIMIZE__=1` fails, `-O2 -U__OPTIMIZE__` passes, and `-O2 -fconstexpr-steps=2097152` passes. Spike A's `plan_tu.cpp`, the same generator behind a lambda, passes at -O2 under the default limit. Why it stays under was not traced (unexplained).

Recommendation for WU7 (Inferred from the measurements):

- Use an inline `constexpr` variable template, `template <class Alg, blade_set A, blade_set B> inline constexpr auto plan_v = make_plan<Alg, A, B>();`. It costs the same as the other forms, is the most direct (STYLE S16: one declaration, no class and no accessor function), is already named by S7 ("held in a `static constexpr` variable or variable template"), and Clang's trace names it ("EvaluateAsInitializer: memo::plan_v"). That serves P1's attribution.
- Every use reads the variable template, including the result-type computation. No use calls the generator directly, because on Clang each direct call is a full evaluation (finding 3).
- Give WU7 a Clang check that each argument set is evaluated once in a unit with at least two uses. `count_evals.py` counts the evaluations in a `-ftime-trace` file. A GCC build cannot see the defect.
- Keep the generator `consteval` (note 0008). It cost nothing here.
- Record the libstdc++ step-count effect (finding 5) with WU6's limits work. Note 0008's "n ≤ 5 at default limits" was measured at -O0 only.

## The unit

`memo.cpp` (in `records/spikes/spike-cdf/`) uses Spike A's `gen.hpp`. The algebra is a tag, `struct r500 { static constexpr metric<5> value = euclidean<5>(); }` (decision 0021). The plan is the full geometric product, all 32 by all 32 blades: 1024 terms of 24 bytes, a 24 KiB table. `make_plan<Alg, A, B>()` counts in a `constexpr` local, so the counting pass runs once per specialization, and then fills a `std::array`. The fill runs each time the call is evaluated, which is where the forms differ.

| `SPIKE_MEMO` | Form |
| --- | --- |
| 0 | Control: each use has `static constexpr auto plan = make_plan<…>();` |
| 1 | `template <…> inline constexpr auto plan_v = make_plan<…>();` |
| 2 | `template <…> struct plan_c { static constexpr auto value = make_plan<…>(); };` |
| 3 | `template <…> constexpr const auto& plan_f() { static constexpr auto p = make_plan<…>(); return p; }` (C++23, P2647) |
| 4 | Calibration: as 0, but each use calls a distinct specialization `make_plan<…, K>`, so no compiler can share an evaluation |

A use is a specialization `use<K, Alg, A, B>`. It reads the plan in a constant expression, as the size of an array, and loops over its terms at run time. `SPIKE_USES = U` instantiates U of them through an `integer_sequence`; U = 0 is the baseline (header and `main`).

All compiles use the S4 flags with `-Werror -ffp-contract=off`, and limits raised as in Spike A (`-fconstexpr-ops-limit=2^40`, `-fconstexpr-steps=0`, which means no limit) because of finding 5. Every form, the control included, also compiles under bare `-std=c++23 -pedantic-errors` on both compilers (Local; `out/memo/limits.tsv`).

## Measurements

Machine: Intel Core i9-14900K, Linux 7.3.0-4-cachyos, GCC 16.2.1 20260810, Clang 23.1.4, libstdc++ 16. Every measured compile was pinned with `taskset -c 6` to performance core 3, whose sibling (CPU 7) was 0.00 to 0.07 busy during each run. Compiles ran one at a time, with a one-minute load average of 0.8 to 1.0 from other work. Every counter was counted 100% of the time (no multiplexing). Peak memory comes from Spike A's `measure.py`, and counters from `perf stat` on `cpu_core` (`pinned.py`). Package energy is in the TSVs, but the machine was not idle, so it is not comparable between runs (P13). Two samples (`main`, `main2`). `make_plan` is `consteval` unless stated.

Instructions (1e9, the two samples), wall time (s, the two samples), and peak memory (MiB, first sample). From `out/memo/main.tsv` and `main2.tsv`, printed by `summarize.py d`:

| Compiler, level | Form | 1 use | 4 uses | 16 uses | Peak at 16 |
| --- | --- | --- | --- | --- | --- |
| GCC -O0 | baseline (0 uses) | 0.60, 0.60; 0.10, 0.10 | | | 64.4 |
| GCC -O0 | control | 1.92, 1.91; 0.18, 0.18 | 2.04, 2.04; 0.16, 0.15 | 2.59, 2.59; 0.21, 0.20 | 83.6 |
| GCC -O0 | variable template | 1.90, 1.91; 0.20, 0.17 | 1.94, 1.93; 0.15, 0.17 | 2.04, 2.04; 0.18, 0.19 | 83.5 |
| GCC -O0 | class static | 1.90, 1.91; 0.17, 0.21 | 1.95, 1.93; 0.16, 0.16 | 2.05, 2.05; 0.17, 0.16 | 83.5 |
| GCC -O0 | function local | 1.91, 1.90; 0.16, 0.15 | 1.93, 1.93; 0.18, 0.17 | 2.05, 2.05; 0.18, 0.18 | 83.6 |
| GCC -O2 | baseline (0 uses) | 0.60, 0.60; 0.08, 0.13 | | | 63.9 |
| GCC -O2 | control | 1.91, 1.91; 0.18, 0.19 | 2.10, 2.10; 0.23, 0.16 | 2.94, 2.93; 0.20, 0.20 | 89.4 |
| GCC -O2 | variable template | 1.90, 1.90; 0.17, 0.17 | 1.97, 1.97; 0.17, 0.18 | 2.32, 2.32; 0.20, 0.20 | 86.7 |
| GCC -O2 | class static | 1.89, 1.90; 0.14, 0.17 | 1.97, 1.97; 0.17, 0.18 | 2.31, 2.32; 0.19, 0.18 | 86.7 |
| GCC -O2 | function local | 1.90, 1.91; 0.16, 0.17 | 1.98, 1.98; 0.17, 0.17 | 2.32, 2.32; 0.20, 0.19 | 86.8 |
| Clang -O0 | baseline (0 uses) | 0.56, 0.56; 0.11, 0.11 | | | 108.6 |
| Clang -O0 | control | 6.90, 6.97; 0.42, 0.42 | 19.64, 19.61; 0.97, 0.97 | 70.74, 73.34; 3.28, 3.37 | 118.9 |
| Clang -O0 | variable template | 6.96, 6.95; 0.40, 0.38 | 6.98, 6.97; 0.40, 0.40 | 6.98, 6.95; 0.42, 0.40 | 114.0 |
| Clang -O0 | class static | 6.99, 6.94; 0.40, 0.40 | 6.92, 6.99; 0.40, 0.40 | 6.97, 6.97; 0.40, 0.41 | 114.4 |
| Clang -O0 | function local | 6.95, 6.94; 0.41, 0.39 | 7.00, 7.13; 0.43, 0.41 | 7.01, 6.98; 0.41, 0.40 | 114.3 |
| Clang -O2 | baseline (0 uses) | 0.57, 0.57; 0.10, 0.11 | | | 110.9 |
| Clang -O2 | control | 7.06, 7.13; 0.42, 0.42 | 20.15, 20.04; 1.02, 0.98 | 71.79, 73.40; 3.33, 3.39 | 125.3 |
| Clang -O2 | variable template | 7.04, 7.07; 0.40, 0.41 | 7.19, 7.11; 0.40, 0.41 | 7.30, 7.29; 0.42, 0.47 | 120.5 |
| Clang -O2 | class static | 7.15, 7.05; 0.41, 0.40 | 7.10, 7.15; 0.40, 0.41 | 7.33, 7.24; 0.47, 0.42 | 120.5 |
| Clang -O2 | function local | 7.08, 7.07; 0.40, 0.41 | 7.29, 7.11; 0.42, 0.41 | 7.35, 7.25; 0.43, 0.40 | 120.6 |

Tables in the object (`nm -S`, symbols of exactly 24,576 bytes): 1 for every memoized form at every number of uses, and U for the control, on both compilers.

### Evaluation counts

`run_memo.py trace` and `calib`; results in `out/memo/trace.tsv` and `calib.tsv`.

| Compiler | Form | Count at 1 use | Count at 16 uses | How counted |
| --- | --- | --- | --- | --- |
| Clang, -O0 and -O2, both initializers | variable template, class static, function local | 1 | 1 | `-ftime-trace`, outermost Evaluate* events over 20 ms (about 0.19 s each) |
| Clang | control | 1 | 16 | same |
| Clang | calibration | 1 | 16 (4 at 4 uses) | same |
| GCC, -O0 and -O2, both initializers | every memoized form, and the control | 0.06 s (two -O0 readings 0.09; the report resolves 0.01 s), 19 to 20 MB | 0.06 s, 19 to 22 MB | `-ftime-report`, "constant expression evaluation" |
| GCC | calibration | 0.06 s, 20 MB | 0.20 s, 79 MB (0.09 s, 32 MB at 4) | same |

- **Claim.** Clang's trace names the evaluated variable: "EvaluateAsInitializer: memo::plan_v", "EvaluateAsInitializer: memo::plan_c<memo::r500, blade_set<32>{…}, blade_set<32>{…}>::value", and for the function-local form "EvaluateAsInitializer: p" with a `constexpr` generator, but "EvaluateAsConstantExpr: <…/memo.cpp:84:29, col:50>", a source range, with a `consteval` one. The control's evaluations read "EvaluateAsInitializer: plan". Checked: Local.
- **Claim.** On GCC the calibration's 16 distinct evaluations cost 5.51e9 instructions and 129.6 MiB at -O0, against 1.92e9 and 81.1 MiB for one. The control's 2.59e9 and 83.6 MiB match one evaluation plus 16 use functions and 16 emitted tables, not 16 evaluations. Checked: Local. Inferred: GCC's constexpr call cache serves the identical call. GCC's cost per extra calibration evaluation, about 0.24e9 against 1.32e9 for the first, suggests that the cache also serves the inner `blade_product` calls, whose arguments repeat across the distinct specializations.
- **Claim.** `consteval` against `constexpr` `make_plan`, at 16 uses, mean of two samples, 1e9 instructions: GCC -O0 2.04/2.04 (variable template), 2.05/2.05 (class static), 2.05/2.05 (function local); GCC -O2 2.32/2.32, 2.31/2.32, 2.32/2.33; Clang -O0 6.97/6.98, 6.97/7.02, 7.00/6.99; Clang -O2 7.30/7.26, 7.29/7.30, 7.30/7.35. The control: GCC 2.59/2.59 and 2.93/2.94, Clang 72.0/71.4 and 72.6/72.0. No difference beyond the sample spread. Checked: Local.

### Limits

- **Claim.** Clang, default limits, n = 5 full plan as `make_plan` (consteval or constexpr): passes at -O0, and fails at -O1 and -O2 in every form with "constexpr evaluation hit maximum step limit; possible infinite loop?", at `span::size()` inside `out.empty()` of the fill pass. It fails at `-fconstexpr-steps=1048576` (2²⁰, the default) and passes at 2097152 (2²¹). Checked: Local, `run_memo.py limits` (`out/memo/limits.tsv`); first seen when the trace run at `96eb5fc7` failed.
- **Claim.** libstdc++ 16, `x86_64-pc-linux-gnu/bits/c++config.h`, lines 603 to 610: `_GLIBCXX_ASSERTIONS` is defined when neither `_GLIBCXX_DEBUG` nor `__OPTIMIZE__` nor `_GLIBCXX_NO_ASSERTIONS` is defined ("Enable assertions for unoptimized builds"). Lines 651 to 665: without it, `__glibcxx_assert(cond)` is `if (std::__is_constant_evaluated() && !bool(cond)) std::__glibcxx_assert_fail();`. Checked: Docs. Confirmed as the cause by toggling `__OPTIMIZE__` (Local). This also means that libstdc++'s run-time precondition checks are on by default in this toolchain's unoptimized builds, which bears on STYLE S5 and on reading -O0 timings (Inferred). Whether this default is upstream GCC 16 or a distribution patch was not checked.

## Reproduction

From the repository root, serially, on a quiet machine:

```bash
S=docs/agent/records/spikes/spike-cdf
tools/timed test - spike-d:run_memo-trace -- python3 -I $S/run_memo.py trace limits              # 63 s
tools/timed test - spike-d:run_memo -- python3 -I $S/run_memo.py calib main main2 second         # 117 s
python3 -I $S/summarize.py d
python3 -I $S/count_evals.py path/to/unit.json     # any Clang -ftime-trace file
```

Each measured point runs `tools/timed build - spike-d:<set>:<compiler>:<opt>:<init>:<form>:u<uses> -- taskset -c 6 python3 -I measure.py -- perf stat -x, -e <events> -- <compiler> …` through `pinned.py`. No randomness is involved, so there are no seeds.

## Open questions

1. Across translation units, an inline variable template is emitted in each object that uses it and merged by the linker. Each unit still evaluates it once. Not measured: the cost of many units, which is a property of header-only delivery rather than of the form.
2. Why Spike A's lambda form stays under Clang's -O2 step limit at n = 5 while the function form does not. Not traced.
3. Whether WU6's generator should avoid `std::span` and `std::array::operator[]` in constant evaluation, to escape the libstdc++ assertion steps. Not measured.
