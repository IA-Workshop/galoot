# 0026 doctest 2.5.3 as the test dependency, with its continuity record

- Status: accepted
- Date: 2026-10-31
- Recorded by: Claude (agent, build-expert, WU1), under C18; a decision record takes effect when recorded, and the owner may reverse it.

## Context

C13 makes adopting a test dependency a decision that weighs the part used, its support, its cost, and its license, and asks a dependency that one person supports for a record of how work continues if support ends. doctest 2.5.3 is vendored unchanged at `include/ext/doctest/` (research note 0001), under the MIT license (`include/ext/doctest/LICENSE.txt`).

## Options

1. doctest 2.5.3, vendored and frozen.
2. Catch2 or GoogleTest: larger, slower to compile, and GoogleTest needs a built library.
3. An owned minimal runner now.

## Decision

Option 1. Galoot uses the single header, `TEST_CASE`, `CHECK`, `SUBCASE`, `TEST_CASE_TEMPLATE`, the built-in `main`, and the console reporter. The copy is frozen at 2.5.3; an update is a new decision. doctest is maintained by a small group, with one main maintainer.

Continuity: if doctest's support ends, Galoot keeps 2.5.3, which needs nothing beyond the C++ standard library, or replaces it with an owned minimal runner covering the parts listed above. Tests reach doctest only through its public macros, so a replacement changes the runner and the macro definitions, not the tests.

## Consequences

The core does not depend on doctest (C13); only `test/` does. `include/ext/` is never formatted or linted (STYLE S3, S4). Known 2.5.x limits that bind tests are in note 0001: no `SUBCASE` in loops (#1208), one declaring macro per line on Clang 22 and later.

## Addendum, 2026-11-01: trimmed tree

Decision 0094 trimmed the vendored tree to the three files Galoot uses. SHA-256: `doctest.h` cfd518a3ef90f67e1f3ba514df23fb3627437de1a2feeba78cf5062a40021421, `doctest.cpp` 392d4e28cfa11ce8bc534797532cbf32372ade10a4a3967db25c41747daeca0b, `LICENSE.txt` 0fe0b331fa1513dcce8604ff1fa925f32d1cea17d8aeb1c2471fad40d291adc5. The files are otherwise unchanged from 2.5.3.
