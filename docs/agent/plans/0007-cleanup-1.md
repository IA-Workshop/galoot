# 0007 Cleanup 1: citations, gate hardening, test gaps, snippets review

- Status: closed (outcome below), awaiting the W5 review and the merge; branch `feature/cleanup-1`, worktree `.claude/worktrees/cleanup-1`; decision numbers 0080 to 0089 (0080 to 0082 used).
- Goal: bring the files merged so far into line with the style ruling and the review findings that were deferred, so the next units start from a clean base, and close M0.
- Scope:
  1. Remove every citation of a governance provision or of agent work from human-facing files: `include/`, `test/` (sources, CMake, test names, comments, messages), `cmake/`, `CMakeLists.txt`, `CMakePresets.json`, `tools/` (comments and docstrings), and generated files. State the reason in place. The deliberate pointer text in the rejecting fixtures under `test/tools/test_references/` stays and is excluded from the new check.
  2. `CMakePresets.json` names a path under `docs/agent` for the measurement presets' committed compile record. Propose and record a design that keeps agent paths out of build files (recommended: measurement presets write to the build tree, and a documented gate step copies the gate and measurement rows into the committed record), and implement it.
  3. A mechanical check, run as a CTest over the human-facing files named above, that flags citations: decision, plan, research note, review, lesson, spike, workbench, and agent-name references, provision tokens with a document name, and the name of the conventions specification. It has a rejection test on fixtures that must exit 1 and an accepting case that exits 0, and it excludes the fixtures that hold deliberate text.
  4. Trace gate hardening (review findings of the M0 delta): fail on a convention item that has no table row (D2); flag a table line with an extra `|`; evaluate Implemented per row and not per item; scan helper headers under `test/` for uses of golden values (D6); close the remaining pointer forms (D4); clear the lint nit (D7).
  5. Recorder test gaps: a stand-in perf that records its `-e` list so the test checks `task-clock` is requested (R-N1); read the program once in the build-tree rule (R-N2); git failing, as opposed to missing (R-N3); a single blank line as a header (R-N5); the `cases` dict and docstring nits (R-N6).
  6. Metric nits: a FORBID twin for `conformal<SIZE_MAX - 1>()` (R1), document that `;` splits FORBID literals or reject it (R3), reflow two comments (R5).
  7. The snippets coalescing review for M0 and M1: survey the workbench, `docs/agent/records/spikes/`, and the unit scratch files; write `docs/agent/snippets/reviews/M0-M1.md` (reusable, duplicate, merge, delete, promote to `tools/` or `test/`); apply only moves inside the snippets directory.
- Definition of done: every item above done with evidence, or reported as not done with the reason; the full suite passes on all seven presets; the new citation check passes on the tree; the trace gate passes and fails on its new rejection fixtures.
- Acceptance criteria: (1) a grep of the human-facing files for the citation patterns returns nothing outside the excluded fixtures; (2) each new check has a rejection test that fails when the check is disabled (mutation shown); (3) no expected value of an existing test changes without a recorded reason; (4) commits keep W2 where a split is possible without a failing commit; (5) plan closed with its outcome and a verification and validation line.
- Evidence: test output per preset, mutation runs, and the before and after greps.
- Agent cost: tracked for information only (decision 0019).
- Review: W5 blind review before the merge.
- Open questions: item 2 is a design choice the unit proposes; it is recorded as a decision and the owner may reverse it.

## Outcome (2026-10-31, Claude, agent, Opus 5.5)

Done on `feature/cleanup-1`, 13 commits from `eb208e5` to `2e8567d`, then this record. Every item is done; the M0 gate itself is not closed by this unit (departure 1).

### Departures and ambiguities (C20)

1. The goal says "close M0". Read as: deliver the M0 items this plan lists, including the snippets review a gate needs. Closing a milestone is the owner's acceptance (W11) after an auditor run and a gate report, which this unit does not write.
2. The citation check also scans `.clang-format` and `.clang-tidy`, which the plan's list does not name: they are the style configuration the lint preset reads, and both cited STYLE.md. `.gitignore` and `.gitattributes` are neither cleaned nor scanned: they must name `docs/agent` paths to work, and `.gitignore` still says "PERFORMANCE.md P7" and "WORKFLOW.md W6" in two comments. `docs/galoot-design-study.md` and the empty `README.md` are outside the plan's list and were not checked.
3. The marker and trace rejection tests now read their expected findings from `*.expected` files beside the fixtures, so `test/CMakeLists.txt` holds no citation text (decision 0081). The literals are unchanged; the trace accepting test's "items with no test: 3.1" became "3.1, 4.1" because the fixture specification gained item 4.1 (decision 0082). No other expected value of an existing test changed.
4. `citations_bad.expected` was written from the command's output on the fixtures, after each of its 36 lines was checked by hand against the fixture line it names (one intended citation per line, two on the JSON line). The pattern-removal mutations show each pattern is needed for it.
5. Item 2 removes the configure-time check that refused `GALOOT_COMPILE_RECORD` without `GALOOT_PRESET` (plan 0003, N7): the record now always has a value (decision 0080).
6. Item 7 applies nothing: no move inside the snippets directory is called for. Deletions and index edits are recommendations.
7. Pre-existing ruff findings were left: B905 five times in `clifford_reference.py` and E741 once in `conventions_check.py` (ruff `--select B,E,F,W`). Ruff is not one of the project's checks.
8. Commits keep W2 and W9 where a split was possible: code first with existing tests passing (`b327565`, `e10ca2b`, `92af9f7`), then its tests (`5f4eb42`, `df64149`, `c156d15`), each shown failing against the code before. The citation cleanup (`9c08e21`) changes no test logic. The FORBID twin (`fb07f85`) passes on the header as it stands; its mutation is shown below.
9. Agent cost: the harness reported no token count to this agent. About 120 tool calls; session wall time about 25 minutes (20:39 to 21:05 local).

### Evidence

| Item | Before | After | Commands |
| --- | --- | --- | --- |
| 1, 3: citations | `citations` over the scanned paths, run on the tree of `e10ca2b` before its last pattern edit: 106 findings in 16 files: `test/CMakeLists.txt` 35 (15 of them expected findings quoting fixture text), `cmake/galoot_testing.cmake` 12, `CMakeLists.txt` 9, `clifford_reference.py` 8, `tools/timed` 7, `.clang-tidy` 6, `conventions_check.py` 5, `CMakePresets.json` 4, `measure_header_test.py`, `expect_output.cmake`, `bare_user.cpp`, `assert_consteval_fail.cpp`, `.clang-format` 3 each, `tools/test_references.py` 2 (the new docstring's own wording), `assert_fail.cpp` 2, `trace_bad_table.md` 1. The narrowed run-log exception then also caught `CMakePresets.json:65` (item 2) | 0: "63 files, none citing a project document or record"; `tests.citations` passes on all seven presets | `python3 -I tools/test_references.py citations --exclude tools/citation_patterns.txt --exclude test/tools/test_references/markers_bad.cpp --exclude test/tools/test_references/markers_bad.expected --exclude test/tools/test_references/trace_bad_table.expected --exclude test/tools/test_references/citations_bad --exclude test/tools/test_references/citations_bad.expected include/galoot test cmake tools CMakeLists.txt CMakePresets.json .clang-format .clang-tidy` |
| 3: rejection and mutation | No check | `tools.test_references_citations_rejects` (36 findings in five file types) exits 1 as required; `..._accepts` exits 0. Each of 23 pattern lines removed in turn: 23 of 23 fail a rejection test. Lines searched one at a time, findings dropped: both fail. Bare C, S, V rejected, or the run-log exception dropped: the accepting tests and `tests.citations` fail | runs `mutation:no-pattern-line-<n>`, `mutation:lines-not-joined`, `mutation:no-citation-findings`, `mutation:bare-c-s-v-tokens`, `mutation:no-run-log-exception`, all exit 8 |
| 2: compile record | `CMakePresets.json:65` named `docs/agent/records/performance/compile.tsv`; a `-B` tree wrote to its preset's tree | `grep -n docs/agent CMakePresets.json CMakeLists.txt cmake test/CMakeLists.txt` finds nothing; gcc-measure 58 of 58 with rows in `build/gcc-measure/compile.tsv` and the committed record's MD5 unchanged; `-B build/gcc-debug-other` records in its own tree; the copy step rehearsed on a scratch copy | decision 0080 |
| 4: D2, rows | 20 items without rows passed; an extra `|` was skipped; Implemented per item | Each fails: `trace_bad_table.md:11` (per row), `:12` (extra cell), item 4.1 with no row; old script lacks all three | run `before:new-trace-tests-old-script` (exit 8) |
| 4: D6 | Helper headers not read | `tools.test_references_golden_helper_header` requires `helper.hpp:7: golden values of alpha are used outside a test case`; old script lacks it | same run |
| 4: D4 | Forms of the delta review passed | Each form is a rejection fixture (`citations_bad/bad.cpp` lines 9, 5, 11, 8, 21, 22), except bare C20, which stays accepted with the other bare C numbers (decision 0081); `markers_good.cpp` still passes | as item 3 |
| 4: D7 | ruff B023 at `test_references.py:314` | `ruff check --select B,E,F,W tools/test_references.py`: all checks passed | |
| 5: R-N1 | Mutant without task-clock survived | Killed: "shared events requested of perf" | `mutation:no-task-clock-requested` |
| 5: R-N2 | `ctest -L make -C Debug` recorded unavailable | Options read from the preset tree; `ctest -R ninja ... --test-dir` from that tree. Old recorder and the per-argument mutant fail both | `mutation:old-recorder`, `mutation:program-reset-per-argument` |
| 5: R-N3 | Two mutants survived | Killed: git failing gives branch `unavailable` (not `detached`); failed `git status` gives revision `unavailable` | `mutation:git-failure-as-empty`, `mutation:failed-status-ignored` |
| 5: R-N5 | A single blank line read as an empty header, exit 0 | Exit 1 naming the file | `mutation:blank-header-accepted` |
| 5: R-N6 | `cases` unpacked by position; three paragraphs unreflowed | Named cases, each checked against its own row; reflowed | |
| 6: R1 | Plain block in `conformal` passed all metric tests | `core.metric_conformal_wrap_zero` fails under that mutant on gcc-debug and clang-debug | `mutation:conformal-plain-block` (exit 8 twice) |
| 6: R3 | `;` in FORBID undocumented | Documented in `galoot_testing.cmake`: it splits, and both parts apply | |
| 6: R5 | Two short-wrapped comments | Reflowed; `clang-format --dry-run -Werror` clean | |
| 7 | No review | `docs/agent/snippets/reviews/M0-M1.md` | |

Tests added, with their expected values: `tests.citations` (exit 0, "N files, none citing ..."); `tools.test_references_citations_rejects` (exit 1, the 36 lines of `citations_bad.expected`); `tools.test_references_citations_accepts` (exit 0, "3 files, none citing a project document or record"); `tools.test_references_golden_helper_header` (exit 1, `helper_headers/helper.hpp:7: golden values of alpha are used outside a test case`); three new expected findings in `tools.test_references_trace_rejects` (`trace_bad_table.md:11: item 1.1 is implemented and no test states its fact`, `:12: row of item 2.1 is not three cells between | signs`, `item 4.1 of .../trace_conventions.md has no row`); `core.metric_conformal_wrap_zero` (build fails with the too-many-generators message and `static assertion failed`, without "at least one generator"); in `tools.measure_record`: shared events exactly `cpu_core/instructions/u,cpu_atom/instructions/u,task-clock`, options `-Wall -Werror -O1 -g` for both new ctest commands, revision and branch `unavailable` when git fails, revision `unavailable` and branch `main` when only `git status` fails, exit 1 naming the file for a blank header line, and per case `inst_e_u` of `0`, `unavailable`, `unavailable`, `unavailable`.

P9 at `748751f`, incremental trees except the new release, asan, and lint trees, `tools/timed workflow <p> all -- cmake --workflow --preset <p>`: gcc-debug, gcc-release, gcc-asan, clang-debug, clang-release, clang-asan, lint each 62 of 62; wall times (s) 4.95, 6.77, 9.08, 3.92, 6.15, 7.61, 3.56; `all_verify_interface_header_sets` on gcc-debug 0.37 and clang-debug 0.41. Gate commands from the worktree root: trace exit 0 (items with no test: 1.6, 1.7, 3.2 to 7.1); markers "28 test cases, each with a reference marker"; golden "every use of golden values states its fact"; `conventions_check.py` passes (seed 20261031). The final rerun on the closing revision is recorded below.

Verification and validation: verified, by the tests and mutations above, that each new check rejects what it targets and passes the tree. Validation is not applicable: the unit changes tools, tests, and comments, and no mathematical result; the one header change is two reflowed comments.

### Next step

1. W5 blind review of `feature/cleanup-1` against this plan and the departures above, then the merge into `develop`.
2. The owner decides the snippets review's recommendations, and whether `.gitignore`'s two comments should be reworded (departure 2).

### Open questions for the owner

1. Item 2's design (decision 0080) is proposed by this unit and may be reversed.
2. Whether the bare W and P rule numbers should stay rejected everywhere, as decision 0081 chose, at the cost of renaming a future variable such as P1.

### Final run on the closing revision

At `99b84cea1ae2` (this plan's outcome; only records changed since `748751f` and `2e8567d`, a comment), `tools/timed workflow <p> all -- cmake --workflow --preset <p>`: gcc-debug, gcc-release, gcc-asan, clang-debug, clang-release, clang-asan, lint each 62 of 62; wall times (s) 4.11, 3.74, 3.83, 3.73, 3.44, 3.39, 2.41; `all_verify_interface_header_sets` on gcc-debug and clang-debug pass. From the worktree root: the trace gate exits 0 with the same items without tests, the marker check finds 28 test cases all marked, the golden check passes, and `golden.py --check` matches. The commit after it adds only these lines and the run rows. All 117 run rows of the unit (builds, tests, mutations, gates) total 146.8 s of wall time.
