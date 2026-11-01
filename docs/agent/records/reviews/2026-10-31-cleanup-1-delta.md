# W5 delta review: feature/cleanup-1 (plan 0007 fix pass), b9d87ec to ed3783e

- Reviewer: blind-reviewer, Claude Opus 5.5, 2026-10-31, in a detached worktree of its own (removed after use; its 37 run rows, 118.3 s, are in the session scratchpad under `rv2/review-cleanup2-runs.tsv`). Before noticing, the reviewer overwrote three workflow logs left in the shared scratchpad by an earlier agent.
- Reviewed: `git diff b9d87ec ed3783e` (15 branch commits plus three records-only develop commits via the 58bd179 merge); every changed file read apart from the `runs.tsv` rows. `develop` moved to c0842a7 (two cost rows).
- Verdict: merge after fixes D1 and D2 (a few lines and one fixture row, in a separate test commit), then merge develop (D9). D3 to D8 are non-blocking.
- Not run: gcc-release, gcc-asan, clang-debug, clang-release, the measure presets, the header-set check (the plan records 62 of 62 for all seven presets at 4d998f5, and ed3783e changes only the plan and run rows); the author's 25 line removals, 48 part narrowings, and leader mutations (ten mutations of the reviewer's own were run instead); the Ninja lock claim; the copy step against the committed record (scratch copies only). The repository has no ruff configuration; ad hoc ruff reports nothing new.

## Checks

- Clean builds at ed3783e, 62 of 62: gcc-debug 4.56 s, clang-asan 6.95 s, lint 3.25 s; no CMake warning with the default record. Gates: trace exit 0 (items with no test: 1.6, 1.7, 3.2 to 7.1), markers 28 of 28, golden passes, citations "63 files, none citing"; the fixture output matches `citations_bad.expected` exactly (70 lines; the earlier 36 byte-identical).
- W2 holds at each of the 14 first-parent commits (exported with `git archive`; gcc-debug 62 of 62). Tests are committed apart from code; every commit carries agent attribution. No removed line weakens anything: each pattern edit is a superset of the old one; the test edits only add assertions.
- B1: a mutant writing no row for a `<not counted>` event without task-clock passes the old test and fails the new ("got 2, expected 3"); a mutant writing no row when task-clock times differ fails both; the unmutated recorder passes.
- N1 comment accurate. N2: a `1.6.` first cell gets the new finding (regression in D1). N3: all 25 of the reviewer's bad lines are caught; one contrived miss (a split at the hyphen of "decision-" with "0080" on the next line); legitimate text passes (C3, S2, V4, M2(R), Lengyel, Dorst, "For C3, see the rotation", "per the 2 by 2 case", "ticket counter").
- N4: an empty or whitespace-only expected file fails configure. N5: the warning fires for a record outside the tree, a sibling tree with a shared prefix, `t4/../escape.tsv`, and a relative path; not for a subdirectory of the tree or after `-UGALOOT_COMPILE_RECORD`; one harmless false positive through a symlink. The copy step run verbatim on a scratch copy and a fresh tree built once (8 rows): fresh clean tree appends 8 rows; short row, header mismatch, `-dirty` revision, two revisions, header only, and missing file each exit 1 with nothing appended; revision `unavailable` and rows duplicated by a rebuild are appended (D4).
- N6, N7, N8, and the `.gitignore` rewording accurate (`git check-ignore -v` unchanged). N10: `cmake -E env`, `cmake -E time`, nested `-E`, and an absolute cmake path all give the tree; `ctest -L make` still gives `build/<preset>`.
- No citation, narration, or changelog comment in added lines.

## Findings

- D1 (fix before merge, confirmed). A regression in the trace gate at `tools/test_references.py:443-445`: rows before a separator line are now skipped, so a second table with a header and no separator, holding `| 1.6 | a fact no test states | yes |`, exits 0 (the old script exits 1). A single table without its separator still fails loudly, so today's file is safe. Treat an item-number first cell before any separator as a finding, with a fixture line.
- D2 (with D1). Decision 0082 was not amended: its options still say "a line is a row when its first cell is an item number", and the `read_table` docstring lists only the three-cell finding.
- D3 (non-blocking). Citation parts with no fixture: narrowing `(?:per|see|cf\.?)` to `(?:per|see)`, or `[CSV]` to `[CS]` in that pattern, passes every fixture test; also a multi-level `§` reduced to `\d+\.\d+` and `tickets?` to `ticket`; and dropping `\b` before `per` (which would reject "upper S2") has no near-miss in the accepting fixture.
- D4 (non-blocking). The copy step copies rows whose revision is `unavailable` and rows duplicated by a rebuild at the same revision; its failures are silent (dirty, two revisions, header only, and a short row each exit 1 with no message; a missing file gives a traceback).
- D5 (non-blocking). The build-expert definition points to "the copy step of decision 0080", whose Decision section still holds the old block above the amendment that replaces it.
- D6 (non-blocking). Lesson 0001 still names "more than one field count" as a detector; that check now fires on every file.
- D7 (non-blocking, pre-existing). A rejecting test still passes when an unexpected extra finding appears (appending "see decision 0099" to the parts fixture left the test passing).
- D8 (plausible, pre-existing). `cmake -E chdir T ninja` records `build/<preset>` instead of T.
- D9. Merge `develop` into the branch or at the merge (records only).
