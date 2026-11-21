# The rounding unit split: cost and equivalence, before and after

Measured by Claude (agent, Opus 5.5), implementer of plan 0043 U1 in the build-expert and doctest-expert roles, on 2026-11-21 from 17:02 to 19:10 UTC, in the worktree of `feature/cost-cleanup-1-0-1--rounding`. Before: `01dce7f` (the code of `main` and of `feature/cost-cleanup-1-0-1` when the branch was cut; the records commits `a7e64af`, `20b55e9` change nothing under `include/`, `test/` or `cmake/`). After: `7daa3b0`, the split.

## Question

Does spreading `core_identities_rounding_test`'s laws over several sources (plan 0043 section 6, row U1; owner ruling 0252 item 1, option (a)) cut its compile cost under clang-measure to the plan's criterion (section 8.3: the program's units total less than 50% of 4,273 G instructions and no unit exceeds 500 G), while the test keeps every assertion, every draw and their order (section 5)?

## What changed

The test case, its seed, its engine and every draw stay in `test/core/identities_rounding_test.cpp`. Each law of associativity (2) and each De Morgan law (8) is a function template of its own, explicitly instantiated for rigid(3) and conformal(3), in five new sources: `identities_rounding_product_test.cpp` (⟑ associativity), `identities_rounding_antiproduct_test.cpp` (⟇ associativity), `identities_rounding_complement_product_test.cpp` (right and left of a ⟑ b), `identities_rounding_complement_antiproduct_test.cpp` (right and left of a ⟇ b), `identities_rounding_complement_wedge_test.cpp` (right and left of a ^ b and of a v b). The comparison and its helpers moved unchanged into `identities_rounding_checks.hpp`. The compositions stay in the driver (the plan proposed a source of their own; section 6 lets U1 regroup, and the driver with them is 72.5 G, below every law source but the wedge one). Every source takes the raised step limit.

The rigid(3) laws go with their conformal(3) counterparts rather than into a rigid(3) source, so that each law is written once; they add little to each source.

## Conditions

Shared mode, unpinned, all 32 processors eligible, timing lock held (`lock.sh`), job pool exported, builds under `memguard.sh` (never fired). The before build ran under heavy load from other sessions (load average 6.9 at the start, 28.3 at the end; `conditions-before-*.txt`); the after build under light load (4.3; `conditions-after-*.txt`). **Wall times are not comparable between the two; instructions and peak memory are** (P14). The toolchain is GCC 16.2.1 and Clang 23.1.5; the release matrix's rows were taken with Clang 23.1.4, so this record's own before row, not the release's, is the baseline.

## Cost

Rows: `compile-before-<preset>.tsv` (fresh trees) and `compile-after-<preset>.tsv` (the six rows of the measured rebuild), from `tools/measure tu`; instructions are performance plus efficiency cores.

clang-measure:

| Unit | Inst. before (10⁹) | Inst. after (10⁹) | Peak before (KiB) | Peak after (KiB) | Wall before, loaded (s) | Wall after (s) |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `identities_rounding_test.cpp` | 4,272.5 | 72.5 | 1,912,136 | 317,240 | 760.9 | 7.3 |
| `identities_rounding_product_test.cpp` | | 100.6 | | 640,472 | | 14.1 |
| `identities_rounding_antiproduct_test.cpp` | | 105.0 | | 645,884 | | 14.7 |
| `identities_rounding_complement_product_test.cpp` | | 102.5 | | 435,108 | | 13.3 |
| `identities_rounding_complement_antiproduct_test.cpp` | | 103.7 | | 440,644 | | 13.4 |
| `identities_rounding_complement_wedge_test.cpp` | | 45.8 | | 270,148 | | 4.6 |
| all units of the program | 4,272.5 | 530.1 (12.4%) | 1,912,136 | 645,884 (largest) | 761.0 build | 15.2 build |

gcc-debug:

| Unit | Inst. before (10⁹) | Inst. after (10⁹) | Peak before (KiB) | Peak after (KiB) | Wall before, loaded (s) | Wall after (s) |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `identities_rounding_test.cpp` | 259.1 | 53.9 | 2,810,476 | 687,924 | 49.5 | 5.6 |
| `identities_rounding_product_test.cpp` | | 56.7 | | 1,001,424 | | 6.2 |
| `identities_rounding_antiproduct_test.cpp` | | 56.9 | | 1,003,068 | | 6.2 |
| `identities_rounding_complement_product_test.cpp` | | 66.9 | | 737,152 | | 6.7 |
| `identities_rounding_complement_antiproduct_test.cpp` | | 66.9 | | 736,696 | | 6.7 |
| `identities_rounding_complement_wedge_test.cpp` | | 32.4 | | 429,728 | | 3.3 |
| all units of the program | 259.1 | 333.7 (+29%) | 2,810,476 | 1,003,068 (largest) | 52.4 build | 7.2 build |

Reading:

- **The criterion is met on clang-measure**: 530.1 G is 12.4% of the before (criterion: below 50%), and the largest unit is 105.0 G (criterion: none above 500 G). The program's build, bounded by its largest unit, fell from 761 s (loaded) to 15 s (light load).
- **The split saved more than the probe predicted** (plan section 3.1: about 1,328 G, the largest law 308 G). One law in a function of its own costs about 100 G here against 301 G in the probe's variant. The cause is inferred, not traced: in the one source, and in the probe's variants, each check function was inlined into the test case with its sample count known, so Clang could replicate the loop body that holds the laws; a law defined in another source cannot be inlined into the loop.
- **Unfavorable: gcc-debug's total work rose by 29%** (259.1 G to 333.7 G), while its largest unit fell from 2.81 GB to 1.00 GB of peak memory and the build's wall time from 52 s to 7 s. Inferred cause, not traced: each of the six sources parses the headers and instantiates the shared machinery again, and each law source instantiates its law for both algebras.

## Equivalence (plan section 5)

| Check | Files | Result |
| --- | --- | --- |
| CTest listing and labels | `evidence-<before\|after>/<preset>/ctest-listing.txt`, `ctest-labels.tsv` | identical on both presets |
| doctest `--count`, `--list-test-cases` | `count.txt`, `list-test-cases.txt` | identical: one test case, the same name |
| summary lines and the `MESSAGE` line at seeds 0 to 50 (`--rand-seed=k`; 0 is the default) | `seeds.tsv` | byte-identical on both presets; also identical between gcc-debug and clang-measure, before and after |
| engine state after the last draw, seeds 0 to 50 (SHA-256 of `operator<<` of the `std::mt19937_64`, from an uncommitted copy of the driver that prints it, compiled with each preset's exact flags against a `git archive` of the revision and linked with the tree's other objects) | `engine/<before\|after>-<preset>.tsv` | identical on both presets; 51 distinct states |
| mutation verdicts: the six faults of `faults_identities.toml` judged by `^core\.identities_rounding$` alone (`../../../mutations/0043-cost-cleanup/faults_rounding.toml`) | `mutations-<before\|after>-<preset>.tsv` | verdicts, expectations and killers identical on both presets: four killed (two by a failed static assertion in `plan.hpp`, two by the test), two survived; only the object paths in the build-failure text differ |
| sentinel: the after driver with the call of `left_complement_of_antiwedge` deleted, gcc-debug | `evidence-sentinel/gcc-debug/`, `sentinel-comparison.txt` | `seeds.tsv` differs (3,022 coefficients against 3,310); the comparison can fail |
| the diff read with `git diff --color-moved=zebra` | `git diff 20b55e9 7daa3b0` | for the reviewer (section 8.2) |

Notes on these checks:

- doctest's assertion count is 2 for every seed before and after; it cannot see a dropped law, since a law's checks report through `FAIL_CHECK` only on a problem. The coefficient count in the `MESSAGE` line does see one, as the sentinel shows.
- In the sentinel's directory the CTest listing, labels and test-case files come from the real build tree, not from the sentinel program, so their equality there says nothing.
- The two surviving mutants: `one_coefficient_doubled` doubles g₁₂, which is zero in both rigid(3) and conformal(3), so it changes nothing this test computes; `order_swapped` changes the canonical order of e12 and e13, which the test, comparing blade by blade on canonical masks with their orientations, does not see. The message of commit `20b55e9` says both "change only forms the test does not use", which is right for the first and loosely put for the second.

## Mutation runs' cost

| Run | gcc-debug | clang-measure |
| --- | ---: | ---: |
| before (`a7e64af`, loaded) | 314 s | 5,039 s |
| after (`7daa3b0`, light load) | 45 s | 112 s |

Each mutant rebuilds the whole program. Before, the clang-measure baseline also rebuilt (1,608 s), because the gcc-debug restores had touched the edited headers' times. Peak memory of the run's process tree: 3.01 GB before, 4.29 GB after (the sources now compile at once; `memguard-mutate-*.tsv`).

## Reproducing

The scripts that produced these files are in `scripts/`, as they ran, with this machine's worktree and scratch paths in them: `measured_build.sh <tag>` (the timed builds under the lock and the memory guard), `evidence.sh <preset> <tag> [program]` (registration, test cases and the 51 seeds), `probe_build.py <tag> <preset>...` with `probe_engine.py` (the engine-state copy of the driver; `PROBE_MAKER=sentinel.py` builds the sentinel instead), `engine_states.sh <program> <out.tsv>`, and `compare.sh <dir A> <dir B>`. Each ran through `tools/timed`; its rows are in the run log under the selections `core.identities_rounding:seeds-0-50`, `scratch:engine-probe-*`, `scratch:engine-state-*`, `scratch:sentinel*` and `scratch:after-trial*` (a trial build of the split in a scratch copy, before it was committed).
