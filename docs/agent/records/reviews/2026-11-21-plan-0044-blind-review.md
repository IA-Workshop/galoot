# Blind review: plan 0044, release 1.0.1 header and interface hygiene (U6)

- Reviewer: blind-reviewer role, Claude (agent, Opus 5.5), 2026-11-21. Independent of the authors of U1 to U5 and of the integration step; it shares none of their working context.
- Reviewed: `main...feature/header-hygiene-1-0-1` at `67907a50e9571044adfc323e0ff9ef1946116aed`, merge base `01dce7f39d1cae6b65ab57349fe870aa187f5663` (whose `include/`, `test/CMakeLists.txt` and `docs/guide/core.md` equal `c95fee5`'s, the plan's base). 25 commits, 38 files. Units U1 to U5.
- Review branch `review/header-hygiene-1-0-1`, created at the reviewed revision, in the worktree `galoot-wt/review-header-hygiene`, with its own build trees (gcc-debug, clang-debug, clang-release, lint).
- Criteria: plan 0044 sections 2 and 4, section 6 per unit, sections 7 and 8; decision 0252 items 6 to 10; CONSTITUTION, WORKFLOW, VVUQ, STYLE; the usability checklist of the role file for the macro's diagnostics, the cache option's message and the guide text.
- Did the brief carry a verdict? No. It carried the authors' departures and the run results they reported. I treated those results as claims and reran the ones listed below.
- Outcome: **approve, with non-blocking findings.** No blocking or major finding.

## Not part of this review: the `cpp rejected` block for the deleted `==`

Definition of done item 7, second half, is open with the owner and is not counted as a finding. What the branch does: `docs/guide/core.md` (after the paragraph on `same_rows`) says that `a == b` and `a != b` do not compile and that the compiler shows the header line ending in `// use same_rows(a, b)` in a note. I checked that sentence on both compilers (below). The existing header tests `core.basis_change_equality` and `core.basis_change_inequality` require the literal "// use same_rows(a, b)" in the compiler output. I ran both on gcc-debug and clang-debug at the reviewed revision, and both passed. What the branch does not do: no documentation test pins the hint. No `cpp rejected` block was added, and the documentation harness was not changed.

## What I checked, and how

All runs were made at `67907a5` through `tools/timed`. Their rows are committed with this record. The machine was shared throughout: load average 19 to 24, and the job pool read 0, 11, 18, 19 and 20 of 23 slots free at various points, with other workstreams' `ninja` as clients. The wall times below are not comparable with idle-machine figures.

1. **The diff, in full.** I read every changed file under `include/`, `test/`, `CMakeLists.txt`, `CMakePresets.json`, `README.md` and `docs/guide/`, and the records (decision 0254 and the mutation README with its rows). I also read each commit message.
2. **Section 4, item 1 (the include diff).** The diff holds the `#elif` chain and four `#undef`s in `assert.hpp`, the "build preset" word, `l`→`c` and `m`→`k`, and the two deleted overloads. It also holds U3's extra renames in `metric.hpp` and `motions.hpp` (finding m2).
3. **Section 7.1 over the whole branch, against `c95fee5`.** I ran the committed `behavior/compare_lines.sh` with `git archive c95fee5 include` as the old side (row `preprocessed-compare-branch`). It covers sixteen modes: both compilers, checking 0 and 1, and forced inlining unset, bare `-D`, 0 and 1. In every mode, the normalized `-E -P` output differs only by the renamed tokens (`k`, `result`, `form`, `c`, `axis`) and the two deleted templates. It loses 38 lines and gains 25, or loses 36 for Clang at mode 0, where no attribute line prints. The `-dM` output loses exactly the four `GALOOT_DETAIL_*` helpers and gains nothing.
4. **Section 7.2, code generation.** I compiled `test/bench/kernels_inline.cpp` at `-O2 -ffp-contract=off -DGALOOT_CHECKING=0` against `c95fee5`'s include tree and the tip's, with each compiler, and compared `objdump -d --no-show-raw-insn` (row `codegen-compare-kernels-inline`). Both are identical: 5,115 lines for GCC and 7,513 for Clang. I did not use the test's own `-std` and `-march` list; I used `-std=c++23` alone.
5. **The guide's spellings of `GALOOT_FORCE_INLINE` (n2).** Each spelling was compiled with the whole core under `-Wall -Wextra -Wpedantic -Wundef` on GCC 16.2.1 and Clang 23.1.5 (row `probe-force-inline-spellings`):
   - Unset, bare `-DGALOOT_FORCE_INLINE`, `0` and `1` give no diagnostic.
   - Empty, `ON`, `true`, `2`, `01` and `0x1` give exactly one error, "GALOOT_FORCE_INLINE must be 0 or 1".
   - `-1` gives one error, the paste error.

   Every clause of the guide's new text holds. `1 2`, which the guide does not mention, gives two errors. Each rejected spelling also draws one `-Wundef` warning (finding m4).
6. **The deleted `==` sentence.** `a == b` and `a != b` on two `from_rows` values were compiled on both compilers with `-I`, and again with `-isystem` (row `probe-deleted-eq`). Both compilers report the deleted `operator==`, and a note echoes `basis_change.hpp:177`, which ends with `// use same_rows(a, b)`.
7. **The README's copy route.** I copied `include/galoot` alone to `third_party/galoot` in a scratch directory and compiled README example 1 with `-std=c++23 -I third_party` on both compilers. It built and ran with exit code 0 (row `readme-copy-route`). The two CMake blocks are unchanged in the diff.
8. **Tests, in my own trees.**
   - On gcc-debug and clang-debug, I ran labels `core.assert` and `core.mv`: 112 tests per tree, all passed. They include `core.public_macros`, `core.force_inline_empty` (now `ONE_ERROR`), `core.force_inline_on`, and the four rewritten compile-fail tests.
   - Also on both trees: `tools.configure_rejects_force_inline`, `tools.configure_rejects_checking`, `core.basis_change_equality` and `_inequality`, `tests.messages_pinned` and `tools.catalogue`. All 6 passed in each tree.
   - On clang-release, where forced inlining is off: label `core.assert` (12 tests) and the two configure tests. All passed.
   - On both debug trees: `core.include_scan`, `rga3.include_scan`, the `bare_compile` tests and `tests.compile_fail_flags`. 10 per tree, all passed.
   - I read one configure test's verbose output. The inner configure stops at `CMakeLists.txt:112` with "GALOOT_FORCE_INLINE must be ON or OFF, not '2'." after compiler detection. Each configure test takes about 0.2 s.
9. **Lint.** I built `core_user_names`, `core_linear_user_names`, `core_force_inline_build` and `core_mv_test` in the lint preset, which runs clang-tidy with warnings as errors (117 s). All were clean, so no header name is confusable with the unit's `I` or `rn`.
10. **Formatting.** `clang-format --dry-run -Werror` on the thirteen changed C++ files: clean. No changed line is longer than 80 columns. The two long lines in those files predate the branch.
11. **Convention trace gate,** as W5 gives it: exit 0. The items with no test (6.1 to 6.4) are as on `main`.
12. **STYLE S11.** I searched the added lines outside `docs/agent/` for decision, plan, review, governance provision, unit or agent citations, and found none.
13. **Process.**
    - Each test change that judges its author's change is in its own commit, before the code (W9): `b2665c3`, `361007c`, `e2ab646` with `01a4a20`, and `62374f1`.
    - The commits that leave tests failing name those tests (W2): `b2665c3`, `361007c` and `e2ab646`.
    - Decision 0254 records the U1 choice (C18). Decision 0252 items 6 to 10 are followed: O1 (a), ON or OFF only; O2 (a), the copy route by wording; O3 (a), the guide note; O4 (a), governance and agent files untouched; O5, one feature branch.
    - The tip's tree equals that of `86ba743`, on which the integration step ran the documentation selection, so I did not rerun that selection (P10).
    - A gcc-debug whole-suite run passed at `62374f1a4ed1-dirty`, whose tree is `652bba7`'s. Only documentation and records changed after it.
14. **Mutants (section 8).** I read the specs and rows. The U1 and U2 rows show each mutant killed by the test written for it. The m5 propagation mutant ran on clang-release, an OFF tree. I did not rerun any mutant. For U3, I confirmed only that the lint build of `core_user_names` is clean at the tip.

## What I did not check

- The full P9 `main` row: the whole suite on gcc-debug, clang-debug, gcc-asan and clang-measure, plus `format-check` over the tree. That is U7's (plan section 9). I ran no whole suite at the tip.
- The W10 compile-cost comparison (section 7.6). That is U7's, under the timing lock. I took no timing lock and made no cost measurement.
- `tools/runlog check`, which the merge runs. The section 3.3 confusable scan, which is U7's; I relied on the lint build of the user-names units instead.
- The other nine presets. The documentation pages outside README, `core.md` and `limits.md`.
- Clause N4950 [cpp.cond] p14, which decision 0254 quotes. I did not reread it in the standard. The compilers' behavior agrees with it on both toolchains.
- Whether any user relies on the removed `GALOOT_DETAIL_*` macros. That is for the release notes (plan 0045).

One row of the run log, `check - include-scan`, is a `--help` invocation of `tools/include_scan.py` that I ran by mistake. It checked nothing. The real include-scan tests are the `include_scan|bare_compile|compile_fail_flags` rows. One `build gcc-debug review-selection` row with exit 126 is a mistyped memguard invocation that built nothing. The next row is the real build.

## Usability checklist (macro diagnostics, cache option message, guide text)

The tasks, from the examples only:
1. Turn the forced inlining off for one heavy unit (`-DGALOOT_FORCE_INLINE=0`), and get told plainly when the spelling is wrong.
2. Configure Galoot's own build with the inlining off (`-DGALOOT_FORCE_INLINE=OFF`), and get told when the value is not ON or OFF.
3. Compare two basis changes, finding out from the error or the guide that `==` is deleted and `same_rows` is the comparison.
4. Vendor the headers with no build system.

For task 1 I had to read `assert.hpp` to learn that `1 2` or `1+0` are not covered by the guide. Neither is a plausible spelling. The other tasks needed no implementation reading.

- **Names.** The interface has no new public names. `GALOOT_DETAIL_*` are now gone from users' macros. Met.
- **One error per misuse, message on the first error line.** Met for every rejected spelling under the flags of Galoot's tests and of the probe. Under a user's `-Werror -Wundef`, the first error line names `GALOOT_DETAIL_FORCE_INLINE_IS_<value>` (finding p1; this predates the branch).
- **Message starts with what was written, states the mistake, gives the fix.** The macro's literal "GALOOT_FORCE_INLINE must be 0 or 1" names the macro and the accepted values. It does not echo the value, and it has no fix clause after a semicolon. The plan keeps the literal unchanged and the branch does not alter it, so I record this as an observation, not a finding. The cache option's message "GALOOT_FORCE_INLINE must be ON or OFF, not '2'." names the option, the value and the accepted values, and matches `GALOOT_CHECKING`'s. Met.
- **Distinct causes give distinct messages.** The `#error` covers every bad spelling, the paste error covers `-1`, and the CMake check covers a bad cache value. Met.
- **No hidden default changes a result.** Every build preset now states its forced-inline mode. Met (C8).
- **Examples compile and are the idiom to copy.** The README copy route compiles from `include/galoot` alone. The guide's `==` text is accurate on both compilers. Nits n2 and n3 suggest wording.
- **Before release (Hyrum's law).** The only user-visible removals are the four `GALOOT_DETAIL_*` macros, documented as not interface. Plan 0045's release notes should name them, as decision 0254 says. The cache option no longer accepts `TRUE`, `YES`, `1` and the like (decision 0252 item 6). Only Galoot's own top-level builds see it, since it sits after `if(NOT PROJECT_IS_TOP_LEVEL) return()`. No suggested change would break users later.

## Findings, most severe first

None blocks the merge.

### m1 (minor, confirmed). The `///` block of `GALOOT_ALWAYS_INLINE` no longer sits directly before its definition

`include/galoot/core/assert.hpp:43-97`. The block that documents `GALOOT_ALWAYS_INLINE` (lines 43 to 70) is now followed by a `//` comment, then the four helper `#define`s with their NOLINT block (85 to 89), and only then by the `#if` chain that defines the attribute.

Failure scenario: a reader, or any tool that attaches `///` to the next declaration, takes the block as the documentation of `GALOOT_DETAIL_PASTE_`. The plan asked for the block to be "kept directly before its definition" (U1), and S11 asks for a `///` comment immediately before each declaration. The author flagged the order (U1 departure 3).

Fix: move the helper comment and the four `#define`s with their NOLINT lines above the `///` block, for example where the old check stood, before `#include <cstdio>`. The `///` block, the "A macro, because..." comment, the chain and the `#undef`s would then follow in that order. Preprocessed output is unchanged. Since the fix moves `#define` lines in `include/`, it is not a comments-only change and needs a delta review (W5, plan U6). Recommendation: fix before U7, or accept as is and note it for 1.1.0. The behavior is correct either way.

### m2 (minor, confirmed). The include diff is wider than section 4 item 1 lists

`include/galoot/core/metric.hpp:188-260` (`m` to `result` and `form`) and `include/galoot/presets/rga3/motions.hpp:32-137` (`l` to `axis` in the four `rotation` overloads and their `///` text) are outside U3's named files. The criterion lists only "two renamed locals with their comment words".

The author reported this as U3 departure 1, with the cause: clang-tidy reports only the first header name of each look-alike group, so the planned scan was masked. I confirmed the result. The lint build of `core_user_names` is clean only with these renames, so DoD 4 needs them. No interface changes: C++ has no named arguments, and rga3 is below 1.0. The preprocessed and code-generation comparisons show nothing else changed.

Failure scenario: none in behavior. The literal criterion is not met. Recommendation: the coordinator accepts the wider rename as the reading every alternative permits, and U8 records it in the plan's outcome. Reverting `c956775` would leave DoD 4 unmet. No owner decision is needed, since decision 0252 accepted the plan's own reading that renaming a parameter changes no interface.

### m3 (minor, confirmed). Two commits do not identify agent authorship

`e63e3fd` ("Records: the U3 mutant spec") and `7526c07` ("Records: the U3 mutant's run and rows") have no agent line and no Co-Authored-By trailer. W7 says "a change made by an agent is identified as such in its message". The other 23 commits carry the trailer.

History is not rewritten, so the resolution is a record: both commits are U3's agent work (Claude, Sonnet 5.5, implementer of U3, by the mutation README's own account). U8 may repeat this in the plan's outcome.

### m4 (minor, confirmed). Decision 0254's account of `-Wundef` is incomplete

`docs/agent/decisions/0254-forced-inline-mode-check-in-one-chain.md`, Evidence, first bullet, says that under `-Wundef` "an empty value and `-1` also draw a warning". In my probe, every rejected spelling draws one `-Wundef` warning on both compilers: empty, `ON`, `true`, `2`, `01`, `0x1` and `-1`. The warning names `GALOOT_DETAIL_FORCE_INLINE_IS_<spelling>`.

Failure scenario: a reader of the decision concludes that `ON` under `-Wundef` is quiet, when it is not. Decisions are not rewritten, so a correcting note belongs in a later record. U8's plan outcome is enough.

### p1 (predates the branch, minor, confirmed). Under `-Werror -Wundef`, the first error names an internal macro

`include/galoot/core/assert.hpp:90`. A user who builds with `-Werror -Wundef` and writes `-DGALOOT_FORCE_INLINE=ON` sees two errors. The first is `'GALOOT_DETAIL_FORCE_INLINE_IS_ON' is not defined, evaluates to 0 [-Werror=undef]`, and only then does the literal follow (both compilers). C9 and the checklist ask that the first error line carry the message, with no internal names. 1.0.0 behaves the same.

Option (a): wrap the chain's first `#if` in `#pragma GCC diagnostic push`, `#pragma GCC diagnostic ignored "-Wundef"` and `#pragma GCC diagnostic pop`. A standalone probe shows that this leaves exactly the `#error` on GCC 16.2.1 and Clang 23.1.5. It costs three lines and a test with `-Wundef -Werror`. Option (b): leave it as is. Galoot's builds do not set `-Wundef`.

Recommendation: (a), in 1.0.1 if the coordinator wants it inside this plan's scope (it touches the same block as m1), or in 1.1.0. Reported, not fixed, as outside this review.

### n1 (nit). The cache option's comment overstates the mode's neutrality

`CMakeLists.txt:100-101` says "it changes no value an operation computes". `assert.hpp:28-31` and `docs/guide/core.md` say that with multiply-add contraction the values can depend on the mode. The claim holds in Galoot's builds because every preset passes `-ffp-contract=off`. Suggested wording: "changes no value an operation computes in these builds, which turn contraction off".

### n2 (nit). The `!=` case of the guide's sentence

`docs/guide/core.md:1138-1140` says "the compiler reports that the operator is deleted". For `a != b`, both compilers name `operator==`, which the language rewrites `!=` through. Suggested wording: "reports that `==` is deleted (for `!=` too, which the language rewrites through `==`)".

### n3 (nit). README wording

`README.md:38-39`. "Copy or vendor the `include/galoot` directory, with its name, into a directory on your include path" is hard to parse at "with its name". Suggested wording: "keeping its name, `galoot`,".

### n4 (nit). "Preset" for a build configuration in new test text

`test/core/user_names.cpp:2` says "in the lint preset" without "build" before it. DoD 8 concerns user-facing text and the plan's inventory did not include test comments, so this is a nit only.

### n5 (nit). The rga3 guide's notation

`docs/guide/rga3.md:154` still writes `rotation(l, theta)`, while the parameter is now `axis`. It reads as notation for the line and is correct. Changing it is optional (U3 departure 8).

### n6 (nit). The `product_on_result` comment

`test/core/mv_test.cpp:1081-1083`. "as a test that names no layout reads it" is unclear. Suggested wording: "the layout a product takes when no target is named".

### o1 (note). `tools/mutate` cannot judge a lint-build mutant

The U3 mutant's verdict is "unjudged", because the tool classifies a clang-tidy error in a header as `invalid` (U3 departure 4). The mutant is killed in fact. This is a defect of `tools/mutate`, outside this plan's files. Recommendation: list it for plan 0043 or the backlog.

### o2 (note). The catalogue's record of message places is stale

`records/diagnostics/catalogue/scan-output.md` names message places in `mv.hpp` below the deletion that are now 10 or 18 lines off (U4 departure 4). No check reads the places. Recommendation: U8 regenerates the record if the release record quotes places, and otherwise leaves it.

## Departures of the authors, as judged here

- U1 1 to 8, U2 1 to 9, U4 1 to 6 and U5 1 to 5: each reading is one that the plan and decision 0252 permit. None hides a weakened check. The W9 test changes (U1 5, U3 2, U4 1) each ask for at least what they asked before, and the expected values in `mv_test.cpp` are unchanged.
- U3 1: see m2. U3 4: see o1. U4 4: see o2.
- The integration step's readings (1) to (3) are consistent with the plan. Its extra records commit `86ba743` holds only run rows.

## Open items for the coordinator (each with a recommendation)

1. **m1, the order of the `///` block.** Fixing it costs a small edit and a delta review. Leaving it keeps a documentation block misplaced in a public header. Recommendation: fix before U7, together with p1 if p1 is taken in 1.0.1.
2. **p1, `-Wundef` under `-Werror`.** (a) Pragmas and a test now; (b) 1.1.0; (c) leave it. Taking it now costs three lines in the same block as m1 and adds a new compile-fail test. Recommendation: (a) or (b), and not (c).
3. **m2, the wider rename.** Accept it, and have U8 record it in the plan's outcome.
4. **The `==` documentation test** (DoD 7, second half). This is the owner's ruling, not part of this review.
