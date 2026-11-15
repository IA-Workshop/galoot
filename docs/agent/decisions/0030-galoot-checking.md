# 0030 GALOOT_CHECKING: values, default, and how the build sets it

- Status: accepted
- Date: 2026-10-31
- Recorded by: Claude (agent, build-expert, WU1), under C18; a decision record takes effect when recorded, and the owner may reverse it.

## Context

C8 and DoD-20 require the checking mode to be selected explicitly in every preset. Decision 0020, item 2, sets the default when undefined to 0, checking ON for debug, asan, and lint, and OFF for release.

## Decision

- The macro takes 0 or 1. `include/galoot/core/assert.hpp` uses 0 when it is undefined, documented at the macro, and stops compilation with `#error` for any other value.
- The CMake variable `GALOOT_CHECKING` is ON or OFF; configuring Galoot as the top-level project without it, or with another value, fails with a message that names it. Every visible preset sets it in its own body: ON for `*-debug`, `*-asan`, and `lint`; OFF for `*-release` and the four measurement presets.
- The build passes it as `$<BUILD_INTERFACE:GALOOT_CHECKING=0|1>` on `galoot_core`'s interface, so every target that links `galoot::core`, including the header-check targets, gets the preset's mode, and nothing installed carries it. A directory-level `add_compile_definitions` was tried first; CMake applies it at generation in addition to a target's own definitions, so a target could not replace it (lessons-learned 0001).
- The two programs that test both modes (`core_assert_mode_1` and `core_assert_mode_0`) set their own value and take the headers by include path instead of linking `galoot::core`, because a second definition with another value is an error under `-Werror`.
- `GALOOT_ASSERT` checks its condition in every constant evaluation, whatever the mode, and at run time only when the mode is 1; at run time with mode 0 the condition is not evaluated.

## Consequences

A user who adds only the headers gets an unchecked build without configuring anything (C13, DoD-6). A future `.clang-tidy` or preset that sets the mode another way must keep it to one definition per target.
