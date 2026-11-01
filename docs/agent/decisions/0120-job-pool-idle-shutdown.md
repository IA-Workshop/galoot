# 0120 The job pool stops itself after it has had no client for a while

- Status: accepted (requested by the owner, 2026-11-01)
- Date: 2026-11-01
- Decided by: the owner; recorded by Claude (agent, Sonnet 5.5)

## Context

The pool of build job slots (PERFORMANCE P11) is a user service that outlives any one session. A session that is lost leaves the pool running until someone stops it by hand.

## Decision

1. `tools/jobpool start` takes `--idle MINUTES` (default 30; 0 means never). The service checks every 30 seconds whether any process other than itself and its own children holds the FIFO open. If none has for the idle time, it exits, and the service's stop step removes the FIFO.
2. A client counts only while it holds the FIFO. Ninja holds it for a whole build, so a running build is never cut off. A client killed with SIGKILL no longer counts once it is gone, which a free-slot test would not have shown.
3. `tools/jobpool ensure` starts the pool with the defaults unless it is running. A build begun after an idle shutdown needs it first; agent briefs and sessions use `ensure` in place of `start`.
4. `status` prints the idle time. `JOBPOOL_UNIT`, `JOBPOOL_FIFO`, and `JOBPOOL_POLL_SECONDS` override the unit name, the FIFO path, and the poll interval, so a test can run a pool of its own.
5. The service runs the script's own `_serve` subcommand, not an inline `bash -c`, because systemd expands `$` sequences in a command line.

## Evidence

On a separate unit and FIFO with a one-minute idle time and a 5-second poll: the pool stayed up past the idle time while a process held the FIFO for 80 seconds, then stopped itself about 55 seconds after that process exited and removed the FIFO. The running pool of the project was not touched.

## Consequences

- The pool that is running now was started without the watcher. It gets one when it is next restarted: `tools/jobpool stop && tools/jobpool start 23`, with no agent building.
- PERFORMANCE P11 is governance, which changes only by an owner-ratified change; its text is not edited here.
