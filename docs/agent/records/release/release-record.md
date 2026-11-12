# Release record: galoot core 1.0.0

- Status: filled 2026-11-12 by Claude (agent, Opus 5.5), planner, plan 0042 unit U9, on branch `feature/release-1.0.0`, from the release matrix at the release commit R (`records/gates/2026-11-12-release-matrix.md`, unit U8). Records only. Cells marked **[U10]**, **[owner]** or **[coordinator]** are filled by the release audit, the owner's walkthrough, or the coordinator at U11. The owner merges to `master` and tags (W13); nothing here is a release until then.
- History: first written 2026-11-10 by Claude (agent, Sonnet 5.5) at `develop` `1ed0e6a` as `release-record-draft.md`; rewritten 2026-11-11 by Claude (agent, Opus 5.5), planner, at `develop` `a73cd4e` (unit U6), from decisions 0141 to 0248, plans 0038, 0040 and 0042, and the M7 gate audit; filled and renamed by U9, which also read decision 0249 and the blind review of the release commit (`records/reviews/2026-11-12-release-commit-blind.md`); completed 2026-11-12 by Claude (agent, Opus 5.5), U11, from the release audit (`records/gates/2026-11-12-release-audit.md`) and the owner's rulings on it (decision 0250): departures 3, 8, 9 and 10, rule 4, exception 12, known friction 10 to 13, and the cells of U10 and the coordinator.

## Departures, unknowns, and readings, first (C20)

1. **The runs name `7276f82`, not R.** U8 ran at the branch tip `7276f82`, which adds one run row to R `7580e88`, so every run row and copied compile row says `7276f826533f`. `git diff --stat 7580e88 7276f82 -- . ':!docs/agent'` prints nothing: the code, tests, tools and build files that ran are R's.
2. **One change after R is outside `docs/agent/`: the README's rga3 sentence** (`1cd7ac8`, decision 0249 item 2). The plan's rule for T, "`git diff --stat R T -- . ':!docs/agent'` is empty" (plan 0042 section 1 item 4), cannot hold once decision 0249 puts that edit in T. The rule this record uses instead is under "The tip T". The README is read by its example tests, `tests.docs_pages` and `tests.citations`; they passed after the edit, in gcc-debug and clang-debug. The edit changes prose only and keeps the README's line count; the rebuild that followed found no work, so no extracted example changed (inferred from Ninja's "no work to do").
3. **Those README checks first ran outside `tools/timed`.** The job pool had stopped itself (after 30 minutes without a client), `tools/timed` refuses a build or test step without it, and the unit's brief allowed no pool change. The rebuild (no work: "ninja: no work to do") and the 11 tests at `1cd7ac8` ran directly, so those runs have no row in `runs.tsv` (P13 departure). The same trees' documentation tests were then rerun through `tools/timed` at `8118033`, whose README is `1cd7ac8`'s: rows 21958 (gcc-debug) and 21959 (clang-debug), selection `gate:docs@8118033`, both exit 0, committed by `18eb677`. That commit's message gives no command and no reason, and the row does not record the command, so whether the selection was the same `-R "readme|docs_pages|citations"` is not known. The release audit reran that selection at `18eb677` (rows 21969 to 21971, 11 of 11 on each compiler). So the README after the rga3 sentence has timed rows; the first, direct runs keep none.
4. **No benchmark figure of the release's own code is claimed.** The benchmarks ran once at R as a check that they run and that both forms agree, not as a measurement (P6). E2's figures are on code before the forced inlining.
5. **DoD-22 at R is shared mode**, by the owner's ruling (decision 0248 item 5). Pinned serial builds (decision 0233 item 5) stay deferred (decision 0246 item 2).
6. **The repository URL is a placeholder** in the released README (decision 0249 item 1), against decision 0245 item 3, which expected it in the release commit. Listed under "Known friction", item 9.
7. **No failure was found** in the matrix, the checks, the campaign or the benchmarks; nothing below is a failure written as a pass.
8. **What the tag names.** Decision 0248 item 9 (O4) says the tag names the records tip T. Plan 0042 section 8's commands tag `master` after `git merge --no-ff develop`, a merge commit M whose tree equals `develop`'s (`git diff develop master --stat` prints nothing). The owner ruled for M (decision 0250 item 4, the audit's O-D (a)): "the tag names T" is read as "names a commit whose tree is T's", checked by `git diff develop master`. `develop` at the merge is T plus records-only commits (the coordinator's records after U11, and the owner's CONVENTIONS 1.18 commit with its gate rows), which plan 0042 section 8 checks before the merge.
9. **P9's release row: no update of the performance history.** P9 asks at release for "the full suite in every supported configuration, the benchmarks, and an update of the performance history (W13)". The history (`records/performance/history/`) was last updated at `973fa35`, the M3 acceptance; its one era, `ebdd06d09289`, holds the rows of 2026-10-31T20:40:59Z to 2026-11-01T23:52:19Z. `tools/runlog` updates it only by `compact`, which needs the owner's approval (P13). By the owner's ruling (decision 0250 item 2, the audit's O-B (a)) no compaction is made at release. Nothing is lost: `runs.tsv` keeps every row, `tools/runlog check` passes, and every performance result is track-only, so no result is compared with the history.
10. **Six merge commits of the window do not identify agent work** (the audit's m6; W7): `6d7d7b4`, `4483b6d` and `6c278df` (merges of `feature/documentation-lane--pages` into its parent), `db81e97` (a merge of `develop` into that branch), `f580f2a` (a merge of `feature/pre-release-core-pass` into `feature/pre-release-core-pass--comments`) and `7284061` (the merge of `records/rescued-run-rows`, 96 rescued run rows). Each carries the owner's git identity and no agent trailer. The commits they merge do identify their agents. History is not rewritten (W7), so they are recorded here.

## Release

- Component: galoot core, version 1.0.0 (plan 0001 D17).
- Release commit R: `7580e88` (2026-11-12T02:23:01Z, "Release 1.0.0: project version, README status and FetchContent tag"; blind review: approve, no blocker or major, `records/reviews/2026-11-12-release-commit-blind.md`).
- Tag: `1.0.0` (decision 0248 item 9, O6), on the `--no-ff` merge commit M of `develop` into `master`, whose tree is `develop`'s at the merge (decision 0250 item 4). The records tip T that U11 hands back is the commit that adds, to U11's last records commit, only the run rows of its closing gates (rule 3); its hash is in plan 0042 section 11 once the coordinator fills it. The tag's commit is the owner's.
- CONVENTIONS version implemented: 1.17 at R, at the U9 tip and at T. The owner ruled 1.18 (decision 0248 item 2, the errata-naming row; decision 0250 item 5, line 12's sentence in the same edit), and its text is ready in `conventions-1.18-owner-text.md`; the edit is pending, since only the owner edits CONVENTIONS. The tag message names 1.18 (plan 0042 section 8).
- Preset versions: `rga3` 0.1.0 for the record (decision 0248 item 9, O6); no file carries a version and none is added for 1.0 (decision 0249 item 2). The README says that the preset ships in the same tree, is below 1.0, and that its interface may still change. cga3: not in this release (decision 0211).
- CMake project version: `project(galoot VERSION 1.0.0 ...)` at `CMakeLists.txt:6` (R).
- No `install()` rule and no package for `find_package` in 1.0 (decision 0240 item 5); the README names `add_subdirectory` and `FetchContent`.
- The study's byline: no author was named, so the default stands (decision 0241 item 3).

## The tip T

T is the commit the owner merges (`develop`, fast-forwarded to it at U11). A commit qualifies as T when all of these hold:

1. `git diff --stat 1cd7ac8 T -- . ':!docs/agent'` prints nothing, and `git diff --stat 7580e88 1cd7ac8 -- . ':!docs/agent'` lists only `README.md` (2 lines changed, the rga3 sentence). Together: from R to T, nothing outside `docs/agent/` changed except that sentence.
2. The README's tests passed on a tree whose README is T's: true from `1cd7ac8` on while rule 1 holds (below, "Evidence").
3. `tools/runlog check` and the trace gate passed on T's tree, or on its parent's tree with T adding only their run rows to `runs.tsv` (as `ab6836a` did for `4e7d4aa`).
4. When the owner adds CONVENTIONS 1.18 before the tag, the trace gate reruns on that commit, since it is the only check that reads CONVENTIONS.md (`conventions_check.py` and the CTests read no copy of it, by grep), and `tools/runlog check` reruns because the trace gate's row changes `runs.tsv`. `conventions_check.py` does not rerun. The tag message names 1.18. The commands are in `conventions-1.18-owner-text.md`, section 5.
5. A change under `include/`, `test/`, `tools/`, `cmake/`, `docs/guide/` or the build files after R voids T and reruns the matrix whole (plan 0042 section 6 item 6). A further README change voids rule 2 until its tests rerun on both compilers.

## Class covered (DoD-1)

- Metrics: diagonal R(p, q, r) with null generators last (`R<P, Q, Z>()`, `rigid<D>()`), the conformal metric (`conformal<D>()`), and any symmetric integer form (`from_gram`), each with entries within `max_form_entry`. An exact change of basis (`basis_change`, `conformal_change`) maps between two such algebras with rational rows.
- Size: 1 to 16 generators (`max_generators`); a factory's `MaxN` raises the bound to at most 32 (`mask_bits`), untested above 16. A count of 0 or above the bound is rejected at compile time with a message (limits.md, "Number of generators").
- Names: blade names, `get<"...">`, `blades<...>` and printing work up to 9 generators (`max_compact_generators`); above that a layout is written as masks.
- Graded signatures have no spelling, and are rejected by `metric_like` (decision 0205).
- Stated from the guide's limits page and decision 0205, not from a new test.

## Platform and toolchain

- Tested at R: g++ 16.2.1 20260810 and clang++ 23.1.4, each with libstdc++ 16, CMake 4.4.5, Ninja 1.13.2, glibc 2.44, on x86-64 Linux, C++23 without compiler extensions (limits.md, "Toolchains"); the same as gate 3. MSVC, other standard libraries and other platforms are not tested and not claimed.
- The core throws no exceptions; the core, the preset and `format.hpp` compile with `-fno-exceptions` (`core.bare_compile_no_exceptions`, `core.bare_compile_format_no_exceptions`, `rga3.bare_compile_no_exceptions`; passed at R). The test runner uses exceptions. DoD-21 stands on these bare compiles (decision 0240 item 4).
- The rounding check's bound assumes one rounding per operation to the scalar's own precision, to nearest, with gradual underflow and IEEE 754's NaN and infinity. A build that flushes subnormals to zero is outside it; a checking build under `-ffast-math` or `-ffinite-math-only` does not compile. The bound holds with or without multiply-add contraction (limits.md, "The rounding check"; decision 0243). Every preset builds with `-ffp-contract=off`.

## The release matrix at R

All eleven presets, each from a new empty tree, one at a time on the shared pool (23 of 23 slots free before and after every step), as three steps through `tools/timed`, selection `gate:all@7580e88`, `runs.tsv` rows 21905 to 21937, 2026-11-12 from 02:28:25 to 03:16:56 UTC. A memory watchdog (stop below 15 GiB available or above 20 GiB in one process) never fired; the least available was 92.4 GiB of 125 GiB.

| Preset | Forced inlining | Tests | Wall (s) | Largest compile |
| --- | --- | ---: | ---: | --- |
| gcc-debug | on | 657 of 657 | 159.5 | `transform_identities_test.cpp`, 5.93 GiB, 92.3 s |
| clang-debug | on | 657 of 657 | 100.0 | `transform_identities_test.cpp`, 1.51 GiB, 32.8 s |
| gcc-release | on | 657 of 657 | 157.1 | `transform_identities_test.cpp`, 1.96 GiB, 87.7 s |
| gcc-asan | off | 649 of 649 | 228.8 | `transform_identities_test.cpp`, 4.73 GiB, 151.3 s |
| clang-release | off | 657 of 657 | 113.7 | `transform_identities_test.cpp`, 1.03 GiB, 45.7 s |
| clang-asan | off | 657 of 657 | 204.5 | `transform_identities_test.cpp`, 2.68 GiB, 132.0 s |
| lint | on | 657 of 657 | 264.9 | clang-tidy, `all_verify_interface_header_sets`, `format-check`; no diagnostic |
| gcc-measure | on | 649 of 649 | 153.5 | `transform_identities_test.cpp`, 1.85 GiB, 84.1 s |
| gcc-measure-o3 | on | 649 of 649 | 167.7 | `transform_identities_test.cpp`, 2.03 GiB, 97.9 s |
| clang-measure | on | 649 of 649 | 667.4 | `identities_rounding_test.cpp`, 602.5 s, 1.81 GiB |
| clang-measure-o3 | on | 649 of 649 | 666.6 | `identities_rounding_test.cpp`, 601.5 s, 1.80 GiB |

2,883.7 s in all. Each suite takes about 63 s, bounded by `tools.jobpool` (61.9 s). The two `-O3` presets ran in a gate for the first time (decision 0248 item 9, O5). The test counts are under each test preset's filter, which leaves out the labels `runlog` and `bench`, and equal gate 3's.

## Known exceptions

Each is a departure the owner has accepted or deferred. None is a blocking defect unless the owner says so.

1. **The antiproduct's growth at fixed use (decision 0141, C6).** The geometric antiproduct and the antiwedge are built through the complements in all n generators, so their compile cost grows with n at fixed use. WU17's grid (`records/performance/wu17-growth/README.md`, at `18849b5`, measured 2026-11-10, before the forced inlining) entered the antiproduct's figures against 0141, as did `compose` and `transform`, which are built on it. Accepted for twelve months, to 2027-11-01; it ends at the first of: a production report of a build-time problem; a model, named type or use needing more than six generators while using few; 2027-11-01. The remedy of decision 0140 stays proposed. Listed by 0141 item 5.
   - With it, by decision 0163 item 3: the antiproducts are verified but not validated against a published non-diagonal table; the gap stands until such a table is found.
   - With it, by decision 0184 item 3: on Clang without libstdc++'s assertions (that is, in an optimized build), the geometric antiproduct of two `full<5>` multivectors of conformal(3) exceeds Clang's default constant-evaluation steps and does not build; `-fconstexpr-steps=1200000` builds it (limits.md, "Compile-time limits: products and plans").
2. **`bulk_dual`'s growth at fixed use (decision 0247, beside 0141).** From n = 4 to 16 at fixed use, +8.2 M instructions (+1.8%) on GCC and +34.4 M (+7.3%) on Clang, beyond the larger range of 0.5 M and 3.0 M (`wu17-growth/README.md`, at `18849b5`, 2026-11-10, before the forced inlining; the inlining does not change the plan generation the growth comes from, inferred). The cause given, the product with the antiscalar of grade n, is inferred from the pattern. Same end conditions and review date as exception 1 (0247 items 1 and 2); listed by 0247 item 5. The weight duals, the antidot and the projections were not measured.
3. **D8: no collected plans in 1.0 (decision 0239 item 1).** Operation counts of composed plans can exceed collected-term counts where terms cancel. DoD-24's counts are form A's, the kernels as specified (decision 0233 item 2). DoD-4 stands on the declared-output check: `transform` checks that what it drops is zero (exactly over an exact scalar, within the forward bound over a rounding one). The tests, all passed at R in every preset: `core.transform_nonzero_int64` and `core.transform_nonzero_rational`, `core.transform_consteval_nonzero` and `core.transform_consteval_nonzero_rational`, `core.transform_checking_recorded`, the `core.transform_unchecked_*` twins, and `core.transform_rounding`. The first candidate after 1.0 is a named batch-under-one-motor interface.
4. **DoD-24: what is and is not claimed about calls and speed.**
   - Claimed, at R: with `GALOOT_FORCE_INLINE` 1 (the default), at `-O2` without the checking mode, on x86-64, the 52 probe functions of the rga3 preset's operations compile with no call or jump into namespace `galoot`, on both compilers, in one unit and each alone (`rga3.kernels_inline`, `rga3.kernels_inline_alone`; passed in every preset of the matrix, the three trees with the opt-out included, since the tests build the forced code themselves).
   - Not claimed: other layouts and algebras (some operations on conformal(3) multivectors of all or of even blades still compile to a call), `-Os` and other levels, `GALOOT_FORCE_INLINE` 0, and checking builds, where `transform` calls its checks out of line by design (decision 0244 item 1, "What is not claimed"; `assert.hpp`'s comment).
   - No blade branch and no allocation: read in E1 at `18849b5` (2026-11-10), before the forced inlining, on both compilers (`wu17-asm/README.md`, "Findings"); not re-read since. The structure test checks calls and jumps, not branches on blade data or allocation, so at R this half is inferred.
   - The pinned benchmark against the in-repo straight-line form (E2, `wu17-bench/README.md`, at `2de0d36`, 2026-11-10, CPU 4, under the lock, seed 20261110) measured the code before the forced inlining: 10 of 22 loops slower and 6 faster under the strict reading. **The release claims no benchmark of its own code** (plan 0042 section 3.4); the rerun is deferred past 1.0 (decision 0246 item 2). At R the benchmarks ran once ("Benchmarks at R" below), which shows that they run and that both forms compute the same results, and measures nothing.
5. **The strict scalar rule (decision 0240 item 7).** Scalars must match exactly: an integer literal is not converted where the scalar type is not an integer, in the linear operations and the preset's motions. Kept for 1.0 and taught on the guide's first page ("2. Scalars must match exactly").
6. **`%` and `!` (decision 0240 item 7).** `%` is the geometric antiproduct and `!x` the right complement, not a boolean. Taught on the guide's first page ("3. Two operators to learn: `%` and `!`").
7. **Compile-time limits** of each operation are recorded on the limits page, not in the headers (decision 0240 item 9): `docs/guide/limits.md`.
8. **The one-blade catch-all is deferred past 1.0 (decision 0241 item 1).** A conversion a layout of one blade does not accept gets the compiler's own error, not a library message.
9. **Spurious overflows after the row reduction (decision 0236 item 2).** A change of basis can still be rejected as overflowing where its reduced inner products would fit. Accepted for 1.0; a backlog item unless a user meets it.
10. **The six helpers are public with fewer users than DoD-2 asks (decision 0248 item 9).** See "DoD-2" below.
11. **The `rga3` preset's `rotation`** builds its motor at run time only, since `std::sin` and `std::cos` are not constant expressions on every compiler (rga3.md); for a motor in a constant expression, write its coefficients.
12. **Other open items at release.** From the release audit (`records/gates/2026-11-12-release-audit.md`: 0 blocking, 3 major, 9 minor, 10 info), each ruled by decision 0250: MA1 closed by `dod-1-to-29.md` and `dod-3-operation-map.md`; MA2 declared as departure 9; MA3 closed by decision 0250's addendum; the tag's commit ruled (departure 8); the friction lines added (items 10 to 13 below). Open for the owner: the CONVENTIONS 1.18 edit (pending, above), the walkthrough, the merge and tag, the repository URL (decision 0249 item 1), and two gaps of the DoD table that no ruling covers (DoD-5's named test-only model; DoD-28's lessons). **[owner]** The walkthrough adds here what it finds.

## Known friction

Each is measured or recorded where stated; none is a defect.

1. **Debug builds' peak memory.** gcc-debug's largest compile, `transform_identities_test.cpp`, peaks at 5.93 GiB (6,214,900 KiB) in 92.3 s at R, the same as at gate 3 (`2438604`, 2026-11-11), against 2.49 GB at `ed9288f` before the forced inlining (plan 0040 outcome, gate 3; the two units are those the records give). A user's large debug unit can need several GiB.
2. **Clang at `-O2` with the forced inlining compiles operation-heavy units slowly.** The clang-measure tree takes 667.4 s at R (configure 0.6, build 603.0, suite 63.8), of which `identities_rounding_test.cpp` takes 602.5 s (1.81 GiB); gate 3 gave 664.0 s and 599.0 s, and `ed9288f`, before the forced inlining, 116.1 s. clang-measure-o3 takes the same, 666.6 s. This is about 667 s of every P9 merge into `develop` (decision 0244 item 17). `GALOOT_FORCE_INLINE` 0 is the user's remedy; gcc-asan, clang-asan and clang-release use it.
3. **Debug builds compile slower with the forced inlining.** GCC `product_test.cpp` +80.5% instructions, 2.18 s to 4.32 s, 418 to 830 MiB; `transform_test.cpp` +35.8%; Clang `product_test.cpp` +17.5% (`records/performance/pre-release-core-pass/cost-public-ops/`, at `a6f3196`, 2026-11-11; decision 0245 item 1, whose friction statement stands).
4. **Four helper macros stay defined in every user unit**: `GALOOT_DETAIL_PASTE_`, `GALOOT_DETAIL_PASTE`, `GALOOT_DETAIL_FORCE_INLINE_IS_0` and `GALOOT_DETAIL_FORCE_INLINE_IS_1` (`assert.hpp:44-47`), with `GALOOT_ALWAYS_INLINE`. Their `#undef` is on the post-1.0 backlog.
5. **An empty `GALOOT_FORCE_INLINE` gives two errors**: the literal "GALOOT_FORCE_INLINE must be 0 or 1" first, then the preprocessor's own from a later `#if` with no left operand (decision 0244 item 17; `core.force_inline_empty`). A value that does not paste into one token, such as `-1`, gets only the preprocessor's error.
6. **`-Os` builds still call into the library** (exception 4).
7. **`-ffp-contract=fast` can change the last bits** of a transform between a checking and a non-checking build (10% to 14% of coefficients measured) and between a constant evaluation and run time (43% to 49%); with `off` or `on` the compilers tested gave the same bits (decision 0243; limits.md, "The rounding check"). GCC contracts by default when the target has FMA.
8. **A forced-inline operation cannot be called from a function built for fewer instruction-set features** than its unit (for example a `[[gnu::target("arch=x86-64")]]` fallback in a `-march=x86-64-v3` file): both compilers stop with an error (`assert.hpp`'s comment on `GALOOT_ALWAYS_INLINE`).
9. **The README's `FetchContent` block names no real repository.** It holds the marked placeholder `GIT_REPOSITORY "<galoot-repository-url>"` with `GIT_TAG "1.0.0"`, and the text above it and the comment in it say to replace the URL. Copied as it stands, the block does not configure. There is no repository URL yet; the owner fills it when there is one (decision 0249 item 1). This departs from decision 0245 item 3, which expected the URL in the release commit. The documentation tests point `FetchContent` at the source tree offline, so they do not read the URL or the tag (`test/documentation.cmake`).
10. **Two header locals look like names a user may write**, under clang-tidy's `misc-confusable-identifiers`: `l` (`basis_change.hpp:542`) against a user's `I`, and `m` (`bits.hpp:171`) against a user's `rn`. A user who lints with Galoot's headers not marked as system headers gets the warning in their own unit (`records/reviews/2026-11-11-pre-release-core-pass-delta2-review/lint/scan-26f1b62.tsv`, rows 35 and 53). Renaming them is on the post-1.0 backlog (plan 0042 section 9).
11. **The CMake cache option `GALOOT_FORCE_INLINE` is not validated.** A configure value such as `2` or `foo` gives ON with no message (plan 0040 section 17, m2). The macro itself is checked strictly: a compile with `-DGALOOT_FORCE_INLINE=ON` stops with "GALOOT_FORCE_INLINE must be 0 or 1". Validating the cache option is on the post-1.0 backlog.
12. **The deleted `==` of `basis_change` names `same_rows` only in the source line the compiler echoes** (plan 0040 section 17, m5). The diagnostic says the function is deleted; the hint is the comment at the end of the declaration's line, which both compilers print with it. `= delete("...")` with a message needs C++26 (the post-1.0 backlog's "the `==` hint note").
13. **The README's "Copy the headers" route also copies doctest.** "Copy or vendor the `include/` directory" brings `include/ext/doctest/` (the test framework, its `.cpp` and its MIT license) into a user's project, beside the README's "Galoot is headers alone". Nothing breaks, and the core includes none of it. The README is not changed (decision 0250 item 7, the audit's m4 (a)), since a README change after R reopens rule 2.

## Baselines and the dates of what this record cites

Decision 0246 item 2 asks the release record to state the date of every baseline it cites.

| What | Revision | Date | Record |
| --- | --- | --- | --- |
| Performance history era | `ebdd06d09289` | rows 2026-10-31T20:40:59Z to 2026-11-01T23:52:19Z | `records/performance/history/eras.tsv` |
| E1, assembly read | `18849b5` | 2026-11-10 | `records/performance/wu17-asm/` |
| E3, growth grid | `18849b5` | 2026-11-10, lock held 17:48 to 17:52 UTC | `records/performance/wu17-growth/` |
| E2, benchmark (inline rerun) | `2de0d36` | 2026-11-10, lock held 18:34 to 18:58 UTC | `records/performance/wu17-bench/` |
| Fault injection (DoD-14) | `d5ffafd` | 2026-11-08 | `records/mutations/0029-identities/` |
| Core pass W10, parts A to E | `7a27f76` | 2026-11-11 | `pre-release-core-pass/cost/` |
| Core pass W10, public operations forced | `a6f3196` | 2026-11-11 | `pre-release-core-pass/cost-public-ops/` |
| Core pass W10, after D1 | `d2568e3` | 2026-11-11 (loaded machine: instructions comparable, wall times not) | `pre-release-core-pass/cost-d1/` |
| Opt-out, six identity units | `2d292ef` | 2026-11-11 | `pre-release-core-pass/opt-out/` |
| Gate 3 matrix (nine presets) | `2438604` | 2026-11-11 | `runs.tsv` rows 21815 to 21826; plan 0040 outcome |
| M5 seed campaign | `1aa1e40` | 2026-11-10, 02:55:48 to 03:11:23 UTC | `records/gates/2026-11-10-M5-seeds.tsv` |
| Newest `compile.tsv` rows before R | `c799821` | 2026-11-07T21:44:22Z, shared | `records/performance/compile.tsv` |
| Release matrix | `7276f82` (code of R `7580e88`) | 2026-11-12, 02:28:25 to 03:16:56 UTC | `runs.tsv` rows 21905 to 21937; `records/gates/2026-11-12-release-matrix.md` |
| DoD-22 at R | `7276f82` (code of R) | 2026-11-12, 02:28 to 03:16 UTC, shared | `compile.tsv` lines 4673 to 5616 |
| Release seed campaign | `7276f82` (code of R) | 2026-11-12, 03:20:21 to 03:39:22 UTC | `records/gates/2026-11-12-release-seeds.tsv` |
| Benchmarks once at R | `7276f82` (code of R) | 2026-11-12, lock held 03:39:44 to 03:39:45 UTC | `runs.tsv` rows 21947, 21948 |

E2's benchmark and E1's read predate the forced inlining (decision 0244), and no rerun is part of 1.0. The WU17 baseline directory that part H names (`wu17-baseline/`) was not made (decision 0246 item 2).

## DoD-22: compile records

- At the release revision: per-translation-unit wall time and peak memory on both compilers, from the matrix's gcc-debug and clang-debug trees, copied into `records/performance/compile.tsv` by decision 0080's step (the function of its second amendment), in **shared mode**: 944 rows, `compile.tsv` lines 4673 to 5616, all at `7276f826533f` on `feature/release-1.0.0`. gcc-debug 471 rows (137 copied after the build, 334 after the suite), clang-debug 473 (137 and 336). The rows after the suite are the compile-fail tests' compiles, which run while CTest runs; 662 rows have a nonzero exit, as those tests expect. Each copy passed the step's checks (one revision, not dirty, no compile twice), and no source and object pair is in both copies of a tree. `tools/measure read` accepts the record.
- Largest units: gcc-debug `transform_identities_test.cpp` 6,214,900 KiB (5.93 GiB), 92.3 s; clang-debug the same unit 1,585,484 KiB (1.51 GiB), 32.8 s.
- **P14 note.** These rows are in shared mode: the builds ran 23 jobs at once on the shared pool, unpinned, on processors 0 to 31, so each compile may have run on either kind of core with the other thread busy. Their wall times are not comparable with pinned rows (the 12 pinned rows of WU1 and its recorder follow-ups) and, strictly, not with one another; they are comparable with the earlier shared rows only as far as the machine's load allows. Peak memory and retired instructions do not depend on the kind of core, and those columns can be compared.
- Before and after for each core unit (W10): `records/performance/core-unit-w10-index.md`. **Accepted past gaps** (decision 0248 item 4): WU9 (plan 0018) has no W10 record and no `compile.tsv` row from its branch; plan 0009 has an after record with one before figure. No retroactive measurement. Cleanup 1 (plan 0007) changed comments only and owes none.

## DoD-2: the public interface

`dod-2-inventory.md`, re-grepped at `a73cd4e` (U6). `git diff --stat a73cd4e 7580e88 -- include/` prints nothing, so the inventory holds at R; the release commit changed only `CMakeLists.txt` and the README, neither an entry. Two departures from DoD-2's text stand, both by the owner's ruling:

1. **The list is the inventory, not plan 0001 section 3's text.** Decision 0248 item 9 (O1 (a)) amends section 3 to the real exports, which the inventory lists. Plan 0001's section 3 itself is not edited; its M7 addendum (`c41ed75`) points to the inventory.
2. **Entries with no present user are kept, not removed.** DoD-2 says "an entry with no user is removed". `sum_layout` and `basis_change_like` have no user outside their headers and tests and stay public by decision 0248 item 9. The layout names `compose_layout`, `change_basis_layout`, `change_basis_back_layout` and the ten dual layouts stay public by decision 0187 (a user names the result type of an operation), and `GALOOT_FORCE_INLINE`'s user is a build, not code.

## Other checks at R

All at `7276f82`, each through `tools/timed` with its row in `runs.tsv`, each exit 0:

| Check | Row | Result |
| --- | --- | --- |
| Convention trace gate | 21938 | items with no test: 6.1, 6.2, 6.3, 6.4 only (the cga3 items, decision 0211 item 2) |
| `conventions_check.py`, seed 20261031 | 21939 | "all convention checks passed (seed 20261031)" |
| `format-check` on the lint tree | 21940 | passed; also inside the lint build |
| `tools/runlog check` | 21949 | "1 eras match the history; 1727 kept rows present once; no removed row present" |

### `plan_cost_model` and `jobserver_check.py` (decision 0248 item 7)

| Check | Row | Result |
| --- | --- | --- |
| `comments_test.py` | 21941 | 16 tests, OK |
| `cost.py comments include/galoot/core/plan.hpp`, both figures files | 21942 | "0 counts in the comments are not in the figures". `plan.hpp` now holds no count, so this run checks nothing |
| `cost.py comments docs/guide/limits.md --section "Steps and operations of each plan"`, both figures files | 21943 | "0 counts ... are not in the figures": the run with content |
| `cost.py selfcheck`, Clang data | 21944 | 1228 measured cases predicted; 9 whole units whose largest evaluation is the metric's check, not compared; 0 differences |
| `cost.py selfcheck`, GCC data | 21945 | 1235 measured cases predicted; 9 not compared; 0 differences. Both equal the results at `6a3cf13` |
| `jobserver_check.py`, all five cases | 21946 | SIGKILL returns 0 of 3 tokens, SIGINT and SIGTERM 3 of 3; a pool replaced under a build lets 8 jobs run; a missing FIFO lets Ninja run 34 jobs on 32 processors: the results of `6a3cf13` and `b28cc34`, with Ninja 1.13.2, on a private pool |

The snippets index rows for `jobserver check` and `plan cost model`, and the header of `jobserver_check.py`, now name this run (`7bad7b2`). `probe.py verify` was not rerun; its last run is at `753a334`.

### The seed campaign (decision 0216 item 6)

The six randomized identity programs of M5, `--rand-seed=<k>` for k = 1 to 50, on the gcc-debug tree, one run at a time, each through `tools/timed --log` into `records/gates/2026-11-12-release-seeds.tsv` (selections `gate:seed:<k>:<program>@7580e88`), 2026-11-12 from 03:20:21 to 03:39:22 UTC. **300 of 300 runs exit 0**, so no failure became a fixed test or a defect report (V7, W9). It is a sample of 50 seeds per program, a hypothesis and not a theorem (V9).

`core_identities_test` took a median 21.8 s per run against M5's 16.2 s. The cause is not traced: the program has changed since `1aa1e40`, and both campaigns ran in shared mode, so the times are not comparable (P14). Other test sources also read doctest's seed (21 files); as at M5, only the six programs ran.

### Benchmarks at R

`taskset -c 4 tools/timed test P gate:bench@7580e88 -- ctest --test-dir build/P -L '^bench$' -V` on gcc-measure (row 21947) and clang-measure (row 21948), pinned to CPU 4 under the timing lock (`lock.sh`), once each. **22 of 22 tests passed on each compiler, and the checksums of the two forms agree in all 11 pairs.** One process per test, 524,288 operations, no repeats, seed 20261110: no spread is known, the protocol of `wu17-bench` (11 interleaved rounds, three sizes, a busy rule) did not run, and the sibling CPU's load was not read. So these figures support no comparison (P6), and **the release makes no benchmark claim of its own code** (plan 0042 section 3.4). The figures are in the matrix record.

## Corrections to the records of this release

- **`4e7d4aa`'s message miscounts the presets**: it says "657 of 657 on the seven 657-test presets". Six presets have 657 tests (gcc-debug, clang-debug, gcc-release, clang-release, clang-asan, lint) and five have 649 (gcc-asan and the four measure presets). `ab6836a`'s message corrects it, and the matrix record's table was right. Commits are not rewritten (W7), so the slip stays in the history.

## Interfaces removed or changed incompatibly (W13)

1.0 is the first release, so this list is informational: no prior release had these interfaces. Changes a reader of the history may meet:

- `basis_change`'s `==` is deleted, its comment and diagnostic naming `same_rows(a, b)` (decisions 0235 and 0236 item 3); rows are reduced by their gcd before the checks.
- The `rga3` model moved from the core to `galoot::presets::rga3` (decision 0213).
- `detail::inverse_change` deleted (decision 0236 item 1).
- The sixteen `*_layout` names, `compose_layout` and `antiproject_layout` are public interface from 1.0 (decision 0187).
- The six helpers `has_name`, `same_rows`, `basis_change_like`, `index_of`, `sum_layout` and `max_generators` stay public (decision 0248 item 9, superseding 0241 item 6's move to `detail`). Moving any of them later is a 2.0.0 change.
- `GALOOT_FORCE_INLINE` is new public configuration (decision 0244 item 16); `GALOOT_ASSERT` is public (decision 0241 item 6); `GALOOT_ALWAYS_INLINE` and the `GALOOT_DETAIL_*` macros are not interface.

## Accepted limits of the project's tools

Not seen by a user of the headers; listed because the release accepts them.

- Job pool: plan 0027's I2, I3 and I5 and plan 0028's I5 to I7 are known limits (decision 0248 item 8); an unreadable `/proc/<pid>/fd` hides a client (decision 0228 item 9); the cost of `tools.jobpool`, 61.9 s of gcc-debug's suite at R, is kept (decision 0228 item 9).
- `tools/mutate`: GCC reports "unjudged" where Clang says "killed" for a guard reached through a `consteval` call or a `constinit` variable of a header (decisions 0231 and 0232).
- Plan 0011's skipped delta reviews are accepted as a departure (decision 0248 item 8).
- `records/spikes/spike-cdf/run_memo.py` does not start, since `count_evals.py` was deleted (decision 0248 item 6); the recovery command is beside its import.

## Evidence

- V10 table: `v10-table.md`, every row filled from the matrix at R or from the record it names, with revision, command and seed.
- Every check ran on the released revision (DoD-29): the release matrix ran on R's code (all eleven presets, decision 0248 item 9, O5). From R to T nothing outside `docs/agent/` changed except the README's rga3 sentence (`1cd7ac8`), after which the README's tests passed again ("The tip T", rules 1 and 2). The cheap gates whose inputs are records reran on the records tip:
  - after U8: `tools/runlog check` and the trace gate at `4e7d4aa` (rows 21950, 21951);
  - in U9: the trace gate at `eea9132` (row 21952, after `develop` was merged in), decision 0172's composition check at `1cd7ac8` (row 21953, "total 17878 checks, all checks passed"), and `tools/runlog check`, the trace gate and the citation check over the code paths and over `docs/guide` at the U9 tip (rows in the commit after it);
  - in U10, at `18eb677`: `tools/runlog check`, the citation checks, the trace gate, `conventions_check.py` and the README, documentation-page and citation tests (rows 21960 to 21972);
  - at T: `tools/runlog check`, the trace gate and the citation checks over the code paths and over `docs/guide`, on the tree of U11's last records commit, with T adding only their rows (rule 3; plan 0042 section 11);
  - after T: the trace gate and `tools/runlog check` on the owner's CONVENTIONS 1.18 commit (rule 4).
- README after the rga3 sentence, at `1cd7ac8`, in both debug trees, run directly (departure 3): `cmake --build --preset P -- -j 8` reconfigured and found no work; `ctest --test-dir build/P -R "readme|docs_pages|citations"` passed 11 of 11 in gcc-debug and in clang-debug (`tests.citations`, its two tool tests, `rga3.readme_has_example`, `rga3.readme_example_1` to `_3`, `rga3.readme_cmake_1` and `_2`, `tests.docs_pages`, `tests.citations_guide`), 1.4 s each.
- DoD-2 inventory: `dod-2-inventory.md`; holds at R ("DoD-2" above).
- DoD-27: README, core guide, preset guide; every `cpp` block compiled and run as a test on both compilers, and each `cmake` block built as a project, in every preset at R (the 56 tests whose label matches `docs`, and `tests.docs_pages`). Re-read at `a73cd4e` (U6); the README's status text was filled in R and its rga3 sentence reworded in `1cd7ac8`; no user page cites a decision, plan or record.
- Auditor's release audit: `records/gates/2026-11-12-release-audit.md`, U10, at `18eb677`, branch `gate/release-1.0.0` `5bfb3a3`, merged into `develop` at `9075620` (no blocking finding: yes; 3 major, 9 minor, 10 info, ruled by decision 0250).
- The DoD of plan 0001, criterion by criterion: `dod-1-to-29.md`, with DoD-3's map in `dod-3-operation-map.md`.
- Owner walkthrough: **[owner]** `________`. Open blocking defects: `________`.
- Plans closed (DoD-28): `plan-closures.md`; plan 0038 closed by U1 (its status line and outcome); at U11, plan 0001's outcome (M8's acceptance left to the owner), plan 0042's status line and outcome (the merge facts left to the coordinator), and plan 0019's status line.
- Version and tag: the owner.

## What the release commit filled

Decision 0245 item 3 kept these placeholders until the release commit.

- The README's `FetchContent` block: `GIT_TAG "1.0.0"`; the URL stays `"<galoot-repository-url>"`, marked as a placeholder in the text and the comment (decision 0249 item 1; friction item 9).
- `CMakeLists.txt:6`: `VERSION 1.0.0`.
- The README's status line: "This is the 1.0.0 release of the core." The rga3 sentence was reworded after R, in `1cd7ac8` (decision 0249 item 2): "The rga3 preset ships in the same tree but is below 1.0, and its interface may still change."
- The study's byline: no author named; the default stands (decision 0241 item 3).
- **[owner]** CONVENTIONS 1.18 with the errata-naming row (decision 0248 item 2; the text is MA1 of `records/gates/2026-11-11-M7-audit.md`) and line 12's "most canonical reference" sentence reworded in the same edit (decision 0250 item 5): every text to paste, and the commands after it, are in `conventions-1.18-owner-text.md`. The tag message names 1.18.
