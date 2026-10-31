# 0014 Owned benchmark harness

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner, accepting plan 0001 section 7 (D14). Recorded by Claude (agent).

## Context

P6 benchmarks need a harness. Google Benchmark would be a dependency (C13).

## Decision

An owned harness of about 100 lines under test/bench/, with reference kernels derived from the mathematics. No dependency.

## Consequences

The harness is maintained as test tooling; it has a present user in WU17.
