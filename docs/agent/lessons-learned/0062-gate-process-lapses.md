# 0062 Process lapses found at the M0 and M1 gate

- Date: 2026-11-01
- Unit: M0 and M1 gate audit, Claude (agent, Sonnet 5.5)

## What happened

The audit found lapses that no single review had reason to see:

- A "before" cost was taken from a dirty build (WU4), so the comparison measured uncommitted work.
- Reviewers appended their run rows to the main checkout's `runs.tsv` rather than their worktree's.
- An empty decision file (0042) was created by an empty heredoc append.
- A write used a relative path and landed outside the worktree.
- Scratchpad logs were overwritten by later runs.
- A cleanup weakened a test; the review caught it.
- One pool slot was lost with no client holding it (22 of 23 free); the cause is unknown, and the pool was restarted with no record at the time.

## Cause

Each was a step done by hand without a check that its result was what was intended. The lost slot has no identified cause.

## What detects it now

Compare "before" and "after" only from clean revisions (no `-dirty`). Reviewers write run rows into their own worktree and the coordinator appends them at merge. Create files with the Write tool or check `wc -c` after a heredoc. Use absolute paths for writes. Give logs unique names. A cleanup that changes a test is listed in the review request (W9). Before starting a build, `tools/jobpool status` is read; a restart is logged in the run record the moment it happens, and lost slots are reported to the owner.
