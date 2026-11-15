# 0095 Compile-fail tests run the compiler directly, with the flags read from galoot::core

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), build-expert for plan 0010, answering item 11 of the M0/M1 gate report.

## Context

Each compile-fail test was an `EXCLUDE_FROM_ALL` object library and a CTest test that ran `cmake --build <tree> --target <library>` inside `test/expect_output.cmake`. Two Ninja processes in one tree could corrupt its logs, so every such test held `RESOURCE_LOCK galoot_build_tree` and they ran one at a time. At `b95e88e` the 46 tests took 18.0 to 18.6 s on gcc-debug, longer than the rest of the suite together, and the time grows with every rejection a unit adds.

## Options weighed

1. Keep the build and the lock. Rejected: the cost the gate report names.
2. Keep the object libraries but build them all in one Ninja call before the tests, as a fixture. Removes the per-test Ninja start, but one test's compile then depends on another target's build step, a failure in one shows in the fixture, and a run of one test builds all of them.
3. Run each test's compile directly, with the compiler command written into the test: no build step, no lock, parallel under CTest's job limit. The flags have to come from somewhere other than CMake's own compile rule, which it does not expose.
   - 3a. Copy the flags into the helper. Rejected: they would drift from the library's tests.
   - 3b. Read them where CMake reads them: definitions and include directories from `galoot::core`'s interface properties through generator expressions; `CMAKE_CXX_FLAGS` and the build type's flags from the variables the presets set; the language standard from `galoot::core`'s `cxx_std_<n>` feature, spelled with `CMAKE_CXX<n>_STANDARD_COMPILE_OPTION` (or the extension option when `CMAKE_CXX_EXTENSIONS` is on). Chosen.

## Decision

- `cmake/galoot_testing.cmake` gains `galoot_compile_command(<out>)`, which builds that command, and `galoot_add_compile_fail_test` keeps its signature and its checks but runs `expect_output.cmake -- 1 <literals> -- <launcher> <command> -o <module>_<name>_compile_fail.o -c <source>` in the test directory of the build tree, with `COMMAND_EXPAND_LISTS`. The object library, `SKIP_LINTING`, and the lock are gone. `galoot::core` sets no interface compile options, so the helper reads none (an empty property would become an empty argument); if it gains some, `tests.compile_fail_flags` fails, because the header checks get them.
- The compile goes through `CMAKE_CXX_COMPILER_LAUNCHER` when the build sets one, so `tools/measure` still writes a compile row per compile-fail source to the tree's `compile.tsv`, as the per-unit record requires; the lint preset sets none.
- `-c` rather than `-fsyntax-only` is decision 0096. The check that the command matches CMake's is decision 0097.

## Evidence

The plan's outcome holds the run logs: per preset, the test's compiler arguments against the old object library's command in `compile_commands.json` at `b95e88e`; the CTest time and instruction counts of the group before and after; each compile-fail test run alone against the same test in a parallel run; and the mutation reruns.

## Consequences

- A flag added to a single target in `test/CMakeLists.txt`, or a directory-level `add_compile_options`, does not reach compile-fail tests, as it did not reach the old object libraries unless added there; `tests.compile_fail_flags` compares with a target that links only `galoot::core`, so it does not catch this either.
- The helper assumes a single-configuration generator (`CMAKE_BUILD_TYPE`), as every preset uses Ninja.
- A compile-fail test no longer appears in `compile_commands.json`, so an editor's language server no longer shows its expected errors as problems of a build target.

## Addendum, 2026-11-01 (W5 review findings F2, F3, F5)

- A directory-level `add_compile_definitions` or `add_compile_options` in `test/CMakeLists.txt` reached the old object libraries but does not reach the direct command, and no test notices. None exists today. The phrase above, "did not reach the old object libraries unless added there", is wrong: it did reach them. Options added in the root file are caught by the flags check.
- Under a multi-config generator the helper omits the build-type flags; the flags check fails loudly. No preset uses one.
- The `object` column of compile-fail rows in `compile.tsv` changes from `test/CMakeFiles/<target>.dir/...o` to `<target>.o`, so a comparison across this revision keyed on preset, source, and object sees 46 new units.
