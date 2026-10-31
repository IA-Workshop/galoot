# 0003 Recorder follow-ups (WU1b)

- Status: closed 2026-10-31 (outcome below); awaiting the W5 review and the merge into `develop`
- Branch: `feature/recorder` (worktree `.claude/worktrees/recorder`)
- Goal: carry out items 1 to 3 of decision 0032 in the recorder.
- Definition of done: (1) every run and compile row records the effective compiler options in an `options` column; (2) pinned rows record other-process busy time on the pinned CPU; (3) readers treat nine-cell rows as `--` for the missing cells, with a test; (4) the header extension is done once, in place, with `runs_tsv_merge.py` updated to match; (5) the smoke path and the existing recorder checks still pass on both compilers.
- Acceptance: the same recorder checks as plan 0002 criteria 10 to 12, plus one row each for an overridden `-O` level and a pinned run showing the new fields.
- Evidence: the recorder test output and one row of each kind, with the revision and command.
- Agent cost: tracked for information only (decision 0019); record in `docs/agent/records/agent-cost.tsv`.
- Open questions: none.

## Additional scope from the WU1 review (non-blocking findings N5 to N16)

Findings N5, N6, N7, N8, N9, N10, N12, N13, N14, N15, and N16 of `docs/agent/records/reviews/2026-10-31-wu1-build-skeleton.md` are carried here, except that N9 and N10 are decision-record edits and N12 is a re-record of the P1 baseline on a clean revision. N11 is a process note for the next review request. The blocking findings B1 to B3 and the cheap N1 to N4 are fixed on `feature/build` before it merges.

Also in scope: fix `docs/agent/snippets/runs_tsv_merge.py` so it does not pad old rows (decision 0032 item 3) and runs correctly under `git merge --no-commit`; add checks for a pool restart during builds and token loss when a build is killed; and a doctest-expert review of the smoke test.

## Outcome (2026-10-31, Claude, agent, build-expert, Opus 5.5)

All on `feature/recorder`, from `develop` at `f62ec2f` (decision 0034 and the tightened STYLE S11 were fast-forwarded in before any change). Head at closing: `2d58abb`, then this record.

### Departures and ambiguities (C20)

1. Test and fix commits. `518823d` holds the recorder change and its new test together; the test was first run against the previous recorder and failed seven checks (below). For the include scan the fix (`a94e127`) precedes its separate test commit (`d76371d`), which meets both W2 (every commit builds and passes) and W9 (a test change judging the author's change is committed separately). `1082f97` is a test-only change to the smoke test.
2. N9 and N10 are recorded as new decisions 0041 and 0040, with one status-line pointer added to 0029 and 0027, instead of edits to those records' bodies: W6 says a recorded decision is not rewritten, while this plan called them decision-record edits.
3. Decision 0042, item 5, records perf's `<not counted>` with zero running time as `0` in shared rows. The reading that this is a measured zero, not an unavailable figure under decision 0018, is inferred from perf's semantics and checked on both core types, not proven. The owner may reverse it.
4. The doctest-expert review was made by a general-purpose agent briefed to follow `.claude/agents/doctest-expert.md`, because the `doctest-expert` agent type was not offered to this session.
5. Owner style rulings arrived mid-unit. Everything written after them, and the files this unit rewrote (`tools/measure`, `tools/include_scan.py`, `tools/jobpool`'s header, `assert.hpp`, `smoke_test.cpp`, both snippets), cite no records or provisions. Lines this unit did not touch in `CMakeLists.txt`, `test/CMakeLists.txt`, `cmake/galoot_testing.cmake`, and `tools/timed` keep their old citations for the cleanup unit (decision 0034).
6. Not run through `tools/timed`: the scratch scripts for the snippet merges and the private-pool `tools/jobpool` checks (they run Ninja on sleep-only builds against a private FIFO), the `perf stat` probes, and direct `tools/measure tu`/`read` calls. Their commands and outputs are below.
7. The pinned baseline ran while other agents were working (other CPU time 1.65 s to 2.40 s per runner compile); the pinned CPU and its sibling were free (below).
8. After this merges, a worktree whose build tree has a `compile.tsv` with the old 27-column header gets "not recorded" warnings until that file is removed; the committed records already have the new header.

### Definition of done

| Item | Evidence |
| --- | --- |
| (1) `options` in every run and compile row | Decision 0042, items 1 to 3. The four pinned rows and the override row below; every run row since `518823d` |
| (2) pinned rows record other processes on the pinned CPU | `pinned_other_s`, decision 0042 item 4; pinned rows below: 0.00, 0.03, 0.00, 0.01 s |
| (3) nine-cell rows read as `--`, with a test | `tools/measure read`; `tools.measure_record` case `reader`; the whole committed `runs.tsv` (2125 nine-cell, 804 26-cell, and, at closing, 73 28-cell rows) reads without error, the 2929 older rows showing `--` in `options` |
| (4) header extended once, in place; snippet matches | `518823d` changes only line 1 of each committed record; `b28cc34` rewrites `runs_tsv_merge.py` to copy rows unpadded |
| (5) smoke path and recorder checks pass on both compilers | Full suite below; criterion 10 of plan 0002 on clean gcc-debug and clang-debug trees: 6 rows for 6 compile commands (4 in `all` plus the 2 compile-fail objects the tests build), every row 29 fields; criterion 11: pinned rows with all eight counters numeric and no multiplex warning; criterion 12: run rows keep the nine P13 columns first |

Acceptance rows (`docs/agent/records/performance/compile.tsv`, revision `6d64975a3b7d`, clean):

| TU | Compiler | Mode | Wall s | Peak KiB | Instructions | `sibling_busy_s` | `pinned_other_s` | `options` (abridged) |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| doctest runner | GCC 16.2.1 | pinned, CPU 2 | 1.220 | 355224 | 11.122 G | 0.00 | 0.00 | `... -O0 -g -std=c++23` |
| smoke test | GCC 16.2.1 | pinned, CPU 2 | 0.034 | 45372 | 0.272 G | 0.00 | 0.03 | `-DGALOOT_CHECKING=1 ... -O0 -g` |
| doctest runner | Clang 23.1.4 | pinned, CPU 2 | 0.893 | 287568 | 7.472 G | 0.00 | 0.00 | `... -O0 -g -std=c++23` |
| smoke test | Clang 23.1.4 | pinned, CPU 2 | 0.032 | 103248 | 0.177 G | 0.00 | 0.01 | `-DGALOOT_CHECKING=1 ... -O0 -g` |
| smoke test, `-O` override | GCC 16.2.1 | shared | | | 0.392 G | unavailable | unavailable | `... -D_GLIBCXX_ASSERTIONS -O2 -g -std=c++23` |

Commands: `cmake --preset <c>-debug -B build/<c>-debug-pinned -DGALOOT_COMPILE_RECORD=$PWD/docs/agent/records/performance/compile.tsv`, then `taskset -c 2 tools/timed build <c>-debug pinned:<object> -- ninja -C build/<c>-debug-pinned -j1 <object>`; for the override, `cmake --preset gcc-debug -B build/gcc-debug-o2 -DCMAKE_CXX_FLAGS_DEBUG="-O2 -g" -DGALOOT_COMPILE_RECORD=...` and `ninja -C build/gcc-debug-o2 test/CMakeFiles/core_smoke_test.dir/core/smoke_test.cpp.o`. The override's configure and build run rows also name `-O2 -g`. Against the old baseline (`5134bb5-dirty`), the runner's instructions are 11.122 G for GCC in both and 7.472 G against 7.483 G for Clang (−0.14%); the smoke TU is not comparable, since the smoke test changed in `1082f97`.

### Review findings carried here

| Finding | Commit | Before | After |
| --- | --- | --- | --- |
| N5, re-run when perf named no event | `518823d` | stand-in perf runs the command and reports a renamed event: the command ran twice (`ran\nran\n`) | runs once; counters `unavailable` with a warning. The re-run without perf now happens only when perf wrote no counter line and was not killed by a signal (perf 7.3.1: a bad event exits 129 with no output; a missing workload exits 1 with no counter line) |
| N6, index lock | `518823d` | `git status` | `git --no-optional-locks status` |
| N7, undeclared cache entries | `2da0dc8` | `-DGALOOT_PRESET=x` alone configures, cache shows `UNINITIALIZED`, and every compile fails | configure fails naming `GALOOT_COMPILE_RECORD`; the reverse also fails; entries are `BOOL`, `STRING`, `FILEPATH` with help text |
| N8, LDFLAGS and glob | `2da0dc8` | `LDFLAGS=-Wl,--no-such-linker-flag cmake --preset gcc-debug` fails at the compiler check; a misformatted test source added after configure passes `format-check` (exit 0) | configure succeeds, `CMAKE_EXE_LINKER_FLAGS` empty; `format-check` re-globs and fails (exit 1) |
| N9, N10 | `977f50d` | | decisions 0041 and 0040 |
| N11 | | | test list below |
| N12, baseline on a clean revision | `f5df65f` | `5134bb5-dirty` | `6d64975a3b7d` |
| N13, include scan | `a94e127`, `d76371d` | the old scan, given the three new fixtures, reports none of them, and `tools.include_scan_rejects` with the new literals fails ("output lacks" each) | reported, and the test passes |
| N14, snippet | `b28cc34` | scratch merge 1 (padded side against appending side): appended rows padded; merge 2 (record added on both sides of a base without it): second header written as a data row, which `tools/measure read` rejects | merge 1: appended rows unpadded; merge 2: one header; both read cleanly. Runs during `git merge --no-commit` against `MERGE_HEAD` |
| N15, `assert.hpp` citations | `1e73271` | cites plan 0002, decisions 0012 and 0020, C8, C9, S9 | reasons stated in place; comments only |
| N16, `<not counted>`; unstartable command | `518823d` | `inst_e_u` `unavailable` for a command that never ran on an efficiency core; no row for a command that cannot start | `0` (decision 0042 item 5); a row with exit 127 and `unavailable` costs |

Pool checks (`docs/agent/snippets/jobserver_check.py`, private FIFO of 3 slots, Ninja 1.13.2, at `b28cc34`): SIGKILL to Ninja holding every slot returned 0 of 3; SIGINT to its process group and SIGTERM to Ninja returned 3 of 3; a pool replaced under a running build let 8 jobs run at once where one pool allows 5 for two clients; a `MAKEFLAGS` FIFO that does not exist made Ninja print "Could not initialize jobserver" and run 34 jobs on 32 CPUs, exit 0. Ninja held the FIFO open for its whole build. `tools/jobpool` (`6d64975`) now names the clients holding the FIFO, refuses `stop` while any does, and reports lost slots when none does. On a private unit (only the unit name and FIFO path changed), the old script stopped the pool under a one-job build and, after a SIGKILL, refused to stop ("3 of 3 slots are in use"); the new one refused under the build ("clients: <pid> (ninja)") and after the kill reported "3 slots are lost" and stopped. A live `tools/jobpool status` also listed another agent's `ctest` as a client, so CTest holds the FIFO too. The shared pool was never stopped or restarted. `tools/timed` already refuses to start when the FIFO is missing; a pool stopped during a CTest run whose compile-fail tests start Ninja would still meet the 34-job fallback (not changed).

Smoke-test review (doctest-expert role, read-only): four non-blocking findings. Its N2, the mode check compared `checking_now()` with the macro it returns, is fixed in `1082f97`: with `galoot_core`'s mode definition removed, a gcc-debug `core.smoke` passed before and fails after ("false == true"). Its N1 (citations in the WU1 test files) goes to the decision 0034 cleanup unit. Its N3 (move the mode case to `assert_test.cpp` once that file exists) and N4 (`core.zero_cases` also passes when the program exits nonzero for another reason; proposed: `PASS_REGULAR_EXPRESSION` instead of `WILL_FAIL`) are open, below.

### Tests and expected values (N11)

New or changed, each with its expected value and where that value comes from:

- `tools.measure_record` (new, `test/tools/measure_record_test.py`), from the recorder's stated behavior: a command perf ran is run once (counter file `ran\n`), exit 0, `inst_p_u` `unavailable`; a command that cannot start exits 127 with one row, `exit` `127`, `wall_s` `unavailable`; compile options `-O2 -g -Iinclude` for `/bin/true -O2 -g -I<top>/include -MD -MT x.o -MF x.o.d -o x.o -c <top>/src/x.cpp`, with source `src/x.cpp`, object `x.o`, `pinned_other_s` `unavailable`; per core type `inst_p_u` `1000`, `inst_e_u` `0` for `<not counted>` at zero running time, `unavailable` for `<not supported>`, mode `shared`; run options `-Wall -Werror -O1 -g` from a cache via `-C <tree>` and via `build/<preset>`, `-` with no tree, `unavailable` with no cache; `read old.tsv c a` prints `c\ta\n--\t1\n3\t1\n` with exit 0, and exits 1 on an over-long row, a repeated header, or an unknown column.
- `tools.include_scan_rejects` (three literals added): `galoot/core/include_next.hpp:4: #include_next depends on the order of the user's include paths`; `galoot/core/computed.hpp:5: computed #include GALOOT_CORE_COMPUTED_TARGET cannot be checked`; `galoot/core/not_hpp.h: not a .hpp header`. Line numbers from the fixtures.
- `core.smoke` (changed): `checking_now() == (GALOOT_EXPECTED_CHECKING == 1)`, the expected mode taken from the CMake variable `GALOOT_CHECKING` directly. Test names lost their citations.

### Verification (P9, merge into `develop`)

Full suite, 11 tests (`core.smoke`, `core.zero_cases`, `core.assert_checking`, `core.assert_unchecked`, `core.assert_consteval`, `core.assert_consteval_wrong_message`, `core.bare_compile`, `core.include_scan`, `tools.include_scan_rejects`, `tools.measure_header`, `tools.measure_record`), in gcc-debug, gcc-release, gcc-asan, clang-debug, clang-release, clang-asan, and lint (clang-tidy, both header checks, `format-check`): 11 of 11 each.
- From clean build trees at `f5df65f`, `tools/timed workflow <p> all-clean -- cmake --workflow --preset <p>`: 2.56, 3.86, 6.63, 3.31, 3.53, 4.64, 3.13 s (27.64 s).
- Incrementally at `2d58abb` (the launcher and test changed by one line each): 2.06, 1.82, 1.41, 0.95, 0.66, 1.58, 1.68 s (10.16 s). `all_verify_interface_header_sets` built on gcc-debug and clang-debug at `f5df65f`.
- The four measurement presets were configured only, into scratch trees (exit 0, `CMAKE_EXE_LINKER_FLAGS` empty), because building them appends to the committed `compile.tsv`; P9 does not gate on them.
- Python: `ruff check --select E,F,W,B,BLE,PLC0415 --line-length 80` on the touched scripts passes (ruff is not part of the project's checks).

Not checked: the recorder on a machine without RAPL or with `perf_event_paranoid` above 2 (code read only); a real multiplexed pinned run; `pinned_other_s` under real contention on CPU 2 (it was free); TSan (no threaded test).

### Open

1. Smoke-test review N4: `core.zero_cases` passes on any nonzero exit. Small; for the next unit that touches `test/CMakeLists.txt`.
2. Smoke-test review N3: move the checking-mode case to `test/core/assert_test.cpp` when that file exists.
3. Ninja falls back to its own parallelism when the FIFO is missing; a CTest run outliving a stopped pool can oversubscribe through its compile-fail tests. A guard would sit in `galoot_add_compile_fail_test` or `test/expect_output.cmake`.
4. Whether decision 0042 item 5 stands (owner).

### Agent cost (decision 0019)

Estimate: none was written in this plan. Actual: the implementing agent's tokens are not reported to it (unavailable); about 110 tool calls over about 40 minutes by the clock. The read-only smoke-test reviewer: 80839 tokens, 17 tool calls, 150 s, as the harness reported. Both rows are in `agent-cost.tsv`.

<details><summary>Scratch scripts behind the snippet and jobpool evidence</summary>

The merge check created two throwaway repositories with the union driver: case 1, base `utc_start a b` with two rows; branch `padded` rewrote the rows with two `--` cells, extended the header by `c d`, and appended a row; `develop` appended two rows; `git merge --no-commit develop` from `padded` doubled the file (2 headers, 9 lines); the snippet was then run as its usage states. Case 2, a base without the file and each side adding it with one row. The jobpool check used copies of the old and new `tools/jobpool` with `unit=galoot-jobserver-check` and `fifo=/run/user/1000/galoot-jobserver-check.fifo` (a FIFO under the session's `/tmp` was not visible to the user service), `MAKEFLAGS="-j4 --jobserver-auth=fifo:<that fifo>"`, `start 3`, a one-job build (`sleep 3`) with `status` and `stop` during it, then an eight-job build (`sleep 2` each) killed with SIGKILL after 0.7 s, `status` and `stop` after its jobs ended.

</details>
