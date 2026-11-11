# Release record: galoot core 1.0.0 (draft)

- Status: draft, rewritten for the release (plan 0042 unit U6). Records only. Cells marked `____` are filled by U9 from the release matrix at the release commit R, or by the owner. Placeholders marked **[release commit]** are filled in the release commit itself (decision 0245 item 3), not before. The owner merges to `master` and tags (W13); nothing here is a release until then.
- First written: 2026-11-10 by Claude (agent, Sonnet 5.5) at `develop` `1ed0e6a`. Rewritten: 2026-11-11 by Claude (agent, Opus 5.5), planner, at `develop` `a73cd4e`, on branch `records/release-prep`, from decisions 0141 to 0248, plans 0038, 0040 and 0042, the M7 gate audit (`records/gates/2026-11-11-M7-audit.md`), and the records each item names.

## Departures, unknowns, and readings, first (C20)

1. **No figure here is from the release revision.** Every measured figure below names its revision and date. DoD-22's rows at R, the V10 table and the checks at R are U8's and U9's.
2. **The known exceptions were searched, not only copied.** I read decisions 0141 to 0248 for "exception", "known limit", "accepted as", "not claimed", "after 1.0", "past 1.0", "after v1", "release record", "known friction", "unsupported" and "not supported", and read each hit. Hits that bear on users of the release are in "Known exceptions" or "Known friction"; hits about the project's own tools (decision 0231's mutation-tool limits, decision 0228 item 9 and decision 0248 item 8's job-pool limits) are under "Accepted limits of the project's tools", since a user never runs them. Decisions before 0141 were not searched; their accepted items reached 1.0 through later rulings or the gate audits (inferred, not checked).
3. **The DoD-24 departure is stated from the records before the forced inlining.** E1 and E2 measured the code before decision 0244's attributes, and their reruns are deferred past 1.0 (decision 0246 item 2). What holds at the tip is the structure test's claim, which is narrower than E1's read (exception 4).
4. **DoD-1's class** is stated from the guide's limits page and decision 0205, not from a new test.
5. **DoD-22 at R is shared mode**, by the owner's ruling (decision 0248 item 5). Pinned serial builds (decision 0233 item 5) stay deferred.
6. **The tag names the records-only tip T, not R** (decision 0248 item 9, O4). DoD-29's "every check ran on the released revision" is met through the empty code diff from R to T and the cheap gates rerun at T; this record says so in those words below, at U9.

## Release

- Component: galoot core, version 1.0.0 (plan 0001 D17).
- Release commit R: ____ (U7). Tag: `1.0.0` (decision 0248 item 9, O6), on the records tip T: ____ (the owner).
- Code diff from R to T: `git diff --stat R T -- . ':!docs/agent'` prints ____ (must be empty).
- CONVENTIONS version implemented: ____ (1.17 at `a73cd4e`; 1.18 once the owner adds the errata-naming row, decision 0248 item 2).
- Preset versions: `rga3` 0.1.0 (decision 0248 item 9, O6; no file carries a version). cga3: not in this release (decision 0211).
- CMake project version: **[release commit]** `project(galoot VERSION 0.1.0 ...)` at `CMakeLists.txt:6` becomes 1.0.0.
- No `install()` rule and no package for `find_package` in 1.0 (decision 0240 item 5); the README names `add_subdirectory` and `FetchContent`.

## Class covered (DoD-1)

- Metrics: diagonal R(p, q, r) with null generators last (`R<P, Q, Z>()`, `rigid<D>()`), the conformal metric (`conformal<D>()`), and any symmetric integer form (`from_gram`), each with entries within `max_form_entry`. An exact change of basis (`basis_change`, `conformal_change`) maps between two such algebras with rational rows.
- Size: 1 to 16 generators (`max_generators`); a factory's `MaxN` raises the bound to at most 32 (`mask_bits`), untested above 16. A count of 0 or above the bound is rejected at compile time with a message (limits.md, "Number of generators").
- Names: blade names, `get<"...">`, `blades<...>` and printing work up to 9 generators (`max_compact_generators`); above that a layout is written as masks.
- Graded signatures have no spelling, and are rejected by `metric_like` (decision 0205).

## Platform and toolchain

- Tested: GCC 16.2.1 and Clang 23.1.4, each with libstdc++ 16, on x86-64 Linux, C++23 without compiler extensions (limits.md, "Toolchains"). MSVC, other standard libraries and other platforms are not tested and not claimed. Versions at the release run: ____ (U8).
- The core throws no exceptions; the core, the preset and `format.hpp` compile with `-fno-exceptions` (`core.bare_compile_no_exceptions`, `core.bare_compile_format_no_exceptions`). The test runner uses exceptions. DoD-21 stands on these bare compiles (decision 0240 item 4).
- The rounding check's bound assumes one rounding per operation to the scalar's own precision, to nearest, with gradual underflow and IEEE 754's NaN and infinity. A build that flushes subnormals to zero is outside it; a checking build under `-ffast-math` or `-ffinite-math-only` does not compile. The bound holds with or without multiply-add contraction (limits.md, "The rounding check"; decision 0243). Every preset builds with `-ffp-contract=off` (`CMakePresets.json`, the shared flags; read for all eleven presets).

## Known exceptions

Each is a departure the owner has accepted or deferred. None is a blocking defect unless the owner says so.

1. **The antiproduct's growth at fixed use (decision 0141, C6).** The geometric antiproduct and the antiwedge are built through the complements in all n generators, so their compile cost grows with n at fixed use. WU17's grid (`records/performance/wu17-growth/README.md`, at `18849b5`, measured 2026-11-10, before the forced inlining) entered the antiproduct's figures against 0141, as did `compose` and `transform`, which are built on it. Accepted for twelve months, to 2027-11-01; it ends at the first of: a production report of a build-time problem; a model, named type or use needing more than six generators while using few; 2027-11-01. The remedy of decision 0140 stays proposed. Listed by 0141 item 5.
   - With it, by decision 0163 item 3: the antiproducts are verified but not validated against a published non-diagonal table; the gap stands until such a table is found.
   - With it, by decision 0184 item 3: on Clang without libstdc++'s assertions (that is, in an optimized build), the geometric antiproduct of two `full<5>` multivectors of conformal(3) exceeds Clang's default constant-evaluation steps and does not build; `-fconstexpr-steps=1200000` builds it (limits.md, "Compile-time limits: products and plans").
2. **`bulk_dual`'s growth at fixed use (decision 0247, beside 0141).** From n = 4 to 16 at fixed use, +8.2 M instructions (+1.8%) on GCC and +34.4 M (+7.3%) on Clang, beyond the larger range of 0.5 M and 3.0 M (`wu17-growth/README.md`, at `18849b5`, 2026-11-10, before the forced inlining; the inlining does not change the plan generation the growth comes from, inferred). The cause given, the product with the antiscalar of grade n, is inferred from the pattern. Same end conditions and review date as exception 1 (0247 items 1 and 2); listed by 0247 item 5. The weight duals, the antidot and the projections were not measured.
3. **D8: no collected plans in 1.0 (decision 0239 item 1).** Operation counts of composed plans can exceed collected-term counts where terms cancel. DoD-24's counts are form A's, the kernels as specified (decision 0233 item 2). DoD-4 stands on the declared-output check: `transform` checks that what it drops is zero (exactly over an exact scalar, within the forward bound over a rounding one). The tests: `core.transform_nonzero_int64` and `core.transform_nonzero_rational`, `core.transform_consteval_nonzero` and `core.transform_consteval_nonzero_rational`, `core.transform_checking_recorded`, the `core.transform_unchecked_*` twins, and `core.transform_rounding`. The first candidate after 1.0 is a named batch-under-one-motor interface.
4. **DoD-24: what is and is not claimed about calls and speed.**
   - Claimed, at the tip: with `GALOOT_FORCE_INLINE` 1 (the default), at `-O2` without the checking mode, on x86-64, the 52 probe functions of the rga3 preset's operations compile with no call or jump into namespace `galoot`, on both compilers, in one unit and each alone (`rga3.kernels_inline`, `rga3.kernels_inline_alone`; passed in gate 3's workflows at `2438604`, 2026-11-11). At R: ____ (U8).
   - Not claimed: other layouts and algebras (some operations on conformal(3) multivectors of all or of even blades still compile to a call), `-Os` and other levels, `GALOOT_FORCE_INLINE` 0, and checking builds, where `transform` calls its checks out of line by design (decision 0244 item 1, "What is not claimed"; `assert.hpp`'s comment).
   - No blade branch and no allocation: read in E1 at `18849b5` (2026-11-10), before the forced inlining, on both compilers (`wu17-asm/README.md`, "Findings"); not re-read since. The structure test checks calls and jumps, not branches on blade data or allocation, so at the tip this half is inferred.
   - The pinned benchmark against the in-repo straight-line form (E2, `wu17-bench/README.md`, at `2de0d36`, 2026-11-10, CPU 4, under the lock, seed 20261110) measured the code before the forced inlining. Galoot was slower on Clang in the three transforms and `compose`, and on GCC in p ∧ q throughput, `compose` and transform(l, Q) latency at the smallest size: 10 of 22 loops slower and 6 faster under the strict reading. **The release claims no benchmark of its own code**; the rerun is deferred past 1.0 (decision 0246 item 2).
5. **The strict scalar rule (decision 0240 item 7).** Scalars must match exactly: an integer literal is not converted where the scalar type is not an integer, in the linear operations and the preset's motions. Kept for 1.0 and taught on the guide's first page ("2. Scalars must match exactly").
6. **`%` and `!` (decision 0240 item 7).** `%` is the geometric antiproduct and `!x` the right complement, not a boolean. Taught on the guide's first page ("3. Two operators to learn: `%` and `!`").
7. **Compile-time limits** of each operation are recorded on the limits page, not in the headers (decision 0240 item 9): `docs/guide/limits.md`.
8. **The one-blade catch-all is deferred past 1.0 (decision 0241 item 1).** A conversion a layout of one blade does not accept gets the compiler's own error, not a library message.
9. **Spurious overflows after the row reduction (decision 0236 item 2).** A change of basis can still be rejected as overflowing where its reduced inner products would fit. Accepted for 1.0; a backlog item unless a user meets it.
10. **The six helpers are public with fewer users than DoD-2 asks (decision 0248 item 9).** `basis_change_like` and `sum_layout` have no user outside their headers and tests; DoD-2's "an entry with no user is removed" is departed from for them by the owner's ruling. See `dod-2-inventory.md`.
11. **The `rga3` preset's `rotation`** builds its motor at run time only, since `std::sin` and `std::cos` are not constant expressions on every compiler (rga3.md); for a motor in a constant expression, write its coefficients.
12. **Other open items at release**: ____ (the release audit and the owner's walkthrough add here).

## Known friction

Each is measured or recorded where stated; none is a defect.

1. **Debug builds' peak memory.** gcc-debug's largest compile, `transform_identities_test.cpp`, peaked at 5.93 GiB at gate 3 (`2438604`, 2026-11-11), against 2.49 GB at `ed9288f` before the forced inlining (plan 0040 outcome, gate 3; the two figures are in the units the record gives). A user's large debug unit can need several GiB.
2. **Clang at `-O2` with the forced inlining compiles operation-heavy units slowly.** The clang-measure tree took 664.0 s at gate 3 against 116.1 s at `ed9288f`, and `identities_rounding_test.cpp` alone 599.0 s (1.81 GiB). This is about 664 s of every P9 merge into `develop` (decision 0244 item 17). `GALOOT_FORCE_INLINE` 0 is the user's remedy; gcc-asan, clang-asan and clang-release use it.
3. **Debug builds compile slower with the forced inlining.** GCC `product_test.cpp` +80.5% instructions, 2.18 s to 4.32 s, 418 to 830 MiB; `transform_test.cpp` +35.8%; Clang `product_test.cpp` +17.5% (`records/performance/pre-release-core-pass/cost-public-ops/`, at `a6f3196`, 2026-11-11; decision 0245 item 1, whose friction statement stands).
4. **Four helper macros stay defined in every user unit**: `GALOOT_DETAIL_PASTE_`, `GALOOT_DETAIL_PASTE`, `GALOOT_DETAIL_FORCE_INLINE_IS_0` and `GALOOT_DETAIL_FORCE_INLINE_IS_1` (`assert.hpp:44-47`), with `GALOOT_ALWAYS_INLINE`. Their `#undef` is on the post-1.0 backlog.
5. **An empty `GALOOT_FORCE_INLINE` gives two errors**: the literal "GALOOT_FORCE_INLINE must be 0 or 1" first, then the preprocessor's own from a later `#if` with no left operand (decision 0244 item 17; `core.force_inline_empty`). A value that does not paste into one token, such as `-1`, gets only the preprocessor's error.
6. **`-Os` builds still call into the library** (exception 4).
7. **`-ffp-contract=fast` can change the last bits** of a transform between a checking and a non-checking build (10% to 14% of coefficients measured) and between a constant evaluation and run time (43% to 49%); with `off` or `on` the compilers tested gave the same bits (decision 0243; limits.md, "The rounding check"). GCC contracts by default when the target has FMA.
8. **A forced-inline operation cannot be called from a function built for fewer instruction-set features** than its unit (for example a `[[gnu::target("arch=x86-64")]]` fallback in a `-march=x86-64-v3` file): both compilers stop with an error (`assert.hpp`'s comment on `GALOOT_ALWAYS_INLINE`).

## Baselines and the dates of what this record cites

Decision 0246 item 2 asks the release record to state the date of every baseline it cites.

| What | Revision | Date | Record |
| --- | --- | --- | --- |
| Performance history era | `ebdd06d09289` | rows 2026-10-31T20:40:59Z to 2026-11-01T23:52:19Z | `records/performance/history/eras.tsv` |
| E1, assembly read | `18849b5` | 2026-11-10 | `records/performance/wu17-asm/` |
| E3, growth grid | `18849b5` | 2026-11-10, lock held 17:48 to 17:52 UTC | `records/performance/wu17-growth/` |
| E2, benchmark (inline rerun) | `2de0d36` | 2026-11-10, lock held 18:34 to 18:58 UTC | `records/performance/wu17-bench/` |
| Core pass W10, parts A to E | `7a27f76` | 2026-11-11 | `pre-release-core-pass/cost/` |
| Core pass W10, public operations forced | `a6f3196` | 2026-11-11 | `pre-release-core-pass/cost-public-ops/` |
| Core pass W10, after D1 | `d2568e3` | 2026-11-11 (loaded machine: instructions comparable, wall times not) | `pre-release-core-pass/cost-d1/` |
| Opt-out, six identity units | `2d292ef` | 2026-11-11 | `pre-release-core-pass/opt-out/` |
| Gate 3 matrix (nine presets) | `2438604` | 2026-11-11 | `runs.tsv` rows 21815 to 21826; plan 0040 outcome |
| Newest `compile.tsv` rows before R | `c799821` | 2026-11-07T21:44:22Z, shared | `records/performance/compile.tsv` |
| DoD-22 at R | R | ____ | ____ (U8, shared mode) |

E2's benchmark and E1's read predate the forced inlining (decision 0244), and no rerun is part of 1.0. The WU17 baseline directory that part H names (`wu17-baseline/`) was not made (decision 0246 item 2).

## DoD-22: compile records

- At the release revision: per-translation-unit wall time and peak memory on both compilers, from the release matrix's gcc-debug and clang-debug trees by decision 0080's copy step, in **shared mode** (the job pool, not pinned): ____ (U8, U9).
- **P14 note.** These rows are not comparable with pinned rows: times measured on different kinds of core, or with different load on the core's other thread, are not compared. They are comparable with the earlier shared rows of `compile.tsv` (all but the 12 pinned rows of WU1 and its recorder follow-ups) only as far as the machine's load allows. Peak memory does not depend on the kind of core.
- Before and after for each core unit (W10): `records/performance/core-unit-w10-index.md`. **Accepted past gaps** (decision 0248 item 4): WU9 (plan 0018) has no W10 record and no `compile.tsv` row from its branch; plan 0009 has an after record with one before figure. No retroactive measurement. Cleanup 1 (plan 0007) changed comments only and owes none.

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

- Job pool: plan 0027's I2, I3 and I5 and plan 0028's I5 to I7 are known limits (decision 0248 item 8); an unreadable `/proc/<pid>/fd` hides a client (decision 0228 item 9); the cost of `tools.jobpool`, about 57 s per preset, is kept (decision 0228 item 9).
- `tools/mutate`: GCC reports "unjudged" where Clang says "killed" for a guard reached through a `consteval` call or a `constinit` variable of a header (decisions 0231 and 0232).
- Plan 0011's skipped delta reviews are accepted as a departure (decision 0248 item 8).
- `records/spikes/spike-cdf/run_memo.py` does not start, since `count_evals.py` was deleted (decision 0248 item 6); the recovery command is beside its import.

## Evidence

- V10 table: `v10-table-skeleton.md`, filled at R: ____ (U9).
- Every check ran on the released revision (DoD-29): the release matrix ran at R (all eleven presets, decision 0248 item 9, O5); `git diff --stat R T -- . ':!docs/agent'` is empty; the cheap gates whose inputs are records reran at T: ____ (U9).
- `plan_cost_model`'s `selfcheck`, `comments`, `comments_test.py` and `jobserver_check.py` at R, and the snippets index row refreshed (decision 0248 item 7): ____ (U8).
- DoD-2 inventory: `dod-2-inventory.md`, re-grepped at `a73cd4e`; at R: ____ (U9, a diff check).
- DoD-27: README, core guide, model guide; every `cpp` block is compiled and run as a test on both compilers, and each `cmake` block built as a project (the example tests that `test/documentation.cmake` registers for the README and the four guide pages, labeled `<module>.docs`, and `tests.docs_pages`, in every preset). Re-read at `a73cd4e` (U6): the README's status line is a placeholder for the release commit; its pointers to the guide's four pages resolve, as do every relative link and anchor of the README and the guide; no user page cites a decision, plan or record. At R: ____ (U8).
- Auditor's release audit: ____ (no blocking finding: ____).
- Owner walkthrough: ____. Open blocking defects: ____.
- Plans closed (DoD-28): `plan-closures.md`; plans 0001, 0038 and 0042 by their own units: ____.
- Version and tag: the owner.

## Placeholders filled in the release commit

Marked so that nothing is filled early (decision 0245 item 3). U7 fills them once the owner gives the URL and the byline (decision 0248 item 9).

- **[release commit]** The README's `FetchContent` block: `GIT_REPOSITORY "<galoot-repository-url>"` and `GIT_TAG "<release-tag>"` (README, "Using Galoot"), and the sentence above it that calls them placeholders. The URL comes from the owner; the tag is `1.0.0`.
- **[release commit]** `CMakeLists.txt:6`: `VERSION 0.1.0` to `1.0.0`.
- **[release commit]** The README's status line: "The core is close to its first stable release, and its interface may still change until then."
- **[release commit]** The study's byline (`research-notes/0000-design-study.md` line 4), only if the owner names an author; otherwise the default stands (decision 0241 item 3).
- **[owner]** CONVENTIONS 1.18 with the errata-naming row (decision 0248 item 2; the text is in `records/performance/core-unit-w10-index.md`), and the "most canonical reference" sentence at the walkthrough (decision 0241 item 2). The tag message names the CONVENTIONS version in force at T.
