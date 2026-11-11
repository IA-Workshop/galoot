# Spike E, part F: a prototype collection pass

Plan 0038 part F (section 4 item 8), run by Claude (agent, Opus 5.5) in the performance-engineer role on 2026-11-10, branch `research/spike-e--bench` from `develop` at `d0bbe49`. Nothing under `include/` changed; the prototype is `collect.hpp` here, which includes the library's `transform.hpp` and reads its two kernels.

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
