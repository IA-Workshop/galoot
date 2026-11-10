# Plan 0039 build cost: before and after the reduced rows

Plan 0039 definition of done item 6 (W10, P1). Run by Claude (agent, Opus 5.5, implementer of plan 0039) on 2026-11-10 at `42b2c22` in the unit's worktree:

```bash
tools/compiles cost docs/agent/records/performance/basis-change-rows-cost/units.toml \
  docs/agent/records/performance/basis-change-rows-cost/out --before 3ddc371 --cpu 4 --repeats 3
```

under the shared timing lock (`lock.sh take`, taken at once, released at the end). "Before" is `3ddc371`, the branch point; the three sources are the same in both trees, so each difference is the header's. Every compile was pinned to CPU 4, a performance core, with the flags of the debug trees (gcc-debug: g++ 16.2.1; clang-debug: clang++ 23.1.4), three repeats interleaved. The machine was not idle: other workstreams ran, and the load average read 4.0 at the start and 4.1 at the end; the job pool read 23 of 23 slots free. Rows: `out/rows.tsv`; campaign log: `out/runs.tsv`; means and ranges: `out/summary.txt`. The extracted tree, the objects, and the console output are not committed.

## Mean retired instructions (10⁹), before → after, and peak memory after

| Unit | GCC | Clang | Peak memory after, GCC / Clang (MiB) |
| --- | --- | --- | --- |
| `basis_change_conformal_test.cpp` | 55.039 → 55.032 (−0.01%) | 55.532 → 55.558 (+0.05%) | 434 / 312 |
| `basis_change_random_test.cpp` | 47.858 → 47.863 (+0.01%) | 40.112 → 40.104 (−0.02%) | 492 / 303 |
| `basis_change_witt_test.cpp` | 46.779 → 46.819 (+0.09%) | 42.245 → 42.223 (−0.05%) | 477 / 314 |

Every difference is within about one range of the repeats (up to 0.4% of the mean for Clang's conformal unit after), so the reduction's n² gcds per change tag cost nothing measurable here. Wall times moved by at most 0.13 s, on a loaded machine; they are recorded, not compared.

Test time (P1): the 50 seeds of `core_basis_change_test` and `core_basis_change_identities_test` at the tip took a median 0.031 s and 0.191 s (`records/mutations/0039-basis-change-rows/seeds/runs.tsv`); at the WU12 tip they took 0.056 s and 0.173 s on the same preset, with a different `core_basis_change_test`, so the figures are noted, not compared.
