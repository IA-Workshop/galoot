# 0002 CMake 4.x for Galoot

Date: 2026-10-31. Checked by: a research subagent (Claude Opus 5.5) working for the owner. Scope: what Galoot's build, tests, lint, and cost measurement need from CMake 4.4.5 and Ninja 1.13.2. No Galoot file other than this note was changed.

How each claim was checked:

- **Docs**: read in the CMake 4.4.5 documentation sources installed at `/usr/share/cmake/Help`, which generate the cmake.org v4.4 pages cited. Three web pages (presets, `discover_tests`, instrumentation) were spot-checked and match.
- **Local**: verified on this machine in a scratch project. One run per measurement; these are observations, not benchmarks.
- **Inferred**: my reasoning, not checked.

## Summary of recommendations

1. Require CMake 4.4 for Galoot's own build. The generated package files still load in older CMake, so users of an installed package are not bound to 4.4 (open question 1).
2. Make the core an `INTERFACE` library `galoot_core` with alias `galoot::core`, a `HEADERS` file set rooted at `include/`, and `cxx_std_23`. Install it with an export set, a config file, and a `SemanticVersion`, `ARCH_INDEPENDENT` version file. Keep warning, `-Werror`, and sanitizer flags out of the interface.
3. Set `CMAKE_CXX_SCAN_FOR_MODULES OFF` and `CMAKE_CXX_EXTENSIONS OFF` project-wide.
4. Compile doctest's implementation and `main` once, in a static library that every test program links, with `include/ext` as a `SYSTEM` include.
5. Build one program per `<component>_test.cpp`, register it as test `<module>.<component>` with labels `<module>` and `<module>.<component>`, and select with anchored regexes (`ctest -L '^core$'`).
6. Map a change to tests with `ninja -t deps`, and build only what a test needs with `CMAKE_TEST_BUILD_DEPENDS` and its `test_prep/<test>` targets.
7. Use presets schema 12, `binaryDir` `${sourceDir}/build/${presetName}`, and presets for GCC and Clang in debug, release, ASan+UBSan, and TSan, plus `lint` and workflow presets. Per-user settings go in an untracked `CMakeUserPresets.json`.
8. Only a shared FIFO jobserver caps load across separate worktrees; Ninja 1.13 and CTest join it on their own. Build presets must then not set `jobs`. The token count is a bound, so it is the owner's decision (C14, W10). Never use `ctest --test-load` below the current machine load; it stalls.
9. Record wall time, CPU time, and peak memory per translation unit with a small compiler launcher. Use Clang's `-ftime-trace` to see which constant evaluation costs time.
10. Lint through a `lint` preset: `CMAKE_CXX_CLANG_TIDY`, `VERIFY_INTERFACE_HEADER_SETS` on `galoot_core` (one translation unit per header, also linted), and a `format-check` target.
11. Keep unity builds and precompiled headers off by default. Revisit them later as opt-in, measured presets.
12. Leave C++ modules for later.

## Findings

### F1 Minimum version and policies

- **Claim.** CMake 4.0 removed compatibility with versions before 3.5; setting an older policy version is an error, while the `<min>...<max>` form still works. The policies introduced in 3.4 and earlier (CMP0000 to CMP0065) are listed as removed, so their `NEW` behavior is mandatory. Sources: https://cmake.org/cmake/help/v4.4/release/4.0.html and https://cmake.org/cmake/help/v4.4/manual/cmake-policies.7.html. Checked: Docs.
- **Claim.** A policy's `OLD` behavior may warn two years after its introduction and may be removed six years after, in a later major version. Source: cmake-policies(7). Checked: Docs. Inferred: policies from 3.20 (2021) on are the next candidates for removal.
- **Claim.** Of the 4.4 policies (CMP0211 to CMP0219), only CMP0211 touches this kind of project: a file may belong to at most one non-`HEADERS` file set. Source: https://cmake.org/cmake/help/v4.4/release/4.4.html. Checked: Docs.
- **Claim.** CMake 4.4.5 writes export files that begin `cmake_policy(VERSION 3.0.0...4.2)` and use `target_sources(FILE_SET)` only on CMake 3.23 and later. Checked: Local (read the generated `demoTargets.cmake`). Not tested on an older CMake, because none is installed.
- **Gotcha.** The 4.4 policies manual still shows `cmake_minimum_required(VERSION 3.10...4.3)`. Copy the form, not the numbers. Checked: Docs.

### F2 Header-only target, install, and export

- **Claim.** This pattern configured, installed, and was found by a separate consumer through `find_package(demo 0.1 CONFIG REQUIRED)` and `target_link_libraries(use PRIVATE demo::core)`. Checked: Local (names changed to Galoot's below).

```cmake
cmake_minimum_required(VERSION 4.4)
project(galoot VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(galoot_core INTERFACE)
add_library(galoot::core ALIAS galoot_core)
set_target_properties(galoot_core PROPERTIES EXPORT_NAME core)
target_sources(galoot_core INTERFACE FILE_SET HEADERS BASE_DIRS include
  FILES include/galoot/core/bits.hpp)            # list each header
target_compile_features(galoot_core INTERFACE cxx_std_23)

set(cfg ${CMAKE_INSTALL_DATADIR}/cmake/galoot)
install(TARGETS galoot_core EXPORT galootTargets FILE_SET HEADERS)
install(EXPORT galootTargets NAMESPACE galoot:: DESTINATION ${cfg})
configure_package_config_file(cmake/galootConfig.cmake.in
  galootConfig.cmake INSTALL_DESTINATION ${cfg})
write_basic_package_version_file(galootConfigVersion.cmake
  COMPATIBILITY SemanticVersion ARCH_INDEPENDENT)
install(FILES ${PROJECT_BINARY_DIR}/galootConfig.cmake
  ${PROJECT_BINARY_DIR}/galootConfigVersion.cmake DESTINATION ${cfg})
```

- **Claim.** `SemanticVersion` (new in 4.4) acts like `SameMinorVersion` for 0.y.z and like `SameMajorVersion` from 1.0. `ExactVersion` is deprecated. `ARCH_INDEPENDENT` is meant for header-only packages. Source: https://cmake.org/cmake/help/v4.4/module/CMakePackageConfigHelpers.html. Checked: Docs, and Local (requesting 1.0 from the installed 0.1.0 failed as expected).
- **Claim.** The consumer got `-isystem <prefix>/include` and `-std=gnu++23`. Galoot's own tests got plain `-I` and `-std=c++23`. Checked: Local. Inferred: warnings in Galoot headers are hidden from users but not from Galoot's tests, and S2's "extensions disabled" can be enforced only in Galoot's own builds.

### F3 Presets

- **Claim.** CMake 4.4.5 accepts presets schema 1 to 12 and rejects 13 with `Unrecognized "version" 13: must be >=1 and <=12`. Version 12 (4.4) renames `dev` to `author` under `warnings` and `errors`, fixes `${fileDir}` to the file containing it, and adds `testPassthroughArguments`. Source: https://cmake.org/cmake/help/v4.4/manual/cmake-presets.7.html. Checked: Docs and Local.
- **Claim.** A version 12 file with hidden bases (`base`, `gcc`, `clang`, `debug`, `release`, `asan`, `tsan`) and visible presets `gcc-debug`, `gcc-release`, `gcc-asan`, `clang-debug`, `clang-release`, `clang-asan`, `clang-tsan`, and `lint` parsed, configured, built, and passed its tests for each preset, each in its own `build/<preset>`. Workflow presets `gcc-debug` and `clang-asan` ran configure, build, and test in one command. A `CMakeUserPresets.json` inheriting from these presets overrode job pools and `jobs`. Checked: Local.
- **Claim.** For a build preset, `jobs: 0` means `--parallel` without a number (the build tool decides). For a test preset, `jobs: 0` means unbounded. Negative values are rejected since 4.3. Source: cmake-presets(7). Checked: Docs.

### F4 CTest

- **Claim.** CMake 4.4's `discover_tests()` runs a listing command at test time and registers one test per matching line. For doctest, `DISCOVERY_ARGS --list-test-cases --no-intro --no-version`, `DISCOVERY_MATCH "^([^=[].*)$"`, and `TEST_ARGS "--test-case=\\1"` gave one CTest test per case. Names with spaces worked, and each test ran exactly one case. Source: https://cmake.org/cmake/help/v4.4/command/discover_tests.html. Checked: Docs and Local. If the program is not built, `ctest` reports "discover_tests failed to run command" and exits with 8 (Local).
- **Claim.** doctest's `doctest_discover_tests` runs the test program in a post-build step. Source: https://github.com/doctest/doctest/blob/master/scripts/cmake/doctest.cmake. Checked: Docs. The script is not in `include/ext/doctest`. Inferred: the 4.4 command needs nothing more vendored and does not run sanitizer programs during the build.
- **Claim.** `-L`, `-LE`, `-R`, and `-E` are regular expressions, so `-L core` also matches `core.bits`. Checked: Local (`-L '^core\.sign$'` gave 1 test; `-LE '^case$'` gave 2; a test preset filter `{"include": {"label": "^core$"}}` gave 4).
- **Claim.** Selecting a test that requires a fixture also selects the fixture's setup test; `-FS` turns that off. Checked: Local.
- **Claim.** With a resource spec file, `RESOURCE_GROUPS` keeps slot use within capacity. Six tests needing 8 of 16 slots ran at most 2 at a time under `-j 6`. `RESOURCE_LOCK` serializes tests naming the same lock. Sources: https://cmake.org/cmake/help/v4.4/manual/ctest.1.html ("Resource Allocation") and https://cmake.org/cmake/help/v4.4/prop_test/RESOURCE_LOCK.html. Checked: Docs, and Local for resource groups. Inferred: both act within one `ctest` process, not across worktrees.
- **Claim.** `--rerun-failed` reruns the last failures and ignores `-L` and `-R`. `--timeout` sets a default for tests without `TIMEOUT`. `--test-dir` picks the build tree. Since 4.4, arguments after `--` reach the test programs. Source: ctest(1). Checked: Docs, and Local for `--rerun-failed` and `--`.
- **Claim.** `ctest -j` alone uses the processor count; `-j 0` is unbounded. `CTEST_PARALLEL_LEVEL` is an environment variable. A test preset's `jobs` overrides it, and `--parallel` on the command line overrides the preset. Source: https://cmake.org/cmake/help/v4.4/envvar/CTEST_PARALLEL_LEVEL.html. Checked: Docs.

### F5 Mapping a change to the tests it affects

- **Claim.** `ninja -t deps` lists each object's headers, taken from compiler depfiles. Filtering it for `bits.hpp` gave exactly the objects that include it directly or through `sign.hpp`. Checked: Local. The log exists only after a first build of the tree.
- **Claim.** With `CMAKE_TEST_BUILD_DEPENDS` on (4.4), Ninja gets `test_prep/<test>` and `test_prep/all`. `test_prep/core.sign` built only `sign_test` and the doctest library. Source: https://cmake.org/cmake/help/v4.4/variable/CMAKE_TEST_BUILD_DEPENDS.html. Checked: Docs and Local.
- Inferred workflow: find test objects that list a changed file, build their `test_prep/<test>`, and run `ctest -R '^(<a>|<b>)$'`. Module labels are the coarser fallback. W4 still requires the full suite before a merge.

### F6 Machine load from concurrent builds

- **Claim.** `CMAKE_JOB_POOLS` initializes the `JOB_POOLS` property, and `CMAKE_JOB_POOL_COMPILE` and `CMAKE_JOB_POOL_LINK` assign pools (4.4 adds a file-set `JOB_POOL_COMPILE`). Source: https://cmake.org/cmake/help/v4.4/prop_gbl/JOB_POOLS.html. Checked: Docs, and Local (preset cache variables produced `pool compile` with `depth = 8` and `pool link` with `depth = 2`).
- **Claim.** `CMAKE_BUILD_PARALLEL_LEVEL` is an environment variable read by `cmake --build`. Source: https://cmake.org/cmake/help/v4.4/envvar/CMAKE_BUILD_PARALLEL_LEVEL.html. Checked: Docs. Ninja's default here is 34 jobs (Local, `ninja --help`).
- **Claim.** Ninja 1.13 joins a GNU Make jobserver when `MAKEFLAGS` has `--jobserver-auth=fifo:PATH` and no `-j` is given. Sources: https://ninja-build.org/manual.html ("GNU Jobserver") and https://github.com/ninja-build/ninja/releases/tag/v1.13.0. Checked: Docs and Local. With 3 tokens in a FIFO, two separate Ninja builds ran at most 5 jobs together (3 tokens plus one implicit slot each). With `-j16`, Ninja ignored the jobserver and ran 16.
- **Claim.** CTest (3.29 and later, POSIX) takes one jobserver token per test. Source: ctest(1) ("Job Server Integration"). Checked: Docs and Local. Under the same FIFO, `ctest -j` and `ctest -j 12` both ran at most 4 tests at once.
- **Claim.** With machine load near 1.2, `ctest --test-load 1` started no test in 40 seconds (an earlier run waited over 4 minutes). With `--test-load 8` at load 1.5, it ran up to 6 at once. Checked: Local.
- **Claim.** `ninja -l N` drops to one job when the load average is above N but does not stall. The average lags, so `-j16 -l 8` still started 10 jobs at load 1.4. Checked: Local.
- Inferred recommendation. Pools, `-j`, and `-l` act within one build tree, so K worktrees with P jobs each can run K×P jobs at once. A shared FIFO jobserver is the only mechanism found that caps the whole machine. A wrapper creates the FIFO once, writes T tokens, and exports `MAKEFLAGS="-jT --jobserver-auth=fifo:/path"` to every agent. Presets omit `jobs`, and `CMAKE_BUILD_PARALLEL_LEVEL` stays unset. Without one, a per-worktree fallback is a compile pool sized from measured peak memory per file (pools cannot see memory), a link pool of 2, and a fixed `ctest -j`. Any T or pool depth is a bound on local resources, which C14 and W10 leave to the owner. The owner has said such use is tracked, not bounded, for now.

### F7 Measuring compile time and memory

- **Claim.** `CMAKE_CXX_COMPILER_LAUNCHER` prefixes every compile. Source: https://cmake.org/cmake/help/v4.4/prop_tgt/LANG_COMPILER_LAUNCHER.html. Checked: Docs and Local. A 25-line Python launcher that forks the compiler, calls `os.wait4`, and appends output, wall time, CPU time, and `ru_maxrss` to a locked TSV file worked with both compilers. For GCC it reported about 63 MiB per test file, so the `cc1plus` child is counted. The doctest implementation file took 1.0 s and 274 MiB (Clang) and 1.4 s and 345 MiB (GCC), against about 0.2 s for a small test file. `/usr/bin/time` is not installed.
- **Claim.** Clang's `-ftime-trace` writes a JSON trace per object with constant-evaluation events (`EvaluateAsConstantExpr`, `EvaluateAsInitializer`) and instantiation events. GCC 16's `-ftime-report` showed phases but no constant-evaluation line. Checked: Local. ClangBuildAnalyzer aggregates these traces (`--start`, `--stop`, `--analyze`), and ninjatracing turns `.ninja_log` into a Chrome trace. Sources: https://github.com/aras-p/ClangBuildAnalyzer and https://github.com/nico/ninjatracing. Checked: Docs only; neither is installed.
- **Claim.** `.ninja_log` (v7) records start ms, end ms, mtime, output, and hash per job. `ninja -d stats` shows Ninja's own overhead only. `cmake --profiling-format=google-trace --profiling-output=<file>` traces the configure step (2,854 events here). Source for the last: https://cmake.org/cmake/help/v4.4/manual/cmake.1.html. Checked: Local.
- **Claim.** CMake instrumentation (4.3), enabled by a query file in `<build>/.cmake/instrumentation/v1/query/`, records the duration of each configure, compile, link, and test step and can write a Google trace. Its memory fields are host-wide before and after, not per-command peaks. The 4.4 `compileTrace` option collects Clang traces. Source: https://cmake.org/cmake/help/v4.4/manual/cmake-instrumentation.7.html. Checked: Docs and Local. Only one of three Clang traces remained after indexing; I did not find out why.

### F8 clang-tidy, clang-format, and header self-containment

- **Claim.** `CMAKE_CXX_CLANG_TIDY` runs clang-tidy with each compile. With the repository's `WarningsAsErrors: '*'`, a finding fails the build. `SKIP_LINTING` excludes a target (4.2) or file set (4.4). Sources: https://cmake.org/cmake/help/v4.4/prop_tgt/LANG_CLANG_TIDY.html and https://cmake.org/cmake/help/v4.4/prop_tgt/SKIP_LINTING.html. Checked: Docs and Local. On the scratch code, the `operator[]` bounds checks and `readability-qualified-auto` failed the build, including inside doctest `CHECK(...)` arguments.
- **Claim.** `VERIFY_INTERFACE_HEADER_SETS` creates `<target>_verify_interface_header_sets` with one generated file per header that includes only that header, plus `all_verify_interface_header_sets`. These are excluded from `all` and never use unity builds or precompiled headers. Source: https://cmake.org/cmake/help/v4.4/prop_tgt/VERIFY_INTERFACE_HEADER_SETS.html. Checked: Docs and Local.
- **Claim.** These targets got `-Werror` from `CMAKE_COMPILE_WARNING_AS_ERROR` and ran clang-tidy from `CMAKE_CXX_CLANG_TIDY`, but not the `-Wall ...` flags attached to the test programs. Checked: Local. Inferred: put the S4 warning flags in a hidden preset's `CMAKE_CXX_FLAGS`, or in `$<BUILD_INTERFACE:...>` options on `galoot_core` when `PROJECT_IS_TOP_LEVEL`.
- **Claim.** Setting `CMAKE_VERIFY_INTERFACE_HEADER_SETS` globally created verification targets for every target, test programs included (CMP0209, and 4.4 creates them even with no headers). The docs reserve that variable for developers. Checked: Docs and Local. Inferred: set the property on `galoot_core` only.
- **Claim.** `compile_commands.json` listed the header-verification files as well as the tests. The docs say it does not work well with unity builds. Source: https://cmake.org/cmake/help/v4.4/variable/CMAKE_EXPORT_COMPILE_COMMANDS.html. Checked: Docs and Local.
- **Claim.** doctest 2.5.3's implementation file gave no warnings under the S4 flags with either compiler, using `-I` or `-isystem`. Checked: Local. Inferred: `SYSTEM` is a safeguard for now, and `.clang-tidy`'s `HeaderFilterRegex` already hides doctest from clang-tidy.
- **Claim.** A custom target running `clang-format --dry-run -Werror` on `$<TARGET_PROPERTY:galoot_core,HEADER_SET>` and the test sources (with `COMMAND_EXPAND_LISTS`) checks S3 without compiling. Checked: Local.

### F9 Sanitizers and hardening

- **Claim.** `-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer` stopped a program at a signed overflow with exit code 1 under GCC 16 and Clang 23. The `gcc-asan`, `clang-asan`, and `clang-tsan` presets built and passed. Checked: Local.
- **Claim.** libstdc++ defines `_GLIBCXX_ASSERTIONS` by default only without optimization. Source: https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_macros.html. Checked: Docs and Local (an out-of-bounds `operator[]` asserted at `-O0` and not at `-O2`, with both compilers). Inferred: release and sanitizer presets need an explicit `-D_GLIBCXX_ASSERTIONS` for S5.
- **Claim.** `_LIBCPP_HARDENING_MODE` (`_NONE`, `_FAST`, `_EXTENSIVE`, `_DEBUG`) applies to libc++ only. Source: https://libcxx.llvm.org/Hardening.html. Checked: Docs. libc++ is not installed, so Clang here uses libstdc++.
- **Claim.** `CMAKE_LINKER_TYPE` selects `LLD`, `MOLD`, `BFD`, and others, and also applies to `try_compile`. Source: https://cmake.org/cmake/help/v4.4/variable/CMAKE_LINKER_TYPE.html. Checked: Local. `LLD` worked with both compilers (`.comment` read `Linker: LLD 23.1.4`). `MOLD` failed the compiler check at configure time, because mold is not installed.
- **Claim.** `CMAKE_COMPILE_WARNING_AS_ERROR` initializes `COMPILE_WARNING_AS_ERROR`, and `cmake --compile-no-warning-as-error` overrides it. Source: https://cmake.org/cmake/help/v4.4/prop_tgt/COMPILE_WARNING_AS_ERROR.html. Checked: Docs and Local.

### F10 Unity builds and precompiled headers

- **Claim.** Projects should leave `UNITY_BUILD` to the developer and should not push precompiled headers onto consumers of exported targets. Sources: https://cmake.org/cmake/help/v4.4/prop_tgt/UNITY_BUILD.html and https://cmake.org/cmake/help/v4.4/command/target_precompile_headers.html. Checked: Docs.
- **Claim.** In a synthetic test, a header built a 262,144-entry sign table at compile time and six files used it. Total compile CPU at `-j1`:

| Compiler | Separate files | Unity | PCH of the header | PCH that uses the table |
| --- | --- | --- | --- | --- |
| GCC 16 | 8.9 s | 1.6 s | 8.8 s | 2.6 s for the PCH, then 0.2 s per file |
| Clang 23 | 13.9 s | 2.3 s | 13.7 s | 2.3 s for the PCH, then 2.3 s per file |

Peak memory was about 200 MiB per job, except 389 MiB to build GCC's table-using PCH. Checked: Local, one run each. Inferred: unity computes a shared table once, which saves CPU (machine load) but not wall time under parallel builds. GCC's PCH kept the evaluated table; Clang's did not. Neither should be a default: both hide missing includes (S6) and merge the per-file costs C14 asks Galoot to record.

### F11 C++ modules

- **Claim.** CMake scans C++20-and-later sources for modules by default (CMP0155), and a scanned source is left out of unity builds. Sources: https://cmake.org/cmake/help/v4.4/policy/CMP0155.html and https://cmake.org/cmake/help/v4.4/manual/cmake-cxxmodules.7.html. Checked: Docs and Local. Without `CMAKE_CXX_SCAN_FOR_MODULES OFF`, the scratch build grew from 7 to 13 steps.
- **Claim.** `import std` needs `CMAKE_EXPERIMENTAL_CXX_IMPORT_STD` set to `f35a9ac6-8463-4d38-8eec-5d6008153e7d` before `project()`, and the value may change between versions. Source: https://gitlab.kitware.com/cmake/cmake/-/blob/v4.4.5/Help/dev/experimental.rst. Checked: Docs and Local. With it, `CMAKE_CXX_COMPILER_IMPORT_STD` was `23;26` for both compilers, and an `import std;` program built (3.0 s GCC, 2.3 s Clang, standard library module included) and ran.
- Inferred: C13 requires the core to work from headers alone, so modules can only be an optional extra, such as a `galoot.core` module wrapping the headers. Measure that once the core exists.

### F12 Other gotchas

- A build preset `jobs` value or a set `CMAKE_BUILD_PARALLEL_LEVEL` passes `-j` to Ninja and so turns off the jobserver (F6). Checked: Docs, and Local for Ninja.
- CMake 4.4 deprecates `-Wdev` (now `-Wauthor`), `--warn-uninitialized` (now `-Wuninitialized`), and `CMAKE_WARN_DEPRECATED` and `CMAKE_ERROR_DEPRECATED` (now `cmake_diagnostic`). Source: 4.4 release notes. Checked: Docs.
- Since 4.1, `install(TARGETS)` no longer depends on file sets being defined before it. Source: https://cmake.org/cmake/help/v4.4/release/4.1.html. Checked: Docs.

## Local verification

The scratch project, which may be cleared, is at `/tmp/claude-1000/-home-brianm-projects-utils-math-core/3f0e5da4-7683-484b-ba15-aca37a023195/scratchpad/cmk`. It has an `INTERFACE` library with two headers, a doctest library built from `test_main.cpp` with `-isystem /home/brianm/projects/utils/math-core/galoot/include/ext`, two `<component>_test.cpp` programs, and the presets of F3.

```bash
cmake --list-presets=all                    # 8 configure, 8 build, 8 test, 2 workflow
cmake --workflow --preset gcc-debug         # configure, 7 build steps, 4/4 tests passed
for p in clang-debug clang-release gcc-release gcc-asan clang-asan clang-tsan; do
  cmake --preset $p && cmake --build --preset $p && ctest --preset $p
done                                        # each: 100% tests passed out of 4
cmake --preset lint && cmake --build --preset lint   # stopped on clang-tidy findings
cmake --install build/gcc-debug             # consumer found, built, and ran it
ninja -C build/gcc-debug all_verify_interface_header_sets  # 2 header TUs
ninja -C build/gcc-debug -t deps            # header-to-object map
cmake --build build/gcc-debug --target test_prep/core.sign
```

The jobserver, load-limit, resource-group, unity, PCH, and `import std` checks used small separate projects in the same scratch directory.

## Open questions

1. Should Galoot require exactly CMake 4.4, or a range such as `3.28...4.4` without the 4.4-only features (`discover_tests`, `CMAKE_TEST_BUILD_DEPENDS`, presets 12, `SemanticVersion`)? This is the owner's dependency decision.
2. Should agents share a FIFO jobserver, and with how many tokens? The owner's notes mention a machine-wide lock and a run recorder (`agent/records/workbench/tools/runrec`) that I could not find under `/home/brianm/projects/utils/math-core`, so how they would fit together is open.
3. One CTest test per program, or one per doctest case? Per case selects and reruns more finely but starts the program once per case.
4. The `operator[]` checks in `.clang-tidy` will fire inside test assertions. Should `test/` get its own `.clang-tidy` with recorded exclusions (S1, S4)?
5. Not installed: GNU `time`, mold, libc++ (needed to test `_LIBCPP_HARDENING_MODE`), ClangBuildAnalyzer, and ninjatracing. The first three are in the configured repositories (`sudo pacman -S --needed time mold libc++ libc++abi`); the last two are not packaged.
6. Not confirmed: link-step measurement through `CMAKE_CXX_LINKER_LAUNCHER`, why instrumentation kept one compile trace, and loading the export files in a CMake older than 4.4. "Professional CMake" was not consulted; every claim rests on official or upstream docs or a local check.
