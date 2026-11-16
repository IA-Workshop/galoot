# 0043 1.0.1: test, build and tool cost cleanup

- Status: proposed 2026-11-16 by Claude (agent, Opus 5.5), planner in the build-expert and performance-engineer roles. From `main` at `c95fee5`. Committed on `plan/0043`; the coordinator merges it into `main`, which is the claim (W3, W7). No unit below has started.
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
