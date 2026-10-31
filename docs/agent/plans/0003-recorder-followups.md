# 0003 Recorder follow-ups (WU1b)

- Status: claimed; starts after `feature/build` merges into `develop`
- Branch: `feature/recorder` (worktree `.claude/worktrees/recorder`)
- Goal: carry out items 1 to 3 of decision 0032 in the recorder.
- Definition of done: (1) every run and compile row records the effective compiler options in an `options` column; (2) pinned rows record other-process busy time on the pinned CPU; (3) readers treat nine-cell rows as `--` for the missing cells, with a test; (4) the header extension is done once, in place, with `runs_tsv_merge.py` updated to match; (5) the smoke path and the existing recorder checks still pass on both compilers.
- Acceptance: the same recorder checks as plan 0002 criteria 10 to 12, plus one row each for an overridden `-O` level and a pinned run showing the new fields.
- Evidence: the recorder test output and one row of each kind, with the revision and command.
- Agent cost: tracked for information only (decision 0019); record in `docs/agent/records/agent-cost.tsv`.
- Open questions: none.

## Additional scope from the WU1 review (non-blocking findings N5 to N16)

Findings N5, N6, N7, N8, N9, N10, N12, N13, N14, N15, and N16 of `docs/agent/records/reviews/2026-10-31-wu1-build-skeleton.md` are carried here, except that N9 and N10 are decision-record edits and N12 is a re-record of the P1 baseline on a clean revision. N11 is a process note for the next review request. The blocking findings B1 to B3 and the cheap N1 to N4 are fixed on `feature/build` before it merges.
