# 0170 The run log is distilled into the performance history; campaigns keep their own logs

- Status: proposed, 2026-11-01. The wording of P7, P13, and DoD-25 below takes effect only when the owner ratifies it. The first compaction of `runs.tsv` (plan 0017) is done under the owner's ruling of 2026-11-01 and does not wait for this record.
- Date: 2026-11-01
- Proposed by: Claude (agent, Opus 5.5), performance-engineer role, planner of plan 0017.

## Context

On 2026-11-01 `docs/agent/records/performance/runs.tsv` held 235,590 rows in 76 MB. 210,848 of them were `ceiling-cert:*` compiles of the limit searches of research note 0017 and plan 0015, and in all 221,684 had the step `measure`; 124,928 of those were at `-dirty` revisions. Every branch appends to the file and merges it with git's `union` driver. A reviewer of plan 0015 recommended that measurement campaigns write to their own log. The owner ruled: "The run log should be preserved insofar as the key information is distilled to show how performance is tracking, but the raw entries are getting unwieldy."

P13 says `tools/timed` appends every build and test run to `runs.tsv`; P7 says the performance history shows how measured quantities vary; DoD-25 (plan 0001) says `runs.tsv` covers every gate run. W7 says version control is the recovery path and no archive copies are kept.

## Options

1. Leave the log as it is and only route future campaigns elsewhere. The 76 MB stays; every merge and every `git status` diff of the log keeps its cost.
2. Move the campaign rows into per-campaign files. The bulk stays in the working tree under other names, which is an archive copy in all but name.
3. Distill every row up to a boundary revision into a committed history of per-kind statistics, keep the build and test rows of committed revisions (which include every gate run) in the log verbatim, remove the rest, and leave the raw rows in git, retrievable by one command. Route future campaigns to their own logs, distilled the same way when the campaign closes.

## Decision

Option 3, as plan 0017 designs it: `tools/runlog` (`summary`, `compact`, `check`, `prune`); the history in `docs/agent/records/performance/history/` (`eras.tsv`, `groups.tsv`, `toolchain.tsv`); `tools/timed --log <file>` and `TIMED_LOG` for campaign logs. A compaction keeps, besides the build and test rows, the anchor rows that keep union merges of branches based before the boundary correct, and is checked by regenerating the history from git byte for byte.

Proposed wording, for the owner to ratify:

P7, a new second sentence:

> The history also holds the run log of P13 in distilled form: for each kind of run (its step, preset, selection family, placement, and outcome), the number of runs, the revisions and times they span, the toolchain in force, and the first, latest, least, median, and range of their wall time, peak memory, and retired instructions.

P13, the first sentence replaced by two, and one sentence added after the sentence on the energy counter:

> Each build and test run is timed by `tools/timed`, which appends its wall time, revision, branch, preset, selection, and exit status to the run log, `docs/agent/records/performance/runs.tsv`. The runs of a measurement campaign go to the campaign's own log instead, named when the run starts and committed with the campaign's records.

> When a log grows too large to work with, the owner may approve its compaction at a recorded boundary revision: its rows up to the boundary are distilled into the performance history (P7) and removed from the log, except the rows of build and test runs of committed revisions, which include every gate run and stay in the log unchanged, and the few rows that later merges of other branches need. The removed rows stay in version control, and the history names the command that retrieves them.

DoD-25 of plan 0001:

> DoD-25 (P7, P13). The performance history baseline is committed; `runs.tsv` keeps a row for every gate run through every compaction of the log, and the history distills the rest (P7).

## Consequences

- `runs.tsv` drops from 235,590 rows to about 1,669 at the boundary plus later rows; the raw rows are read back with `git show <era>:docs/agent/records/performance/runs.tsv`.
- Per-row values of the removed rows (each probe's time, counters, energy, options) are no longer in the working tree. Aggregates of wall time, peak memory, and instructions are.
- A branch based before a boundary that was not a local branch at compaction time can bring removed rows back in a merge; `tools/runlog check` finds them and `tools/runlog prune` removes them.
- Each later compaction, including the distilling of a closed campaign's log, needs the owner's approval until the wording above is ratified, and afterwards as the ratified P13 says.
