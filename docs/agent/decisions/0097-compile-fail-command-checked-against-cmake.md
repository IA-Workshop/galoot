# 0097 A test compares the compile-fail command with CMake's for galoot::core's header checks

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), build-expert for plan 0010, for its criterion 2.

## Context

Decision 0095 builds the compile-fail command from `galoot::core`'s properties and the build's flag variables instead of letting CMake write it. A mistake in that helper, such as a property left out, would not show in the compile-fail tests themselves: losing `-DGALOOT_CHECKING=1`, `-Werror`, or the warning flags leaves every current rejection in place (shown in the mutation record of plan 0010). Something has to fail when the command drifts from what the library's tests get.

## Options weighed

1. A one-time comparison, recorded in the plan. Shows the command right today and nothing after.
2. A source that fails with a message built from predefined macros (`__cplusplus`, `GALOOT_CHECKING`, `_GLIBCXX_ASSERTIONS`). Covers what the preprocessor can see, not warning flags, optimization, or sanitizers.
3. A test that reads the build's `compile_commands.json`, takes the command CMake generated for a target that links `galoot::core` and nothing else, and compares it, argument by argument and in order, with `galoot_compile_command`'s, apart from the source, the object, and `-x <language>`. The target is `galoot_core_verify_interface_header_sets`, which exists in every preset, and its unit for `galoot/core.hpp`. Chosen.

## Decision

`tests.compile_fail_flags` runs `test/compile_flags_check.cmake` on that unit and the helper's command; `tests.compile_fail_flags_rejects` gives it the command with one argument added and passes only if the check fails with "the commands differ". Both carry the labels `tests` and `tests.compile_fail`. The check needs `CMAKE_EXPORT_COMPILE_COMMANDS`, which the `base` preset sets; without it the test fails and says so.

## Consequences

- The check depends on two details of CMake: the path of the generated header-check unit, and the `-x c++` it adds for a source whose `LANGUAGE` property is set. A change to either fails the test with a message naming the file or both commands, not silently.
- It does not cover the launcher, which is the same list for every compile in the tree, or what decision 0095 lists under consequences.

## Addendum, 2026-11-01 (W5 review finding F1)

The check compares `galoot_compile_command` with CMake's command for the library check. It does not compare the command the test itself runs: adding `-O3`, `-w`, or `-Wno-error` in the helper's `add_test`, or dropping `COMMAND_EXPAND_LISTS`, survives on both compilers. The statement above that it "does not cover the launcher" understates this. Closing the gap (comparing one real compile-fail test's full command) is left to a later tooling unit.
