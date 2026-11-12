# Release matrix of core 1.0.0, at the release commit `7580e88`

- Unit: U8 of plan 0042 (section 4, "Matrix"; section 5). Run by Claude (agent, Opus 5.5), in the performance-engineer role, on 2026-11-12 UTC (2026-11-11 local), in the worktree `galoot-wt/release-1.0.0`, branch `feature/release-1.0.0`.
- Revision: the release commit R is `7580e88`. The branch tip when the runs started was `7276f82`, which adds one run row to R, and every row of this run names `7276f826533f` as its revision. `git diff --stat 7580e88 7276f82 -- . ':!docs/agent'` prints nothing, so the code, tests, tools and build files the runs used are R's. The selections name R: `gate:all@7580e88` and the like.
- Toolchain: g++ 16.2.1 20260810, clang++ 23.1.4, libstdc++ 16, CMake 4.4.5, Ninja 1.13.2, the same as gate 3 at `2438604`.
- Outcome: every check passed. No preset failed, the memory watchdog never fired, and no check was skipped.

## Departures and readings (C20)

1. **The revision in the rows is `7276f82`, not R.** The brief names the worktree's branch at `7276f82` and R as `7580e88`, with records only after R. I ran at the tip and did not check out R, so the run rows and the copied compile rows say `7276f826533f`. The code diff from R is empty, as above.
2. **Each preset ran as three steps, not as a workflow.** Every preset, not only the two debug presets, ran `cmake --preset P`, then `cmake --build --preset P`, then `ctest --preset P`, each through `tools/timed`, so a build and its suite each have a row. The wall time in the table is the sum of the three rows. Gate 3 ran `cmake --workflow` for each preset, so its single figures include the workflow's own small overhead.
3. **Order.** Following plan 0042 section 5 (U8): gcc-debug and clang-debug were configured and built first; their compile rows were then copied by decision 0080's step; then their suites ran. The other nine presets followed one at a time.
4. **Two copies per debug tree.** The compile-fail tests compile their sources while CTest runs, and the compile launcher records those compiles in the tree's `compile.tsv`. So I ran 0080's copy step twice per debug tree: once after the build (137 rows each), as the plan's order says, and once after the suite (334 rows for gcc-debug, 336 for clang-debug), as decision 0080's "after the run" says. Before the second copy, a check found that no source and object pair of the suite's rows was also among the build's rows. Each copy passed every check of the step's second amendment: one revision, not dirty, no compile twice. In all, 944 rows were appended to `records/performance/compile.tsv`.
5. **DoD-22 is in shared mode (decision 0248 item 5).** The rows come from parallel builds on the shared pool, not from pinned serial builds (P14 note below).
6. **The benchmarks ran once each, as asked. That is not a P6 measurement.** Each benchmark test ran once on each compiler, pinned to CPU 4, under the timing lock. A single process of 524,288 operations, with no repeats, gives no spread, so the figures below are recorded and are not compared (P6, P14). This matches the release's claim, which states no benchmark of its own code (plan 0042 section 3.4).
7. **Added check.** `conventions_check.py` (seed 20261031) is not in the brief's list. It ran at M5, and plan 0042 section 10 lists its seed among the release's evidence, so I ran it as well (2.7 s).
8. **The snippets index was not edited.** Decision 0248 item 7 says the `plan_cost_model` and `jobserver_check.py` index rows are to be refreshed. The brief limits this unit's commits to run rows and this record, so the refresh is left open (see "Open").
9. **`cost.py comments` on `plan.hpp` checks nothing.** It printed "0 counts in the comments are not in the figures", but `plan.hpp` now holds no count, so header mode passes on nothing (blind review of plan 0040, `records/reviews/2026-11-10-pre-release-core-pass-review.md`). The Markdown mode on the limits page's section "Steps and operations of each plan" is the check that has content, and it also passed.

## The matrix

All rows are `gate:all@7580e88`, at `7276f826533f`, `runs.tsv` rows 21905 to 21937, and each tree started empty. The one existing tree, `build/gcc-debug`, held only U7's configure step. It was removed before the run; it was listed first and its path resolved to absolute.

| Preset | Forced inlining | Tests | Wall (s) | Notes |
| --- | --- | ---: | ---: | --- |
| gcc-debug | on | 657 of 657 | 159.5 | build 95.8, suite 62.9. Largest compile `transform_identities_test.cpp` 5.93 GiB (6,214,900 KiB), 92.3 s. Gate 3: 162.8 |
| clang-debug | on | 657 of 657 | 100.0 | build 35.5, suite 63.9. Largest `transform_identities_test.cpp` 1.51 GiB, 32.8 s. Gate 3: 104.0 |
| gcc-release | on | 657 of 657 | 157.1 | build 92.7, suite 63.8. Largest `transform_identities_test.cpp` 1.96 GiB, 87.7 s. Gate 3: 155.9 |
| gcc-asan | off | 649 of 649 | 228.8 | build 164.5, suite 63.7. Largest `transform_identities_test.cpp` 4.73 GiB, 151.3 s. Gate 3: 231.5 |
| clang-release | off | 657 of 657 | 113.7 | build 49.3, suite 63.9. Largest `transform_identities_test.cpp` 1.03 GiB, 45.7 s. Gate 3: 113.7 |
| clang-asan | off | 657 of 657 | 204.5 | build 139.7, suite 64.1. Largest `transform_identities_test.cpp` 2.68 GiB, 132.0 s. Gate 3: 198.8 |
| lint | on | 657 of 657 | 264.9 | build 201.1 (clang-tidy, `all_verify_interface_header_sets`, `format-check`), suite 63.0. No diagnostic in the build log. Gate 3: 267.3 |
| gcc-measure | on | 649 of 649 | 153.5 | build 89.2, suite 63.9. Largest `transform_identities_test.cpp` 1.85 GiB, 84.1 s. Gate 3: 154.5 |
| gcc-measure-o3 | on | 649 of 649 | 167.7 | build 103.6, suite 63.7. Largest `transform_identities_test.cpp` 2.03 GiB, 97.9 s. First gate run of this preset |
| clang-measure | on | 649 of 649 | 667.4 | build 603.0, suite 63.8. Largest `identities_rounding_test.cpp` 602.5 s, 1.81 GiB. Gate 3: 664.0 |
| clang-measure-o3 | on | 649 of 649 | 666.6 | build 602.1, suite 64.0. Largest `identities_rounding_test.cpp` 601.5 s, 1.80 GiB. First gate run of this preset |

- "Forced inlining" is each tree's `GALOOT_FORCE_INLINE` cache value. gcc-asan, clang-release and clang-asan set it OFF (decision 0246 item 1); the other eight keep the default, ON. `rga3.reference_kernels_forced` and the structure tests `rga3.kernels_inline` and `rga3.kernels_inline_alone` run the forced code in every preset, the three OFF trees included.
- The test counts are each preset's full suite under its test preset's filter, which leaves out the labels `runlog` and `bench`. They equal gate 3's.
- Peak memory is the largest resident size of one process. The table's figures come from each tree's `compile.tsv`; for the two debug trees, they come from the copy saved before the rows were moved. They agree with the build rows' `maxrss_kib` in `runs.tsv`.
- Every suite takes about 63 s because its longest test, `tools.jobpool` (61.9 s on gcc-debug), bounds it. The next longest is `core.identities` at 24.1 s.
- Total matrix wall time: 2,883.7 s (48 min), from 02:28:25 to 03:16:56 UTC.

### Pool and memory

The job pool read "running: 23 of 23 slots free" before and after every step. It was running at the start, so it was not started, and it was never stopped. Each step ran in its own session (`setsid`), with a watchdog that sampled every 2 s. The watchdog read `MemAvailable` and the largest resident size of any process in the step's session, and would have stopped the session below 15 GiB available or above 20 GiB in one process, as at gate 3. It never fired. The least memory available during any step was 92.4 GiB (the gcc-asan build), out of 125 GiB. The largest process it sampled was gcc-debug's `cc1plus` at 5.91 GiB.

### Friction check values (plan 0042 section 4, "Known friction")

- Item 1: gcc-debug's `transform_identities_test.cpp` peaks at **5.93 GiB** (6,214,900 KiB) in 92.3 s at R. That is the same as gate 3's 5.93 GiB, so the release record's figure stands.
- Item 2: the clang-measure tree takes **667.4 s** at R (configure 0.6, build 603.0, suite 63.8), and `identities_rounding_test.cpp` takes 602.5 s of the build. Gate 3 gave 664.0 s and 599.0 s. clang-measure-o3 takes the same: 666.6 s, with 601.5 s on that unit.

## DoD-22 rows (decision 0080, shared mode)

Rows from both debug trees were appended to `docs/agent/records/performance/compile.tsv` by the function in decision 0080's second amendment, copied verbatim:

| Tree | After the build | After the suite (compile-fail tests) | Total |
| --- | ---: | ---: | ---: |
| gcc-debug | 137 | 334 | 471 |
| clang-debug | 137 | 336 | 473 |

Every row is at `7276f826533f` on `feature/release-1.0.0`. After the copies, `tools/measure read` accepts the committed record.

**P14 note.** These rows are in shared mode. The builds ran 23 jobs at once on the shared pool, unpinned, on processors 0 to 31, so each compile may have run on either kind of core with the other thread busy. Their wall times are not comparable with pinned serial rows, nor, strictly, with one another. Peak memory and retired instructions do not depend on the kind of core, and those columns can be compared. Pinned serial builds (decision 0233 item 5) stay deferred (decision 0246 item 2, decision 0248 item 5).

## The other checks

All at `7276f826533f`, with each row in `runs.tsv`:

| Check | Selection, row | Wall (s) | Result |
| --- | --- | ---: | --- |
| Convention trace gate (W5) | `gate:trace@7580e88`, 21938 | 0.3 | exit 0; "items with no test: 6.1, 6.2, 6.3, 6.4" (the cga3 items deferred by decision 0211 item 2), as before |
| `conventions_check.py`, seed 20261031 | `conventions_check@7580e88`, 21939 | 2.7 | "all convention checks passed (seed 20261031)" |
| `format-check` on the lint tree | `gate:format-check@7580e88`, 21940 | 1.7 | exit 0; also passed inside the lint build |
| `comments_test.py` | `gate:comments_test@7580e88`, 21941 | 0.2 | 16 tests, OK |
| `cost.py comments include/galoot/core/plan.hpp` with both figures files | `gate:cost-comments-plan@7580e88`, 21942 | 0.1 | "0 counts in the comments are not in the figures" (a check of nothing; departure 9) |
| `cost.py comments docs/guide/limits.md --section "Steps and operations of each plan"` with both figures files | `gate:cost-comments-limits@7580e88`, 21943 | 0.1 | "0 counts in 'Steps and operations of each plan' are not in the figures" |
| `cost.py selfcheck`, Clang data | `gate:cost-selfcheck-clang@7580e88`, 21944 | 80.6 | "1228 measured cases predicted; 9 whole units whose largest evaluation is the metric's check, not compared; 0 differences" |
| `cost.py selfcheck`, GCC data | `gate:cost-selfcheck-gcc@7580e88`, 21945 | 80.6 | "1235 measured cases predicted; 9 whole units ... not compared; 0 differences". Its wall time equals the Clang run's to the millisecond. The two rows differ in start time, user time and instructions, so these are two runs and the equal times are a coincidence |
| `jobserver_check.py`, all five cases | `gate:jobserver-check@7580e88`, 21946 | 12.7 | SIGKILL returns 0 of 3 tokens, SIGINT and SIGTERM 3 of 3; a pool replaced under a build lets 8 jobs run; a missing FIFO lets Ninja run 34 jobs on 32 processors. These are the results of `6a3cf13` and `b28cc34`, with Ninja 1.13.2. The jobs only sleep, on a private pool; the shared pool is never opened |
| `tools/runlog check` | `gate:runlog@7580e88`, 21949 | 2.6 | "1 eras match the history; 1727 kept rows present once; no removed row present" |

The selfcheck and comments commands are those of `records/performance/plan-cost-model/README.md`, "Commands". Both `selfcheck` results equal those recorded at `6a3cf13`.

## The 50-seed identity campaign (decision 0216 item 6)

The six programs of M5, run as `<program> --rand-seed=<k>` for k = 1 to 50 on the gcc-debug tree. Each run went through `tools/timed --log` into the campaign log `records/gates/2026-11-12-release-seeds.tsv`, under the selections `gate:seed:<k>:<program>@7580e88`, from 03:20:21 to 03:39:22 UTC, one run at a time.

| Program | Runs | Passed | Median wall (s), low median | Range (s) |
| --- | ---: | ---: | ---: | --- |
| `core_identities_test` | 50 | 50 | 21.825 | 21.767 to 21.937 |
| `core_dual_identities_test` | 50 | 50 | 0.032 | 0.028 to 0.087 |
| `core_transform_identities_test` | 50 | 50 | 0.191 | 0.185 to 0.270 |
| `core_identities_rounding_test` | 50 | 50 | 0.159 | 0.152 to 0.272 |
| `core_basis_change_test` | 50 | 50 | 0.055 | 0.043 to 0.143 |
| `core_basis_change_identities_test` | 50 | 50 | 0.235 | 0.163 to 0.387 |

300 of 300 runs exit 0, so no failure became a fixed test or a defect report (V7, W9). This is a sample of 50 seeds per program, a hypothesis and not a theorem (V9). `core_identities_test` took 21.8 s per run against M5's 16.2 s. The cause is not traced: the program has changed since `1aa1e40`, and both campaigns ran in shared mode, so the times are not comparable (P14).

Other test sources also read doctest's seed (`grep -l rand_seed test`, 21 files, among them `reference_kernels_test.cpp` and the conformal and Witt basis-change tests). The brief says "as at M5", so the campaign ran M5's six programs only.

## Benchmarks, once

`taskset -c 4 tools/timed test <preset> gate:bench@7580e88 -- ctest --test-dir build/<preset> -L '^bench$' -V`, on gcc-measure (row 21947) and clang-measure (row 21948). Each run is pinned to CPU 4, a performance core (as in `wu17-bench`). The busy time of its sibling CPU 5 was not read, so P14's "other thread idle" is assumed, not shown. Each run holds the timing lock (`lock.sh take`, taken after 0 s at 03:39:44 UTC and released at 03:39:45 UTC by the script's exit trap). The pool read 23 of 23 free before and after, and the 1-minute load average was 1.63 before and after. The other load on the machine is unknown, so no energy figure is attributed. Both runs passed 22 of 22 tests, and the checksums of the two forms agree in every one of the 11 pairs on each compiler.

Nanoseconds per operation, one process each, 524,288 operations (128 operand sets times 4,096 passes), seed 20261110:

| Operation | Loop | GCC, Galoot | GCC, reference | Clang, Galoot | Clang, reference |
| --- | --- | ---: | ---: | ---: | ---: |
| `wedge_point_point` | throughput | 15.000 | 3.879 | 1.461 | 1.531 |
| `wedge_point_point_point` | throughput | 6.277 | 10.170 | 2.778 | 3.023 |
| `antiwedge_line_plane` | throughput | 7.339 | 6.402 | 1.446 | 1.637 |
| `transform_point` | throughput | 8.100 | 8.013 | 5.725 | 5.588 |
| `transform_point` | latency | 7.734 | 10.450 | 8.512 | 6.696 |
| `transform_line` | throughput | 9.734 | 9.277 | 8.507 | 8.752 |
| `transform_line` | latency | 12.661 | 12.347 | 11.620 | 11.404 |
| `transform_plane` | throughput | 7.393 | 7.431 | 5.476 | 5.440 |
| `transform_plane` | latency | 7.307 | 9.772 | 6.094 | 6.574 |
| `compose_motors` | throughput | 8.100 | 7.828 | 5.973 | 5.443 |
| `compose_motors` | latency | 8.054 | 7.933 | 6.977 | 6.925 |

Each test is one short process, a few milliseconds long, and each figure includes the checksum's work. There are no repeats, so no spread is known. GCC's first figure, 15.0 ns, is the first process of the run. It is far from its reference (3.9 ns) and from Clang's 1.5 ns. A start-up effect is likely, but that is inferred and not traced. The protocol of `records/performance/wu17-bench/` (11 interleaved rounds, three environment sizes, a busy rule) did not run, so no row here supports a comparison of the two forms (P6).

## Open

- **Snippets index rows (decision 0248 item 7).** The `jobserver check` and `plan cost model` rows of `docs/agent/snippets/README.md` and the header of `jobserver_check.py` still name `6a3cf13` as their last check. They could name this run at `7276f82` (code of `7580e88`), whose results are the same. That is a records-only edit, left to U9 or the coordinator because this unit commits only run rows and this record.
- **Benchmark figures.** If the release record should cite benchmark figures of the 1.0.0 code, the protocol of `wu17-bench` must run again at R: about 25 minutes under the lock, at the cost of one more lock session. Otherwise the release keeps its stated position of no benchmark claim, and this run stands as a check that the benchmarks run and that both forms agree. I recommend keeping the position, as plan 0042 section 3.4 already states it.

## Commands

From the worktree root, with `eval "$(tools/jobpool env)"`, for each preset `P` in the order of the table (the debug pair first, with the copy step between their builds and their suites):

```bash
tools/timed configure P gate:all@7580e88 -- cmake --preset P
tools/timed build P gate:all@7580e88 -- cmake --build --preset P
tools/timed test P gate:all@7580e88 -- ctest --preset P
```

The copy step is the function `copy_compile_rows` of decision 0080's second amendment, run as `copy_compile_rows <absolute tree> docs/agent/records/performance/compile.tsv`. The seed runs are `tools/timed --log <abs>/docs/agent/records/gates/2026-11-12-release-seeds.tsv test gcc-debug gate:seed:<k>:<program>@7580e88 -- build/gcc-debug/test/<program> --rand-seed=<k>`.
