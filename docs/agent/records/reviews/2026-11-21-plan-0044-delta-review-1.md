# Delta review 1: plan 0044, the fix round after the blind review

- Reviewer: blind-reviewer role, Claude (agent, Opus 5.5), 2026-11-21. I did not write the change and share no working context with its author (the cpp-expert fixer) or with the reviewer of `2026-11-21-plan-0044-blind-review.md`. I read that record as the list of earlier findings.
- Reviewed: `67907a50e9571044adfc323e0ff9ef1946116aed..3aa92085484f2353ec6f28fc5e1f515d3d1c01bb` on `feature/header-hygiene-1-0-1`. Five commits: `97435f9` (assert.hpp), `8a7fb13` (three comments), `c64ae65` (README and guide), `108a477` (correction appended to decision 0254), `3aa9208` (records and run rows). Nine files.
- Where: the review branch `review/header-hygiene-1-0-1`, worktree `galoot-wt/review-header-hygiene`. I merged `3aa9208` into the review branch (`cc98fb5`, no conflicts). That merge's tree differs from `3aa9208` only by the earlier review record and its run rows, so every run below at `cc98fb5` ran on the reviewed code.
- Criteria: the earlier findings; plan 0044 sections 2, 4, 6, 7 and 8; decisions 0252 and 0257; CONSTITUTION, WORKFLOW (W5, W6, W7, W9), STYLE (S3, S4, S11); the role file's usability checklist, since `assert.hpp` holds a public diagnostic.
- Did the brief carry a verdict? No. It carried the fixer's twelve departures. I treated each run they report as a claim and reran the ones listed below.
- Outcome: **approve.** No blocking or major finding. Four nits and two notes, none of which needs a change before the merge.

## What I checked, and how

Every run went through `tools/timed`. Rows `delta1-*`, `mutant:*`, `convention-trace` and the two test selections at `cc98fb5` are committed with this record. The pool read 4 of 23 slots free at the start (load average 15.8), then 22 or 23 free during my builds (load 5 to 10). Wall times are for a shared machine.

1. **The diff, in full.** I read all of `assert.hpp` at the tip and every hunk of the other eight files. I read each commit message. All five commit messages say what changed and why, identify agent work, and carry the trailer (W7).
2. **What a user sees for each spelling** (`delta1/probe.sh`, rows `delta1-force-inline-spellings-tip` and `-67907a5`). Each case compiles one unit that includes `core.hpp`, `presets/rga3.hpp` and `core/format.hpp`, with `-std=c++23 -fsyntax-only`, on GCC 16.2.1 and Clang 23.1.5. I varied the include flag (`-I`, `-isystem`), the warnings (none, `-Wundef`, `-Wundef -Werror`) and fourteen spellings: unset, bare `-D`, `0`, `1`, empty, `ON`, `OFF`, `true`, `2`, `01`, `0x1`, `-1`, `1 2` and `1+0`. That makes 168 cases per revision. I ran the same probe against `git archive 67907a5 include`. **The two tables are identical once the line numbers in `assert.hpp` are normalized.** The fix moves the chain down two lines and changes nothing else a user sees. At the tip:
   - Unset, bare, `0` and `1`: no diagnostic in any of the 24 combinations.
   - Empty, `ON`, `OFF`, `true`, `2`, `01` and `0x1`: exactly one error, the literal "GALOOT_FORCE_INLINE must be 0 or 1", with no warning flag, and with `-isystem` under any flags.
   - With `-I -Wundef`, each of those seven spellings gives the same one error plus one warning naming `GALOOT_DETAIL_FORCE_INLINE_IS_<spelling>`, and the warning comes first. With `-I -Wundef -Werror`, there are two errors and the first names the internal macro. This is the pre-existing p1. **With `-isystem`, neither compiler warns,** since the header is then a system header.
   - `-1`: one paste error, two with `-I -Wundef -Werror`. `1 2`: two errors. `1+0`: accepted silently, and it evaluates as 1. All of these predate the branch and were noted in the first review.
3. **Preprocessed output of the valid spellings** (`delta1/pp.sh`, row `delta1-preprocessed-compare-tip-vs-67907a5`). Both compilers, `-I` and `-isystem`, `GALOOT_CHECKING` 0 and 1, `GALOOT_FORCE_INLINE` unset, bare, 0 and 1: 32 `-E -P` outputs and 32 `-E -dM` outputs. Each is hashed after the include root is replaced by a fixed word. **All 64 are byte-identical to `67907a5`'s.** The hashes differ between modes, so the comparison can tell modes apart. At the tip, `-dM` lists only the guards, `GALOOT_CHECKING`, `GALOOT_FORCE_INLINE`, `GALOOT_ALWAYS_INLINE` and `GALOOT_ASSERT`, on both compilers.
4. **Mutants of the header change** (`tools/mutate` with my spec `delta1-assert.toml` in my scratchpad, quoted in the appendix; gcc-debug and clang-debug; selection `core.assert`, 12 tests). The fix adds no test, so I wrote three mutants of the move it makes and reran U1's three against the tip:

   | Mutant | gcc-debug | clang-debug | Killers |
   | --- | --- | --- | --- |
   | `helpers_after_undef`: the helper block moved below its `#undef`s | killed | killed | `core.public_macros`, `core.force_inline_{on,empty,value}`, `core.assert_checking_value` |
   | `is_0_lost`: `GALOOT_DETAIL_FORCE_INLINE_IS_0` dropped in the move | unjudged | unjudged | the build of `core_force_inline_off` fails with the `#error`; see o1 below |
   | `helpers_in_error_group`: the helpers moved into the error group | killed | killed | as the first |
   | U1 `undef_dropped` | killed | killed | `core.public_macros` |
   | U1 `error_group_undefined` | killed | killed | `core.force_inline_on` |
   | U1 `elif_split` | killed | killed | `core.force_inline_empty` |

   `git status` showed the header restored after the run. So the U1 verdicts that the fixer carried over (departure 7) now also hold at the tip.
5. **Tests** (rows at `cc98fb5`):
   - Label `core.assert` with `tools.configure`, 14 tests each on gcc-debug and clang-debug (forced inlining ON) and on clang-release (OFF): all passed.
   - Label `core.mv` on gcc-debug and clang-debug, after rebuilding `core_mv_test`, whose comment changed: 100 per tree, all passed.
   - On gcc-debug, the README and `core.md` documentation tests with `tests.docs_pages`, `tests.citations_guide` and `core.basis_change_equality` and `_inequality`: 35 passed. This is the compiler the fixer did not use for that selection.
   - The fixer's own whole-suite gcc-debug run is at `108a477`, whose code tree is `3aa9208`'s. I did not repeat it (P10).
6. **Lint.** I built `core_user_names`, `core_linear_user_names`, `core_force_inline_build` and `core_mv_test` in the lint preset, which runs clang-tidy with warnings as errors: clean (67 s). The moved `NOLINTBEGIN` and `NOLINTEND` pair still covers the four helper `#define`s.
7. **Formatting.** `clang-format --dry-run -Werror` on `assert.hpp`, `mv_test.cpp` and `user_names.cpp`: clean. No added line is longer than 80 columns.
8. **Convention trace gate,** from the root as W5 gives it: exit 0. The untested items are 6.1 to 6.4, as before.
9. **The text of the fixes, against the compilers and the code:**
   - n2. `a == b` and `a != b` (the header's own compile-fail sources) on both compilers, with `-I` and `-isystem` (row `delta1-deleted-eq-hint`). GCC reports "use of deleted function ... operator==". Clang reports "overload resolution selected deleted operator '=='". Each echoes one line ending in `// use same_rows(a, b)` in a note. The guide's new sentence holds in all eight cases.
   - n3. The README's copy route. I copied `include/galoot` alone to `third_party/galoot` and built and ran README example 1 with the README's command, on both compilers (row `delta1-readme-copy-route`): exit 0.
   - n6. `geometric_product_layout`, `wedge_layout`, `geometric_antiproduct_layout` and `antiwedge_layout` are each `product_plan<Op, ...>.result` (`product.hpp:194-215`). `BinaryOperation` has only those four products (`plan.hpp:88-98`). The comment is exact.
   - n1. All eleven configure presets inherit `checks`, `sanitizers` or `measurement`, and each of those passes `-ffp-contract=off` (`CMakePresets.json:46,54,62`). The new claim holds for the build presets. See nit d3.
   - m4 and p1. I reproduced the pragma option on a scratch copy of the tip's header (row `delta1-p1-pragma-variant`): `#pragma GCC diagnostic push`, `ignored "-Wundef"` and `pop` around the chain. With it, `-I -Wundef -Werror` gives exactly the literal for every spelling that draws it, on both compilers. `-1` keeps one paste error and `1 2` keeps two errors. The `-E -P` output of a valid spelling gains exactly the three pragma lines, on both compilers. That confirms the fixer's account in the correction to decision 0254. I did not probe the `__has_cpp_attribute` and `system_header` variants.
10. **STYLE S11.** No added line outside `docs/agent/` cites a decision, plan, review, provision or agent. The new comments in `assert.hpp` state why, in the file's own voice.
11. **W9 and the departures.** No test, expected value or build rule changed (departure 2 holds: the hunks in `test/` and `CMakeLists.txt` are comments only). Decision 0257 is respected: no `cpp rejected` block, and `tools/doc_examples.py` and `test/documentation.cmake` are untouched (departure 12). Decision 0252 items 6 to 10 are unaffected by this round.

## What I did not check

- The P9 `main` row: the whole suite on gcc-debug, clang-debug, gcc-asan and clang-measure, the full lint tree, and `format-check`. That is U7's.
- The W10 compile-cost comparison. I took no timing lock and made no cost measurement.
- `tools/runlog check`. The fixer's configure comparison of the eleven presets (departure 9): I did not rerun it. The only `CMakeLists.txt` change is a comment, and the three trees I rebuilt reconfigured without error.
- The `__has_cpp_attribute` and `#pragma GCC system_header` probes of the decision 0254 correction. Whether Clang contracts multiply-adds by default (d3 rests on that; inferred, not probed).
- Compilers other than GCC 16.2.1 and Clang 23.1.5.

One slip of mine, disclosed under C20. A command whose regex variable was unset started `ctest --test-dir build/gcc-debug` with no selection, outside `tools/timed` and without the pool's environment. I stopped it after about two minutes, and no process of it remained. Its results are discarded, it has no run-log row, and it wrote only to that build tree's `Testing/` directory. The gcc-debug documentation run with exit 8 is a separate, recorded run made before the example programs were built (35 "Not Run"). The next row is the real run.

## The earlier findings

| Finding | Resolution | Judged here |
| --- | --- | --- |
| m1, the `///` block of `GALOOT_ALWAYS_INLINE` | `97435f9`: the helper comment, the `NOLINT` pair and the four `#define`s now come before the `///` block. The block is followed by the `//` "A macro, because..." comment and then the chain (`assert.hpp:43-99`). | **Resolved.** The order is `///`, `//` rationale, then the definition, which is the pattern `GALOOT_ASSERT` uses at `:137-154` (fixer departure 3). Preprocessed output is identical, and the mutants above kill a misplaced or incomplete move. The split comments are each accurate where they sit. |
| m2, the wider rename | No branch change. | **Reasonably left.** U8 records it in the plan's outcome. |
| m3, two commits without agent attribution | No branch change; history is not rewritten. | **Reasonably left.** U8 records it. All five commits of this round are attributed. |
| m4, decision 0254's `-Wundef` account | `108a477`: a dated correction appended to 0254, the text above it unchanged. | **Resolved.** Its spellings match my probe for `-I`. It does not say that `-isystem` silences the warning (d1). An appended, dated correction has precedent in this directory (for example 0026, 0095 and 0170), and 0254 is not yet on `main`. So I read it as consistent with W6, not as a rewrite. |
| p1, `-Wundef -Werror` puts an internal name first | Not changed (departure 1). Probed, with the reason recorded in the 0254 correction. | **Reasonably left for 1.0.1.** It predates 1.0.0. The pragma fix adds three lines to every valid spelling's `-E -P` output, against DoD 2's "preprocess the whole core exactly as before". The first review offered 1.1.0 as an option. My probe adds that only users who include Galoot by `-I` and build with `-Wundef -Werror` and misspell the mode are affected. See open item 1. |
| n1, the cache option's comment | `8a7fb13`. | **Resolved.** True for the build presets; see d3. |
| n2, the `!=` case in the guide | `c64ae65`. | **Resolved.** Verified on both compilers. The sentence is long but parses. |
| n3, README wording | `c64ae65`. | **Resolved.** The route compiles. |
| n4, "lint preset" | `8a7fb13`: "lint build preset". | **Resolved.** |
| n5, `rotation(l, theta)` in the rga3 guide | Not changed (optional). | **Reasonably left.** |
| n6, the `product_on_result` comment | `8a7fb13`, with the fixer's own wording (departure 4). | **Resolved.** The wording is exact; verified against `product.hpp`. |
| o1, `tools/mutate` and lint mutants | Reported only. | **Reasonably left.** The same limitation shows in my `is_0_lost` (o1 below). |
| o2, the catalogue's message places | Not changed (optional). | **Reasonably left.** The fix moves no message in `mv.hpp`. Its two-line shift in `assert.hpp` touches no catalogued place: the record's only place in that file is `GALOOT_CHECKING`'s message at `core/assert.hpp:17`, which did not move. |

## Usability checklist (the forced-inline diagnostic)

The tasks, from the examples and the guide only:
1. Turn the forced inlining off for one heavy unit, with `-DGALOOT_FORCE_INLINE=0`.
2. Be told plainly when the value is misspelled (`ON`, empty, `2`).
3. Do both while including Galoot as a system header, or under a strict warning set.

I needed the implementation only to learn the `-Wundef` behavior (p1) and that `1+0` is accepted. The guide covers the rest.

- **Names.** No new public name. The helper macros are absent from a program's macros, by test and by my `-dM`. Met.
- **One error per misuse, message on the first error line.** Met for every documented rejected spelling under no flags, `-Wundef`, and `-isystem` with any flags. Not met under `-I -Wundef -Werror` (p1, which predates the branch and is unchanged by this round).
- **Message states the mistake in the user's terms and gives the fix.** "GALOOT_FORCE_INLINE must be 0 or 1" names the macro and the accepted values. It neither echoes the value nor has a clause after a semicolon. The literal is fixed by plan and test, and this round does not touch it. Observation only, as in the first review.
- **Distinct causes give distinct messages.** Unchanged. Met.
- **Examples compile and are the idiom to copy.** The README route and the guide's `==` sentence are verified. Met.
- **Before release.** This round changes no interface. If p1's pragma form is taken in 1.1.0, it changes only the preprocessed text and the diagnostics of rejected spellings, which nothing a user relies on depends on (inferred).

## Findings, most severe first

No finding blocks the merge. None needs a change before it.

### d1 (nit, confirmed). The 0254 correction and the p1 record omit the `-isystem` case

`docs/agent/decisions/0254-forced-inline-mode-check-in-one-chain.md`, the correction's first item. It says that under `-Wundef` every rejected spelling draws one warning. That holds with `-I`. With `-isystem`, neither GCC 16.2.1 nor Clang 23.1.5 warns, and `-Wundef -Werror` gives the literal alone. Also, for `-1` under `-Werror` the paste error, not the `-Wundef` error, comes first.

Failure scenario: a reader weighing p1 overstates who is affected. A CMake consumer through an installed `find_package` target gets `-isystem` (inferred from CMake's default for imported targets). The README copy route and `add_subdirectory` or `FetchContent` without `SYSTEM` get `-I` (inferred).

Fix: none on this branch. U8, or whoever carries p1 forward, states the `-I` scope.

### d2 (nit, confirmed). The mutation README names the wrong revision for the fix round's tests

`docs/agent/records/mutations/0044-header-hygiene/README.md`, the last section. It says "Tests at `c64ae65`", but the rows are at `108a477`. The two trees differ only in decision 0254, so the evidence stands. Records only, so no change is needed.

### d3 (nit, plausible). "In these builds" holds for the build presets only

`CMakeLists.txt:101-102`. `-ffp-contract=off` comes only from the presets' `CMAKE_CXX_FLAGS`. A top-level configure without a preset, `cmake -S . -B x`, with Clang would contract by default (inferred), so there the mode could change a rounded value. The sentence's antecedent reads as "the build presets" two lines up, so a careful reader gets it right. Optional wording: "in the build presets, which turn multiply-add contraction off". No change needed for 1.0.1.

### d4 (nit, confirmed). The guide's `!=` sentence is long

`docs/guide/core.md:1138-1141` nests three clauses in one sentence. It is accurate. An optional split: "The compiler reports that `==` is deleted; for `a != b` it reports the same, since the language rewrites `a != b` through `==`. A note shows the line of the header that deletes it, which ends with `// use same_rows(a, b)`."

### o1 (note). `tools/mutate` cannot judge a header mutant that breaks a program's build

My `is_0_lost` mutant is killed in fact, because `core_force_inline_off` does not compile. The tool calls it unjudged: the failed object's first error line is an `#error` in `include/`, which it classifies as `invalid`, and the test whose program is missing is not run. This adds a second case to the first review's o1 (lint mutants), the same defect of `tools/mutate`, outside this plan's files. Recommendation: one backlog item for both cases. A mutant of the header that a test-time compile judges, like the other five, is unaffected.

### o2 (note). The review branch holds a merge of the feature tip

`cc98fb5` merges `3aa9208` into `review/header-hygiene-1-0-1`, so that my runs name a revision whose code is the reviewed code. When the review records are cherry-picked into `main` (W7), take this record's commit and skip the merge commit.

## Departures of the fixer, as judged here

1. p1 not changed: a reasonable reading. Confirmed by my pragma probe. See open item 1.
2. No test, expected value or build rule changed: confirmed from the diff.
3. The order around `GALOOT_ALWAYS_INLINE`: accepted (m1).
4. n6's wording: accepted, and exact.
5. n5 and o2 left, m2 and m3 left, o1 reported: accepted.
6. P8 scope of four lint units: matches what the fix can affect. I repeated it.
7. U1 mutants not rerun: I reran them at the tip; all killed.
8. README route not rerun: I reran it; it passes.
9. Configure comparison under load: not rerun, and not needed for a comment change.
10. The `probe - n2-*` rows with exit 1: expected for compile-fail probes.
11. The inferred `system_header` claim: it stays inferred. I did not probe it.
12. Decision 0257: respected.

## Open items for the coordinator

1. **p1 in a later release.**
   - (a) In 1.1.0: the pragma form plus a compile-fail test with `-I -Wundef -Werror` that requires the literal and one error. It costs three lines in `assert.hpp`, a test, a delta review, and an accepted change to the preprocessed text.
   - (b) Leave it as is, documented in the guide as a known limit.
   - (c) Take it in 1.0.1. That reopens this branch, needs a new review, and contradicts DoD 2 as written.

   Recommendation: (a). U8 should list it in the plan's outcome with d1's `-I` scope, so that it is not dropped.
2. **o1 for `tools/mutate`:** one backlog item covering lint mutants and header mutants that break a program's build. Recommendation: add it to plan 0043's follow-ups or the backlog, outside 1.0.1.
3. **Merge readiness.** Nothing in this round blocks U7. The P9 `main` row is still to run at the merge.

## Appendix: the mutant spec

`delta1-assert.toml` (in the reviewer's scratchpad; reproduced here so the run can be repeated):

```toml
presets = ["gcc-debug", "clang-debug"]

[selection.assert]
targets = ["core_assert_mode_0", "core_assert_mode_1",
  "core_force_inline_off", "core_force_inline_default",
  "core_force_inline_build"]
labels = '^core\.assert$'

# helpers_after_undef: remove the four helper #defines, then insert the same
#   four lines after "#undef GALOOT_DETAIL_FORCE_INLINE_IS_1".
# is_0_lost: remove "#define GALOOT_DETAIL_FORCE_INLINE_IS_0 1".
# helpers_in_error_group: remove the four helper #defines, then insert them
#   after the #error line.
# undef_dropped, error_group_undefined, elif_split: as in
#   records/mutations/0044-header-hygiene/u1-mode-check.toml.
```

The probe scripts `probe.sh` and `pp.sh` are in the same scratchpad directory. Each states its inputs in its header comment. The spellings and flags they use are listed under "What I checked" above.
