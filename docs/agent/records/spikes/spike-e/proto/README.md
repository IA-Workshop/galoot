# Spike E, part F: a prototype collection pass

Plan 0038 part F (section 4 item 8), run by Claude (agent, Opus 5.5) in the performance-engineer role on 2026-11-10, branch `research/spike-e--bench` from `develop` at `d0bbe49`. Nothing under `include/` changed; the prototype is `collect.hpp` here, which includes the library's `transform.hpp` and reads its two kernels.

## Departures, skipped checks, and ambiguities (C20)

1. **"The existing plan's cost"** is read as the threshold of a unit that builds the transform's two kernels (both antiproducts' plans and the kernels drawn from them), and the pass's cost as the threshold of a unit that runs the pass alone on the kernels written as literals, as a phase probe of the cost model holds its inputs as literals. A unit with the plan and the pass together has the larger of the two as its threshold, which hides whichever is smaller; that unit is measured too (`cost/thresholds.tsv`, unit `pass`).
2. **No weights for the pass.** The cost model's weights cover the library's generators, not this code, so the pass is measured (thresholds by `harness.py`'s checked search, compiler instructions by `tools/compiles cost`) and not predicted. The plans are both: the measured plan thresholds equal `cost.py predict`'s largest phase (`fill` of the second antiproduct) in all 18 (case, compiler) pairs (`cost/predictions.tsv`).
3. **Which n = 5 layouts.** The plan names "conformal(3) motors" and no object. The units use the motor of `transform.hpp`'s comment (e415 e425 e435 e12345 e235 e315 e125 e5) on x of each grade 1 to 4, and as a stress case the even versor on all 16 blades of even antigrade on grades 1 and 2.
4. **Clang refuses one unit at its default step limit**, the 16-blade versor on grade 2 with the pass (below). The first `tools/compiles` run stopped at that compile; its rows are in `cost/out-refused/runs.tsv`, and the unit was moved to a GCC-only spec (`cost/units-gcc.toml`) for the second run.
5. **Machine.** Thresholds were searched unpinned with eight pool slots; the counts are deterministic (a second full search gave every non-baseline threshold the same). The first search's baseline units failed on Clang for an unused-variable warning under `-Werror`, not for a limit; they were given a trivial use and every unit searched again. `tools/compiles cost` ran pinned to CPU 4 under the lock (2026-11-11 01:21:12 to 01:21:50 UTC), three repeats, with the one-minute load average at 2.8 to 3.3 from other users, so its wall times are less reliable than its instruction counts (`cost/protocol.log`).
6. **Formatting after measurement.** `collect.hpp` and `cost/dump.cpp` were run through `clang-format` after the measurements; no statement changed, and the rebuilt probe's assembly and the dumped kernels (`cost/cases.json`) are byte for byte the same.
7. **Not done:** no sparse version of the pass, so the remedy named below for the 16-blade case is inferred; the instantiation cost of the pass's run-time kernels is not separated from its evaluation (both are inside the `tools/compiles` figures).

## Why C++ constexpr, not Python

The question is what collection would cost the library at compile time. Only a pass that runs where the library's generators run, in constant evaluation, can be measured by the cost model's method (thresholds by `harness.py`'s checked search, compiler instructions by `tools/compiles cost`). A Python pass would show the algorithm, which `../forms.py` already does, and nothing about compile cost. The pass reads the library's own kernels (`detail::first_kernel<Alg, V, X, false>` and `detail::second_kernel<Alg, V, X, true>`), so its input is exactly what `transform` computes, and it works for any algebra and layouts, not only rigid(3).

## What the pass does

`summarize` composes the two kernels, y_o = Σ c1 c2 v_a v_b x_j, into a dense table C[o][j][p ≤ q] of exact integer coefficients (the first kernel's terms bucketed by middle place, so the composition costs one step per composed term), then for each entry M[o][j] divides out the greatest common divisor of its coefficients (the entry's scale, a doubling when it is 2), makes its first coefficient positive by carrying the sign into the result's sum, shares an entry equal to an earlier one (a fingerprint compared before the row), and numbers each product v_p v_q that some entry reads. `make_collected` writes that as three kernels (products, entries, result), each sum starting from a positive term where it has one; `setup` and `apply` run them with always-inlined fold expressions, as the library's `run_product` does.

Two assumptions the library's `transform` does not make, and which a core version would have to state (C8): the scalar's multiplication commutes (v_p v_q and v_q v_p are one product), and the sums are evaluated in a different grouping, so the result differs from `transform`'s in rounding, though not in exact value.

## Correctness

`../bench/check.cpp` (output `../bench/out/check.*.txt`, both compilers): over the test rational, the pass's form equals the sandwich computed from the library's full antiproducts for 200 random and 200 unit motors and each of the point, line, and plane, and equals `galoot::core::transform` for 50 random motors on e415 e425 e435 e12345 e235 e315 e125 e5 in conformal(3) with x of each grade 1 to 4. Over double, `../bench/<program> check` finds the pass's form within 1.2e-15 of the library's result, relative to its largest coefficient.

## Operation counts

From `collect::count` (printed by `check.cpp`), against `../counts.md`'s form B (rule S):

| Case | Composed terms | Setup mul, add, scl (other literals) | Apply mul, add | Form B of `../forms.py` |
| --- | ---: | --- | --- | --- |
| rigid(3) point | 64 | 22, 27, 9 | 13, 9 | 22, 26, 9; 13, 9 |
| rigid(3) line | 144 | 26, 42, 15 | 27, 21 | 26, 41, 15; 27, 21 |
| rigid(3) plane | 64 | 22, 27, 9 | 13, 9 | 22, 26, 9; 13, 9 |
| conformal(3) motor, grade 1 | 100 | 26, 39, 13 | 18, 13 | not derived |
| conformal(3) motor, grade 2 | 376 | 36, 92, 30 (6) | 61, 51 | not derived |
| conformal(3) motor, grade 3 | 376 | 36, 86, 28 (6) | 61, 51 | not derived |
| conformal(3) motor, grade 4 | 100 | 26, 39, 13 | 18, 13 | not derived |

No negation in any case. The pass's setup has one addition more than `forms.py`'s form B in each rigid(3) case: `forms.py` builds each entry's sum as a left fold over shared nodes, so two entries whose first two terms are the same share that partial sum, and the pass shares only whole entries. The "other literals" are coefficients ±2 left inside an entry whose other coefficients are ±1, each a product by the literal (it could be a doubling of the product instead; the pass does not do that).

## Compile cost

Commands, from the worktree's root, with the pool's environment set and `TIMED_LOG` naming `cost/runs.tsv` (every compile, configure, and search through `tools/timed`; the last two under the lock):

```bash
g++ -DGALOOT_CHECKING=0 -Iinclude -O0 -std=c++23 docs/agent/records/spikes/spike-e/proto/cost/dump.cpp -o build/spike-e-bench/dump
build/spike-e-bench/dump > docs/agent/records/spikes/spike-e/proto/cost/cases.json
python3 -I docs/agent/records/spikes/spike-e/proto/cost/units.py
python3 -I docs/agent/records/spikes/spike-e/proto/cost/predict.py
python3 -I docs/agent/records/spikes/spike-e/proto/cost/thresholds.py docs/agent/records/spikes/spike-e/proto/cost/thresholds.tsv 8
cmake --preset gcc-debug; cmake --preset clang-debug    # for the flags tools/compiles reads
tools/compiles cost docs/agent/records/spikes/spike-e/proto/cost/units.toml docs/agent/records/spikes/spike-e/proto/cost/out --before d0bbe49 --cpu 4 --repeats 3
tools/compiles cost docs/agent/records/spikes/spike-e/proto/cost/units-gcc.toml docs/agent/records/spikes/spike-e/proto/cost/out-gcc --before d0bbe49 --cpu 4 --repeats 3
```

Flags: the thresholds take `harness.py`'s `galoot-O0` (`-O0 -DGALOOT_CHECKING=1 -D_GLIBCXX_ASSERTIONS` and the warning flags), GCC with its call cache off (`-fconstexpr-cache-depth=1`, "cold", as the model is fitted) and on ("warm"); `tools/compiles` takes the debug trees' flags of `core.assert_consteval`.

### Thresholds: the largest constant evaluation

The plan's threshold (largest evaluation of the transform's two plans and kernels) against the pass's (the pass alone, on literal kernels; its largest evaluation is `summarize` on both compilers). The compilers' default limits are 33,554,432 (GCC) and 1,048,576 (Clang).

| Case | GCC cold: plan | pass | ratio | GCC warm: plan | pass | Clang: plan | pass | ratio |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| rigid(3) motor, point | 213,058 | 446,426 | 2.10 | 48,253 | 426,478 | 39,811 | 69,953 | 1.76 |
| rigid(3) motor, line | 213,066 | 887,161 | 4.16 | 48,253 | 842,803 | 39,811 | 139,513 | 3.50 |
| rigid(3) motor, plane | 213,058 | 449,396 | 2.11 | 48,253 | 429,200 | 39,811 | 70,423 | 1.77 |
| conformal(3) motor, grade 1 | 659,778 | 645,054 | 0.98 | 229,737 | 613,722 | 122,687 | 101,417 | 0.83 |
| conformal(3) motor, grade 2 | 659,794 | 2,210,242 | 3.35 | 229,737 | 2,089,312 | 122,687 | 350,359 | 2.86 |
| conformal(3) motor, grade 3 | 659,778 | 2,215,628 | 3.36 | 229,737 | 2,094,686 | 122,687 | 351,437 | 2.86 |
| conformal(3) motor, grade 4 | 659,794 | 646,702 | 0.98 | 229,737 | 615,354 | 122,687 | 101,722 | 0.83 |
| conformal(3) even versor (16), grade 1 | 1,274,610 | 3,095,690 | 2.43 | 452,633 | 2,973,264 | 237,071 | 481,823 | 2.03 |
| conformal(3) even versor (16), grade 2 | 1,274,594 | 12,261,602 | 9.62 | 452,633 | 11,780,212 | 237,071 | 1,913,176 | 8.07 |

So the pass is a new evaluation 1.8 to 4.2 times as costly as the plan's largest in rigid(3), and 0.8 to 3.4 times at n = 5 for the motor. It fits every default limit but one: **at n = 5 the pass is feasible for conformal(3) motors on every grade on both compilers** (largest 2.2 M of GCC's 33.6 M; 351 k of Clang's 1.05 M, a third of it), **and infeasible at Clang's default limit for the 16-blade even versor on grade 2** (1.91 M steps, 1.8 times the limit; GCC fits, at 37% of its limit).

Why: `summarize` is dense. It scans a table of X² · V² coefficients (X the object's blades, V the versor's) several times, so its cost follows that size, not the number of composed terms: Clang's threshold divided by X² V² is 68, 61, 55, and 75 steps for the point (1,024 cells), the line (2,304), the motor on grade 2 (6,400), and the 16-blade versor on grade 2 (25,600). The composition itself is linear in its terms (64 to 376 here). A sparse table, the composed terms sorted by (o, j, p, q) and merged, would cost about T log T for T composed terms instead, and would bring the 16-blade case under Clang's limit (inferred from the scaling, not built).

### Compiler instructions, time, and memory

Retired compiler instructions of the plan unit minus the baseline unit (the same algebra and layouts, no plan), and of the pass unit minus the plan unit, mean of three pinned compiles (ranges in `cost/out/summary.txt`); wall time and peak memory of the plan and pass units.

| Case | GCC: plan | + pass | wall plan, with pass | peak memory | Clang: plan | + pass | wall plan, with pass | peak memory |
| --- | ---: | ---: | --- | --- | ---: | ---: | --- | --- |
| rigid(3) motor, point | 278 M | 304 M | 0.156, 0.171 s | 89, 94 MiB | 682 M | 290 M | 0.199, 0.218 s | 122, 124 MiB |
| rigid(3) motor, line | 292 M | 543 M | 0.157, 0.185 s | 89, 96 MiB | 782 M | 539 M | 0.211, 0.236 s | 122, 125 MiB |
| rigid(3) motor, plane | 290 M | 309 M | 0.156, 0.172 s | 89, 94 MiB | 704 M | 298 M | 0.203, 0.216 s | 122, 124 MiB |
| conformal(3) motor, grade 1 | 636 M | 415 M | 0.174, 0.198 s | 93, 100 MiB | 1,541 M | 403 M | 0.244, 0.267 s | 122, 124 MiB |
| conformal(3) motor, grade 2 | 779 M | 1,284 M | 0.181, 0.243 s | 95, 111 MiB | 1,874 M | 1,315 M | 0.260, 0.340 s | 122, 130 MiB |
| conformal(3) motor, grade 3 | 757 M | 1,259 M | 0.181, 0.249 s | 95, 111 MiB | 1,875 M | 1,328 M | 0.266, 0.344 s | 122, 130 MiB |
| conformal(3) motor, grade 4 | 648 M | 421 M | 0.172, 0.199 s | 93, 100 MiB | 1,539 M | 410 M | 0.249, 0.270 s | 122, 124 MiB |
| conformal(3) even versor (16), grade 1 | 1,120 M | 1,908 M | 0.198, 0.291 s | 100, 121 MiB | 2,882 M | 1,896 M | 0.313, 0.423 s | 123, 132 MiB |
| conformal(3) even versor (16), grade 2 | 1,483 M | 7,522 M | 0.223, 0.599 s | 104, 187 MiB | 3,594 M | refused | 0.348 s, - | 123 MiB, - |

In instructions the pass adds about as much again as the plan on GCC in rigid(3) (1.1 to 1.9 times the plan's own) and 0.4 to 0.7 times on Clang; at n = 5 for the motor, 0.3 to 1.7 times; per unit, 0.013 to 0.08 s of wall time and up to 16 MiB of memory. A program would pay this once for each (versor layout, object layout) pair it transforms, as it pays the plans.
