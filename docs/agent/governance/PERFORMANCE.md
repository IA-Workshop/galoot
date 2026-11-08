# Galoot performance

Status: ratified by the owner on 2026-10-31 (decision 0002). P5 amended on 2026-11-01, ratified by the owner: proceed as recommended (decision 0161). P7, P8, P9, and P13 amended on 2026-11-02, ratified by the owner (decision 0170, ratified by decision 0162 item 3). P9 amended on 2026-11-07, ratified by the owner: proceed with recommendations (decision 0184). P9's sentence on the `runlog` label amended on 2026-11-08, ratified by the owner: accept the review's recommendations (decision 0215 item 10).

This document says how Galoot's costs are measured and kept in proportion to their use. It adds detail to C6, C12, and C14 of the [constitution](CONSTITUTION.md) and to V15 of [VVUQ.md](VVUQ.md), and it sets the scope of the test runs that [WORKFLOW.md](WORKFLOW.md) asks for.

Galoot has three costs:

- Build cost: the time and memory a compiler spends generating structures. Galoot is header-only and generates its code at compile time, so every user pays this cost on every build.
- Run cost: the speed and memory use of the generated code.
- Development cost: the machine time and agent cost of building and testing Galoot itself.

All three are measured and recorded. None has a bound unless the owner sets one (C14). A notable change in a measured cost is reported with its cause.

## Build cost

### P1 Build cost is measured

The build records, for each test and benchmark translation unit, its wall time and the compiler's peak memory, on each supported compiler. A change to the core records both before and after the change (W10). Where a cost needs explaining, the compiler's own time report attributes it to parsing, template instantiation, constant evaluation, or code generation.

### P2 Growth with use

A benchmark measures build cost against the number of generators and against the number of distinct operations a program instantiates, each over at least three sizes, so that the rate of growth is visible. Build cost that grows with the dimension of the algebra while the use stays fixed contradicts C6 and is recorded as a defect, unless the growth follows from the algebra's own structure, such as the sparsity and symmetry pattern of its signature data; that growth is measured in compiler instruction counts over at least three sizes, reported, and never blocks progress (decision 0005).

### P3 Work done once

Each distinct table, meaning a signature, an operation, and its input blade sets, is generated once per translation unit and reused (STYLE S7). A header costs little beyond parsing for the structures a program does not use, and a program includes only the modules it uses. A precomputed or cached table replaces generation only under C3.

## Run cost

### P4 Generated code

The code generated for an operation is what a careful person would write by hand for it: straight-line arithmetic over the terms that can be nonzero, with no run-time branching on basis elements, no allocation, and no indirection. Representative operations are checked by reading the compiler's output at the optimization level users build with, and by a benchmark against a hand-written form (C3, V15).

### P5 Memory layout

A named type stores only the components that can be nonzero (C6), contiguously, so its size is its number of components times the size of its scalar, which a compile-time check confirms. Values are trivially copyable (STYLE S8), so arrays of them are dense and are traversed in memory order. Other layouts, for vector units or for arrays of components, are extensions chosen by measurement.

### P6 Benchmarks

A run-time benchmark takes inputs the compiler cannot know and uses its results, so that the optimizer cannot fold or remove the work it measures. It repeats over separate runs, not only over iterations within one run, and reports a central value with its spread. A comparison states its baseline, and both sides share the revision, toolchain, options, machine, and workload (V15). A benchmark on a shared machine holds the processors it measures for its duration, and where the other load on the machine is unknown, its record says so.

### P7 Performance history

The measurements of P1, P2, and P6 are kept in `docs/agent/records/performance/`, each with its revision, toolchain, machine, options, and workload. Once the history shows how a quantity varies, a change that can affect the quantity is compared with the history by a criterion stated before the comparison, and a change beyond the variation is reported with its cause. A change of compiler, library, or machine is marked in the history so that it is not read as a change of the code. The history also holds the run log of P13 in distilled form: for each kind of run (its step, preset, selection family, placement, and outcome), the number of runs, the revisions and times they span, the toolchain in force, and the first, latest, least, median, and range of their wall time, peak memory, and retired instructions.

## Development cost

### P8 Test scope follows change scope

A test run holds the tests a change can affect, and no others until a merge needs more (P9). These are the tests of each component the change touches and of each component that includes one of them, directly or through others, together with each new or changed test and each test that failed at its last run. A change to something many tests share widens the run:

| Change | Tests that run |
| --- | --- |
| A component's header | Its tests, and the tests of every component that includes it |
| A test, or a helper shared by tests | The tests that use it |
| The build description, compiler options, or toolchain | Every test built under what changed |
| A vendored dependency in `include/ext/` | Every test that uses it |
| A reference script or data in `test/reference/` | The checks that read it |
| The run log, a campaign log, or the history in `docs/agent/records/performance/` | `tools/runlog check`, once for the merge (P9) |
| Documentation only | None |

Tests carry labels for their module and component, so a selection is named by labels rather than listed test by test.

### P9 Scope by merge level

Each merge runs the checks for the scope it integrates. A lower level never runs more than the level above it.

| Point | Checks that run |
| --- | --- |
| During work and before each commit | The selection of P8 in one configuration |
| Merge into a feature or subfeature branch | The selection of P8 for everything the branch changed since it left its parent, in each configuration the change can affect |
| Merge into `develop` | The full suite on each supported compiler, one of them under the sanitizers of STYLE S5, the full suite under `clang-measure`, and the mechanical style checks (W5) |
| Release | The full suite in every supported configuration, the benchmarks, and an update of the performance history (W13) |

A merge that changes a log under `docs/agent/records/performance/` runs `tools/runlog check` on the merged tree before the merge is committed, once for the merge and not in each configuration, and `tools/runlog prune` when the check reports removed rows. The CTest test `tools.runlog_check` carries the label `runlog` and is left out of the presets' runs, so a test preset selects nothing by that label; it runs from a build tree with `ctest --test-dir <tree> -L '^runlog$'`.

A test long enough to slow the edit and test cycle carries the label `slow`. It runs before a commit only when the change touches the component it tests, and otherwise from the next merge level up. Deferral moves a check to a later merge; it never omits one.

### P10 Runs not repeated

A passing run is evidence for the revision and configuration it ran on, so a run is not repeated on an unchanged revision and configuration. After a fix, the tests that failed run first, and then the selection. A test is never rerun to get a pass: a test that both passes and fails on one revision is a defect of the test or of the code (W9).

### P11 Shared machine

Several workstreams, of people and of agents, share the development machine. Each works in its own checkout of the repository with its own build directory, so that no two builds write the same files. Concurrent workstreams draw their parallel build and test jobs from one pool of job slots shared across the machine, so that together they share the processors rather than oversubscribing them; a limit set inside one build tree does not do this, because it cannot see the others. The owner sets the pool at 23 slots, one fewer than the physical cores, leaving one for the owner (C14, P14). A workstream does not start a build or a run that the free processors and memory cannot serve. A run that measures performance (P1, P6) does not overlap other heavy work on the machine.

### P12 Build only what runs

A run builds only the targets its selection needs. Build directories persist between runs, so builds are incremental. A clean build is made only when the build description or the toolchain changes, or when a build is suspected to be stale.

### P13 Record of a run

Each build and test run is timed by `tools/timed`, which appends its wall time, revision, branch, preset, selection, and exit status to the run log, `docs/agent/records/performance/runs.tsv`. The runs of a measurement campaign go to the campaign's own log instead, named when the run starts and committed with the campaign's records. Where the machine exposes them, a run also records peak memory, user-space performance counters (retired instructions, cycles, and cache references and misses at each level), and the package energy counter, each with the processor it ran on; a quantity the machine does not expose is recorded as unavailable. The energy counter covers the whole package, so an energy figure is comparable only for a run made while the machine is otherwise idle, and says so (decision 0018). These times are tracked and noted, never asserted on: they show how long the suites take and how that changes. The runs of the checks of a merge or a milestone gate are named `gate:<selection>`, and a compaction keeps every row so named. When a log grows too large to work with, the owner may approve its compaction at a recorded boundary revision: its rows up to the boundary are distilled into the performance history (P7) and removed from the log, except these, which stay in the log unchanged: the rows named `gate:<selection>`; the workflow runs of named presets at any revision; and, at committed revisions, the configure, build, and test runs of named presets, the `lint` and `gate` steps, and the `check` and `trace` steps of the cheap gates `tools/runlog` names (`trace`, `trace-gate`, `convention-trace`, `markers`, `golden`, `citations`, `golden-facts`, `reference-markers`, `golden-check`, `conventions_check`); and the few rows that later merges of other branches need. Any other row, a gate run under another name or on an uncommitted tree among them, is counted in the history and is retrieved from version control by the command the history names. The message of each merge names the selection that ran, its configurations, its outcome, its wall time, and any check deferred to a later merge (P9). Agent work also records its agent cost (W10).

## The development machine

### P14 Processors of two kinds

The development machine has performance cores, each running two hardware threads, and efficiency cores, each running one at a lower clock rate with smaller caches. The same work takes different times on the two kinds, and two threads on one performance core slow each other. So a measurement of time (P1, P6) runs pinned to one performance core whose other thread is idle, records the processor it ran on, and also records the count of retired instructions, which does not depend on clock rate. Times measured on different kinds of core, or with different load on the core's other thread, are not compared. Peak memory does not depend on the kind of core. A test's timeout allows for the test running on an efficiency core. The shared pool of P11 is sized against physical cores, not hardware threads, so that concurrent jobs fill every core once before any core runs two, and the remaining threads stay free for interactive work.
