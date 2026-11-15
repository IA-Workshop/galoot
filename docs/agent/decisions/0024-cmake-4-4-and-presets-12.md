# 0024 CMake 4.4 for Galoot's own build, and presets schema 12

- Status: accepted
- Date: 2026-10-31
- Recorded by: Claude (agent, build-expert, WU1), under C18; a decision record takes effect when recorded, and the owner may reverse it.

## Context

Galoot's build needs `CMAKE_TEST_BUILD_DEPENDS` (the `test_prep/<test>` targets of P12), presets schema 12, and `SKIP_LINTING`, all of CMake 4.4 or earlier 4.x (research note 0002, F1, F3, F5, F8). CMake 4.4.5 is the platform's CMake (decision 0006). Research note 0002, open question 1, asked whether to require exactly 4.4 or a range.

## Options

1. `cmake_minimum_required(VERSION 4.4)` and presets schema 12.
2. A range such as `3.28...4.4`, without the 4.4-only features.
3. A lower minimum with the 4.4 features behind version checks.

## Decision

Option 1. `CMakeLists.txt` requires 4.4 and `CMakePresets.json` is schema 12 with `cmakeMinimumRequired` 4.4.0. No older CMake is installed, so a lower minimum could not be tested (decision 0006: no untested minimum is claimed), and a range would give up `test_prep` targets that P12 relies on.

## Consequences

Contributors need CMake 4.4. Users of the core need no build system at all (C13). Install and export are deferred (decision 0022, item 6), so whether exported files load in an older CMake (note 0002, F1) is not yet a question.
