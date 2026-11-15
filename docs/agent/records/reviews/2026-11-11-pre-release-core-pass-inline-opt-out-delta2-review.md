# Second delta review of the forced-inline opt-out, the fixes after its review: `048aac5..b517977`

- Reviewer: blind-reviewer (Claude Opus 5.5), 2026-11-11. Range `048aac5..b517977` on `feature/pre-release-core-pass--inline-opt-out`, reviewed from a worktree on `review/pre-release-core-pass-inline-opt-out-delta2` at `b517977`, with fresh build trees. The earlier range `54f2487..048aac5` has its review in `3deebff` on `review/pre-release-core-pass-inline-opt-out`, which I read.
- The brief carried no verdict. It listed what the range changes, which I took as the scope to check, not as a claim. I did not read the implementer's report. I read the commit messages of the range, as the procedure's `git log` step requires; their claims were checked, not relied on.
- Scope: the spelling check of `GALOOT_FORCE_INLINE` in `include/galoot/core/assert.hpp`, the compile-fail tests `core.force_inline_on` and `core.force_inline_empty`, the sentences on mixing modes (header comment, README, guide), the twin `rga3.reference_kernels_forced`, clang-measure, the lint suppression, the spec `opt-out-spelling.toml`, plan 0040 sections 17 and 18.2, and decision 0244 item 17.

## Departures, skipped checks, ambiguities

- The role file returns the record to the merging agent; the brief asked me to write and commit it on my review branch, which I did.
- Beyond the brief's run list, I configured (and did not build) gcc-asan, clang-release and clang-measure, through `tools/timed`, to read the compile flags of the reference-kernel programs in those presets, which the brief asks for. I also ran the convention trace gate, which the role file requires for a change to `test/` and `include/`. I listed the selection with `ctest -N` once without `tools/timed`; it ran nothing.
- I compiled single-file probes with g++ 16.2.1 and clang++ 23.1.4 directly, in the scratch directory, outside `tools/timed`: the macro values, the include orders, the preprocessed output of the core against `048aac5`, the two new compile-fail tests against `048aac5`'s header, and a variant of the check. None is a build or test run of a Galoot tree.
- Not run: the lint preset (I read the suppression and the implementer's lint rows), the format check, `tools/mutate` on `opt-out-spelling.toml`, the sanitizer, release and measure builds and their tests (so `rga3.reference_kernels_forced` did not run forced under ASan or at Clang `-O2` in my hands), and the full suites. The gate runner's matrix covers the OFF presets.
- Ambiguity: plan 0040 section 17 says the minors were deferred "as the owner asked"; decision 0244 item 17 says the fixes were made "at the coordinator's request". Decision 0246 item 1 on `develop` records the owner's ruling. See m3.

## What I checked, and how

**1. Valid spellings expand as before.** I preprocessed `<galoot/core.hpp>` and `<galoot/presets/rga3.hpp>` together, from copies of `include/` at `048aac5` and at `b517977` placed at the same relative path, with `GALOOT_FORCE_INLINE` not defined, `-DGALOOT_FORCE_INLINE` (no value, so 1), `=0` and `=1`, on both compilers. The output is byte for byte the same at the two revisions in all eight cases. `-dM` differs only by the four new macros `GALOOT_DETAIL_PASTE_`, `GALOOT_DETAIL_PASTE`, `GALOOT_DETAIL_FORCE_INLINE_IS_0` and `_IS_1`. In mode 1 the output holds 26 `[[gnu::always_inline]]`; in mode 0, none. With `-Wundef -Werror` the valid values give no warning on either compiler, because both pasted names are defined. Nothing under `include/` changed except `assert.hpp`, and `GALOOT_ALWAYS_INLINE` is defined in one place, `assert.hpp:86-90`, by an `#if`/`#else` that defines it on both branches. Each of the six headers that use it includes `assert.hpp`.

**2. Invalid spellings.** One probe stringizes `GALOOT_ALWAYS_INLINE` and stops with its own `#error` if it is undefined. Results, the same on both compilers unless noted:

| Value | Result |
| --- | --- |
| not defined, `-D` with no value, `=1` | mode 1, no diagnostic |
| `=0` | mode 0, no diagnostic |
| `ON`, `OFF`, `true`, `false`, `01`, `0x1`, `2`, `1L`, `1u` | the literal `#error`, one error line |
| empty (`-DGALOOT_FORCE_INLINE=`) | the literal, then a second error from the `#if` at line 86 (GCC: "operator '==' has no left operand"; Clang: "invalid token at start of a preprocessor expression") |
| `-1` | the preprocessor's paste error, one error line |
| `(1)` | the paste error, then "missing binary operator" (Clang: "token is not a valid binary operator") |
| `1.0` | the paste error and two more preprocessor errors |
| a name defined as `ON`, a name defined as empty, a function-like macro's name | the literal (the empty one also draws the second error) |
| a name defined as 1 or 0 (`-DMYONE=1 -DGALOOT_FORCE_INLINE=MYONE`) | accepted, read as that value: the argument is expanded before the paste |
| `1+0`, `1*0`, `1&&0` | accepted with no diagnostic: the paste sees only the first token, so `!1*0` is 0. `1*0` and `1&&0` then give mode 0 (n2) |

`GALOOT_ALWAYS_INLINE` stayed defined after every error in the table (my probe's `#error` never fired).

**3. Include orders and cascades.** Two units, one including `presets/rga3.hpp` before `core/assert.hpp`, the other `core/mv.hpp`, `core.hpp`, `core/assert.hpp`, then `presets/rga3.hpp`, compiled with `-fsyntax-only` and the test warnings: for unset, 0 and 1, no error; for `ON`, exactly one error line, the literal; for empty, exactly two. So no header cascades on an invalid value today, and the include guard means the check and the definition happen once, whatever the order. No test holds this (m1).

**4. The new tests can fail.** I ran each test's own `expect_output.cmake` invocation with the header of `048aac5` in place of the tree's. On both compilers `core.force_inline_on` fails ("result was '0', expected '1'", the old `#if` reads `ON` as 0) and `core.force_inline_empty` fails ("output lacks" the literal). With the tree's header both pass. That header differs from the mutant `spelling_unchecked` of `opt-out-spelling.toml` only by the four unused macros, so the mutant would be killed by both tests on both compilers; this is a probe, not a `tools/mutate` run (m2). The tests (`511323a`) come before the code (`d648ba2`), and the message says which fail (W9). `core.force_inline_on` has `ONE_ERROR`; `core.force_inline_empty` does not, and the registration says why.

**5. The forced twin.** From each tree's `build.ninja`, the reference-kernel programs compile with:

| Preset | `rga3_reference_kernels_test` | `_unforced_test` | `_forced_test` |
| --- | --- | --- | --- |
| gcc-debug | no mode | `=0` | no mode |
| gcc-asan | `=0` | `=0` | no mode, with `-fsanitize=address,undefined -O1` |
| clang-release | `=0` | `=0` | no mode, at `-O2` |
| clang-measure | no mode | `=0` | no mode |

The twin's other flags, include directories and `GALOOT_CHECKING` value are those of `rga3_reference_kernels_test` in every tree I read. It does not link `galoot::core` or `galoot::presets::rga3`, and neither `galoot_reference_kernels`, `galoot_test_support` nor `galoot_doctest_main` links `galoot::core`, so no mode reaches it. It is therefore forced in the OFF presets, and it runs the same test source as `rga3.reference_kernels`, which checks every reference kernel against its rga3 operation exactly over the test rational and bitwise over `double` for a transform. That is a real test of the default code under the sanitizers and at Clang `-O2`, which closes the earlier review's M1. In the forced presets it duplicates `rga3.reference_kernels`. I did not run it in an OFF tree; the implementer's run rows show a direct run (14 of 14 cases) at `84b2d02` in gcc-asan and clang-release, outside CTest.

**6. clang-measure.** Its entry in `CMakePresets.json` at `b517977` is equal, as parsed JSON, to its entry at `54f2487`, before the opt-out. The only presets that differ from `54f2487` are gcc-asan, clang-asan and clang-release. In the clang-measure tree, `rga3_reference_kernels_test` gets no mode. Plan 0040 section 18.2 item 2 and decision 0244's consequence now say so. No guide, README, build or test file names clang-measure as OFF.

**7. Lint suppression (`assert.hpp:42-48`).** `NOLINTBEGIN(*-macro-usage,*-macro-to-enum)` covers the four macro lines only, and the `NOLINTEND` repeats its list, as clang-tidy requires. Among the modules `.clang-tidy` enables, the globs match `cppcoreguidelines-macro-usage`, `cppcoreguidelines-macro-to-enum` and its target `modernize-macro-to-enum`, and nothing else in LLVM 23. The reason given is true: `#if` cannot see an enumerator, and only the preprocessor can test a macro's spelling. Every other suppression under `include/` names exact checks; a list of the three exact names would not fit in 80 columns on one line, which is a fair reason for the globs. The run log shows the lint of `force_inline_test.cpp` failing at `84b2d02` and passing at `51d1079`; I did not rerun it.

**8. Documentation.** The header comment's reason for the paste ("reads a name it does not know, such as ON, as 0, takes 01 and 0x1 for 1, and reports an empty value with an error of its own") matches the old behavior I measured in the first review. The guide's sentence on other values matches the table above for every value a person is likely to write: `ON`, `true`, `2` and empty give the literal, and `-1` gives the preprocessor's error. It is not exact for the edge cases of n2. The ODR sentences are right: two units with different modes give the core's inline functions different token sequences, which the one-definition rule forbids with no diagnostic required, so "must" is the right word. The added lines of `include/`, `test/`, the build files, the README, the guide, plan 0040, decision 0244 and the mutation spec pass `amspell.py convert --check` with no change and no flag. In the added lines of `include/`, `test/`, the build files, the README and the guide, I found no citation of a decision, plan, review, record, provision or agent; the only hit is the word "decision" in "every inlining decision" (S11).

**Usability checklist (public macro, delta only).** Misuse and diagnostics: the likely misuse `=ON` now gives one error, the literal, on the first error line, on both compilers, with a compile-fail test that requires the literal and one error. The empty value gives the literal first and a second error (m1, n2). Distinct causes: a value that does not paste gets a different message, the compiler's own, which names the pasted token; acceptable for a spelling nobody means. Before release: the accepted spellings are now 0, 1, `-D` with no value, and names that expand to one of those, which is the interface to freeze. The four helper macros become visible names in every user's unit (n1).

## Mechanical checks

All through `tools/timed` after `eval "$(tools/jobpool env)"`. The pool read 23 of 23 slots free before my builds; afterwards it read 0 of 23 free, held by another client's ninja (the gate runner), not by me. I did not start or stop it. The rows are in this branch's `runs.tsv`.

- gcc-debug and clang-debug, fresh trees at `b517977`: configure; build `core_assert_mode_0`, `core_assert_mode_1`, `core_force_inline_off`, `_default`, `_build`, `rga3_reference_kernels_test`, `_unforced_test` and `_forced_test`; then `ctest -R 'core\.force_inline|core\.assert_|rga3\.reference_kernels|rga3\.kernels_inline'`: 16 of 16 pass on each, exit 0. The 16 are `core.assert_checking`, `_unchecked`, `_consteval`, `_consteval_wrong_message`, `_checking_value`, the six `core.force_inline_*`, the three `rga3.reference_kernels*`, and `rga3.kernels_inline` and `_alone`.
- gcc-asan, clang-release and clang-measure: configure only, exit 0, to read compile flags.
- Convention trace gate: exit 0 ("items with no test: 6.1, 6.2, 6.3, 6.4", as before).
- `tools/runlog check` on this branch's run log, after my rows: exit 0.

These are the first CTest and Ninja runs of the range: the implementer's rows at `84b2d02` and `51d1079` are direct compiler runs logged as step `probe`, because the pool was not running for that session.

## Findings

No blocker.

- **m1 (minor), `test/core/force_inline_on_fail.cpp`, `force_inline_empty_fail.cpp`, `test/CMakeLists.txt:137-150`.** Decision 0244 item 17 keeps the empty value's second error because folding the check into the later `#if` "would leave `GALOOT_ALWAYS_INLINE` undefined after the error and give a cascade in every header". Nothing tests that there is no cascade. Both new tests include only `assert.hpp`, which does not use `GALOOT_ALWAYS_INLINE`, so a cascade cannot show there. `ONE_ERROR` on `core.force_inline_on` holds the error count only for that one header. Failure scenario: a later edit moves the `#define GALOOT_ALWAYS_INLINE` lines into an `#elif` after the `#error`, or otherwise leaves the macro undefined after an invalid value; `-DGALOOT_FORCE_INLINE=ON` then gives the literal followed by hundreds of errors from `mv.hpp`, `product.hpp` and the rest, and every test passes. Fix, test-only: have `force_inline_on_fail.cpp` include `<galoot/core.hpp>` after `assert.hpp`, so that `ONE_ERROR` holds over the whole core (one more second of compile, inferred, not measured). Optional, in the header: test the mode in the second `#if` by pasting as well (`GALOOT_DETAIL_FORCE_INLINE_ON_0` 0, `_ON_1` 1). I probed this: the empty value then gives one error on both compilers, valid values preprocess to the same output, but `-1` gets two paste errors instead of one. That trade is the owner's; the test-only fix is enough.
- **m2 (minor), `docs/agent/records/mutations/0040-pre-release-core-pass/opt-out-spelling.toml`, its `README.md`.** The spec is committed but not run, and the mutation record's README does not mention it, so the record of mutants for plan 0040 is incomplete. My probe in check 4 shows that both killers fail against the mutant's behavior on both compilers, so the expected verdict "killed" is right. Fix: run `tools/mutate` on it on the two debug presets (five targets, seconds) and add a paragraph to the README, or say in the README that the probe stands in for the run.
- **m3 (minor, records), plan 0040 section 17 (`0040-pre-release-core-pass.md:301`), decision 0244 item 17.** The plan says the deferral of m2, m5 and m6 is "as the owner asked"; decision 0244 item 17 says the fixes were made "at the coordinator's request". Decision 0246 item 1 on `develop` is the owner's ruling for both: the scope (strict value check, one forced twin, measure presets forced) and the deferral of the remaining minors to plan 0040 section 17. Neither record cites it. Failure scenario: an auditor reading the branch alone finds a change under `include/` with no recorded owner ruling (C18). Fix: cite decision 0246 item 1 in both places once `develop` is merged into the branch. This also settles the note on decision 0245 in the first review.
- **n1 (nit, before release), `assert.hpp:44-47`.** The four helper macros are used on one line and stay defined in every unit that includes Galoot. `GALOOT_DETAIL_PASTE` in particular reads as a general utility a user might start to use. `#undef`ing the four after the check would keep them out of the interface; nothing under `include/` uses `#undef` today, and `GALOOT_ALWAYS_INLINE` is left defined by design and documented as not part of the interface. Low risk either way. The `GALOOT_DETAIL_` prefix says "internal".
- **n2 (nit), `docs/guide/core.md:1043-1047`.** "Any other value stops the compile" holds for every likely value but not for a name defined as 0 or 1 (accepted as that value) or an expression whose first token is 0 or 1 (`1+0` accepted as 1, `1*0` as 0, with no diagnostic). The guide also does not say that `-DGALOOT_FORCE_INLINE` with no value is 1, while `-DGALOOT_FORCE_INLINE=` is the empty value that stops the compile, or that the empty value draws a second error. The sentence is also long for what it says. A shorter form: "`-DGALOOT_FORCE_INLINE` with no value means 1. A word, another number such as `2` or `01`, or an empty value (`-DGALOOT_FORCE_INLINE=`) stops the compile with "GALOOT_FORCE_INLINE must be 0 or 1"; a spelling such as `-1` stops it with an error of the preprocessor's own." The expression cases need no text.
- **Note.** In the forced presets, `rga3.reference_kernels_forced` builds the same program as `rga3.reference_kernels`, and in the OFF presets `rga3.reference_kernels` and `_unforced` do, as the first review found. That is about a second of compile per program and the accepted price of one target definition for every preset, not a defect.

## Open owner items

1. **The cascade guard (m1).** (a) The test-only fix: `force_inline_on_fail.cpp` includes the whole core, so `ONE_ERROR` holds over every header. It costs about a second of compile per debug tree and touches no core code, so it needs no further delta review. (b) Also make the empty value give one error by pasting in the second `#if`. That is a change under `include/` and needs another delta review, and `-1` then gets two errors. (c) Defer it with the other minors. Recommended: (a) now, (b) not at all.
2. **The helper macros (n1).** (a) `#undef` the four after the check: four lines under `include/` and a delta review. (b) Leave them, relying on the `GALOOT_DETAIL_` prefix. Recommended: (b) before the release, given the ruling for the least change, and (a) with the next change to `assert.hpp`.
3. **Records (m2, m3).** No trade-off of substance: run the five-target mutation and cite decision 0246 item 1 when `develop` is merged in. Recommended: both, before the merge into `develop`.
4. **Still deferred by decision 0246 item 1, unchanged here:** the first review's m2 (cache option not validated), m5 (no mutant of the CMake propagation on an OFF tree) and m6 (the unexplained `convention-trace` exit at `81c923136055`). I have nothing new on them.

## Outcome

Approve with non-blocking findings. For 0, 1, no definition and `-D` with no value, the core preprocesses to exactly what it did at `048aac5` on both compilers. Every likely invalid spelling, `ON` and the empty value among them, now stops the compile with the literal message, and no invalid value leaves `GALOOT_ALWAYS_INLINE` undefined or makes a header cascade. The new tests fail against the old header and pass against the new. The twin is forced in the OFF presets by construction, as their compile flags show, and runs a real value test there. clang-measure is exactly as it was before the opt-out. The suppression is narrow and its reason holds. The findings are a missing guard on the cascade that a decision relies on, a mutation spec left unrun, records that do not cite the owner's ruling, and two nits.

Tip reviewed: `b517977`.

## Resolution

- **m1** (nothing guards against a cascade of errors): fixed; the compile-fail test now includes the whole core.
- **m2** (mutation spec not run): fixed; the spec ran on gcc-debug and clang-debug, both mutants killed, and the mutation README describes it.
- **m3** (owner's ruling not cited): fixed; plan 0040 section 17 and decision 0244 item 17 cite decision 0246 item 1.
- **n1** (helper macros stay defined): left for the next change to `assert.hpp`.
- **n2** (guide wording): deferred to the post-1.0 backlog.

