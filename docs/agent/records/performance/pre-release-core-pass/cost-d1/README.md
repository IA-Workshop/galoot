# Plan 0040: build cost before the pre-release core pass and with project and antiproject forced too (W10, third campaign)

Plan 0040 section 9, run a third time because the delta review's D1 fix changed the chain again: `project` and `antiproject` now carry `GALOOT_ALWAYS_INLINE`, and `run_sum`'s pack `I` is renamed `Is` (no effect on cost). Measured by Claude (agent, Opus 5.5, implementer 1 of plan 0040) on 2026-11-11 from 06:44:13 to 06:54:52 UTC, in the worktree of `feature/pre-release-core-pass--public-ops` at `d2568e3`, with the protocol of the first two campaigns (`../cost/README.md`), under the timing lock (taken at once, released at the end):

```
tools/compiles cost docs/agent/records/performance/pre-release-core-pass/cost-d1/units.toml \
  docs/agent/records/performance/pre-release-core-pass/cost-d1/out --before 8058589 --cpu 4 --repeats 3
```

- Before: `8058589`, the branch point. After: `d2568e3`. Units: the same seven in the debug and measure trees of both compilers (`units.toml` differs from the others only in its comment), three repeats, 168 compiles, each pinned to CPU 4.
- **Conditions: the machine was loaded.** Another workstream's test run used the job pool (`conditions.txt`: one `ctest` client before, the pool 0 of 23 free after) and the load average was 5.9 before and 9.6 after. In 28 of the 168 compiles CPU 4's sibling thread was busy for more than 0.5 s, up to 12.9 s; other work ran on CPU 4 itself for at most 0.13 s per compile. So **the wall times of this campaign are not comparable** with the other two (P14), and are listed only for completeness. Retired user-space instructions do not depend on load: every before-side mean agrees with the second campaign's within 0.25%.
- Files: `out/rows.tsv`, `out/summary.txt`, `out/runs.tsv`, `out/units/`; the extracted before tree and the objects are not kept.

## Before and after

Instructions are the mean of the three repeats, in millions, with the range of the repeats. "Change at `a6f3196`" is the second campaign's change.

| Kind | Compiler | Unit | Inst. before (10⁶) | Range | Inst. after (10⁶) | Range | Change | > 2 × larger range | > 5% | Change at `a6f3196` | Wall before (s) | Wall after (s) | Peak before (MiB) | Peak after (MiB) |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | --- | --- | ---: | ---: | ---: | ---: | ---: |
| debug | GCC | `includer/core` | 995.0 | 0.1 | 994.4 | 0.4 | -0.06% | no | no | -0.08% | 0.101 | 0.101 | 81 | 81 |
| debug | GCC | `probe/e1` | 4,830.4 | 2.0 | 5,237.9 | 2.7 | +8.43% | yes | yes | +8.38% | 0.388 | 0.407 | 155 | 172 |
| debug | GCC | `core/transform` | 136,096.1 | 75.2 | 184,741.1 | 174.3 | +35.74% | yes | yes | +35.76% | 9.181 | 14.755 | 972 | 1432 |
| debug | GCC | `core/product` | 32,473.8 | 23.3 | 58,587.1 | 28.9 | +80.41% | yes | yes | +80.53% | 2.933 | 5.948 | 418 | 830 |
| debug | GCC | `core/identities` | 2,664.6 | 0.8 | 2,665.1 | 0.7 | +0.02% | no | no | -0.05% | 0.372 | 0.317 | 166 | 166 |
| debug | GCC | `basis_change/conformal` | 55,096.8 | 69.4 | 56,647.9 | 42.4 | +2.82% | yes | no | +2.81% | 5.203 | 5.908 | 434 | 546 |
| debug | GCC | `rga3/chains` | 4,390.0 | 1.2 | 5,155.9 | 4.7 | +17.45% | yes | yes | +17.41% | 0.595 | 0.681 | 173 | 194 |
| debug | Clang | `includer/core` | 1,000.3 | 1.1 | 998.5 | 3.6 | -0.17% | no | no | -0.13% | 0.126 | 0.127 | 114 | 114 |
| debug | Clang | `probe/e1` | 4,418.2 | 2.2 | 4,516.2 | 8.0 | +2.22% | yes | no | +1.95% | 0.394 | 0.398 | 151 | 152 |
| debug | Clang | `core/transform` | 124,265.0 | 249.9 | 124,679.2 | 37.8 | +0.33% | no | no | +0.35% | 8.498 | 10.064 | 545 | 547 |
| debug | Clang | `core/product` | 30,283.4 | 52.4 | 35,652.0 | 77.2 | +17.73% | yes | yes | +17.46% | 2.884 | 3.348 | 262 | 316 |
| debug | Clang | `core/identities` | 2,499.2 | 1.8 | 2,499.9 | 6.7 | +0.03% | no | no | -0.04% | 0.331 | 0.332 | 169 | 169 |
| debug | Clang | `basis_change/conformal` | 55,555.4 | 233.1 | 55,913.7 | 139.2 | +0.64% | no | no | +0.80% | 3.960 | 3.932 | 312 | 307 |
| debug | Clang | `rga3/chains` | 4,320.6 | 3.6 | 4,545.2 | 15.4 | +5.20% | yes | yes | +5.01% | 0.375 | 0.395 | 169 | 169 |
| measure | GCC | `includer/core` | 940.1 | 0.2 | 939.4 | 0.6 | -0.07% | no | no | -0.08% | 0.159 | 0.157 | 74 | 74 |
| measure | GCC | `probe/e1` | 3,397.6 | 2.7 | 3,459.9 | 0.6 | +1.83% | yes | no | +1.82% | 0.488 | 0.497 | 138 | 138 |
| measure | GCC | `core/transform` | 184,387.0 | 187.6 | 249,041.4 | 49.0 | +35.06% | yes | yes | +35.06% | 21.196 | 21.166 | 796 | 782 |
| measure | GCC | `core/product` | 31,986.7 | 7.1 | 73,364.6 | 77.6 | +129.36% | yes | yes | +129.47% | 2.564 | 4.878 | 345 | 411 |
| measure | GCC | `core/identities` | 2,706.0 | 0.7 | 2,704.4 | 0.3 | -0.06% | no | no | -0.09% | 0.288 | 0.335 | 155 | 155 |
| measure | GCC | `basis_change/conformal` | 83,777.6 | 7.2 | 99,056.7 | 37.6 | +18.24% | yes | yes | +17.95% | 6.792 | 7.353 | 418 | 438 |
| measure | GCC | `rga3/chains` | 12,918.8 | 4.6 | 8,635.7 | 3.0 | -33.15% | no | no | -33.14% | 1.061 | 0.689 | 199 | 186 |
| measure | Clang | `includer/core` | 994.4 | 3.4 | 994.1 | 2.4 | -0.03% | no | no | -0.04% | 0.127 | 0.127 | 114 | 114 |
| measure | Clang | `probe/e1` | 4,317.1 | 4.0 | 4,337.3 | 28.8 | +0.47% | no | no | +0.17% | 0.384 | 0.383 | 155 | 154 |
| measure | Clang | `core/transform` | 209,787.3 | 254.9 | 254,648.4 | 478.0 | +21.38% | yes | yes | +21.30% | 16.943 | 22.152 | 527 | 527 |
| measure | Clang | `core/product` | 34,448.1 | 34.8 | 47,506.2 | 66.8 | +37.91% | yes | yes | +37.75% | 2.395 | 3.479 | 254 | 254 |
| measure | Clang | `core/identities` | 2,523.9 | 3.2 | 2,527.0 | 10.3 | +0.12% | no | no | +0.00% | 0.308 | 0.306 | 173 | 173 |
| measure | Clang | `basis_change/conformal` | 86,462.4 | 98.1 | 98,850.9 | 94.9 | +14.33% | yes | yes | +13.42% | 6.335 | 7.400 | 306 | 306 |
| measure | Clang | `rga3/chains` | 5,107.8 | 3.4 | 7,276.4 | 1.8 | +42.46% | yes | yes | +42.60% | 0.429 | 0.602 | 172 | 172 |

## The knob criterion

As fixed in plan 0040 section 9: the debug-build cost is notable when, for any debug unit, the mean of retired instructions after exceeds the mean before by more than twice the larger of the two ranges of the repeats, and by more than 5%.

**It is met**, by the same six debug units as in the second campaign, each within 0.3 points of its change there:

| Compiler | Unit | Change | Twice the larger range (10⁶) | Increase (10⁶) | Peak memory (MiB), before to after |
| --- | --- | ---: | ---: | ---: | --- |
| GCC | `core/product` | +80.41% | 57.9 | 26,113.3 | 418 to 830 |
| GCC | `core/transform` | +35.74% | 348.6 | 48,644.9 | 972 to 1,432 |
| GCC | `rga3/chains` | +17.45% | 9.5 | 765.9 | 173 to 194 |
| GCC | `probe/e1` | +8.43% | 5.4 | 407.4 | 155 to 172 |
| Clang | `core/product` | +17.73% | 154.4 | 5,368.6 | 262 to 316 |
| Clang | `rga3/chains` | +5.20% | 30.7 | 224.6 | 169 to 169 |

No knob is added; the numbers go to the owner as O2. Forcing `project` and `antiproject` changes the cost of these units little, since none of them projects multivectors of all blades; the second campaign's wall times, measured on a quiet machine (GCC `product_test.cpp` 2.18 s to 4.32 s, `transform_test.cpp` 9.19 s to 13.22 s), are the ones to quote.
