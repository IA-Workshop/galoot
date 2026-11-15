# 0007 Cleanup 1: citations, gate hardening, test gaps, snippets review

- Status: closed; the unit merged into `develop` by `6857058` on 2026-10-31 (status corrected 2026-11-11, from `git log --merges` on `develop`). Status text as written before that correction, kept as history: closed (outcome below), awaiting the W5 review and the merge; branch `feature/cleanup-1`, worktree `.claude/worktrees/cleanup-1`; decision numbers 0080 to 0089 (0080 to 0082 used).
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

## W5 fixes (2026-10-31, Claude, agent, Opus 5.5)

The review `docs/agent/records/reviews/2026-10-31-cleanup-1.md` (on `develop`) returned "merge after named fixes". This section records the fix pass, `dd549fb` to `4d998f5` plus this record; the outcome above stands except where it changes it.

### Departures and ambiguities (C20)

1. N1: the comment was made truthful (FORBID stricter, REQUIRE looser, write neither with a `;`) rather than rejecting `;` in REQUIRE: `cmake_parse_arguments` has already split the value, so a `;` cannot be told from two values.
2. N3: one part has no fixture of its own: the whitespace run `\s+` is exercised by a double space only in the findings pattern (gap recorded in decision 0081). "§3.1 of the conventions", which the review listed and the brief did not, is caught too.
3. N5: beyond the brief, the copy step also checks that the tree's rows come from one revision that is not `-dirty`. The fail-fast step was rehearsed on scratch copies only; the committed record is unchanged.
4. N6: the correction of the snippets review also notes that lesson 0001's field-count check no longer detects anything (that lesson is not edited).
5. The B1 mutant had to be narrowed: a recorder that crashes whenever task-clock is missing is already caught by another case; the survivor is one that writes no row for a `<not counted>` event without task-clock.

### Fixes

| Finding | Fix | Before | After |
| --- | --- | --- | --- |
| B1 | `counters_per_core_type` checks the record has index + 1 rows after each case (`dd549fb`, test only) | Mutant writing no row for a `<not counted>` event without task-clock: 1 of 1 passes | Same mutant: "rows after the case no task-clock line: got 2, expected 3" and the same for the fourth case; run `mutation:no-row-without-task-clock`, exit 0 then 8 |
| B2 | `develop` (af48044: plan 0008, the review record, agent-cost rows) merged as `58bd179` | Three records-only commits behind | Level; records only, so P10 covers the merge; one header in `runs.tsv` and `agent-cost.tsv` |
| N1 | `galoot_testing.cmake` comment | "errs on the strict side" for both | FORBID stricter, REQUIRE looser |
| N2 | `read_table`: every line after a table's separator is a row; a first cell that is not an item number is a finding (`7291f4f`; tests `459b524`) | `1.l` and `**1.2**` rows skipped | `trace_bad_table.md:13: row's first cell '1.l' is not an item number`, `:14: ... '**1.2**' ...`; the old script lacks both (run `before:malformed-items-old-script`) |
| N3 | Patterns widened (`b953b8b`); `citations_bad/parts.cpp` and a `#` split in `bad.cmake` (`4469299`) | `decision-0080`, `plan-0007`, `finding  B1`, `ticket D2`, `per S11`, `see C20`, `§3.1 of the conventions` passed; part narrowings survived | 70 expected findings (36 unchanged, 34 new); 25 of 25 line removals and 48 of 48 part narrowings fail a rejection test (runs `mutation:no-pattern-line-<n>` and `mutation:<part>`) |
| N4 | `galoot_expected_findings` fails on an empty file (`b7ed556`) | An empty `.expected` passed on exit 1 alone | Configure fails: "<file> lists no expected finding" (run `mutation:empty-expected`, exit 1) |
| N5 | Configure-time warning when `GALOOT_COMPILE_RECORD` is not under `CMAKE_BINARY_DIR`; copy step with `set -eu`, one-revision and not-dirty checks, and the clean-build rule (decision 0080 amendment; `d75cdd8`) | Silent | `-DGALOOT_COMPILE_RECORD=<scratchpad>/outside.tsv` warns, naming the file; the default does not; the step refuses the `gcc-debug` tree (rows of several revisions; nothing appended) and copies 8 rows from a fresh `build/gcc-measure-gate` at `d75cdd8` |
| N6 | Snippets review sentence | "one field count (28)" | 9, 26, and 28 fields (2125, 1808, 347 rows); one header |
| N7 | Trace editing rule, `tools/measure` dirty-check comment, `CMakeLists.txt` record comment | Stale | Row's own fact; the compile record changes only by the copy step; "writes nothing outside" only for the default |
| N8 | Ninja-lock comment | Stated as fact | Marked inferred, not checked against Ninja's source |
| N10 | `build_tree` looks for the program again after `cmake -E` (`fe3ca16`; tests `11509b9`) | `cmake -E env FOO=1 ctest --test-dir <tree>` and `cmake -E time ninja -C <tree>` read `build/<preset>` | Both read `<tree>`; the old recorder and the rule disabled both fail (runs `mutation:old-recorder-cmake-e`, `mutation:no-cmake-e-wrapper`) |
| .gitignore | Two comments drop "(PERFORMANCE.md P7)" and "(WORKFLOW.md W6)" | | `git check-ignore -v` gives the same rules for the workbench, `build/`, and `__pycache__` |
| W and P | Decision 0081 amendment: the first mathematical use narrows them to the parenthesized rule | | |

Known gaps, not changed (review N9 and N11): `golden` and `trace` read `*_test.cpp`, `*.hpp`, and `*.h` under `test/`, not helper `*.cpp` or `*.hh` files; a fact is matched as a substring, so a short fact is pinned by any marker that contains it. The citation check scans the filesystem, not git, so an untracked local file can change its result.

Tests changed or added, with expected values: `tools.measure_record` gains a row count per core-type case (1, 2, 3, 4) and options `-Wall -Werror -O1 -g` for `cmake -E env FOO=1 ctest --test-dir <tree>` and `cmake -E time ninja -C <tree>`; `tools.test_references_trace_rejects` gains the two malformed-row findings above; `tools.test_references_citations_rejects` requires the 70 lines of `citations_bad.expected`; `tools.test_references_citations_accepts` still requires "3 files, none citing a project document or record", now with near-misses for `see` and `ticket`. No existing expected value changed.

P9 at `4d998f5`, `tools/timed workflow <p> all -- cmake --workflow --preset <p>`: gcc-debug, gcc-release, gcc-asan, clang-debug, clang-release, clang-asan, lint each 62 of 62, no CMake warning; wall times (s) 2.82, 2.33, 3.28, 1.70, 2.75, 2.82, 1.24; `all_verify_interface_header_sets` on gcc-debug and clang-debug pass. From the worktree root: trace gate exit 0 (items with no test: 1.6, 1.7, 3.2 to 7.1), markers 28 of 28, golden passes, citations "63 files, none citing a project document or record", `golden.py --check` matches, `conventions_check.py` passes (seed 20261031). The commit after it adds only this section and run rows. The fix pass logged 237 run rows, 78.1 s of wall time, most of them mutation runs.

Verification and validation: verified by the tests and mutations above; validation not applicable (tools, tests, comments, and records only).

## Delta review fixes (2026-10-31, Claude, agent, Opus 5.5)

The delta review `docs/agent/records/reviews/2026-10-31-cleanup-1-delta.md` (on `develop`) asked for D1 and D2 before the merge and D3 to D9 as non-blocking. All are done, `64cc969` to `cbd59f1`, plus this record.

### Departures and ambiguities (C20)

1. D7: the exact mode (`ONLY <regex>` in `test/expect_output.cmake`) applies to the six `test_references` rejecting tests (`ONLY "^/"`, since every finding starts with the fixture's path) and to `tools.include_scan_rejects` (`ONLY "."`). It immediately found a finding the golden rejection test always produced and never listed (`golden_bad.cpp:6`, the `using` directive's use of gamma outside a test case); that literal was added, a stricter expectation. `trace_no_rows` now lists all nine findings of the empty table in `trace_empty_table.expected`, which `tests.citations` skips like the other expected files, so the citation command in this plan's outcome gains `--exclude test/tools/test_references/trace_empty_table.expected`.
2. D7 also fixed a weakness found on the way: since the expected files were added, a `;` inside an expected line (`'(C13; S6)'`) was split by CMake into two literals, each checked alone. Literals and output now hold `;`, `[`, and `]` as control characters while compared.
3. D4: the copy step is a shell function in decision 0080's second amendment, rehearsed from that file's text on scratch copies; the committed record is unchanged. A refusal after appending (the final `tools/measure read`) cannot be undone by the function; its message says to restore the record with git.
4. D8: `cmake -E chdir <dir> <program>` takes `<dir>` as the tree when the program names none, and a tree it names relative to `<dir>`.

### Fixes

| Finding | Fix | Before | After |
| --- | --- | --- | --- |
| D1 | `read_table`: a line before a separator whose first cell is an item number is a finding (`64cc969`; test `48c76b5`) | A second table with a header, no separator, and `| 1.2 | ... | yes |` passed | `trace_bad_table.md:17: row of item 1.2 comes before any separator line`; the script at `ed3783e` lacks it (run `before:row-before-separator-at-ed3783e`, exit 8) |
| D2 | Decision 0082 amendment replaces "a row is a line whose first cell is an item number"; `read_table` docstring lists the three findings | | |
| D3 | `parts.cpp`: cf. S12, cf V3, per V9, §3.1.2 and §3 of the conventions, tickets D3; `good.cpp`: "upper S2", "proper C3" (`1500c06`) | Eight narrowings survived | 8 of 8 killed; dropping `\b` before per fails the accepting test on both near-misses; 76 expected findings, the earlier 70 unchanged |
| D4 | Copy function with a message per refusal (decision 0080, second amendment, `a38f3ff`); `tools/measure read` names an unreadable file (`08ab8ba`; test `e6a203a`) | `unavailable` revisions and rebuilt rows copied; refusals silent; a missing file gave a traceback | Clean tree: 8 rows copied. Missing record, other header, no rows, two revisions, `-dirty`, `unavailable`, and a compile written twice: each "copy refused: <reason>", exit 1, copy unchanged. Missing file: "measure: cannot read <path>: No such file or directory"; the recorder before `08ab8ba` fails the new check (run `mutation:old-recorder-missing-file`) |
| D5 | Decision 0080's two earlier blocks marked as replaced; `.claude/agents/build-expert.md` points to the second amendment's function | Pointed to the first block | |
| D6 | Lesson 0001 amendment: the field-count detector no longer works; the header count and `tools/measure read` detect a doubled record | | |
| D7 | `ONLY <regex>` (`0cfbf9c`) | An extra finding passed | Appending "See decision 0099." to `parts.cpp` fails with "output has an unexpected line"; with the ONLY check disabled it passes (runs `mutation:extra-finding`, `mutation:extra-finding-no-only`) |
| D8 | `build_tree` reads `cmake -E chdir` (`41a3635`; test `f2e64c9`) | `cmake -E chdir T ninja` read `build/<preset>` | Both new checks pass; the recorder before `41a3635` gets unavailable for both (run `mutation:old-recorder-chdir`) |
| D9 | `develop` (`70060c5`: the delta review and cost rows) merged as `cbd59f1` | Behind by records only | Level; P10 covers the merge |

Tests changed or added, with expected values: `tools.test_references_trace_rejects` adds `trace_bad_table.md:17: row of item 1.2 comes before any separator line`; `tools.test_references_citations_rejects` requires the 76 lines of `citations_bad.expected`; `tools.test_references_citations_accepts` still "3 files, none citing a project document or record"; `tools.test_references_golden_rejects` adds `golden_bad.cpp:6: golden values of gamma are used outside a test case or a static_assert`; `tools.test_references_trace_no_rows` requires the nine lines of `trace_empty_table.expected`; all six `test_references` rejecting tests and `tools.include_scan_rejects` fail on an unlisted finding; `tools.measure_record` adds options `-Wall -Werror -O1 -g` for `cmake -E chdir <tree> ninja` and `cmake -E chdir <top>/build ninja -C p`, and exit 1 naming the file without "Traceback" for a read of a missing record.

P9 at `cbd59f1`, `tools/timed workflow <p> all -- cmake --workflow --preset <p>`: gcc-debug, gcc-release, gcc-asan, clang-debug, clang-release, clang-asan, lint each 62 of 62, no CMake warning; wall times (s) 3.94, 3.73, 3.46, 3.50, 3.77, 3.40, 2.99; `all_verify_interface_header_sets` on gcc-debug and clang-debug pass. From the worktree root: trace gate exit 0 (items with no test: 1.6, 1.7, 3.2 to 7.1), markers 28 of 28, golden passes, citations "63 files, none citing a project document or record" (with the `trace_empty_table.expected` exclusion), `golden.py --check` matches, `conventions_check.py` passes (seed 20261031). The commit after it adds only this section and run rows. This pass logged 55 run rows, 77.6 s.

Verification and validation: verified by the tests and mutations above; validation not applicable (tools, tests, and records only).
