# 0044 Release 1.0.1: public header and interface hygiene

- Status: proposed 2026-11-16 by Claude (agent, Opus 5.5), planner in the cpp-expert and hci-expert roles. From `main` (the integration branch, formerly `develop`) at `c95fee5`. Committed on `plan/0044` in the worktree `galoot-wt/plan-0044`; the claim (W3, W7) takes effect when the coordinator merges this plan into `main`. No unit below has started.
- Source of scope: the coordinator's brief for release 1.0.1 (cleanup only, no new feature, no change to a public name or behavior that would force 2.0.0); the known friction of `records/release/release-record.md` (items 4, 5, 10, 11, 12, 13); plan 0040 section 17 (review minors m2, m5 of the opt-out review; n1, n2 of its second delta review; m5 of the blind review, the `==` hint); plan 0042 section 9; decisions 0241 item 1 (the two untargeted `apply` overloads, to delete after 1.0), 0248 item 9 (the six helpers stay public), 0250 item 7 (the README's copy route), 0215 item 11 (wording).
- Siblings: plan 0043 (test, build and tool cost) and plan 0045 (the 1.0.1 release procedure). Neither existed at `c95fee5`; section 10 lists the files this plan expects to share with them, inferred from their titles.
- Machine at planning (2026-11-16, 04:38 to 04:48 EST): 32 cores, 125 GB, 105 GB available; load average 2.3 rising to 7.2 while other sessions worked (an unrelated `rq5_driver.py` session holds two cores; not touched). Job pool: running, 23 of 23 slots free. The planner's probes were three clang-tidy runs and a few single-file compiles, outside the pool, each under `tools/timed --log` into the scratchpad; no build tree was touched.
- Next free decision number at `c95fee5`: 0252.

## 1. Departures, ambiguities, and readings (C20)

1. **"Verify no test or doc uses them" (item d) does not hold.** The two untargeted `apply` overloads are called from 28 lines of `test/core/mv_test.cpp` and from four compile-fail sources (`mv_apply_inexact_{float,double,short}_fail.cpp`, `mv_get_absent_ordered_fail.cpp`), as the M7 audit found. No user documentation and no header under `include/` calls them. So unit U4 rewrites those tests (W9: committed apart and listed for review). No public name goes: both are in `galoot::core::detail` (`mv.hpp:720` to `:1022`). The line numbers have not drifted: the binary form is `mv.hpp:928-936`, the unary form `:969-974`.
2. **"Model" to "preset" (item g) finds nothing to change in user documentation.** `README.md` and `docs/guide/` contain no "model" at all (`grep -rni model`). The three hits under `include/` and `test/` are other senses and stay (section 3.7). Decision 0215 item 11 names a second, opposite wording issue: "preset" used for a CMake configuration, which now reads as an algebra preset. Three user-facing places use it that way, and the plan fixes them. The remaining "model" uses are in ratified governance (CONSTITUTION, STYLE, VVUQ, CONVENTIONS), which only the owner edits (O4).
3. **"`#undef` at the end of each header that defines them" (item a).** Only `assert.hpp` defines the four macros, and only line 49 uses them. The plan puts the `#undef` lines straight after the check's `#endif`, not at the end of the file. The effect on users is the same, and the definitions, their one use and their removal then sit in one block.
4. **The `==` hint note (item f).** C++26's `= delete("reason")` would put the hint on the error line. STYLE S2 allows a later edition's feature only in an optional header that the default headers never include. So using it in `basis_change.hpp`, even behind `__cpp_deleted_function`, is a departure only the owner can rule (O3). The plan's default is a guide note plus a `cpp rejected` block that pins the hint. Replacing the deleted `==` with a template that fails a `static_assert` is ruled out: `std::equality_comparable<basis_change<N>>` would become true (`basis_change_test.cpp:187` pins it false), and that is a change of behavior.
5. **P8 and P9 for headers that every unit includes.** `assert.hpp`, `bits.hpp` and `mv.hpp` are included, directly or through others, by every test of the core and of rga3. So P8's selection for each header unit is the whole suite. Reading followed: the header units commit one after another on one feature branch, each running the whole suite in one configuration before its commits (P9, first row), plus the named targeted tests in a tree with the forced inlining off. The full P9 `main` row runs once, at the merge (section 9). No subfeature branches for the header units, because a merge into a feature branch would ask for the P8 selection in every configuration the change can affect, which here is all eleven presets, four times over.
6. **The claim.** The brief asks for this plan to be committed on `plan/0044` only. W7 claims a unit by committing its plan to the integration branch, so the claim is the coordinator's merge.
7. **Confusable names beyond `l` and `m` (item c).** The scan of section 3.3 also finds `q1` (the parameter of `compose`, `transform.hpp:654`) against a user's `ql` or `qI`, and the template parameter `All` (`transform.hpp:320`) against spellings with digits (`A11`, `AI1`). The plan keeps both. `q1` and `q2` are the notation `compose(q2, q1)` documents, and neither `ql` nor a name with digits is a name a user plausibly writes. U3's reviewer may disagree; renaming a parameter changes no interface.
8. **Checks of this commit.** Records only (P8: documentation, no tests). No build, test or lint run was made for the plan; the probes in section 3 are evidence for the plan, not checks of a revision.

## 2. Goal and definition of done

Remove six known-friction items of the 1.0.0 release record without changing any public name, any accepted value of a public macro, or any value or type a public operation computes. Done when:

1. The four `GALOOT_DETAIL_*` macros are not defined after including any Galoot header, in any order, and a test fails if one is left defined.
2. An empty `GALOOT_FORCE_INLINE` gives exactly one error, the literal "GALOOT_FORCE_INLINE must be 0 or 1", on both compilers, and `core.force_inline_empty` asks for one error. The valid spellings (unset, `-D` with no value, `0`, `1`) preprocess the whole core exactly as before.
3. The CMake cache option `GALOOT_FORCE_INLINE` rejects any value but `ON` or `OFF` with a message, as `GALOOT_CHECKING` does; a mutant of the option's propagation has run on a tree with the forced inlining off, and its verdict is recorded.
4. No Galoot header declares a name that clang-tidy's `misc-confusable-identifiers` reports against a user's `I` or `rn` at namespace scope, and a lint unit fails if one comes back.
5. The two untargeted `apply` overloads are gone, every test that called them names a target, and every compile-fail test keeps its message and its error count.
6. The README's copy route no longer brings doctest with it; the CMake routes are unchanged.
7. The guide states the edge spellings of `GALOOT_FORCE_INLINE` (n2) and where the deleted `==`'s hint appears, the second in a documentation test.
8. "Preset" no longer names a CMake configuration in user-facing text without "build" before it.
9. The full suite passes in the P9 `main` configurations at the merge; one blind review approves the branch; the outcome is recorded here and fed to plan 0045's release record.

## 3. What the tree holds now (verified at `c95fee5`)

### 3.1 The helper macros (`include/galoot/core/assert.hpp`)

- Lines 44 to 47 define `GALOOT_DETAIL_PASTE_`, `GALOOT_DETAIL_PASTE`, `GALOOT_DETAIL_FORCE_INLINE_IS_0` and `_IS_1`; line 49 is their only use. No `#undef` exists anywhere under `include/galoot/` (`grep -rl '#undef'` finds only doctest).
- No other file names them: `grep -rn GALOOT_DETAIL test tools docs/guide README.md CMakeLists.txt` is empty. Only the mutation records of plan 0040 anchor on line 49's text.
- Every header that uses a Galoot macro (`GALOOT_ASSERT`, `GALOOT_CHECKING`, `GALOOT_ALWAYS_INLINE`) includes `assert.hpp` directly: all twelve users, checked file by file. So no header relies on a macro another header defines, and since `assert.hpp` is guarded, the definitions, the check and the removal happen once per unit, whatever the include order. [read]
- `GALOOT_ALWAYS_INLINE` must stay defined: six headers use it after including `assert.hpp`, and `test/core/force_inline_test.cpp` reads its expansion. The release record (line 201) lists it and the `GALOOT_DETAIL_*` macros as not interface.

### 3.2 The empty value

- With `-DGALOOT_FORCE_INLINE=`, line 49's paste gives `GALOOT_DETAIL_FORCE_INLINE_IS_`, read as 0, so the `#error` fires; then line 86's `#if GALOOT_FORCE_INLINE == 1 && ...` has no left operand and gives the preprocessor's own error. `core.force_inline_empty` therefore does not ask for one error (`test/CMakeLists.txt:144-151`).
- Probe [checked-self, GCC 16.2.1 and Clang 23.1.4, `-std=c++23 -Wall -Wextra -Wundef`]: making the definition an `#elif` of the check, with the error group defining `GALOOT_ALWAYS_INLINE` empty, gives one error for the empty value and for `ON` and `2`, one error (the paste error) for `-1`, no diagnostic for unset, `0` and `1`, and `GALOOT_ALWAYS_INLINE` defined after every error, so no header cascades:

  ```text
  #if !GALOOT_DETAIL_PASTE(GALOOT_DETAIL_FORCE_INLINE_IS_, GALOOT_FORCE_INLINE)
  #error "GALOOT_FORCE_INLINE must be 0 or 1"
  #define GALOOT_ALWAYS_INLINE
  #elif GALOOT_FORCE_INLINE == 1 && (defined(__GNUC__) || defined(__clang__))
  #define GALOOT_ALWAYS_INLINE [[gnu::always_inline]]
  #else
  #define GALOOT_ALWAYS_INLINE
  #endif
  #undef GALOOT_DETAIL_PASTE
  ...
  ```

  Neither compiler evaluates the `#elif` once the first group is taken. That the standard requires this ([cpp.cond]: the groups after the first taken one are skipped, and their directives are processed as in a skipped group) is recalled, not read; U1 cites the clause from N4950 after reading it.
- This answers the reason decision 0244 item 17 gave for keeping two errors (folding the check into the later `#if` "would leave `GALOOT_ALWAYS_INLINE` undefined after the error"), so U1 records a decision that supersedes that choice (decision 0248 item 3: supersession by a new decision).
- The `///` comment block before line 86 sits between the check and the definition; U1 moves it so that it still precedes the definition it documents.

### 3.3 Confusable names

A clang-tidy 23.1.4 scan with only `misc-confusable-identifiers` and the repository's header filter, over one unit that includes `core.hpp`, `presets/rga3.hpp` and `core/format.hpp` and declares at namespace scope every one- and two-character identifier and a set of three-character ones built from `l`, `I`, `1`, `O`, `0`, `r`, `s`, `n`, `x` (5,457 names; each `int <name> = 0;`), reports these header names [checked-self]:

| Header name | Where | User names it clashes with | Plan |
| --- | --- | --- | --- |
| `l` | `basis_change.hpp:542` (and the loop at `:556`) | `I` | rename (U3) |
| `m` | `bits.hpp:171` | `rn` | rename (U3) |
| `q1` | `transform.hpp:654`, `compose`'s parameter | `ql`, `qI` | keep (section 1, item 7) |
| `All` | `transform.hpp:320`, a template parameter | `A11`, `AI1` and other digit spellings | keep |

A wider scan of 1,856 one-substitution variants of every identifier in the headers (`sum` against `surn`, `file` against `fiIe`, and so on) finds only names no user writes. As the second delta review of plan 0040 found, clang-tidy reports only clashes with names visible at namespace scope; user locals and parameters do not fire. `I` matters most: a geometric algebra user names the pseudoscalar `I`.

### 3.4 The two untargeted `apply` overloads (`include/galoot/core/mv.hpp`)

- Binary form `:928-936` (comment `:928-929`, declaration `:930-936`), unary form `:969-974` (comment `:969`); each forwards to the targeted overload with the plan's own result set. Both in `namespace galoot::core::detail`. [read]
- Callers under `include/`: none; every `detail::apply` call there names a target (`product.hpp:241`, `:269`, `:359`, `:391`; `unary.hpp:72` to `:110`; `transform.hpp:670`). Callers in tests: 28 lines of `mv_test.cpp` (including the generic helpers at `:1098` and `:1123`) and the four compile-fail sources of section 1, item 1. Callers in `docs/guide/` and `README.md`: none. The `apply<C, Back>` and `apply<N>` of the basis-change tests are those tests' own functions.
- The four compile-fail tests: `core.mv_apply_inexact_{float,double,short}` ask for `ONE_ERROR` and the message "apply: the scalar type cannot hold one of the product's exact coefficients; use a scalar type that holds larger integers exactly"; `core.mv_get_absent_ordered` asks for the `get` message.

### 3.5 The README's copy route

`README.md:40-46` says "Copy or vendor the `include/` directory", which brings `include/ext/doctest/` (`doctest.h`, `doctest.cpp`, `LICENSE.txt`). The core includes none of it. The `text` block there is not compiled by the documentation tests. The two CMake routes use `FILE_SET HEADERS BASE_DIRS include` (`CMakeLists.txt:16-48`), so a consumer's include path also holds `include/ext` (reachable only as `<ext/doctest/doctest.h>`; inferred harmless). The vendored path is named in STYLE S3, PERFORMANCE P8's table, decisions 0025, 0026 and 0081, `test/CMakeLists.txt:12-19` and `CMakeLists.txt:151`; `format-check` globs `test/*.cpp`, so a move under `test/` would sweep `doctest.cpp` into it.

### 3.6 The cache option and the OFF trees

- `CMakeLists.txt:105-109`: `option(GALOOT_FORCE_INLINE ... ON)` with no validation, so `2` or `foo` reads as ON through `$<BOOL:...>`. `GALOOT_CHECKING` is checked against `^(ON|OFF)$` (`:83-85`). Neither check has a test (`grep "must be ON or OFF" test tools` is empty). Both sit after `if(NOT PROJECT_IS_TOP_LEVEL) return()` (`:50-51`), so only Galoot's own builds see them; a consumer's `add_subdirectory` or `FetchContent` never does.
- Presets that set it: gcc-asan, clang-release, clang-asan (`OFF`); the other eight leave the default.
- m5: the propagation (`:108-109`) was never mutated on an OFF tree, so only `core.force_inline_build` in an OFF tree can kill its loss.

### 3.7 Wording

- "model" in user-facing files: `dual.hpp:57-58` ("the conformal model", the field's name for that geometry), `transform.hpp:461` (Higham's rounding model), `test/CMakeLists.txt:2383` ("the conformal model"). None is the old sense of a preset; all stay.
- "preset" for a CMake configuration in user-facing text: `assert.hpp:9` ("in every preset"), `docs/guide/limits.md:409` ("Galoot's presets build for baseline x86-64"), `CMakeLists.txt:99` ("for the presets whose units"). The other uses in `CMakeLists.txt` already say "build preset".
- Governance with "model" in the old sense, owner only: CONSTITUTION (Terms, C5), STYLE (S1 table, S10 table), VVUQ (line 38, "models" in the order of verification), CONVENTIONS (lines 14, 68, 88, the *model* component markers). "Preset" for a CMake configuration also runs through `tools/` and `.claude/agents/` (91 lines), which decision 0215 item 11 leaves to the next governance pass.

### 3.8 The guide's text on `GALOOT_FORCE_INLINE` and `==`

- `docs/guide/core.md:1042-1052` (n2): the sentence on other values is long, does not say that `-DGALOOT_FORCE_INLINE` with no value means 1 while `-DGALOOT_FORCE_INLINE=` is the empty value, and (after U1) must not imply a second error.
- `docs/guide/core.md:1132-1134` says a `basis_change` has no `==` and names `same_rows`; nothing tells the reader where the compiler shows the hint. `basis_change.hpp:177` holds `= delete; // use same_rows(a, b)`, and `core.basis_change_equality` and `_inequality` pin "// use same_rows(a, b)" in the output on both compilers.

## 4. Acceptance criteria

Each unit's criteria are in section 6. Across the branch:

- `git diff c95fee5 -- include/` shows only: the `#elif` restructure and four `#undef` lines in `assert.hpp`, its comment moves and the "build preset" word; two renamed locals with their comment words; the two deleted overloads with their comments.
- The preprocessed comparison of section 7 shows no other change for valid modes.
- No test is weakened (W9): every changed test asks for at least what it asked before; `core.force_inline_empty` asks for more.
- No added text cites a decision, plan, review or agent (S11).
- The P9 `main` checks pass at the merge revision.

## 5. Units

| # | Unit | Role, model | Branch | Blind review |
| --- | --- | --- | --- | --- |
| U1 | Helper macros and the empty value: `#elif` restructure, `#undef`, the macro-set test, `ONE_ERROR` on the empty test, guide n2, the decision superseding 0244 item 17's choice | implementer, cpp-expert, Opus | `feature/header-hygiene-1-0-1` | yes (public diagnostics; usability checklist) |
| U2 | The cache option: validation, the mode declared in every preset, configure-time tests for both options, the m5 mutant on an OFF tree, the "build presets" word in `CMakeLists.txt` | implementer, build-expert, Sonnet | same branch, after U1 | yes |
| U3 | Confusable names: rename `l` and `m`, a lint unit naming `I` and `rn` | implementer, cpp-expert, Sonnet | same branch, after U2 | yes |
| U4 | Delete the two `apply` overloads; rewrite the 28 test lines and four compile-fail sources | implementer, doctest-expert, Opus | same branch, after U3 | yes |
| U5 | User documentation: the README's copy route, the `==` hint note and its rejected block, `limits.md:409` | implementer, hci-expert, Sonnet | `feature/header-hygiene-1-0-1--docs`, merged into the feature branch | yes (with the branch) |
| U6 | Blind review of the whole branch, then a delta review of any fix | blind-reviewer, Opus | `review/header-hygiene-1-0-1` | (is the review) |
| U7 | Merge gate: `main` merged in, the P9 `main` row, W10 rows, merge | performance-engineer, Opus | the feature branch | none beyond U6 |
| U8 | Records: this plan's outcome, the friction items closed, the text plan 0045's release record needs | coordinator or Sonnet | `records/header-hygiene-1-0-1` | records only (decision 0215) |

## 6. Unit detail

### U1. The helper macros and the empty value

- Files: `include/galoot/core/assert.hpp`; `test/CMakeLists.txt` (the `core.force_inline_*` section, `:101-151`, and one new test); `test/core/` (one new source); `docs/guide/core.md:1042-1052`; a new decision `docs/agent/decisions/0252-*.md`.
- Change: section 3.2's form, with the `#undef` of the four macros straight after the `#endif`; the `///` block of `GALOOT_ALWAYS_INLINE` kept directly before its definition; `assert.hpp:9` "in every preset" to "in every build preset". The guide's sentence rewritten along the lines of the review's n2 proposal: "`-DGALOOT_FORCE_INLINE` with no value means 1. A word, another number such as `2` or `01`, or an empty value (`-DGALOOT_FORCE_INLINE=`) stops the compile with "GALOOT_FORCE_INLINE must be 0 or 1"; a spelling such as `-1` stops it with an error of the preprocessor's own." U1 checks each clause against the compilers before writing it.
- Tests first (W2), in a commit that names the tests it leaves failing:
  - `core.force_inline_empty` gains `ONE_ERROR`; its registration comment loses the sentence about a second error. Fails at `c95fee5`.
  - A new test, for example `core.public_macros`: `expect_output 0 ONLY "^#define GALOOT_"` with the allowed lines as literals (the include guards, by their `_HPP ` suffix; `GALOOT_CHECKING `, `GALOOT_FORCE_INLINE `, `GALOOT_ALWAYS_INLINE`, `GALOOT_ASSERT(`), over `<compiler> -std=c++23 -I include -dM -E` of a unit that includes `format.hpp`, `presets/rga3.hpp` and `core.hpp`, in that order, then `assert.hpp` again. It names exactly the macro set a user sees, so a future helper left defined fails it too. Labels `core;core.assert;core.headers`. Fails at `c95fee5` (the four macros). Preprocessing only: about a second (inferred; U1 records it).
  - `core.force_inline_on` already includes the whole core with `ONE_ERROR`, so it holds the absence of a cascade under the new form.
- Mutants (W3), a new spec under `records/mutations/0044-header-hygiene/`, on gcc-debug and clang-debug: one `#undef` dropped (killer `core.public_macros`); the error group's `#define GALOOT_ALWAYS_INLINE` dropped (killer `core.force_inline_on`, by the cascade); the `#elif` turned back into a separate `#if` (killer `core.force_inline_empty`). Plan 0040's specs anchor on the old text and stay as history.
- P8: the whole suite in gcc-debug (every header includes `assert.hpp`); the `core.force_inline_*` and `core.public_macros` tests also in clang-debug and in one OFF tree (clang-release).
- Behavior preserved: section 7's comparison for the four valid spellings; `-dM` differs only by the four macros' absence.
- Decision 0252 (or the next free number): the empty value now gives one error; supersedes the reason 0244 item 17 gave; records the alternatives the second delta review of plan 0040 weighed (pasting in the second `#if`, which gives `-1` two paste errors) and why the `#elif` keeps `-1` at one.

### U2. The cache option

- Files: `CMakeLists.txt:97-109`; `CMakePresets.json` (eight configure presets gain `"GALOOT_FORCE_INLINE": "ON"`, or the shared hidden presets carry it; U2 picks the form that leaves every preset's mode readable in one place); `test/CMakeLists.txt` (two new tests); `records/mutations/0044-header-hygiene/`.
- Change: after the `option()`, `if(NOT GALOOT_FORCE_INLINE MATCHES "^(ON|OFF)$") message(FATAL_ERROR "GALOOT_FORCE_INLINE must be ON or OFF, not '${GALOOT_FORCE_INLINE}'.")`, the form `GALOOT_CHECKING` uses (O1 for the accepted spellings). The comment's claim "it changes no value a test checks" is corrected (`core.force_inline_build` checks one). `CMakeLists.txt:99` "the presets" to "the build presets".
- Tests: `tools.configure_rejects_force_inline` and `tools.configure_rejects_checking`, each `expect_output 1 "<message>" -- cmake -S <source> -B <scratch under the build tree> -G Ninja -DGALOOT_CHECKING=... -DGALOOT_FORCE_INLINE=2` (and `-DGALOOT_CHECKING=2`), labelled `tools;tools.configure`. Configure stops at the check, after compiler detection: a few seconds each (inferred; U2 records it, and removes the scratch tree in the test). The test of `GALOOT_CHECKING` closes a gap of the same mechanism at the same cost; it judges no new code, so its mutant is the deleted `GALOOT_CHECKING` check, expected killed.
- m5: a mutant of `CMakeLists.txt:108-109` (the definition dropped), run with `tools/mutate --preset clang-release` (an OFF tree), selection `core_force_inline_build`, expected killed by `core.force_inline_build`. If `tools/mutate` does not regenerate a tree after a `CMakeLists.txt` edit, U2 runs the mutant by hand under `tools/timed`, records why, and reports it as a departure. The validation's own mutant (the `FATAL_ERROR` check deleted) is killed by `tools.configure_rejects_force_inline`.
- P8: "the build description": every preset configures (eleven `cmake --preset` runs on existing trees, seconds each); the new tests; `core.force_inline_*` in one ON and one OFF tree. The full suite waits for U7 (reading in section 1, item 5).
- Behavior preserved: every preset's cache value of `GALOOT_FORCE_INLINE` and its compile commands (`compile_commands.json`) are the same before and after; U2 diffs them.

### U3. Confusable names

- Files: `include/galoot/core/basis_change.hpp:542-560`, `include/galoot/core/bits.hpp:163-172`; a new `test/core/user_names.cpp` (or a second unit beside `linear_user_names.cpp`; the two cannot share one file, since a user's `l` and `I` clash with each other) and its registration as an OBJECT library in `test/CMakeLists.txt`.
- Change: `l` to a letter the scan does not flag (the column index, for example `c`), in both loops of `keeps_inner_products`; `m` to `k` in `antireverse_sign`, with its `///` text "(-1)^(m(m-1)/2) for its antigrade m" changed to match, as `reverse_sign` above it reads. U3 reruns the scan of section 3.3 on the result.
- Test first: the lint unit includes `core.hpp` and `presets/rga3.hpp` and declares `I` (a pseudoscalar) and `rn` at namespace scope, used, as `linear_user_names.cpp` does. Under the lint preset it fails at `c95fee5` (clang-tidy, `'l' is confusable with 'I'`), shown by running clang-tidy with the repository's `.clang-tidy` on it against the old headers.
- Mutant: the old name restored in one place; killer: the lint preset's build of the unit (a lint error is a build failure, so `tools/mutate` judges it by the object's first error line; U3 confirms the classification on one run).
- P8: the whole suite in gcc-debug (`bits.hpp` is everywhere), and the lint preset.
- Behavior preserved: a local's name is not part of any interface or symbol; section 7's comparison shows only the renamed tokens; `kernels_inline.cpp`'s disassembly is unchanged.

### U4. The `apply` overloads

- Files: `include/galoot/core/mv.hpp:928-936`, `:969-974`; `test/core/mv_test.cpp`; `test/core/mv_apply_inexact_{float,double,short}_fail.cpp`; `test/core/mv_get_absent_ordered_fail.cpp`.
- Tests first, committed apart (W9) and listed for review: each call names its target. In `mv_test.cpp` either the explicit form `apply<Op, product_plan<Op, Alg, A, B>.result>(x, y)` or, where 28 lines would repeat it, two short test-local functions next to the generic helpers at `:1098` and `:1123` that state what they compute; U4 chooses by readability, and the review judges it. The four compile-fail sources name the target their comment describes. Their messages, `ONE_ERROR` and the `FORBID_FIRST` sets are unchanged, and each still fails at the same `static_assert`. This commit passes before the deletion too, so it shows the rewritten tests still judge the targeted overload.
- Change: the two overloads and their `///` comments deleted; nothing else in `mv.hpp` moves.
- Evidence: the build of the whole suite is the proof that nothing else calls them; `tests.messages_pinned` and `tools/catalogue`'s check pass (the messages' places move; the catalogue's record is regenerated if its check asks). No mutant: a deletion of a forwarder has no behavior to kill.
- P8: the whole suite in gcc-debug; the compile-fail tests of `mv` and the catalogue tests in clang-debug too.

### U5. User documentation

- Files: `README.md:38-46`; `docs/guide/core.md:1132-1134`; `docs/guide/limits.md:409`.
- README (O2, default wording): "Copy or vendor the `include/galoot` directory into a directory on your include path, and compile as C++23", with the command naming that directory. U5 checks the route once: `include/galoot` copied alone into a scratch directory, README example 1 compiled from it on both compilers. The two CMake blocks are not touched; their documentation tests rerun.
- Guide: one or two sentences after `core.md:1134`: `a == b` does not compile, the compiler reports a use of a deleted function, and the line it shows from the header ends with `// use same_rows(a, b)`. Then a `cpp rejected` block whose `// rejected:` line names "use same_rows(a, b)", so a documentation test fails if the hint is lost. U5 confirms on both compilers that the literal comes from the header's echoed line and not from the block itself, by a run with the header's comment removed.
- `limits.md:409`: "Galoot's presets build" to "Galoot's build presets build" or "Galoot's test builds are built"; U5 picks the plainer.
- P8: the documentation tests of the two pages and the README (`tests.docs_pages`, the doc-example tests, `tests.citations_guide`) in gcc-debug and clang-debug.

### U6 to U8

- U6 reviews `c95fee5..<branch tip>` blind (W5): the diff, section 2 and 4's criteria, and each implementer's departures; the usability checklist for the macro's diagnostics, the cache option's message and the guide text. A fix under `include/` gets a delta review.
- U7 merges `main` into the branch and runs section 9, then merges into `main` with the P13 line in the message.
- U8 writes the outcome here, the agent cost rows, and a short list for plan 0045: which release-record friction items 1.0.1 closes (4, 5, 10, 11, 13; item 12 narrowed, with O3's ruling) and that no public interface is removed or changed (W13).

## 7. How behavior is shown unchanged

1. **Preprocessed comparison** (U1, U3, U4, then once over the branch by U7): one unit including `core.hpp`, `presets/rga3.hpp` and `core/format.hpp`, `-E -P`, on both compilers, `GALOOT_CHECKING` 0 and 1, `GALOOT_FORCE_INLINE` unset, `-D` with no value, `0` and `1`: sixteen outputs, each diffed against `c95fee5`'s. Allowed differences: the deleted overloads, the renamed tokens, and the integer `__LINE__` arguments of `GALOOT_ASSERT` expansions below a deletion (normalized before the diff, and the normalization stated). `-dM` of the same units: only the four helper macros missing.
2. **Code generation**: `test/bench/kernels_inline.cpp` compiled at `-O2` with the structure test's flags at `c95fee5` and at the tip, both compilers; the disassembly (`objdump -d --no-show-raw-insn`) is identical. `rga3.kernels_inline` and `_alone` pass in every preset of U7.
3. **Compile-fail lane**: every `*_fail` test passes with its message; `core.force_inline_empty` now with `ONE_ERROR`; `tests.messages_pinned` and the catalogue check pass; `tests.compile_fail_flags` passes.
4. **Documentation tests**: every `cpp`, `cpp rejected` and `cmake` block of the README and the guide, in every preset of U7.
5. **Include scan and header checks**: `core.include_scan`, the header-set units (`all_verify_interface_header_sets`), `core.bare_compile` and its no-exceptions twins, unchanged and passing; the new `core.public_macros`.
6. **W10**: `tools/compiles cost` on one heavy unit (GCC debug `core/product`), `--before c95fee5`, under the timing lock (`math-tools/exercises/performance/pe-common/lock.sh take` and `release`), three repeats; expected within the repeats' range, since no instantiated code changes.

## 8. Mutants (W3)

| Unit | Mutant | Expected killer |
| --- | --- | --- |
| U1 | one `#undef` dropped | `core.public_macros` |
| U1 | error group's `GALOOT_ALWAYS_INLINE` definition dropped | `core.force_inline_on` |
| U1 | `#elif` split back into its own `#if` | `core.force_inline_empty` |
| U2 | `GALOOT_FORCE_INLINE` validation deleted | `tools.configure_rejects_force_inline` |
| U2 | `GALOOT_CHECKING` validation deleted | `tools.configure_rejects_checking` |
| U2 | `CMakeLists.txt:108-109` definition dropped, OFF tree (m5) | `core.force_inline_build` |
| U3 | `l` restored at `basis_change.hpp:542` | the lint unit, lint preset |

Each test counts when the mutant written for it is among those it fails (W3).

## 9. Checks (P8, P9, P13)

- Before each unit's commits: section 6's P8 selection in its configurations, each run through `tools/timed`.
- U7, the P9 `main` row on the merged tree, `gate:all`, one preset at a time on the pool: gcc-debug, clang-debug, gcc-asan (sanitizers, forced inlining off), clang-measure, lint (with `format-check`); then the trace gate, `tools/runlog check`, and the confusable scan of section 3.3. Gate 3's times give about 24 minutes in all (gcc-debug 163 s, clang-debug 104 s, gcc-asan 232 s, clang-measure 664 s, lint 267 s; inferred; plan 0043 may change them). The release matrix of all eleven presets is plan 0045's, at its release commit.
- Every run reads `tools/jobpool status` before and after; the pool is started with `tools/jobpool ensure` only when it reads "not running", never twice, and never stopped. Heavy runs go under `math-tools/exercises/performance/pe-common/memguard.sh`.

## 10. Files shared with plans 0043 and 0045

Inferred from the plans' titles; to confirm against their texts when they land.

| File | This plan | 0043 (cost) | 0045 (release) |
| --- | --- | --- | --- |
| `test/CMakeLists.txt` | U1, U2, U3 add or change tests in separate sections | likely: labels, test cost | — |
| `CMakeLists.txt` | U2: lines 97 to 109 | possible: build cost | `project(... VERSION 1.0.0)` to 1.0.1 |
| `CMakePresets.json` | U2: the mode in each preset | likely: presets' cost | — |
| `README.md` | U5: lines 38 to 46 | — | status line ("the 1.0.0 release"), `GIT_TAG`, the repository URL |
| `docs/guide/core.md` | U1 (`:1042-1052`), U5 (`:1132-1134`) | — | — |
| `records/release/` | U8 feeds items | — | the 1.0.1 release record |
| `records/performance/runs.tsv` | run rows | run rows | run rows |

W7: units that run at once change different files. So the coordinator sequences any 0043 unit that edits `test/CMakeLists.txt`, `CMakeLists.txt` or `CMakePresets.json` before or after this branch, and whichever merges second merges `main` in first. This branch merges before plan 0045's release commit.

## 11. Risks

1. **`tools/mutate` and a CMake edit.** It may not regenerate the tree or classify a configure failure. Mitigation: U2's hand run, recorded.
2. **The `#elif` on another compiler version.** The skipped-group rule is recalled for C++23; if a supported compiler evaluates the `#elif`, the empty value gives two errors again and `core.force_inline_empty` fails, which is the intended alarm.
3. **The macro-set test is brittle by design.** A new public macro or guard spelling fails it, which is the point; its comment says how to extend the allowed list.
4. **The configure tests' cost and environment.** They configure the project again, so they need the same compilers and Python; they run within the existing presets' environment. If one takes more than a few seconds, U2 reports it, and the owner may drop the `GALOOT_CHECKING` twin.
5. **Overlap with plan 0043** in `test/CMakeLists.txt` and the presets (section 10): merge conflicts, not semantic ones, if sequenced.
6. **A user who used the `GALOOT_DETAIL_*` macros** breaks. They are documented as not interface; plan 0045's release notes say they are no longer defined.

## 12. Agent cost estimate (W3)

About 1.2 M tokens: U1 200k, U2 120k, U3 80k, U4 150k, U5 80k, U6 250k plus a delta review 80k, U7 150k, U8 50k, coordinator 50k. Machine time, inferred: four whole-suite gcc-debug runs (about 3 minutes each with the rebuild), one lint run (about 5 minutes), the mutants (minutes), the W10 unit (about 5 minutes under the lock), and U7's 24 minutes.

## 13. Owner items (each with a default if not ruled)

- **O1. Which spellings the cache option takes.** (a) `ON` or `OFF` only, as `GALOOT_CHECKING`; (b) also `0` and `1`, since the macro's own spellings are 0 and 1 and a developer may type them; (c) any CMake boolean constant (`TRUE`, `YES`, `N`, ...), rejecting only what CMake reads as a path or a number other than 0 and 1. Tradeoff: (a) is one rule for both options and needs no new wording; (b) is friendlier where the two spellings meet, at the cost of two rules; (c) keeps every value that works today, but still lets typos such as `NO_` pass. Only Galoot's own builds see the option. Recommendation and default: (a).
- **O2. The README's copy route.** (a) Wording: copy `include/galoot` into a directory on the include path; (b) layout: move doctest out of `include/` (to `third_party/doctest` or `test/ext/doctest`), which needs owner-ratified edits of STYLE S3 and PERFORMANCE P8's table, changes to `test/CMakeLists.txt` and `format-check`'s exclusion, and a note to decision 0026's record. Tradeoff: (a) costs three lines and no test; (b) also takes doctest off a consumer's include path, but it is a governance change and a build change in a patch release. Recommendation and default: (a) in 1.0.1; (b) only if the owner wants the include path clean, in a later release.
- **O3. The deleted `==`'s hint.** (a) The guide note and its rejected block (U5); (b) also `= delete("use same_rows(a, b)")` behind `__cpp_deleted_function` in `basis_change.hpp`, which departs from STYLE S2 and so needs the owner's ruling or an S2 amendment; under C++23 builds nothing changes, and under C++26 the hint reaches the error line; (c) nothing. Tradeoff: (b) helps only users building with C++26 and puts the first later-edition feature into a default header; (a) helps every user who reads the guide. Recommendation and default: (a).
- **O4. "Model" in the governance and "preset" in the agent files.** (a) Leave them to the next governance pass, as decision 0215 item 11 says, with a prepared owner text (like `records/release/conventions-1.18-owner-text.md`) for CONSTITUTION's Terms and C5, STYLE S1 and S10, VVUQ and CONVENTIONS' markers; (b) prepare that text now as a records-only side unit of this plan; (c) keep "model" in governance as the ratified term. Tradeoff: (b) costs about 40k tokens and gives the owner one paste; the agent files and `tools/` comments are 91 lines of churn with no user effect. Recommendation and default: (a), with the agent files and tools left alone.
- **O5. Confirm the reading of section 1, item 5** (one feature branch, P8 in one configuration per unit, one P9 `main` matrix at the merge). Default: as read.

Only the owner can: rule O1 to O5; edit governance or CONVENTIONS; release 1.0.1 (plan 0045).

## 14. Next step

The coordinator merges this plan into `main` (the claim), sequences it against plan 0043's build-file units, and briefs U1 in a worktree on `feature/header-hygiene-1-0-1` from `main`, with this plan, section 3's evidence, and the owner's answers to O1 to O5.
