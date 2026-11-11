# 0010 Compile-fail tests built directly and in parallel

- Status: closed; the unit merged into `develop` by `0addac1` on 2026-11-01 (status corrected 2026-11-11, from `git log --merges` on `develop`). Status text as written before that correction, kept as history: claimed 2026-11-01; branch `feature/compile-fail-lane`, worktree `.claude/worktrees/compile-fail-lane`; decision numbers 0095 to 0099.
- Goal: stop compile-fail test time from growing with every unit (gate report item 11). Today each rejection test is one Ninja build under a tree-wide lock, 0.2 to 0.45 s, serial; CTest time per preset went from about 3.2 s (57 tests) to 12 to 14 s (88 tests) and 97 tests now.
- Definition of done: every compile-fail test compiles its source with one direct compiler command, so it needs no build-tree lock and CTest runs them in parallel; no test's pass or fail changes; the set of REQUIRE, FORBID, and ONLY checks is unchanged; the whole suite passes on all seven presets and the lint preset.
- Acceptance criteria: (1) `galoot_add_compile_fail_test` keeps its signature; (2) the compile command uses the same compiler, language standard, include paths, definitions, and warning flags as the library's tests for that preset, read from the target, not copied; (3) a source that compiles cleanly is a failing test, as now; (4) a compile-fail test run alone and the same test run beside the others give the same output; (5) CTest wall time of the compile-fail group on gcc-debug drops against the committed `runs.tsv` rows at `dc3c67c`, measured on clean revisions, and the cost is recorded, not asserted; (6) the existing mutants of the compile-fail tests (the records under `docs/agent/records/mutations/`) still fail the same tests.
- Approach: build-expert. Replace the object library and the Ninja call by `add_test` running `expect_output.cmake` around the compiler with `-fsyntax-only` and flags from generator expressions on `galoot::core` and the test interface; remove `RESOURCE_LOCK galoot_build_tree` from these tests; measure the choice of `-fsyntax-only` against `-c` on both compilers (a constraint or static_assert failure must still appear, and diagnostics that appear only in code generation must not be lost: list any).
- Evidence: per-preset test output, the cost measurements, and the mutant reruns.
- Review: W5 blind review before the merge.
- Agent cost: tracked for information only.

## Outcome (2026-11-01, Claude, agent, Opus 5.5, build-expert)

Status: done on `feature/compile-fail-lane`, pending the W5 blind review. Code at `2888987`; records at `21441cd` and the commit that adds this section.

### Departures, skipped checks, and ambiguities (C20)

1. `-c`, not `-fsyntax-only`. The approach named `-fsyntax-only`; the measurement it asked for found the same output on every source and 1 to 4 percent fewer instructions, but `tools/measure` finds the source and object only through `-c` and `-o`, so `-fsyntax-only` would have emptied the compile-fail rows of the per-unit compile record and made the gate copy step refuse every tree. Decision 0096.
2. Criterion 5 names `runs.tsv` rows at `dc3c67c`. There are none, in this worktree or in the main checkout. The baseline was measured instead at `b95e88e`, the branch's base, whose build files, tests, and headers are those of `dc3c67c` (`git diff dc3c67c b95e88e` touches only the plan and `compile.tsv`).
3. "One direct compiler command": the compile still runs through the compile-cost launcher (`tools/measure tu`) when the preset sets one, as every other compile does, so that the compile record keeps a row per compile-fail source. Read as "the test runs the compiler, not a build tool"; there is no Ninja call and no lock (decision 0095).
4. "Flags from generator expressions on galoot::core and the test interface": no test-interface target exists. Read as the preset's `CMAKE_CXX_FLAGS` and build-type flags, the variables the library's tests get their warning flags from. `galoot::core`'s interface compile options are not read, because it has none and an empty property would become an empty argument; a check fails if it gains some.
5. Two tests added beyond the plan, `tests.compile_fail_flags` and `tests.compile_fail_flags_rejects` (decision 0097), so the suite has 99 tests, not 97. The 97 that existed pass or fail as before. The new tests were committed after the change they judge (`2888987` after `2824f55`), because they call the helper the change adds; each commit passed on all seven presets.
6. Two helper mutants survive: dropping the launcher, and `-fsyntax-only` in place of `-c`. Both change only the compile-cost rows, which no test reads. Not closed.
7. The job pool had 2 of 23 slots lost with no client at the start and at the end (`tools/jobpool status`); not restarted, as instructed.
8. Anomaly, unexplained: at `b95e88e` the whole gcc-debug suite (16.5 s) took less time than its compile-fail group run alone (18.0 to 18.6 s), though the suite contains the group and the lock serialized it in both. Not traced; one suite run only.
9. Cost runs were in shared mode (CTest runs tests in parallel, so a single pinned CPU would measure a different thing), with the machine otherwise lightly loaded (load average 0.5 to 2.6; `other_cpu_s` is in each row). Three group runs on gcc-debug before and after, one on clang-debug.
10. The mutation reruns ran on gcc-debug and clang-debug only, as the existing records did.

### Criteria

1. Met. `galoot_add_compile_fail_test(NAME MODULE COMPONENT SOURCE MESSAGE [REQUIRE...] [FORBID...])` is unchanged, as are the `;` check and every call in `test/CMakeLists.txt`.
2. Met. `galoot_compile_command` reads the definitions and include directories from `galoot::core` through `$<TARGET_PROPERTY>`, the standard from its `cxx_std_23` feature through `CMAKE_CXX23_STANDARD_COMPILE_OPTION`, and the flags from `CMAKE_CXX_FLAGS` and `CMAKE_CXX_FLAGS_<type>`. On all seven presets, each of the 46 tests' compiler and arguments equal, in order, the old object library's command in `compile_commands.json` at `b95e88e` apart from `-o` (`records/spikes/compile-fail-modes/out/flags-vs-b95e88e.txt`). `tests.compile_fail_flags` keeps them equal to CMake's command for `galoot::core`'s header check; the mutants that drop the definitions, the include directories, the preset flags, the build-type flags, or the standard, or switch to the extension standard, each fail it on both compilers, while dropping the definitions, the flags, or (on GCC) the standard fails no compile-fail test.
3. Met. Mutant `source_compiles` (metric_zero pointed at `bare_user.cpp`) fails `core.metric_zero` with result 0 on both compilers; `wrong_message` fails it too (`records/mutations/0010-compile-fail-lane/results-2888987.tsv`). `core.assert_consteval_wrong_message` still passes only through `WILL_FAIL`.
4. Met. On gcc-debug and clang-debug at `21441cd`, each of the 46 tests run alone (`ctest -R '^<name>$' -V`) printed the same lines as in a parallel `ctest -V` of the whole suite (`alone_vs_together.py`; 46 of 46 on both).
5. Met, recorded not asserted. `runs.tsv`, shared mode, `ds-001`:

| | `b95e88e` (Ninja, lock) | `21441cd` (direct) |
| --- | --- | --- |
| gcc-debug compile-fail group, wall | 18.55, 18.06, 18.02 s | 0.553, 0.552, 0.541 s |
| gcc-debug group, instructions (P + E cores) | 38.27, 38.27, 38.27 G | 35.61, 35.61, 35.61 G |
| gcc-debug group, user + sys CPU | 18.4, 17.9, 17.9 s | 8.7, 8.9, 8.7 s |
| gcc-debug whole suite, wall | 16.55 s (97 tests) | 2.28 s (99 tests) |
| clang-debug compile-fail group, wall | 15.77 s | 0.552 s |
| clang-debug group, instructions | 33.56 G | 30.83 G |
| clang-debug whole suite, wall | 14.89 s | 2.29 s |

The wall time falls by a factor of about 33 because the 46 compiles now run at once; the instructions fall by 7 to 8 percent, which is the Ninja start and dependency scan per test (inferred). CPU time falls by half while instructions barely move: with the tests in parallel, part of the work runs on efficiency cores (inferred from the split of instructions between core types, 16.6 G of 35.6 on E cores after against 2.3 G before).
6. Met. The 32 mutants of `0008-blade-set/mutate.py` whose anchor still exists, run at `b95e88e` with the old lane and at `2888987` with the new, give the same build exit, test exit, failed tests, and verdict on both compilers (64 of 64 rows equal; all killed). The 14 mutants of `0009-blade-set-merge-fix/mutate.py` at `2888987` give rows equal to `results-28ef08e.tsv` (28 of 28), whose header and tests are those of this branch. Three 0008 mutants (`merge_conflict`, `index_sign_check`, `index_repeat_check`) no longer apply, because plan 0009 rewrote that code.

Code-generation diagnostics (decision 0096): with the presets' flags, only `-c` reports a call to an `error`- or `warning`-attribute function (both compilers), inline assembly the assembler rejects (both), and on GCC `-Wreturn-type`, `-Wuninitialized`, `-Wuse-after-free`, `-Wfree-nonheap-object`, `-Wunused-function` and `-Wunused-variable` for unused statics, `-Wstringop-overflow` or `-Warray-bounds` on an overflowing `memcpy` at `-O0` and `-O1`, `-Warray-bounds` at `-O2`, and failed `always_inline`. A front-end error stops code generation on both compilers, so none of these can change a test whose source fails in the front end; no current test relies on one.

### Verification

All seven presets (gcc-debug, clang-debug, gcc-release, clang-release, gcc-asan, clang-asan, lint, the last including `all_verify_interface_header_sets` and `format-check`) built and passed 99 of 99 at `2888987`, and again at the commit that adds this section. Not run: the W5 convention trace gate (no convention or test reference changed), TSan (no preset exists), and the measurement presets.

### Commits

- `0d6e533` baseline run rows; `2824f55` the direct-compile helper; `2888987` the flags check; `21441cd` decisions 0095 to 0097, the mutation record, the mode-comparison spike record; then this outcome with the after-change run rows.

### Next

W5 blind review, then the merge to `develop` with the P9 checks. Open: the two surviving mutants (item 6), and the unexplained baseline timing (item 8).
