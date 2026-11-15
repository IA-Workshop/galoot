# Performance records

The performance history of P7 and the run log of P13.

| Path | Holds |
| --- | --- |
| `runs.tsv` | The run log: one row per build, test, or other command run through `tools/timed` (P13), appended from every worktree and merged with git's union driver (`.gitattributes`). Columns: decision 0028 and `tools/measure`'s docstring. After a compaction it holds the workflow runs of named presets, the build, test, style, and gate runs of committed revisions, a few anchor rows, and every row written since the last boundary (`history/README.md` gives the rule). A gate run recorded outside the rule is in the history's counts and in git. |
| `compile.tsv` | One row per translation unit of a gate or measurement build (P1), copied from a fresh build tree by the step of decision 0080. Not compacted. |
| `history/` | The distilled run log: one row per compaction and per kind of run in each, and the toolchain in force. See `history/README.md`. |
| `<campaign>/` | The scripts, outputs, and notes of one measurement campaign. A campaign's own run log, when it has one, is `<campaign>/runs.tsv`. |

## Campaign logs

A measurement campaign that runs many commands names its own log, so that the run log keeps the build and test runs:

```bash
TIMED_LOG="$PWD/docs/agent/records/performance/<campaign>/runs.tsv" python3 -I <campaign script>   # every run of the script
tools/timed --log docs/agent/records/performance/<campaign>/runs.tsv measure - <selection> -- <command>   # one run
```

`TIMED_LOG` must be an absolute path (`tools/timed` refuses a relative one, since each run would read it from its own directory); `--log` is taken from the current directory. The log has the run log's columns and gets a header when it is new; files under this directory do not mark a revision `-dirty`. Commit it with the campaign's records; when the campaign's plan closes, distill it with `tools/runlog compact <revision> --log <path>` (owner approval, decision 0170).

## Raw rows removed by a compaction

```bash
git show <era>:docs/agent/records/performance/runs.tsv
```

`<era>` is the first column of `history/eras.tsv`. The first era is `ebdd06d09289`, the claim of plan 0017.
