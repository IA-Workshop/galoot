# Plan 0040: build cost before the pre-release core pass and with the public operations forced (W10, second campaign)

Plan 0040 section 9, run again because the blind review's M1 changed the chain: eleven public functions (`geometric_product`, `wedge`, `geometric_antiproduct`, `antiwedge`, the operators `*`, `^`, `%`, `&`, `weight`, `left_weight_dual`, `antidot`) now carry `GALOOT_ALWAYS_INLINE` too. Measured by Claude (agent, Opus 5.5, implementer 1 of plan 0040) on 2026-11-11 from 05:38:00 to 05:47:09 UTC, in the worktree of `feature/pre-release-core-pass--public-ops` at `a6f3196`, with the protocol of the first campaign (`../cost/README.md`):

```
tools/compiles cost docs/agent/records/performance/pre-release-core-pass/cost-public-ops/units.toml \
  docs/agent/records/performance/pre-release-core-pass/cost-public-ops/out --before 8058589 --cpu 4 --repeats 3
```

under the timing lock (`lock.sh take`, taken at once with no queue; released at the end), and nothing else of this workstream running.

- Before: `8058589`, the branch point, as in the first campaign. After: `a6f3196`, the first campaign's headers plus the eleven attributes, the two includes of `assert.hpp` and comments.
- Units: the first campaign's seven, unchanged (`units.toml` differs from `../cost/units.toml` only in its comment). Debug and measure trees of both compilers, three repeats, 168 compiles.
- Toolchain and machine as in the first campaign: g++ 16.2.1 20260810, clang++ 23.1.4, libstdc++ 16, host ds-001, every compile pinned to CPU 4; its sibling thread busy at most 0.01 s and other work on CPU 4 at most 0.06 s per compile. Pool 23 of 23 free and load average 1.35 before, 23 of 23 and 2.19 after (`conditions.txt`).
- Files: `out/rows.tsv`, `out/summary.txt`, `out/runs.tsv`, `out/units/`, as in the first campaign; the extracted before tree and the objects are not kept.
- Repeatability: every before-side mean agrees with the first campaign's within 0.5%, the same tree measured an hour apart.

## Before and after

Instructions are the mean of the three repeats, in millions, with the range of the repeats; wall time is the mean; peak memory the largest of the three. "Change at `7a27f76`" is the first campaign's change, before the public operations were forced.

| Kind | Compiler | Unit | Inst. before (10⁶) | Range | Inst. after (10⁶) | Range | Change | > 2 × larger range | > 5% | Change at `7a27f76` | Wall before (s) | Wall after (s) | Peak before (MiB) | Peak after (MiB) |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | --- | --- | ---: | ---: | ---: | ---: | ---: |
| debug | GCC | `includer/core` | 995.1 | 0.1 | 994.3 | 0.2 | -0.08% | no | no | -0.04% | 0.142 | 0.130 | 81 | 81 |
| debug | GCC | `probe/e1` | 4,831.8 | 1.5 | 5,236.8 | 2.8 | +8.38% | yes | yes | +7.55% | 0.409 | 0.432 | 155 | 173 |
| debug | GCC | `core/transform` | 136,108.7 | 64.8 | 184,785.8 | 181.1 | +35.76% | yes | yes | +35.12% | 9.187 | 13.223 | 972 | 1432 |
| debug | GCC | `core/product` | 32,472.9 | 56.2 | 58,621.7 | 37.9 | +80.53% | yes | yes | +3.50% | 2.177 | 4.322 | 418 | 830 |
| debug | GCC | `core/identities` | 2,665.6 | 0.6 | 2,664.3 | 1.0 | -0.05% | no | no | -0.02% | 0.299 | 0.298 | 166 | 166 |
| debug | GCC | `basis_change/conformal` | 55,081.6 | 16.6 | 56,631.6 | 8.5 | +2.81% | yes | no | +1.53% | 3.850 | 3.940 | 434 | 542 |
| debug | GCC | `rga3/chains` | 4,390.5 | 2.0 | 5,155.0 | 0.6 | +17.41% | yes | yes | +17.40% | 0.376 | 0.426 | 173 | 194 |
| debug | Clang | `includer/core` | 999.8 | 4.9 | 998.5 | 1.5 | -0.13% | no | no | +0.04% | 0.151 | 0.153 | 114 | 114 |
| debug | Clang | `probe/e1` | 4,429.1 | 2.0 | 4,515.6 | 5.1 | +1.95% | yes | no | +1.68% | 0.401 | 0.399 | 151 | 152 |
| debug | Clang | `core/transform` | 124,197.6 | 423.8 | 124,633.5 | 333.8 | +0.35% | no | no | +0.37% | 8.030 | 7.972 | 545 | 546 |
| debug | Clang | `core/product` | 30,292.1 | 69.6 | 35,581.1 | 12.9 | +17.46% | yes | yes | +1.54% | 2.000 | 2.478 | 263 | 317 |
| debug | Clang | `core/identities` | 2,499.9 | 1.8 | 2,498.9 | 7.4 | -0.04% | no | no | -0.14% | 0.331 | 0.331 | 169 | 170 |
| debug | Clang | `basis_change/conformal` | 55,513.6 | 161.3 | 55,958.5 | 55.9 | +0.80% | yes | no | +2.45% | 3.672 | 3.658 | 312 | 307 |
| debug | Clang | `rga3/chains` | 4,323.0 | 1.9 | 4,539.4 | 8.8 | +5.01% | yes | yes | +5.53% | 0.375 | 0.391 | 169 | 169 |
| measure | GCC | `includer/core` | 940.1 | 0.2 | 939.4 | 0.4 | -0.08% | no | no | -0.02% | 0.124 | 0.137 | 74 | 74 |
| measure | GCC | `probe/e1` | 3,397.9 | 1.0 | 3,459.8 | 1.9 | +1.82% | yes | no | +1.59% | 0.311 | 0.313 | 138 | 138 |
| measure | GCC | `core/transform` | 184,347.9 | 53.9 | 248,984.7 | 29.1 | +35.06% | yes | yes | +34.01% | 14.205 | 17.676 | 796 | 782 |
| measure | GCC | `core/product` | 31,975.1 | 23.6 | 73,372.7 | 23.0 | +129.47% | yes | yes | +2.60% | 2.178 | 4.145 | 345 | 411 |
| measure | GCC | `core/identities` | 2,706.8 | 0.6 | 2,704.4 | 0.5 | -0.09% | no | no | -0.06% | 0.296 | 0.295 | 155 | 155 |
| measure | GCC | `basis_change/conformal` | 83,796.0 | 8.2 | 98,837.9 | 12.5 | +17.95% | yes | yes | +11.80% | 5.838 | 6.496 | 418 | 435 |
| measure | GCC | `rga3/chains` | 12,916.5 | 1.6 | 8,635.6 | 1.5 | -33.14% | no | no | -14.08% | 1.020 | 0.681 | 199 | 186 |
| measure | Clang | `includer/core` | 993.9 | 0.2 | 993.5 | 1.7 | -0.04% | no | no | -0.19% | 0.154 | 0.166 | 114 | 114 |
| measure | Clang | `probe/e1` | 4,320.7 | 21.9 | 4,327.9 | 6.6 | +0.17% | no | no | -0.05% | 0.387 | 0.385 | 155 | 154 |
| measure | Clang | `core/transform` | 209,845.1 | 153.6 | 254,547.8 | 471.2 | +21.30% | yes | yes | +20.70% | 15.929 | 19.762 | 526 | 527 |
| measure | Clang | `core/product` | 34,489.6 | 50.5 | 47,508.4 | 90.6 | +37.75% | yes | yes | -2.42% | 2.368 | 3.463 | 254 | 254 |
| measure | Clang | `core/identities` | 2,524.2 | 5.7 | 2,524.3 | 9.6 | +0.00% | no | no | -0.21% | 0.337 | 0.340 | 173 | 173 |
| measure | Clang | `basis_change/conformal` | 86,455.5 | 238.5 | 98,054.7 | 32.7 | +13.42% | yes | yes | +2.71% | 6.306 | 7.226 | 307 | 307 |
| measure | Clang | `rga3/chains` | 5,102.8 | 5.2 | 7,276.8 | 4.2 | +42.60% | yes | yes | +37.92% | 0.452 | 0.580 | 172 | 172 |

## The knob criterion

As fixed in plan 0040 section 9: the debug-build cost is notable when, for any debug unit, the mean of retired instructions after exceeds the mean before by more than twice the larger of the two ranges of the repeats, and by more than 5%.

**It is met**, now by six of the seven debug units on one compiler or both:

| Compiler | Unit | Change | Twice the larger range (10⁶) | Increase (10⁶) | At `7a27f76` |
| --- | --- | ---: | ---: | ---: | ---: |
| GCC | `core/product` | +80.53% | 112.4 | 26,148.8 | +3.50% |
| GCC | `core/transform` | +35.76% | 362.3 | 48,677.1 | +35.12% |
| GCC | `rga3/chains` | +17.41% | 3.9 | 764.5 | +17.40% |
| GCC | `probe/e1` | +8.38% | 5.5 | 405.0 | +7.55% |
| Clang | `core/product` | +17.46% | 139.3 | 5,289.0 | +1.54% |
| Clang | `rga3/chains` | +5.01% | 17.5 | 216.4 | +5.53% |

No knob is added; the numbers go to the owner as O2. Forcing the public products adds most to `product_test.cpp`, which calls them many times directly: on GCC's debug tree its compile takes 2.18 s before and 4.32 s after, and its peak memory goes from 418 MiB to 830 MiB; on Clang's, 2.00 s to 2.48 s and 263 MiB to 317 MiB. GCC's `transform_test.cpp` stays at about +36% (9.19 s to 13.22 s, 972 MiB to 1,432 MiB), as in the first campaign.

## What else the table shows

- `<galoot/core.hpp>` alone is unchanged within 0.13% on both compilers and both kinds.
- In the measure trees, `product_test.cpp` grows 129.5% on GCC and 37.8% on Clang, `basis_change_conformal_test.cpp` 18.0% and 13.4%, `transform_test.cpp` 35.1% and 21.3%, and `chains_test.cpp` 42.6% on Clang while it shrinks 33.1% on GCC. These are tracked (C14), not judged; the criterion reads only the debug units.
- Inferred, not traced with the compilers' time reports: the growth is code generation and optimization of the bodies now copied into each call site, since parsing is unchanged.
