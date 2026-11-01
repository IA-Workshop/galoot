# 0080 Every build tree keeps its own compile record; a gate step copies rows into the committed one

- Status: accepted
- Date: 2026-10-31
- Decided by: Claude (agent, Opus 5.5), in cleanup unit 1 (plan 0007, item 2), on the design the plan recommends. The owner may reverse it (C18). Supersedes the last bullet of decision 0028 (where compile rows go) and item 4 of decision 0020 in so far as it put the measurement presets' rows in the committed file directly.

## Context

`CMakePresets.json` gave the four measurement presets `GALOOT_COMPILE_RECORD = ${sourceDir}/docs/agent/records/performance/compile.tsv`, so a build file named a path under `docs/agent`. STYLE S11 keeps agent paths and records out of build files, and decision 0072 (review finding B1 of the M0 scripts review) already removed the one other such path, because clearing or moving agent records must never change what a build does for a human contributor. The same review's delta (outside the delta, `CMakePresets.json:63`) and plan 0007 item 2 ask for a design.

A second defect sat beside it. The `measured` preset set `GALOOT_COMPILE_RECORD = ${sourceDir}/build/${presetName}/compile.tsv`. Presets have no macro for the binary directory, so a tree configured with `cmake --preset gcc-debug -B build/gcc-debug-pinned`, the form plan 0003 used for pinned gate compiles, wrote its rows into `build/gcc-debug/compile.tsv`, mixed with that tree's routine rows, unless the command line overrode the record.

## Options weighed

1. Keep the committed path in the measurement presets. Rejected: it is the citation S11 forbids, and a human building a measurement preset appends to an agent record.
2. Keep explicit paths in the presets, under `build/`, and have gate runs pass `-DGALOOT_COMPILE_RECORD=<tree>/compile.tsv`. No agent path, but the `-B` trap stays: forgetting the override mixes gate rows into another tree's record without any warning (C8).
3. The build tree's own `compile.tsv` by default, set in `CMakeLists.txt` from `CMAKE_BINARY_DIR`, with no record path in any preset; a documented gate step copies the rows of a gate or measurement tree into the committed record. Chosen.

## Decision

- `CMakeLists.txt` declares `GALOOT_COMPILE_RECORD` as a `FILEPATH` cache entry whose default is `${CMAKE_BINARY_DIR}/compile.tsv`. `CMakePresets.json` sets only `GALOOT_PRESET` (every preset but `lint`) and names no record. Configuring with `GALOOT_PRESET` set and the record explicitly empty still fails.
- The check that refused a record without `GALOOT_PRESET` (plan 0003, N7) is removed: the record now always has a value, and without `GALOOT_PRESET` no launcher is set, so the value is unused and harmless.
- Rows reach `docs/agent/records/performance/compile.tsv` only by this step, run by an agent after a gate or measurement build. `<tree>` is the absolute path of the build tree. Before the run, list and remove the tree's old record so that it holds only the run's rows (`ls -l -- "<tree>/compile.tsv" && rm -- "<tree>/compile.tsv"`, if it exists). After the run:

```bash
tree=<absolute path of the build tree>
record=docs/agent/records/performance/compile.tsv   # from the repository root
tools/measure read "$tree/compile.tsv" > /dev/null    # refuses a malformed record
cmp <(head -n 1 "$tree/compile.tsv") <(head -n 1 "$record")   # same columns
tail -n +2 "$tree/compile.tsv" >> "$record"
tools/measure read "$record" > /dev/null
rm -- "$tree/compile.tsv"   # so the same rows cannot be copied twice
```

  Then commit the record with the run's revision in the message. A `cmp` failure means the recorder's columns changed since the committed header was written; the header is extended in place, as decision 0042 item 1 describes, before copying.

## Evidence

At the commit that makes this change: `cmake --workflow --preset gcc-measure` passed 58 of 58, its cache holds `GALOOT_COMPILE_RECORD=<worktree>/build/gcc-measure/compile.tsv`, that file has a header and 20 rows, and the committed record's MD5 (`f9cd3ca5...`) is the same before and after. `cmake --preset gcc-debug -B build/gcc-debug-other` records into `build/gcc-debug-other/compile.tsv`. The copy step above, run on a scratch copy of the committed record with the gcc-measure tree's rows, added 20 rows and both reads exited 0. `grep -n 'docs/agent' CMakePresets.json CMakeLists.txt cmake test/CMakeLists.txt` finds nothing.

## Consequences

- A measurement build no longer records anything committed until an agent copies it, so a forgotten copy loses the rows from the history (they stay in the tree until removed). The gate report says whether the step ran.
- An existing build tree configured before this change keeps the old cache value, which for a measurement tree is the committed path. Such a tree is reconfigured with `cmake --preset <name> --fresh`, or its `GALOOT_COMPILE_RECORD` entry is removed from `CMakeCache.txt`.
- `tools/measure` still leaves `docs/agent/records/performance/` out of its check for uncommitted changes, so copying rows does not mark a later run's revision dirty.

## Amendment, 2026-10-31 (W5 review of `feature/cleanup-1`, finding N5)

The copy step above does not stop when a check fails, so a failed `tools/measure read` or `cmp` still appended rows, and it did not say which trees may be copied. It is replaced by the step below. The review's other point, that a stale cache or a stray `-D` override writes into another file without notice, is handled in the build: `CMakeLists.txt` warns at every configure when `GALOOT_COMPILE_RECORD` is not under `CMAKE_BINARY_DIR`, naming the file. Shown: configuring `build/gcc-debug-other` with `-DGALOOT_COMPILE_RECORD=<scratchpad>/outside.tsv` prints the warning; with `-UGALOOT_COMPILE_RECORD` the entry returns to the tree's own `compile.tsv` and no warning appears.

A gate or measurement tree whose rows are copied is a clean build: configured fresh (`cmake --preset <name> -B <tree> --fresh`, or a new directory), with no `compile.tsv` before the build, and built once at a committed revision with no uncommitted changes, so that every row comes from one build of one revision. A tree that was rebuilt, or whose rows show more than one revision or a `-dirty` one, is not copied.

```bash
set -eu
tree=<absolute path of the build tree>
record=docs/agent/records/performance/compile.tsv   # from the repository root
tools/measure read "$tree/compile.tsv" > /dev/null
cmp <(head -n 1 "$tree/compile.tsv") <(head -n 1 "$record")
test "$(tools/measure read "$tree/compile.tsv" revision | tail -n +2 | sort -u | wc -l)" = 1
tools/measure read "$tree/compile.tsv" revision | tail -n +2 | grep -qv -- '-dirty$'
tail -n +2 "$tree/compile.tsv" >> "$record"
tools/measure read "$record" > /dev/null
rm -- "$tree/compile.tsv"
```

Run it in a subshell (`bash -c '...'` or a script), so that `set -e` ends only the step. The first failing command stops it before any row is appended.
