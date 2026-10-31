# 0025 Test structure: one CTest test per program, one runner, labels

- Status: accepted
- Date: 2026-10-31
- Recorded by: Claude (agent, build-expert, WU1), under C18; a decision record takes effect when recorded, and the owner may reverse it.

## Context

Plan 0002, section 4.10, item 2. CMake 4.4's `discover_tests()` can register one CTest test per doctest case (research note 0002, F4); `add_test` registers one per program (note 0001, F36 to F38). doctest's runner costs about 1 s and 300 MiB per copy (note 0001, Table 1). `DOCTEST_CONFIG_SUPER_FAST_ASSERTS` roughly halves assertion compile cost (note 0001, Table 2).

## Options

1. One CTest test per program, with `add_test`; one runner compiled once as an OBJECT library.
2. One CTest test per doctest case, with `discover_tests()`.
3. A runner per test program.

## Decision

Option 1. Each `test/<module>/<component>_test.cpp` is its own program, linked to the OBJECT library `galoot_doctest_main`, compiled once from `include/ext/doctest/doctest.cpp` with `DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN` as its only private definition (decision 0022, item 7). `DOCTEST_CONFIG_SUPER_FAST_ASSERTS` is a public definition of that library, so every test gets it; its savings are rechecked on the first real tests at M1. Per-case discovery adds a listing run per program and gains nothing while every program is fast.

Labels: every test carries `<module>` and `<module>.<component>`. The component label names the header or facility under test, so one label can select several tests: `core.assert` (the four assertion tests), `core.headers` (`core.bare_compile`, `core.include_scan`), `core.runner` (`core.zero_cases`), `core.smoke`, and `tools.include_scan`. Every doctest program has `FAIL_REGULAR_EXPRESSION "test cases: +0 \\|"`, and every test preset has `noTestsAction: error`, so neither an empty doctest filter nor an empty label selection passes.

## Consequences

Per-case timing is not in CTest's log. Option 2 can be revisited when a suite is slow enough to need per-case selection (P9's `slow` label).
