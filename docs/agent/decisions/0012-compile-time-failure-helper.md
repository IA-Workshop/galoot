# 0012 Compile-time failure without exceptions

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner, accepting plan 0001 section 7 (D12). Recorded by Claude (agent).

## Context

S9 says the core throws no exceptions, yet consteval code needs a way to fail evaluation.

## Decision

The core uses a helper that calls a non-constexpr function to fail constant evaluation, so no throw appears. One sentence is added to STYLE when it is next edited.

## Consequences

STYLE is a governance document; the sentence takes effect when the owner ratifies the edit.
