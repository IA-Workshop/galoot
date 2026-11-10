# 0072 A scratch experiment restores only the files it changed, never the run log

- Date: 2026-11-10
- Unit: WU17 part D (plan 0038), found by its delta review (R1)

## What happened

During the review fixes of part D, the test author ran stand-in compiles and runs through `tools/timed` in the tests worktree (`.claude/worktrees/cost-evidence--bench--tests`). Each run appended a row to that worktree's `docs/agent/records/performance/runs.tsv`. After committing the test change, the author ran `git checkout -- docs/agent/records/performance/runs.tsv` there, intending to leave the worktree clean as it had done once before. That time it had copied the rows to the code branch first; this time it had not. About 13 rows were lost. They were scratch runs on an uncommitted tree, so no gate row and no merge evidence went, but they were the only run evidence for two claims in a commit message, which other evidence had to carry afterwards (`records/performance/wu17-bench/part-d-build-cost.md`, departure 4).

## Cause

`runs.tsv` changes whenever a run goes through `tools/timed`, so it is never one of the files an experiment set out to change. A checkout or reset that names it, or a broad one such as `git checkout -- .` or `git stash`, deletes rows that version control does not yet hold, against W7.

## What to do

- To undo a scratch experiment, restore by name only the files it edited. Never name `runs.tsv` or a campaign log in a `git checkout`, `git restore`, `git reset --hard`, or `git stash`.
- Before cleaning a worktree whose branch is never merged (a tests branch, a review branch), first copy its new run rows into a branch that is merged, or commit them there, and check with `git diff --stat` that the log is the only file left changed.
- Run `git status --short` before any command that discards changes, and read the list of files it would touch (W7: resolve and list the targets first).
