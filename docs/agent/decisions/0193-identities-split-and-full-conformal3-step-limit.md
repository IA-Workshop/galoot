# 0193 The identities test is split by group of signatures, and the conformal(3) `full<5>` antiproduct cases build under a raised Clang step limit

- Status: accepted (implementer decision within the owner's rulings of decisions 0184 and 0189, 2026-11-07). The route for B1 differs from the means that decision 0184's option 1 names ("by grade subsets"); see "B1: options weighed". If the owner wants grade subsets, this record is superseded and the two sources are rewritten.
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), implementer of the test split and the B1 fix, build-expert and doctest-expert roles
- Bears on: decisions 0184 (B1) and 0189 (the split); PERFORMANCE P1, P8, P9, P11; W2, W9, W10; STYLE S4, S14; VVUQ V2, V7; `include/galoot/core/product.hpp` (the documented limit of the geometric antiproduct)
- Branch: `feature/test-split`, commits `cac4bf1` (B1, product test), `4353c6d` (the identities split), and `4efe4d0` (internal linkage of the shared checks); `develop` merged at `c28fa89`

## Context

Two owner rulings of 2026-11-07, both test-only units:

1. B1 (decision 0184). Under `clang-measure` (Clang 23, `-O2`, without libstdc++'s assertions) `core_product_test` and `core_identities_test` did not build: the plan of the geometric antiproduct of two `full<5>` multivectors of conformal(3) is not a constant expression there. Measured here: the two sources that build it need between 1,090,625 and 1,092,187 steps under `clang-measure` (bisection of `-fconstexpr-steps` with `-fsyntax-only`), over Clang's default of 1,048,576. `product.hpp` states about 950,000 with the assertions on. The coordinator's brief adds: the case must still be tested on every preset, without weakening what is tested, and a build that cannot compile it must not skip it silently.
2. The split (decision 0189). At `b504aa7`, `identities_test.cpp` was one source of 295 G instructions, 20.1 s of a 20.4 s gcc-debug build and 113.5 s of a 113.8 s gcc-asan build at 3.5 GB. Every test case, label, reference marker, and the trace gate are to be kept.

## B1: options weighed

1. Grade subsets, as decision 0184's option 1 words it: check the conformal(3) identities and tables on grade-restricted layouts whose plans fit every configuration. The full `full<5>` × `full<5>` antiproduct plan, the one a user of conformal(3) builds, would then be exercised by no test. By linearity the grade pairs cover every blade pair's terms, but not the plan's own generation, merging, or ordering at that size, which is the code that hits the limit. That is a narrower test, which the brief rules out.
2. Raise the step limit for the whole of `core_product_test` and `core_identities_test`. Every other case in them would then also build under the raised limit, so a later change that pushed an ordinary plan past Clang's default would no longer fail these programs.
3. Leave out the case where the assertions are off (`#if` on `_GLIBCXX_ASSERTIONS`, or a label that the measurement presets exclude). The case would be skipped silently in those configurations.
4. Move the cases that build that plan to their own sources, each compiled on Clang with `-fconstexpr-steps=1200000` in every configuration, and keep them in the same programs, tests, and labels. **Chosen.**

Option 4 tests the same plan, with the same values and assertions, on every preset, including the four measurement presets. A configuration that cannot compile it fails the build; nothing is skipped. The other cases of both programs still build at the compilers' defaults. 1,200,000 leaves about 10% over the measured need; it is the value `product.hpp` already gives users as enough. GCC builds the cases at its defaults and gets no option.

What option 4 gives up: in the debug presets the cases used to show, as a side effect, that the plan fits Clang's default limit when the assertions are on. No test asserted it, and the limit stays documented in `product.hpp`. A change that grows the plan beyond 1,200,000 steps fails both sources loudly on Clang.

## The split: options weighed

1. One test case per group of signatures, each in its own source. It would change the test cases (three would become about twenty), their names, and their markers.
2. One source per test case. Each source would build the same products again (the three identities share most of their plans per form), so the total work would roughly triple, and no source would be much smaller.
3. Keep the three test cases in `identities_test.cpp` and move each group of signatures into its own source as a function that checks one identity over the group. **Chosen.**

Under option 3 each test case runs its identity over seven groups, in the same order and from the same seeded source as before, so a seed gives the same multivectors. Each group checks each identity on the layouts it used before and on no others. A first draft gave the dense five-generator forms both layout pairs for every identity, through a run-time switch that instantiated all of them; it raised that source from 60 G to 105 G of instructions at gcc-debug and built products of three bivectors in those forms, which their own comment says one compiler cannot evaluate at its defaults (not tried on Clang). It was corrected before any commit, and the dense group now names its layout per identity. The groups:

| Source | Forms | Layouts |
| --- | --- | --- |
| `identities_up_to_three_test.cpp` | 22 forms of one to three generators | every blade |
| `identities_four_test.cpp` | 12 forms of four | every blade |
| `identities_five_diagonal_test.cpp` | R(4,1,0), R(4,0,1) | every blade |
| `identities_conformal3_test.cpp` | conformal(3), under the raised Clang limit | every blade |
| `identities_five_dense_test.cpp` | two dense random forms of five | bivectors; vectors for the antiproduct laws |
| `identities_six_test.cpp` | R(4,2,0), R(3,3,0), conformal(4) | vectors and bivectors |
| `identities_sparse_test.cpp` | R(8,0,0), R(6,6,0), R(15,0,1), R(8,8,0) | the sparse sets |

The checks, the layouts, the group list, and `check_each` are in `test/core/identities_checks.hpp`; the product test's shared helpers are in `test/core/product_checks.hpp`. Both follow `test/core/signatures.hpp`, a named namespace under `galoot_test` with using-declarations in each source, but hold their checks in an unnamed namespace inside it, so that the checks keep the internal linkage they had in the single sources (see the build cost below). clang-tidy's `misc-anonymous-namespace-in-header` is suppressed at each of the two namespaces, with the reason beside it. The group sources end in `_test.cpp` so that the reference-marker, golden-fact, and trace scans of `tools/test_references.py` read them.

## Decision

Option 4 for B1 and option 3 for the split, as committed in `cac4bf1`, `4353c6d`, and `4efe4d0`.

## Consequences

- Test cases: the same 255 `TEST_CASE` names across the suite before and after. `core_identities_test` has 3 test cases and 2,208 assertions, `core_product_test` 17 and 243, the same as before. CTest counts unchanged: gcc-debug and gcc-release 335, gcc-asan 327, clang-debug, clang-release, clang-asan, and lint 337. `clang-measure` now builds everything and passes 329 of 329, where it passed 327 and could not build two. `core.identities` keeps the labels `core`, `core.identities`, `core.product`, `core.unary`, and `slow`.
- STYLE S14 says one file per component. The identities component now has eight source files and one shared header, and the product component two and one, by the owner's ruling to split it (decision 0189) and B1's to isolate the case (decision 0184). Each file states what it holds.
- The lint preset's clang-tidy header filter covers only `include/galoot/`, so the two new headers are not checked by the lint build itself, like the existing `test/core/signatures.hpp` and `test/support/` headers. clang-tidy with the repository's `.clang-tidy` and a header filter naming them was run by hand on every new source; it is clean after two fixes (an `std::uint8_t` base and a trailing comma for the `Identity` enumeration).
- Build cost (P1, W10): see the table below. The longest unit of each build is now `dual_identities_test.cpp` or `rounding_test.cpp`, and the builds take a little more than half as long. Total compile work rose by 1% to 4% (shared headers parsed once more per unit).
- `core.identities` is still labeled `slow`: its eight units together are 314 G at gcc-debug, more than twice `dual_identities_test.cpp`, the next largest.
- The mutants of `records/mutations/0019-antiproducts/` give the same kills before and after; see `records/mutations/0193-test-split/README.md`.

## Build cost before and after (shared mode, not pinned)

Before: `b504aa7` in new trees of this worktree. After: `4efe4d0` in new trees. One build each, through `tools/timed` (selections `all` before, `gate:all` after), with the 23-slot job pool. During the after builds another workstream's `ctest` held 5 of the pool's slots; the instruction counts do not depend on that, the wall times may. Instructions are the build's retired user-space instructions on both core kinds; per-unit figures are from each tree's `compile.tsv`. Wall times on a shared machine are noted, not compared under P7's criterion.

| Preset | Build wall | Build instructions | Longest unit | Identities units: work, longest, peak memory | Product units: work |
| --- | --- | --- | --- | --- | --- |
| gcc-debug before | 20.4 s | 886 G | `identities_test.cpp` 20.1 s | 1 unit: 295 G, 20.1 s, 1,376 MiB | 48.2 G |
| gcc-debug after | 11.5 s | 902 G | `dual_identities_test.cpp` 11.0 s | 8 units: 314 G, 7.4 s, 449 MiB | 55.0 G |
| clang-debug before | 19.5 s | 879 G | `identities_test.cpp` 19.4 s | 1 unit: 300 G, 19.4 s, 696 MiB | 48.1 G |
| clang-debug after | 11.5 s | 895 G | `dual_identities_test.cpp` 10.6 s | 8 units: 321 G, 5.8 s, 314 MiB | 53.7 G |
| gcc-asan before | 113.8 s | 3,549 G | `identities_test.cpp` 113.5 s | 1 unit: 1,309 G, 113.5 s, 3,505 MiB | 212 G |
| gcc-asan after | 55.6 s | 3,590 G | `dual_identities_test.cpp` 54.4 s | 8 units: 1,347 G, 36.8 s, 1,238 MiB | 223 G |

The units at `4efe4d0`, gcc-debug / clang-debug / gcc-asan, in G instructions: `identities_four_test.cpp` 83.5 / 81.3 / 372.9; `identities_five_dense_test.cpp` 58.5 / 67.2 / 280.1; `identities_five_diagonal_test.cpp` 49.0 / 52.0 / 233.3; `identities_six_test.cpp` 39.9 / 42.5 / 118.3; `identities_up_to_three_test.cpp` 34.3 / 25.2 / 153.2; `identities_conformal3_test.cpp` 28.7 / 30.3 / 138.5; `identities_sparse_test.cpp` 17.8 / 19.6 / 47.5; `identities_test.cpp` 2.6 / 2.4 / 3.0; `product_test.cpp` 31.8 / 29.5 / 125.2; `product_conformal3_test.cpp` 23.2 / 24.1 / 97.3.

The other presets, in new trees at `c28fa89` (the split with external linkage, `develop` merged), against the M3 gate's figures at `b504aa7` (another worktree, same machine): gcc-release 19.6 s (25.1 s), clang-release 19.2 s (26.2 s), clang-asan 62.1 s (82.6 s), lint 56.2 s (119.3 s); `clang-measure` 19.3 s, which did not build before. At `4efe4d0` they were rebuilt incrementally, so their times there are not build times of the suite.

At `c28fa89`, before `4efe4d0`, the shared checks had external linkage. At `-O0` that cost about 8 G at gcc-debug (322 G against 314 G) and nothing at clang-debug, but under gcc-asan (`-O1` with the sanitizers) the identities units took 1,961 G (+50% over the single source), 64.1 s for the longest, and the product units 328 G. Two probes, run outside the build through `tools/timed` with the log `probe.tsv` in the session scratchpad (not committed), found the cause. The eight sources concatenated into one unit took 1,939 G, so splitting was not it. `identities_four_test.cpp` and `identities_sparse_test.cpp` with the checks in an unnamed namespace took 373 G and 47.5 G, against 555 G and 61.2 G. The reading, inferred and not checked in GCC's dumps: at `-O1` GCC inlines a function with internal linkage into its one caller and drops it, which it does not do for a template instantiation that another unit could use. `4efe4d0` restored the internal linkage.

## Record of the process

- The B1 change, the split, and the linkage change are three commits, each building and passing on the presets checked before it was made (W2); each commit message lists its checks. The run rows are in `runs.tsv`; the mutant runs are under the selections `mutant:<name>`, the matrix under `gate:all`.
- The step-limit bisection (22 compiles with `-fsyntax-only` under `clang-measure`'s flags) ran one compile at a time outside `tools/timed`; its result is above. The concatenation and linkage probes ran through `tools/timed` into a scratch log, as said above.
- The decision number: the brief named 0190, which the records clerk's merge (`ac76bbd`) took while this unit ran; 0193 is the next free number.
