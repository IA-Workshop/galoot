# The transform unit split and the forms of four: cost and equivalence, before and after

Measured by Claude (agent, Opus 5.5), implementer of plan 0043 U2 in the build-expert and doctest-expert roles, on 2026-11-21 from 19:14 to 21:12 UTC, in the worktree of `feature/cost-cleanup-1-0-1--transform`. Before: `bbc75cb` (the tip of `feature/cost-cleanup-1-0-1` when the branch was cut; the records commit `e5e9353` changes nothing under `include/`, `test/` or `cmake/`). After: `2628936`, the split.

## Question

Does spreading `core_transform_identities_test`'s forms over group sources (plan 0043 section 6, row U2), and `identities_four_test.cpp`'s forms over two group sources (owner ruling 0252 item 2, option (b) of the plan's O2), bring gcc-debug's largest unit of each program down to the plan's criterion (section 8.3: no group of `transform_identities` above 2.5 GiB on gcc-debug), while both tests keep every assertion, every draw and their order (section 5)?

## What changed

`core.transform_identities`: the two test cases and `check_all_forms` stay in `test/core/transform_identities_test.cpp`. The checks moved unchanged into `transform_identities_checks.hpp`, with internal linkage as before. The forms went to five sources, which `check_all_forms` calls in the order of the old lists, as plan section 6 proposed:

| Source | Forms |
| --- | --- |
| `transform_identities_up_to_three_test.cpp` | the eleven forms of two and three generators |
| `transform_identities_four_named_test.cpp` | rigid(3), R(4,0,0), R(2,2,0), R(1,3,0), conformal(2) |
| `transform_identities_four_gram_test.cpp` | the three forms of four given by Gram matrices |
| `transform_identities_five_rigid_conformal_test.cpp` | rigid(4), conformal(3) |
| `transform_identities_five_dense_large_test.cpp` | R(4,1,0); the dense form of five; R(8,0,0) and rigid(7) |

The fold over a list of forms that `check_all_forms` held for the small forms became `check_each` in the header; it instantiates the same `check_identities<form<G>, of_grade<n, n - 1>, n>` for each form. The dense and large forms keep their own folds, in the last group's function.

`core.identities`: `identities_four_test.cpp` became `identities_four_diagonal_test.cpp` (`diagonal_forms_of_four`: R(4,0,0), rigid(3), R(1,3,0), R(3,1,0), R(2,2,0), R(2,0,2), R(0,0,4)) and `identities_four_other_test.cpp` (`other_forms_of_four`: conformal(2), the Witt form of two modes, rigid(3) with its null generator first, and the two random forms). The two replace `forms_of_four` in the `groups` array of `identities_checks.hpp`, one after the other, so the forms are checked in the old order.

The split point of the forms of four was chosen by a scratch probe (`scripts/four_probe.py`, run-log selections `scratch:four-probe-<k>` and `scratch:four-split-*`), which compiled `identities_four_test.cpp` under gcc-debug's flags once per form and at two split points:

| Variant | Peak (KiB) | Inst. (10⁹) |
| --- | ---: | ---: |
| frame alone (no form) | 166,976 | 2.5 |
| one form alone, the seven diagonal forms | 507,220 to 832,604 | 33.7 to 65.5 |
| one form alone: conformal(2), Witt, rigid(3) null first | 832,080, 831,868, 731,540 | 65.5, 65.4, 53.8 |
| one form alone: the two random forms | 1,976,880, 1,499,476 | 172.0, 122.2 |
| first 7 / last 5 (the split chosen) | 2,379,076 / 2,858,756 | 372.9 / 463.5 |
| first 8 / last 4 | 2,262,996 / 3,249,720 | 434.3 / 402.4 |

Instructions add up over the forms (the twelve alone sum to 873 G with twelve frames, against 832 G for the unit); peak memory does not, so the split was chosen from the measured pairs: after the seventh form, the larger half peaks at 2.86 GB against 3.25 GB for the other split.

## Conditions

Shared mode, unpinned, all 32 processors eligible, timing lock held (`lock.sh`) for both measured builds, job pool exported, builds under `memguard.sh` (never fired). The before build ran at load average 5.2 to 7.2, the after build at 13.3 to 10.1 (`conditions-*.txt`), with other sessions' work on the machine. **Wall times are not comparable between the two; instructions and peak memory are** (P14). Toolchain: GCC 16.2.1, Clang 23.1.5, as for U1.

## Cost

Rows: `compile-before-<preset>.tsv` (the measured build of fresh trees, the rows between the two `conditions-before-*` times; the copy first committed in `e5e9353` also held the rows of the before mutation runs' rebuilds, taken from the tree after those runs) and `compile-after-<preset>.tsv` (the measured rebuild after `ninja -t clean` of both programs). Instructions are performance plus efficiency cores. The doctest runner's row is in both files and in neither total.

gcc-debug, `core_transform_identities_test`:

| Unit | Inst. before (10⁹) | Inst. after (10⁹) | Peak before (KiB) | Peak after (KiB) | Wall before (s) | Wall after (s) |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `transform_identities_test.cpp` | 982.5 | 2.6 | 6,215,432 | 166,432 | 73.8 | 0.5 |
| `transform_identities_up_to_three_test.cpp` | | 93.8 | | 875,888 | | 9.2 |
| `transform_identities_four_named_test.cpp` | | 197.4 | | 1,312,712 | | 17.5 |
| `transform_identities_four_gram_test.cpp` | | 184.4 | | 1,317,620 | | 16.4 |
| `transform_identities_five_rigid_conformal_test.cpp` | | 250.1 | | 1,400,840 | | 21.7 |
| `transform_identities_five_dense_large_test.cpp` | | 274.1 | | 1,749,460 | | 23.8 |
| all units of the program | 982.5 | 1,002.4 (+2.0%) | 6,215,432 | 1,749,460 (largest) | 74.0 build | 23.9 build |

clang-measure, `core_transform_identities_test`:

| Unit | Inst. before (10⁹) | Inst. after (10⁹) | Peak before (KiB) | Peak after (KiB) | Wall before (s) | Wall after (s) |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `transform_identities_test.cpp` | 962.8 | 2.5 | 1,093,972 | 184,088 | 77.6 | 0.3 |
| `transform_identities_up_to_three_test.cpp` | | 90.6 | | 350,820 | | 14.9 |
| `transform_identities_four_named_test.cpp` | | 209.4 | | 405,040 | | 17.9 |
| `transform_identities_four_gram_test.cpp` | | 140.1 | | 372,560 | | 13.0 |
| `transform_identities_five_rigid_conformal_test.cpp` | | 263.2 | | 403,176 | | 22.6 |
| `transform_identities_five_dense_large_test.cpp` | | 273.3 | | 418,300 | | 22.6 |
| all units of the program | 962.8 | 979.1 (+1.7%) | 1,093,972 | 418,300 (largest) | 77.8 build | 23.2 build |

`core_identities_test`, the forms of four (the other nine units are unchanged; their instructions are within 0.4% and their peaks within 1.9% of their before rows, inferred to be run-to-run variation):

| Preset, unit | Inst. before (10⁹) | Inst. after (10⁹) | Peak before (KiB) | Peak after (KiB) | Wall before (s) | Wall after (s) |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| gcc-debug `identities_four_test.cpp` | 832.0 | | 5,069,692 | | 74.1 | |
| gcc-debug `identities_four_diagonal_test.cpp` | | 372.6 | | 2,380,692 | | 42.4 |
| gcc-debug `identities_four_other_test.cpp` | | 463.0 | | 2,856,512 | | 51.1 |
| gcc-debug, all units of the program | 2,545.8 | 2,549.5 (+0.1%) | 5,069,692 | 3,146,228 (`identities_five_diagonal_test.cpp`) | 74.3 build | 51.3 build |
| clang-measure `identities_four_test.cpp` | 500.2 | | 1,003,752 | | 45.2 | |
| clang-measure `identities_four_diagonal_test.cpp` | | 245.2 | | 550,524 | | 23.0 |
| clang-measure `identities_four_other_test.cpp` | | 258.0 | | 606,620 | | 27.3 |
| clang-measure, all units of the program | 1,725.9 | 1,729.0 (+0.2%) | 1,003,752 | 623,388 (`identities_up_to_three_test.cpp`) | 45.3 build | 31.3 build |

Reading:

- **The criterion is met**: the largest group of `transform_identities` on gcc-debug peaks at 1,749,460 KiB (1.67 GiB; criterion: none above 2.5 GiB), against 6,215,432 KiB (5.93 GiB) in one unit. The plan's estimate from the probe's additivity was about 1.7 GiB.
- **The forms of four**: gcc-debug's larger half peaks at 2,856,512 KiB (2.72 GiB), against 5,069,692 KiB (4.83 GiB). The largest unit of `core_identities_test` on gcc-debug is now `identities_five_diagonal_test.cpp` at 3,146,228 KiB (3.00 GiB), as plan section 11 (O2 (b)) expected. With U1's split, that is also the largest unit of the gcc-debug tree, judged from the release revision's rows of the other units (`compile.tsv` at `7276f826533f`, none of the others above 2.81 GB, which was `identities_rounding_test.cpp` before U1); the feature branch's P9 matrix measures the whole tree.
- **Unfavorable, small**: the transform program's total work rose by 2.0% on gcc-debug (982.5 G to 1,002.4 G) and 1.7% on clang-measure; the identities program's by 0.1% and 0.2%. Inferred cause, not traced: each new source parses the headers and instantiates the shared helpers again; the frame alone is about 2.5 G, and five new transform sources at 2.5 G each would account for about 12 G of the 20 G.
- The program's build, bounded by its largest unit, fell from 74 s to 24 s (transform) and 74 s to 51 s (identities) on gcc-debug, under different loads.

## Equivalence (plan section 5)

| Check | Files | Result |
| --- | --- | --- |
| CTest listing and labels | `evidence-<before\|after>/<preset>/<component>/ctest-listing.txt`, `ctest-labels.tsv` | identical on both presets, both programs |
| doctest `--count`, `--list-test-cases` | `count.txt`, `list-test-cases.txt` | identical: 2 test cases (transform), 9 (identities), the same names |
| summary lines at seeds 0 to 50 (`--rand-seed=k`; 0 is the default) | `seeds.tsv` | byte-identical on both presets, both programs; also identical between gcc-debug and clang-measure, before and after |
| engine state after each test case's last draw, seeds 0 to 50 (SHA-256 of `operator<<` of the `std::mt19937_64` after each call of `check_all_forms`, 8 per run, and after the groups of each of the 9 identities test cases, from an uncommitted copy of the driver that prints them, compiled with each preset's exact flags against a `git archive` of the revision and linked with the tree's other objects) | `engine/<before\|after>-<preset>-<component>.tsv` | identical on both presets, both programs; 51 distinct hashes in each file |
| mutation verdicts: `0029-identities/transform_bound.toml`'s nine mutants (`../../../mutations/0043-cost-cleanup/transform_bound.toml`, a copy) and `faults_identities.toml`'s six faults judged by `^core\.identities$` alone (`faults_core_identities.toml`) | `mutations-<before\|after>-<spec>-<preset>.tsv` | verdicts, expectations and killers identical on both presets: `transform_bound`, seven killed by `core.transform_identities` and the two squares mutants equivalent, as declared; `faults_core_identities`, all six killed, three by a failed static assertion in `plan.hpp` and three by the test; the objects named in the build-failure text differ only where `identities_four_test.cpp.o` became one or both of the two new objects, with the same messages |
| sentinel: the after driver with one group left out (`gram_forms_of_four` in the transform driver; `other_forms_of_four` skipped in the identities driver's loop), gcc-debug | `evidence-sentinel/gcc-debug/`, `engine/sentinel-gcc-debug-*.tsv`, `sentinel-comparison.txt` | the assertion count differs at all 51 seeds (transform 9,260 against 10,728 at seed 0; identities 4,584 against 5,074) and so does the engine state; the comparison can fail |
| the diff read with `git diff --color-moved=zebra` | `git diff e5e9353 2628936` (add `-M` to see the rename of `identities_four_test.cpp`) | for the reviewer (section 8.2) |

Notes on these checks:

- Unlike the rounding test, both programs count a passing `CHECK` as an assertion. The transform test's count varies with the seed (37 distinct counts over 51 seeds), because the number of versors with lambda not zero does; the identities test's count is 5,074 at every seed, fixed by its structure. A dropped group shows in both counts, as the sentinel does.
- In the sentinel's directory the CTest listing, labels and test-case files come from the real build tree, not from the sentinel program, so their equality there says nothing.
- `core.identities` runs about 22 s per seed on gcc-debug, so the 51 seeds of every evidence step ran as one Ninja job each, drawing from the shared pool (`scripts/seed_runs.sh`), instead of one after another as in U1.

## Mutation runs' cost

| Run | `transform_bound`, both presets | `faults_core_identities`, both presets |
| --- | ---: | ---: |
| before (`e5e9353`, the code of `bbc75cb`; load average up to 30 from other sessions) | 1,489 s | 2,196 s |
| after (`2628936`; load average about 18) | 624 s | 732 s |

Each mutant rebuilds the whole program. Peak memory of the run's process tree: `memguard-mutate-<before|after>-<spec>.tsv`: 6.54 GB before and 5.85 GB after for `transform_bound`, 13.27 GB before and 15.88 GB after for `faults_core_identities` (more of the program's sources compile at once).

The after run of `faults_core_identities` records its revision as `2628936a4f52+dirty`: when it started, the two tracked files `compile-before-<preset>.tsv` of this directory had been rewritten (below) and not yet committed. No file under `include/`, `test/`, `tools/` or `cmake/` differed from `2628936`.

## Reproducing

The scripts that produced these files are in `scripts/`, as they ran, with this machine's worktree and scratch paths in them: `measured_build.sh <tag>` (the timed builds under the lock and the memory guard), `evidence.sh <preset> <tag> <component> [program]` (registration, test cases and the 51 seeds, through `seed_runs.sh`), `probe_build.py <tag> <component> <preset>...` with `probe_engine.py` (the engine-state copy of the driver; `PROBE_MAKER=sentinel.py` builds the sentinel instead), `engine_states.sh <program> <out.tsv>`, `four_probe.py <dir> [split]` (the per-form probe), and `compare.sh <dir A> <dir B>`. Each ran through `tools/timed`; its rows are in the run log under the selections `core.transform_identities:seeds-0-50`, `core.identities:seeds-0-50`, `scratch:engine-probe-*`, `scratch:engine-state-*`, `scratch:sentinel*`, `scratch:four-probe*`, `scratch:four-split*`, `scratch:one-run-*` and `scratch:after-trial*` (a trial build of the split, before it was committed, on gcc-debug and lint).
