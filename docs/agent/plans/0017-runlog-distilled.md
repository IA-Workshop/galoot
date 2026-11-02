# 0017 A distilled performance history, campaign logs, and a compacted run log

- Status: claimed 2026-11-01 (planner and implementer: Claude, agent, Opus 5.5, performance-engineer role with the build-expert role file). Not started.
- Unit: one unit on branch `feature/runlog-distilled`, worktree `.claude/worktrees/runlog-distilled`, branched from `develop` at the commit that claims this plan. It changes the recorder (`tools/timed`, `tools/measure`), adds a tool (`tools/runlog`) with its self-check, and rewrites the run log once.
- Decision: 0170 (proposed), which carries the amended wording of P7, P13, and DoD-25 for the owner to ratify. Governance is not edited here.
- Owner ruling (2026-11-01): "The run log should be preserved insofar as the key information is distilled to show how performance is tracking, but the raw entries are getting unwieldy." This is the owner's approval to move existing rows out of `runs.tsv`, within W7 (no archive copies; version control is the recovery path).
- Goal: `runs.tsv` keeps the build and test runs, including every gate run, and becomes small enough to work with; every other row up to a recorded boundary revision is distilled into a committed history that shows how each kind of run is tracking, and stays retrievable from git with one command; measurement campaigns write to their own logs from now on, so that the run log does not fill up again.

## 1. Departures, ambiguities, and owner questions (C20)

D1. **The gate rows stay in `runs.tsv`, in place, rather than being copied into the history file.** The brief puts "every gate-run row at a merge or milestone kept verbatim" in the distilled history. Keeping them where they are gives the same rows verbatim without a second copy (W7 keeps no archive copies), keeps DoD-25's promise that `runs.tsv` covers every gate run literally true, and the rows left in place are what keep union merges of other branches correct (section 4.4). The history counts them in its statistics and the README says where they are.

D2. **Which rows are gate rows is decided by a rule, and the rule keeps more than the merge gates.** No column marks a gate run, merge messages do not name the selection, and agents have named the same full-suite run `all`, `all-clean`, `final:all`, `all:final-f812191`, `p9`, `review-clean`, `fresh`, and more. Matching revisions to merge commits finds 34 workflow rows of the hundreds of gate runs. So the rule keeps every row of a build or test run of a committed revision (section 4.2): 1,666 rows at `2d90f41`, about 0.6 MB. That is a superset of the gate runs and is what P13 is about; the rows it drops are measurement probes, mutation runs (almost all at `-dirty` revisions), spikes, and runs of uncommitted trees.

D3. **"Regenerating the history from the retained rows" is read as regenerating it from the rows retained in version control.** The statistics of the removed rows cannot be recomputed from the rows left in `runs.tsv`. The check (`tools/runlog check`) reads each era's rows back with `git show <boundary>:<log>`, recomputes the history rows and the era record, and compares them byte for byte with the committed files; it also checks the rows left in the log. A reading in which the history is a function of the rows left in `runs.tsv` alone would need the removed rows to stay, which the ruling rules out.

D4. **A few non-gate rows also stay, as anchors for merges.** Union merges of a branch whose base is older than the boundary need certain rows of the boundary's log to stay unchanged (section 4.4). At `2d90f41` these are three rows: the last row at the boundary and, for `feature/core-mv-a`, the last row at its fork point `49cf28f` and the row after it. They are summarized like every other row and recorded in `history/eras.tsv`.

D5. **`compile.tsv` is not compacted.** It is 1.3 MB, 2,521 rows, written only by the copy step of decision 0080 for gate and measurement trees, and every row is a P1 measurement. Nothing in it is probe noise.

D6. **The run log has 28 columns, not 27.** The header is P13's nine, the 18 cost columns from `mode` to `pinned_other_s`, and `options`. Rows written before the recorder grew hold 9 or 26 cells (2,125 and 1,808 rows); the tool reads them with `--` in the missing cells, as `tools/measure read` does.

D7. **The compiler and library versions are not in the rows.** The history takes them from a committed table, `history/toolchain.tsv`, of the toolchain in force from a given time on each host, written from the package manager's log and the tools' `--version` output (section 4.1). Over the whole of the first era (2026-10-31T20:40:59Z to the boundary) one state was in force: GCC 16.2.1 20260810 with its libstdc++, Clang 23.1.4, CMake 4.4.5, Ninja 1.13.2, Python 3.14.8, perf 7.3.1, kernel 7.3.0-4-cachyos. Recording the versions in each row is deferred (section 13).

D8. **The existing campaign rows are distilled and removed, not moved into campaign logs.** Moving them would keep the 71 MB in the working tree under another name, which is neither what the ruling asks nor allowed as an archive copy (W7). They stay in git at the boundary revision.

D9. **The campaign log is chosen per run (`--log`) or per shell (`TIMED_LOG`), not by a rule on the step.** Routing by step (`measure` rows to a campaign file) would also divert P1 and P6 measurements that the history should keep, and is a default the caller cannot see (C8). The environment variable lets an existing campaign script such as `docs/agent/snippets/plan_cost_model/harness.py` write to a campaign log without being edited; the snippet itself is not edited (it is maintained at milestone reviews, decision 0021). Builds and tests still go to `runs.tsv` unless a log is named.

D10. **Not edited, though they mention the run log:** `.claude/agents/build-expert.md` ("logs its wall time to `runs.tsv`"), the harness docstring ("so that it has a row in runs.tsv"), and the governance texts of section 5. The first two stay true for builds and tests; the coordinator may update them after the owner rules on decision 0170.

Ambiguity A1. "Spread" is taken as the range, the largest value minus the least, which with the least value gives the largest. A robust spread (interquartile) is not given; with the low median and the range a reader sees the center and the extremes.

Ambiguity A2. The median is the low median (the middle value, or the lower of the two middle values for an even count), so that it is always a value that was measured and needs no rounding.

## 2. Definition of done

1. `tools/runlog` exists, with `summary`, `compact`, `check`, and `prune`, and a self-check registered in CTest as `tools.runlog` that kills every mutant of section 7.
2. `tools/timed` and `tools/measure` accept a log path (`--log`, or `TIMED_LOG`), with tests in `tools.measure_record`.
3. `docs/agent/records/performance/history/` holds `README.md`, `toolchain.tsv`, `eras.tsv`, and `groups.tsv`; `docs/agent/records/performance/README.md` describes the run log, the compile record, campaign logs, the history, compaction, and the retrieval command.
4. One compaction commit, separate from every code commit, at the boundary revision of section 4.3, and `tools/runlog check` passes at it and at the branch head.
5. Trial merges with `feature/core-mv-a` in both directions, uncommitted in a throwaway worktree, pass `tools/runlog check`; plus the merge order that broke the first anchor rule (section 4.4), in a scratch clone.
6. Decision 0170 records the proposed wording; this plan's outcome records the row counts, the commits, and the checks with revisions.

## 3. Acceptance criteria

1. `runs.tsv` after compaction holds the header, the rows the rule of section 4.2 keeps, the anchor rows, and every row written after the boundary, each byte-identical to its line before compaction and in the same order. Expected at the boundary (from `2d90f41`, which the claim does not change): 235,590 rows before; 1,666 kept by the rule and 3 anchors (none of which the rule keeps), so 1,669 rows from the boundary, plus the rows this branch appends after it.
2. `history/groups.tsv` has one row per era, step, preset, selection family, mode, and outcome, with the columns of section 4.1; `history/eras.tsv` one row for the era. `tools/runlog check` at the compaction commit and at the branch head exits 0, which includes the byte-for-byte regeneration.
3. The retrieval command of section 4.5 prints the removed rows: at least the 233,921 rows that are not kept.
4. `tools/runlog check` exits 1 and names the problem when a removed row is back in the log, when a kept row is missing or doubled, when a history row differs in any cell, and when an era row differs.
5. `tools/runlog compact` refuses, with a message and the files unchanged, when the era is already recorded, when the working log lacks a row of the boundary's log, or when the history's headers are not the tool's.
6. `tools/runlog prune` removes resurrected rows and nothing else.
7. A union merge, in the self-check's fixture repository, of a compacted branch with a branch based before the boundary (both directions, and with a third branch merged in between) passes `check`.
8. `tools/timed --log <file> ...` and `TIMED_LOG=<file> tools/timed ...` write the row to that file and not to `runs.tsv`, create it with the header if it is new, say where on stderr, and refuse a file whose header is another recorder's; without either, rows go to `runs.tsv` as before.
9. Every mutant of section 7 is killed by the self-check named for it.
10. The tool, its tests, and the recorder change cite no governance provision or agent work (S11); `ruff check` and `ruff format --check` pass on the new and changed Python, as for `tools/measure` (lint rows `ruff-measure`), if ruff is installed; otherwise said.
11. The cheap gates at each commit on a clean tree (markers, citations, golden, trace, include scan), the affected tests at each commit, the full suite once at the end, and all seven presets from new empty trees at the final revision, because `test/CMakeLists.txt` changes.

## 4. Design

### 4.1 The history

`docs/agent/records/performance/history/`:

- `eras.tsv`, one row per compaction: `era` (the boundary revision, 12 hex digits), `log` (the log's path in the checkout), `rows` (rows summarized), `kept` (rows left in the log), `anchors` (line numbers, in the boundary's log, of the anchor rows, or `-`), `first_utc`, `latest_utc`.
- `groups.tsv`, one row per era and group. The group is (`step`, `preset`, `family`, `mode`, `outcome`): `family` is the selection up to its first `:` (so the 210,848 `ceiling-cert:<case>:<cache>:<compiler>:<config>:L<limit>` rows form a handful of groups); `mode` is `pinned`, `shared`, or `--` for rows from before the column existed (P14: pinned and shared figures are never compared); `outcome` is `pass` (exit 0) or `fail`, because a failed build or a limit search's failing compile costs something different from a passing one. Columns: `era step preset family mode outcome rows dirty revisions first_utc latest_utc first_revision latest_revision hosts toolchains`, then for each of `wall_s`, `maxrss_kib`, and `inst_u`: `_n` (rows with a value), `_first`, `_latest` (by `utc_start`, ties by line order), `_min`, `_median` (low median), `_range` (largest minus least). `inst_u` is `inst_p_u + inst_e_u` when both are counts, or `inst_p_u` in a pinned row (the run could not use an efficiency core); otherwise the row has no instruction count. All arithmetic is decimal and exact, so the file is the same on every run.
- `toolchain.tsv`, one row per state of the toolchain: `since_utc host gcc libstdcxx clang cmake ninja python perf kernel source`. A group's `toolchains` lists the `since_utc` of each state in force on its hosts during its time span, so a change of compiler, library, or machine shows in the history (P7).
- `README.md`: what each file holds, the column meanings, the retrieval command, and the commands to compact, check, and prune.

### 4.2 The rows that stay in the run log

A row stays when its revision is a committed one (12 hex digits, no `-dirty`, not `unavailable`) and

- its step is `configure`, `build`, `test`, or `workflow` and its preset is named (not `-`); or
- its step is `gate`; or
- its step is `check` or `trace` and its family is one of the cheap gates: `trace`, `trace-gate`, `convention-trace`, `markers`, `golden`, `citations`.

At `2d90f41` this keeps 1,666 rows, 612,647 bytes.

### 4.3 The compaction and its boundary

The boundary is the commit on `develop` that claims this plan, written `B`. The worktree branches from it. Before the compaction commit, the rows this branch has appended are committed on their own, so that the compaction commit only removes rows and adds the history.

`tools/runlog compact B`, from the worktree root:

1. Reads the log at `B` (`git show B:<log>`), and the rows earlier eras of the same log kept; the era is every row of the log at `B` not kept by an earlier era (none for the first). Refuses if `B` is already an era, if the history's headers are not the tool's, or if the working log does not hold every row of the era.
2. Finds the anchors: the last row of the log at `B`, and for each local branch not merged into `B`, the last row of the log at its merge base with `B` and the row after it in `B`'s log. Refuses if an anchor row is not in `B`'s log.
3. Appends the era's groups to `groups.tsv` and its row to `eras.tsv`, and rewrites the log in one replace: every line is kept, in order and byte for byte, except the era's rows that are neither kept by the rule nor anchors.

Then the agent runs `tools/runlog check`, reads the counts it prints, and commits the log and the history alone, with the counts in the message.

### 4.4 Why union merges stay correct

`runs.tsv` is merged with git's `union` driver: where both sides changed the same place, it keeps both sides' lines. A branch appends at the end of its copy of the log. Two cases matter.

- A branch whose base is the boundary or later (this branch's own merge with `develop`, and any branch started afterwards). The compacted side deleted rows of the base and the other side appended after the base's last row. If the deletion reached the base's last line, the two changes would touch, git would see one conflict, and the union would keep the other side's copy of the deleted rows. Keeping the boundary's last row separates them.
- A branch based before the boundary, here `feature/core-mv-a` at `49cf28f`. Its own rows are appended after its base's last row; the log at its base is an ordered subsequence of the log at `B` (checked: no row missing, order kept), so that row is somewhere inside `B`'s log. Keeping it separates the compacted side's deletions before it from the branch's rows after it. If the branch has merged `develop` first and that merge has reached `develop`, the branch's rows sit right after that row in `develop`'s log, inserted before the row that followed it; keeping that following row too keeps the deletion after the insertion point from touching it. Without it, the 52 rows of `B` between the two would come back.

A branch whose base is not covered (a worktree's uncommitted rows on an old detached revision, committed later on a branch made from it) can bring removed rows back. `tools/runlog check` names them and `tools/runlog prune` removes them; the README says to run the check after every merge that touches the log. Rows copied into `develop` by appending (as done for review worktrees) are not affected.

### 4.5 Retrieval

The raw rows of an era, as they were at its boundary:

```bash
git show <era>:docs/agent/records/performance/runs.tsv
```

`<era>` is the first column of `history/eras.tsv`. For this era it is `B`. The README carries the command, and a filter for one family (`| awk -F'\t' 'NR == 1 || $6 ~ /^ceiling-cert:/'`).

### 4.6 Campaign logs

`tools/timed [--log <file>] <step> <preset> <selection> -- <command...>`; without `--log`, the environment variable `TIMED_LOG`, if set and not empty, names the file. A relative path is taken from the current directory. The row is written by the same recorder, with the same columns, and the stderr line says where it went when that is not `runs.tsv`. A campaign's log is `docs/agent/records/performance/<campaign>/runs.tsv`: under the directory that the recorder leaves out of its check for uncommitted changes, covered by the union rule of `.gitattributes`, and committed with the campaign's records. When the campaign's plan closes, its log is distilled with `tools/runlog compact <revision> --log <path>` and the file is deleted, under the same owner approval as any compaction (decision 0170). `tools/measure run` takes the same `--log`.

### 4.7 What is kept and what is lost

Kept: every row of a build or test run of a committed revision, verbatim; the anchor rows; every row after the boundary; for every group of removed and kept rows, the count, the counts of failed and dirty rows, the distinct revisions, the first and latest time and revision, the hosts and toolchain states, and first, latest, least, low median, and range of wall time, peak memory, and retired instructions; all raw rows in git at `B`.

Lost from the working tree (still in git): each removed row's own values; its selection beyond the family (the case and limit of a calibration compile, for example; the campaigns' results are in their own records directories); user and system time, cycles, cache events, energy, other processes' CPU time, the sibling's and the pinned CPU's busy time, the processors, and the options of the removed rows, none of which is aggregated.

## 5. Proposed governance wording (decision 0170, for the owner to ratify)

Nothing below takes effect until ratified. Until then the texts stand as they are; this unit's compaction is done under the owner's ruling of 2026-11-01.

P7 (new second sentence; the rest unchanged):

> The measurements of P1, P2, and P6 are kept in `docs/agent/records/performance/`, each with its revision, toolchain, machine, options, and workload. The history also holds the run log of P13 in distilled form: for each kind of run (its step, preset, selection family, placement, and outcome), the number of runs, the revisions and times they span, the toolchain in force, and the first, latest, least, median, and range of their wall time, peak memory, and retired instructions. Once the history shows how a quantity varies, a change that can affect the quantity is compared with the history by a criterion stated before the comparison, and a change beyond the variation is reported with its cause. A change of compiler, library, or machine is marked in the history so that it is not read as a change of the code.

P13 (first sentence replaced, and a sentence added after the energy sentence):

> Each build and test run is timed by `tools/timed`, which appends its wall time, revision, branch, preset, selection, and exit status to the run log, `docs/agent/records/performance/runs.tsv`. The runs of a measurement campaign go to the campaign's own log instead, named when the run starts and committed with the campaign's records. [Where the machine exposes them ... (unchanged) ... and says so (decision 0018).] When a log grows too large to work with, the owner may approve its compaction at a recorded boundary revision: its rows up to the boundary are distilled into the performance history (P7) and removed from the log, except the rows of build and test runs of committed revisions, which include every gate run and stay in the log unchanged, and the few rows that later merges of other branches need. The removed rows stay in version control, and the history names the command that retrieves them. [These times are tracked and noted ... (unchanged).]

DoD-25 (plan 0001):

> DoD-25 (P7, P13). The performance history baseline is committed; `runs.tsv` keeps a row for every gate run through every compaction of the log, and the history distills the rest (P7).

## 6. Tests and expected values

`test/tools/runlog_test.py <tools/runlog> <scratch directory>`, registered as `tools.runlog` (labels `tools;tools.runlog`). Each case builds what it needs in a fresh git repository under the scratch directory, with a `.gitattributes` holding the union rule, and runs the tool there; the real log is never read. The fixture logs are written by the test, so each expected value is stated next to its rows.

1. `summary` of a fixture of 12 rows against an expected text written in the test: a group of four passing rows with wall times 3.000, 1.000, 4.000, 2.000 (first 3.000, latest 2.000 by time; least 1.000; low median 2.000; range 3.000); two rows with the same `utc_start` (first and latest follow line order); a pinned row with `inst_p_u` 100 and `inst_e_u` `unavailable` (instructions 100) beside a shared row with 100 and 50 (150) and a shared row with 100 and `unavailable` (none); a 9-cell row (`--` mode, no memory, no instructions, `_n` 0 and `unavailable` cells); a failing row in its own group; a `-dirty` row counted in `dirty` and with `revisions` counting it with its clean twin as one; two hosts; a row before the first toolchain state (`toolchains` lists none: `unrecorded`).
2. The rule of section 4.2, row by row on a fixture of 14 rows: kept are `build gcc-debug all` clean, `test clang-asan ^core$` clean, `workflow lint all-clean` clean, `configure gcc-debug fresh` clean, `gate - trace` clean, `check - golden:x` clean; not kept are the same `build` at `-dirty`, at `unavailable`, `build - spike-a` (no preset), `measure - ceiling-cert:...`, `probe - p`, `check - metric-like-holes`, `spike - api-design:run`, and a `gate` at `-dirty`.
3. `compact`: a repository with commits A (5 rows) and B (A plus 6 rows), a branch `old` at A with 2 rows of its own, and 3 rows appended in the working log after B. Expected log: header, the kept rows of B's log, A's last row and the row after it, B's last row, and the 3 later rows, in their original order; `eras.tsv` and `groups.tsv` rows as written in the test; the stdout line with the counts.
4. `check` after the compaction is committed: exit 0. Then, each on a copy: a removed row appended (exit 1, the message names its line); a kept row deleted (exit 1); a kept row doubled (exit 1); one cell of a `groups.tsv` row changed (exit 1, naming the era and the row); `eras.tsv`'s `rows` changed (exit 1).
5. Refusals, each leaving all three files byte-identical: `compact B` a second time; a working log missing one row of B's log; a `groups.tsv` with a changed header.
6. `prune` on a log with two removed rows appended: both removed, nothing else changed, then `check` exits 0.
7. Union merges in the fixture repository, each followed by `check`: `old` merged into the compacted branch; the compacted branch merged into `old`; and `old` first merged into a third branch from B, that merged into the compacted branch's parent line, then the compacted branch merged with it (the order of section 4.4's second case). Every merge completes without conflict markers, every row of `old` is present once, and `check` exits 0. A variant with the anchors removed (the tool's `--no-anchors` is not offered; the test edits the compacted log to drop them before committing) shows the resurrection: `check` exits 1. This last case shows that the anchors are needed, not only sufficient.
8. A second era: more rows, a new boundary C, `compact C`: the first era's kept rows are not summarized again (the second era's `rows` equals the rows added between B and C), and `check` passes for both eras.

`test/tools/measure_record_test.py` gains:

9. `measure run --log <dir>/c.tsv other - sel -- true`: `c.tsv` has a header and one row with selection `sel`; `runs.tsv` is unchanged.
10. `timed --log c.tsv` (relative, from a subdirectory) writes `<subdirectory>/c.tsv`; `TIMED_LOG=<file> timed ...` writes there; `--log` beats `TIMED_LOG`; an empty `TIMED_LOG` means `runs.tsv`; stderr names the file.
11. A `--log` file with a 9-column header: the row is not written, the warning names the file, the command's exit status passes through.
12. `timed --log` with no file after it: usage, exit 2, nothing run.

## 7. Mutations

Each mutant is a one-line edit of a copy of the tool, applied by a driver in `docs/agent/records/mutations/0017-runlog-distilled/` that checks its anchor text occurs once, runs the named test against the copy, and records killed or survived.

| Mutant | Change | Expected killer |
| --- | --- | --- |
| `median_high` | the upper of the two middle values | case 1 |
| `range_from_first` | range as largest minus first | case 1 |
| `ties_reversed` | ties in time ordered last line first | case 1 |
| `pinned_inst_dropped` | a pinned row without `inst_e_u` has no instruction count | case 1 |
| `outcome_not_in_key` | passing and failing rows in one group | case 1 |
| `dirty_kept` | the rule ignores `-dirty` | case 2 |
| `unnamed_preset_kept` | the rule accepts preset `-` | case 2 |
| `no_fence_row` | the row after a branch base's last row is not an anchor | case 7 |
| `no_boundary_anchor` | the boundary's last row is not an anchor | case 7 |
| `no_containment_check` | `compact` does not check that the working log holds the era | case 5 |
| `no_resurrection_check` | `check` skips removed rows found in the log | case 4 |
| `groups_not_compared` | `check` does not compare `groups.tsv` | case 4 |
| `earlier_era_resummarized` | earlier eras' kept rows are not subtracted | case 8 |
| `log_ignored` (measure) | `--log` parsed but the row goes to `runs.tsv` | case 9 |
| `env_ignored` (timed) | `TIMED_LOG` not read | case 10 |

## 8. Commits

1. `tools/runlog`, `test/tools/runlog_test.py`, and the CTest registration.
2. `--log` and `TIMED_LOG` in `tools/timed` and `tools/measure`, with the new cases of `tools.measure_record`.
3. `docs/agent/records/performance/README.md`, `history/README.md`, `history/toolchain.tsv`, and the mutation driver.
4. The rows this branch appended so far (rows only).
5. The compaction: `runs.tsv`, `history/eras.tsv`, `history/groups.tsv`, nothing else.
6. The records: mutation results, this plan's outcome, the agent cost, and the rows appended since commit 5.

## 9. Evidence and verification

Every build and test through `tools/timed`; their rows go to `runs.tsv` (this unit names no campaign log for them). At each commit: the affected tests (`tools.runlog`, `tools.measure_record`, `tools.measure_header` on gcc-debug), the cheap gates on a clean tree, `ruff` where installed. At the end: the full suite on gcc-debug and clang-debug, gcc-asan for the sanitizers, and the lint preset; and the seven presets by `cmake --workflow --preset <p>` from new empty build directories at the final revision, because `test/CMakeLists.txt` is a build input. The trial merges of section 2 item 5, with their commands and results, in the outcome. Every claim names its revision.

## 10. Review

A W5 blind reviewer, not this agent, gets the branch, sections 2, 3, and 6 of this plan, and the departures of section 1 and of the outcome, and checks among other things: that the compaction commit only removes lines (`git diff --numstat` shows 0 added lines for `runs.tsv`), that `tools/runlog check` passes at the head, that the retrieval command reproduces the era, and a trial merge with `feature/core-mv-a` at its head of that time. The coordinator merges after the review.

## 11. Cost estimate (for information, decision 0019)

About 450,000 tokens and 250 tool calls for planning and implementation, and 2 to 3 hours of wall time, most of it the seven fresh workflows and the full-suite runs. Machine time: one `git show` of the 76 MB log per `check` (a few seconds), the self-check under 30 s.

## 12. Risks

- A branch based before the boundary that is not a local branch at compaction time brings back removed rows when merged. Detected by `check`, repaired by `prune`.
- The merge of a 76 MB file against a 0.6 MB one is a large diff for git; the trial merges measure its time.
- If `feature/core-mv-a` merges `develop` and reaches `develop` before this branch, the merge base of this branch changes nothing (its anchors are rows of `B`), but the trial merge of section 2 item 5 is then repeated against the new `develop` before the merge.
- The rule of section 4.2 keeps the build and test rows of committed revisions, but a gate run at a `-dirty` revision (none should exist: gates run on committed trees) would be summarized only.

## 13. Deferred

- Recording the compiler and library versions in each row (a new column; every branch's recorder must change at once, because the header check refuses a mismatched header).
- Aggregating CPU time, cycles, cache events, and energy in the history.
- Compacting `compile.tsv` (not needed at 1.3 MB).
- Updating `.claude/agents/build-expert.md` and the harness docstring once the owner rules on decision 0170.
