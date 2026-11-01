# 0042 Recorder fields added by plan 0003

- Status: accepted; item 5 is superseded by decision 0043
- Date: 2026-10-31
- Recorded by: Claude (agent, build-expert, plan 0003), under C18; a decision record takes effect when recorded, and the owner may reverse it. Carries out decision 0032, items 1 to 3, and adds to the format of decision 0028.

## Context

Decision 0032 asked for an `options` column in the compile and run records, for the busy time of other processes on a pinned CPU, and for old rows to be read as `--` without padding. The W5 review of WU1 (finding N16) also found that shared rows wrote perf's `<not counted>` as `unavailable`, which loses the per-core-type instruction sum for a command that ran on only one core type. Several details were left open; this record fixes them.

## Decision

1. Columns. Both records end with `sibling_busy_s pinned_other_s options`: `compile.tsv` has 29 columns and `runs.tsv` 28. The headers of the committed records were extended once, in place, and no row was rewritten. A recorder whose columns differ from a record's header refuses to append to it, with a warning, rather than writing rows the header does not describe.
2. `options` in a compile row: the compiler's arguments other than the compiler itself, the source (`-c`), the object (`-o`), and the dependency-file arguments (`-MD`, `-MMD`, `-MT`, `-MF`, `-MQ`), joined as a shell would quote them, with the checkout's path prefix removed so that rows from different worktrees compare. Defines and include paths stay, because they change what is compiled.
3. `options` in a run row: `CMAKE_CXX_FLAGS` followed by `CMAKE_CXX_FLAGS_<BUILD_TYPE>` from `CMakeCache.txt` of the run's build tree, read after the command so that a configure step records what it set. The tree is the directory after `-C`, `-B`, `--build`, or `--test-dir` in the command, else `build/<preset>`, which is every preset's `binaryDir`. A run with preset `-` and none of those arguments has no tree and records `-`; a tree without a readable cache records `unavailable`. Per-target options (definitions, include paths, sanitizer flags set on a target) are in the compile rows, not here.
4. `pinned_other_s`: in a pinned row, the pinned CPU's busy time from `/proc/stat` during the run, less the run's own user and system time. Every process of the run inherits the one-CPU affinity, so the remainder is other processes'. The figure has the clock tick's resolution (10 ms) and can come out slightly negative near zero; it is recorded as computed, not clamped. Shared rows record `unavailable`.
5. `<not counted>`: in a shared row, an instruction event that perf reports as `<not counted>` with zero running time is recorded as `0`. Each core type's instruction event is alone on its PMU's fixed counter, so it is never multiplexed, and zero running time then means the command never ran on that core type: it retired no instructions there. This is inferred from perf's semantics, not proven; it was checked by `taskset -c 2` (performance core: the `cpu_atom` event `<not counted>`, run time 0) and `taskset -c 20` (efficiency core: the `cpu_core` event `<not counted>`), with perf 7.3.1. Any other non-numeric value, such as `<not supported>`, and any `<not counted>` in a pinned row, stays `unavailable` (decision 0018).
6. Reading: `tools/measure read <record> [<column>...]` reads a record by column name and writes `--` in each cell a row lacks (decision 0032, item 3). It exits 1 on a row with more cells than the header, on a repeated header line (the sign of a doubled union merge, lessons-learned 0001 item 4), and on an unknown column.

## Consequences

An `-O` level set at configure time is visible in both records without its own preset (decision 0020, item 5). Readers that split rows by position, such as `awk -F'\t'` with column numbers, see three row lengths (9, 26 or 27, and 28 or 29); reading by name through `tools/measure read` avoids that. Item 5 changes some shared rows' `inst_e_u` or `inst_p_u` from `unavailable` to `0`; rows written before this record keep `unavailable`.
