# 0016 Single repository, core only

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner, accepting plan 0001 section 7 (D16). Recorded by Claude (agent).
- Owner amendment: agree, this is core only.

## Context

The design study proposes fourteen repositories.

## Decision

One repository with module directories under include/galoot/. This plan and its repository cover the core only; extensions are planned separately. The split waits for a second repository with a present user.

## Consequences

The rga3 model ships in this repository at 0.x as the C5 and V8 evidence.
