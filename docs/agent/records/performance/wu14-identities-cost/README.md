# WU14: the compile cost of the changed and new test units

Plan 0029 criterion 14 (P1, V15; track-only, no bound applies, C14). Measured by Claude (agent, Opus 5.5, implementer of plan 0029 part C3) on 2026-11-08 from 21:15:56 to 21:25:36 UTC. The command was `tools/compiles cost units.toml out --before e076a8b --cpu 4 --repeats 3`, run in a worktree detached at `d5ffafd`, whose `include/` and `test/` are those of the unit's tip. It ran under the shared timing lock (`lock.sh take`, taken at once). The job pool showed 23 of 23 slots free before and after, the load average fell from 3.0 to 1.0 during the run, and none of this unit's builds ran then. Every compile was pinned to CPU 4, a performance core, with flags from the debug trees (gcc-debug: g++ 16.2.1; clang-debug: clang++ 23.1.4), three repeats interleaved. "Before" is `e076a8b`, the unit before part B. Rows: `out/rows.tsv`; campaign log: `out/runs.tsv`; means and ranges: `out/summary.txt`. The extracted trees and objects are not committed.

Mean retired instructions (10⁹), before → after:

| Unit | GCC | Clang |
| --- | --- | --- |
| `identities_up_to_three` | 34.25 → 55.07 (+61%) | 25.13 → 35.49 (+41%) |
| `identities_four` | 83.63 → 96.65 (+16%) | 81.00 → 91.45 (+13%) |
| `identities_five_diagonal` | 48.92 → 51.16 (+5%) | 52.03 → 55.84 (+7%) |
| `identities_conformal3` | 28.73 → 29.94 (+4%) | 30.19 → 32.09 (+6%) |
| `identities_five_dense` | 58.59 → 65.68 (+12%) | 67.21 → 72.87 (+8%) |
| `identities_six` | 39.89 → 57.38 (+44%) | 42.65 → 56.23 (+32%) |
| `identities_sparse` | 17.76 → 37.69 (+112%) | 19.69 → 35.86 (+82%) |
| `identities` | 2.56 → 2.60 (+2%) | 2.43 → 2.46 (+1%) |
| `identities_five_drawn` (new) | 37.47 | 43.66 |
| `identities_six_drawn` (new) | 27.12 | 24.37 |
| `identities_rounding` (new) | 84.41 | 69.11 |
| `dual_identities` | 137.52 → 159.21 (+16%) | 153.30 → 178.76 (+17%) |
| `dual` | 47.51 → 48.11 (+1%) | 43.77 → 44.18 (+1%) |
| `transform` | 125.70 → 135.97 (+8%) | 112.10 → 123.41 (+10%) |
| `support/tracked` | 1.62 → 1.73 (+7%) | 1.00 → 1.05 (+5%) |

- **The program `core_identities_test`, its ten sources together:** GCC 314.3 → 460.8 (+47%); Clang 320.3 → 450.3 (+41%). The growth comes from the products' associativity, the defining relation, and the new involution rules on every group (each group's source grows), from the basis-blade loops, and from the two drawn groups' sources.
- **The longest unit** of each compiler is still `dual_identities_test.cpp`, before and after, grown by 16% and 17% for the drawn R(p, q, r) forms its cases now run (a change in the longest unit, reported with its cause). Its wall time is about 10.6 s on GCC and 10.8 s on Clang on CPU 4, and its peak memory 801 MiB on GCC.
- **The largest new unit** is `identities_rounding_test.cpp`, about 5.2 s on GCC and 4.2 s on Clang. With the raised step limit, it builds the full<5> antiproduct of conformal(3) over both `double` and the tracking scalar.
- The ranges over three repeats are at most 1.2% of a mean (`out/summary.txt`), so every change above is far beyond the variation.
