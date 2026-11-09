# Plan 0031: the compile cost of the changed test units

Plan 0031 criterion 7 (P1, V15; track-only, no bound applies, C14). Measured by Claude (agent, Opus 5.5, implementer of plan 0031) on 2026-11-09 from 04:20:47 to 04:29:45 UTC with `tools/compiles cost units.toml out --before 774ff66 --cpu 4 --repeats 3`, in the unit's worktree at `0e7c247`, whose `include/` and `test/` are the unit's tip. It ran under the shared timing lock (`lock.sh take`, taken at once). The job pool showed 23 of 23 slots free before and after; the load average was 0.9 at the start and 1.9 at the end, and none of this unit's other builds or tests ran meanwhile. Every compile was pinned to CPU 4, a performance core, with the flags of the debug trees (gcc-debug: g++ 16.2.1; clang-debug: clang++ 23.1.4), three repeats interleaved. "Before" is `774ff66`, the claim, whose tests are those of `develop` at `ba12e5a`. Rows: `out/rows.tsv`; campaign log: `out/runs.tsv`; means and ranges: `out/summary.txt`. The extracted tree and the objects are not committed.

Mean retired instructions (10⁹), before → after, and the after wall time on CPU 4:

| Unit | GCC | Clang |
| --- | --- | --- |
| `identities_up_to_three` | 55.12 → 65.31 (+18.5%), 5.19 s | 35.51 → 42.24 (+19.0%), 3.54 s |
| `identities_four` | 96.72 → 99.06 (+2.4%), 6.32 s | 91.61 → 92.71 (+1.2%), 5.77 s |
| `identities_five_diagonal` | 51.17 → 51.72 (+1.1%), 3.15 s | 55.54 → 56.38 (+1.5%), 3.37 s |
| `identities_conformal3` | 29.93 → 30.32 (+1.3%), 1.92 s | 32.09 → 32.44 (+1.1%), 2.02 s |
| `identities_five_dense` | 65.64 → 65.74 (+0.2%), 4.04 s | 72.86 → 73.14 (+0.4%), 4.41 s |
| `identities_six` | 57.44 → 57.41 (−0.1%), 3.57 s | 56.30 → 56.18 (−0.2%), 3.53 s |
| `identities_sparse` | 37.69 → 37.74 (+0.1%), 2.72 s | 35.87 → 36.01 (+0.4%), 2.51 s |
| `identities_five_drawn` | 37.55 → 38.29 (+2.0%), 2.33 s | 43.65 → 44.11 (+1.1%), 2.60 s |
| `identities_six_drawn` | 27.14 → 27.16 (+0.1%), 1.86 s | 24.34 → 24.46 (+0.5%), 1.70 s |
| `identities` | 2.60 → 2.66 (+2.3%), 0.28 s | 2.46 → 2.50 (+1.8%), 0.32 s |
| `dual_identities` | 158.96 → 162.41 (+2.2%), 10.82 s | 179.06 → 181.21 (+1.2%), 11.64 s |
| `blade_product` | 8.77 → 8.84 (+0.8%), 0.78 s | 5.90 → 5.93 (+0.5%), 0.59 s |
| `plan` | 32.24 → 32.25 (+0.0%), 2.54 s | 35.36 → 35.42 (+0.2%), 2.53 s |

- **`core_identities_test`, its ten sources together:** GCC 461.0 → 475.4 (+3.1%); Clang 450.2 → 460.2 (+2.2%).
- **The growth is in `identities_up_to_three_test.cpp`** (+18.5% and +19.0%): it now builds every product and map of the two new forms' full layouts, and the generator-rule and recursion checks for its 24 forms. Each full-layout group's source grows by the checking code alone (1 to 2.4%); the groups on vectors, bivectors, or sparse sets do not build the checks and are unchanged within their ranges.
- **`dual_identities_test.cpp`** grows by 2.2% and 1.2% for the two forms; it stays the longest unit of this table (about 10.8 s and 11.6 s).
- **`blade_product_test.cpp` and `plan_test.cpp`** read the two forms at run time through `for_each_signature`, so their cost does not change beyond the variation.
- The ranges over three repeats are at most 1.3% of a mean (`out/summary.txt`), so changes below about 1% (the six, sparse, dense, six-drawn, and plan units) are within the variation.
