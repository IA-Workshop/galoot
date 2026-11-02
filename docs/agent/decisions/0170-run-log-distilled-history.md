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

   *Pointer added 2026-11-01, the text above kept as written:* "which include every gate run" is false; see the addenda below.

## Decision

Option 3, as plan 0017 designs it: `tools/runlog` (`summary`, `compact`, `check`, `prune`); the history in `docs/agent/records/performance/history/` (`eras.tsv`, `groups.tsv`, `toolchain.tsv`); `tools/timed --log <file>` and `TIMED_LOG` for campaign logs. A compaction keeps, besides the build and test rows, the anchor rows that keep union merges of branches based before the boundary correct, and is checked by regenerating the history from git byte for byte.

Proposed wording, for the owner to ratify:

P7, a new second sentence:

> The history also holds the run log of P13 in distilled form: for each kind of run (its step, preset, selection family, placement, and outcome), the number of runs, the revisions and times they span, the toolchain in force, and the first, latest, least, median, and range of their wall time, peak memory, and retired instructions.

P13, the first sentence replaced by two, and one sentence added after the sentence on the energy counter:

> Each build and test run is timed by `tools/timed`, which appends its wall time, revision, branch, preset, selection, and exit status to the run log, `docs/agent/records/performance/runs.tsv`. The runs of a measurement campaign go to the campaign's own log instead, named when the run starts and committed with the campaign's records.

> When a log grows too large to work with, the owner may approve its compaction at a recorded boundary revision: its rows up to the boundary are distilled into the performance history (P7) and removed from the log, except the rows of build and test runs of committed revisions, which include every gate run and stay in the log unchanged, and the few rows that later merges of other branches need. The removed rows stay in version control, and the history names the command that retrieves them.

*Pointer added 2026-11-01, the text above kept as written:* this sentence is false ("which include every gate run") and is replaced by the wording of the last addendum below, the one proposed for ratification.

DoD-25 of plan 0001:

> DoD-25 (P7, P13). The performance history baseline is committed; `runs.tsv` keeps a row for every gate run through every compaction of the log, and the history distills the rest (P7).

*Pointer added 2026-11-01, the text above kept as written:* replaced by the first addendum's DoD-25 wording below.

## Consequences

- `runs.tsv` drops from 235,590 rows to about 1,669 at the boundary plus later rows; the raw rows are read back with `git show <era>:docs/agent/records/performance/runs.tsv`.

  *Pointer added 2026-11-01, the text above kept as written:* with the rule widened in the first addendum, 1,727 rows of the era stay.
- Per-row values of the removed rows (each probe's time, counters, energy, options) are no longer in the working tree. Aggregates of wall time, peak memory, and instructions are.
- A branch based before a boundary that was not a local branch at compaction time can bring removed rows back in a merge; `tools/runlog check` finds them and `tools/runlog prune` removes them.
- Each later compaction, including the distilling of a closed campaign's log, needs the owner's approval until the wording above is ratified, and afterwards as the ratified P13 says.

## Addendum, 2026-11-01: after the W5 review (findings F1 and F2)

- Status of this addendum: proposed, like the record above, which is kept as written. Where the two differ, this addendum's wording is the one proposed for ratification.
- Recorded by: Claude (agent, Opus 5.5), performance-engineer role, implementer of plan 0017, on the coordinator's instruction recording the owner's intent: the information that shows how performance is tracking is preserved, and the promise about gate runs is honest.

### What the review found

The proposed P13 sentence above says the rows a compaction keeps "include every gate run", and the proposed DoD-25 says `runs.tsv` "keeps a row for every gate run". The first compaction did not do that. Gate runs are not marked in the rows, and the rule kept only build and test steps of committed revisions under certain names. It removed, for example, the whole recorded P9 gate of `develop`'s merge of WU1 (11 rows at `0863bad-dirty`), the cheap gates `reference-markers` and `golden-facts` at `d9c5059`, style checks with the step `lint`, and reviewers' trial-merge runs on uncommitted trees.

The rule was widened (plan 0017, "W5 review fixes"). It now keeps every `workflow` row of a named preset at any revision, `lint` rows of committed revisions, and four more cheap-gate names. In the first era that puts back 58 rows: 1,727 of the era's 235,590 rows stay instead of 1,669. Some gate steps are still outside the rule, such as the four `build` and `configure` rows of the WU1 merge at `0863bad-dirty`, and runs named in ways the rule does not know. They are counted in the history and retrievable from git, not kept in the log. The wording below says only what the rule keeps.

### Proposed wording, replacing the P13 sentence and DoD-25 above

P13, the sentence added after the energy sentence:

> When a log grows too large to work with, the owner may approve its compaction at a recorded boundary revision: its rows up to the boundary are distilled into the performance history (P7) and removed from the log, except the workflow runs of named presets, the build, test, style, and gate steps of committed revisions, and the few rows that later merges of other branches need, which stay in the log unchanged. Any other row, a gate run recorded under another name or on an uncommitted tree among them, is counted in the history and is retrieved from version control by the command the history names.

DoD-25 of plan 0001:

> DoD-25 (P7, P13). The performance history baseline is committed. `runs.tsv` keeps, through every compaction, the rows P13 says a compaction keeps, and every other gate run is counted in the history and retrievable from version control.

If the gate marker below is adopted, DoD-25 can promise more: "... and every run marked as a gate run".

### Proposed: a marker for gate runs

DoD-25 can promise that every gate run stays in the log only if the rows say which runs are gate runs. Two ways:

1. **A column.** This is not cheap. Every branch's recorder appends under a header it checks, and refuses a file whose header differs, so the column would have to reach every open branch at the same moment. A union merge of a log with the old header and one with the new would leave two header lines. Every existing row would also need a `--` in the new cell.
2. **A selection prefix (recommended).** The runs of a merge's or milestone's checks are named `gate:<selection>` (for example `tools/timed workflow gcc-debug gate:all-clean -- ...`), and a compaction keeps every row whose selection starts with `gate:`, at any revision. This changes no format and no recorder, and the rule change is one line in `tools/runlog`. It holds as long as callers use the prefix. The merge message, which P13 already says must name the selection, shows whether they did.

Proposed P13 sentence for option 2: "The runs of the checks of a merge or a milestone gate are named `gate:<selection>`, and a compaction keeps every row so named."

### Proposed: the check of the log as a merge check (F2)

A union merge can bring removed rows back with no conflict. For example, a branch based at an old merge of `develop` that appends one row brings back 88,931 rows. `tools/runlog check` finds this, and the CTest test `tools.runlog_check` now runs it in every preset's full suite. Proposed for ratification:

- P8, a row of the table: "| The run log, a campaign log, or the history in `docs/agent/records/performance/` | `tools.runlog_check` |".
- P9, after the table: "A merge that changes a log under `docs/agent/records/performance/` runs `tools/runlog check` on the merged tree before the merge is committed, at every level, and `tools/runlog prune` when the check reports removed rows."

Open for the owner: `tools.runlog_check` reads agent records. That sits against the principle of decision 0080, that clearing or moving agent records should not change what a build does for a human contributor. The test is skipped outside a git checkout and in a clone without the eras' commits. A contributor who deletes `docs/agent/records/performance/` would see it fail. If that is unwanted, the test can be skipped when that directory is absent, or kept out of the default test set by its label.
