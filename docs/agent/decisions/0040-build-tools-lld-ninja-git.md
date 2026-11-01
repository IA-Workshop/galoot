# 0040 Build tools: LLD, Ninja, and git

- Status: accepted
- Date: 2026-10-31
- Recorded by: Claude (agent, build-expert, plan 0003), under C18; a decision record takes effect when recorded, and the owner may reverse it. Adds to decision 0027, which named Python 3, perf, and taskset but not these three (W5 review of WU1, finding N10).

## Context

C13 makes each dependency of the tooling or the tests a decision. WU1's build uses three platform tools that no decision names: the LLD linker (`CMAKE_LINKER_TYPE LLD` in the base preset), the Ninja generator (every preset), and git (`tools/measure` records the revision and branch of each row, and `tools/timed` finds the checkout root with it). Decision 0006 limits the toolchain to platform-supplied software.

## Options

1. LLD, Ninja, and git, all platform packages.
2. The GNU linkers (`ld.bfd`, `ld.gold`) instead of LLD: they are also on the platform, but LLD is the one research note 0002 measured and the one both compilers drive the same way.
3. Make instead of Ninja: no `test_prep/<test>` targets, no `-t deps` or `-t commands` queries, which the test selection and the compile-row count rely on.
4. A revision supplied by the caller instead of git: every row would depend on the caller passing it, and the `-dirty` mark could not be computed.

## Decision

Option 1, at the versions on the development machine on 2026-10-31: `lld 23.1.4-2` (LLD 23.1.4), `ninja 1.13.2-3.1` (Ninja 1.13.2), and `git 2.56.0-1.1` (git 2.56.0). No package outside the platform. Ninja is also the jobserver client that draws from the shared pool; its behavior when the pool restarts or a build is killed is recorded in plan 0003.

## Consequences

The core depends on none of them (C13). A Ninja release that changes the jobserver protocol or the `-t` tools changes the pool's behavior and the test selection; `docs/agent/snippets/jobserver_check.py` rechecks the former. git runs once per recorded compile, without optional locks, so it does not contend with commits for the index lock.
