# 0016 Generalizing the shared job pool

- Date: 2026-11-01
- Author: Claude (agent, Opus 5.5), in the build-expert role, as researcher
- Branch: `research/jobpool-generalization`, from `develop` at `e1af2ac`
- Question (from the owner): is `tools/jobpool` useful beyond Galoot, and if so, how should it be extracted and generalized?
- Decision proposal: [0130](../decisions/0130-extract-the-job-pool.md)

Tags: **confirmed** means run on this machine or read in the primary source, named in place; **inferred** means reasoned from what was read or run, not checked directly; **background** means recalled, not re-read.

## Answer in brief

1. No existing tool does what `tools/jobpool` does with these five properties together: one pool for the whole machine, an idle stop, a status that tells lost slots from slots in use, one pool per user, and no root setup. The closest is Gentoo's `steve` (and Alexander Monakov's `guildmaster`, which it resembles). Both serve the same protocol from a CUSE character device, which lets them count tokens per client and take them back when a client dies. They need `/dev/cuse`, which is root-only on this machine (`crw------- root root`), and neither is installed. The other tools found either act inside one build (Ninja's coming `--jobserver-pool`, Make, Cargo), wrap one command at a time (GNU `sem`), or bound processor time rather than the number of jobs (systemd slices). Section 2 has the survey.
2. The pool is already used beyond Galoot. `math-tools` calls `galoot/tools/jobpool env` by absolute path (`exercises/numerical/tools-domains/install/sollya.sh`, line 14) and tells its agents to use it (`RECOMMENDATION.md`, line 25). The knowledge base's `AGENTS.md` does the same. Other projects of the owner build with CMake and would draw from the same pool: `meshing` (9 CMakeLists files), `compsci` (3), `physics` (3), `quantum` (2). Each new caller that names a Galoot path couples another project to this repository's layout.
3. The current script handles the main hazards, but has four weaknesses worth fixing in any extraction. It recognizes clients only by exact path (demonstrated below). Each client's implicit slot lets k clients run 23 + k jobs. Lost slots can be recovered only by a restart, and the restart is itself a hazard. There is a narrow race between the idle stop and a new client. Section 4 lists all of them.
4. Recommendation: extract it, as a single-file Python tool with a plain-bash self-test, in a standalone repository of the owner's (for example `/home/brianm/projects/utils/jobpool`). Galoot keeps a vendored copy at `tools/jobpool`. A prototype that fixes the weaknesses is at [`records/spikes/jobpool-generalization/`](../records/spikes/jobpool-generalization/); its self-test passes 38 of 38 checks on a throwaway pool. Decision 0130 lists the choices that are the owner's.

## 1. Toolchain on this machine (confirmed, 2026-11-01)

| Tool | Version | Jobserver role |
| --- | --- | --- |
| GNU Make | 4.4.1 | Server (pipe or fifo) and client |
| Ninja | 1.13.2 | Client only; fifo style only (binary has only `PosixJobserverClient`; `ninja --help` lists no server option) |
| CMake / CTest | 4.4.5 | CTest is a client since 3.29; `JOB_SERVER_AWARE` (3.28) affects only Makefile generators |
| GCC | 16.2.1 | `-flto=jobserver` and `-flto=auto` join a running jobserver (gcc.info) |
| Clang / LLVM | 23.1.4 | `libLLVM` contains `llvm::JobserverClient` (symbol names and its warning strings); `ld.lld --help` names no jobserver option. Which LLVM tools use the client was not checked |
| Cargo / rustc | 1.98.1 | Client; joins an inherited FIFO (below) |
| Python | 3.14.8 (only `python3.14`) | |
| bash | 5.3.20 | |
| systemd | 262 | `systemd-run --user`, transient units |
| util-linux `flock` | 2.42.4 | |
| GNU parallel / `sem` | 20260922 | Counting semaphore, not a jobserver |
| distcc, icecream, sccache, ccache, bats, shellcheck | not installed | |
| CPU | i9-14900K, 32 threads, 24 physical cores (24 distinct `thread_siblings_list` sets in sysfs) | |
| Lingering | `loginctl show-user brianm -p Linger` gives `Linger=no` | The user manager, and with it the pool, ends when the user's last session ends (inferred from systemd's documented behavior) |

## 2. What already exists

### 2.1 The protocol

GNU Make manual, section 13.1 "Sharing Job Slots with GNU make" and 13.1.1 "POSIX Jobserver Interaction" (installed `make.info`, Make 4.4.1; online at https://www.gnu.org/software/make/manual/html_node/Job-Slots.html), confirmed by reading:

- Clients find the jobserver in `MAKEFLAGS` as `--jobserver-auth=`; only the last instance counts.
- POSIX has two styles. `fifo:PATH` names a named pipe that any process can open by path. `R,W` names inherited descriptors, which reach only commands that Make knows are recursive (prefixed with `+`). `--jobserver-style=fifo|pipe` chooses the style, and fifo is the default where it is supported. The fifo style first appeared in GNU Make 4.4 (the Ninja 1.13.0 release note says fifo needs "version 4.4 on non-Windows systems"; https://github.com/ninja-build/ninja/releases/tag/v1.13.0, read 2026-11-01). Before 4.2 the option was spelled `--jobserver-fds` (background).
- "Every command make starts has one implicit job slot reserved for it before it starts." A client runs one job without a token and must not return that slot.
- A client must return exactly the tokens it read, the same characters, "even under error conditions", including signals.
- The pipe is preloaded with one byte per extra slot. A pool of N bytes therefore serves `-j(N+1)` in one Make tree.

### 2.2 Clients, and servers inside one build

| Tool | Behavior | Evidence |
| --- | --- | --- |
| Ninja 1.13 | Joins a fifo jobserver from `MAKEFLAGS` automatically; an explicit `-j` disables it. Runs tokens + 1 jobs. When SIGKILLed it returns nothing; on SIGINT to its process group and SIGTERM it returns everything. Holds the FIFO open for its whole build. A FIFO that does not exist makes it print "Could not initialize jobserver" and run 34 jobs on 32 CPUs | Confirmed: `docs/agent/snippets/jobserver_check.py`, rerun today at `e1af2ac` (12.8 s, all five cases as in its 2026-10-31 record) |
| Ninja 1.14 | Adds `--jobserver-pool`, which creates a fifo pool sized by `-j` for one Ninja invocation and its children. It does not create a pool when a parent jobserver is detected. Not installed here | Read: PR ninja-build/ninja#2634, merged 2026-09-03, milestone 1.14.0 (https://github.com/ninja-build/ninja/pull/2634, read 2026-11-01). Release status of 1.14 not checked |
| CTest 4.4.5 | With `-j` and no value, as Galoot's test presets set it, takes one token per test and runs tokens + 1 tests at once | Confirmed: probe of 8 sleeping tests on a 3-token throwaway pool, most at once 4. Documented in `Help/manual/ctest.1.rst`, "Job Server Integration" |
| CTest 4.4.5, programs missing | Each test whose target program is missing ("Not Run") keeps its token. Once the tokens are gone, the run stops early: with 3 tokens and 6 such tests it listed "4 tests failed out of 4", leaving 2 tests out of the count. Run serially on an empty pool, the same tree lists all 6. A missing program named by an absolute path (`COMMAND /nonexistent/...`) lost no token | Confirmed: minimal project of six `add_executable` + `add_test(NAME COMMAND target)` pairs, programs deleted after the build, repeated twice with the same result. Extends lesson 0063, which saw the loss but not the dropped tests |
| GNU Make 4.4.1 | Started with an inherited `MAKEFLAGS` of the pool, it joins it (8 one-second jobs, most at once 4 with 3 tokens). An explicit `-j8` prints "warning: -j8 forced in submake: resetting jobserver mode." and runs 8 | Confirmed: probe makefile on a throwaway pool |
| Cargo 1.98.1 | Joins an inherited FIFO and passes it to build scripts (`CARGO_MAKEFLAGS="-j --jobserver-fds=fifo:… --jobserver-auth=fifo:…"`). Without one, it creates its own pipe jobserver (`--jobserver-auth=3,5`) | Confirmed: probe crate whose build script prints the variables. The `jobserver` crate's `Client::new` creates a server for one process tree (https://docs.rs/jobserver/latest/jobserver/struct.Client.html, 0.1.35, read 2026-11-01) |
| Cargo, token holding | Keeps tokens across jobs rather than returning each one; the protocol allows this | Read: M. Górny, "How hard is it to get a jobserver client right?", 2026-09-02, https://blogs.gentoo.org/mgorny/2026/09/02/how-hard-is-it-to-get-a-jobserver-client-right/ (cites cargo issue 17391), read 2026-11-01 |
| Other client defects | GNU Make does not return tokens on SIGINT (Savannah 67687). Make and GCC write back a different character from the one they read (67822, GCC 124212). GCC treats a jobserver that fails to open as an internal compiler error (GCC 124211). `nasm-rs` writes an extra token. The `jobserver` crate cannot install signal handlers | Read: same post; not reproduced here |

None of these is a pool shared across independent builds: each serves one process tree.

### 2.3 Machine-wide jobservers

| Tool | How | Lost tokens | Idle stop | Per user, no root | Status |
| --- | --- | --- | --- | --- | --- |
| `steve` (Gentoo, 2025) | CUSE character device `/dev/steve`, given to clients as `--jobserver-auth=fifo:/dev/steve`; also limits on load average and free memory; `stevie` changes the job count at run time | Takes back tokens when the process that took them exits | No (a system daemon) | No: needs `CONFIG_CUSE` and access to `/dev/cuse`; the service runs as user `steve`, and access to the device is through a group | `stevie -t` shows tokens left |
| `guildmaster` (A. Monakov) | CUSE device `/dev/guild`; `gm-run CMD` takes a token for the command and sets `MAKEFLAGS` | Counts tokens per open file description and refills the pool when one closes with tokens unreturned | No | No: `/dev/guild` is root-only by default; widening it needs udev rules | Not described |
| `tools/jobpool` | A FIFO held by a transient user service | Detected when no client holds the FIFO; recovered by a restart | Yes (decision 0120) | Yes | Free, total, clients, lost |

Sources, read 2026-11-01: Gentoo wiki, "Steve" (https://wiki.gentoo.org/wiki/Steve); Gentoo news, "2025 in retrospect" (https://www.gentoo.org/news/2026/01/05/new-year.html: "an implementation of a token-accounting system-wide jobserver"); M. Górny, "One jobserver to rule them all", 2025-11-30 (https://blogs.gentoo.org/mgorny/2025/11/30/one-jobserver-to-rule-them-all/); guildmaster README (https://codeberg.org/amonakov/guildmaster). Górny's view is that proper token accounting cannot be built on a plain FIFO, which is why both use CUSE. The gitweb page of steve itself refused the fetch (HTTP 403), so steve's license and source were not read. On this machine `CONFIG_CUSE=m`, the module is not loaded, and `/dev/cuse` is `crw------- root root` (confirmed). Using either tool would therefore need root once: a udev rule and a group, or a system service.

### 2.4 Other ways to bound the machine

- GNU `sem` (`parallel --semaphore`, 20260922, `man sem`, confirmed by reading): a counting semaphore named by `--id`, stored under `~/.parallel/semaphores/`, one slot per wrapped command. It speaks no jobserver protocol. It could only bound whole builds, or wrap each compiler call as a launcher, which pays a Perl start per translation unit while Ninja still starts all 34 processes (inferred).
- `flock`-based slots, as in distcc's local slots (distcc keeps per-user lock files in `DISTCC_DIR`, by default `~/.distcc/`; https://www.distcc.org/man/distcc_1.html, read 2026-11-01). The kernel drops a lock when its holder dies, so slots cannot be lost. But only commands that take the locks are bounded, typically compiler wrappers, and Ninja, CTest, and Cargo do not take them (the per-slot lock-file scheme is background).
- icecream, sccache-dist: schedulers that distribute compiles across machines. They do not bound local non-compile work such as tests (background; not installed).
- systemd resource control (`man systemd.resource-control`, systemd 262, confirmed by reading): `CPUQuota=`, `AllowedCPUs=`, `MemoryHigh=`, `TasksMax=` on a slice or a `systemd-run --user --scope`. They bound processor time and memory, not the number of jobs. Without a pool, 4 builds × 34 jobs still start 136 compilers, time-sliced into the quota, each with its own memory (inferred). They complement a pool. For example, a slice with `MemoryHigh=` would protect the owner's session from a build that exceeds memory. They do not replace it.

### 2.5 Verdict

No tool found has all five properties. `steve` covers the hard part that a FIFO cannot, reclaiming tokens from dead clients, at the price of root-level setup and a system daemon. Adopting it is a valid alternative for the owner (option C in decision 0130). Short of that, a FIFO pool with good status and automatic repair is the per-user, root-free design, and nothing ready-made provides one.

## 3. Hazards of the protocol, and how the pool meets them

| Hazard | Today (`tools/jobpool` at `e1af2ac`) | Evidence | Prototype |
| --- | --- | --- | --- |
| H1 A client killed with SIGKILL keeps its tokens | Detected only while no client holds the FIFO; recovered by `stop` and `start` | Confirmed: snippet, 0 of 3 returned; prototype self-test | The service refills the FIFO whenever no client holds it (`repair = auto`), and `jobpool repair` does it on demand. Safe because a token can be in use only by a process that holds the FIFO open (inferred: true of Make, Ninja, CTest, and Cargo as observed, which hold it for the whole run; a client that closed and reopened the FIFO while holding tokens would be shortchanged) |
| H2 CTest keeps the token of a test whose program is missing, and then drops tests | Not prevented; lesson 0063's rule for drivers | Confirmed above | Same refill. The dropped-test behavior is CTest's and is recorded for awareness only |
| H3 A restart under running clients | `stop` refuses while clients hold the FIFO | Confirmed: snippet, 8 jobs at once where one pool allows 5 for two clients | `stop` refuses as before. Automatic repair removes the main reason to restart (four restarts on 2026-11-01, handoff note) |
| H4 A client starts during the idle stop | Not handled. The watcher counts clients, then exits, and the stop step removes the path. A client that opens the FIFO in between keeps the old pool, and the next `ensure` starts a second one, so both serve at once. A client that starts after the removal without `ensure` finds no FIFO, and Ninja then runs 34 jobs (H6) | Inferred from the script; the window is one poll's decision, milliseconds after 30 idle minutes; not reproduced | The service decides under an exclusive `flock` and removes the FIFO itself before it releases the lock. `run` opens the FIFO under a shared lock and checks that the inode it opened is the one at the path. So `run` is either counted or sees the FIFO gone and starts a new pool. Clients started directly are not covered. Not tested: the race cannot be forced reliably from a shell |
| H5 The implicit slot | Each client runs one job without a token, so k clients run 23 + k jobs. P11 speaks of 23 slots leaving one core for the owner; with three clients the machine runs 26 jobs on 24 cores | Confirmed: Ninja 4 with 3 tokens (snippet); CTest 4 with 3 tokens; two Ninja clients 8 (5 expected by the protocol; the restart case) | `run` takes one token for the command and returns it when the command ends, so builds under `run` total at most N jobs (self-test: one Ninja under `run` 3 with 3 tokens, two Ninja builds under `run` together 3, Ninja started directly 4) |
| H6 The FIFO is missing | `tools/timed` refuses a build or test step when the FIFO named in `MAKEFLAGS` is missing; `tools/jobpool env` prints a line even when no pool runs | Confirmed: Ninja runs 34 jobs (snippet) | `env` fails with exit 1 when no pool runs; `run` starts the pool if needed |
| H7 Nested builds | Inherited `MAKEFLAGS` reach nested Make, Ninja, CTest tests, and Cargo build scripts, which join the pool as the protocol intends. An explicit `-jN` in a nested call leaves the pool and runs N | Confirmed: Make `-j8` warning and 8 jobs; Cargo passes the FIFO to build scripts | `run` inside `run` takes no second token: the inner command uses the outer one's slot as its implicit slot. The marker is `JOBPOOL_HELD=<device>:<inode>`, not `MAKEFLAGS`, so a shell that ran `eval "$(jobpool env)"` still gets a slot (self-test) |
| H8 How the idle test finds clients | `count_clients` lists processes with a descriptor whose `readlink` equals the FIFO path exactly, minus the service shell and its descendants. `status` and `stop` use `find -lname "$fifo"`, which treats the path as a glob pattern | Confirmed weakness: a pool started with `JOBPOOL_FIFO=/run/user/1000//jobpool-research-b.fifo` and a client holding one token: `status` said "no client holds the pool, so 1 slots are lost", `_count_clients` gave 0 for that path and 1 for the canonical one, and `stop` without `--force` stopped the pool under the client. The idle watcher would also have stopped it under a running build. The real pool uses `$XDG_RUNTIME_DIR/jobserver.fifo`, which is canonical, so the live pool is not affected | Holders are found by the device and inode of the FIFO, and the path is made canonical at start |

## 4. Weaknesses in `tools/jobpool` at `e1af2ac`

Read line by line, and checked on throwaway units `jobpool-research-a`, `-b`, and `-c` with FIFOs under `/run/user/1000/`. The unit `galoot-jobserver` and its FIFO were never opened, stopped, or started; only `systemctl --user is-active galoot-jobserver.service` was read, and it was `active` before and after every run.

1. Client detection by exact path (H8). Confirmed above.
2. The implicit slot (H5) makes the 23-slot bound 23 + k. Confirmed.
3. A surplus is not reported. After a client wrote two extra bytes, `status` printed "running: 5 of 3 slots free", exit 0, with no warning (confirmed). Client defects of this kind are documented (nasm-rs, section 2.2).
4. Recovery from lost slots needs a restart, and a restart is H3 if a client appears. Confirmed by design.
5. The idle-stop race (H4). Inferred.
6. `env` prints a `MAKEFLAGS` line when no pool runs; the slot count then falls back to the default 23. Only `tools/timed` stands between that line and a Ninja run of 34 jobs, and only for commands it recognizes (confirmed by reading; the 34-job fallback is confirmed).
7. The total slot count is parsed from the unit's description text (`"... N slots"`); it would break if the description were edited. The default of 23 and the name "Galoot build job slots" are fixed in the script.
8. The idle clock is bash `SECONDS`. Whether bash 5.3 derives it from the wall clock or a monotonic clock was not checked. If from the wall clock, a clock step changes the idle time (inferred).
9. The FIFO path is pasted into `ExecStopPost=` and into the `_serve` command line. A path with a space, `%`, or `$` would be split or expanded by systemd (inferred from decision 0120 item 5 and systemd's command-line syntax; not tried).
10. Small points: `status` prints "after 1 minutes"; `start 23 24` silently takes the last number; `--idle` takes whole minutes only, so a test of the idle stop takes over a minute.
11. Lingering is off (section 1). A pool started from an agent session ends when the owner's last session ends. This is harmless, because `ensure` restarts it, but it is worth knowing (inferred).

The idle stop itself works as decision 0120 recorded. Unit `jobpool-research-a`, started with `--idle 1` and a 2-second poll, stopped itself with `Result=success` about a minute after its last client and removed its FIFO (confirmed).

## 5. Recommendation

Extract it. Decision 0130 holds the choices; in brief:

- **Where.** A standalone git repository, for example `/home/brianm/projects/utils/jobpool`, beside `utils/code-style` and `utils/amspell`. The tool serves every project of the owner and belongs to none. A directory inside `math-core` would still couple non-math projects to math-core. A package (AUR, PyPI) can come later; it adds release work and nothing that a clone or a copy lacks now.
- **Name.** Keep `jobpool`. It is what every brief, agent file, and the math-tools and knowledge-base instructions already say.
- **Language.** One Python 3 file, standard library only. The bash script already needs Python twice (for `FIONREAD` and the process scan). Python gives inode comparison, `flock`, monotonic time, and signal forwarding in one place. The prototype is 587 lines including its documentation, and its self-test 217; the bash original is 185.
- **Interface** (prototype). The commands are `start [SLOTS] [--slots N] [--idle 30m|90s|2h|0] [--no-repair]`, `ensure`, `run [--no-slot] -- CMD`, `status` (exit 0 healthy, 1 not running, 3 lost or surplus), `env` (exit 1 when not running), `repair`, `stop [--force]`, and `config`. Settings come from built-in defaults, then `$XDG_CONFIG_HOME/jobpool/config` (`key = value`), then `JOBPOOL_*` variables, then the command line. The default slot count is the physical cores less `reserve` (1). The prototype counts distinct `thread_siblings_list` sets, which gives 24 here and so 23 slots, the count P11 sets. The reasoning of P14 (fill each core once before any core runs two jobs) is kept in the tool's documentation without citing P11.
- **Service.** Keep `systemd-run --user` with a transient unit (`--collect`, settings passed with `--setenv`, not on the command line). A unit template would have to be installed and gains nothing: the pool starts on demand through `ensure` or `run`, and systemd's socket activation cannot preload tokens.
- **Shell completion.** Optional. Eight subcommands and a handful of flags fit a 15-line bash completion; it is not in the prototype.
- **Tests.** A plain-bash `selftest.sh` against a unit and FIFO named after its pid, with a guard that refuses any other unit name. bats is not installed and is not needed.
- **Portability.** Linux with a systemd user manager only: `/proc`, `FIONREAD` on a FIFO, and `systemd-run`. Without systemd, a fallback holder (`setsid` plus a pid file) would be about 20 lines; worth adding only if a non-systemd machine appears. macOS would need launchd, `lsof` for client detection, and GNU Make 4.4 from a package manager; not cheap, and not recommended.
- **License.** MIT, as Galoot's `LICENSE` is, unless the owner prefers otherwise.
- **Galoot's side.** `tools/jobpool` becomes a copy of the extracted tool at a recorded revision, vendored the way `include/ext` is. Galoot then stays usable from its checkout alone, and `tools/timed` keeps working unchanged. C13 asks that adopting a tooling dependency weigh it against an owned implementation, and that a dependency one person supports have a record of how work continues; the owner owns both, and the vendored copy is that record. The alternative is a three-line shim that runs `jobpool` from `PATH`. It saves the copy, but it fails in a fresh environment where the tool is not installed. Either way, the code and comments cite no records (S11), and the prototype already follows that rule.
- **Migration of the running pool.** For the changeover, the extracted tool can be configured with `unit = galoot-jobserver` and `fifo = /run/user/1000/jobserver.fifo`, so that the old and new scripts address the same pool. The pool switches to the new service code only when it next starts. With the idle stop, that happens without anyone restarting it: after 30 minutes with no client, the next `ensure` starts the new code. No agent restarts the pool.

### Effort estimate (inferred)

- Standalone repository: the prototype plus a README, a LICENSE, completion, and a review of the self-test. About 2 to 3 agent hours at Opus, plus one independent review.
- Galoot: vendor the copy, update `tools/timed`'s message, and replace `tools/jobpool start 23` and `stop && start` in the agent files and briefs with `ensure`, `status`, and `repair`. About 1 agent hour. P11's text about 23 slots changes only if the owner changes what a slot means (decision 0130, choice 6).
- math-tools and the knowledge base: replace the absolute Galoot paths with `jobpool` on `PATH`. Minutes, in their own worktrees.

## 6. The prototype

[`docs/agent/records/spikes/jobpool-generalization/`](../records/spikes/jobpool-generalization/) holds `jobpool` and `selftest.sh`. It sits with the spikes because `docs/agent/workbench/` is ignored by `.gitignore` (departure; see decision 0130). It does not replace `tools/jobpool`.

- Self-test: `tools/timed selftest - jobpool-prototype -- bash docs/agent/records/spikes/jobpool-generalization/selftest.sh` at `0b75d0a`: 38 of 38 checks passed, 21.2 s, exit 0 (row in `runs.tsv`). It covers settings and defaults, lifecycle, clients by inode under a non-canonical path, refusal of `stop` and `repair` under a client, lost and surplus slots with `status` exit 3 and `repair`, automatic repair, `run` (it starts the pool, holds one slot, gives a nested run no second slot, passes exit status and 128 + signal, forwards SIGTERM, returns the slot), Ninja concurrency directly and under `run`, and the idle stop with FIFO removal.
- The self-test can fail: three mutants of the tool each failed it. Taking no slot in `run` failed 4 checks. Doing nothing in the token balance failed 5. Never matching a holder failed 12.
- Not tested: the H4 lock (by reasoning only), behavior under memory pressure or with many clients, use from a non-systemd system, and stability over repeated runs (the idle and concurrency checks depend on timing, with margins of about one second).
- Earlier runs at `e1af2ac`, before the files were committed, are also in `runs.tsv`: two failing runs whose failures were defects of the self-test (a background function's `$!` names the subshell, not the process; an empty variable used to override a setting), and one passing run.

## Sources

Primary sources read on 2026-11-01:
- GNU Make manual 4.4.1, sections 13.1, 13.1.1, and the `--jobserver-style` option (installed info pages).
- CMake 4.4.5 documentation: `Help/manual/ctest.1.rst` (Job Server Integration), `Help/release/3.28.rst`, `3.29.rst`, `4.0.rst`, `Help/command/add_custom_command.rst`.
- GCC 16 manual, `-flto` (installed info pages).
- `man sem` (GNU parallel 20260922); `man systemd.resource-control` (systemd 262).
- Ninja v1.13.0 release notes, https://github.com/ninja-build/ninja/releases/tag/v1.13.0; PR #2634, https://github.com/ninja-build/ninja/pull/2634.
- jobserver crate 0.1.35, https://docs.rs/jobserver/latest/jobserver/struct.Client.html; Cargo book, build scripts, https://doc.rust-lang.org/cargo/reference/build-scripts.html.
- Gentoo wiki, Steve, https://wiki.gentoo.org/wiki/Steve; Gentoo news 2026-01-05; M. Górny's posts of 2025-11-30 and 2026-09-02 at https://blogs.gentoo.org/mgorny/; guildmaster README, https://codeberg.org/amonakov/guildmaster.
- distcc(1), https://www.distcc.org/man/distcc_1.html.

Some of these pages were read through a summarizing fetch tool; the quotations above are the ones it returned. The steve source repository (gitweb) refused access and was not read.
