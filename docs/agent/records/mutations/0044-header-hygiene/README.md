# Plan 0044 mutants and behavior comparisons: release 1.0.1 header hygiene

Plan 0044 sections 6, 7 and 8. Unit U1 run by Claude (agent, Opus 5.5, implementer of U1) on 2026-11-21 in the worktree `galoot-wt/header-hygiene` on `feature/header-hygiene-1-0-1`, on gcc-debug (g++ 16.2.1) and clang-debug (clang++ 23.1.5, both with libstdc++ 16). Rows: `out/<spec>-<preset>.tsv`; the runs are the rows `mutant:<name>`, `mutant:baseline` and `mutant:restore` of `records/performance/runs.tsv`. The job pool read 20 of 23 slots free before the run and 21 after, with one other workstream's `ninja` as a client; the machine's load average was about 5 to 6 from other sessions.

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
