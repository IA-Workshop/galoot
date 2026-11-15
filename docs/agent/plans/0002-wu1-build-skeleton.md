# 0002 WU1: build skeleton

- Status: closed; the unit merged into `develop` by `eb9a8fa` on 2026-10-31 (criterion 15) (status corrected 2026-11-11, from `git log --merges` on `develop`). Status text as written before that correction, kept as history: implemented on `feature/build` (2026-10-31). The W5 review (`docs/agent/records/reviews/2026-10-31-wu1-build-skeleton.md`) asked for B1 to B3 and N1 to N4, which section 15 records as done. Criteria 1 to 14 are met; criterion 15 is the merge into `develop`, made by the coordinating session.
- Unit: WU1 of plan 0001 (milestone M0). Branch: `feature/build`, in the worktree `.claude/worktrees/build`.
- Written by: Claude (agent, build-expert, Opus 5.5), 2026-10-31, against `develop` at 2862040, where P13 already includes the decision 0018 sentence. Inputs: the governance (CONSTITUTION, WORKFLOW, PERFORMANCE, STYLE, VVUQ), research notes 0001 and 0002, plan 0001, and decisions 0006, 0011 to 0014, and 0018.
- Probes: the facts marked "probed" below come from throwaway runs in `docs/agent/workbench/` on 2026-10-31. They were deleted afterward and are not V1 evidence; WU1 repeats what it relies on (V10).

## 1. Departures and ambiguities (C20)

1. No TSan preset. The build-expert agent file asks for one; the brief and S5 ("tests that run code in more than one thread") say it is not applicable until a test runs threads. WU1 records TSan as not applicable in a decision record and adds the preset when the first threaded test exists.
2. Header self-containment is checked in every preset, not only `lint`. `VERIFY_INTERFACE_HEADER_SETS` is set on `galoot_core` only (as build-expert requires), whenever Galoot is the top-level project. Its targets are not in `all`, so the cost is paid only when `all_verify_interface_header_sets` is built. This lets S6's "compiles on its own" be checked on both compilers.
3. `tools/timed` changes in WU1. Decision 0018 says `tools/timed` and the launcher grow the new fields, and the brief asks for them, so WU1 changes `tools/timed`. Its command line and the nine columns P13 names stay the same. WU1 also makes `tools/timed` refuse to start a run when the shared job-slot pool is missing (section 4.9), because Ninja fails open without it (probed). The brief does not name this guard, so the owner should confirm it (open question 6).
4. The runtime half of the checking-mode test cannot be a `WILL_FAIL` test, although DoD-20 says so. Probed with CTest 4.4.5: a test whose process dies by `SIGABRT` is reported as "Subprocess aborted", and neither `WILL_FAIL` nor `PASS_REGULAR_EXPRESSION` turns that into a pass. WU1 runs the program from a `cmake -P` script that uses `execute_process`, which returns "Subprocess aborted" as a string, and checks both the abort and the literal message on stderr. This is stricter than `WILL_FAIL`. DoD-20's wording should change when plan 0001 is next edited.
5. Measurement runs use `-j1`. A pinned measurement (P14) builds one translation unit at a time on one CPU, so it passes `-j1`, which makes Ninja ignore the jobserver. That uses fewer slots than any jobserver build. Every other build adds no `-j`.
6. Reading of "record format per P1/P13, runs.tsv". Per-TU rows need columns that a run row does not have (source, object, compiler), so they go to a sibling file, `docs/agent/records/performance/compile.tsv`. It follows the same conventions: tab-separated, a header line, UTC times, and union merge through `.gitattributes`. `runs.tsv` keeps its nine P13 columns first and gains the decision 0018 columns after them. See open question 5 for the existing rows.
7. Reading of "a compile-fail helper that matches a literal message regex". The helper takes the literal message and escapes it into a regular expression itself, so a test cannot pass on a loose pattern such as `.*` (DoD-19: "matching the literal message").
8. Decisions plan 0001 assigns to WU0 that are still unwritten: CMake 4.4, presets, doctest with its C13 continuity record, and the build choices of note 0006 item 18. They belong to the build lane, so WU1 writes them (section 4.10). The golden-value format is not a build decision and stays out of WU1.
9. Install and export are left out. Build-expert's structure section asks for an export set, a package config file, and a version file. Neither the brief nor plan 0001's WU1 asks for them, and they have no user yet (C12). They are deferred to the first unit with an installed user (open question 11).

## 2. Goal

Give Galoot a build that later units extend without changing its shape. The build must:

- compile and run doctest tests one program per component, selectable by label;
- check compile-time rejections by their literal messages;
- record per-TU and per-run cost (P1, P13, decision 0018);
- declare its checking mode, warnings, floating-point options, and sanitizers explicitly (C8, S2 to S5, S15);
- check the S3 to S6 style rules mechanically;
- share the machine's job-slot pool (P11).

## 3. Definition of done

WU1 is done when every acceptance criterion in section 5 holds on the head of `feature/build` and that head has merged into `develop`. The merge needs:

- the checks P9 sets for `develop`: the full suite on GCC and Clang, one of them under ASan and UBSan, and the lint and format checks;
- a W5 review record from `blind-reviewer`;
- the evidence of section 6 committed;
- the decision records of section 4.10;
- this plan closed with its outcome (W3).

If the job-slot pool is not running, no build starts and the unit halts under W8.

## 4. Approach

### 4.1 Machine and toolchain (checked 2026-10-31)

| Tool | Installed | Package (platform-supplied, decision 0006) |
| --- | --- | --- |
| CMake, CTest | 4.4.5 | `cmake 4.4.5-1.1` |
| Ninja | 1.13.2 | `ninja 1.13.2-3.1` |
| GCC | 16.2.1 20260810 | `gcc 16.2.1+r23+gd564253eb6c8-3` |
| Clang, clang-tidy, clang-format | 23.1.4 | `clang 23.1.4-2` |
| LLD | 23.1.4 | `lld 23.1.4-2` |
| perf | 7.3.1 | `perf 7.3.1-1` |
| Python | 3.14.8 | `python 3.14.8-1` |
| taskset, flock | util-linux 2.42.4 | |
| GNU time, mold, libc++ | not installed | not needed by WU1 |

CMake 4.4 is installed (4.4.5), so `cmake_minimum_required(VERSION 4.4)` and presets schema 12 work as written. The machine is an i9-14900K with 32 hardware threads on 24 cores. CPUs 0 to 15 are 8 performance cores, paired as 2k and 2k+1 (the `cpu_core` PMU). CPUs 16 to 31 are 16 efficiency cores (`cpu_atom`). It has 125 GiB of RAM. `perf_event_paranoid` is 2, and `/sys/class/powercap/intel-rapl:0/energy_uj` (package-0) is world-readable; the counter wraps at `max_energy_range_uj` = 262143328850.

### 4.2 Shared job-slot pool (P11): requirements and current state

Requirements, from P11, P14, build-expert, and note 0002 F6:

- a GNU Make FIFO jobserver holding the owner's 24 tokens;
- one long-lived process that keeps the FIFO open, because tokens vanish when the last holder closes it;
- `MAKEFLAGS="-j24 --jobserver-auth=fifo:<path>"` in the environment of every shell and agent that builds;
- no `jobs` in build presets and no `CMAKE_BUILD_PARALLEL_LEVEL`;
- test presets set `"execution": {"jobs": ""}`, because CTest without `-j` runs one test at a time, even under a jobserver (probed).

Current state: there is no pool. `MAKEFLAGS` is unset in agent shells, and no jobserver FIFO or holder process exists on the machine. `/run/user/1000/runrec/heavy.lock` exists; it is unrelated and was left by a process that is no longer running.

Behavior, probed with Ninja 1.13.2 and CTest 4.4.5:

| Situation | Result |
| --- | --- |
| 3 tokens, two Ninja builds at once | At most 5 jobs together. Each client also has one implicit slot, so K clients run up to T + K jobs. All tokens were returned. |
| 3 tokens; `ctest -j`, `ctest -j 8`, or a test preset with `"jobs": ""` | At most 4 tests |
| 3 tokens; `ctest` with no `-j` | 1 test at a time |
| No jobserver; test preset with `"jobs": ""` | 8 of 8 tests at once (processor count) |
| `MAKEFLAGS` names a missing FIFO | Ninja prints "error: Could not initialize jobserver", then runs its default 34 jobs and exits 0. It fails open. |
| FIFO exists, holder process gone | Ninja runs 1 job at a time. It fails closed, but slowly. |

The commands that start the pool are in the final report and in section 9. WU1 starts no build until the pool runs.

### 4.3 CMake project

- Top-level `CMakeLists.txt`:
  - `cmake_minimum_required(VERSION 4.4)` and `project(galoot VERSION 0.1.0 LANGUAGES CXX)`. The version is a placeholder until decision 0017's 1.0.0.
  - `CMAKE_CXX_EXTENSIONS OFF` and `CMAKE_CXX_SCAN_FOR_MODULES OFF`. No unity builds, no PCH.
  - The INTERFACE library `galoot_core`, with alias `galoot::core`, a `HEADERS` file set based at `include` that lists each header, and `cxx_std_23`. Its interface carries no warning or sanitizer flags.
  - When `PROJECT_IS_TOP_LEVEL`: `VERIFY_INTERFACE_HEADER_SETS ON` on `galoot_core`, `enable_testing()`, `add_subdirectory(test)`, and a hard error if `GALOOT_CHECKING` is not defined.
- `include/galoot/core/assert.hpp`: `GALOOT_ASSERT(condition, message)`.
  - In a constant evaluation, a false condition calls the non-`constexpr` `galoot::detail::assertion_failed`, which ends the evaluation without a `throw` (decision 0012, S9). Probed on both compilers: the diagnostic names `assertion_failed` and shows the source line that holds the message.
  - At run time, when `GALOOT_CHECKING` is 1, a false condition writes `galoot: assertion failed: <message> (<file>:<line>)` to stderr and calls `std::abort()`. When it is 0, the macro does nothing at run time.
  - A value other than 0 or 1 stops compilation with `#error`. With no value, the header uses 0 and documents that default at the macro (open question 2).
  - It includes only `<cstdio>` and `<cstdlib>`.
- `include/galoot/core.hpp`: the umbrella header, empty apart from its guard `GALOOT_CORE_HPP` and a comment (open question 3).
- `cmake/galoot_testing.cmake` provides two functions:
  - `galoot_add_test(MODULE <m> COMPONENT <c>)` builds `test/<m>/<c>_test.cpp` into the program `<m>_<c>_test`, linked to the runner. It registers the test `<m>.<c>` with labels `<m>` and `<m>.<c>`, and with `FAIL_REGULAR_EXPRESSION "test cases: +0 \\|"`, so a run of zero test cases fails (S14, note 0001 F28).
  - `galoot_add_compile_fail_test(MODULE <m> COMPONENT <c> SOURCE <file> MESSAGE <literal>)` builds an `EXCLUDE_FROM_ALL` object library. It registers a test that runs `cmake --build <tree> --target <it>` with `PASS_REGULAR_EXPRESSION` set to the escaped literal, labels `<m>` and `<m>.<c>`, and `RESOURCE_LOCK galoot_build_tree`. The lock is there because two Ninja processes in one build tree can corrupt `.ninja_log` and `.ninja_deps` (inferred).
- The doctest runner is the OBJECT library `galoot_doctest_main`, built from `include/ext/doctest/doctest.cpp`:
  - `DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN` is a private definition.
  - `DOCTEST_CONFIG_SUPER_FAST_ASSERTS` is a public definition, so every test gets it.
  - `include/ext` is a `SYSTEM` include.
  - `SKIP_LINTING ON`, because `include/ext` is never linted.
  - It uses the stock doctest `main` (open question 12).
- The `format-check` target runs `clang-format --dry-run -Werror` over `$<TARGET_PROPERTY:galoot_core,HEADER_SET>` and the test sources, with `COMMAND_EXPAND_LISTS` (note 0002 F8).

### 4.4 Presets (`CMakePresets.json`, schema 12)

- Hidden base preset:
  - Ninja generator, `binaryDir` `${sourceDir}/build/${presetName}`;
  - `CMAKE_LINKER_TYPE LLD`, `CMAKE_EXPORT_COMPILE_COMMANDS ON`, `CMAKE_TEST_BUILD_DEPENDS ON`;
  - `CMAKE_CXX_FLAGS` with the S4 flags `-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror`, so that header-check targets get them too (note 0002 F8), and `-ffp-contract=off` (S15, DoD-9; open question 7);
  - `-D_GLIBCXX_ASSERTIONS` in every preset, declared rather than left to the `-O0` default (C8, S5; note 0002 F9);
  - `GALOOT_PRESET=${presetName}` for the launcher.
- Visible configure presets: `gcc-debug`, `gcc-release`, `gcc-asan`, `clang-debug`, `clang-release`, `clang-asan`, and `lint`. Each sets `GALOOT_CHECKING` in its own body, not only through inheritance (DoD-20). The proposed values are ON for debug, asan, and lint, and OFF for release (open question 2).
  - Build types and optimization flags are set in the preset (open question 7), not left to CMake defaults.
  - The asan presets add `-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer` to compile and link flags.
  - The `lint` preset uses Clang with `CMAKE_CXX_CLANG_TIDY` and no compiler launcher.
- Measurement presets (owner-approved, decision 0020): `gcc-measure`, `clang-measure` (`-O2`), `gcc-measure-o3`, and `clang-measure-o3` (`-O3`). They share every other option, omit `_GLIBCXX_ASSERTIONS`, set `GALOOT_CHECKING` OFF, and exist for P4 assembly reads and P6 benchmarks. The `-O3` pair is for comparison only: it is never a correctness or release preset, and the run record names the level. Whether the generated kernels respond to `-O3` is a tracked question, not an assertion.
- Build presets: one per configure preset, with no `jobs`. The `lint` build preset builds `all`, `all_verify_interface_header_sets`, and `format-check`.
- Test presets: `"execution": {"jobs": "", "noTestsAction": "error"}` and `"output": {"outputOnFailure": true}`. With `noTestsAction: error`, a label filter that matches no test exits 8 instead of 0 (probed). The asan test presets set `ASAN_OPTIONS` and `UBSAN_OPTIONS` explicitly (C8). No test timeout is set, because a timeout would be a bound (C14, decision 0013).
- Workflow presets: configure, build, and test for each visible preset.

### 4.5 Smoke test and C13 check

- `test/core/smoke_test.cpp` (S14 layout, open question 4) includes `<galoot/core.hpp>` and `<galoot/core/assert.hpp>`. It has one `TEST_CASE` that checks `GALOOT_ASSERT` with a true condition at run time and in a `static_assert`. It is the present user of the runner, the labels, the launcher, and both headers.
- `core.bare_compile`: a CTest test that runs the configured compiler with only `-std=c++23 -pedantic-errors -I include -fsyntax-only` on `test/core/bare_user.cpp`, which includes both headers. This checks that the core works with no build system (C13, DoD-6).
- `core.assert_checking`: a program built with `GALOOT_CHECKING=1` that fails an assertion. A `cmake -P` script expects "Subprocess aborted" and the literal message on stderr (departure 4).
- `core.assert_unchecked`: the same source built with `GALOOT_CHECKING=0`, which must exit 0.
- `core.assert_consteval`: a compile-fail test, the present user of the helper, matching the literal assertion message. Its twin `core.assert_consteval_wrong_message` expects a different message and carries `WILL_FAIL`. It shows that the helper can fail (V2); `WILL_FAIL` does invert a regex mismatch (probed).
- `core.zero_cases`: runs the smoke program with `--test-case=no-such-case`, with `WILL_FAIL`. It shows that the zero-test-cases rule fires (V2; probed that `WILL_FAIL` inverts a `FAIL_REGULAR_EXPRESSION` failure).

### 4.6 Include scan (C13, S6)

`tools/include_scan.py` uses only the Python standard library. It reads every header under `include/galoot/` that belongs to the core, which today is `core/` and `core.hpp`, and fails with a file:line message when:

- an include names anything other than a C++23 standard header or an existing core header. The standard headers are listed once, with their source, ISO/IEC 14882:2024 [headers] Tables 24 and 25 (S12). Quoted includes are resolved relative to the including file.
- the core's include graph has a cycle;
- a header has no include guard named from its path (`GALOOT_CORE_ASSERT_HPP`), or its name is not `snake_case.hpp`.

It runs as the CTest test `core.include_scan`. The test `tools.include_scan_rejects` runs it on fixtures under `test/tools/include_scan/`: a non-standard include, a two-header cycle, and a wrong guard. It must reject each one, matched by message. Namespaces (S6) are left to review.

### 4.7 Per-TU launcher and run records (P1, P13, P14, decision 0018)

`tools/measure` is one Python program with two entry points, so that both record kinds share one implementation:

- `tools/measure tu <preset> <compiler-id-version> -- <compiler> <args>` is the compiler launcher, set from `CMakeLists.txt` after `project()` when `GALOOT_PRESET` is defined. The `lint` preset sets none.
- `tools/measure run ...` is called by `tools/timed`. Its command line and the first nine `runs.tsv` columns are unchanged.

For each command it:

- reads the RAPL counter and `/proc/stat` before and after;
- runs the command under `perf stat -x,` with user-space events;
- reaps it with `os.wait4` for user time, system time, and `ru_maxrss`, which includes `cc1plus` (note 0002 F7);
- appends one line, under `flock`, to `compile.tsv` or `runs.tsv`;
- passes through the exit status and output unchanged.

If recording fails, it warns on stderr and still returns the compiler's status.

Columns of `compile.tsv`:

```
utc_start revision branch preset compiler source object exit host
mode cpus wall_s user_s sys_s maxrss_kib
inst_p_u inst_e_u cycles_u l1d_loads_u l1d_misses_u l2_refs_u l2_misses_u
llc_refs_u llc_misses_u energy_pkg_j other_cpu_s sibling_busy_s
```

`runs.tsv` keeps its nine columns and then adds the same fields from `mode` through `sibling_busy_s`.

- `mode` is `pinned` when the process's affinity is one performance-core CPU (0 to 15), and `shared` otherwise. It is read from `os.sched_getaffinity`, with no environment variable. `cpus` records the affinity list, which is the processor the run ran on (P14, decision 0018).
- In `pinned` mode, all eight `cpu_core` events are counted: instructions, cycles, L1d loads and misses, `l2_rqsts.references` and `l2_rqsts.miss`, and the architectural LLC references and misses (`cache-references` and `cache-misses`). Probed on CPUs 2 and 4: all eight ran at 100% with no multiplexing, and instruction counts on the two CPUs agreed within 0.03%. With ten events, including `LLC-loads`, perf multiplexed them (38% to 80%), so WU1 uses exactly these eight.
- In `shared` mode, the process can move between core types. Instructions are recorded split by PMU (`inst_p_u`, `inst_e_u`); their sum does not depend on the core type, and the split shows where the process ran. Cycles and cache fields are written `unavailable`. On `cpu_atom`, `L1-dcache-load-misses` is "not supported" (probed), which is one more reason cache fields come only from a performance core.
- `energy_pkg_j` is the package counter's difference, corrected for wraparound. `other_cpu_s` is the machine's non-idle CPU time during the run minus the run's own. `sibling_busy_s` is the busy time of the other thread of the pinned core. Together they let a reader judge whether energy can be attributed and whether a time is comparable (P6, P14, decision 0018). WU1 sets no threshold for "idle" (open question 9).
- A quantity that is not exposed is written as the literal `unavailable`, never as 0 (decision 0018). Examples: perf missing, `perf_event_paranoid` above 2, an unsupported event, or an unreadable RAPL counter.
- Probed per-call overhead, as medians of 15 runs: perf stat around `true`, 6 to 10 ms; Python with these imports, about 11 ms; `git rev-parse` and `git status`, 1 to 2 ms each. WU1 records the measured launcher overhead on a real compile so that readers can subtract it.

`tools/timed` keeps its interface. It delegates measurement to `tools/measure run`, and it refuses with exit 2 and a message when `MAKEFLAGS` names no `fifo:` jobserver or the path is not a FIFO (departure 3).

### 4.8 Lint and format

The `lint` preset runs clang-tidy with the repository's `.clang-tidy` (warnings are errors) over test sources and the header-check TUs. `format-check` covers the headers and tests. `include/ext` is excluded from both. Whether `test/` needs its own `.clang-tidy` (note 0002 open question 4) is decided only when a finding appears in a test (open question 8).

### 4.9 Running

- All builds and tests run through `tools/timed` in the worktree, with the pool running and no `-j`.
- After the first build, `ninja -t deps` and `test_prep/<test>` give per-change selections (P8, P12).
- The pinned measurement for section 6 builds the runner and smoke TUs with `taskset -c 2 tools/timed build <preset> <target> -- ninja -C build/<preset> -j1 <object>`, with CPU 3 idle and the machine quiet (P6, P11, P14).

### 4.10 Decision records written by WU1 (C18)

Numbers are taken at commit time from the next free number; 0018 is the last used at the time of writing.

1. CMake 4.4 as the minimum for Galoot's own build, and presets schema 12 (note 0002 F1, F3).
2. Test structure: one CTest test per program, not one per doctest case; the runner compiled once; `DOCTEST_CONFIG_SUPER_FAST_ASSERTS`, with its savings rechecked on the first real tests at M1 (note 0001 F5, Table 2).
3. doctest 2.5.3 as a test dependency, with the C13 continuity record. It is vendored and frozen. If support ends, Galoot keeps 2.5.3 or replaces it with an owned minimal runner.
4. Tooling dependencies: Python 3 standard library, perf, and taskset, all platform-supplied (C13, decision 0006).
5. The cost-record format of section 4.7, the `pinned` and `shared` modes, and the `unavailable` token (P1, P13, decision 0018).
6. Floating-point options in the presets (S15, DoD-9).
7. `GALOOT_CHECKING`: its values, the header's behavior with no value, and each preset's value (C8, DoD-20).
8. Unity builds and PCH off, modules deferred, TSan not applicable until threads exist (note 0002 F10, F11; S5).

## 5. Acceptance criteria

Each is checked on the head of `feature/build`, unless stated otherwise.

1. `cmake --list-presets=all` lists the seven configure presets of 4.4 and a build, test, and workflow preset for each. `CMakePresets.json` is schema 12; no build preset has `jobs`; every test preset has `"jobs": ""` and `noTestsAction: error`. `CMakeLists.txt` requires CMake 4.4.
2. Every configure preset declares `GALOOT_CHECKING` in its own body. Configuring the source top-level without it fails with a message that names `GALOOT_CHECKING`.
3. The workflow presets `gcc-debug`, `gcc-release`, `gcc-asan`, `clang-debug`, `clang-release`, and `clang-asan` each configure, build, and pass all their tests through `tools/timed`, with the pool running.
4. `ctest --preset gcc-debug -L '^core$'` selects exactly the `core.*` tests. `-L '^core\.smoke$'` selects one. A label that matches nothing exits nonzero. `core.zero_cases` passes, which shows that zero test cases fail.
5. `core.assert_consteval` passes on both compilers. `core.assert_consteval_wrong_message` passes only through `WILL_FAIL`. Every compile-fail test carries `RESOURCE_LOCK galoot_build_tree`.
6. `core.assert_checking` passes, showing an abort and the literal message. `core.assert_unchecked` exits 0.
7. `all_verify_interface_header_sets` builds under `gcc-debug`, `clang-debug`, and `lint`, one TU per header in `galoot_core`.
8. The `lint` build passes: clang-tidy reports no findings, and `format-check` passes. On a misformatted scratch copy in the workbench, `format-check` fails; this is recorded once, to show the check can fail.
9. `core.include_scan` passes, and `tools.include_scan_rejects` passes on all three fixtures. `core.bare_compile` passes on both compilers.
10. After a clean build of `gcc-debug` and `clang-debug`, `compile.tsv` has exactly one new row per compile command. The count is checked against `ninja -t commands`, and every row has all 27 fields. Rows built under the jobserver have `mode` `shared`, `inst_p_u` and `inst_e_u` numeric, and cache fields `unavailable`.
11. One pinned run per compiler of the runner TU has `mode` `pinned`, `cpus` 2, and all eight counters numeric, without multiplexing (perf's running fraction is 100%, kept in the evidence). `energy_pkg_j`, `other_cpu_s`, and `sibling_busy_s` are numeric. A run with `perf` hidden from `PATH` writes `unavailable` for each counter field and still passes the compiler's exit status.
12. `runs.tsv` rows written by the new `tools/timed` have the nine P13 columns unchanged, followed by the decision 0018 fields. `tools/timed` exits 2 with a message when `MAKEFLAGS` is unset or names a missing FIFO.
13. Every file added has a present user, named in section 7 (C12). Nothing under `include/ext` changes.
14. The decision records of 4.10 are committed.
15. The merge into `develop` carries the P9 checks of section 3 and a W5 review record. Its message names the selection, configurations, outcome, and wall time (P13).

## 6. Evidence

- `runs.tsv` rows for every configure, build, and test run, with the decision 0018 fields from the commit that adds them onward.
- `compile.tsv` rows from the acceptance runs. The pinned rows for the doctest runner and the smoke TU on both compilers form the pre-core P1 baseline. Their records state the CPU, the sibling's busy time, and the other load. Notes 0001 and 0002 measured the runner at about 1.0 s and 274 MiB with Clang and 1.4 s and 345 MiB with GCC, once each, so a large difference is reported with its cause.
- A short evidence section appended to this plan when it is closed. It holds the commands of 5.1 to 5.12 with their outcomes and the revision they ran on, the measured launcher overhead, and what was not checked (V10, C20).
- The W5 review record in `docs/agent/records/reviews/`.

## 7. Files

| File | Change | Present user |
| --- | --- | --- |
| `CMakeLists.txt` | Written (empty now) | Every build |
| `CMakePresets.json` | New | Every build |
| `cmake/galoot_testing.cmake` | New | `test/CMakeLists.txt` |
| `test/CMakeLists.txt` | New | The tests below |
| `test/core/smoke_test.cpp`, `test/core/bare_user.cpp` | New | Criteria 3, 4, 9 |
| `test/core/assert_fail.cpp`, `test/core/assert_consteval_fail.cpp` | New | Criteria 5, 6 |
| `test/expect_abort.cmake` | New | `core.assert_checking` |
| `test/tools/include_scan/` fixtures | New | `tools.include_scan_rejects` |
| `include/galoot/core.hpp` | New, empty umbrella | Smoke, bare compile, include scan |
| `include/galoot/core/assert.hpp` | New | Smoke and assert tests; WU2 and WU3 checks |
| `tools/measure` | New | Compiler launcher, `tools/timed` |
| `tools/timed` | Changed | Every run (P13) |
| `tools/include_scan.py` | New | `core.include_scan` |
| `docs/agent/records/performance/compile.tsv` | New, appended | P1, P7 |
| `docs/agent/decisions/00NN-*.md` (eight) | New | C18 |
| This plan | Closed with outcome | W3 |

`.gitattributes` already gives `*.tsv` under `records` union merges. `.gitignore` already ignores `build/` and `CMakeUserPresets.json`. Neither changes.

## 8. Agent cost estimate

- Implementation by build-expert: about 400k input tokens, possibly 50% more or less, over about 80 tool calls in one or two sessions. Output is about 30k tokens.
- Smoke-test review by doctest-expert: about 50k.
- The W5 review by blind-reviewer: about 100k to 150k.

Basis:

- This planning session used about 150k tokens over about 40 tool calls (inferred; not metered).
- Plan 0001's planner used 141k over 14 calls.
- The scratch project of note 0002 had a similar scope: presets, two tests, lint, and install. Its cost was not recorded.
- Configure and build logs for seven presets on two compilers dominate the input. The decision 0018 fields add perf parsing and probes on both PMUs.

Machine time, tracked only: the full clean build is about 14 compiles per preset, with the runner the largest at about 1 to 2 s. That is well under 5 CPU-minutes for all seven presets (inferred from notes 0001 and 0002).

## 9. Prerequisites the owner provides

1. The job-slot pool. Run these commands as written; the heredoc lines must start in column 1, so they are not indented here:

```bash
mkdir -p ~/.local/bin ~/.config/systemd/user
cat > ~/.local/bin/jobserver-hold <<'EOF'
#!/usr/bin/bash
# Holds a GNU Make FIFO jobserver open with N tokens (Galoot PERFORMANCE P11).
set -eu
fifo=$1 tokens=$2
rm -f -- "$fifo"
mkfifo -m 600 -- "$fifo"
exec 3<>"$fifo"
printf '%*s' "$tokens" '' | tr ' ' '+' >&3
exec sleep infinity
EOF
chmod +x ~/.local/bin/jobserver-hold
cat > ~/.config/systemd/user/jobserver.service <<'EOF'
[Unit]
Description=Shared build job slots (GNU Make FIFO jobserver)

[Service]
ExecStart=%h/.local/bin/jobserver-hold %t/jobserver.fifo 24
ExecStopPost=/usr/bin/rm -f %t/jobserver.fifo
Restart=on-failure

[Install]
WantedBy=default.target
EOF
systemctl --user daemon-reload
systemctl --user enable --now jobserver.service
```

The holder script was probed in the workbench, where the free-slot check read 24 tokens. An identical inline holder with 3 tokens served two Ninja builds, which returned every token. The systemd unit itself was not run.

2. `MAKEFLAGS="-j24 --jobserver-auth=fifo:/run/user/1000/jobserver.fifo"` exported for agent sessions, through `env` in `~/.claude/settings.json`, and for the owner's terminals, in `~/.bashrc`.
3. A check, from a fresh agent session, that the sandbox lets an agent open the FIFO for reading and writing (section 10, risk 2).

## 10. Risks

1. Implicit slots. Each Ninja or CTest client runs one job without a token, so 24 tokens allow 24 + K jobs for K concurrent clients (probed). Open question 1.
2. Agent sandbox. Whether an agent's Bash sandbox may open `/run/user/1000/jobserver.fifo` for writing was not checked, because writing outside the workbench was out of bounds for this session. If it may not, agents' Ninja fails open (risk 3) or cannot return tokens.
3. Ninja fails open: with a missing FIFO it runs 34 jobs. The `tools/timed` guard covers runs made through `tools/timed` only.
4. Token loss. A client killed by `SIGKILL` loses its tokens, and the pool shrinks until the service restarts. Restarting while builds run makes a new FIFO, and old clients keep the old one, so for a time more slots exist than intended. The free-slot check in the final report shows the count.
5. Counter limits. More than eight `cpu_core` events multiplex. A kernel or perf update, a `perf_event_paranoid` change, or RAPL becoming root-only (as some kernels do since CVE-2020-8694) turns fields into `unavailable`, which is recorded and not hidden.
6. The `shared` mode's times and energy are not comparable across rows (P14). Only `pinned` rows on a quiet machine are compared (P7).
7. `compile.tsv` grows with every build in every worktree, and union merges keep every line. Open question 10.
8. Compile-fail message matching. A `static_assert` message is printed by both compilers, but the `GALOOT_ASSERT` message reaches the output only through the echoed source line (probed). That echo depends on caret display, which is on by default. GCC also prints typographic quotes, so expected messages must avoid quote characters.
9. LeakSanitizer does not run under ptrace, for example under strace or a debugger. The ASan test environment states its options, so a failure of this kind is visible.
10. A union merge cannot reconcile a changed header line in `runs.tsv`: both versions of the line would survive. Open question 5.
11. Decision numbers may collide with a concurrent unit. WU1 renumbers its records before merging if they do.

## 11. Open questions for the owner

1. The pool: 24 tokens, which allows up to 24 + K jobs, or 23, which matches `make -j24` for a single client? The commands use 24, as P11 says.
2. `GALOOT_CHECKING`:
   - With no value, does `assert.hpp` default to 0? The recommendation is yes, documented at the macro, so that DoD-6's bare `-std=c++23 -I include` user compiles.
   - Is checking ON for debug, asan, and lint, and OFF for release?
3. Umbrella path: `include/galoot/core.hpp` (recommended, `#include <galoot/core.hpp>`), or `include/galoot/core/core.hpp`, which follows S6's "under `include/galoot/<module>/`" literally?
4. Test layout: `test/<module>/<component>_test.cpp` (recommended), or a flat `test/<component>_test.cpp`?
5. The existing rows of `runs.tsv` (untracked in the main checkout, written by the spike lane). Should WU1 extend the header in place once, with old rows read as `unavailable` for the new fields? This needs one coordinated edit while no other unit writes the file.
6. Should `tools/timed` refuse every run when the pool is missing, or only build and CTest steps?
7. Floating-point and optimization options: `-ffp-contract=off` on both compilers, which differ by default; `-O2` for release, the level P4 reads; and `_GLIBCXX_ASSERTIONS` in release presets too, which affects P4 assembly reads and P6 benchmarks unless those get their own preset.
8. Should a `.clang-tidy` for `test/` wait for the first finding in a test (recommended)?
9. What criterion makes a run idle enough to attribute energy? WU1 records `other_cpu_s` and `sibling_busy_s` and sets no threshold.
10. Should every routine `compile.tsv` row be committed, or only rows from gate and measurement runs, with routine rows kept in the build tree?
11. Install and export: deferred until a user, or added in WU1 as build-expert's structure section describes?
12. Should the stock doctest `main` stay until the first randomized test needs a custom one (note 0001, open question 2)?

### Owner answers recorded 2026-10-31 (decision 0020)

- Question 1: the pool has 23 tokens.
- Question 2: `GALOOT_CHECKING` defaults to 0 when no value is given, documented at the macro. Checking is ON for debug, asan, and lint, and OFF for release.
- Question 5: WU1 extends the `runs.tsv` header once, in place. For rows that lack the expanded fields, the cell holds `--` (not blank, not zero, and not `unavailable`); `unavailable` is for a quantity the machine cannot expose in a run that was made with the new recorder.
- Question 10: only rows from gate and measurement runs are committed to `compile.tsv`; routine rows stay in the build tree.
- Question 7: the owner approved the proposed flags on 2026-10-31: the S4 warning flags and `-ffp-contract=off` in every preset, `-O2` for release, `-D_GLIBCXX_ASSERTIONS` in every preset, the sanitizer flags for asan, and a separate measurement preset without `_GLIBCXX_ASSERTIONS` for P4 and P6. `-O2` stays the default and the level is customizable in the build configuration (decision 0020, item 5).
- Questions 3, 4, 6, 8, 9, 11, and 12 keep their recommendations unless the owner objects before WU1 starts.

- Questions 3, 4, 6, 8, 9, 11, and 12 were ruled on 2026-10-31 and recorded in decision 0022: the umbrella is `include/galoot/core.hpp`; tests use `test/<module>/<component>_test.cpp`; `tools/timed` refuses build and CTest steps without the pool; no test `.clang-tidy` yet; no energy threshold; install and export deferred; doctest's built-in `main` through `DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN` only.

## 12. Next step

Superseded by section 13.

## 13. Outcome (2026-10-31, Claude, agent, build-expert)

### Done

- The build of sections 4.3 to 4.8, with the owner's rulings of decisions 0020 and 0022, at `f5f6ddd` on `feature/build`: `CMakeLists.txt`, `CMakePresets.json` (eleven visible configure presets: the seven of 4.4 and the four measurement presets), `cmake/galoot_testing.cmake`, `include/galoot/core.hpp`, `include/galoot/core/assert.hpp`, nine CTest tests, `tools/measure`, the changed `tools/timed`, and `tools/include_scan.py`.
- The decision records of 4.10, numbered 0024 to 0031 because `develop` took 0023 meanwhile; lessons-learned 0001; the snippet `docs/agent/snippets/runs_tsv_merge.py`.
- `develop` (at `aa8051c`) merged into `feature/build`, and `runs.tsv` rebuilt after that merge (lessons-learned 0001, item 4).
- Every preset's workflow, from a clean build tree, on `f5f6ddd` (section 14).

### Departures from this plan, beyond section 1 (C20)

1. Labels. The component label names the header or facility under test, so one label can hold several tests: `core.assert` (four tests), `core.headers` (`core.bare_compile`, `core.include_scan`), `core.runner` (`core.zero_cases`), `core.smoke`, and `tools.include_scan`. Section 4.3 did not say what the component of a non-program test is (decision 0025).
2. Criterion 1 says "the seven configure presets of 4.4". The file has eleven: the seven, plus the four measurement presets that 4.4 also lists. Each has a build, test, and workflow preset.
3. `runs.tsv` has 26 columns, not 27: `wall_s` is not repeated after the nine P13 columns (decision 0028).
4. `test/expect_abort.cmake` became `test/expect_output.cmake`, which checks a result and any number of literals. It has two users: `core.assert_checking` and `tools.include_scan_rejects`. The fixtures are four, not three: a fixture for the snake_case name rule was added so that rule can fail too (V2).
5. A pinned `build` step's run row takes no counters. Two nested perf sessions of eight events multiplexed every counter (lessons-learned 0001, item 1; decision 0028). Section 4.7 assumed both rows could count.
6. The cache variable `GALOOT_COMPILE_RECORD` (not in 4.7) carries decision 0020, item 4: correctness presets write `build/<preset>/compile.tsv`; measurement presets write the committed `docs/agent/records/performance/compile.tsv`. The pinned baseline used separate trees, `build/<preset>-pinned`, configured with the committed path.
7. `GALOOT_CHECKING` reaches the build through `$<BUILD_INTERFACE:...>` on `galoot_core`, not a directory definition, and the two fixed-mode programs do not link `galoot::core` (decision 0030; lessons-learned 0001, item 2).
8. Levels not fixed by the plan, now stated in each preset: asan at `-O1 -g`; debug and lint at `-O0 -g`; release with no `-DNDEBUG` (CMake's default release flags carry it; nothing in Galoot reads it).
9. The `-dirty` mark of a revision ignores changes under `docs/agent/records/performance/`, because `tools/timed` changes `runs.tsv` on every run and so marked every row dirty.
10. The misformatted-copy check of criterion 8 ran in the session scratchpad, not `docs/agent/workbench/`; both are outside what is committed.
11. The doctest-expert review of the smoke test (section 8) was not made.

### In progress

Nothing on this branch. The W5 review by `blind-reviewer` and the merge into `develop` (criterion 15) are arranged by the coordinating session.

### Next step

1. W5 review of `feature/build` against sections 3 and 5 and the departures above.
2. Before merging: merge `develop` into `feature/build` again, rebuild `runs.tsv` with `docs/agent/snippets/runs_tsv_merge.py` if the union merge doubled it, and renumber decisions 0024 to 0031 if `develop` took any of those numbers (risk 11).
3. Merge into `develop` with a message naming the selection (all nine tests), the configurations (the eleven presets), the outcome, and the wall time (P13). After the merge, each active branch that merges `develop` runs the snippet once (its usage names the swapped sides for that direction).

### Open questions

1. A configure-time override of the optimization level (`-DCMAKE_CXX_FLAGS_RELEASE=-O3`) is not visible in `compile.tsv` or `runs.tsv`, which name only the preset. Decision 0020 asks that "the run record names it". Proposal: an `options` column in `compile.tsv` holding the compile command's `-O` level, or the rule that a changed level always gets its own preset.
2. Pinning keeps a run on CPU 2 but does not keep other processes off CPU 2. Only the sibling's busy time is recorded. Should pinned rows also record the pinned CPU's busy time beyond the run's own?
3. Until every branch runs the new `tools/timed`, rows in the old nine-column format keep arriving through merges and need the snippet. Should the old rows instead be read as `--` by convention, with no padding written?
4. Whether `DOCTEST_CONFIG_SUPER_FAST_ASSERTS` keeps its savings on real tests (decision 0025) is checked at M1.

### Agent cost (decision 0019)

Estimate: about 400k input tokens over about 80 tool calls. Actual: about 100 tool calls in one session; the harness did not report tokens to this agent, so tokens are unavailable here.

## 14. Evidence

All on `feature/build` at `f5f6ddd`, unless a row says otherwise. Every command ran from the worktree root with `eval "$(tools/jobpool env)"` (23 slots), through `tools/timed`, whose rows are in `runs.tsv`.

| Criterion | Command | Outcome |
| --- | --- | --- |
| 1 | `cmake --list-presets=all`; a JSON read of `CMakePresets.json` | 11 configure, build, test, and workflow presets; version 12; no build preset has `jobs`; every test preset inherits `"jobs": ""` and `noTestsAction: error`; `cmake_minimum_required(VERSION 4.4)` |
| 2 | `cmake -S . -B <scratch> -G Ninja -DCMAKE_CXX_COMPILER=g++` (at `c171240`) | Fails: "GALOOT_CHECKING is not set." With `-DGALOOT_CHECKING=yes`: "GALOOT_CHECKING must be ON or OFF". Every visible preset sets it in its own body |
| 3 | `ninja -C build/<p> -t clean; tools/timed workflow <p> all-clean -- cmake --workflow --preset <p>`, for all eleven presets | Each: 9 of 9 tests passed. Wall times (s): gcc-debug 2.23, gcc-release 3.41, gcc-asan 5.13, clang-debug 1.92, clang-release 2.33, clang-asan 3.46, lint 1.59, gcc-measure 3.22, gcc-measure-o3 3.68, clang-measure 2.09, clang-measure-o3 2.27 |
| 4 | `ctest --preset gcc-debug -N -L '^core$'`; `-N -L '^core\.smoke$'`; `-L '^nosuch$'` (at `5134bb5`) | 8 tests, exactly the `core.*` ones; 1 test; "No tests were found", exit 8. `core.zero_cases` passes in every preset |
| 5 | `ctest -V -R '^core\.assert_(consteval|checking)$'`; Clang's `LastTest.log` | Both compilers echo the line holding the literal message; `core.assert_consteval_wrong_message` passes through `WILL_FAIL`; both compile-fail tests carry `RESOURCE_LOCK galoot_build_tree`. The escaping was checked separately with `cmake -P` on a string holding every regex metacharacter |
| 6 | as for 5 | `core.assert_checking`: "Subprocess aborted" and `galoot: assertion failed: assert_fail: argc is never zero (...assert_fail.cpp:9)`. `core.assert_unchecked` exits 0 |
| 7 | `cmake --build --preset <p> --target all_verify_interface_header_sets` for gcc-debug and clang-debug; the lint build preset | Two TUs, one per header of `galoot_core`, on each |
| 8 | `cmake --build --preset lint` (in the lint workflow); a scratch copy with `std::abort( ) ;` in `assert.hpp` (at `c171240`) | Lint: no clang-tidy finding, `format-check` passes. Scratch copy: `format-check` exits 1 with three `-Wclang-format-violations`. clang-tidy failed the first lint build on two real findings, since fixed, which shows it can fail |
| 9 | the workflows of 3 | `core.include_scan`, `tools.include_scan_rejects` (all four fixture messages), and `core.bare_compile` pass on both compilers |
| 10 | clean build of gcc-debug and clang-debug, then a count against `ninja -t commands all` (at `5134bb5`) | 4 new rows for 4 compile commands on each; every row has 27 fields, `mode` `shared`, `inst_p_u` and `inst_e_u` numeric, and cycle and cache fields `unavailable` |
| 11 | `taskset -c 2 tools/timed build <p> pinned:<object> -- ninja -C build/<p>-pinned -j1 <object>` (at `5134bb5-dirty`: the uncommitted change was the pinned-build rule, committed unchanged as `c171240`); the launcher with `perf` hidden from `PATH` | Pinned rows: `mode` `pinned`, `cpus` 2, all eight counters numeric, which the recorder writes only when perf reports 100.00% running; no multiplex warning. `energy_pkg_j`, `other_cpu_s`, `sibling_busy_s` numeric. Without perf: all nine counter fields `unavailable`, exit statuses 0 and 1 passed through |
| 12 | `env -u MAKEFLAGS tools/timed build ...`; `MAKEFLAGS=...fifo:/run/user/1000/no-such.fifo tools/timed test ... -- ctest ...`; `tools/timed check - guard-probe -- true` (at `5134bb5`) | Exit 2 with a message for a build step, a missing FIFO, and `ninja` under step `test`; the other command runs and is recorded. New rows have the nine P13 columns first |
| 13 | `git diff --stat develop -- include/ext` | Empty. Files and their users: section 7, plus `test/expect_output.cmake` (two tests), the fourth fixture, the snippet (merges across the header extension), and lessons-learned 0001 |
| 14 | `ls docs/agent/decisions/` | 0024 to 0031 committed |
| 15 | | Not met yet: the merge into `develop` |

Pre-core P1 baseline, pinned to CPU 2, sibling CPU 3 busy 0.00 s, other agents' work on the machine (other CPU time 0.30 s to 0.36 s during the runner compiles), debug presets (`compile.tsv`, rows of 2026-10-31T21:4x):

| TU | Compiler | Wall s | Peak MiB | Instructions | Cycles |
| --- | --- | --- | --- | --- | --- |
| doctest runner | GCC 16.2.1 | 1.227 | 347 | 11.12 G | 6.38 G |
| doctest runner | Clang 23.1.4 | 0.873 | 281 | 7.48 G | 4.48 G |
| smoke test | GCC 16.2.1 | 0.057 | 44 | 0.239 G | 0.115 G |
| smoke test | Clang 23.1.4 | 0.061 | 101 | 0.162 G | 0.106 G |

Notes 0001 and 0002 had the runner at 9.7 G instructions (GCC) and 6.9 G (Clang) at `-O0`, and at about 1.4 s and 345 MiB (GCC) and 1.0 s and 274 MiB (Clang). The instruction counts here are 15% and 8% higher. The inferred cause, not checked: this build adds `-g`, `-D_GLIBCXX_ASSERTIONS`, `-ffp-contract=off`, and `DOCTEST_CONFIG_SUPER_FAST_ASSERTS` to the S4 flags that note 0001's Table 1 commands used. The four rows just before these in `compile.tsv` are the multiplexed attempt.

Launcher overhead (at `5134bb5-dirty`, as for criterion 11), from the median difference of 15 pinned compiles of the smoke TU with and without `tools/measure tu` (GCC, `-O0`): 25.6 ms and 26.0 ms in two runs, and 61.3 ms in a third run that had a wide spread (77 ms to 100 ms) and was not traced. Contention on CPU 2 is the inferred cause; the next run measured CPU 2 as free. The launcher around `true` alone takes 23.9 ms, of which Python startup with its imports is about 15 ms.

Not checked: the CTest-side handling of an escaped pattern that contains regex metacharacters (only `cmake -P` was checked); the `ninja -t deps` and `test_prep/<test>` selections of section 4.9; token return when a client is killed; ASan under `ptrace`; a pool restart while builds run.

## 15. W5 fixes (2026-10-31, Claude, agent, build-expert, Opus 5.5)

The review of head `a00259c` returned "merge after fixes". Each fix below was shown to fail before it where practical, by a mutation or by the test written first, and to pass after. N5 to N16 are carried in plan 0003 and were not taken on here.

| Finding | Fix | Before | After |
| --- | --- | --- | --- |
| B1, `compile.tsv` header race | `append_row` tests `os.fstat(...).st_size == 0` after `flock` (`9385e56`). New test `tools.measure_header`: holds the lock on an empty record while four launchers queue on it (counted in `/proc/locks`), then counts header lines (`5087d4a`) | 4 headers from 4 launchers, exit 1 | 1 header, 4 rows. A fresh clang-debug tree: 1 header, 6 rows for 6 compile commands (criterion 10 on a clean build) |
| B2, detail namespace | `galoot::core::detail` in `assert.hpp`; `GALOOT_ASSERT` expands to it (`39f98e9`). A smoke test case calls `galoot::core::detail::checking_now()` (`5087d4a`) | The smoke test does not compile: "'galoot::core' has not been declared" | Compiles and passes. Decision 0030 never named the namespace, so it needed no change; section 4.3's `galoot::detail` is superseded |
| N1, compile-fail for the wrong reason | The helper runs the build through `test/expect_output.cmake` and requires a failed build, the literal MESSAGE, and each REQUIRE literal; the assertion tests require `assertion_failed` (`8100c8c`). The regex escaping is gone | With `x % 2 == no_such_name` on the message line, `core.assert_consteval` passed on GCC and Clang | The same mutation fails on both: "output lacks 'assertion_failed'". Restored source passes |
| N2, zero-cases rule not checked | Configure-time check that `core.smoke`'s `FAIL_REGULAR_EXPRESSION` equals `GALOOT_ZERO_CASES_REGEX` (`9ed0f7c`) | With the rule dropped from `galoot_add_test`: configures, and `core.smoke` and `core.zero_cases` pass | Configure fails: "core.smoke's FAIL_REGULAR_EXPRESSION is 'NOTFOUND'" |
| N3, `;` in MESSAGE | FATAL_ERROR (`8100c8c`) | `MESSAGE "a;b"` configured, as `PASS_REGULAR_EXPRESSION "a;b"` (two alternatives) | Configure fails: "MESSAGE may not contain ';'" |
| N4, guard bypass | Step `test` needs the pool, whatever the command (`fca52fa`) | `env -u MAKEFLAGS tools/timed test ... -- taskset -c 2 ctest ...` ran, exit 0 | Exit 2 with a message; `nice ctest` also refused; other steps run |
| B3, merge and P9 | `develop` (`63e2014`) merged with `--no-commit`; it brought only records, decisions, and plans, and left `runs.tsv` with one header | | P9 below, on the merged tree |

P9 on the merged tree (revision recorded as `0863bad-dirty`, the uncommitted merge), each from a clean build tree through `tools/timed workflow <p> all-clean -- cmake --workflow --preset <p>`: gcc-debug, gcc-release, gcc-asan, clang-debug, clang-release, clang-asan, and lint (clang-tidy, both header checks, `format-check`) each passed 10 of 10 tests. Wall times (s): 2.38, 3.34, 5.49, 1.89, 2.47, 3.44, 1.66. `all_verify_interface_header_sets` built under gcc-debug and clang-debug. Total, with the two header-check builds: 9 runs, 21.08 s, no failure. The measurement presets were not rerun; they passed at `f5f6ddd`, and P9 does not gate on them.

Departures in this pass (C20):

1. Commit `5087d4a` holds the B1 and B2 tests before their fixes, as W9 asks, so at that commit the build fails and `tools.measure_header` fails. This is against W2's "building at every recorded step".
2. One `ctest -V` run, made only to read the N1 mutation's output, was not run through `tools/timed`.
3. `develop`'s rows in `runs.tsv` are not padded, following decision 0032, item 3; the rows padded earlier on this branch stay as they are.
4. N4 narrows decision 0022, item 3 ("other commands unchanged"): a step named `test` that runs no CTest, such as a Python reference script, is now refused without the pool.
5. A pinned or wrapped CTest run under a step name other than `build` or `test` still passes the guard.

After this section was first written, `develop` moved to `b250d55` (spikes C, D, and F: research notes 0010 to 0012 and records only), and it was merged into `feature/build` as `e8ff61f`. The union merge doubled `runs.tsv` (two headers; the base's 669 earlier rows twice), because this branch had padded those rows. The file was rebuilt before the merge commit as this branch's version followed by `develop`'s 2125 appended rows, unpadded (decision 0032, item 3), and the merge commit holds one header. No file that a build or test reads differs from `9bed7f5`, so the P9 run above stands for `e8ff61f` (P10). The merge of `feature/build` into `develop` meets the same doubling if `develop` appends rows again, and needs the same rebuild: the branch's version, then `develop`'s rows appended since the merge base.
