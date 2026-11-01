# 0118 The generators keep their `static constexpr` locals

- Status: accepted by the owner, 2026-11-01
- Date: 2026-11-01
- Decided by: the owner; recorded by Claude (agent, Sonnet 5.5)

## Context

The plan generators keep each step's result in a `static constexpr` local. On Clang this stops a plan from being generated twice inside one evaluation (decision 0102 item 7; the check is `core.plan_evaluated_once`, decision 0112). On GCC at -O0 the same locals add read-only symbols to debug object files (126 to 147 in the first measurement, plan 0011 section 12), and each new generator adds more.

## Options

1. Keep the locals and accept the larger debug objects.
2. Use plain locals, which gives smaller debug objects, makes Clang generate each plan twice, and makes `core.plan_evaluated_once` fail by design.

## Decision

Keep the locals (option 1). The extra symbols exist only in debug builds. A release build does not carry them, and the cost of generating plans twice would fall on every build.

## Consequences

- Plan generators written later use the same form, and `core.plan_evaluated_once` stays the check.
- The question listed as open in the handoff note and in the reports of WU7a is closed.
- The other open question, whether the per-algebra `metric_like` check is an accepted exception to P2 (decision 0107 item 4), stays open.
