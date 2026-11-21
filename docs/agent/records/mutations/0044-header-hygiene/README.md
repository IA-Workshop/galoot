# Plan 0044 mutants and behavior comparisons: release 1.0.1 header hygiene

Plan 0044 sections 6, 7 and 8. Unit U1 run by Claude (agent, Opus 5.5, implementer of U1) and unit U2 run by Claude (agent, Sonnet 5.5, implementer of U2) on 2026-11-21 in the worktree `galoot-wt/header-hygiene` on `feature/header-hygiene-1-0-1`, on gcc-debug (g++ 16.2.1) and clang-debug (clang++ 23.1.5, both with libstdc++ 16). Rows: `out/<spec>-<preset>.tsv`; the runs are the rows `mutant:<name>`, `mutant:baseline` and `mutant:restore` of `records/performance/runs.tsv`. The job pool read 20 of 23 slots free before the run and 21 after, with one other workstream's `ninja` as a client; the machine's load average was about 5 to 6 from other sessions.

## U1: the forced-inline mode's check (`u1-mode-check.toml`)

At `97dd597` (the U1 header and guide commits), with only records dirty. Decision 0254 records the change these mutants judge.

| Mutant | Edit | gcc-debug | clang-debug | Written for |
| --- | --- | --- | --- | --- |
| `undef_dropped` | `#undef GALOOT_DETAIL_PASTE_` removed | killed by `core.public_macros` | killed by `core.public_macros` | `core.public_macros` |
| `error_group_undefined` | the error group's empty `#define GALOOT_ALWAYS_INLINE` removed, so every header that uses the attribute adds errors | killed by `core.force_inline_on` | killed by `core.force_inline_on` | `core.force_inline_on` (no cascade) |
| `elif_split` | the `#elif` turned back into an `#endif` and a separate `#if`, so an empty mode is compared with 1 | killed by `core.force_inline_empty` | killed by `core.force_inline_empty` | `core.force_inline_empty` (one error) |

`tools/mutate` exited 0: every verdict is as declared, each declared killer killed its mutant, and each mutant had exactly that one killer. None was unjudged.

The tests also fail against the header of `c95fee5` (the run rows of commit `b2665c3` at `01dce7f39d1c-dirty`, on both debug presets): `core.public_macros` names the four helper macros as unexpected lines, and `core.force_inline_empty` names the second error (GCC "operator '==' has no left operand", Clang "invalid token at start of a preprocessor expression").

## U1: behavior of the valid modes (`behavior/`)

`behavior/compare.sh <old include> <new include> <out>` preprocesses `behavior/unit.cpp` (the printing header, the rga3 preset and the core's umbrella header) with both compilers, `GALOOT_CHECKING` 0 and 1, and `GALOOT_FORCE_INLINE` unset, `-D` with no value, 0 and 1: sixteen cases. It compares the `-E -P` outputs byte for byte after one normalization, stated in the script: the include directory's path, which `__FILE__` writes into each `GALOOT_ASSERT` expansion, becomes `<include>` on both sides. It also diffs the sorted `-dM` output. The old side was `git archive c95fee5 include`; `include/` is unchanged from `c95fee5` to `01dce7f`, the branch's base.

Run at `97dd597` (row `check - preprocessed-compare` of the run log): all sixteen `-E -P` outputs identical; in every case `-dM` loses exactly `GALOOT_DETAIL_FORCE_INLINE_IS_0`, `_IS_1`, `GALOOT_DETAIL_PASTE_` and `GALOOT_DETAIL_PASTE`, and gains nothing.

The invalid spellings were probed by hand on the same compilers with the whole core and `-Wall -Wextra -Wpedantic -Wundef`, not by a script kept here: empty, `ON`, `true`, `2`, `01`, `0x1` and `1u` give one error, the literal "GALOOT_FORCE_INLINE must be 0 or 1"; `-1` gives one error, the paste error; `GALOOT_ALWAYS_INLINE` is defined after each of them. `1+0` is accepted as 1 with no diagnostic, as at 1.0.0 (the second delta review of plan 0040, n2).

## U2: the cache options (`u2-configure-checks.toml`, `u2-propagation.toml`)

The configure spec ran at `c2f0f96` and the propagation spec at `44d0f25` (the commits after the two tests, the check and the presets; neither commit touched `CMakeLists.txt`, the presets or `test/`), on the same worktree. `tools/mutate` exited 0 on both specs: every verdict is as declared and each declared killer killed its mutant. The pool read 21 of 23 slots free before the first run and 22 after, with one other workstream's `ninja` as a client; the load average was about 8 to 10 from other sessions.

| Mutant | Edit | Presets | Verdict | Written for |
| --- | --- | --- | --- | --- |
| `force_inline_check_deleted` | the `GALOOT_FORCE_INLINE` check removed from `CMakeLists.txt`, so that 2 reads as ON | gcc-debug, clang-debug | killed by `tools.configure_rejects_force_inline` on both | that test |
| `checking_check_deleted` | the `GALOOT_CHECKING` check removed | gcc-debug, clang-debug | killed by `tools.configure_rejects_checking` on both | that test |
| `propagation_dropped` (m5) | the `GALOOT_FORCE_INLINE=0` definition removed from the interface of `galoot::core` | clang-release, a tree whose forced inlining is off | killed by `core.force_inline_build` (its case "GALOOT_ALWAYS_INLINE is the always_inline attribute exactly when the forced-inline mode is on") | `core.force_inline_build` |

`tools/mutate` needed no hand run for the CMake edit: Ninja regenerates the tree when `CMakeLists.txt` changes, in the mutant's build and again in the restore build, and the configure tests read the source tree directly. The first run of `u2-propagation.toml` was refused before anything was built, because its selection named only `core_force_inline_build` while the label `core.assert` selects tests that run the other four programs; the selection now builds all five, as the U1 spec does.

`tools.configure_rejects_force_inline` fails at `361007c`, the commit before the check (result 0, message missing); `tools.configure_rejects_checking` passes there, as it pins a check that already existed. Each test takes under a second in the debug trees (0.56 s and 0.15 s in gcc-debug).

## U2: behavior of the presets (`configure/`)

`configure/compare.sh <old source> <new source> <scratch>` configures every configure preset from both trees into scratch build trees and compares `CMakeCache.txt` (comments and blank lines left out, the build and source paths replaced by `<build>` and `<source>`) and `compile_commands.json` under the same replacement. The old tree was `git archive 06ff369`. Result in `configure/out.txt`: all eleven presets have identical caches and compile commands; `GALOOT_FORCE_INLINE` is ON in eight and OFF in gcc-asan, clang-release and clang-asan. The cache identity includes the value of `GALOOT_FORCE_INLINE`, which is ON by default before and is now set to ON by the preset.

## U3: confusable names (`u3-confusable-names.toml`)

Run by Claude (agent, Sonnet 5.5, implementer of U3) on 2026-11-21 in the same worktree, at `66b203b` and `c956775` plus records (the revision column of `out/u3-confusable-names-lint.tsv` reads `e63e3fd1106c`, the spec's own commit). The unit `test/core/user_names.cpp` fails the lint preset at the headers of `f78b54d` with two errors, `'l' is confusable with 'I'` (`basis_change.hpp:542`) and `'m' is confusable with 'rn'` (`bits.hpp:171`); clang-tidy 23.1.5 run by hand on the unit shows the same two.

The check reports only the first header name of each look-alike group. After the two planned renames the lint build still failed, on `m` in `metric.hpp:188` and `l` in `motions.hpp:80`, which the first two had hidden; renaming those (the locals `m` to `result`, the parameter `m` to `form`, the parameter `l` of `rotation` to `axis`) leaves the lint build clean, and a scan of 3,954 one- to three-character names declared beside all the headers reports only `q1` against `qI` and `ql`, the name the plan keeps.

| Mutant | Edit | Preset | Verdict |
| --- | --- | --- | --- |
| `column_index_l_restored` | the first loop of `keeps_inner_products` with its old `l` | lint | unjudged by `tools/mutate`; the build of `core_user_names` fails with `basis_change.hpp:542:22: error: 'l' is confusable with 'I'` |

`tools/mutate` classifies the failed object by its first error line: a lint error names neither a failed static assertion nor `[-Werror`, and lies in a header, not under `test/`, so the class is `invalid` and the verdict `unjudged`, as for any build failure outside those classes. The mutant is killed in fact, by the lint preset's build, which is the judge the plan names; the tool cannot say so. The spec keeps `expect = "killed"`, so `tools/mutate` exits 1 on it.
