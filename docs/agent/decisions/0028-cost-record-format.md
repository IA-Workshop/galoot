# 0028 Format of the cost records: compile.tsv and runs.tsv

- Status: accepted
- Date: 2026-10-31
- Recorded by: Claude (agent, build-expert, WU1), under C18; a decision record takes effect when recorded, and the owner may reverse it.

## Context

P1 asks for per-translation-unit wall time and peak memory, P13 for a row per build and test run, and decision 0018 for counters, energy, and the processor in every row, with `unavailable` for what the machine does not expose. Decision 0020 extends `runs.tsv` in place and commits `compile.tsv` rows only for gate and measurement runs. Plan 0002, section 4.7, proposed the format.

## Options

1. One file with both kinds of row: per-TU rows need source, object, and compiler columns that run rows lack.
2. Two files with shared conventions, written by one recorder.

## Decision

Option 2, implemented once in `tools/measure` (`tu` mode is the compiler launcher; `run` mode backs `tools/timed`).

- `compile.tsv`, 27 columns: `utc_start revision branch preset compiler source object exit host mode cpus wall_s user_s sys_s maxrss_kib inst_p_u inst_e_u cycles_u l1d_loads_u l1d_misses_u l2_refs_u l2_misses_u llc_refs_u llc_misses_u energy_pkg_j other_cpu_s sibling_busy_s`.
- `runs.tsv`, 26 columns: P13's nine (`utc_start revision branch step preset selection wall_s exit host`) followed by the same fields from `mode` to `sibling_busy_s` except `wall_s`, which P13's nine already hold. Rows written before WU1 hold `--` in the new cells (decision 0020, item 3).
- `mode` is `pinned` when the process may run only on one performance-core CPU (read from `os.sched_getaffinity` and `/sys/bus/event_source/devices/cpu_core/cpus`), else `shared`. `cpus` is the affinity as a kernel CPU list.
- Pinned rows count eight `cpu_core` user-space events (instructions, cycles, L1d loads and misses, `l2_rqsts.references` and `l2_rqsts.miss`, `cache-references` and `cache-misses`), and `sibling_busy_s` is the busy time of the core's other thread. A counter perf did not run for its whole enabled time was multiplexed and is written `unavailable`, never as a scaled estimate. Shared rows count instructions per core type (`inst_p_u`, `inst_e_u`, with `--no-scale`), and cycles and cache fields are `unavailable`.
- A pinned `build` step takes no counters in its run row, because the compile rows of its launcher take them on the same core: two perf sessions of eight events each multiplexed every counter (lessons-learned 0001). Its run row's counter fields are `unavailable`.
- `energy_pkg_j` is the package RAPL counter's difference, corrected for wraparound. `other_cpu_s` is the machine's busy CPU time during the run minus the run's own. No idle threshold is set (decision 0022, item 5).
- `revision` is marked `-dirty` when a tracked file other than those under `docs/agent/records/performance/` has uncommitted changes.
- Where compile rows go is the cache variable `GALOOT_COMPILE_RECORD`: `build/<preset>/compile.tsv` for the correctness presets (routine rows, not committed) and `docs/agent/records/performance/compile.tsv` for the measurement presets. A gate run in a correctness preset configures a separate tree with `-DGALOOT_COMPILE_RECORD=<committed file>`, so the choice is recorded in that tree's configuration.

## Consequences

Only pinned rows made on a quiet machine are compared (P7, P14). The launcher adds about 26 ms per compile (plan 0002, evidence). The optimization level is named by the preset; a level overridden at configure time is not visible in the rows (open question in plan 0002's outcome).
