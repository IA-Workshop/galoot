# WU17 E3: the growth grid through the public operations

Plan 0038 section 4 item 7 (DoD-23; P2 as decision 0005 reads it). Run by Claude (agent, Opus 5.5, performance-engineer) on 2026-11-10 in the worktree `.claude/worktrees/wu17-e23`, branch `records/cost-evidence-measurements`, at `18849b5` (the tip of `feature/cost-evidence`). Recorded, never asserted (decision 0013).

## Revision

Measured at `18849b5`. The branch has since merged `feature/cost-evidence` at `2de0d36` (the inline reference kernels). Between the two revisions, nothing under `include/`, `tools/`, or `cmake/` changed, and neither did `CMakeLists.txt` or `CMakePresets.json` (`git diff --stat 18849b5 2de0d36` on those paths is empty). Under `test/`, only `test/bench/` and `test/reference/reference_kernels.py` changed. No unit here includes either, and the compile-fail test whose flags the units take (`core.assert_consteval`) is unchanged. So the grid stands at `2de0d36` and was not rerun.

## Caveats, read first

1. **The figures join decisions 0113 and 0116 in method, not in value.** Those grids compiled plan-level units (`core::detail::product_plan<...>.terms`) with Galoot's flags at `-O0` and no `-g`. These units call the public operations, so each use includes its kernels' instantiation and code, and they take the debug trees' flags through `tools/compiles`, which add `-g`. The same method (`-O0`, three repeats interleaved, each use minus a baseline of the same algebra) makes the growth with n comparable; the absolute figures are not.
2. **The antiwedge's fixed use is empty for n ≥ 6.** The antiwedge of a line (grade 2) and a plane (grade 3) has grade 5 − n, so in R(n − 1, 0, 1) with n = 8 or 16 it has no terms; its cost falls with n because the use does, not because of anything the generator saves. Any antiwedge of rigid(3) objects embedded in a larger algebra degenerates this way.
3. **The busy threshold and the repeat.** Plan 0038 section 6 repeats a run whose sibling or own CPU was busy for more than 1% of its wall time. For compiles of 0.15 to 0.52 s a single 10 ms tick of `/proc/stat` is 2% to 7%, and 60 of the 180 compiles crossed it (the largest busy time of any compile was 0.03 s). `tools/compiles` cannot repeat one compile, so the whole campaign ran a second time (`out-repeat/`) and both are kept; 24 compiles were flagged in both. The two campaigns' means agree within 0.8% for every unit, and every use within 0.4 M or its range. The figures below are the first campaign's.
4. **Choices the plan left open.** The dual is `bulk_dual` and the involution `reverse`. The unit `antiproduct4`, the four typed pairs decision 0116 measured through the geometric antiproduct, is added to the plan's list so that the antiproduct's figures for decision 0141 have the pairs that decision used. The pool, which plan 0038 section 6 lists as one slot for E3, had stopped itself before this run; the compiles ran one at a time on CPU 4 under the lock and took no slot.

## What ran

- Toolchain and machine: g++ 16.2.1 20260810, clang++ 23.1.4, libstdc++ 16; Intel Core i9-14900K, CPU 4 (a performance core), its sibling CPU 5 left idle by this work.
- Flags: those of the `gcc-debug` and `clang-debug` trees, configured fresh in this worktree, taken from the compile-fail test `core.assert_consteval`: `-DGALOOT_CHECKING=1 -I<include> -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -D_GLIBCXX_ASSERTIONS -O0 -g -std=c++23 -c`.
- Units: `generate.py` writes `units.toml`; `tools/compiles` writes each unit's source to `out/units/`. Each unit declares `core::rigid<n − 1>()`, R(n − 1, 0, 1), and the rigid(3) layouts of a point, a line, a plane, and a motor with e4 read as e_n (as `blade_set` literals, since `blades<Alg, "...">` takes one digit per generator), reads eight operands (three points, a line, two planes, two motors) from memory at run time, and returns the sum of every coefficient of the operands and of every result. The baseline has the same operands and no operation. Uses: `wedge` (p ∧ q and (p ∧ q) ∧ r), `antiwedge` (l ∨ g), `product` (the geometric products of point and point, line and plane, motor and point, point and motor, decision 0113's pairs), `antiproduct` (`compose(Q2, Q1)`), `antiproduct4` (the geometric antiproducts of point and point, plane and plane, motor and point, motor and motor, decision 0116's pairs), `transform` (of a point, a line, and a plane by a motor), `dual` (`bulk_dual` of a point, a line, a plane), `involution` (`reverse` of a point, a line, a plane, a motor). Distinct operations: `distinct1`, `distinct4`, `distinct16`, the first 1, 4, and 16 entries of `generate.py`'s `DISTINCT`, at n = 4.
- Measurement: `tools/compiles cost` with trees `["after"]`, `--before 18849b5` (the same revision, so the extraction is unused), `--cpu 4`, three repeats interleaved repeat by repeat, every compile as `taskset -c 4 tools/timed --log out/runs.tsv measure ...`, so its row is pinned with the instructions retired on CPU 4.

Commands, from the worktree's root:

```bash
docs/agent/records/performance/wu17-growth/batch.sh 18849b5              # first campaign, out/
docs/agent/records/performance/wu17-growth/batch.sh 18849b5 out-repeat   # the repeat
```

`batch.sh` writes the units, reads the pool and the load, takes and releases the lock, runs `tools/compiles cost`, reads both again, and runs `analyze.py`, which writes `growth.tsv` (each use: its unit's mean minus the baseline's mean at the same n).

## Lock, pool, and load

| Campaign | Lock held (UTC) | Pool before / after | Load average (1 min) before / after |
| --- | --- | --- | --- |
| `out/` | 17:48:15 to 17:49:01 | not running / not running | 0.97 / 1.52 |
| `out-repeat/` | 17:51:03 to 17:51:50 | not running / not running | 1.32 / 1.68 |

A trial of the same units unpinned (`--cpu shared`, one repeat, 17:26, outside the lock, between two E2 batches, which did not hold CPU 4 then) checked that every unit builds; it is not a measurement, and its one row is in `runs.tsv` here with the configure rows of the two debug trees.

## Fixed use against n

Retired instructions of each use (its unit minus the baseline of the same n), mean of three, with the change from n = 4; the larger of the use's and the baseline's ranges over the three sizes; the unit's mean wall time at n = 4 and n = 16 and its peak memory at n = 16.

### GCC 16

| Use | n = 4 | n = 8 | n = 16 | Larger range | Wall n = 4 → 16, memory |
| --- | --- | --- | --- | --- | --- |
| wedge | 309.1 M | 308.7 M (−0.1%) | 309.6 M (+0.2%) | 0.5 M | 0.175 → 0.180 s, 97 MiB |
| antiwedge | 174.6 M | 85.3 M (−51.1%) | 85.9 M (−50.8%) | 0.5 M | 0.162 → 0.161 s, 92 MiB |
| product | 865.5 M | 866.1 M (+0.1%) | 867.8 M (+0.3%) | 1.4 M | 0.211 → 0.219 s, 106 MiB |
| antiproduct (compose) | 456.3 M | 556.6 M (+22.0%) | 962.1 M (+110.8%) | 4.4 M | 0.181 → 0.207 s, 102 MiB |
| antiproduct4 | 975.6 M | 1172.1 M (+20.1%) | 1949.7 M (+99.9%) | 4.2 M | 0.223 → 0.277 s, 113 MiB |
| transform | 2413.3 M | 2645.1 M (+9.6%) | 3409.4 M (+41.3%) | 3.4 M | 0.312 → 0.367 s, 128 MiB |
| dual | 451.0 M | 453.8 M (+0.6%) | 459.2 M (+1.8%) | 0.5 M | 0.195 → 0.200 s, 101 MiB |
| involution | 274.9 M | 275.6 M (+0.2%) | 275.4 M (+0.2%) | 0.7 M | 0.176 → 0.178 s, 97 MiB |
| baseline (whole unit) | 1164.6 M | 1167.8 M | 1178.8 M | 0.5 M | 0.150 → 0.155 s, 89 MiB |

### Clang 23

| Use | n = 4 | n = 8 | n = 16 | Larger range | Wall n = 4 → 16, memory |
| --- | --- | --- | --- | --- | --- |
| wedge | 352.5 M | 350.4 M (−0.6%) | 351.8 M (−0.2%) | 3.4 M | 0.196 → 0.192 s, 133 MiB |
| antiwedge | 291.7 M | 253.3 M (−13.2%) | 253.1 M (−13.2%) | 3.0 M | 0.188 → 0.187 s, 133 MiB |
| product | 812.3 M | 807.9 M (−0.5%) | 809.5 M (−0.3%) | 5.9 M | 0.218 → 0.219 s, 135 MiB |
| antiproduct (compose) | 608.2 M | 1026.0 M (+68.7%) | 2676.6 M (+340.1%) | 60.4 M | 0.207 → 0.308 s, 133 MiB |
| antiproduct4 | 1077.4 M | 1900.3 M (+76.4%) | 5110.1 M (+374.3%) | 24.9 M | 0.238 → 0.440 s, 136 MiB |
| transform | 2270.8 M | 3166.7 M (+39.5%) | 6401.8 M (+181.9%) | 92.9 M | 0.309 → 0.516 s, 139 MiB |
| dual | 468.6 M | 479.8 M (+2.4%) | 503.0 M (+7.3%) | 3.0 M | 0.204 → 0.209 s, 136 MiB |
| involution | 288.8 M | 286.6 M (−0.8%) | 286.9 M (−0.6%) | 3.1 M | 0.192 → 0.185 s, 132 MiB |
| baseline (whole unit) | 1088.5 M | 1093.4 M | 1103.8 M | 3.0 M | 0.161 → 0.162 s, 124 MiB |

### Each growth and its cause (decision 0005)

- **Wedge, geometric product, reverse: no growth.** Every change from n = 4 to 16 is within twice the larger range (at most 0.6%).
- **Baseline: +14.2 M on GCC (+1.2%), +15.3 M on Clang (+1.4%) from n = 4 to 16.** A unit that declares the algebra and the four types and uses no operation. Inferred, as decisions 0107 and 0113 measured for the plan-level units: the one evaluation per algebra of its signature data and the `metric_like` check, which grow with the signature (O(n²)), so consistent with the algebra's structure.
- **Antiwedge: falls** (−51% on GCC, −13% on Clang). The use is empty at n ≥ 6 (caveat 2). No defect.
- **Geometric antiproduct (compose and the four pairs): +111% and +100% on GCC, +340% and +374% on Clang.** The cause decision 0116 found and its correction classified: the antiproduct is generated as the product of the complements, of grade n − k, through a recursion whose work grows with that grade, while each pair's result stays as small as at n = 4. Growth with the dimension at fixed use, contrary to C6, recorded as a defect found and accepted as decision 0141's exception; it blocks nothing.
- **Transform: +41% on GCC (+996 M), +182% on Clang (+4,131 M).** A transform is two antiproducts (V ⟇ x, then that with V), and only the antiproduct in it grows: the growth is about twice compose's on both compilers (996 against 506 M on GCC, 4,131 against 2,068 M on Clang). Same cause and the same exception. Inferred from the pattern, not from a time report.
- **Bulk dual: +8.2 M on GCC (+1.8%), +34.4 M on Clang (+7.3%).** Beyond the ranges, small. Inferred, not measured: `bulk_dual` is the geometric product of the reverse with the unit antiscalar, a blade of grade n, so its generation runs the same grade recursion over an operand of grade n. By decision 0116's reading this follows the route, not the use, so it is growth at fixed use contrary to C6 of the same kind as the antiproduct's, found here and not covered by decision 0141's exception, which names the antiproduct and the antiwedge. Reported; it blocks nothing.

### The antiproduct's figures for decision 0141 (item 4)

| | GCC n = 4 | GCC n = 16 | Clang n = 4 | Clang n = 16 |
| --- | --- | --- | --- | --- |
| `compose(Q2, Q1)` | 456.3 M | 962.1 M (+110.8%) | 608.2 M | 2,676.6 M (+340.1%) |
| four typed pairs (`antiproduct4`) | 975.6 M | 1,949.7 M (+99.9%) | 1,077.4 M | 5,110.1 M (+374.3%) |
| `transform` of a point, a line, a plane | 2,413.3 M | 3,409.4 M (+41.3%) | 2,270.8 M | 6,401.8 M (+181.9%) |
| decision 0116, the four pairs at the plan level (headers of `f83aa3c`, no `-g`) | 308.8 M | 1,137.2 M (+268%) | 702.5 M | 3,955.9 M (+463%) |

By the absolute growth from n = 4 to 16, the comparable figure (caveat 1), the four pairs grow by 974 M on GCC and 4,033 M on Clang here, against 828 M and 3,253 M at the plan level in decision 0116. The exception's growth is still present and of the same size; at the largest n of the four-pair use a unit costs 0.28 s on GCC and 0.44 s on Clang.

## Fixed n = 4 against distinct operations

| Distinct operations | GCC use | per operation | Clang use | per operation |
| --- | --- | --- | --- | --- |
| 1 | 175.2 M | 175.2 M | 275.3 M | 275.3 M |
| 4 | 1,489.1 M | 372.3 M | 1,533.3 M | 383.3 M |
| 16 | 4,425.7 M | 276.6 M | 3,823.0 M | 238.9 M |

Ranges at most 4.5 M on GCC and 11.5 M on Clang. The cost grows with the number of distinct operations, and its rate per operation follows the mix: the first four include a transform and compose, the two costliest operations here. The grid does not compile the sixteen operations one by one, so whether the cost is exactly the sum of their own costs is not tested.

## Files

- `generate.py`, `units.toml`: the units; `batch.sh`: the locked run; `analyze.py`: the uses.
- `out/` and `out-repeat/`: `units/` (the sources compiled), `rows.tsv` (every compile), `runs.tsv` (the campaign log, `TIMED_LOG`), `summary.txt` (`tools/compiles`'s means and ranges), `growth.tsv`, `protocol.log`. The extracted `before/` trees and the objects are not committed.
- `runs.tsv`: the campaign log of the two debug trees' configure runs and of the unpinned trial.

Verification: each figure is a mean of three pinned compiles with its range, repeated in a second campaign that agrees within 0.8%. Validation: none applies; cost is measured, not validated.
