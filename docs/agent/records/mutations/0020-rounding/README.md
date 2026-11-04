# WU11b rounding tests over double: mutants, observed ratios, and cost

Revision: `feature/core-rounding` at `3c9b9a2`, after the blind review's fixes (the last commit that changes a test or the tracking scalar), for the mutant runs and the ratios. The first runs, before the review, were at `e36a905`; their figures are superseded here, and the cost section below is from `e67eff2`, before the review. Toolchain: g++ 16.2.1 and clang++ 23.1.4, on ds-001. Decision 0175 holds the tolerance policy and the derivations of the margin and the ratio limit; research note 0020 the reading of their source; `records/reviews/2026-11-04-wu11b-review.md` the review.

## Status of the unit

Done on this branch: the tracking scalar `test/support/tracked.hpp` with its self-tests (`support.tracked`), and `core.rounding` for every M3 operation on `develop` at `c86cb58` (WU9, WU10a, WU10b). Pending: the group for WU11a's operations (`bulk`, `weight`, the four duals, `dot`, `antidot`, `project`), which waits for WU11a's merge into `develop` (plan 0020 section 4, step 4), and then the inventory row of decision 0175 for them. Plan 0020's Outcome holds a short status line.

Run-log rows naming `3aa9079` (54) and `38ba916` (2) belong to commits folded away while the branch was unshared (decision 0175, "Record of the process"); they cannot be reproduced from the branch's history.

## Mutants

`mutate.py <preset>` applies each mutant to one file, builds `core_rounding_test` and `support_tracked_test` with `ninja -k 0`, runs the tests labeled `core.rounding` or `support.tracked`, and restores the file. Rows: `out/gcc-debug.tsv`, `out/clang-debug.tsv`; the runs are in `runs.tsv` under the selections `mutant:<name>`. The test cases that failed were read from the programs' output on gcc-debug (`cases.py`, selections `mutant-cases:<name>`). All at `3c9b9a2`.

| Mutant | gcc-debug | clang-debug | Killed by |
| --- | --- | --- | --- |
| `run_product` rounds each product through `float` (plan) | killed | killed | `core.rounding`: "the products round within their forward error bounds" and "the antiproducts round ..." |
| `a / s` multiplies by the reciprocal (plan) | killed | killed | `core.rounding`: "scaling and division round each coefficient once" |
| the tracking scalar's sum adds no rounding (plan) | killed | killed | `support.tracked`: "tracked counts roundings and magnitudes by its rules"; `core.rounding`: the hand case, the products, the antiproducts, and the sums, whose bounds become too small for the real results |
| the margin is removed, `rounding_margin = 0` (plan) | killed | killed | `support.tracked`: "with several roundings the check admits exactly the bound" (1 + 4u against 1 + 2^−53 with K = 3) and "the bound check rejects a result beyond the bound". No `core.rounding` case fails: no result lies between the two tolerances at seed 0 |
| the tracking scalar's product counts the larger depth, not the sum (implementer) | killed | killed | `support.tracked`: the rules case (p (c s) is four deep). `core.rounding` does not see it: every product in a one-stage kernel multiplies inputs of depth 0, where the two rules agree |
| an exact result (K = 0) is checked like one of several roundings (implementer) | survived | survived | Equivalent since the review's fix: with K = 0 the ratio divides by γ_0 = 0, so any nonzero exact error gives an infinite ratio and is rejected, and a zero error is accepted, which is what the equality accepts. Kept as the explicit form |
| a first term of coefficient −1 is multiplied by −1 instead of negated (implementer) | killed | killed | `core.rounding`: "the grade maps and the complements are exact over double" and "negation, part, and truncate are exact over double", whose results now count a rounding. The doubles are unchanged, since −1 × x is exact |
| one ulp added after every sum of the kernels, for results above 2^−9 (blind review) | killed | killed | `core.rounding`: the products, the antiproducts, and the sums. Before the review's fix this mutant survived (ratio 2.997) |
| two ulps, likewise (blind review) | killed | killed | as the one-ulp mutant |
| a result of one rounding checked by the margin's tolerance, not equality (review fix) | killed | killed | `support.tracked`: "with one rounding the check accepts only the nearest double" and "the bound check rejects a result beyond the bound" |
| the ratio check removed from the check of several roundings (review fix) | killed | killed | `support.tracked`: "with several roundings the check admits exactly the bound" (1 − 4u, within the margin's tolerance of fl(e) but 5u from e) and "the bound check rejects a result beyond the bound" (2Ku) |

Every kill is a test failing at run time; no mutant stopped a build. One equivalent survivor, with its reason above.

## Observed ratios

The largest ratio of \|d − e\| to γ_K M per family, at five seeds of `--rand-seed`, over every coefficient that rounds and, after the comma, over those with K ≥ 2, from runs of the gcc-debug program through `tools/timed` (selections `core_rounding_test seed <s>`) at `3c9b9a2`. The doubles do not depend on the compiler (the same IEEE operations in the same order, with contraction off); before the review, gcc-debug and clang-measure gave identical figures. The grade maps and complements, and negation, `part`, and `truncate`, round nothing, so their ratio is 0 and their check is equality; sums have K = 1 only, so their check is equality with fl(e) and the ratio is reported only.

| Family | Coefficients | Seed 0 | Seed 1 | Seed 2 | Seed 3 | Seed 20261104 |
| --- | --- | --- | --- | --- | --- | --- |
| products | 52,720 | 0.998338, 0.963843 | 0.999779, 0.914233 | 0.998217, 0.917789 | 0.999530, 0.917205 | 0.999315, 0.919852 |
| antiproducts | 70,400 | 0.999682, 0.991993 | 0.999954, 0.990679 | 0.999501, 0.955243 | 0.999974, 0.940369 | 0.999834, 0.926875 |
| grade maps and complements | 65,800 | 0 | 0 | 0 | 0 | 0 |
| sums and differences | 44,400 | 0.999751 | 0.999653 | 0.999856 | 0.999562 | 0.999955 |
| negation, part, truncate | 29,600 | 0 | 0 | 0 | 0 | 0 |

No ratio exceeds 1, and each family now checks this against the ratio limit 1 + 6u. The ratios near 1 come from coefficients with one rounding, where a correctly rounded operation can have a relative error close to u and γ_1 = u / (1 − u). The ratios above 0.9 with two or more roundings come mostly from a product by an integer coefficient other than ±1 (the random form has coefficients up to 31), which rounds twice in a row on one term. They are information about the bound, not a reason to tighten it. The scalings and division are checked by equality with the IEEE operation and have no ratio.

## Cost of the new units

Measured before the review, at `e67eff2`; the review added test cases, so these figures are a lower bound for `3c9b9a2` (the final runs' rows in `runs.tsv` give the build times there).


Each compile measured by `tools/measure` (build/<preset>/compile.tsv, not committed) at `e67eff2`, sources touched to force the compile, through `tools/timed` under the selection `gate:core.rounding|support.tracked`; the machine shared with other workstreams, compiles not pinned (P14), so the wall times are indicative and the instruction counts (performance and efficiency cores summed) the comparable figure.

| Preset | `rounding_test.cpp`: wall, peak memory, instructions | `tracked_test.cpp` |
| --- | --- | --- |
| gcc-debug | 5.4 s, 646 MB, 70.6 G | 0.2 s, 91 MB, 1.5 G |
| gcc-release | 8.8 s, 602 MB, 106.5 G | 0.3 s, 97 MB, 3.3 G |
| gcc-asan | 26.1 s, 1,218 MB, 333.2 G | 0.6 s, 129 MB, 5.8 G |
| clang-debug | 4.3 s, 393 MB, 51.5 G | 0.3 s, 123 MB, 0.9 G |
| clang-release | 8.8 s, 385 MB, 105.2 G | 0.2 s, 128 MB, 1.4 G |
| clang-asan | 33.0 s, 1,118 MB, 339.2 G | 0.3 s, 132 MB, 2.1 G |
| clang-measure | 8.9 s, 384 MB, 105.3 G | 0.3 s, 128 MB, 1.4 G |

The two tests together run in 1.0 to 1.9 s per preset (the CTest runs of the same selection). `core.rounding` is not labeled `slow`. No core header changed, so there is no before-and-after figure for other suites. `clang-measure`, the optimized Clang build without libstdc++'s assertions, builds and passes both units: the conformal(3) geometric antiproduct of two `full<5>` values, which that configuration cannot build, is left out of the test, and a pair of grade sets stands in.
