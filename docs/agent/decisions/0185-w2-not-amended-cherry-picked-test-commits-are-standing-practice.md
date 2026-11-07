# 0185 W2 is not amended; test commits cherry-picked one at a time before their code are the standing practice

- Status: accepted (owner answer 2026-11-07, relayed by coordinator: "proceed with recommendations", M3 gate report owner decision 4)
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), records clerk role
- Bears on: WORKFLOW W2 and W9; lesson 0067; plan 0020 section 1 ("The W2 question"); plan 0021 section 1 ("The W2 question"); the post-WU10 audit's owner item 4

## Context

When a test author writes tests before an implementer writes the code, the test commits cannot build before the code exists. WU10a and WU10b left runs of five and fifteen non-building commits (lesson 0067), while W2 allows only a single failing test commit that says so. WU11a avoided the run: the test author committed each group on a tests branch, and the implementer cherry-picked each group with `-x` immediately before the code it judges, so each cherry-picked commit was the single failing commit W2 allows, and its message said so. The M3 audit found that this met W2 with no departure and kept the tests independent.

## Options weighed

1. Do not amend W2; record WU11a's mechanics as the standing practice (recommended).
2. Amend W2 to allow a declared run of failing test commits.

## Decision

Option 1. W2 stays as ratified. For a unit with a separate test author:

1. The test author commits each test group on the test author's branch (W7, decision 0188), first compiling it against a scratch stand-in to show it is well formed and that the plan's mutants fail it.
2. The implementer cherry-picks each group with `git cherry-pick -x` immediately before the commit of the code it judges. That commit is the single failing commit W2 allows, and its message says so.
3. The test author's branch is never merged.

## Consequences

- The question of lesson 0067 is closed; the lesson points here.
- Plan 0021 follows these mechanics as written in its section 1.
