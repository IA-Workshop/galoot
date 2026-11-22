# 0043 1.0.1: test, build and tool cost cleanup

- Status: proposed 2026-11-16 by Claude (agent, Opus 5.5), planner in the build-expert and performance-engineer roles. From `main` at `c95fee5`. Committed on `plan/0043`; the coordinator merges it into `main`, which is the claim (W3, W7). No unit below had started at the proposal. Closed 2026-11-21: U1 to U5 merged into `main` at `26cd467`; the outcome is section 14.
- Source of scope: the coordinator's brief for release 1.0.1 (cleanup only: no new feature, no interface change); the release record's "Known friction" items 1 and 2 and "Accepted limits of the project's tools" (`records/release/release-record.md`); the handoff of 2026-11-15, "Backlog after 1.0", tools and tests line; plan 0042 section 9; decision 0241 item 9 (the dirty-mark gap); decision 0246 item 1 (the opt-out review's m6, deferred); decision 0248 item 8 (plans 0027 and 0028 findings as known limits); decision 0228 item 9.
- Siblings: plan 0044 (public header and API hygiene) and plan 0045 (the 1.0.1 release procedure). This plan does not touch what they own; section 9 lists the files two plans may share.
- Evidence gathered for this plan: `records/performance/cost-cleanup-1-0-1/` (a compile probe of both units, section 3), and the checks named in section 4, run read-only on scratch copies.
- Shared resources: the job pool is read only (`tools/jobpool status` before and after each heavy run; `eval "$(tools/jobpool env)"` before builds; `tools/jobpool ensure` only when the status says "not running"; never stopped). Measurements take the timing lock with `bash /home/brianm/projects/utils/math-core/math-tools/exercises/performance/pe-common/lock.sh take` and `release`; builds that may exceed 10 minutes or 10 GiB run under `pe-common/memguard.sh`. No processor configuration is touched or proposed. No web request is needed.

## 1. Departures, ambiguities, and readings (C20)

1. **The integration branch is `main`.** The governance and the plans say `develop`; the brief says `main` now plays that role. Every "`develop`" in W5, W7 and P9 is read as `main`. `master` is not touched.
2. **The known limits of plans 0027 and 0028 were accepted by decision 0248 item 8, not 0251.** The handoff and the brief cite decision 0251; 0251 does not mention them. Section 7 works from 0248 item 8 and the review records.
3. **The dirty-mark gap, as recorded, does not reproduce.** Decision 0241 item 9 calls it "the unconfirmed dirty-mark gap (index differs from HEAD)" and no other record describes it. A change staged in the index is marked: in a scratch clone at `c95fee5`, the exact `git status` call of `tools/measure` (`revision_and_branch`) prints `M  include/galoot/core.hpp` for a staged edit, so the row would read `-dirty`. The gap found instead is in when the mark is read: once, after the command ends (section 4.1). Reading: the unit fixes that gap and pins the staged case with a test; if the owner meant something else, U3 is rescoped (owner item O4).
4. **The CTest label anomaly has no record.** Plan 0042 section 1 item 3 already said so. A survey of the labels of every CTest test in fresh gcc-debug (680 tests) and clang-measure (672) trees at `c95fee5` found none without its module label, none without a `<module>.<component>` label, and no duplicated label; the presets' filter `^(runlog|bench)$` leaves 657 and 649, the release matrix's counts. Without the observation the item names, nothing is planned for it beyond the record (U6, owner item O5).
5. **"`tools/mutate` running CTest in parallel"** is read as: the tool's CTest calls should draw from the shared pool as the presets do, instead of running the tests of a selection one at a time. Today `tools/mutate` calls `ctest --test-dir <tree> --tests-from-file <list>` with no `--parallel`, and it uses no test preset (the presets set `jobs` to `""`), so CTest runs serially. The opposite reading (it already runs in parallel and should not) is not supported by the code.
6. **The probe's wall times were taken under load.** An unrelated session held two cores at 100% and other work raised the load average to 4 to 6 during the probe; the runs were in shared mode, unpinned. Instructions and peak memory are comparable (P14); wall times are marked "under load" wherever they appear.
7. **The probe's variants are scratch sources.** They are reproducible from the committed patches and Ninja file (`records/performance/cost-cleanup-1-0-1/probe/`), so the figures are reproducible (V10), but the variants themselves are not committed.
8. **Not verified here:** the cost of the restructured units (it is U1's and U2's first measurement); the cause of the superlinear cost in section 3.1 (inferred, no time trace); whether plans 0044 and 0045 touch the files of section 9 (0044 is not on any branch yet; 0045's worktree exists at `c95fee5` with no commit).
9. **Checks of this commit.** Records only (P8: documentation; no tests). The citation check reads code paths, not `docs/agent/`, so it cannot be affected; it was not run.

## 2. Goal and definition of done

Cut the development cost of the two costliest test units and close the cheap tool findings, changing no test's meaning and no public interface.

Done when: (1) `identities_rounding_test.cpp`'s work is spread over several sources of one program, with every assertion, every draw and their order kept, shown equivalent by section 5's evidence, and the clang-measure tree's build cost recorded before and after; (2) the same for `transform_identities_test.cpp`, with gcc-debug's peak per unit recorded before and after; (3) `tools/measure` marks a run dirty if its tree changed at any point between start and end, with a test; (4) `tools/mutate` runs CTest on the shared pool, with a test and a before-and-after run; (5) the cheap findings of section 7 closed and the rest restated as known limits; (6) the m6 anomaly and the label anomaly recorded with what was found; (7) all of it merged into `main` once, after one P9 matrix and the reviews of section 8; (8) the outcome recorded here, and the friction lines plan 0045 needs handed to it.

## 3. What the records and the probe show

### 3.1 `identities_rounding_test.cpp` under clang-measure

At the release revision the clang-measure tree took 667.4 s (configure 0.6, build 603.0, suite 63.8); this unit took 602.5 s and 1.81 GiB, and the build's wall time is its compile plus the link (release record, friction item 2). With `GALOOT_FORCE_INLINE` 0 the same unit takes 120 G instructions on clang-release against 4,275 G forced (`pre-release-core-pass/opt-out/`).

The probe (`records/performance/cost-cleanup-1-0-1/README.md`) compiled each of the twelve checks of the test case alone, and the laws of the two costly checks one at a time:

| Part | Inst. (10⁹) | Peak (MiB) | Wall, under load (s) |
| --- | ---: | ---: | ---: |
| whole unit | 4,272.8 | 1,855 | 639.4 |
| s1, associativity in conformal(3) (two laws) | 1,691.9 | 1,573 | 276.5 |
| the same, one law at a time (⟑, ⟇) | 301.2 + 307.7 | 623, 625 | 41.1, 41.6 |
| s3, De Morgan in conformal(3) (eight laws) | 2,450.6 | 1,596 | 389.9 |
| the same, one law at a time | 4 × 95.1 + 4 × 20.8 | 381 to 384, 266 | 12.1 to 12.9, 2.1 to 2.9 |
| the other ten checks together | 199.9 | 252 to 374 | 3.2 to 28.2 each |
| headers and test frame alone | 8.6 | 252 | 2.3 |

Two findings decide the design:

1. The sections add up (0.7%), so putting them in separate sources only spreads the same work: the build would wait for s3, about 390 s.
2. Within s1 and s3 the cost does not add up: one law at a time costs 36% (s1) and 19% (s3) of the laws together. Each law in a function of its own, compiled where nothing inlines it into the others, takes the unit's parts to about 1,328 G (31% of the whole), the largest one law at 308 G and about 42 s under load. The cause is inferred, not traced: the laws of one section are instantiated in one function body, which Clang's `-O2` pipeline optimizes as a whole with the kernels forced inline.

### 3.2 `transform_identities_test.cpp` under gcc-debug

At the release revision it peaks at 6,214,900 KiB (5.93 GiB) in 92.3 s. The next units of the same tree are `identities_four_test.cpp` (5,069,500 KiB, 4.83 GiB), `identities_five_diagonal_test.cpp` (3.00 GiB) and `identities_rounding_test.cpp` (2.68 GiB) (`compile.tsv` at `7276f826533f`).

The probe compiled each of its 25 forms alone. Cost and peak memory both add up over the forms (1.3% and 1.2%): at `-O0` GCC holds every form's code until the end of the unit. Headers and frame alone take 166 MiB; each form adds:

| Forms | Added peak (MiB) | Added inst. (10⁹) |
| --- | ---: | ---: |
| n = 2, five forms (f0 to f4) | 27 to 35 each, 151 in all | 17.6 in all |
| n = 3, six forms (f5 to f10) | 37 to 100 each, 513 | 76.1 |
| n = 4, `rigid<3>`, `R<4,0,0>`, `R<2,2,0>`, `R<1,3,0>`, `conformal<2>` (f11 to f15) | 145 to 270 each, 1,220 | 205.5 |
| n = 4, the three Gram forms (f16 to f18) | 257, 145, 668; 1,070 | 182.9 |
| n = 5, `rigid<4>`, `conformal<3>` (f19, f20) | 570, 765; 1,335 | 246.9 |
| n = 5, `R<4,1,0>` (f21) | 747 | 137.2 |
| dense form of five | about 800 | about 103 |
| `R<8,0,0>`, `rigid<7>` (f23, f24) | about 0 | 31.0 |

So a split into five sources of contiguous forms, balanced as in section 4.2, would peak at about 1.7 GiB per unit (inferred from the additivity), and gcc-debug's largest unit would then be `identities_four_test.cpp` at 4.83 GiB, which only owner item O2 (b) reduces.

### 3.3 What the split does not change

Both friction items in the release record are about users: a user's debug unit with many operations can need several GiB, and Clang at `-O2` with the forced inlining compiles operation-heavy units slowly. The split moves the project's own test cost; a user's unit with the same operations costs the same. The friction lines stay, restated with the per-form and per-law figures above (handed to plan 0045). Reducing the cost for users, for example a different default for `GALOOT_FORCE_INLINE` at `-O0`, changes configuration behavior and is out of scope for 1.0.1 (owner item O3).

## 4. Findings on the tools (each checked as stated)

### 4.1 `tools/timed` and `tools/measure`: when the dirty mark is read

`tools/measure` (`main`, line 530) calls `revision_and_branch()` once, after the measured command returns. A run that starts on a clean tree and ends after an edit is marked dirty, and one that starts dirty and ends after a commit is marked clean, with the new revision. The run log shows the first case: row 21715 (`test clang-debug all`, started 10:06:23Z, 64.2 s, so ended about 10:07:27Z) reads `81c923136055-dirty`, although it tested the tree that row 21711 built clean. The edit that made the tree dirty came between 10:07:01Z and 10:07:14Z: row 21713 (a lint build that ended at 10:07:01Z) is clean and row 21714 (10:07:14Z) is dirty. The same function marks the compile rows of `compile.tsv`, so a long build is open to the second case too. The exclusion of `docs/agent/records/performance/` from the mark is deliberate and documented in that directory's README; it stays.

### 4.2 The `convention-trace` exit anomaly (the opt-out review's m6)

Rows 21709 and 21710 at `81c923136055`: `check - convention-trace` exit 1 in 0.166 s, then exit 0 in 0.267 s, both at 10:06:20Z. Checked on a `git archive` of that revision in scratch: the trace gate exits 0 three runs out of three (0.52 to 0.55 s under load) and prints "items with no test: 6.1, 6.2, 6.3, 6.4", as at the release. The tool exits 1 for a usage error (too few arguments), for a missing path (run from `test/`, a `FileNotFoundError`) and for a wrong last argument; each of those exits in 0.03 to 0.05 s, faster than the recorded 0.166 s. So the revision's trace passes, and nothing in the tool makes the same command fail and then pass. The failing row is consistent with a different command (one that scanned part of the tree and found a finding, given its 0.166 s), but the run log does not record commands, so the cause cannot be settled. Proposed closure (owner item O5 (a)): record in this plan's outcome that the row is unexplained, that the revision's trace passes three runs of three, and that the evidence does not show a rerun to get a pass (P10). Recording each run's command in the log would settle such cases, but it changes the run log's columns, `tools/runlog` and the history, which is not cheap; it is not proposed.

### 4.3 `tools/mutate` and CTest

`tools/mutate` builds with `ninja -k 0` on the pool and then runs the selection with `ctest ... --tests-from-file <list> --output-on-failure`, serially. Its comments already treat the order of CTest's results as arbitrary ("As sets: CTest and Ninja report in the order their jobs finish"), and it already runs only the tests whose programs exist, which is the guard lesson 0063 asks for (CTest under the pool keeps a slot for a test whose program is missing). Adding `--parallel` with no number makes CTest take one pool token per test (research note 0002, CTest job server integration).

## 5. How the splits are shown equivalent

The test's meaning is its assertions, its inputs and the order in which it draws them from the seeded source. Both units draw everything from one `std::mt19937_64` passed by reference, through `std::uniform_int_distribution`, whose number of engine calls per draw is not fixed; so a split that reseeded, or skipped draws by counting them, would change the inputs. The design keeps one test case driving one source, as `identities_test.cpp` and `identities_checks.hpp` already do for `core.identities`:

- The test case, the seed, the engine and every draw stay in the driver source, in the same order. The checks move, unchanged, into functions with external linkage in other sources, declared in a test header; each function takes the drawn values (or the engine, where a check draws inside itself, as `check_identities` does) and the tally by reference.
- In `identities_rounding_test.cpp` the per-sample loops of s1 and s3 stay in the driver: it draws `a`, `b` (and `c`), then calls one function per law in the order the laws appear now. Nothing is drawn between the laws, so the sequence of draws is the same.
- In `transform_identities_test.cpp`, `check_all_forms` stays in the driver and calls one function per group of forms, in the order of the form lists now.

Evidence, each before (at the base revision) and after, on gcc-debug and clang-measure:

| Check | What it shows | Pass criterion |
| --- | --- | --- |
| CTest listing: `ctest -N -R '^core\.(identities_rounding\|transform_identities)$'` and each test's labels | the registration | one test each, the same labels |
| doctest `--count` and `--list-test-cases` | the test cases | the same names and number (1 and 2) |
| doctest's summary line at seed 0 and `--rand-seed=<k>`, k = 1 to 50 | the assertions run | the same "assertions: N" for each seed |
| the rounding test's `MESSAGE` line (coefficients compared, those with at most one rounding a side, the largest ratio of \|L - R\| to the bound) at the same 51 seeds | every coefficient of every identity, over double, bit for bit | byte-identical output |
| a scratch probe that prints the engine's state after the test case's last draw (`operator<<` of `std::mt19937_64`), in uncommitted copies of the driver before and after | the number of draws | equal state for the 51 seeds |
| `tools/mutate` on `records/mutations/0029-identities/transform_bound.toml` (selection `^core\.transform_identities$`), and a spec of the six faults of `faults_identities.toml` restricted to `^core\.identities_rounding$` (new, `records/mutations/0043-cost-cleanup/`) | the tests' power to fail | each mutant's verdict and killing test unchanged; object paths in the build-failure text may differ and are not compared |
| a sentinel of the comparison itself: a scratch driver with one law call deleted | that the `MESSAGE` and count checks can fail | the comparison reports a difference |
| the diff, read by the reviewer with `git diff --color-moved=zebra` | that the check bodies moved unchanged | only moves, declarations and the calls |

The `Reference:` comments stay above their test cases in the driver, so `tests.reference_markers` and the trace gate see the same markers. The raised step limit (`-fconstexpr-steps=1200000`) is set on every new source of `core_identities_rounding_test`, as on the one source now.

## 6. Units

Each unit has one implementer in its own worktree (W7). U1 to U5 are subfeatures of `feature/cost-cleanup-1-0-1`, branched from `main` after this plan merges; only the feature branch merges into `main`, once, after the matrix (P9) and the reviews. U6 is records only.

| # | Unit | Role, model | Branch | Files (only these) |
| --- | --- | --- | --- | --- |
| U1 | The rounding unit split: driver plus about six sources (rigid(3) checks; s1's ⟑ law; s1's ⟇ law; s3's four ⟑ and ⟇ laws, two per source; s3's four wedge laws; conformal(3) compositions), each law a function of its own; the equivalence evidence of section 5; the before and after records | build-expert and doctest-expert, Opus high | `feature/cost-cleanup-1-0-1--rounding` | `test/core/identities_rounding_test.cpp`; new `test/core/identities_rounding_checks.hpp` and `test/core/identities_rounding_<part>_test.cpp`; `test/CMakeLists.txt` lines 1635-1646 (the step-limit source list) and 1674-1685 (the `core.identities_rounding` block); `records/performance/cost-cleanup-1-0-1/rounding/`; `records/mutations/0043-cost-cleanup/` |
| U2 | The transform unit split: driver plus five group sources (n = 2 and 3; n = 4 named; n = 4 Gram; `rigid<4>` and `conformal<3>`; `R<4,1,0>`, dense and large), the same evidence | the same implementer, after U1 | `feature/cost-cleanup-1-0-1--transform` | `test/core/transform_identities_test.cpp`; new `test/core/transform_identities_checks.hpp` and `test/core/transform_identities_<group>_test.cpp`; `test/CMakeLists.txt` lines 2367-2375; `records/performance/cost-cleanup-1-0-1/transform/` |
| U3 | `tools/measure`: read the revision and status before and after the command; mark the row dirty if either read is dirty or the revision changed between them, and record the start revision; a test for each case, the staged case among them, written first | build-expert, Sonnet high | `feature/cost-cleanup-1-0-1--tools` | `tools/measure` (`revision_and_branch` and its call in `main`); `test/tools/measure_record_test.py`; `docs/agent/records/performance/README.md` only if its sentence on the mark changes |
| U4 | `tools/mutate`: `--parallel` on both CTest calls; a test that the call carries it; one spec run before and after for the time, with the pool read before and after | the U3 implementer, after U3 | the same branch | `tools/mutate` (`test`); `test/tools/mutate_test.py` |
| U5 | `tools/jobpool`: plan 0027's I3 (the missing-FIFO message also names a FIFO deleted by hand and says how to restart it) and plan 0028's I6 (the wrapped header line, the long docstring line) | the U3 implementer, after U4 | the same branch | `tools/jobpool` (the `status` message, line 269, and the header comment); `test/tools/jobpool_test.py` (one assertion on the message; the docstring line) |
| U6 | Records: the known-limits table of section 7; m6's resolution (section 4.2); the label survey (section 1 item 4); this plan's outcome; the friction lines for plan 0045 | coordinator or Sonnet | `records/cost-cleanup-1-0-1` | this plan (its outcome section); the review records are not rewritten (W6), so m6's and the findings' resolutions are stated here; the release record's lines reach `records/release/` only through plan 0045 |

The split sizes are proposals from the probe. U1 measures the restructured unit and may regroup the laws to balance the sources, as long as each law stays a function of its own and section 5's evidence holds.

## 7. Plans 0027 and 0028 findings accepted as known limits (decision 0248 item 8)

| Finding | What it is | Cheap to close? | Plan |
| --- | --- | --- | --- |
| 0027 I2 | a `JOBPOOL_FIFO` override that is a link, a dangling link, or a system without GNU `realpath` | no: each needs a policy for overrides nobody uses; the default path is unaffected | stays a known limit |
| 0027 I3 | `status`'s missing-FIFO message assumes the pool is stopping | yes: one message and one assertion; since plan 0036 the service exits at the next poll when its FIFO is missing, so the state is brief | U5 |
| 0027 I5 | `clients()` fails open: a failed `find` lets `stop` go ahead without `--force` | no: `find` over `/proc/*/fd` fails on every other user's process, so failing closed needs a scan that tells those apart; agents never stop the pool | stays |
| 0028 I5 | the 0.2 s wait is a bound on scheduling, not a proof | no: no deterministic test is known (the review's own words) | stays |
| 0028 I6 | a wrapped header comment line and a long docstring line | yes: comments only | U5 |
| 0028 I7 | the spike copy of the pool keeps its own design | no: a record of a spike, not rewritten (W6) | stays |

The changes of U5 reach the shared pool only when it next starts (it runs the main checkout's tool); no restart is needed or proposed.

## 8. Checks, review, and expected savings

### 8.1 Tests a change can affect (P8) and the merge checks (P9)

| Unit | P8 selection during work (one configuration unless stated) | Also |
| --- | --- | --- |
| U1 | `-L '^core\.identities_rounding$'` on gcc-debug and clang-measure (the unit is cheap to build on gcc-debug and is the cost question on clang-measure) | the cheap gates on every commit: reference markers, citations, trace gate; `tests.catalogue` if it reads the new sources |
| U2 | `-L '^core\.transform_identities$'` on gcc-debug | the same cheap gates |
| U3 | `-L '^tools\.measure$'` (`tools.measure_record`, `tools.measure_header`) | `tools/runlog check` once, since every later row comes through the changed code |
| U4 | `-L '^tools\.mutate$'` | the before-and-after spec run (section 8.3) |
| U5 | `-L '^tools\.jobpool$'` (labeled `slow`, about 62 s) | none |
| U6 | none (records) | citations and style checks of the files it changes |

Merge of a subfeature into the feature branch: its P8 selection in each configuration it can affect (U1: every preset, since the unit builds in all; U2: every preset; U3 to U5: one, the tools do not depend on the preset). Merge of the feature branch into `main`: one P9 matrix from empty trees, all eleven presets, `format-check`, the trace gate and `tools/runlog check`, named `gate:all@<revision>`, with the memory watchdog of the release matrix. The matrix is the after-measurement of U1 and U2 as well (section 8.3).

### 8.2 Review (W5)

- U1 and U2 change tests: one blind review of both together (Opus, high), given the diff, section 2's criteria 1 and 2, section 5's table and the evidence records, and the author's departures, not the author's view. It applies section 5's last row itself and reruns the `MESSAGE` comparison for a few seeds of its own choosing.
- U3 to U5 change tools: one blind review (Sonnet, high; the changes are small and each has its test), given the diff, criteria 3 to 5, and section 4.
- U6 is records only: no review (decision 0215).
- A fix after review gets a delta review (decision 0094), except a comment-only change.

### 8.3 Expected savings and how they are measured

| Item | Before (at the release revision) | Expected after | Measured by |
| --- | --- | --- | --- |
| clang-measure tree, per P9 merge | 667.4 s (build 603.0 s; unit 602.5 s, 4,275 G) | about 130 to 170 s: build bounded by the largest remaining unit, about 42 s alone under load, plus link; suite unchanged at about 64 s, bounded by `tools.jobpool` (inferred) | the matrix's `gate:all` rows and the tree's `compile.tsv` (wall, peak, instructions per unit), against the release matrix's rows and the probe |
| clang-measure-o3 tree, at release | 666.6 s | the same as above | the same, at plan 0045's release matrix |
| the rounding program's instructions on clang-measure | 4,273 G in one unit | about 1,330 G over all its units, about 31% (the probe's sum) | the sum of its units' rows in the tree's `compile.tsv` |
| gcc-debug largest unit | 5.93 GiB (`transform_identities_test.cpp`) | about 1.7 GiB for its largest group (inferred); the tree's largest becomes `identities_four_test.cpp`, 4.83 GiB | the gcc-debug tree's `compile.tsv` |
| gcc-debug and gcc-asan builds | 92.3 s for the unit (gcc-asan 151.3 s, 4.73 GiB) | the unit's groups compile in parallel; total instructions about the same, plus about 3 G of headers per new source | the same rows |
| mutation runs | serial CTest per mutant | the selection's tests at once, bounded by the pool | `tools/mutate` on `records/mutations/0029-identities/faults_identities.toml`, gcc-debug, before and after U4: the `test` rows' wall times; the pool's free count before and after |
| dirty mark | read at the end | read at both ends | U3's tests; no time cost beyond one `git status` per run (about 10 to 50 ms, measured by U3 in its test log) |

The before figures that the after figures meet are the release matrix's (`runs.tsv` rows 21905 to 21937, shared mode) and the probe's (shared mode, under load). The comparison uses instructions and peak memory; wall times are reported, and compared only as "under load" (P14). The criterion, fixed now: U1 succeeds on cost if the clang-measure build of the rounding program's units totals less than 50% of 4,273 G and no unit exceeds 500 G; U2 succeeds if no group of `transform_identities` exceeds 2.5 GiB on gcc-debug. Missing a criterion is reported with its cause, not hidden; the split still merges if section 5's evidence holds, since it costs nothing in meaning.

## 9. Order, dependencies, and files shared with other plans

1. Claim: this plan merged into `main` by the coordinator.
2. `feature/cost-cleanup-1-0-1` from `main`; `--rounding`, `--transform` and `--tools` from it.
3. In parallel: U1 then U2 (one implementer, since both edit `test/CMakeLists.txt`); U3, U4, U5 (another).
4. Each subfeature merges into the feature branch after its P8 checks; the second to merge first merges the feature branch in.
5. Reviews (section 8.2); fixes and delta reviews.
6. Merge `main` into the feature branch if it moved; the P9 matrix from empty trees; merge into `main`.
7. U6 after the merge, with the matrix's figures; then hand the friction lines to plan 0045.

Files two plans may touch (to be rechecked by the coordinator when plans 0044 and 0045 are committed):

| File | This plan | Other plan | Rule |
| --- | --- | --- | --- |
| `test/CMakeLists.txt` | U1 (lines 1635-1646, 1674-1685), U2 (2367-2375), O2 (b) (the `core.identities` block, about 1648-1672) | 0044, likely: tests for the header hygiene items (for example the two `apply` overloads' compile-fail sources, a lint test of user names) | different regions; the later branch merges the earlier first; neither reformats the file |
| headers under `include/galoot/core/` | none | 0044 | a 0044 change to a header these units include changes their compile cost: U1 and U2 take before and after on the same header revision, and the merge matrix is the after |
| `records/release/release-record.md`, the performance history, `runs.tsv` | none edited, except the run log's appended rows | 0045 | this plan hands its figures to 0045 for the friction lines and the P9 release row; it writes none of them |
| `CMakeLists.txt`, `CMakePresets.json`, `VERSION`, `README.md` | none | 0044 (the cache option's validation) and 0045 (the version) | none shared |
| `tools/measure`, `tools/mutate`, `tools/jobpool`, `test/tools/` | U3 to U5 | none known | none |

The matrix of step 6 and plan 0045's release matrix can be one run only if this plan merges before 0045's release commit; otherwise this plan's matrix stands for its merge and 0045 runs its own. The coordinator orders the merges of 0043 and 0044 so that one matrix serves both where they are ready together (saves about 50 minutes of machine time).

## 10. Risks

1. **The superlinear cost may not split as cleanly in the real unit.** The probe kept one law per variant in one function; the real unit has several law functions per source. If a source's cost exceeds the sum of its laws, the functions are being inlined into one another: U1 then keeps one law per source, or marks the law functions `[[gnu::noinline]]` in the test, a test-only attribute, recorded as a choice.
2. **A split that changes the order of draws** would pass every assertion and still change the test. Section 5's byte comparison and engine state detect it; the reviewer reruns them.
3. **Rounding differences between inlined and non-inlined code.** The double results depend only on the order of operations, which the generated kernels fix, and the test build has contraction off; so they should be the same bit for bit (inferred). The `MESSAGE` comparison shows it; a difference is reported as found, not explained away.
4. **More sources, more link and parse cost.** About 3 to 9 G of instructions per new source (the probe's frame figures); small against the savings.
5. **`--parallel` in `tools/mutate` and lesson 0063.** The tool already runs only tests whose programs exist; U4 reads the pool before and after its runs and reports any lost slot.
6. **The dirty mark gets stricter.** Runs during which an agent edits a tracked file now read dirty; that is the point. A gate run that reads dirty for that reason must be rerun on a still tree; that is P10's "unchanged revision" only once the tree stays still.
7. **The other session's load** may slow the matrix; it changes no instruction count or peak.

## 11. Owner decisions (each with a default if not ruled)

- **O1. Restructure the rounding unit by law, not only by section.** (a) One function per law of s1 and s3, in about six sources, as section 6 says; (b) only split by section, which leaves about 390 s on the critical path; (c) turn the forced inlining off for this unit on the Clang optimized presets (`GALOOT_FORCE_INLINE` 0 on its sources), which costs nothing in code but removes the only check of these identities with Clang, forced inlining and `-O2` together. Tradeoff: (a) saves about 450 to 500 s per merge and 69% of the unit's instructions, and keeps every configuration; (b) saves about 210 s; (c) saves the most and drops coverage. Recommendation: (a). Default: (a).
- **O2. The gcc-debug peak.** (a) Split `transform_identities_test.cpp` only, as the brief asks; the tree's largest unit becomes `identities_four_test.cpp` at 4.83 GiB; (b) also split `identities_four_test.cpp` into two group sources the same way (one more source under `core.identities`, `test/core/identities_checks.hpp` and the `core.identities` block of `test/CMakeLists.txt`; about 100k more tokens, no more machine time beyond the matrix), so that the largest unit is about 3.0 GiB (`identities_five_diagonal_test.cpp`). Tradeoff: (b) gives the reduction the friction line points at; (a) is narrower. Recommendation: (b), as a U2 addendum. Default: (a), since (b) is outside the brief's letter.
- **O3. Users' cost.** Both friction items stay true for users after this plan. A default of `GALOOT_FORCE_INLINE` that follows the optimization level (off at `-O0`) would address item 1, but it changes configuration behavior and needs a decision, tests and documentation. Recommendation: no change in 1.0.1; consider it for 1.1.0 with a measurement. Default: no change.
- **O4. The dirty-mark gap.** (a) Fix the read-at-the-end gap and pin the staged case with a test, closing decision 0241 item 9; (b) tell the coordinator what the "index differs from HEAD" observation was, and rescope. Recommendation: (a). Default: (a).
- **O5. Records-only closures.** (a) m6: recorded as unexplained, the revision's trace passing three runs of three, with no evidence of a rerun to get a pass (section 4.2); (b) the CTest label anomaly: closed as "no record and no reproduction" with the survey of section 1 item 4, unless the owner gives the observation; (c) the known-limits table of section 7. Recommendation: accept all three. Default: (a) to (c).

Only the owner can: rule O1 to O5; approve a `runs.tsv` compaction (none proposed); merge to `master` and tag (plan 0045).

## 12. Agent cost estimate (W3)

| Work | Model | Tokens |
| --- | --- | --- |
| This plan | Opus | not visible to me; see the hand-back |
| U1 and U2: restructuring, evidence campaign, records | Opus, high | 300k to 450k |
| U3 to U5 | Sonnet, high | 80k to 140k |
| Blind review of U1 and U2 | Opus, high | 150k to 250k |
| Blind review of U3 to U5 | Sonnet, high | 50k to 80k |
| U6 and the coordinator's merges | Sonnet or coordinator | 40k to 80k |
| Subtotal | | 0.62M to 1.0M |
| With the history's multiplier of about 1.5 for fixes, delta reviews and reruns | | about 0.9M to 1.5M |

Machine time, inferred: U1's before and after evidence about 20 minutes (the clang-measure unit compiles twice at about 10 minutes before, a few after; 51 seeds of a 0.2 s program on two presets); U2 about 10 minutes; the mutation runs about 20 minutes; the feature matrix about 40 minutes, less than the release matrix's 48 since the clang-measure trees shrink by about 8 minutes each.

## 13. Next step

The coordinator merges this plan into `main` and reports O1 to O5 to the owner; on the rulings, U1 and U3 start.

## 14. Outcome

Written 2026-11-21 by Claude (agent, Sonnet 5.5), unit U6, on `records/cost-cleanup-1-0-1` from `main` at `8db4b0b`, from the merge gate's record (`records/gates/2026-11-21-cost-cleanup-1-0-1-matrix.md`), the two review records, the units' own records under `records/performance/cost-cleanup-1-0-1/` and the owner's rulings of decision 0252. Records only: nothing under `include/`, `test/`, `tools/`, `cmake/` or the build files changed. I read the matrix record's figures against the committed compile rows (`matrix/scripts/units.py` on `compile-clang-measure.tsv` and `compile-gcc-debug.tsv`) and the run log's rows for the matrix, and they agree; the other figures below are quoted from the records named, not remeasured.

### Departures and readings (C20)

1. **Row numbers.** The matrix record cites `runs.tsv` rows 22388 to 22420. Those are line numbers in the feature worktree's copy of the log; after the merge into `main` the same 33 rows (selection `gate:all@05229a7`, all at revision `05229a7b086c`, none dirty) are lines 22406 to 22438, and their wall times sum to 1,524.94 s, as the record says. The record is not rewritten (W6); this plan cites the selection, which does not move. The release matrix's rows 21905 to 21937 are the same lines in both, and sum to 2,883.56 s.
2. **The label survey was run again, not recovered.** Section 1 item 4 reports a survey at `c95fee5` but kept no script or output. I repeated it on `main` at `8db4b0b`, in two configured trees, and kept the script, the label lists and the output in `records/verification/0043-ctest-label-survey/` (it needs configure steps only, 0.55 s each, no build). The totals equal the plan's (680 and 672). Four run-log rows were appended for it (selections `survey:ctest-labels` and `survey:ctest-list-preset-filter`).
3. **No change to plan 0045 beyond one line.** Section 6 says the release record's lines reach `records/release/` only through plan 0045. The friction lines below are this plan's deliverable for that plan's unit R7; I added one dated line to plan 0045's outcome section pointing here, and edited nothing else of it.
4. **The two review findings that need a code change are not made here.** M1 (a comment in `test/core/transform_identities_up_to_three_test.cpp`) and F1 (a docstring wrap in `tools/mutate`) are outside this unit's files (records only). They stay open, below.
5. **Not verified here:** anything the gate or the reviewers did not run, as their records say. No benchmark ran (a release step). No agent cost is visible to me; the coordinator records each subagent's cost (decision 0215). The plan's cost estimate (section 12) is therefore not compared with an actual below, except where the records give machine time.

### Units and where they landed

| Unit | Result | Commits and records |
| --- | --- | --- |
| U1 rounding split | `identities_rounding_test.cpp` became a driver and five law sources, each of the ten laws a function template of its own, instantiated for rigid(3) and conformal(3); the checks moved unchanged into `identities_rounding_checks.hpp`; section 5's evidence held (listing, test cases, summaries and `MESSAGE` lines at 51 seeds, engine state, six mutation verdicts, sentinel). Rulings: 0252 item 1, option (a) | code `7daa3b0`; `records/performance/cost-cleanup-1-0-1/rounding/`, `records/mutations/0043-cost-cleanup/` |
| U2 transform split | `transform_identities_test.cpp` became a driver and five group sources; and, as ruled (0252 item 2, option (b)), `identities_four_test.cpp` became two group sources; the same evidence | code `2628936`; `.../transform/` |
| U3 dirty mark | `tools/measure` reads the revision and the tree before and after the command; a row is `-dirty` if either read is, or if HEAD moved, and carries the start revision. Cases written first, the staged case among them (0252 item 4) | code `56a3d21`; tests `b3fc275`, `7fcd913`, `6d35aa8` |
| U4 `tools/mutate` | `--parallel` on the one CTest call that runs tests | code `028fa4c`; test `f495093` |
| U5 `tools/jobpool` | plan 0027's I3 and plan 0028's I6 | code `080c3c2`; test `8305ba8` |
| U6 | this section, the label survey | this branch |

U3 to U5 are the tools subfeature (tip `8f48b32`, merged as `0ad599e`); the rounding and transform subfeatures merged as `bbc75cb` and `1db53c8`; `main` was merged in (`05229a7`, records only since the branch's base `01dce7f`). Reviews (W5): the blind review of U1 and U2 (Opus 5.5) and of U3 to U5 (Sonnet 5.5), both approve, with no blocking finding; their records are `records/reviews/2026-11-21-plan-0043-u1-u2-blind-review.md` and `2026-11-21-plan-0043-u3-u5-blind-review.md`. The gate ran on `05229a7`, and the feature branch merged into `main` as `26cd467` (merge commit), with the run rows of the post-merge checks at `8db4b0b`.

### Definition of done (section 2), item by item

1. **Rounding spread over several sources, behavior kept, clang-measure before and after: met.** Six units; the program's cost on clang-measure fell from 4,272.5 G to 529.9 G (12.4%), and the clang-measure tree from 667.4 s to 120.2 s. The equivalence evidence is U1's, confirmed by the reviewer, who read the diff with `--color-moved`, compared the eleven `compare` calls as sets and reran the `MESSAGE` and summary comparison at five seeds of his own on a third preset (all 15 pairs identical).
2. **The same for the transform program, gcc-debug's peak per unit: met.** The largest group peaks at 1,749,812 KiB (1.67 GiB) against 6,214,900 KiB; the tree's largest unit is now `identities_five_diagonal_test.cpp` at 3,145,516 KiB (3.00 GiB).
3. **`tools/measure` marks a run dirty if its tree changed at any point, with a test: met.** The reviewer ran the new cases against the old tool: five checks failed there, and the staged case passes on both tools, as section 1 item 3 predicted. The one expected value changed (`git status` calls per recorded command, 1 to 2) is in a commit of its own (`7fcd913`) and the reviewer agreed it is right (N3).
4. **`tools/mutate` runs CTest on the shared pool, with a test and a before-and-after run: met for the code and the test; the run shows no gain.** See "Measured savings" below and the open items.
5. **The cheap findings closed and the rest restated as known limits: met.** The table below.
6. **The m6 anomaly and the label anomaly recorded with what was found: met.** Below.
7. **Merged into `main` once, after one P9 matrix and the reviews: met.** The matrix is `gate:all@05229a7`, all eleven presets from empty trees, one at a time, under the memory watchdog and the timing lock: 657 of 657 tests on six presets and 649 of 649 on five, the lint build's clang-tidy, interface header sets and format check clean, `format-check`, the trace gate ("items with no test: 6.1, 6.2, 6.3, 6.4", as at the release) and `tools/runlog check` ("1 eras match the history; 1727 kept rows present once; no removed row present") exit 0. The trace gate and the run log check were repeated on the merged tree `26cd467` and passed. The pool read 23 of 23 free before and after every step; it was running, so the gate neither started nor stopped it.
8. **The outcome recorded here, and the friction lines handed to plan 0045: met.** This section and "Friction lines for plan 0045" below.

### Measured savings (section 8.3)

All from the matrix record; wall times are shared mode under other load (P14), compared only as such. Instructions and peak memory are the comparable columns.

| Item | Before | Expected after | Measured after | Reading |
| --- | --- | --- | --- | --- |
| clang-measure tree | 667.4 s (build 603.0) | 130 to 170 s | **120.2 s** (build 55.0, suite 64.3); build instructions 9,416 G to 5,697 G (-39.5%) | better than expected; the build is now bounded by `identities_five_diagonal_test.cpp` (334.6 G, 53.4 s) and `identities_four_other_test.cpp` (257.7 G, 45.5 s) |
| clang-measure-o3 tree | 666.6 s | the same | **127.6 s** (build 63.5); 10,102 G to 6,408 G (-36.6%) | as above; measured here as well as at the release matrix |
| rounding program on clang-measure | 4,272.5 G in one unit | about 1,330 G (31%) | **529.9 G over six units (12.4%)**, largest 105.1 G | criterion met (below 50%, no unit above 500 G); agrees with U1's own figure to 0.2%; the split saved more than the probe predicted, since a law in a source of its own costs about 100 G against 301 G in the probe's one-function variant (inferred cause: the other sources cannot inline the law into the sample loop) |
| gcc-debug largest unit | 5.93 GiB (`transform_identities_test.cpp`) | about 1.7 GiB per group; the tree's largest 4.83 GiB, or about 3.0 GiB with O2 (b) | largest transform group **1.67 GiB**; the tree's largest **3.00 GiB** | criterion met; O2 (b)'s expectation held. The largest process fell on every preset (gcc-asan 4,961,752 to 2,486,984 KiB; clang-asan 2,814,684 to 1,565,468; gcc-release 2,052,320 to 879,692; the others by 32% to 66%) |
| gcc-debug and gcc-asan builds | 95.8 s and 164.5 s | groups in parallel; instructions about the same plus about 3 G per new source | 76.4 s and 131.0 s; the transform program's six units 1,002.2 G against 981.7 G (+2.1%) | wall and peak fell as planned; gcc-asan's build instructions rose, see below |
| mutation runs | serial CTest per mutant | the selection's tests at once | **not measured** | the before and after rows (`db3de04`, `c645e69`) were taken while other sessions held the pool at 0 of 23 slots free and the machine was loaded; wall about equals user time in every `test` row, so no overlap shows, and the after run was slower. The reviewer's private-jobserver check shows the mechanism works (four 2 s tests ran in 4.0, 4.0 and 2.0 s under `make -j2`, `-j3` and `-j4`). The saving is unmeasured, not absent |
| dirty mark | read at the end | read at both ends | every row of the matrix reads `05229a7b086c`, none dirty | the extra `git status` costs about 12 ms a row (the tools reviewer's measurement) |
| whole matrix wall time | 2,883.6 s (48.1 min) | about 40 min (section 12) | **1,524.9 s (25.4 min)** | |

#### Unfavorable, as found

- **Build instructions rose on nine of eleven presets**, by 0.7% (gcc-measure-o3) to 10.4% (gcc-asan, 10,618 G to 11,722 G); lint rose by 1,024 G (+3.5%). Over the whole matrix the net is a saving of about 4,140 G: the two clang-measure trees save 7,414 G, and the other nine add 3,273 G.
- **The rounding program grew by 28.9% on gcc-debug (258.8 G to 333.5 G) and 79.5% on clang-debug (107.9 G to 193.7 G).** For the other eight presets there is no before figure for the program (no compile rows exist at `01dce7f` for gcc-release, gcc-asan, clang-release, clang-asan, gcc-measure, gcc-measure-o3 and clang-measure-o3). By the gate's recommendation, which the owner's acceptance of all recommendations covers, this outcome states the tree-level bound instead: what the two splits together added to a whole build is at most the figures above (+0.7% to +10.4%). The Clang rows also hold part of the move from Clang 23.1.4 to 23.1.5, which is not separated.
- **The cause is inferred, not traced:** each new source parses the headers and repeats the shared instantiations, and each law source instantiates its law for both algebras. gcc-asan's rise of 1,104 G is larger than that predicts from gcc-debug's 104 G; the instrumentation may make the repeated instantiations dearer. No time trace was taken. The gate recommends recording this as found and leaving the trace to a later cost pass, since gcc-asan's wall time and peak both fell.
- **Machine time ran well over the estimate.** The plan estimated 20 minutes for U1's evidence, 10 for U2's and 20 for the mutation runs. U1's record spans 17:02 to 19:10 UTC and U2's 19:14 to 21:12, mostly mutation runs under heavy load from other sessions (the blind review of U1 and U2, N5, puts the before runs at about 84 minutes for U1 and 2 h 10 min for U2; both units disclose it). The matrix itself took 25.4 minutes.

### Known limits (section 7, final state; decision 0248 item 8, ruled again by decision 0252 item 5 (c))

| Finding | Final state | How |
| --- | --- | --- |
| 0027 I2: a `JOBPOOL_FIFO` override that is a link, a dangling link, or a system without GNU `realpath` | **known limit**, unchanged | each needs a policy for overrides nobody uses; the default path is unaffected |
| 0027 I3: `status`'s missing-FIFO message assumes the pool is stopping | **closed** | U5 (`080c3c2`): the message names a FIFO deleted by hand and says to start a new pool with `tools/jobpool ensure` once status says "not running"; the case `status_without_fifo` fails on the old tool and passes on the new (reviewer). The change reaches the shared pool only when it next starts; no restart was made or is proposed |
| 0027 I5: `clients()` fails open | **known limit**, unchanged | a failed `find` over `/proc/*/fd` lets `stop` go ahead without `--force`; failing closed needs a scan that tells other users' processes apart; agents never stop the pool |
| 0028 I5: the 0.2 s wait bounds scheduling, it does not prove it | **known limit**, unchanged | no deterministic test is known |
| 0028 I6: a wrapped header comment line and a long docstring line | **closed** | U5: the header line is reflowed; the reviewer found no line over 79 columns in `test/tools/jobpool_test.py` at `01dce7f` or now, so the author's "did not reproduce" holds for the docstring |
| 0028 I7: the spike copy of the pool keeps its own design | **known limit**, unchanged | a record of a spike, not rewritten (W6) |

New known limits found by this plan's reviews:

| Finding | State |
| --- | --- |
| Tools review N4: `tools/mutate` and `tools/measure` mark a tree dirty by different rules (`tools/mutate` excludes only its own output directory and the run log from `+dirty`; `tools/measure` excludes all of `records/performance/`) | known limit, reported: not in this plan's files; U2's after run of `faults_core_identities` reads `+dirty` because of it, harmlessly. For the tools backlog |
| Tools review F2: the `tools/mutate` saving is unmeasured | known limit; a quiet-machine repetition under the lock, about 10 minutes, would settle it |
| Tools review F1 (a docstring paragraph of `tools/mutate`: a 96-column line, a parenthesis that now reads as explaining the wrong clause) | open, minor; a comment-only change to `tools/mutate`; suggested fix: put "in parallel on the pool" after the parenthesis and reflow |
| Tests review M1 (a comment in `transform_identities_up_to_three_test.cpp` names a degenerate non-diagonal form that none of its eleven forms is; that form is in `transform_identities_four_gram_test.cpp`) | open, minor; a comment-only change; fix: drop the phrase from that comment |

### The records-only closures (decision 0252 item 5)

**(a) The `convention-trace` exit anomaly, the opt-out review's m6: recorded as unexplained.** In `runs.tsv`, lines 21709 and 21710 are two `check - convention-trace` rows at revision `81c923136055` (branch `feature/pre-release-core-pass--inline-opt-out`), both started at 2026-11-11T10:06:20Z: the first exits 1 in 0.166 s, the second exits 0 in 0.267 s. On a `git archive` of that revision, in scratch, the trace gate exited 0 three runs out of three (0.52 to 0.55 s under load) and printed "items with no test: 6.1, 6.2, 6.3, 6.4", as at the release (plan section 4.2). The tool exits 1 for a usage error, a missing path and a wrong last argument, each in 0.03 to 0.05 s, faster than the recorded 0.166 s; nothing in the tool makes the same command fail and then pass. So the revision's trace passes, and the failing row is consistent with a different command (one that scanned part of the tree and found a finding), but the run log does not record commands, so the cause cannot be settled. The evidence does not show a rerun to get a pass (P10): the two rows share a selection and a start second, no tracked file is recorded as changing between them, and the passing row is not marked as a rerun of a failed gate; the log cannot say more. Every trace-gate row after those two (21 rows after line 21710, including `gate:trace@05229a7` and the merged tree's) exits 0. Recording each run's command in the log would settle such cases, but it changes the log's columns, `tools/runlog` and the history; not proposed. Closed, as ruled.

**(b) The CTest label anomaly: closed as "no record and no reproduction".** No record in the repository says what it was (plan 0042 section 1 item 3, and a search of `docs/agent` for it finds only the backlog lists that name it). The survey of every test's labels, in a fresh gcc-debug tree (680 tests) and a fresh clang-measure tree (672), at `c95fee5` and again at `8db4b0b` (`records/verification/0043-ctest-label-survey/`), finds no test without its module label, none without a `<module>.<component>` label, none with a label twice, none without a module prefix; the presets' filter `^(runlog|bench)$` leaves 657 and 649, the release matrix's counts. The survey covers two of the eleven presets and cannot show an anomaly of another kind; if the owner remembers what it was, that reopens the item. Closed, as ruled.

### Findings of the reviews and their resolutions (W6: the review records are not rewritten)

| Review finding | Resolution |
| --- | --- |
| U1 and U2, M1: a comment names a form its list lacks | open, comment-only; see the table of limits and the open items |
| U1 and U2, M2: the rounding split raises total compile work on every preset except the clang-measure ones, and the records showed one | recorded above: the matrix gives the program's after figure on all ten compiling presets and the before figure on three; the tree-level bound stands for the rest (owner item 1 below) |
| U1 and U2, N1: criterion 1's tree-level after figure waits for the matrix | met by the matrix (`gate:all@05229a7`) |
| U1 and U2, N2: the raised constexpr step limit is set on two sources that do not need it | accepted as written; the CMake comment gives the reason, the flag changes no result |
| U1 and U2, N3: U2 wrote mutation specs outside its listed files | accepted; unchanged copies of the 0029 specs with the presets and targets narrowed, disclosed by U2 |
| U1 and U2, N4: the mutate and measure dirty rules differ | known limit, above |
| U1 and U2, N5: machine time far over the estimate | recorded above |
| U3 to U5, F1 | open, comment-only, above |
| U3 to U5, F2 | recorded above: the saving is unmeasured |
| U3 to U5, N1, N2 (the readings of departures 4 and 5), N3 (the changed expected value), N4 (`status_without_fifo` depends on a 30 s poll interval) | agreed or accepted by the reviewer; no action |

### What stays open

1. **The rounding program's before figures on seven presets.** (a) Build the program at `01dce7f` on those presets, about 20 minutes under the timing lock, mostly the clang-measure-o3 and gcc-asan compiles of the one large unit. (b) State the tree-level bound, as this outcome does. Tradeoff: (a) gives the per-program rise on every preset and costs machine time; (b) costs nothing, and the tree rows already bound the rise at +0.7% to +10.4%; the split itself is the owner's ruling (0252 item 1) whatever the figure. Recommendation and the course taken: (b). If the owner wants the figures, (a) runs as a small unit.
2. **gcc-asan's +10.4% (1,104 G).** (a) A `-ftime-report` of one law source and of the old unit under gcc-asan, about 10 minutes. (b) Record it as found. Tradeoff: (a) says where the work goes and may suggest a cheaper split for the sanitizer trees; (b) leaves it, with gcc-asan's wall time (228.8 to 195.8 s) and its peak (4.73 GiB to 2.37 GiB for the largest process) both falling. Recommendation and the course taken: (b), for 1.0.1; a later cost pass can trace it.
3. **The `tools/mutate` saving.** A repetition of the gcc-debug spec on a quiet machine under the timing lock, about 10 minutes, would show whether tests overlap. Tradeoff: it only supplies a figure; the code and its test are right and the merge did not need it. Recommendation: do it only if a figure is wanted, for instance when the tool next changes.
4. **M1 and F1, two comment-only fixes** in `test/core/transform_identities_up_to_three_test.cpp` and `tools/mutate`. U6 is records only, so neither is made here. Options: (a) one small comment-only unit now (W5 asks no delta review for comments), (b) fold them into the next unit that touches those files, (c) leave them. Tradeoff: (a) costs a worktree, two edits, a build of one unit and a tools test run, a few minutes; (b) costs nothing but may wait long, since the tools branch is quiet; (c) leaves a comment that tells a reader a form exists that does not. Recommendation: (a) with the 1.0.1 work, since a comment that is wrong is the cheapest kind of defect to remove, and both files are in 1.0.1's window; it changes no executable line.
5. **The dirty-mark rule of `tools/mutate`** (N4). Options: make it use `tools/measure`'s rule, or leave. Tradeoff: sharing the rule removes a surprising `+dirty` in mutation records and costs a change to a tool and its test; leaving it costs nothing and the two marks mean "not a committed tree" either way. Recommendation: leave it for the tools backlog.
6. **Whether this matrix counts for plan 0045's release matrix.** The plan's rule (section 9) is that the feature matrix stands for its own merge when 0045's release commit follows it, and 0045 runs its own; the gate raised it as a coordinator question. The rule applies unless the coordinator rules otherwise, and decision 0252 item 16 rules the release matrix's shape. Recommendation: 0045 runs its own, as the rule says, since its matrix runs on the release commit and its figures are the ones the release record cites.
7. **`GALOOT_FORCE_INLINE` and users' cost** stay as decision 0252 item 3 ruled: no change in 1.0.1; considered for 1.1.0 with plan 0047's measurement (V7b).

None of 1 to 7 blocks the release; the owner's own steps (the walkthrough, the merge into `master`, the tag) are plan 0045's.

### Friction lines for plan 0045's release record (a deliverable for that plan's unit R7)

R7 writes `records/release/release-record-1.0.1.md` as a delta on the 1.0.0 record. The lines below are this plan's input to its "Known friction" and "Accepted limits of the project's tools" sections, in the form of the 1.0.0 record's lines, with the figures of the 05229a7 matrix; R7 replaces them with the figures of plan 0045's own matrix at R if they differ (the releases' matrices, not this one, are what the release record cites), and keeps the wording about what is a user's cost and what is the project's.

Known friction, restated (1.0.0 items 1 and 2) and unchanged (item 3):

1. **Debug builds' peak memory.** gcc-debug's largest compile of the project's own tests is now `identities_five_diagonal_test.cpp` at 3,145,516 KiB (3.00 GiB), against 5.93 GiB (`transform_identities_test.cpp`) at 1.0.0; plan 0043 split the test sources, which lowers the project's own peak and does not change a user's cost. A user's large debug unit can still need several GiB. Measured per form at `-O0` on GCC (the plan's probe, `records/performance/cost-cleanup-1-0-1/README.md`): a unit's peak and instruction count add over the forms it instantiates (within 1.3%), because GCC at `-O0` holds every form's code until the unit ends; a form of two or three generators adds 27 to 100 MiB, one of four 145 to 270 MiB (the Gram forms up to 668 MiB), one of five 570 to 765 MiB, and a dense form of five about 800 MiB.
2. **Clang at `-O2` with the forced inlining compiles operation-heavy units slowly.** The project's own clang-measure tree takes 120.2 s at the 05229a7 matrix (build 55.0 s) against 667.4 s at 1.0.0, and clang-measure-o3 127.6 s against 666.6 s, because `identities_rounding_test.cpp` (602.5 s, 4,272.5 G at 1.0.0) is now a driver and five law sources totaling 529.9 G, the largest 105.1 G. A user's unit is not split: the probe found that the cost within one function body grows faster than the code (two associativity laws together 1,691.9 G against 301.2 G and 307.7 G one at a time; eight De Morgan laws together 2,450.6 G against 95.1 G and 20.8 G each), which is inferred to come from the laws being instantiated and optimized in one body with the kernels forced inline, and was not traced with a time trace. A law placed in a source of its own cost about 100 G in the split. `GALOOT_FORCE_INLINE` 0 remains the user's remedy; a default that follows the optimization level is considered for 1.1.0 with plan 0047's measurement (decision 0252 item 3).
3. **Debug builds compile slower with the forced inlining** (1.0.0 item 3): not touched by plan 0043; stands as written.

The project's own cost (not seen by a user), for the performance history's notes: the test sources were split, which raised total build instructions on nine of eleven presets by 0.7% (gcc-measure-o3) to 10.4% (gcc-asan, 10,618 G to 11,722 G) and lowered them on the two clang-measure presets by 36.6% and 39.5%; the largest process of every tree fell, and the whole 11-preset matrix took 1,524.9 s against 2,883.6 s. gcc-asan's rise is not traced.

Accepted limits of the project's tools (restating 1.0.0's list):

- Job pool: plan 0027's I2 and I5 and plan 0028's I5 and I7 stay known limits (decision 0248 item 8); I3 and I6 are closed by plan 0043's U5. An unreadable `/proc/<pid>/fd` hides a client (decision 0228 item 9). The cost of `tools.jobpool` is kept: it bounds every suite at about 64 s (63 s of each).
- `tools/measure`: the dirty mark is read before and after each command, so a row is `-dirty` if the tree was dirty at either end or HEAD moved, and carries the revision the command started on; this closes decision 0241 item 9. The extra `git status` costs about 12 ms a row.
- `tools/mutate`: CTest now runs the selected tests with `--parallel` on the shared pool; the saving is unmeasured (the before and after rows were taken on a saturated pool), and its `+dirty` mark follows a different rule from `tools/measure`'s (it excludes only its own output directory and the run log). GCC reports "unjudged" where Clang says "killed" for a guard reached through a `consteval` call or a `constinit` variable of a header (decisions 0231 and 0232), as before.
- The `convention-trace` row pair of `runs.tsv` lines 21709 and 21710 (the opt-out review's m6) is unexplained: the revision's trace passes, the run log records no commands, and nothing shows a rerun to get a pass. The CTest label anomaly of the 1.0.0 backlog is closed with no record and no reproduction (`records/verification/0043-ctest-label-survey/`).
- Two comment-only findings are open unless a unit fixes them before R: `tools/mutate`'s docstring paragraph (F1) and a comment in `transform_identities_up_to_three_test.cpp` (M1).

### Cost

Agent cost: the plan row of `agent-cost.tsv` (274,727 tokens, Opus) is the planner's; the implementers', reviewers' and this unit's rows are the coordinator's to write at each hand-back (decision 0215), and none is visible to me. Machine time: the matrix 25.4 minutes, in shared mode under a load of 3 to 9; the units' evidence and mutation runs as above.
