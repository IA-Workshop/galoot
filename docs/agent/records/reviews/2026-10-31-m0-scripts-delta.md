# W5 delta review: feature/m0-scripts (M0 closure fixes), 5db7e63 to 12fffdd

- Reviewer: blind-reviewer, Claude Opus 5.5, 2026-10-31, in a detached worktree of its own (removed with `--force` because its run rows were uncommitted there; the nine rows are saved in the session scratchpad as `review-m0b-runs.tsv` for appending at the merge).
- Verdict: merge after D1 (merge the current `develop` in). Every original finding is resolved or stands only as a note.
- Not run by the reviewer: gcc-release, gcc-asan, clang-debug, clang-release (the diff from b3487d6 to 12fffdd outside `docs/` is empty, so the author's 21 of 21 stands under P10). Not timed: the one-off probe runs, ruff, and `git merge-tree`.

## Checks

- Clean builds, 21 tests each: gcc-debug 2.45 s, clang-asan 4.36 s, lint 2.29 s; header sets on gcc-debug 0.53 s.
- The golden header changed only by ten `// Fact:` lines; the reviewer's independent derivation reports 0 mismatches; `conventions_check.py` passes and its edit is only the lift of the table to module level.
- Mutations of copies, each caught: the rigid(3) name `e23` to `e32`; a reordered ordering example; a use through an alias without its fact; `using` of golden names (two forms); an unmarked namespace-scope `static_assert`; a marked one with the wrong fact; a namespace-scope variable outside a test case; setting row 3.1 to `yes` (the gate exits 1) and to `Yes` (rejected). Facts in capitals match, and the wedge and minus signs are read as `^` and `-`.
- The pointer pattern rejects "section 3.1", "WU1", "VVUQ kind 3", "style S11", "gate M0", "M1 milestone", "spike A" and accepts C3.
- The documented gate command, run from the repository root, exits 0 and lists all 29 items with no test; every row is `no`, which is accurate.
- The build files name no `docs/agent` path. Added lines obey the style rule apart from the deliberate pointer text in the rejecting fixtures.
- Test and script went in separate commits (7025d83, b3487d6).
- Bits and metric: every marker finding is "no reference marker" (15 on bits, 11 on metric, 2 each already fixed here); `golden` passes on both; they will pass once markers are added.

## Findings

- D1 (before the merge). `develop` is three records-only commits ahead; merge it in (W7); P10 covers the test run.
- D2. The gate passes vacuously for 20 of the 29 convention items: items 3.2 to 7.1 have no row, so they can never be marked implemented. Fail on a convention item with no row before the first of them is implemented. Related: Implemented is set per row but evaluated per item, so a `yes` row can be satisfied by a test that states another row's fact; a table line with an extra `|` is skipped without a finding.
- D3. Commit 42f0c6e fails its tests (it renames and deletes fixtures that `test/CMakeLists.txt` still names at that commit); plan 0006 mentions only 7025d83. Record it there; history stays.
- D4. Pointer forms that still pass: "the research note on complements", "§3.1 of Conventions.md", "review finding B1", "the performance record and decision 34", and bare "W5", "C20", "P9" (an accepted trade-off for C3 and S2; bare W and P tokens could be rejected alone).
- D5. The gate command is documented only in the trace table and decision 0072; the workflow document, AGENTS.md, and the reviewer and auditor definitions do not mention it, and the command assumes the repository root. Point the merge procedure or the agent definitions at it (an owner call where governance changes).
- D6. Decision 0072's "not handled" wording names the wrong gap: a namespace-scope variable or macro in a `*_test.cpp` is caught as a use outside a test case. The real gap is that the checks read only `*_test.cpp`, so a helper header under `test/` that uses `golden::` values is never scanned.
- D7. Nit: ruff B023 at `tools/test_references.py:314` (the closure reads the loop variable `aliases`); harmless, pass it as an argument.

Outside this delta, for the cleanup unit: `CMakePresets.json:63` writes to a `docs/agent` path from the measurement presets; `test/CMakeLists.txt` comments cite plans, decisions, notes, and review findings; smoke test names cite a style provision and a plan.
