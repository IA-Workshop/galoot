# 0130 Extract the job pool into a tool of its own

- Status: accepted by the owner, 2026-11-02 with the recommended options, the repository at `math-core/jobpool` (decision 0162 item 4); a side lane after WU9, not started
- Date: 2026-11-01
- Proposed by: Claude (agent, Opus 5.5), build-expert role, as researcher
- Evidence: research note [0016](../research-notes/0016-jobpool-generalization.md); prototype in [`records/spikes/jobpool-generalization/`](../records/spikes/jobpool-generalization/)

## Context

`tools/jobpool` keeps the machine's shared pool of build job slots (PERFORMANCE P11; decision 0120 added the idle stop). It is already used outside Galoot. `math-tools` and the knowledge base call it by its absolute Galoot path, and other CMake projects of the owner (`meshing`, `compsci`, `physics`, `quantum`) would share the same pool. No existing tool has its five properties together: machine-wide, idle stop, status with lost-slot detection, per user, no root (note 0016 section 2). The script also has four weaknesses (note 0016 section 4): it finds clients by exact path only; each client's implicit slot gives 23 + k jobs; lost slots need a restart; and the idle stop can race a new client.

## Choices for the owner

Each choice lists the options; the proposal's recommendation comes first and is marked.

1. **Extract or not.**
   - A (recommended): extract into a tool of its own and have Galoot depend on it (choice 3).
   - B: keep `tools/jobpool` in Galoot, fix the weaknesses there, and let other projects keep calling Galoot's path.
   - C: adopt Gentoo's `steve`, a CUSE jobserver that takes tokens back from dead clients. It needs root once: load `cuse`, then a udev rule or group for `/dev/cuse` and `/dev/steve`, and a system service. It has no idle stop, and whether it is packaged for Arch was not checked. It solves lost tokens fully, which a FIFO cannot.
2. **Where it lives.**
   - (recommended) A standalone repository, `/home/brianm/projects/utils/jobpool`.
   - A directory in `math-core`, such as `math-core/tools/jobpool`.
   - A package (AUR or PyPI), later, if it is ever shared beyond this machine.
3. **How Galoot depends on it.**
   - (recommended) A vendored copy at `tools/jobpool`, with the source revision recorded in a decision and refreshed by a small unit. The checkout keeps working alone, and `tools/timed` is unchanged.
   - A shim that runs `jobpool` from `PATH`. It saves the copy, but it fails where the tool is not installed.
4. **Name.** (recommended) Keep `jobpool`. Alternatives: `slotpool`, `buildslots`.
5. **Language.**
   - (recommended) One Python 3 file, standard library only, as in the prototype.
   - bash with embedded Python, as today.
6. **What a slot means.**
   - (recommended) Builds started through `jobpool run` give their command's implicit slot back to the pool, so N slots mean at most N jobs across all builds under `run`. Builds started directly still get N + 1 per client.
   - Keep today's meaning, where each client adds one job beyond N.

   With the recommendation, the owner may want 23 slots, the P11 count, which would then be a true bound, or 24. Either way, any change to P11's wording is the owner's to ratify.
7. **Lost-slot repair.**
   - (recommended) Automatic: whenever no process holds the FIFO, the service restores missing tokens and drains surplus ones. `jobpool repair` does the same on demand.
   - Manual only (`repair`).
   - None: restarts, as today.

   Automatic repair would have replaced the four restarts of 2026-11-01.
8. **Settings.** (recommended) Built-in defaults, then `$XDG_CONFIG_HOME/jobpool/config` (`slots`, `reserve`, `idle`, `poll_seconds`, `unit`, `fifo`, `repair`), then `JOBPOOL_*` variables, then the command line. The default slot count is the physical cores less a reserve of 1, which gives 23 here.
9. **Service.**
   - (recommended) A transient `systemd-run --user` unit, as today, with its settings passed by `--setenv`.
   - An installed unit template.

   A unit template adds an install step, and gains nothing for a pool that starts on demand.
10. **Unit and FIFO names.**
    - (recommended) Defaults `jobpool` and `$XDG_RUNTIME_DIR/jobpool.fifo`. During the changeover, a config file sets `unit = galoot-jobserver` and `fifo = /run/user/1000/jobserver.fifo`, so that old and new scripts serve the same pool. The new service code takes over at the pool's next idle stop, with no restart by anyone.
    - Keep the Galoot names for good.
11. **Portability.**
    - (recommended) Linux with systemd only.
    - Add a non-systemd fallback holder, about 20 lines, if a machine without systemd appears.

    macOS support is not recommended: it would need launchd, `lsof`, and GNU Make 4.4 from a package manager.
12. **Extras.**
    - Bash completion: about 15 lines; optional.
    - `status --json` for scripts: optional.
    - A memory cap on the default slot count, as `spack_tune.py` applies to Spack: optional, since the machine has 125 GiB.
13. **License.** (recommended) MIT, as Galoot's.

## If accepted as recommended

1. Create the repository from the prototype, add a README, a LICENSE, and completion if chosen, and have it reviewed independently (W5).
2. In a Galoot unit: vendor the copy into `tools/jobpool`, record its revision, and update the agent files, briefs, and `tools/timed`'s message. `stop && start 23` becomes `repair`, and `start` becomes `ensure` or `run`.
3. In math-tools and the knowledge base, in their own worktrees: replace `galoot/tools/jobpool` with `jobpool`.
4. Install for the owner. The exact command depends on choice 2; for a standalone repository: `ln -s /home/brianm/projects/utils/jobpool/jobpool ~/.local/bin/jobpool`.

## Consequences

- One pool for every project and agent, with no Galoot path in other repositories.
- The FIFO limits remain: a SIGKILLed client's tokens stay lost while other clients hold the pool, and are restored at the next moment with no client. Only option C removes that limit.
- `env` changes behavior: it fails when no pool runs, instead of printing a line for a FIFO that does not exist.

## Departures in preparing this proposal

- The prototype is in `docs/agent/records/spikes/` rather than `docs/agent/workbench/`, because `.gitignore` excludes the workbench and the brief asked for the prototype to be committed.
- No plan or claim line was committed to `develop`: the brief limited this unit to its own worktree and branch.
