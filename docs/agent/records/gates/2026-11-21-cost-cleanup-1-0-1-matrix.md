# Merge matrix of `feature/cost-cleanup-1-0-1`, at `05229a7`

- Unit: the merge gate of plan 0043 (sections 8.1, 8.3 and 9, steps 6 and 7). Run by Claude (agent, Opus 5.5), in the performance-engineer role, on 2026-11-21 from 21:38:14 to 22:04:26 UTC, in the worktree `galoot-wt/cost-cleanup`, branch `feature/cost-cleanup-1-0-1`.
- Revision: `05229a7b086c26f45b2d407391c96fb0a2b305cd`, the merge of `main` at `dae49f3` into the feature branch (tip `1db53c8`, U1 to U5 merged). `git diff --stat 01dce7f dae49f3 -- . ':!docs/agent'` printed nothing: `main` had moved by records only since the feature branch left it. Every row of this run names `05229a7b086c`, none dirty.
- Toolchain: g++ 16.2.1 20260810, clang++ 23.1.5, CMake 4.5.1, Ninja 1.13.2. The release matrix of 1.0.0 ran with clang++ 23.1.4 and CMake 4.4.5 (`2026-11-12-release-matrix.md`); the units' own before rows (U1, U2) ran with 23.1.5.
- Outcome: **every check passed.** All eleven presets configured, built and passed their full suites; `format-check`, the convention trace gate and `tools/runlog check` exited 0. The memory watchdog never fired. No check was skipped.

## Departures and readings (C20)

1. **The watchdog is `memguard.sh`, with its floor set to the release matrix's.** The brief names `pe-common/memguard.sh`; the plan names "the memory watchdog of the release matrix", which stopped a step below 15 GiB available or above 20 GiB in one process. `memguard.sh` watches the summed resident size of the step's process tree, not the largest process, so I set `MEMGUARD_MIN_GB=15` (the release floor) and left `MEMGUARD_MAX_GB` at its default of 60 for the tree. The largest single process is read from each tree's `compile.tsv` and the build rows' `maxrss_kib` instead.
2. **The timing lock was held for the whole matrix** (taken 21:38:14, released 22:04:26 UTC), since the matrix is also the after-measurement of U1 and U2. The release matrix held it only for its benchmarks.
3. **Each preset ran as three steps** (`cmake --preset`, `cmake --build --preset`, `ctest --preset`), each through `tools/timed`, as the release matrix did, so the wall time per preset is the sum of three rows.
4. **The trees' compile rows are committed under `records/performance/cost-cleanup-1-0-1/matrix/`, not appended to `records/performance/compile.tsv`.** Plan 0043 asks for the trees' `compile.tsv` as the measure; appending the debug trees' rows to the shared record is plan 0045's step (decision 0252 item 19) and was not done here. Only the rows of each build are kept (140 to 148 per tree); the rows the compile-fail tests add during the suite are left out.
5. **Wall times are in shared mode under other load** (see "Conditions") and are compared with the release matrix's only as such (P14). Instructions and peak memory are the comparable columns.

## The matrix

All rows are `gate:all@05229a7`, `runs.tsv` rows 22388 to 22420; each tree started empty (the worktree had no `build/` directory).

| Preset | Tests | Wall (s) | Build (s) | Suite (s) | Largest unit by peak (KiB) | Watchdog: tree peak, least available |
| --- | ---: | ---: | ---: | ---: | --- | --- |
| gcc-debug | 657 of 657 | 141.2 | 76.4 | 64.1 | `identities_five_diagonal_test.cpp` 3,145,516 | 22.2 GiB, 83.2 GiB |
| clang-debug | 657 of 657 | 89.7 | 25.6 | 63.4 | `identities_five_diagonal_test.cpp` 1,073,636 | 9.3 GiB, 98.7 GiB |
| gcc-release | 657 of 657 | 116.4 | 51.6 | 64.3 | `dual_identities_test.cpp` 879,692 | 12.9 GiB, 93.9 GiB |
| gcc-asan | 649 of 649 | 195.8 | 131.0 | 64.3 | `dual_identities_test.cpp` 2,486,984 | 20.9 GiB, 85.7 GiB |
| clang-release | 657 of 657 | 100.8 | 35.1 | 65.1 | `transform_test.cpp` 544,924 | 7.8 GiB, 100.9 GiB |
| clang-asan | 657 of 657 | 156.8 | 92.8 | 63.4 | `dual_identities_test.cpp` 1,565,468 | 14.5 GiB, 93.8 GiB |
| lint | 657 of 657 | 230.1 | 165.0 | 64.3 | (no compile rows; clang-tidy) 1,054,252 in the build row | 9.9 GiB, 98.5 GiB |
| gcc-measure | 649 of 649 | 114.3 | 49.5 | 64.2 | `dual_identities_test.cpp` 801,980 | 12.3 GiB, 94.8 GiB |
| gcc-measure-o3 | 649 of 649 | 132.0 | 67.3 | 64.1 | `identities_rounding_antiproduct_test.cpp` 905,424 | 13.5 GiB, 93.6 GiB |
| clang-measure | 649 of 649 | 120.2 | 55.0 | 64.3 | `identities_rounding_antiproduct_test.cpp` 644,544 | 10.2 GiB, 98.3 GiB |
| clang-measure-o3 | 649 of 649 | 127.6 | 63.5 | 63.5 | `identities_rounding_antiproduct_test.cpp` 636,080 | 10.3 GiB, 98.2 GiB |

- The test counts equal the release matrix's: each preset's full suite under its test preset's filter, which leaves out the labels `runlog` and `bench`.
- The lint build ran clang-tidy on every source, `all_verify_interface_header_sets` and `format-check`, with no diagnostic in its log.
- Every suite still takes about 64 s because `tools.jobpool` (63 s) bounds it; the next longest are `core.identities` (23.5 s on gcc-debug) and `tools.mutate` (21.4 s).
- Total wall time of the 33 rows: 1,524.9 s (25.4 min), against 2,883.6 s (48.1 min) for the release matrix.
- The watchdog's figures are the summed resident size of the step's process tree (up to 23 compiles at once) and the least `MemAvailable` it sampled, every 2 s, out of 125 GiB. Its logs are `matrix/memguard-<preset>-<step>.tsv`.

## The other checks

| Check | Selection, row | Wall (s) | Result |
| --- | --- | ---: | --- |
| `format-check` on the lint tree | `gate:format-check@05229a7`, 22421 | 1.7 | exit 0 |
| Convention trace gate (W5) | `gate:trace@05229a7`, 22422 | 0.3 | exit 0; "items with no test: 6.1, 6.2, 6.3, 6.4", as at the release |
| `tools/runlog check` | `gate:runlog@05229a7`, 22423 | 2.7 | exit 0; "1 eras match the history; 1727 kept rows present once; no removed row present" |

## Conditions

Shared mode, unpinned, processors 0 to 31 eligible; the job pool was exported for every build and test step. The pool read "running: 23 of 23 slots free" before and after every step; it was running at the start, so it was not started, and it was never stopped. Other load throughout, from `top` at the start and end of each preset (`matrix/conditions-*.txt`): an unrelated session's Python job at 1.4 to 2.7 processors and the editor's C++ language service at about 1 processor; no other build drew from the pool at those moments. The 1-minute load average was 2.9 at the start and 7 to 9 between presets, which includes this matrix's own steps.

## Comparison with the before figures (plan 0043 section 8.3)

Before: the release matrix (`gate:all@7580e88`, `runs.tsv` rows 21905 to 21937, Clang 23.1.4), the release revision's debug-tree rows in `records/performance/compile.tsv` (at `7276f826533f`), and the units' own before rows (`cost-cleanup-1-0-1/rounding/` and `transform/`, Clang 23.1.5). After: this matrix. Instructions are performance plus efficiency cores.

### Tree level, every preset

| Preset | Wall before, after (s) | Build before, after (s) | Build instructions before, after (10⁹) | Change | Largest process of the build, before, after (KiB) |
| --- | --- | --- | --- | ---: | --- |
| gcc-debug | 159.5, 141.2 | 95.8, 76.4 | 5,590, 5,694 | +1.8% | 6,214,900, 3,145,516 |
| clang-debug | 100.0, 89.7 | 35.5, 25.6 | 2,923, 3,032 | +3.7% | 1,585,484, 1,073,636 |
| gcc-release | 157.1, 116.4 | 92.7, 51.6 | 6,235, 6,391 | +2.5% | 2,052,320, 879,692 |
| gcc-asan | 228.8, 195.8 | 164.5, 131.0 | 10,618, 11,722 | +10.4% | 4,961,752, 2,486,984 |
| clang-release | 113.7, 100.8 | 49.3, 35.1 | 3,396, 3,552 | +4.6% | 1,084,412, 544,924 |
| clang-asan | 204.5, 156.8 | 139.7, 92.8 | 6,835, 7,255 | +6.1% | 2,814,684, 1,565,468 |
| lint | 264.9, 230.1 | 201.1, 165.0 | 29,562, 30,586 | +3.5% | 1,616,736, 1,054,252 |
| gcc-measure | 153.5, 114.3 | 89.2, 49.5 | 6,147, 6,299 | +2.5% | 1,937,720, 801,980 |
| gcc-measure-o3 | 167.7, 132.0 | 103.6, 67.3 | 7,231, 7,278 | +0.7% | 2,133,492, 905,424 |
| clang-measure | 667.4, 120.2 | 603.0, 55.0 | 9,416, 5,697 | -39.5% | 1,902,840, 644,544 |
| clang-measure-o3 | 666.6, 127.6 | 602.1, 63.5 | 10,102, 6,408 | -36.6% | 1,885,556, 636,080 |

The build rows' instructions count the whole build, the link and the tools included. The Clang presets changed toolchain between the two (23.1.4 to 23.1.5), so a part of their change may be the compiler's; it is not separated here.

### The items of section 8.3

| Item | Before | Expected after | Measured after | Reading |
| --- | --- | --- | --- | --- |
| clang-measure tree, per P9 merge | 667.4 s (build 603.0 s; unit 602.5 s, 4,275 G) | about 130 to 170 s | **120.2 s** (configure 0.9, build 55.0, suite 64.3) | better than expected; the build is now bounded by `identities_five_diagonal_test.cpp` (334.6 G, 53.4 s) and `identities_four_other_test.cpp` (257.7 G, 45.5 s), not by the rounding program |
| clang-measure-o3 tree | 666.6 s | the same | **127.6 s** (build 63.5) | as clang-measure; the plan put this one at plan 0045's matrix, and it is measured here as well |
| the rounding program's instructions on clang-measure | 4,272.5 G in one unit (U1's before, Clang 23.1.5; release probe 4,272.8 G) | about 1,330 G, about 31% | **529.9 G over six units, 12.4%**; largest unit `identities_rounding_antiproduct_test.cpp`, 105.1 G, 644,544 KiB | **U1's criterion is met** (below 50% and no unit above 500 G). The matrix's figures agree with U1's own after rows (530.1 G, largest 105.0 G) to 0.2% |
| gcc-debug largest unit | 5.93 GiB (`transform_identities_test.cpp`, 6,214,900 KiB) | about 1.7 GiB for the transform program's largest group; the tree's largest `identities_four_test.cpp` (4.83 GiB), or about 3.0 GiB with O2 (b) | transform's largest group **1,749,812 KiB (1.67 GiB)**, `transform_identities_five_dense_large_test.cpp`; the forms of four 2,379,676 and 2,857,024 KiB; the tree's largest **`identities_five_diagonal_test.cpp`, 3,145,516 KiB (3.00 GiB)** | **U2's criterion is met** (no group above 2.5 GiB), and O2 (b)'s expectation holds |
| gcc-debug and gcc-asan builds | the unit 92.3 s on gcc-debug; on gcc-asan 151.3 s, 4.73 GiB | the groups compile in parallel; total instructions about the same, plus about 3 G of headers per new source | gcc-debug build 95.8 to 76.4 s; the transform program's six units 1,002.2 G against 981.7 G (+2.1%). gcc-asan build 164.5 to 131.0 s; its largest process 4,961,752 to 2,486,984 KiB; the transform program's largest unit 1,472,852 KiB | the peaks fell as planned. **Unfavorable: gcc-asan's build instructions rose by 10.4%** (10,618 G to 11,722 G), more than the headers of the new sources explain (see below) |
| mutation runs | serial CTest per mutant | the selection's tests at once, bounded by the pool | not part of this matrix; U4's before and after rows (`db3de04`, `c645e69`) | as the tools review (F2) found: the run under a saturated pool shows no overlap of tests, so the saving is unmeasured. The mechanism was shown on a private jobserver by the reviewer |
| dirty mark | read at the end | read at both ends | U3's tests; every row of this matrix reads `05229a7b086c`, none dirty | the extra `git status` costs about 12 ms a row (the tools review's figure) |

### The rounding program on every preset (the tests review's finding M2)

The split raises the rounding program's total work on every preset that does not force-inline at `-O2` under Clang. The before figures exist for three presets only: the release revision's debug-tree rows and U1's clang-measure tree.

| Preset | Before (10⁹), one unit | After (10⁹), six units | Change | Largest unit after (10⁹; KiB) |
| --- | ---: | ---: | ---: | --- |
| gcc-debug | 258.8 | 333.5 | +28.9% | 66.9; 1,003,372 (before 2,809,924) |
| clang-debug | 107.9 | 193.7 | +79.5% | 41.4; 381,560 (before 796,004) |
| gcc-release | not measured | 461.7 | | 92.8; 762,864 |
| gcc-asan | not measured | 1,439.8 | | 380.6; 1,155,624 |
| clang-release | not measured | 256.6 | | 60.1; 318,620 |
| clang-asan | not measured | 768.0 | | 175.6; 747,196 |
| gcc-measure | not measured | 454.9 | | 92.0; 747,268 |
| gcc-measure-o3 | not measured | 584.6 | | 119.2; 905,424 |
| clang-measure | 4,272.5 | 529.9 | -87.6% | 105.1; 644,544 |
| clang-measure-o3 | not measured (release build row: 602 s, as clang-measure) | 531.3 | | 105.3; 636,080 |

On the eight presets without a before figure for the program, the tree-level rise of the table above bounds what the two splits together added: from +0.7% (gcc-measure-o3, 47 G) to +10.4% (gcc-asan, 1,104 G); the bound also holds part of the Clang trees' toolchain change. Across the matrix the net is a saving of about 4,140 G (the two clang-measure trees save 7,414 G; the other nine add 3,273 G, lint's 1,024 G and gcc-asan's 1,104 G the largest), and every tree's largest process is smaller.

The cause of the rises is inferred, not traced: each new source parses the headers and instantiates the shared product machinery again, and each law source instantiates its law for both algebras (U1's record). gcc-asan's +1,104 G is larger than that reading predicts from gcc-debug's +104 G; the instrumentation may make the repeated instantiations dearer, but no time trace was taken.

## Open

- **The rounding program's before figures on seven presets.** Building `core_identities_rounding_test` at `01dce7f` on gcc-release, gcc-asan, clang-release, clang-asan, gcc-measure, gcc-measure-o3 and clang-measure-o3 would close the gap of the table above: about 20 minutes of machine time, mostly the two clang-measure-o3 and gcc-asan compiles of the one large unit, under the lock. Option (b) is to state the tree-level bound, as above, in the plan's outcome. I recommend (b): the tree rows already bound the rise, and the split is the owner's ruling (decision 0252 item 1) whatever the figure.
- **gcc-asan's 10.4% rise.** A `-ftime-report` of one rounding law source and of the old unit under gcc-asan would say where the extra work goes, at about 10 minutes. I recommend recording it in the plan's outcome as found and leaving the trace to a later cost pass, since gcc-asan's wall time and peak both fell.
- **The mutation saving (tools review F2)** stays unmeasured; a quiet-machine repetition under the lock would settle it (about 10 minutes for the gcc-debug spec). Not needed for the merge.

## Commands

From the worktree root, with `eval "$(tools/jobpool env)"`, `MEMGUARD_MIN_GB=15`, the timing lock held, for each preset `P` in the order of the table (`matrix/scripts/matrix.sh`, as it ran):

```bash
bash memguard.sh <log> -- tools/timed configure P gate:all@05229a7 -- cmake --preset P
bash memguard.sh <log> -- tools/timed build P gate:all@05229a7 -- cmake --build --preset P
bash memguard.sh <log> -- tools/timed test P gate:all@05229a7 -- ctest --preset P
```

then `tools/timed build lint gate:format-check@05229a7 -- cmake --build --preset lint --target format-check`, the trace gate of W5 under `gate:trace@05229a7`, and `tools/timed check - gate:runlog@05229a7 -- tools/runlog check`. Each tree's `compile.tsv` was copied after its build to `matrix/compile-<preset>.tsv`; `matrix/scripts/units.py` gives the per-program figures above from those files.
