# 0254 The forced-inline mode's check and the choice of attribute are one `#if` chain, and the check's helper macros are removed

- Status: in force. Supersedes the choice and the reason that decision 0244 item 17 gave for keeping a second error on an empty `GALOOT_FORCE_INLINE` (decision 0248 item 3: supersession by a new decision). The rest of 0244 item 17 stands.
- Date: 2026-11-21
- Decided by: Claude (agent, Opus 5.5), implementer of plan 0044 unit U1, in the cpp-expert role, within the scope the owner accepted in decision 0252 (items 6 to 10: the plan's recommendations).
- Bears on: `include/galoot/core/assert.hpp`; `test/CMakeLists.txt` (`core.force_inline_empty`, `core.public_macros`); `test/core/public_macros.cpp`; `docs/guide/core.md` (the paragraph on `GALOOT_FORCE_INLINE`); plan 0044 sections 3.1, 3.2 and 6 (U1); the second delta review of plan 0040's opt-out (`records/reviews/2026-11-11-pre-release-core-pass-inline-opt-out-delta2-review.md`, m1, n1, n2); release record items on known friction.

## Context

At 1.0.0, `assert.hpp` checks the forced-inline mode by pasting it onto a name defined only for 0 and 1 (`GALOOT_DETAIL_PASTE(GALOOT_DETAIL_FORCE_INLINE_IS_, GALOOT_FORCE_INLINE)`), stops with `#error "GALOOT_FORCE_INLINE must be 0 or 1"` when the paste does not name one, and then, in a separate `#if`, compares the mode with 1 to choose `GALOOT_ALWAYS_INLINE`. Two frictions followed:

1. An empty value (`-DGALOOT_FORCE_INLINE=`) gave the literal and then a second error, from the later `#if GALOOT_FORCE_INLINE == 1`, which has no left operand (GCC: "operator '==' has no left operand"; Clang: "invalid token at start of a preprocessor expression"). Decision 0244 item 17 kept that second error, because "folding the check into that `#if` would leave `GALOOT_ALWAYS_INLINE` undefined after the error and give a cascade in every header". `core.force_inline_empty` therefore asked for the literal but not for one error.
2. The four helper macros (`GALOOT_DETAIL_PASTE_`, `GALOOT_DETAIL_PASTE`, `GALOOT_DETAIL_FORCE_INLINE_IS_0` and `_IS_1`) stayed defined in every unit that includes a Galoot header (the review's n1). They are documented as not interface, but `GALOOT_DETAIL_PASTE` reads like a utility a user could start to use.

The reason 0244 item 17 gave holds only if the folded form defines nothing after the error. An error group can define the attribute itself.

The rule that makes a chain safe is [cpp.cond] paragraph 14 of N4950 (15.2, read for this decision in `knowledge-base/references/cpp-language/n4950.txt`): "Only the first group whose control condition evaluates to true (nonzero) is processed; any following groups are skipped and their controlling directives are processed as if they were in a group that is skipped", and in a skipped group "the rest of the directives' preprocessing tokens are ignored". So once the check's group is taken, the `#elif` that compares the mode with 1 is never evaluated, and an empty mode cannot reach it.

## Options weighed

| Option | Empty value | `-1` | Valid spellings | Cost |
| --- | --- | --- | --- | --- |
| (a) Keep 1.0.0's form (decision 0244 item 17) | two errors | one (the paste error) | unchanged | none; the friction stays |
| (b) Paste in the second `#if` too, with names `..._ON_0` 0 and `..._ON_1` 1 (the review's optional fix) | one | two paste errors | unchanged | two more helper macros to define and remove |
| (c) One chain: `#if !paste` / `#error` and an empty `#define GALOOT_ALWAYS_INLINE` / `#elif mode == 1 && compiler` / `#else` / `#endif` | one | one (the paste error) | unchanged | none beyond the restructure |
| (d) A flag macro defined in the error group and tested first in the second `#if` | two: the second `#if` must still parse `== 1` with no left operand, whatever `&&` would skip | one | unchanged | a fifth helper; no gain over (c) |

For removing the helpers, the choices were to `#undef` the four straight after the chain's `#endif`, at the end of `assert.hpp`, or not at all. Only `assert.hpp` defines and uses them, on one line, and it is include-guarded, so the definitions, the use and the removal happen once per unit whatever the include order. Removing them straight after the chain keeps the definitions, their one use and their removal in one block; removing them at the end of the file has the same effect on users and separates them by sixty lines.

## Decision

1. Option (c). The error group defines `GALOOT_ALWAYS_INLINE` as nothing, so after the error no header that uses the attribute adds errors of its own; `core.force_inline_on`, which includes the whole core with `ONE_ERROR`, holds this.
2. The four helper macros are `#undef`ed straight after the chain's `#endif`. `core.public_macros` preprocesses a unit that includes `core/format.hpp`, `presets/rga3.hpp`, `core.hpp` and `core/assert.hpp` again with `-dM` and allows, among the macros named `GALOOT_`, only the include guards (by their `_HPP ` suffix), `GALOOT_CHECKING`, `GALOOT_FORCE_INLINE`, `GALOOT_ALWAYS_INLINE` and `GALOOT_ASSERT`. A new public macro or a guard of another form must be added to its list.
3. `core.force_inline_empty` asks for one error.
4. The guide's sentence on other values is replaced by the review's n2 form, checked against both compilers: `-DGALOOT_FORCE_INLINE` with no value defines it as 1; a word such as `ON` or `true`, another number such as `2` or `01`, or an empty value stops the compile with the literal; a spelling such as `-1` stops it with an error of the preprocessor's own. The expression cases the review listed (`1+0` accepted as 1, `1*0` as 0, with no diagnostic) behave as at 1.0.0 and get no text, as the review advised.

## Evidence

On GCC 16.2.1 and Clang 23.1.5 (Clang was 23.1.4 when plan 0044 was written), at the U1 commits of `feature/header-hygiene-1-0-1`:

- Every spelling, compiled with the whole core and `-Wall -Wextra -Wpedantic -Wundef` (the probe of plan 0044 section 3.2, repeated on the committed header): unset, `-D` with no value, `0` and `1` give no diagnostic and the attribute expected for the mode; empty, `ON`, `true`, `2`, `01`, `0x1` and `1u` give one error, the literal; `-1` gives one error, the paste error. Under `-Wundef`, a user's flag that Galoot's builds do not set, an empty value and `-1` also draw a warning that `GALOOT_DETAIL_FORCE_INLINE_IS_` is not defined, as at 1.0.0.
- The preprocessed output of a unit including `core.hpp`, `presets/rga3.hpp` and `core/format.hpp` (`-E -P`), on both compilers, with `GALOOT_CHECKING` 0 and 1 and `GALOOT_FORCE_INLINE` unset, `-D` with no value, 0 and 1, is identical to `c95fee5`'s in all sixteen cases, once the include directory's path that `__FILE__` writes into each `GALOOT_ASSERT` expansion is replaced by one placeholder on both sides; `-dM` differs only by the four helpers' absence.
- The tests and the mutants are recorded in `records/mutations/0044-header-hygiene/README.md`.

## Consequences

- A user who used one of the `GALOOT_DETAIL_*` macros breaks. They were documented as not interface (the release record lists them so); the 1.0.1 release notes should say they are no longer defined.
- `GALOOT_ALWAYS_INLINE` stays defined, as before, and stays outside the interface.
- Plan 0040's mutation specs anchor on the old text of `assert.hpp` and are kept as history; they no longer apply to the tree.
