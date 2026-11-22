# Release record: galoot core 1.0.1

- Status: written 2026-11-21 (UTC 2026-11-22) by Claude (agent, Opus 5.5), in the planner's role, plan 0045 unit R7, on branch `feature/release-1.0.1`, from the release matrix at the release commit R (`records/gates/2026-11-21-release-1.0.1-matrix.md`, unit R6). Records only. **The run-log compaction of decision 0252 item 14 is not made yet**: the check of open branches that must precede it found two branches beyond the one exception decision 0261 item 3 allows (departure 1), so the unit stopped before it. The sections that depend on it are marked **[compaction]**. Cells marked **[R8]**, **[owner]** or **[coordinator]** are filled by the release audit, the owner's walkthrough, or the coordinator at R10. The owner merges into `master` and tags (W13); nothing here is a release until then.
- This record is a delta on the 1.0.0 record, `release-record.md`. What it does not restate stands as that record says, and the section names and item numbers below are that record's.

## Departures, unknowns, and readings, first (C20)

1. **The compaction is held back.** Plan 0045 section 7.1 makes it a precondition that no other branch holding new `runs.tsv` rows is open; decision 0261 item 3 allows one exception, `feature/docs--tooling`. Rerun at this branch's tip (`6d09c0e`, the matrix's records tip), for every local branch, as the rows its log holds that the tip's log lacks, three branches hold such rows:

   | Branch | Commits not in the tip | Rows the tip lacks | Rows of its base it drops or edits |
   | --- | ---: | ---: | ---: |
   | `feature/docs--tooling` (the allowed exception) | 17 | 191 | 0 |
   | `review/docs-tooling` (the blind review of plan 0049 unit U2, from `fdd1557` of that branch) | 16 | 210 (the 191 and 19 of its own) | 0 |
   | `review/release-1.0.1` (the blind review and delta review of R5, `6745da5` and `9f8a70f`) | 3 | 10 | 0 |

   Every other branch is merged into the tip, or holds no row the tip lacks (`master` and the other eleven `review/` branches). Neither review branch edits or drops a row of its base, so decision 0261 item 3's reasoning ("new rows written after the release branch was cut; the compaction removes none of them") reads as true of both, and `tools/runlog compact` computes anchors for every local branch not merged into its boundary. But the brief names one exception, and widening it is the coordinator's reading, not this unit's, so the compaction waits for it (W8). Options are under "Open", item 1.
2. **The debug trees' compile rows were not copied again.** The brief asks this unit to copy them as decision 0252 item 19 rules; unit R6 already did, by decision 0080's function, at R's code. A second copy of the same trees would add every row twice, which the function itself refuses ("no compile twice"). Reading followed: the copy is R6's, and this unit checked it ("DoD-22" below).
3. **The rows' revision is `f6e00fb`, not R**, as at 1.0.0 (its departure 1). `git diff --stat 578aa15 f6e00fb -- . ':!docs/agent'` prints nothing.
4. **clang-measure-o3's wall time at R is not comparable.** A lint build of the documentation-tooling unit held the pool from about 23:35 to 23:39 UTC (matrix record, departure 6). Known friction item 2 quotes that tree's wall time from the cost-cleanup matrix at `05229a7` with R's instruction count, the matrix record's open item 4, option (a), which no reading forbids.
5. **No benchmark figure of the release's own code is claimed**, as at 1.0.0 (its departure 4). The benchmarks ran once at R as a check.
6. **The R5 review records are not on this branch.** The blind review (`2026-11-21-release-1.0.1-r5-blind-review.md`, `6745da5`) and the delta review (`2026-11-21-release-1.0.1-r5-delta-review-1.md`, `9f8a70f`) are on `review/release-1.0.1` only. W7 has a review record's commits cherry-picked; this unit's brief does not ask for it, so they are cited here by branch and commit (see "Open", item 1 (b)).
7. **No failure was found** in the matrix, the checks, the campaign, the benchmarks or this unit's checks; nothing below is a failure written as a pass.

## Release

- Component: galoot core, version 1.0.1, a patch release (W13; decision 0252 item 20).
- Release commit R: `578aa15bdb6a1f30c2e9de491a757eb6bb9aea8c` (2026-11-21T23:02:18Z, "Release 1.0.1: fixes from the blind review of the release commit"). The release commit of R5 is `2b3af4f`; R is the last commit of the delta fixes that changes anything outside `docs/agent/` (decision 0261 item 5). Blind review of `2b3af4f`: approve, no blocking or major finding, three minor and four nits; delta review of the fixes to `f6e00fb`: approve, one nit (departure 6). Both by Claude (agent, Sonnet 5.5), blind-reviewer role.
- Tag: `1.0.1`, on the `--no-ff` merge commit of `main` into `master`, whose tree is `main`'s at the merge (decision 0250 item 4, as at 1.0.0). The previous release, the tag `1.0.0` (`95a088e`, on `2383e6d`, 2026-11-15T16:40:19Z, "galoot core 1.0.0, CONVENTIONS 1.18"), and `master` are on the remote (decision 0257 item 2).
- CONVENTIONS version implemented: **1.18**, unchanged since the 1.0.0 tag (`git diff --stat 1.0.0 578aa15 -- docs/agent/governance/CONVENTIONS.md` prints nothing; its last change is `68f77fc`, 2026-11-15). The tag message names 1.18 (plan 0045 section 4.1).
- Version strings at R: `project(galoot VERSION 1.0.1 LANGUAGES CXX)` at `CMakeLists.txt:6`; the README's status line "This is the 1.0.1 release of the core."; the README's `FetchContent` block with `GIT_REPOSITORY "https://github.com/IA-Workshop/galoot.git"`, `GIT_TAG "1.0.1"` and `GIT_SHALLOW TRUE` (decision 0252 items 13 and 15). No header carries a version.
- Release notes: `CHANGELOG.md` at the repository root (decision 0252 item 15, option (b)), with a 1.0.0 entry and a 1.0.1 entry. **The 1.0.1 entry is dated 2026-11-21, the date of the release commit, whatever day the tag is made** (decision 0261 item 4).
- Preset version: `rga3` stays below 1.0, as the README and the changelog say. cga3 is not in this release.
- No `install()` rule and no package for `find_package`, as at 1.0.0.

## The tip T

T is the commit the owner merges (`main`, fast-forwarded to it at R10). A commit qualifies as T when all of these hold:

1. `git diff --stat 578aa15 T -- . ':!docs/agent'` prints nothing. Unlike 1.0.0, no change outside `docs/agent/` follows R, so the README and the changelog that the matrix tested are T's.
2. `tools/runlog check` and the trace gate passed on T's tree, or on its parent's tree with T adding only their run rows to `runs.tsv` (1.0.0's rule 3, which the close-out script's step 3 checks).
3. A change after R under `include/`, `test/`, `tools/`, `cmake/`, `docs/guide/`, the build files, `README.md` or `CHANGELOG.md` voids R and reruns the matrix whole (plan 0045 section 8, R9). A change to the governance after T stops the close-out script.
4. If the owner changes CONVENTIONS before the tag, the trace gate and `tools/runlog check` rerun on that commit and the tag message names the new version (1.0.0's rule 4). None is planned.
5. **[compaction]** The compaction, when made, is a records commit at or after the matrix's records tip; rule 2 then holds only if `tools/runlog check` passes after it.

## Platform and toolchain

- Tested at R: g++ 16.2.1 20260810 and clang++ 23.1.5 (clang-tidy 23.1.5), each with libstdc++ 16, CMake 4.5.1, Ninja 1.13.2, glibc 2.44, on x86-64 Linux, C++23 without compiler extensions. 1.0.0 was tested with clang++ 23.1.4 and CMake 4.4.5.
- **The change is marked in the performance history** (P7): `history/toolchain.tsv` gained the state of 2026-11-21T09:30:08Z (`9ad75e9`), read from the package log, the boot log and each tool's `--version`: llvm, clang and compiler-rt 23.1.4 to 23.1.5, CMake 4.4.5 to 4.5.1, Python 3.14.9 to 3.14.10, perf 7.3.1 to 7.3.5 and glibc 2.44+r50 to 2.44+r78, all between 09:29:25Z and 09:30:08Z; no run row falls in that window. The kernel package was upgraded too, but the running kernel is still 7.3.1-1-cachyos (booted 2026-11-21T00:25:31Z). Every gate of the 1.0.1 work (the matrices at `05229a7`, `41d8029` and R) ran after it, so their figures share one toolchain; against 1.0.0's figures the Clang rows hold the compiler change as well as the code's (plan 0043 section 14).
- What the 1.0.0 record says of exceptions, the bare compiles without exceptions and the rounding check's assumptions stands; the tests behind it passed at R in every preset.

## The release matrix at R

All eleven presets, each from an empty tree, one at a time on the shared pool, three steps each through `tools/timed`, selection `gate:all@578aa15`, `runs.tsv` lines 22710 to 22742, 2026-11-21 from 23:12:32 to 23:39:01 UTC (the last step's start), every row exit 0: plan 0045's acceptance criterion for the matrix holds. The memory watchdog (`MEMGUARD_MIN_GB=15`, `MEMGUARD_MAX_GB=60` for the step's process tree) never fired; the least memory available was 85.2 GiB of 125 GiB. Figures from the matrix record:

| Preset | Forced inlining | Tests | Wall (s) | Largest compile by peak |
| --- | --- | ---: | ---: | --- |
| gcc-debug | on | 660 of 660 | 141.0 | `identities_five_diagonal_test.cpp`, 3,145,604 KiB (3.00 GiB) |
| clang-debug | on | 660 of 660 | 89.8 | `identities_five_diagonal_test.cpp`, 1,053,688 KiB |
| gcc-release | on | 660 of 660 | 114.8 | `dual_identities_test.cpp`, 879,564 KiB |
| gcc-asan | off | 652 of 652 | 195.7 | `dual_identities_test.cpp`, 2,489,096 KiB |
| clang-release | off | 660 of 660 | 98.2 | `transform_test.cpp`, 538,596 KiB |
| clang-asan | off | 660 of 660 | 158.3 | `dual_identities_test.cpp`, 1,563,524 KiB |
| lint | on | 660 of 660 | 228.5 | clang-tidy, `all_verify_interface_header_sets`, `format-check`; no diagnostic |
| gcc-measure | on | 652 of 652 | 114.5 | `dual_identities_test.cpp`, 802,268 KiB |
| gcc-measure-o3 | on | 652 of 652 | 129.9 | `identities_rounding_antiproduct_test.cpp`, 905,184 KiB |
| clang-measure | on | 652 of 652 | 124.3 | `identities_rounding_antiproduct_test.cpp`, 644,168 KiB |
| clang-measure-o3 | on | 652 of 652 | 213.5 (not comparable, departure 4) | `identities_rounding_antiproduct_test.cpp`, 639,152 KiB |

1,608.5 s in all (26.8 min), against 2,883.6 s at 1.0.0. Each suite takes about 64 s, bounded by `tools.jobpool` (63.0 s). The counts are three above 1.0.0's (657 and 649): `core.public_macros` and the two configure tests of plan 0044. I checked the counts against `records/gates/2026-11-21-release-1.0.1-matrix/out/tests.tsv` (no test of any preset other than passed) and the rows against `runs.tsv`.

**The tests the five 652-test presets leave out** (the matrix record's section): the eight per-unit cases of the plan cost model's canary, `core.plan_cost_model.{gp_full5, gp_dense4, gp_grades_3_7, antigp_sparse16, reverse_full7, right_complement_full8, antiwedge_full5, gp_vectors_full6}`, which `test/CMakeLists.txt` registers only when `_GLIBCXX_ASSERTIONS` is defined and, on GCC, only without `-fsanitize`. They passed at R on the six other presets (48 runs), clang-asan among them. **No GCC sanitizer build runs them, by design**: GCC's sanitizers raise the counted operations by 2.2% to 4.4% (decision 0131), so the counted thresholds do not hold there (the matrix record's open item 3; nothing to act on).

## Other checks at R

All at `f6e00fb962bc`, each through `tools/timed`, each exit 0, from the matrix record and its logs:

| Check | Line | Result |
| --- | --- | --- |
| `format-check` on the lint tree | 22743 | passed; also inside the lint build |
| `conventions_check.py`, seed 20261031 | 22744 | "all convention checks passed (seed 20261031)" |
| `comments_test.py` | 22745 | 16 tests, OK |
| `cost.py comments include/galoot/core/plan.hpp` | 22746 | "0 counts in the comments are not in the figures"; `plan.hpp` holds no count, so it checks nothing, as at 1.0.0 |
| `cost.py comments docs/guide/limits.md --section "Steps and operations of each plan"` | 22747 | "0 counts ... are not in the figures" |
| `cost.py selfcheck`, Clang and GCC data | 22748, 22749 | 1228 and 1235 measured cases predicted, 9 not compared each, 0 differences: the results of 1.0.0 |
| `jobserver_check.py`, five cases | 22750 | the results of 1.0.0, with the `tools/jobpool` of plan 0043's U5, on a private pool |
| Benchmarks once, gcc-measure and clang-measure, CPU 4, under the lock | 22751, 22752 | 22 of 22 on each; both forms' checksums agree in all 11 pairs; sibling CPU 5 busy 10 ms over both runs. One process each, no repeats: no comparison is claimed (P6) |
| Convention trace gate | 22753 | "items with no test: 6.1, 6.2, 6.3, 6.4", the cga3 items, as at 1.0.0 |
| `tools/runlog check` | 22754 | "1 eras match the history; 1727 kept rows present once; no removed row present" |

**The seed campaign** (decision 0216 item 6): M5's six programs, seeds 1 to 50, on the matrix's gcc-debug tree, one run at a time, into `records/gates/2026-11-21-release-1.0.1-seeds.tsv` (300 rows, every one exit 0, from 23:43:29 to 00:02:52 UTC; checked here by counting the log). `core_identities_rounding_test` and `core_transform_identities_test` are each built from six sources now and are still one program each. A sample of 50 seeds per program supports a hypothesis, not a theorem (V9).

**The closing gates of this unit**, on this branch's records tip before the compaction: see "Evidence".

## Interfaces removed or changed incompatibly (W13)

**None.** No public name is added, removed or changed, and no user-set option is added, so 1.0.1 is a patch (decision 0252 item 20). Plan 0045 section 4.3's check, redone by this unit at R:

1. **The diff.** `git diff 1.0.0 578aa15 -- include/ docs/guide/` (eight files, 78 insertions, 76 deletions), read hunk by hunk:
   - `core/assert.hpp`: the check of `GALOOT_FORCE_INLINE` and the choice of `GALOOT_ALWAYS_INLINE` became one `#if` chain, and the four `GALOOT_DETAIL_*` helpers are `#undef`ed after it; `<cstdio>` and `<cstdlib>` are included earlier, the same headers; comment text. Every public macro (`GALOOT_CHECKING`, `GALOOT_FORCE_INLINE`, `GALOOT_ASSERT`) and every accepted value is as it was.
   - `core/basis_change.hpp`, `core/bits.hpp`, `core/metric.hpp`: locals renamed (`l` to `c`, `m` to `k`, `m` to `result`), the parameter of `detail::is_valid_form` renamed, comment words with them.
   - `core/mv.hpp`: the two untargeted overloads of `apply` deleted; they are inside `namespace detail` (the block from line 720 to 1004 of the 1.0.0 file), as `dod-2-inventory.md` line 81 states.
   - `presets/rga3/motions.hpp`: the parameter `l` of the four `rotation` overloads renamed `axis`, with its `///` comment. A parameter's name is not part of a C++ interface.
   - `docs/guide/core.md` and `limits.md`: the spellings of `GALOOT_FORCE_INLINE`, the paragraph on `==` and `!=` of two basis changes, and "build presets". No new name is documented.
2. **The inventory, mechanically.** `clang++ -std=c++23 -fsyntax-only -Xclang -ast-list` on one unit that includes `core.hpp`, `core/format.hpp` and `presets/rga3.hpp`, at the include trees of `1.0.0` and of R (by `git archive`, in scratch): the qualified names in `galoot::core` and `galoot::presets::rga3` outside `detail` are the same 115 at both. Each is in `dod-2-inventory.md` (the rga3 layouts under "their layouts"), except three template parameters the listing reports at namespace scope (`Name`, `Names`, `X`), which are not declarations. `-E -dM` on the same unit: the `GALOOT_` macros at R are those of 1.0.0 less the four `GALOOT_DETAIL_*` helpers. The four are not interface (the inventory's row); its words "never undefined" describe 1.0.0 and are now out of date. The inventory is a record of 1.0.0 and is not rewritten (W6).
3. **The build.** `CMakeLists.txt` adds no option; it rejects any `GALOOT_FORCE_INLINE` but `ON` or `OFF`, which only Galoot's own top-level build sees (the option sits after the `PROJECT_IS_TOP_LEVEL` return). Plan 0045 section 3.4 reads this as a bug fix, listed among the user-visible changes, not as an incompatible change.

User-visible changes, as the changelog states them for users (plan 0044 section 15): the four helper macros are no longer defined after including Galoot; an empty `GALOOT_FORCE_INLINE` gives one error instead of two; Galoot's own CMake option accepts only `ON` or `OFF`; locals and a preset parameter renamed; two `detail::apply` overloads removed; the README names the repository, the tag `1.0.1` and `GIT_SHALLOW TRUE`, and its copy route names `include/galoot`; the guide states where the deleted `==`'s hint appears; the project's own tests compile in smaller pieces.

## Known exceptions

Items 1 to 11 of the 1.0.0 record stand as written. 1.0.1 touches none of them: no change under `include/` touches the antiproducts, `bulk_dual` or the generation they compose, so decision 0247 item 4's fixed-use measurement does not apply (plan 0045 section 3.4). Decisions 0141 and 0247 keep their end conditions and the review date 2027-11-01. The tests named in items 3 and 4 (`core.transform_*` and `rga3.kernels_inline`, `rga3.kernels_inline_alone`) passed at R in every preset of the matrix.

Item 12, 1.0.0's open items at release, is closed: CONVENTIONS 1.18 (`68f77fc`), DoD-5 and DoD-28 and the readings (decision 0251), the walkthrough, merge and tag (`2383e6d`, tag `1.0.0`), and the push (decision 0257 item 2). The repository URL is filled by R (decision 0252 item 13), which closes decision 0249 item 1.

New for 1.0.1:

13. **Plan 0044's finding p1 is not fixed in 1.0.1** (decision 0260 section 1 item 5). A user who includes Galoot with `-I`, builds with `-Wundef -Werror` and misspells `GALOOT_FORCE_INLINE` gets two errors, the first naming the internal `GALOOT_DETAIL_FORCE_INLINE_IS_<spelling>`; 1.0.0 behaves the same. With `-isystem` the literal comes alone. Each of the three fixes probed inside `assert.hpp` changes something for valid builds or other spellings (decision 0254, its correction). It is a row of the hardening register for plan 0047's H6; the delta review of plan 0044 recommends the pragma form with a compile-fail test in 1.1.0.
14. **No limits text is added in 1.0.1** (decision 0261 item 2). The six-generator Clang failure with its `-fbracket-depth` remedy, and the higher constant-evaluation step counts of optimized Clang builds, go to plan 0049's limits page. The owner was asked (decision 0259 section 3 item 2) and has not ruled; the stated default holds, and ruling otherwise needs a new release commit and matrix.

## Known friction

Items 3, 6, 7 and 8 of the 1.0.0 record stand as written. The others:

1. **Debug builds' peak memory, restated.** gcc-debug's largest compile of the project's own tests is now `identities_five_diagonal_test.cpp`, 3,145,604 KiB (3.00 GiB) in 75.6 s at R, against `transform_identities_test.cpp` at 5.93 GiB at 1.0.0: plan 0043 split the test sources. This lowers the project's own peak and does not change a user's cost; a user's large debug unit can still need several GiB. Measured per form at `-O0` on GCC (`records/performance/cost-cleanup-1-0-1/README.md`): a unit's peak and instruction count add over the forms it instantiates, within 1.3%, because GCC at `-O0` holds every form's code until the unit ends; a form of two or three generators adds 27 to 100 MiB, one of four 145 to 270 MiB (the Gram forms up to 668 MiB), one of five 570 to 765 MiB, and a dense form of five about 800 MiB.
2. **Clang at `-O2` with the forced inlining compiles operation-heavy units slowly, restated.** The project's own clang-measure tree takes 124.3 s at R (build 59.3 s) against 667.4 s at 1.0.0; clang-measure-o3 took 127.6 s (build 63.5 s) at the cost-cleanup matrix `05229a7` against 666.6 s, and its build at R costs the same instructions, 6,405.1 G against 6,407.7 G (departure 4). The two builds' instructions fell by 39.5% and 36.6% from 1.0.0's, because `identities_rounding_test.cpp` (602.5 s, 4,272.5 G at 1.0.0) is now a driver and five law sources, 530.0 G in all on clang-measure at R, the largest `identities_rounding_antiproduct_test.cpp` at 105.1 G. A user's unit is not split: plan 0043's probe found that the cost within one function body grows faster than the code (two associativity laws together 1,691.9 G against 301.2 G and 307.7 G one at a time; eight De Morgan laws together 2,450.6 G against 95.1 G and 20.8 G each), inferred to come from the laws being instantiated and optimized in one body with the kernels forced inline, and not traced. `GALOOT_FORCE_INLINE` 0 is the user's remedy; a default that follows the optimization level is considered for 1.1.0 with plan 0047's measurement (decision 0252 item 3). clang-measure's build is now about 60 s of every P9 merge into `main`, against about 600 s at 1.0.0.
4. **Closed.** The four helper macros are `#undef`ed straight after the check that uses them (`core.public_macros` fails if one comes back). `GALOOT_ALWAYS_INLINE` stays defined and stays outside the interface.
5. **Closed.** An empty `GALOOT_FORCE_INLINE` gives one error, the literal "GALOOT_FORCE_INLINE must be 0 or 1" (`core.force_inline_empty` asks for one). A value that does not paste into one token, such as `-1`, still gets one error, the preprocessor's own. The guide states which spellings are accepted. Exception 13 is the remaining case.
9. **Closed by R.** The README's `FetchContent` block names `https://github.com/IA-Workshop/galoot.git`, `GIT_TAG "1.0.1"` and `GIT_SHALLOW TRUE`, and the placeholder text is gone. The blind review of R5 ran the block with `1.0.0` in place of `1.0.1` against the remote and it cloned one commit, configured, built and ran on both compilers; that `1.0.1` resolves can be checked only after the owner's push (plan 0045 R12). The documentation tests stay offline by design.
10. **Closed.** `l` and `m` are renamed, with three more names the check then reported (the local and the parameter `m` of `metric.hpp`, the parameter `l` of rga3's `rotation`). The lint unit `core_user_names` declares `I` and `rn` beside the headers and fails the lint build if a header name is confusable with them. `q1` (`transform.hpp:654`) stays, against a user's `ql` or `qI`, and the parameter `All` of `transform.hpp:320` against names with digits.
11. **Closed.** Configuring Galoot's own build with `GALOOT_FORCE_INLINE` other than `ON` or `OFF` stops with "GALOOT_FORCE_INLINE must be ON or OFF, not '<value>'.", as `GALOOT_CHECKING` does; each build preset states its mode; `tools.configure_rejects_force_inline` and `tools.configure_rejects_checking` pin both messages. Values that 1.0.0 read as ON (`TRUE`, `YES`, `1`, `2`, any word) are rejected. A consumer's `add_subdirectory` or `FetchContent` never reaches the option.
12. **Narrowed** (decision 0252 item 8, decision 0257). The guide says that `==` and `!=` on two basis changes do not compile and that the compiler shows, in a note, the header line ending in `// use same_rows(a, b)`; `core.basis_change_equality` and `core.basis_change_inequality` pin the hint on both compilers. The hint still appears only on that note line, because `= delete("...")` needs C++26 and STYLE S2 stands. No documentation test pins it.
13. **Closed.** The README's copy route says to copy `include/galoot`, so a copy no longer brings the vendored test framework.

The project's own cost, not seen by a user, for the performance history's notes: the test splits raised total build instructions on nine of eleven presets against 1.0.0, by +0.7% (gcc-measure-o3) to +10.5% (gcc-asan, 10,618.0 G to 11,727.7 G at R), and lowered them on the two clang-measure presets by 39.5% and 36.6%; over the matrix the net is −4.2% (98,056 G to 93,947 G). The largest process of every tree fell. The sanitizer rise reproduces from a second empty tree to within 0.1% (gcc-asan 11,722.3 G at `05229a7`, 11,727.7 G at R; clang-asan 7,255.2 G and 7,260.9 G), so it is a stable cost of the split sources; its cause is not traced (no time trace was taken). The growth of the build rows outside their compile rows that plan 0044's gate saw at `41d8029` (+25 G on gcc-asan and clang-measure alike, by the matrix record's method) did not repeat at R (49.9 G and 37.9 G, within 0.4 G of `05229a7`); R6's probe puts the compile launcher at about 290 M instructions a call, about 41 G over 141 compiles, so the launcher holds most of that part; that the one-time rise came from it is inferred, not traced.

## Accepted limits of the project's tools

Not seen by a user of the headers. 1.0.0's list, updated from plan 0043 section 14 and plan 0044 section 15:

- Job pool: plan 0027's I2 and I5 and plan 0028's I5 and I7 stay known limits (decision 0248 item 8, decision 0252 item 5 (c)); **I3 and I6 are closed** by plan 0043's U5 (`080c3c2`). An unreadable `/proc/<pid>/fd` hides a client (decision 0228 item 9). The cost of `tools.jobpool` is kept: it bounds every suite at about 64 s (63.0 s on gcc-debug at R).
- `tools/measure`: the dirty mark is read before and after each command, so a row is `-dirty` if the tree was dirty at either end or HEAD moved, and carries the start revision (plan 0043 U3, `56a3d21`); this **closes decision 0241 item 9**. The extra `git status` costs about 12 ms a row.
- `tools/mutate`: CTest runs the selected tests with `--parallel` on the shared pool (U4, `028fa4c`); the saving is unmeasured, since the before and after rows were taken on a saturated pool. Its `+dirty` mark follows a different rule from `tools/measure`'s (it excludes only its own output directory and the run log; tools review N4). GCC reports "unjudged" where Clang says "killed" for a guard reached through a `consteval` call or a `constinit` variable of a header (decisions 0231 and 0232). A mutant that only a lint build or an `#error` catches is classed unjudged; this waits for the first unit after 1.0.1 that changes the tool (decision 0260 section 2 item 4).
- The two comment-only findings plan 0043 left open are **fixed before R**: the comment of `test/core/transform_identities_up_to_three_test.cpp` (M1, `fadf768`) and the docstring paragraph of `tools/mutate` (F1, `cf59617`), both checked by the R5 blind review.
- The `convention-trace` row pair of `runs.tsv` lines 21709 and 21710 (the opt-out review's m6) is recorded as unexplained, and the CTest label anomaly of the 1.0.0 backlog is closed with no record and no reproduction (decision 0252 item 5 (a), (b); `records/verification/0043-ctest-label-survey/`).
- A cherry-pick of a review record's run-log rows brings the reviewed branch's rows along when the review branch's base holds rows the target lacks (plan 0044 section 15, departure 7 and open item 6); the 139 rows it brought are counted there and history was not rewritten.
- Plan 0011's skipped delta reviews and `records/spikes/spike-cdf/run_memo.py` stand as at 1.0.0.

## Records changed for the release

- **The diagnostics catalogue was regenerated** (decision 0261 item 1): `records/diagnostics/catalogue/scan-output.md` at `12ab75b`, its header and `misuse-catalogue.md`'s result line brought in step (183 messages, 161 pinned, 22 internal), and the wording that the message predates 1.0.1 at `8acd4ab`. **1.0.0's catalogue lacked one message**, "GALOOT_FORCE_INLINE must be 0 or 1", which has been in the headers since 2026-11-11 and is in 1.0.0; the record, written at `ebe7336`, had not been regenerated since.

## Corrections to the records of this release

- **The regenerated catalogue names the wrong Clang version.** `scan-output.md`'s header says its listings come from "clang-debug (clang++ 23.1.4)" trees; they were configured at about 22:49 UTC on 2026-11-21 (`runs.tsv` rows at `550e6ea` and `12ab75b`), after Clang 23.1.5 was installed at 09:29:29Z, so the compiler was 23.1.5 (inferred from the times; the trees no longer exist). The listing of test names does not depend on the version. Not corrected here: the file is outside this unit's files.
- **Two records cite line numbers of a feature worktree's log**: the cost-cleanup matrix record's rows 22388 to 22420 are lines 22545 to 22577 of this branch's log (plan 0043 section 14, departure 1). This record cites selections and this branch's lines.
- **The header-hygiene gate's absolute figures outside the compile rows** (263.1 to 303.5 G and 102.0 to 127.1 G) cannot be reproduced from its committed per-tree copies, and its record keeps no script (matrix record, departure 10). Its rises are what this record uses, by R6's method.

## Baselines and the dates of what this record cites

Decision 0246 item 2 asks the release record to state the date of every baseline it cites. All figures of the 1.0.1 work ran under the toolchain state of 2026-11-21T09:30:08Z.

| What | Revision | Date | Record |
| --- | --- | --- | --- |
| Performance history era | `ebdd06d09289` | rows 2026-10-31T20:40:59Z to 2026-11-01T23:52:19Z | `history/eras.tsv` |
| Toolchain state in force | none (machine state) | since 2026-11-21T09:30:08Z | `history/toolchain.tsv`, row 4 (`9ad75e9`) |
| 1.0.0 release matrix | `7276f82` (code of `7580e88`) | 2026-11-12, 02:28:25 to 03:16:56 UTC | `runs.tsv` lines 21905 to 21937; `records/gates/2026-11-12-release-matrix.md` |
| Plan 0043's probe (per-form cost, laws in one body) | as its README names | 2026-11-21 | `records/performance/cost-cleanup-1-0-1/` |
| Cost-cleanup merge matrix | `05229a7` | 2026-11-21, 21:38:15 to 22:03:18 UTC (step starts) | `runs.tsv` lines 22545 to 22577; `records/gates/2026-11-21-cost-cleanup-1-0-1-matrix.md` |
| Header-hygiene merge gate (five presets) | `41d8029` | 2026-11-21, 22:18:50 to 22:30:49 UTC (step starts) | `runs.tsv` lines 22589 to 22603; `records/gates/2026-11-21-header-hygiene-1-0-1-gate.md` |
| Header-hygiene W10, `product_test.cpp` on GCC debug, pinned | `c95fee5` against `41d8029` | 2026-11-21 | `records/gates/2026-11-21-header-hygiene-1-0-1-gate/w10/` |
| Release matrix | `f6e00fb` (code of R `578aa15`) | 2026-11-21, 23:12:32 to 23:39:01 UTC (step starts) | `runs.tsv` lines 22710 to 22742; `records/gates/2026-11-21-release-1.0.1-matrix.md` |
| DoD-22 copy at R | `f6e00fb` (code of R) | 2026-11-21, 23:12 to 23:40 UTC, shared | `compile.tsv` lines 5617 to 6584 |
| Release seed campaign | `f6e00fb` (code of R) | 2026-11-21T23:43:29Z to 2026-11-22T00:02:52Z | `records/gates/2026-11-21-release-1.0.1-seeds.tsv` |
| Benchmarks once at R | `f6e00fb` (code of R) | 2026-11-22, lock held 00:02:59 to 00:03:00 UTC | `runs.tsv` lines 22751, 22752 |
| Launcher probe | `f6e00fb` | 2026-11-22T00:03:14Z, under the lock | `records/gates/2026-11-21-release-1.0.1-matrix/out/probe-launcher.txt` |

The baselines of the 1.0.0 record (E1, E2, E3, fault injection, the core pass's W10 records, gate 3, M5) stand with their dates; 1.0.1 reruns none of them, and decision 0246 item 2's deferred items (WU17 parts G and H, the E1 and E2 reruns) stay deferred.

## DoD-22: compile records (decision 0252 item 19)

- Copied by unit R6 from the matrix's gcc-debug and clang-debug trees, by decision 0080's function, in shared mode: **968 rows, `compile.tsv` lines 5617 to 6584**, gcc-debug 483 (149 after the build, 334 after the suite) and clang-debug 485 (149 and 336). Checked by this unit: every row is at `f6e00fb962bc` on `feature/release-1.0.1`, none dirty; no preset, source and object triple occurs twice; 662 rows have a nonzero exit (331 per tree), the compile-fail tests', as at 1.0.0. The largest units are gcc-debug `identities_five_diagonal_test.cpp` 3,145,604 KiB in 75.6 s and clang-debug the same unit 1,053,688 KiB in 17.9 s. R6 reports that `tools/measure read` accepts the record.
- P14: the rows' wall times are not comparable with pinned rows; peak memory and retired instructions are.
- W10 for plan 0044's core change is in its gate record (above); plan 0043 changed no core header.

## The performance history (P7, P9, P13)

- **Toolchain:** marked (above, `9ad75e9`); `tools/runlog check` passed after it on this branch (`gate:runlog-toolchain@6d09c0e`, exit 0, "1 eras match the history; 1727 kept rows present once; no removed row present").
- **The compaction (decision 0252 item 14): [compaction] not made.** The state before it, at this branch's tip `9ad75e9`: `runs.tsv` holds 22,754 rows under its header (22,755 lines), one era (`ebdd06d09289`), and `tools/runlog check` reports "1 eras match the history; 1727 kept rows present once; no removed row present". The command, when the open-branch question is settled: `tools/timed check - gate:runlog-compact@<tip> -- tools/runlog compact <tip>`, then `tools/runlog check`, the log and the two history files committed alone with the counts `compact` prints, and a trial merge with each open branch in a throwaway worktree followed by `tools/runlog check` (`history/README.md`, "Compacting a log", step 4).
- Until it is made, P9's release row is met for the suites and the benchmarks, and for the history only in the toolchain marking.

## Evidence (V10)

- Verification: the full suite in all eleven presets at R passed (above), with the convention trace gate, `conventions_check.py`, the seed campaign, the plan cost model checks and `jobserver_check.py`; revision, commands and seeds are in the matrix record. Validation: unchanged from 1.0.0. 1.0.1 changes no computed value; the rows of `v10-table.md` stand, with the matrix at R as their newest run. The benchmark rows support no comparison (P6).
- Every check ran on the released code: the matrix on `f6e00fb`, whose code is R's, and nothing outside `docs/agent/` changed after R (rule 1 of "The tip T").
- **The closing gates of this unit, on its tree before the compaction:** `tools/runlog check`, the trace gate, and the citation check over the code paths and over `docs/guide`; their rows are in the commit after this record's, at its revision. **[compaction]** They rerun after the compaction.
- The release audit (plan 0045 R8): **[R8]**. Owner walkthrough (decision 0252 item 18, light): **[owner]** `________`. Open blocking defects: `________`.
- Version and tag: the owner.

## Open

Each with options, tradeoffs and a recommendation. None blocks the release except item 1, which blocks only the history update.

1. **The compaction's open branches** (departure 1). Options: (a) the coordinator extends decision 0261 item 3's exception to `review/docs-tooling` and `review/release-1.0.1`, whose rows are, like the exception's, new rows that drop or edit no row of their bases, and a short continuation of this unit compacts, checks, makes the trial merges and reruns the closing gates; (b) first cherry-pick the two R5 review commits (`6745da5`, `9f8a70f`) into this branch with `-x`, which W7 asks for a review record anyway and which this record cites, so that only `review/docs-tooling` needs the extended exception; (c) wait until both review branches are cherry-picked where they belong, then compact; (d) decline the compaction for 1.0.1 and declare the departure from P9's history update, as 1.0.0 did. Tradeoff: (a) is quickest, and the tool's anchors and `prune` are made for open branches, but it widens a ruling the coordinator worded narrowly; (b) adds a records commit to this branch that the release needs, and leaves one branch in the exception's own family; (c) may wait for another workstream; (d) costs nothing now and is the second release in a row to miss P9's history update. Recommendation: (b), then compact.
2. **Plan 0044's open item 4, the growth outside the compile rows.** R6 recommends closing it as a single sample not reproduced at R (its open item 1, option (a)); the alternative, repeating the gcc-asan build at `41d8029` (about 5 minutes), would name the cause of one run that changed no compile row. Recommendation: close it.
3. **The sanitizer rise.** Options: (a) leave it recorded as found; (b) a `-ftime-report` of one law source and the old unit under gcc-asan, about 10 minutes, in the next cost pass. Tradeoff: (b) says where about 1,100 G goes and changes nothing in 1.0.1, whose sanitizer trees build faster and with lower peaks than 1.0.0's. Recommendation: (a) for this release, (b) in the next cost pass.
4. **The catalogue's Clang version** (corrections, first item). Options: (a) a one-line records correction of `scan-output.md`'s header, dated, in a records commit before T; (b) leave this correction as the record. Tradeoff: (a) costs a minute and makes the record say what ran; (b) costs nothing, and the version does not bear on the listing. Recommendation: (a).
5. **`CHANGELOG.md` is not in `tests.citations`' paths.** The test reads the README, `include/galoot`, `cmake`, `test`, `tools` and the build files; the changelog, a user page by decision 0252 item 15, is checked by no test. This unit's closing gate reads it by hand (see "Evidence"), and the R5 blind review searched it. Options: (a) add it to the test's paths in the first unit after 1.0.1 that changes `test/CMakeLists.txt` (a change now voids R); (b) leave it to each release commit's review. Recommendation: (a).
6. Still the owner's: the walkthrough, the close-out script, the merge into `master`, the tag, and the push of `main`, `master` and `1.0.1` (decision 0252 items 12 and 18; decision 0257 item 2).
