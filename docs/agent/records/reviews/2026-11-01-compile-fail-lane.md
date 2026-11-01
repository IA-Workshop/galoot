# W5 review: feature/compile-fail-lane (plan 0010) into develop

- Reviewer: blind-reviewer, Claude Opus 5.5 (agent), 2026-11-01. Its own build trees were `build/review-<preset>` in the branch worktree (removed after use), plus two detached scratch worktrees: one at `b95e88e` for the old lane, one at `5741867`, later `2824f55`, for mutants and the W2 check. Both scratch worktrees were removed after use. Their 146 run rows are appended to `runs.tsv` with their own revisions, branch `detached`, and selections prefixed `review`.
- Reviewed: `git diff develop...HEAD` at `5741867`; merge base `b95e88e`. Since then develop has moved to `00c660f`, which changes only `docs/agent/plans/0011-*` and `agent-cost.tsv`. `git merge-tree` finds no conflict, but no trial merge was built.
- Brief: it named the branch, the plan, and a list of checks. It gave the author's Outcome and report only as claims to check, together with the author's admission that dropping definitions, flags, or the standard fails no compile-fail test. It carried no verdict.
- Verdict: approve with non-blocking findings. Nothing blocks the merge. F1 and F2 are corrections to decisions 0097 and 0095, worth making with the merge.
- Not run: the pinned `-c` against `-fsyntax-only` cost table of decision 0096 (only output equality was rechecked); 11 of the 16 code-generation probes (5 were spot-checked); the 0008 blade-set mutants (the 0009 set was rerun); the decision-0080 copy-step claim; pinned timings (every timing here was taken in shared mode); TSan (no preset exists); the measurement presets.

## Departures of this review (C20)

- `ctest -j 64` is bounded by the 23-slot job pool (CTest joins the jobserver), so "high -j" means 23 concurrent jobs, not 64. No run was made outside the pool.
- The base tree lives at a different path from the author's. That may explain why its instruction counts differ from the author's (F4).

## Checks against the plan

| Criterion | Result | Evidence |
| --- | --- | --- |
| DoD: REQUIRE, FORBID, ONLY unchanged | Met | `test/CMakeLists.txt` only gains the two flag tests. Every `galoot_add_compile_fail_test` call, the helper's argument handling, and `test/expect_output.cmake` are unchanged. |
| DoD: suite on all seven presets | Met | Fresh trees at `5741867`, built with `tools/timed`: gcc-debug, clang-debug, gcc-release, clang-release, gcc-asan, clang-asan, and lint (`all`, `all_verify_interface_header_sets`, `format-check`). 99 of 99 tests pass on each, 2.6 to 3.3 s. The trace gate, markers (43 test cases), and golden all exit 0. |
| 1 signature kept | Met | The diff of `galoot_add_compile_fail_test` leaves the signature unchanged. |
| 2 same command, read not copied | Met | For each preset, `ctest --show-only=json-v1` in the new tree was compared with each `*_compile_fail` entry of `compile_commands.json` in a fresh `b95e88e` tree, ignoring `-o`, `-c`, and the roots. On all seven presets, 46 of 46 commands are equal argument by argument and in order, and each test compiles the same source as before. The launcher is the tree's `tools/measure tu ... --` on the six measured presets and absent on lint, as before. Under Unix Makefiles, the 46 tests and both flag tests pass. Under Ninja Multi-Config, `tests.compile_fail_flags` fails with `-C Debug` and `-C Release`, because `-O0 -g` and `-DCMAKE_INTDIR` are missing: a loud failure, not a silent one (F3). Flags are fixed at configure time in both lanes. `CTestTestfile.cmake` holds them literally, and so does `compile_commands.json`, so the two stay consistent. |
| 3 a clean source fails | Met | The reviewer's mutant `clean_source_full_zero` (`core.blade_set_full_zero` pointed at `bare_user.cpp`) fails exactly that test on gcc-debug, clang-debug, and gcc-release. |
| 4 alone equals together | Met | On gcc-debug, clang-debug, and gcc-asan, each test's lines in `ctest -V -R '^name$'` equal its lines in a parallel `ctest -V -j` of the whole suite: 46 of 46 on all three. |
| 5 cost drops, recorded | Met | See the cost section. |
| 6 earlier mutants unchanged | Met for the 0009 set | The 14 mutants of `0009-blade-set-merge-fix/mutate.py` at `5741867` give rows equal to `results-28ef08e.tsv` on gcc-debug and clang-debug (28 of 28). |

## Mutants of the reviewer's own

These were applied in a scratch checkout at `5741867`. Each one rebuilt the tree and ran the whole suite. The column "Caught by" names the tests that fail.

| Mutant | gcc-debug | clang-debug | gcc-release |
| --- | --- | --- | --- |
| `clean_source_full_zero` | `core.blade_set_full_zero` | same | same |
| `wrong_require` (`assertion_failedX` on `bits_consteval`) | `core.bits_consteval` | same | not run |
| `forbid_present` (`same_blade` forbids "static assertion failed") | `core.blade_set_same_blade` | same | not run |
| `drop_definitions` | `tests.compile_fail_flags` only | same | same |
| `checking_flipped` (`GALOOT_CHECKING` given the other value) | `tests.compile_fail_flags` only | same | same |
| `drop_includes` | all 46 compile-fail tests and the flags check | same | not run |
| `drop_standard` | `tests.compile_fail_flags` only | 37 compile-fail tests and the flags check | not run |
| `drop_preset_flags` (`CMAKE_CXX_FLAGS`) | `tests.compile_fail_flags` only | same | not run |
| `drop_build_type_flags` | `tests.compile_fail_flags` only | same | same |
| `drop_werror` | `tests.compile_fail_flags` only | same | not run |
| `dir_option_at_top` (`add_compile_definitions` in the root, before the core) | `tests.compile_fail_flags` | same | not run |
| `no_expand_lists` (the helper's `add_test` without `COMMAND_EXPAND_LISTS`) | survives | survives | not run |
| `extra_flag_in_test_only` (`-O3` added in the helper's `add_test`) | survives | survives | not run |
| `dir_option_in_test` (`add_compile_definitions` at the top of `test/CMakeLists.txt`) | survives | survives | not run |

The author is right that losing the definitions, the checking macro, the preset or build-type flags, or (on GCC 16) the standard fails no compile-fail test. That gap does not weaken the lane. Each of the 46 rejections is a `static_assert` or a failed constant evaluation, and neither depends on warning flags or on the checking mode. The criterion asks for the command to match the library's tests, and `tests.compile_fail_flags` enforces that: it caught every mutant of `galoot_compile_command`. The three survivors are all outside that function (F1, F2).

## Parallel safety

- Object names: `<module>_<name>_compile_fail.o` in the calling directory's binary tree. Test names are unique, so the names are unique today. In principle, module `core` with name `a_b` would collide with module `core_a` with name `b`; there is no such pair. The object is written only when the source compiles, and then the test fails anyway.
- Concurrency: two `ctest -j 64 --repeat until-fail:3` runs of the 46 tests, started at once in one tree, pass three times each. `tools/measure` appends `compile.tsv` under `flock`, and perf's output goes to a `mkstemp` file.
- Stale objects: a planted `core_metric_zero_compile_fail.o` leaves the test passing on both compilers. GCC leaves a pre-existing file in place when the compile fails; Clang deletes it. No check reads the object.

## Cost (shared mode, job pool, load average 0.9 to 2.1)

| | `b95e88e` (reviewer) | `5741867` (reviewer) | Author |
| --- | --- | --- | --- |
| gcc-debug group, wall | 17.94, 18.81, 19.26 s | 0.574, 0.633, 0.524 s | 18.55, 18.06, 18.02 → 0.553, 0.552, 0.541 s |
| gcc-debug group, instructions P + E | 41.85, 41.85, 41.85 G | 35.61, 35.61, 35.61 G | 38.27 → 35.61 G |
| gcc-debug group, user + sys | 17.8, 18.7, 19.1 s | 8.5, 9.1, 8.8 s | 17.9 to 18.4 → 8.7 to 8.9 s |
| gcc-debug suite, wall | 18.09, 18.26 s | 2.069, 2.078 s | 16.55 → 2.28 s |
| clang-debug group, wall | 18.26 s | 0.430 s | 15.77 → 0.552 s |
| clang-debug suite, wall | 18.05 s | 2.127 s | 14.89 → 2.29 s |

The after-change figures reproduce, including the instruction count to four digits. The reduction factor (about 32 on gcc-debug), the halving of CPU time, and the shift of work to efficiency cores reproduce too.

## Findings, most severe first

None blocks the merge.

- F1 (non-blocking). `tests.compile_fail_flags` checks `galoot_compile_command` but not the command that `galoot_add_compile_fail_test` runs. An argument added on the helper's own `add_test` line (`cmake/galoot_testing.cmake:120-121`), such as `-O3` (mutant `extra_flag_in_test_only`), survives on both compilers, and by the same route `-w` or `-Wno-error` would (inferred, not run). So does removing `COMMAND_EXPAND_LISTS` (line 123), latent while the core has one definition and one include directory. Decision 0097's consequence says only that the check "does not cover the launcher". The fix: either say that it covers the shared function and not the `add_test` line, or record the full command of one real compile-fail test (for example in a global property set by the helper) and compare that.
- F2 (non-blocking, wording). A directory-level `add_compile_definitions` or `add_compile_options` in `test/CMakeLists.txt` before the calls reaches the doctest programs and would have reached the old object libraries. It does not reach the new command, and no test notices (mutant `dir_option_in_test` survives on both compilers). Decision 0095 lists this as a consequence but says the option "did not reach the old object libraries unless added there". That reads as if the old lane missed it too, which it did not when the option was added in `test/`. Nothing uses such an option today (C12), so the fix is to the record only. A root-level option is caught (mutant `dir_option_at_top`).
- F3 (non-blocking). Under a multi-configuration generator, the helper silently leaves out the build-type flags (`CMAKE_BUILD_TYPE` is empty). Only `tests.compile_fail_flags` reports it, and the comment on `galoot_compile_command` does not state the assumption, which decision 0095 records. A `FATAL_ERROR` on `GENERATOR_IS_MULTI_CONFIG` would give the reason at configure time. No preset uses such a generator.
- F4 (informational; the author's outcome item 8). The baseline anomaly does not reproduce. In two runs at `b95e88e` the gcc-debug suite took 18.09 and 18.26 s, against 17.94 to 19.26 s for the group alone. The compile-fail tests' own times sum to 17.98 and 18.11 s inside the suite and 17.79 s alone, which is the serialization the lock predicts. The author's single 16.55 s suite run, and the clang-debug base of 15.77 and 14.89 s, are below every base run here. The cause stays unexplained, but that row is an outlier, and the outcome's "16.5 → 2.3 s" understates the suite's gain: here it is 18.2 → 2.1 s. The base instruction count also differs (41.85 G here, 38.27 G for the author, for the same 46 compiles). The "fall by 7 to 8 percent" therefore depends on the checkout; here the fall is 15 percent. The outcome marks its explanation of the figure as inferred.
- F5 (informational, plausible). Compile-fail rows in `compile.tsv` change their `object` from `test/CMakeFiles/<target>.dir/core/<source>.o` to `<target>.o`. A comparison of the compile record across this revision, keyed by preset, source, and object, will see the 46 units as new. Neither decision mentions it. It was not checked whether any comparison uses that key across revisions.
- F6 (nit). `cmake/galoot_testing.cmake:35-37` wraps unevenly ("They are / read from galoot::core and the build's flag variables / rather than written out"); reflow it.

## Process

- W2: `2824f55` (the helper alone) builds and passes 97 of 97 on all seven presets. Since `2888987`, only files under `docs/` have changed, so the HEAD runs cover `2888987`. The other commits are records.
- W9: the two new tests that judge the change came in a separate commit (`2888987`) after the code, and the outcome lists them (item 5). No existing test, expected value, or literal changed.
- W7: every commit message says what and why, identifies agent work, and carries a `Co-Authored-By` line.
- C18: decisions 0095 to 0097 are recorded. In them, the code-generation-only diagnostics were spot-checked on GCC 16.2.1 and Clang 23.1.4 (`return_type`, `error_attribute`, `unused_static`, `uninitialized`, `fe_and_codegen`), and they agree with decision 0096. With `-c` and with `-fsyntax-only`, the 46 sources give the same exit (1) and the same output on gcc-debug, clang-debug, and gcc-release (138 of 138 pairs).
- C20: the outcome opens with its departures. Item 2 is confirmed: develop's `runs.tsv` has no row at `dc3c67c`, and `git diff dc3c67c b95e88e` touches only the plan and `compile.tsv`.
- S11: no provision, decision, plan, or agent citation in `cmake/galoot_testing.cmake`, `test/CMakeLists.txt`, or `test/compile_flags_check.cmake`, by reading and by `tests.citations` on all seven presets. The comments give reasons and read as a maintainer's, except for F6.
