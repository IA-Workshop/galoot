# 0041 Release and measurement presets do not define NDEBUG

- Status: accepted
- Date: 2026-10-31
- Recorded by: Claude (agent, build-expert, plan 0003), under C18; a decision record takes effect when recorded, and the owner may reverse it. Adds to decision 0029; the choice was stated only in plan 0002's outcome, departure 8 (W5 review of WU1, finding N9).

## Context

CMake's default `CMAKE_CXX_FLAGS_RELEASE` is `-O3 -DNDEBUG`. Every Galoot preset states its level instead (C8): the release presets set `CMAKE_CXX_FLAGS_RELEASE` to `-O2` and the `-O3` measurement presets to `-O3`, which replaces the default and so drops `-DNDEBUG`. Galoot's checking is selected by `GALOOT_CHECKING` (decision 0030), not by `NDEBUG`. Nothing in `include/galoot`, `test/`, or the vendored doctest reads `NDEBUG` (checked with `grep -rn NDEBUG` on 2026-10-31); `_GLIBCXX_ASSERTIONS` is set or left out by each preset on its own.

## Options

1. Leave `NDEBUG` undefined in every preset.
2. Add `-DNDEBUG` to the release and measurement presets, to match CMake's default and what most users' release builds define.

## Decision

Option 1. Defining `NDEBUG` would change nothing Galoot builds or tests today, and a flag with no present effect is left out (C12). A test that reaches a standard `assert`, or a header that comes to read `NDEBUG`, reopens this record, because then the release presets would test a different program from users' release builds.

## Consequences

A release preset's options are exactly what its body lists, and the `options` column of the records shows them (decision 0042). The independence of `GALOOT_CHECKING` from `NDEBUG` is unchanged: a user's `-DNDEBUG` does not turn Galoot's checks off.
