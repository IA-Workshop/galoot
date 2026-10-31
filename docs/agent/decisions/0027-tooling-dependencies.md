# 0027 Tooling dependencies: Python 3, perf, and taskset

- Status: accepted
- Date: 2026-10-31
- Recorded by: Claude (agent, build-expert, WU1), under C18; a decision record takes effect when recorded, and the owner may reverse it.

## Context

WU1's tools need a scripting language and access to performance counters: `tools/measure` (the cost recorder), `tools/include_scan.py`, and the pinned runs of P14. C13 makes each tooling dependency a decision; decision 0006 limits the toolchain to platform-supplied software.

## Options

1. Python 3 with its standard library only, `perf`, and `taskset` (util-linux), all platform packages.
2. Shell and awk only: `wait4` peak memory and `perf` output parsing are awkward and fragile there.
3. A compiled C++ tool: more code to build and maintain for no gain.

## Decision

Option 1: Python 3 (3.14.8 here, `python 3.14.8-1`), standard library only, run with `-I`; `perf` (`perf 7.3.1-1`); `taskset` and `flock` from util-linux 2.42.4. No package outside the platform. If `perf` is missing or cannot open its events, `tools/measure` still runs the command and records the counters as `unavailable` (decision 0018).

## Consequences

The core does not depend on any of them (C13). A kernel or perf change that alters event names or `perf_event_paranoid` turns counter fields into `unavailable`, which the records show (plan 0002, risk 5).
