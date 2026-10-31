# 0022 Rulings on the remaining WU1 questions

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner, answering plan 0002 section 11 ("agree with all ... for tests use the per module layout with a doctest main that has only the macro required"). Recorded by Claude (agent).

## Decision

1. Umbrella header: `include/galoot/core.hpp`, included as `<galoot/core.hpp>`. The module headers stay under `include/galoot/core/`. STYLE S6 gets a one-line note when it is next edited.
2. Test layout: `test/<module>/<component>_test.cpp`, with module and component labels.
3. `tools/timed` refuses to run build and CTest steps (exit 2) when the job-slot pool is missing, and runs other commands unchanged.
4. No `.clang-tidy` for `test/` until the first real finding in a test.
5. No energy-idle threshold. WU1 records the evidence (other-CPU time, sibling-thread busy time) and sets none; a threshold waits for data.
6. Install and export are deferred until something uses them.
7. The doctest runner uses doctest's built-in `main`, selected with the single `DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN` macro in one translation unit, and no custom `main`. A custom `main` waits for the first randomized test (V7).
