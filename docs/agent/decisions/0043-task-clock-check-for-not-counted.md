# 0043 A missing per-core-type count is zero only when task-clock confirms it

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner ("proceed as recommended"), accepting the recommendation of the W5 review of `feature/recorder` (`docs/agent/records/reviews/2026-10-31-recorder.md`, finding N1). Recorded by Claude (agent, build-expert, plan 0003). Supersedes decision 0042, item 5; the rest of 0042 stands.

## Context

Decision 0042, item 5, wrote perf's `<not counted>` with zero running time as `0` in shared rows, on the inference that each core type's lone instruction event cannot be multiplexed, so zero running time means the command never ran on that type. The review found the inference likely on this machine but unchecked per row: a `<not counted>` event could also be one that never got a counter while the command did run on that type (a false zero), which needs another, privileged perf session to happen. Decision 0018 says an unavailable quantity is never written as zero.

## Options

1. Keep item 5: write 0 on the inference alone.
2. Revert to `unavailable` for every `<not counted>`: safe, but loses the per-core-type sum for every command that ran on one core type only.
3. Cross-check each row: add the software event `task-clock` to the shared events, and write 0 for a `<not counted>` core type only when the other core type's running time equals task-clock's running time; otherwise `unavailable`.

## Decision

Option 3. Each per-type event runs while the command runs on that type, so the two running times add up to task-clock's: probed on 2026-10-31 with perf 7.3.1, 6705191 + 3324318 = 10029509 ns and 7082885 + 2239587 = 9322472 ns in two unpinned runs, exact, and the reviewer saw the same in 6 of 6 runs. When one type is `<not counted>` and the other's running time is all of task-clock's, the command spent no time on the first type and retired no instructions there, which the row then shows directly rather than by inference. perf prints the software event as `task-clock:u`, and it is matched by that base name. Pinned rows are unchanged: they take no task-clock, and a `<not counted>` there stays `unavailable`.

## Consequences

Shared rows from 2026-10-31 on carry a per-row check. Rows written under decision 0042 item 5 between `518823d` and this record keep their `0`s; none was compared with anything. The review's further observation, that the same sum would show when a numeric per-type count is partial, is not acted on here; a later record may mark such rows.
