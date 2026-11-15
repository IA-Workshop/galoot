# Plan 0040: build cost before and after the pre-release core pass (W10)

Plan 0040 section 9. Measured by Claude (agent, Opus 5.5, implementer 1 of plan 0040) on 2026-11-11 from 04:37:33 to 04:46:20 UTC, in the worktree of `feature/pre-release-core-pass` at `7a27f76` (parts A to E merged), with

```
tools/compiles cost docs/agent/records/performance/pre-release-core-pass/cost/units.toml \
  docs/agent/records/performance/pre-release-core-pass/cost/out --before 8058589 --cpu 4 --repeats 3
```

under the timing lock (`lock.sh take`, taken at once with no queue; released at the end), and nothing else of this workstream running.

- Before: `8058589`, the feature's branch point, whose `include/` is that of `77cd947`. After: `7a27f76`. No unit's source changed between them, so each difference is the headers': the forced inlining, the deleted `==` and `inverse_change`, and the comments.
- Units (`units.toml`): `#include <galoot/core.hpp>` alone; E1's probe as a text unit; `test/core/transform_test.cpp`, `product_test.cpp`, `identities_test.cpp`, `basis_change_conformal_test.cpp`; `test/presets/rga3/chains_test.cpp`. Each in the debug trees (`-O0 -g`, checking on, `_GLIBCXX_ASSERTIONS`) and the measure trees (`-O2`, checking off), with the flags those trees give the source (the text units the flags of `core.plan_map_term_zero_n`). Seven units, two kinds, two compilers, two trees, three repeats: 168 compiles.
- Toolchain: g++ 16.2.1 20260810, clang++ 23.1.4, libstdc++ 16; host ds-001; every compile pinned to CPU 4 (a performance core) with `taskset`; its sibling thread busy at most 0.01 s and other work on CPU 4 at most 0.05 s per compile (`out/rows.tsv`, columns `sibling_busy_s` and `pinned_other_s`). Pool 23 of 23 slots free and load average 1.78 before, 23 of 23 and 2.05 after (`conditions.txt`).
- Files: `out/rows.tsv` (every compile), `out/summary.txt` (per compiler, unit and tree: mean and range of retired user-space instructions, mean wall time, largest peak memory, the command), `out/runs.tsv` (the campaign log), `out/units/` (the two text units). The extracted before tree and the objects are not kept.

## Before and after

Instructions are the mean of the three repeats, in millions, with the range of the repeats; wall time is the mean; peak memory the largest of the three.

| Kind | Compiler | Unit | Inst. before (10⁶) | Range | Inst. after (10⁶) | Range | Change | > 2 × larger range | > 5% | Wall before (s) | Wall after (s) | Peak before (MiB) | Peak after (MiB) |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | --- | --- | ---: | ---: | ---: | ---: |
| debug | GCC | `includer/core` | 994.8 | 0.1 | 994.4 | 0.3 | -0.04% | no | no | 0.138 | 0.131 | 81 | 81 |
| debug | GCC | `probe/e1` | 4,829.7 | 3.9 | 5,194.3 | 1.3 | +7.55% | yes | yes | 0.408 | 0.425 | 155 | 173 |
| debug | GCC | `core/transform` | 136,152.5 | 18.2 | 183,970.3 | 143.3 | +35.12% | yes | yes | 9.187 | 13.062 | 972 | 1422 |
| debug | GCC | `core/product` | 32,475.3 | 64.6 | 33,611.1 | 34.5 | +3.50% | yes | no | 2.162 | 2.266 | 418 | 398 |
| debug | GCC | `core/identities` | 2,664.8 | 0.7 | 2,664.3 | 0.5 | -0.02% | no | no | 0.298 | 0.300 | 166 | 166 |
| debug | GCC | `basis_change/conformal` | 55,052.3 | 65.3 | 55,894.4 | 30.8 | +1.53% | yes | no | 3.862 | 3.877 | 434 | 505 |
| debug | GCC | `rga3/chains` | 4,389.7 | 3.2 | 5,153.6 | 1.7 | +17.40% | yes | yes | 0.377 | 0.425 | 173 | 190 |
| debug | Clang | `includer/core` | 997.8 | 0.6 | 998.2 | 3.0 | +0.04% | no | no | 0.152 | 0.152 | 114 | 114 |
| debug | Clang | `probe/e1` | 4,426.9 | 14.1 | 4,501.1 | 6.9 | +1.68% | yes | no | 0.392 | 0.395 | 151 | 152 |
| debug | Clang | `core/transform` | 124,105.7 | 292.9 | 124,570.2 | 161.2 | +0.37% | no | no | 8.035 | 7.957 | 544 | 547 |
| debug | Clang | `core/product` | 30,248.4 | 97.5 | 30,714.7 | 144.2 | +1.54% | yes | no | 2.000 | 2.022 | 263 | 262 |
| debug | Clang | `core/identities` | 2,501.1 | 5.8 | 2,497.6 | 1.6 | -0.14% | no | no | 0.328 | 0.330 | 170 | 170 |
| debug | Clang | `basis_change/conformal` | 55,583.8 | 155.0 | 56,945.1 | 52.4 | +2.45% | yes | no | 3.670 | 3.768 | 312 | 312 |
| debug | Clang | `rga3/chains` | 4,327.5 | 5.2 | 4,566.8 | 4.6 | +5.53% | yes | yes | 0.383 | 0.396 | 169 | 169 |
| measure | GCC | `includer/core` | 940.1 | 0.2 | 939.9 | 0.6 | -0.02% | no | no | 0.122 | 0.137 | 74 | 74 |
| measure | GCC | `probe/e1` | 3,398.7 | 3.6 | 3,452.7 | 2.3 | +1.59% | yes | no | 0.315 | 0.314 | 138 | 138 |
| measure | GCC | `core/transform` | 184,343.4 | 26.2 | 247,029.5 | 13.6 | +34.01% | yes | yes | 14.235 | 17.575 | 796 | 779 |
| measure | GCC | `core/product` | 31,959.1 | 22.7 | 32,788.5 | 31.1 | +2.60% | yes | no | 2.179 | 2.216 | 345 | 343 |
| measure | GCC | `core/identities` | 2,707.5 | 1.8 | 2,705.9 | 1.2 | -0.06% | no | no | 0.295 | 0.296 | 155 | 155 |
| measure | GCC | `basis_change/conformal` | 83,832.1 | 38.8 | 93,725.0 | 50.8 | +11.80% | yes | yes | 5.808 | 6.170 | 418 | 423 |
| measure | GCC | `rga3/chains` | 12,915.7 | 2.0 | 11,097.0 | 2.7 | -14.08% | no | no | 1.026 | 0.858 | 199 | 192 |
| measure | Clang | `includer/core` | 995.6 | 1.8 | 993.7 | 1.0 | -0.19% | no | no | 0.151 | 0.159 | 114 | 114 |
| measure | Clang | `probe/e1` | 4,321.8 | 11.4 | 4,319.4 | 14.0 | -0.05% | no | no | 0.391 | 0.388 | 154 | 154 |
| measure | Clang | `core/transform` | 209,757.6 | 529.2 | 253,179.9 | 308.0 | +20.70% | yes | yes | 15.920 | 19.626 | 527 | 527 |
| measure | Clang | `core/product` | 34,421.7 | 122.5 | 33,587.6 | 62.8 | -2.42% | no | no | 2.335 | 2.292 | 254 | 253 |
| measure | Clang | `core/identities` | 2,525.3 | 2.2 | 2,520.1 | 3.7 | -0.21% | no | no | 0.335 | 0.332 | 173 | 173 |
| measure | Clang | `basis_change/conformal` | 86,438.9 | 55.1 | 88,781.1 | 187.4 | +2.71% | yes | no | 6.258 | 6.324 | 306 | 306 |
| measure | Clang | `rga3/chains` | 5,101.5 | 0.3 | 7,035.9 | 5.7 | +37.92% | yes | yes | 0.456 | 0.564 | 172 | 172 |

## The knob criterion

Fixed in plan 0040 section 9 before any run: the debug-build cost is notable when, for any debug unit, the mean of retired instructions after exceeds the mean before by more than twice the larger of the two ranges of the repeats, and by more than 5%.

**It is met**, by four of the seven debug units:

| Compiler | Unit | Change | Twice the larger range (10⁶) | Increase (10⁶) |
| --- | --- | ---: | ---: | ---: |
| GCC | `core/transform` | +35.12% | 286.7 | 47,817.8 |
| GCC | `rga3/chains` | +17.40% | 6.5 | 763.9 |
| GCC | `probe/e1` | +7.55% | 7.8 | 364.6 |
| Clang | `rga3/chains` | +5.53% | 10.4 | 239.3 |

So, as the plan says, no knob is added; the numbers go to the owner as O2. GCC's debug compile of `transform_test.cpp` also takes 42% longer (9.19 s to 13.06 s) and 46% more peak memory (972 MiB to 1,422 MiB). Clang's debug units grow less than GCC's (at most 5.5%), and its `transform_test.cpp` by 0.37%, inside twice its range.

## What else the table shows

- The text unit `#include <galoot/core.hpp>` costs the same before and after on both compilers and both kinds (within 0.2%, inside its ranges): the macro, the deleted `==` and the trimmed comments add nothing measurable to parsing.
- In the measure trees, where the forced inlining is the point, the change is mixed: `transform_test.cpp` grows 34.0% on GCC and 20.7% on Clang, `chains_test.cpp` grows 37.9% on Clang and shrinks 14.1% on GCC, `basis_change_conformal_test.cpp` grows 11.8% on GCC and 2.7% on Clang, and E1's probe is within 1.6%. The plan's criterion reads only the debug units; these are tracked (C14), not judged.
- `identities_test.cpp` is unchanged within 0.21% everywhere.
- Inferred, not traced with the compilers' time reports: the growth is in code generation and optimization of the bodies now copied into each call site, since parsing is unchanged; a trace (`-ftime-report`, `-ftime-trace`) of `transform_test.cpp` would attribute it (P1).

## Code size and test times

The text size of E1's probe is in `../object-sizes/` (at `-O0`, GCC +10.3% and Clang -1.3%; at `-O2`, GCC -0.4% and Clang -8.8%; the same at `7a27f76` as at `d382516`).

Test times, from the `gate:all` workflow rows of the run log (configure, build and tests from empty trees, on the shared pool, so recorded and not compared): the last matrix on `develop` before the branch, `merge/wu17-gate` at `43cc6da`, against this unit's matrix at the merge of `--code` (staged on `8058589`, rows `805858966ecd-dirty`). This unit's final matrix, before its merge into `develop`, has not run yet; its rows are the ones the plan names.

| Preset | `43cc6da` (s) | `--code` merge (s) |
| --- | ---: | ---: |
| gcc-debug | 98.7 | 142.0 |
| gcc-release | 111.6 | 148.3 |
| gcc-asan | 227.6 | 647.4 |
| clang-debug | 92.8 | 99.1 |
| clang-release | 117.0 | 149.0 |
| clang-asan | 201.1 | 310.8 |
| lint | 267.6 | 255.2 |
| gcc-measure | not run | 149.6 |
| clang-measure | 113.1 | 142.8 |

The `--code` matrix ran on a pool other workstreams were also using (its merge message gives load average 1.5 before and 6.1 after), so its larger times are not attributed to the change; the campaign above is the measurement.
