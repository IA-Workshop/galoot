# Plan 0032: the build cost of one initializer per place in the kernels

Plan 0032 criterion 6 (P1, W10, V15; track-only, no bound applies, C14). Measured by Claude (agent, Opus 5.5, cpp-expert, implementer of plan 0032) on 2026-11-10 from 01:35:49 to 01:42:04 UTC, in the unit's worktree at `e8c1fa1` (whose `include/` and `test/` are those of the fix, `6d1624e`), with

```
tools/compiles cost docs/agent/records/performance/kernel-init-cost/units.toml docs/agent/records/performance/kernel-init-cost/out --before 6b8e2b6 --cpu 4 --repeats 3
tools/compiles cost docs/agent/records/performance/kernel-init-cost/units-gcc.toml docs/agent/records/performance/kernel-init-cost/out-gcc --before 6b8e2b6 --cpu 4 --repeats 3
```

under the shared timing lock (`lock.sh take`, taken at once, released at the end). "Before" is `6b8e2b6`: the old kernels with the canaries already in `mv_test.cpp`, so every difference below is the change of the kernels alone. The job pool read 23 of 23 slots free before and after; the load average was 0.6 at the start and 1.1 at the end, and none of this unit's builds or tests ran meanwhile. Every compile was pinned to CPU 4, a performance core, with the flags of the debug trees (gcc-debug: g++ 16.2.1; clang-debug: clang++ 23.1.4), three repeats interleaved; the sibling thread was busy at most 0.02 s during a compile. Rows: `out*/rows.tsv`; campaign logs: `out*/runs.tsv`; means and ranges: `out*/summary.txt`. The extracted tree, the text units, and the objects are not committed. `make_units.py` writes both unit files.

## Units

- **Sources that instantiate the kernels:** the test sources of `core.mv`, `core.linear`, `core.product`, `core.unary`, `core.transform`, `core.basis_change`, and the rga3 objects and motions.
- **Largest places.** Each text unit is one out-of-line function, so that its kernel is compiled for run time, over double and over the test rational (its header's text inlined, since the text units' commands carry no `-I` for `test/`):
  - `map`: a conversion from the scalar layout to `full<n>` (`run_map`, one term);
  - `sum`: the sum of two `full<n>` values (`run_sum`, 2 × 2ⁿ terms);
  - `apply`: the geometric product of two scalars written by `detail::apply` onto `full<n>` (`run_product`, one term, 2ⁿ places);
  - `product`: the geometric product of a scalar and a `full<9>` value (`run_product`, 512 terms and places).
- **1,024 places (`full<10>`) on GCC only.** Clang 23 builds no `mv` on `full<9>` or `full<10>` at its default constant-evaluation limits (`distinct_masks`, `mv.hpp:154`, "must be initialized by a constant expression"), so the units on both compilers stop at 256 places. The product of a scalar and `full<10>` does not build on GCC 16 either, before and after alike: the plan's result collection fails in constant evaluation (`collect_result`, `plan.hpp:313`, a `std::span` bounds check that is "not a constant expression"); it builds at `full<9>`. That failure is outside the kernels and is not investigated here.

## Mean retired instructions (10⁹), before → after; the after wall time on CPU 4 and the after peak memory (its change)

An asterisk marks a change within the larger of the two ranges over the repeats.

| Unit | GCC | Clang |
| --- | --- | --- |
| `core/mv` | 66.410 → 66.410 (+0.00%*), 4.88 s, 679 MiB (−1.0) | 59.074 → 59.191 (+0.20%*), 4.20 s, 456 MiB (+3.1) |
| `core/linear` | 42.562 → 42.715 (+0.36%), 3.02 s, 465 MiB (+17.2) | 37.992 → 38.123 (+0.35%), 2.63 s, 343 MiB (+4.1) |
| `core/product` | 32.407 → 32.406 (−0.00%*), 2.06 s, 417 MiB (+0.0) | 30.259 → 30.276 (+0.06%*), 1.92 s, 261 MiB (−0.1) |
| `core/unary` | 15.689 → 15.773 (+0.53%), 1.24 s, 338 MiB (+2.2) | 12.324 → 12.341 (+0.14%*), 1.02 s, 245 MiB (+0.9) |
| `core/transform` | 136.087 → 135.909 (−0.13%), 8.84 s, 975 MiB (+8.9) | 123.564 → 124.354 (+0.64%), 7.77 s, 543 MiB (+1.4) |
| `core/basis_change` | 9.629 → 9.681 (+0.53%), 0.77 s, 243 MiB (+0.1) | 9.044 → 9.064 (+0.22%*), 0.73 s, 187 MiB (+0.3) |
| `presets/rga3/objects` | 12.945 → 12.940 (−0.04%*), 0.97 s, 296 MiB (+0.4) | 11.844 → 11.892 (+0.41%), 0.89 s, 210 MiB (+0.4) |
| `presets/rga3/motions` | 14.765 → 14.781 (+0.11%), 1.35 s, 342 MiB (+0.6) | 10.943 → 10.968 (+0.23%), 1.03 s, 286 MiB (+0.1) |
| `places256/map_double` | 2.704 → 2.723 (+0.69%), 0.20 s, 108 MiB (+2.1) | 2.668 → 2.670 (+0.09%*), 0.23 s, 130 MiB (−0.1) |
| `places256/map_rational` | 2.738 → 2.760 (+0.79%), 0.21 s, 108 MiB (−0.0) | 2.668 → 2.710 (+1.55%*), 0.23 s, 130 MiB (−0.2) |
| `places256/sum_double` | 6.282 → 6.305 (+0.36%), 0.42 s, 145 MiB (+0.1) | 6.622 → 6.637 (+0.23%*), 0.41 s, 130 MiB (+0.1) |
| `places256/sum_rational` | 6.334 → 6.366 (+0.50%), 0.42 s, 147 MiB (+2.4) | 6.692 → 6.663 (−0.43%*), 0.42 s, 132 MiB (−0.1) |
| `places256/apply_double` | 2.754 → 2.777 (+0.82%), 0.21 s, 109 MiB (+0.0) | 2.879 → 2.865 (−0.46%*), 0.24 s, 132 MiB (+0.5) |
| `places256/apply_rational` | 2.811 → 2.829 (+0.62%), 0.22 s, 111 MiB (+0.1) | 2.881 → 2.901 (+0.71%*), 0.24 s, 134 MiB (+0.4) |
| `places1024/map_double` | 24.158 → 24.270 (+0.46%), 1.06 s, 321 MiB (−0.1) | not built (above) |
| `places1024/map_rational` | 24.342 → 24.357 (+0.06%*), 1.08 s, 321 MiB (+0.3) | not built |
| `places1024/sum_double` | 65.632 → 65.860 (+0.35%), 3.59 s, 418 MiB (+0.5) | not built |
| `places1024/sum_rational` | 65.649 → 65.790 (+0.21%), 3.56 s, 418 MiB (+0.1) | not built |
| `places1024/apply_double` | 24.338 → 24.495 (+0.64%), 1.07 s, 321 MiB (−0.0) | not built |
| `places1024/apply_rational` | 24.396 → 24.472 (+0.31%), 1.08 s, 322 MiB (+0.6) | not built |
| `terms512/product_double` | 32.161 → 32.157 (−0.01%*), 1.63 s, 376 MiB (+0.2) | not built |
| `terms512/product_rational` | 32.124 → 32.292 (+0.52%*), 1.64 s, 380 MiB (−0.1) | not built |

## Reading

- **No unit grows notably.** The plan fixed, before measuring, that a growth above 5% in a largest-place unit or above 1% in an existing test source would be reported and would bring form B into question. The largest growth beyond the variation is +0.82% (GCC, `places256/apply_double`); at 1,024 places it is at most +0.64% (GCC, `apply_double`). The existing sources change by at most +0.64% (Clang, `core/transform`). Form A stays, and form B was not measured.
- **The cost of the helper is small next to what a kernel already costs at those sizes.** At 1,024 places the conversion's whole unit is 24 × 10⁹ instructions, nearly all of it the layout and the kernel's terms; the change adds about 0.11 to 0.23 × 10⁹ to the double units (the differences of the means; not attributed by a time report).
- **Peak memory** changes by at most 3 MiB, except GCC's `core/linear` (+17.2 MiB, 3.7%) and `core/transform` (+8.9 MiB, 0.9%). Peak memory is the largest of the three repeats; its variation over repeats was not measured separately, so whether these two are beyond it is not known.
- The ranges over three repeats are at most 2.0% of a mean (`out*/summary.txt`, Clang's `places256/map_rational`), and under 0.5% for the existing sources.
- **Test time.** The unit adds two test cases to `core_mv_test` and changes no other test; their time was not measured separately. The suites' times are in the run log rows of the P9 matrix.
