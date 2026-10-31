# 0001 doctest 2.5.3 for Galoot tests

- Subject: doctest 2.5.3, vendored unchanged at `include/ext/doctest/`. Upstream clone `/home/brianm/projects/utils/doctest`, tag v2.5.3, commit 2d0a9359 (2026-07-07). The vendored files are byte-identical to the clone (`diff -rq`).
- Read and checked by: Claude (agent), 2026-10-31. Not yet reviewed by the owner.
- Toolchain: GCC 16.2.1, Clang 23.1.4, CMake 4.4.5, Ninja 1.13.2, LLD 23.1.4. Machine: Intel i9-14900K, 32 threads, 125 GB RAM.
- "Docs" means `doc/markdown/` in the clone. "Source" means `doctest/parts/` in the clone, which is the same as the vendored copy.

## Summary of recommendations

1. Build one test executable per component (`<component>_test`). Compile the runner once, as a CMake OBJECT library from `include/ext/doctest/doctest.cpp`, with `DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN` given as a compile definition. Never define the runner in each test file.
2. Include `<doctest/doctest.h>` through a SYSTEM include of `include/ext`. The parts layout gives no compile-time gain and is not meant for consumers.
3. Define `DOCTEST_CONFIG_SUPER_FAST_ASSERTS` for the whole test build. It roughly halves assertion compile cost. Its one real cost, losing per-assert exception catching, matters little because the core throws nothing.
4. Do not use bare `doctest::Approx`. Its default tolerance is tuned to `float`, it adds an absolute floor, and it converts everything to `double`. Write a small project helper that takes a named tolerance (V12). Log floating values with `std::format("{}", x)`, because doctest prints only six significant digits.
5. Use doctest's `--rand-seed` as the single seed source (V7). Read it in tests through `doctest::getContextOptions()->rand_seed`. Log it with `INFO` in every randomized test.
6. Do not put a `SUBCASE` inside a loop. In 2.5.x this is very slow (issue #1208). Use a plain loop with `CAPTURE`, or `GENERATE` for short literal lists.
7. On Clang 22 and later, write at most one declaring doctest macro per line. Two `TEST_CASE`, `INFO`, or `CAPTURE` uses on one line fail to compile.
8. Register tests with `add_test` per executable, with a `LABELS` entry per component and a `FAIL_REGULAR_EXPRESSION` that catches a run of zero test cases. Treat `doctest_discover_tests` as optional. Its scripts are not vendored, and its `ADD_LABELS` option is slow.
9. Write compile-fail tests as `EXCLUDE_FROM_ALL` targets that a CTest test builds, with `PASS_REGULAR_EXPRESSION` set to the expected `static_assert` message.
10. Build tests with exceptions enabled. If a no-exceptions test build is ever needed, define `DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS` globally.
11. Keep doctest's POSIX signal handler in the sanitizer build, because it names the crashed test case. To investigate a crash, rebuild the runner with `DOCTEST_CONFIG_NO_POSIX_SIGNALS`, because ASan then reports the true fault address.

## Findings

### Build structure and compile time

Confirmed:

- F1. `DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN` or `DOCTEST_CONFIG_IMPLEMENT` goes in exactly one source file of a binary. Upstream advises a separate file with nothing else in it. Source: docs `configuration.md`, `main.md`, `faq.md` ("How to use with multiple files?"). Checked: read in docs.
- F2. The vendored `doctest.cpp` only defines `DOCTEST_CONFIG_IMPLEMENT` and includes `doctest/doctest.h`. Upstream's `doctest_with_main` target compiles this file with `DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN` as a definition. Source: `CMakeLists.txt` in the clone, commit da544d5. Checked: read in source.
- F3. The parts layout is the development form. `scripts/assemble.py` generates `doctest.h` from it. A consumer of the parts layout must compile the 34 files in `parts/private/*.cpp` as the runner. Source: `scripts/assemble.py`; changelog for v2.5.0 on the GitHub releases page (https://github.com/doctest/doctest/releases). Checked: read in source and on the web.
- F4. For test files, the single header and the parts header cost the same. The runner costs about one second per copy. Building the runner from the parts files costs far more. Checked: measured locally (Table 1).
- F5. `DOCTEST_CONFIG_SUPER_FAST_ASSERTS` turns each assert into one function call. It implies `DOCTEST_CONFIG_NO_TRY_CATCH_IN_ASSERTS`, so an exception in an assert expression ends the test case. A debugger stops one frame inside doctest. Source: docs `configuration.md`, `faq.md`. Checked: read in docs. The savings were measured locally (Table 2).
- F6. Other relevant options: `DOCTEST_CONFIG_NO_MULTITHREADING` (runner TU only) speeds up single-threaded runs. `DOCTEST_CONFIG_REQUIRE_STRINGIFICATION_FOR_ALL_USED_TYPES` stops compilation when an asserted type cannot be printed. `DOCTEST_CONFIG_NO_EXCEPTIONS` must be global. Source: docs `configuration.md`. Checked: read in docs.
- F7. A precompiled header with nine standard headers and doctest cut a small test TU from 0.20 s to 0.064 s (GCC) and from 0.22 s to 0.057 s (Clang). The doctest header alone costs about 20 ms, so the gain comes from the standard headers. Checked: measured locally, `g++ -x c++-header` and `clang++ -include-pch`.
- F8. LLD linked a runner and one test in 0.06 s; the default linker took 0.16 s. Checked: measured locally.

Table 1. One TU, `-O0`, pinned to one performance core (`taskset -c 2`), median of 9 to 21 runs. Instruction counts from `perf stat -e instructions:u`.

| TU | g++ time | g++ instructions | clang++ time | clang++ instructions |
| --- | --- | --- | --- | --- |
| Empty function | about 0.02 s | 62 M | about 0.02 s | 41 M |
| One test, `doctest/doctest.h` | 0.019 s | 191 M | 0.021 s | 128 M |
| One test, `doctest/parts/doctest_fwd.h` | 0.018 s | 177 M | 0.021 s | 132 M |
| Runner TU (`IMPLEMENT_WITH_MAIN`) | 0.98 s | 9.7 G | 0.78 s | 6.9 G |
| Runner TU at `-O2` | 2.22 s | | 1.30 s | |
| 34 parts runner TUs, one at a time | 18.6 s | | 20.5 s | |
| 34 parts runner TUs, 32 jobs | 1.43 s | | 1.58 s | |

Times under about 50 ms are noisy, because idle cores sit at 800 MHz. The instruction counts are the stable measure.

Table 2. One TU with 30 test cases and 300 asserts on `constexpr` table entries, pinned, median of 15 runs. SFA means `DOCTEST_CONFIG_SUPER_FAST_ASSERTS`.

| Compiler, level | `CHECK(a == b)` | with SFA | `CHECK_EQ(a, b)` | with SFA |
| --- | --- | --- | --- | --- |
| g++ `-O0` | 0.249 s | 0.132 s | 0.199 s | 0.080 s |
| g++ `-O2` | 0.947 s | 0.358 s | 0.734 s | 0.162 s |
| clang++ `-O0` | 0.148 s | 0.103 s | 0.105 s | 0.064 s |
| clang++ `-O2` | 0.316 s | 0.160 s | 0.224 s | 0.091 s |

Table 3. CMake and Ninja with 32 jobs, GCC, LLD: 20 executables with three test cases each and one shared OBJECT runner. Clean build: 1.43 s with `add_test`, 1.77 s with `doctest_discover_tests`. Rebuild after touching one test file: 0.21 s and 0.32 s. For comparison, 20 copies of a runner TU took 23.0 s of compile time one at a time, or 1.94 s with 32 jobs.

Commands: `EXT=galoot/include/ext`, `W="-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror"`, then `taskset -c 2 g++ $W -isystem $EXT -O0 -c <file>.cpp -o x.o` (the same with `clang++`), timed by a Python `perf_counter` loop. The parts runner was built with `-DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN` on each `parts/private/*.cpp`. It linked and ran.

Inferred:

- One executable per component costs one link, about 0.06 s, plus one test TU per change. A single executable would relink everything on every change and could not be selected by component in CTest. Per-component executables fit S14 and the parsimony goal better.
- A precompiled header becomes worth adding once Galoot's own headers are heavy. That should be measured then, not assumed.

### Compile-time testing

Confirmed:

- F9. doctest has no compile-time assertion facility. A `static_assert` at namespace scope or inside a `TEST_CASE` body sits beside doctest tests and fails the build, not the run. A `consteval` call with constant arguments works inside `CHECK`. Checked: built locally on both compilers (`CHECK(demo::twice(2) == 4)` with a `consteval` function).
- F10. `TEST_CASE_TEMPLATE_DEFINE` and `TEST_CASE_TEMPLATE_APPLY` with `std::tuple<...>` work over wrapper types that carry class-type template arguments, such as `template <metric M> struct sig`. Braces do not protect commas inside macro arguments, so give each signature an alias first. `TYPE_TO_STRING_AS("pga3", pga3)` names the instances `dimension matches metric<pga3>`. This macro is in the source (`parts/public/macros.h`) but not in the docs. Checked: built and run locally on both compilers.
- F11. A compile-fail test works as an `EXCLUDE_FROM_ALL` executable plus `add_test(COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target <name>)`, with `PASS_REGULAR_EXPRESSION` set to the `static_assert` message. CTest ignores the exit code when a pass expression is set, so the test passes only if that specific message appears. Checked: measured locally with CMake 4.4.5; it took 0.12 s.

Inferred:

- `WILL_FAIL` alone would also pass on an unrelated error, such as a typo, so it cannot show that a rejection still happens for the intended reason. `try_compile` at configure time runs on every configure and slows it, so it suits feature probes, not a growing set of rejection tests.

### Assertions and printing

Confirmed:

- F12. `Approx` holds `double` only. The default epsilon is `100 * FLT_EPSILON`, about 1.19e-5. The default scale is 1.0. Equality is `|a - v| < eps * (scale + max(|a|, |v|))`, a strict `<`. So `Approx(x).epsilon(0)` never matches, not even `x` itself. With scale 1, every pair of values below about 1e-5 compares equal. Source: `parts/private/matchers/approx.cpp`; docs `assertions.md`; issue #389 (https://github.com/doctest/doctest/issues/389), where a contributor agrees the `float` default is "not ideal" but cannot change it before 3.x. Checked: read in source, read on the web, and confirmed by six `CHECK`s run locally.
- F13. `long double` and other types convert silently to `double` inside `Approx`. The conversion is in a system header, so `-Wconversion` does not report it. Checked: built locally.
- F14. doctest prints floating values with the default stream precision of six digits, so a failed `CHECK(0.1 + 0.2 == 0.3)` prints `0.3 == 0.3`. `INFO("a = ", std::format("{}", a))` printed `0.30000000000000004`. A `const char*` prints as a pointer unless `DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING` is set, so `INFO(s.c_str())` logs an address. Source: `parts/private/string.cpp`. Checked: read in source and run locally.
- F15. `CHECK` continues after a failure; `REQUIRE` ends the test case by throwing. `CHECK_MESSAGE` is a scoped `INFO` plus `CHECK`. `INFO` and `CAPTURE` are evaluated lazily, only on failure. Source: docs `assertions.md`, `logging.md`. Checked: read in docs and seen locally.
- F16. A type prints through `operator<<` found by argument-dependent lookup, a `toString` overload in its namespace, or a `doctest::StringMaker<T>` specialization; otherwise it prints `{?}`. `operator<<` worked for a multivector-like aggregate and `StringMaker` for a rational type. Source: docs `stringification.md`. Checked: run locally.
- F17. doctest suppresses `-Wsign-compare` and `-Wsign-conversion` inside its comparison templates by default. `CHECK(u == i)` with `unsigned` and `int` compiled cleanly under the project flags. Source: docs `configuration.md` (`DOCTEST_CONFIG_NO_COMPARISON_WARNING_SUPPRESSION`). Checked: built locally.

Inferred:

- Because `CHECK(a == b)` compares inside a doctest template, `-Wfloat-equal` on user floats is reported inside the doctest header, and `-isystem` hides it. The compiler therefore cannot enforce S15 ("every comparison within a tolerance names its tolerance") through `CHECK`. Review or a project helper has to.

### Parameterized and randomized tests

Confirmed:

- F18. `TEST_CASE_TEMPLATE("name", T, float, double, Rational)` registers `name<float>`, `name<double>`, and `name<demo::Rational>`, given `TYPE_TO_STRING` for the class type. Source: docs `parameterized-tests.md`. Checked: run locally.
- F19. A test case with subcases runs its body once per leaf path. Two siblings, one with two children, gave three entries. Source: docs `tutorial.md`, `testcases.md`. Checked: run locally.
- F20. `GENERATE` is new in 2.5.0. It runs the body once per value; the last call in source order varies fastest. It must not sit inside control flow that varies independently of the traversal, for example on randomness. Source: docs `parameterized-tests.md`; commit c27d4e6. Checked: read in docs and run locally.
- F21. A `SUBCASE` inside a loop is very slow in 2.5.x. Locally, 256 named subcases ran in 0.27 s and 1,024 in 7.1 s, against 0.01 s for a plain loop with `CAPTURE`. Issue #1208 (https://github.com/doctest/doctest/issues/1208) traces this to the traversal rework in PR #1056. As of 2026-10-20 it is open, with no fix. Checked: read on the web and measured locally (GCC `-O2`).
- F22. `--order-by=rand` shuffles with `std::srand(seed)` and `std::rand()`. The seed defaults to 0, so repeated runs give the same order. The seed is an `unsigned` (32 bits). A non-numeric value such as `--rand-seed=abc` silently becomes 0, and `-1` becomes 4294967295. The console reporter does not print the seed; the XML reporter records it in `<Options>`. Source: `parts/private/context.cpp`, `parts/public/context/options.h`. Checked: read in source and run locally.
- F23. A test can read the seed through `doctest::getContextOptions()->rand_seed`. A test that seeds `std::mt19937_64` from it and logs it with `INFO` printed `seed = 42` on failure under `--rand-seed=42`. Checked: run locally.

Inferred:

- A fixed default seed keeps CI deterministic. A scheduled job can pass a fresh seed (`--rand-seed=$(date +%s)`) and keep the XML report as the record. A failure found that way becomes a fixed test (V7).

### Selection, exit codes, and reporters

Confirmed:

- F24. Filters are comma-separated wildcard lists and are case-insensitive by default: `-tc`, `-tce`, `-ts`, `-tse`, `-sf`, `-sfe`, `-sc`, `-sce`. `-sc` does not run subcases nested inside the selected one; upstream plans to change this in 2.6 (#519). Queries: `--count`, `--list-test-cases`, `--list-test-suites`. Source: docs `commandline.md`. Checked: read in docs; `-ltc`, `-lts`, `-c -ts=random`, and `-tc` were run locally.
- F25. Decorators include `skip`, `may_fail`, `should_fail`, `expected_failures`, `timeout`, and `test_suite`. `timeout` marks a test failed but does not stop it. Source: docs `testcases.md`. Checked: read in docs; `may_fail` and `test_suite` were run locally.
- F26. The run returns `EXIT_FAILURE` (1) when any test case failed and 0 otherwise. A filter that matches nothing returns 0 and prints `Status: SUCCESS!`. So does a run whose only failures are in `may_fail` tests. `--no-exitcode` forces 0. Source: `parts/private/context.cpp`. Checked: read in source and run locally.
- F27. The reporters are `console` (streaming), `xml` (streaming, doctest's own format), and `junit` (buffered, written at the end). `--out=<file>` redirects output. Source: docs `reporters.md`. Checked: read in docs; all three were listed with `-lr` and run locally.
- F28. A CTest property `FAIL_REGULAR_EXPRESSION "test cases: +0 \\|"` turned a mistyped filter into a failed test. Checked: run locally with CTest.

Inferred:

- doctest cannot tell which tests a change affects. Ninja already rebuilds only the test executables whose sources or headers changed. Labels per component (`ctest -L core`) or a name filter (`ctest -R`) then select what to run. `ctest --rerun-failed` repeats only failures.
- For CI, pass `--reporters=junit --out=<dir>/<component>.xml` per executable, so parallel tests do not write the same file. The console output already suits CTest's log.

### Exceptions, sanitizers, warnings, and clang-tidy

Confirmed:

- F29. doctest detects `-fno-exceptions` by itself. A `REQUIRE` then fails to compile with "Exceptions are disabled! Use DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS ...". With that option, `REQUIRE` acts like `CHECK` and the exception asserts compile to nothing. A runner and tests built with `-fno-exceptions` and no `REQUIRE` ran normally. Source: `parts/public/config.h`, `parts/public/macros.h`; docs `configuration.md`, which still says the macros are "undefined". Checked: read in source and built locally on both compilers.
- F30. Under `-fsanitize=address,undefined -fno-sanitize-recover=all -D_GLIBCXX_ASSERTIONS`, a heap overflow, a signed overflow, and a null load were each reported at the right line, and the run exited with 1. The process stops, so later test cases do not run, and the stack shows `DOCTEST_ANON_FUNC_<n>` instead of the test name. Checked: run locally on both compilers.
- F31. doctest's POSIX handler catches `SIGSEGV`, prints `TEST CASE: <name>` and "test case CRASHED", restores the previous handler, and re-raises. ASan then reports, but the fault address it prints is wrong (`0x03e8...`, not `0x0`). With `DOCTEST_CONFIG_NO_POSIX_SIGNALS` in the runner, ASan reports the true address but doctest prints no test name. Source: `parts/private/signals.cpp`. Checked: read in source and run locally with ASan alone.
- F32. With the project flags plus `-Werror`, a test file and the runner produced no diagnostics on GCC 16.2.1 or Clang 23.1.4, at `-O0` and `-O2`, with `-isystem` and even with a plain `-I`. doctest wraps its headers in warning push and pop pragmas. Source: `parts/public/warnings.h`. Checked: built locally.
- F33. With Galoot's `.clang-tidy`, a test file that uses `TEST_CASE`, `SUBCASE`, `CHECK`, and `TEST_CASE_TEMPLATE` produced no findings in user code. doctest's macros carry `NOLINT` comments; upstream fixed lint noise up to clang-tidy 22 (commit 3f4a605). One finding remains: `#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN` breaks the `GALOOT_` macro-prefix rule. Passing the macro with `-D` avoids it. Checked: run locally with clang-tidy 23.1.4.
- F34. On Clang 22 and later, doctest names its hidden symbols with `__LINE__` instead of `__COUNTER__`, because Clang now warns that `__COUNTER__` is a C2y extension under `-Wpedantic`. Two `TEST_CASE`s produced by one macro, or two `INFO`s on one line, then fail with "redefinition of 'DOCTEST_ANON_VAR_3'". GCC is not affected. Two `SUBCASE`s or `CHECK`s on one line are fine. Defining `DOCTEST_COUNTER=__COUNTER__` with `-Wno-c2y-extensions` also works. Source: `parts/public/utility.h`; commit ad5ce85 (#1079); nlohmann/json PR #5243 (https://github.com/nlohmann/json/pull/5243). Checked: read in source, read on the web, and built locally.

### CMake and CTest integration

Confirmed:

- F35. CMake 4.4.5 ships no doctest module, only `GoogleTest`. `doctest_discover_tests` is in the clone's `scripts/cmake/doctest.cmake` and `doctestAddTests.cmake`, which Galoot has not vendored. Checked: `cmake --help-module-list` and reading the source.
- F36. `doctest_discover_tests` adds a post-build step that runs `<exe> --list-test-cases` and writes one CTest test per test case, each run as `--test-case=<name>`. Options include `TEST_SPEC`, `EXTRA_ARGS`, `PROPERTIES`, `ADD_LABELS`, and `JUNIT_OUTPUT_DIR`. `ADD_LABELS 1` runs the executable once more per test case, to read its suite name as a label. Source: `scripts/cmake/doctest.cmake`, `doctestAddTests.cmake`. Checked: read in source and run locally.
- F37. Discovery cost for a 60-case executable: 0.04 s without labels, 0.33 s with `ADD_LABELS`. An ASan build starts in 0.036 s against 0.009 s, so label discovery costs about four times more there. Checked: measured locally.
- F38. Two test cases with the same name (allowed by doctest) became two CTest entries that each ran both cases. Checked: run locally.

Inferred:

- `add_test` per executable, with labels set in CMake, gives component selection at no build-time cost. Discovery adds per-case granularity and per-case timing. That suits slow suites, which Galoot does not have yet.

## Open questions

1. Does Galoot want `DOCTEST_CONFIG_SUPER_FAST_ASSERTS` from the start? It is a decision (C18), and its savings should be checked on real Galoot tests.
2. Should the runner be a custom `main` (`DOCTEST_CONFIG_IMPLEMENT`) that prints the seed once and sets a project default, or the stock `main`?
3. The floating-point helper (recommendation 4) needs a design: relative, absolute, or ULP-based, with the tolerance named per V12. Issue #389 says commits after 2.5.3 add a Catch2-style `margin()`. That is not in the clone at v2.5.3 and was not confirmed.
4. Can a test-runner script map changed files to component labels reliably, or is Ninja's rebuild set enough?
5. When will #1208 be fixed? Revisit `SUBCASE` loops after a release with the fix.
6. Not checked: thread sanitizer (S5), libc++ in place of libstdc++, `DOCTEST_CONFIG_USE_STD_HEADERS`, and behavior when the runner and the tests are built with different exception settings.
