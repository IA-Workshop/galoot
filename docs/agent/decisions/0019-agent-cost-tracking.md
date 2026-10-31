# 0019 Agent and token costs are tracked for information only

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner, 2026-10-31 ("track these agent and token costs for information only, not for assertion"). Recorded by Claude (agent).

## Context

W3 asks for an agent-cost estimate in each plan and W10 for a record of agent cost. Decision 0013 makes build and run cost track-only. Plan 0001 (OD-20, section 7) left the unit of agent cost open.

## Decision

Agent cost is recorded and never asserted or used as a bound. Each delegated agent's tokens, tool calls, and wall time are appended to `docs/agent/records/agent-cost.tsv` as the harness reports them, with the model; a figure the harness does not report is written as unavailable, never as zero. Estimates in plans stay estimates and are compared with the actuals in the plan's closing outcome. The main session's own cost is recorded when the harness exposes it.

## Consequences

No gate, review, or merge depends on a cost figure. The owner may set a bound later by a decision.
