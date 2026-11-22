# Merge gate of `feature/header-hygiene-1-0-1`, at `41d8029`

- Unit: U7 of plan 0044 (sections 5, 7, 9 and 10), the merge gate. Run by Claude (agent, Opus 5.5), in the performance-engineer role, on 2026-11-21 from 22:18:50 to 22:34:50 UTC, in the worktree `galoot-wt/header-hygiene`, branch `feature/header-hygiene-1-0-1`.
- Revision: `41d8029c004b53769524c82f9905df6d9bd05252`, the merge of `main` at `95341f6` into the feature branch (tip `3aa9208`: U1 to U5 and the fix round). The merge had no conflict: `test/CMakeLists.txt` merged automatically, and `runs.tsv` kept every row of both sides (22,449 rows on `main` plus 22,119 on the branch, less 21,980 shared, gives the merged 22,588; no row of either side is missing). `main` changed nothing under `include/`, `test/bench/`, `CMakeLists.txt` or `CMakePresets.json` since the branch left it (`git diff --stat 01dce7f 95341f6` over those paths is empty), and the merge left `include/` as at `3aa9208`. Every row of the gate names `41d8029c004b`, none dirty.
- Toolchain: g++ 16.2.1 20260810, clang++ 23.1.5 (clang-tidy and clang-format 23.1.5), CMake 4.5.1, Ninja 1.13.2: the same as plan 0043's matrix at `05229a7`.
- Reviews: the blind review of the branch at `67907a5` and the delta review of the fix round at `3aa9208` both approve (`review/header-hygiene-1-0-1`, commits `5019bff` and `3df1e81`).
- Outcome: **every check passed.** The five presets of section 9 configured, built and passed their full suites; `format-check`, the convention trace gate and `tools/runlog check` exited 0; the confusable scan finds only `q1`, which the plan keeps. The section 7 comparisons show only the allowed differences. The memory watchdog never fired. No check was skipped or deferred.

## Departures and readings (C20)

1. **Three of the five trees were built incrementally.** gcc-debug, clang-debug and lint were the trees U1 to U5 left in this worktree; the merge's change to the build description made CMake regenerate them, and Ninja rebuilt what changed (P12). gcc-asan and clang-measure did not exist and were built from empty trees. So only those two builds compare with plan 0043's matrix, which started every tree empty (below).
2. **The timing lock was held only for the W10 rows**, from 22:33:35 to 22:34:03 UTC and again, for the control, from 22:34:22 to 22:34:50. The matrix is not a timing measurement, and the preprocessed and code-generation comparisons compare outputs, not times, so they ran outside it. The brief's "under the timing lock" was read as applying to the measurement.
3. **The W10 result is not "within the repeats' range" in the literal sense** (plan section 7, item 6): the after mean is 70.1 M instructions (0.12%) below the before mean, and the two ranges do not overlap. A control that the plan does not ask for, the same tool with the gated revision on both sides, shows a spread of 77.7 M across its six compiles of identical sources and puts its before side 41.3 M above its after side. So the difference is within what the tool resolves for this unit, and no change is attributed to the branch. The tool's before tree sits under a longer path, which `__FILE__` and the debug information carry; that this explains the before side's lean is inferred, not shown.
4. **The confusable scan is a reconstruction.** Section 3.3 gives the planner's name set by its rule and its count (5,457 names in one unit), not as a file. `scan.py` takes every one- and two-character identifier that does not start with an underscore and the three-character ones built from `l I 1 O 0 r s n x`, 3,886 names once nine are left out that cannot be declared in the global namespace beside the headers (`do if or j0 j1 jn y0 y1 yn`). Since the check reports only one clash per look-alike group, the names are spread over 18 units, no two in one unit sharing a skeleton (`I` and `1` read as `l`, `0` as `O`, `rn` as `m`), so that a user's own pair cannot hide a header name. The same scan against the headers of `c95fee5` reports `l` and `m`, so it can fail.
5. **The watchdog's settings** are plan 0043's: `MEMGUARD_MIN_GB=15`, `MEMGUARD_MAX_GB=60` for the step's process tree.
6. **Definition of done, item 7, second half,** is read as decision 0257 rules: met by `core.basis_change_equality` and `core.basis_change_inequality` with the guide paragraph, with no rejected block. Both tests passed in all five presets.
7. **Section 4's first criterion lists "two renamed locals".** The branch's diff of `include/` against `c95fee5` also renames the locals `m` of `metric.hpp` to `result`, the parameter `m` of `is_valid_form` to `form`, and the parameter `l` of rga3's `rotation` to `axis`, which U3 found hidden behind the first two and recorded (`records/mutations/0044-header-hygiene/README.md`); the blind review covered them. A parameter's name is not part of a C++ interface. Nothing else differs (below). Noted for U8's outcome.
8. **The run log's conditions files of W10** (`out/conditions-w10-*.txt`) are the control's, since the control ran the same script and overwrote them. The first run's conditions are in its campaign log (`w10/runs.tsv`) and in "Conditions" below.

## The matrix (section 9, P9 `main` row)

All rows `gate:all@41d8029`, `runs.tsv` rows 22589 to 22603.

| Preset | Tree | Tests | Wall (s) | Build (s) | Suite (s) | Watchdog: build tree peak, least available |
| --- | --- | ---: | ---: | ---: | ---: | --- |
| gcc-debug | incremental | 660 of 660 | 129.4 | 65.2 | 63.8 | 18.3 GiB, 86.8 GiB |
| clang-debug | incremental | 660 of 660 | 90.9 | 24.3 | 66.0 | 8.6 GiB, 99.5 GiB |
| gcc-asan (sanitizers, forced inlining off) | empty | 652 of 652 | 202.5 | 137.8 | 64.1 | 20.6 GiB, 88.5 GiB |
| clang-measure | empty | 652 of 652 | 119.4 | 54.3 | 64.2 | 9.9 GiB, 102.1 GiB |
| lint (clang-tidy, header sets, `format-check`) | incremental | 660 of 660 | 218.4 | 153.6 | 64.4 | 9.5 GiB, 102.1 GiB |

- Total wall time of the 15 rows: 760.6 s (12.7 min); the gate's elapsed time, from the first configure to the run log check, was 13 min 9 s. Section 9 estimated about 24 minutes from gate 3's times, before plan 0043.
- The counts are three above plan 0043's matrix (657 and 649): `core.public_macros`, `tools.configure_rejects_force_inline` and `tools.configure_rejects_checking`. The sanitizer and measure presets' test presets leave out eight tests, as before.
- No build log holds a warning or an error. The lint build ran clang-tidy on every source (among them the new `core_user_names` unit), `all_verify_interface_header_sets` and `format-check`.
- The tests section 7 names passed in all five presets: `core.public_macros`, `core.force_inline_empty`, `_on` and `_build`, both configure tests, `core.basis_change_equality` and `_inequality`, `tests.messages_pinned`, `tests.compile_fail_flags`, `tools.catalogue`, `core.include_scan`, `core.bare_compile` and its twins, `rga3.kernels_inline` and `_alone`, `tests.docs_pages` and every documentation example test.

## The other checks

| Check | Selection, row | Wall (s) | Result |
| --- | --- | ---: | --- |
| `format-check` on the lint tree | `gate:format-check@41d8029`, 22604 | 1.7 | exit 0 |
| Convention trace gate (W5) | `gate:trace@41d8029`, 22605 | 0.3 | exit 0; "items with no test: 6.1, 6.2, 6.3, 6.4", as at the release and at plan 0043's gate |
| `tools/runlog check` | `gate:runlog@41d8029`, 22606 | 2.5 | exit 0; "1 eras match the history; 1727 kept rows present once; no removed row present" |
| Confusable scan (section 3.3), `scan.py` | `confusable-scan@41d8029`, 22607 | 10.1 | 3,886 names, 18 units; two pairs with a header side, both `q1` (`transform.hpp:654`) against `qI` and `ql`, kept by the plan (section 1, item 7). No header declares `ql` or `qI`, so nothing hides behind `q1` |
| The same scan on the headers of `c95fee5` (control) | `confusable-scan-control@c95fee5-headers`, 22608 | 10.2 | four pairs: `l` (`basis_change.hpp:542`) against `I`, `m` (`bits.hpp:171`) against `rn`, and the two of `q1` |

## Section 7: behavior unchanged

1. **Preprocessed comparison** (`records/mutations/0044-header-hygiene/behavior/compare_lines.sh`, old side `git archive c95fee5 include`, new side the gated `include/`; row 22609, 3.1 s). Both compilers, `GALOOT_CHECKING` 0 and 1, `GALOOT_FORCE_INLINE` unset, bare, 0 and 1: in every mode the normalized `-E -P` output loses 38 lines and gains 25 (Clang at mode 0: loses 36, since it prints no attribute line where GCC prints an empty one). The normalization is the script's: the include path becomes `<include>`, and the `__LINE__` argument of each `GALOOT_ASSERT` expansion becomes `N`. The lines are the two deleted `apply` templates and the renamed tokens (`m` to `k` in `antireverse_sign`; `m` to `result` and `form` in `metric.hpp`; `l` to `c` in `keeps_inner_products`; `l` to `axis` in `rotation`); the sixteen diffs come in four identical shapes. `-dM` loses exactly `GALOOT_DETAIL_FORCE_INLINE_IS_0`, `_IS_1`, `GALOOT_DETAIL_PASTE_` and `GALOOT_DETAIL_PASTE` in every mode, and gains nothing. These are the fix round's counts.
2. **Code generation** (`compare_codegen.sh`, row 22610, 7.5 s): `test/bench/kernels_inline.cpp`, unchanged on the branch, compiled with `rga3.kernels_inline`'s flags (`-std=c++23 -DGALOOT_CHECKING=0 -O2 -ffp-contract=off` and the warnings as errors) against both header sets; `objdump -d --no-show-raw-insn` is identical on GCC and on Clang, with the forced-inline mode unset (as in the test) and at 0.
3. **Compile-fail lane, documentation tests, include scan and header checks**: in the suites above.
4. **W10** (below).

## W10: build cost before and after (section 7, item 6)

`tools/compiles cost units.toml w10 --before c95fee5 --cpu 4 --repeats 3`: `test/core/product_test.cpp` with the gcc-debug tree's flags, pinned to CPU 4 (a performance core; its sibling, CPU 5, was busy for at most 0.01 s during any compile), repeats interleaved, under the timing lock. Campaign logs `w10/runs.tsv` and `w10-control/runs.tsv`; rows `w10/rows.tsv`.

| Run | Tree | Instructions, mean (10⁶) | Range (10⁶) | Wall, mean (s) | Peak (KiB) |
| --- | --- | ---: | ---: | ---: | ---: |
| W10 | before (`c95fee5`) | 58,594.3 | 12.6 | 4.517 | 849,668 |
| W10 | after (`41d8029`) | 58,524.1 | 40.3 | 4.486 | 849,632 |
| control | "before" (`41d8029` from the tool's extracted tree) | 58,575.3 | 60.6 | 4.507 | 849,628 |
| control | after (`41d8029`) | 58,534.0 | 25.0 | 4.501 | 850,692 |

Reading: after minus before is −70.1 M (−0.12%); the control's two sides, identical sources, differ by −41.3 M, and its six compiles span 77.7 M. Peak memory agrees to 0.1%. So the branch's effect on this unit is below what this measurement resolves, as the plan expected, though not inside the repeats' range (departure 3).

### Tree level, against plan 0043's matrix

The two empty-tree builds compare with `gate:all@05229a7` (`runs.tsv` rows 22388 to 22420), whose code differs from this revision by plan 0044's change alone, with the same toolchain. Shared mode, so instructions and peaks, not wall times, are the comparable columns (P14).

| Preset | Build instructions, `05229a7` to `41d8029` (10⁹) | Compile units' sum (10⁹) | Units in common | `core/user_names.cpp` (new) | Largest process (KiB) |
| --- | --- | --- | --- | --- | --- |
| gcc-asan | 11,722.3 to 11,752.2 (+0.26%) | 11,459.2 to 11,448.7 | 129: −0.13% | 4.34 | 2,486,984 to 2,489,252 |
| clang-measure | 5,696.7 to 5,718.1 (+0.38%) | 5,594.7 to 5,591.0 | 129: −0.15% | 4.86 | 644,544 to 644,040 |

- The compiles of the units both revisions share got cheaper; the largest falls are `mv_test.cpp` on clang-measure (87.6 G to 80.3 G, −8.3%), which U4 rewrote, and on gcc-asan `transform_rounding_assert_fail.cpp` (−16.0%) and `transform_assert_fail.cpp` (−13.5%), sources the branch did not change. That the deleted `apply` overloads, no longer candidates at each `detail::apply` call, explain the second pair is inferred, not traced.
- **Unexplained:** the build rows rose although the compiles' sum fell. The part of each build outside the compile rows (links, the tools, Ninja and CMake) grew from 263.1 G to 303.5 G on gcc-asan and from 102.0 G to 127.1 G on clang-measure. It was not traced; no compile row accounts for it.
- The per-unit rows are `out/compile-gcc-asan.tsv` and `out/compile-clang-measure.tsv`; plan 0043's are `records/performance/cost-cleanup-1-0-1/matrix/`.

## Conditions

Shared mode, unpinned, processors 0 to 31 eligible, the job pool exported for every build and test step. The pool read "running: 23 of 23 slots free" before and after every step; it was running at the start, so it was not started, and it was never stopped. Other load, from `top` at each preset's start and end (`out/conditions-*.txt`): the editor's C++ language service at about one processor and an unrelated session's Python job at about one; no other build drew from the pool at those moments. The 1-minute load average was 3.8 at the start and 6 to 9 between presets, which includes the gate's own steps. During the W10 compiles the load average was 4.6 and falling, the pool idle, and no build of this gate running.

## Files

- `gate.sh` (the matrix, as run, with its log `gate.log`), `out/` (watchdog logs, conditions, the logs of `format-check`, the trace gate and `tools/runlog check`, and the two empty trees' compile rows).
- `scan.py`, with `scan/` (the gated revision) and `scan-c95fee5-headers/` (the control): `summary.txt` and `clashes.tsv`.
- `compare_codegen.sh`; the preprocessed comparison is the branch's `records/mutations/0044-header-hygiene/behavior/compare_lines.sh`.
- `units.toml` and `w10.sh` (the W10 run; its header says how the control differed), `w10/` and `w10-control/` (`summary.txt`, `rows.tsv`, the campaign log `runs.tsv`).

## Open

- **The diagnostics catalogue's places in `mv.hpp`** (`records/diagnostics/catalogue/scan-output.md`) are those before U4's deletion, which moved the messages below it by 10 and 18 lines. No check reads them, and `tools.catalogue` passed in every preset. Options: (a) regenerate the record in U8 or at plan 0045's release commit, a few seconds and a records-only change; (b) leave it until the catalogue is next touched. I recommend (a) at plan 0045's release commit, so that the release's record names the released places.
- **The build rows' growth outside the compiles** (above): about 40 G and 25 G. Options: (a) trace it now, by comparing the two revisions' link rows and Ninja's own log, about 15 minutes; (b) watch for it in plan 0045's eleven-preset matrix, which builds every tree from empty and will show whether it repeats. I recommend (b): it is 0.3% of the build, no compile accounts for it, and the release matrix gives a second sample at no extra cost.
