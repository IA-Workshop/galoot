# 0015 Plan term lists stay in a detail namespace

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner, accepting plan 0001 section 7 (D15). Recorded by Claude (agent).

## Context

Robust predicates may later want the term lists, but that user is anticipated, not present (C12).

## Decision

Term lists live in detail in v1. Tests may use detail. Promotion to the public interface is a later decision.

## Consequences

No public stability promise on term lists.
