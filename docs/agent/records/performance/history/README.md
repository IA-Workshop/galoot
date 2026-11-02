# The performance history

The distilled run log (P7, P13; decision 0170, proposed; plan 0017). `tools/runlog` writes and checks everything here except `toolchain.tsv`, which is written by hand. Every file is append-only, so the union merge rule of `.gitattributes` keeps it whole.

## Files

| File | One row per | Written by |
| --- | --- | --- |
| `eras.tsv` | compaction (era) of a log | `tools/runlog compact` |
| `groups.tsv` | era and kind of run | `tools/runlog compact` |
| `toolchain.tsv` | state of the toolchain on a host, from its `since_utc` until the next row of that host | hand, from the package manager's log and each tool's `--version` |

### `eras.tsv`

`era` is the boundary revision (12 hex digits); `log` the log's path; `rows` the rows the era holds (every row of the log at `era` that no earlier era of the same log kept); `kept` how many of them stayed in the log; `anchors` the line numbers, in the log at `era`, of the rows kept only so that union merges of older branches stay correct (`-` if none); `first_utc` and `latest_utc` the earliest and latest `utc_start` of the era's rows.

### `groups.tsv`

A kind of run is the tuple `step`, `preset`, `family` (the selection up to its first `:`), `mode` (`pinned`, `shared`, or `--` for rows written before the column existed), and `outcome` (`pass` for exit 0, else `fail`). Pinned and shared figures are never compared (P14).

| Column | Meaning |
| --- | --- |
| `rows`, `dirty` | rows of the kind, and how many of them were at a `-dirty` revision |
| `revisions` | distinct revisions, a `-dirty` one counted with its clean twin |
| `first_utc`, `latest_utc`, `first_revision`, `latest_revision` | the first and latest row by start time, ties in log order |
| `hosts` | the machines, comma-separated |
| `toolchains` | the `since_utc` of each state of `toolchain.tsv` in force on those hosts between the first and latest row; `unrecorded` for a time before the first state |
| `<q>_n` | rows with a value of the quantity; the rest are `unavailable` or `--` in the row |
| `<q>_first`, `<q>_latest` | the value of the first and latest of those rows |
| `<q>_min`, `<q>_median`, `<q>_range` | the least value, the low median (the lower of the two middle values for an even count, so always a measured value), and the largest minus the least |

The quantities `<q>` are `wall_s`, `maxrss_kib` (peak resident set of the command and its children, KiB), and `inst_u` (user-space retired instructions: `inst_p_u + inst_e_u` when both are counts, or `inst_p_u` alone in a pinned row). Arithmetic is decimal, so `tools/runlog check` can recompute every row exactly. User and system time, cycles, cache events, energy, and the busy time of other processes are not distilled; they stay in the kept rows and in git.

## The rows that stay

A compaction leaves in the log, byte for byte and in order: every row whose revision is committed (12 hex digits, no `-dirty`) and that is a `configure`, `build`, `test`, or `workflow` run of a named preset, a `gate` step, or a `check` or `trace` step of the cheap gates (`trace`, `trace-gate`, `convention-trace`, `markers`, `golden`, `citations`); the anchors; and every row written after the boundary. Every gate run is among them.

## Retrieving the raw rows

The raw rows of an era are the log as it was at the era's revision:

```bash
git show <era>:docs/agent/records/performance/runs.tsv > /tmp/runs-<era>.tsv
git show <era>:docs/agent/records/performance/runs.tsv | awk -F'\t' 'NR == 1 || $6 ~ /^ceiling-cert:/'   # one family
```

That output also holds the rows that stayed and, for a later era, the rows earlier eras kept.

## Commands

From the root of a worktree:

- `tools/runlog summary <log>`: the groups of every row of a log, for a look at a log that is not compacted (a campaign's, or the run log since the last era).
- `tools/runlog check`: recomputes each era from git and compares it with `eras.tsv` and `groups.tsv` byte for byte, and checks that each log holds each kept row once and no removed row. Run it after any merge that touched a log; exit 1 lists every difference.
- `tools/runlog prune`: removes rows that a merge brought back (a branch based before a boundary whose base was not an anchor). Commit the result with the count it prints.
- `tools/runlog compact <revision> [--log <path>]`: a new era (below).

## Compacting a log

Only with the owner's approval (decision 0170). The boundary is a commit on `develop`, usually the commit that claims the unit; the unit's branch starts there.

1. Commit the rows the branch has appended so far, on their own.
2. Run `tools/runlog compact <boundary>`. It refuses if the era exists, if the history's headers are not its own, or if the working log lacks a row of the boundary's log. It finds the anchors itself from the local branches not merged into the boundary; a branch kept only in a worktree's detached HEAD, or in another clone, is not seen, so list `git worktree list` and the branches first, and say in the commit which branches were open.
3. Run `tools/runlog check`, and commit the log and the two history files alone, with the counts `compact` printed.
4. Try a merge with each open branch in a throwaway worktree (`git merge --no-commit --no-ff`), run `tools/runlog check` there, and abort it.

A campaign's own log is distilled the same way when its plan closes, with `--log <path>`; it then keeps only its kept rows.
