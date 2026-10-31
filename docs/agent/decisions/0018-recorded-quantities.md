# 0018 Quantities recorded for every measured run

- Status: accepted; the P13 sentence it adds awaits the owner's ratification
- Date: 2026-10-31
- Decided by: the owner, who recommended tracking these quantities on 2026-10-31. Recorded by Claude (agent).

## Context

Decision 0013 makes cost track-only. The owner asked that wall time, performance counters, energy counters, RAM, and cache behavior all be tracked. On the development machine (Intel Core i9-14900K) `perf stat` works for user-space events with `perf_event_paranoid` = 2, and `/sys/class/powercap/intel-rapl:0/energy_uj` is readable. The efficiency cores do not support some cache events, and the energy counter covers the whole package, not one process.

## Decision

Every recorded run (P1, P6, P13) captures, where the machine exposes it: wall time; peak memory; retired instructions, cycles, and cache references and misses at each level, in user space; the package energy counter before and after; and the processor it ran on. Cache and counter events are taken on a performance core (P14). Energy is attributed to a run only when the machine is otherwise idle, and the record says whether it was. A quantity that is not available is recorded as unavailable, never as zero. No bound is set on any of them (0013).

## Consequences

`tools/timed` and the per-translation-unit launcher of WU1 grow these fields. Reading RAPL may need an owner action if its permissions change. The P13 sentence is a governance edit and takes effect when the owner ratifies it.
