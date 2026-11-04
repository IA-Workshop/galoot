# WU11b's last group: rounding tests of the dual operations, mutants and observed ratios

Revision: `feature/core-rounding-wu11` at `c32c619`, from `develop` at `535c91e` (WU11a merged). Toolchain: g++ 16.2.1 and clang++ 23.1.4, on ds-001. Decision 0181 holds the design and the inventory rows; decision 0175 the check; the first group's records are in `../0020-rounding/`.

## Mutants

`mutate.py <preset>` as in the first group: each mutant applied to one file, `core_rounding_test` and `support_tracked_test` built with `ninja -k 0`, the tests labeled `core.rounding` or `support.tracked` run, the file restored. Rows: `out/gcc-debug.tsv`, `out/clang-debug.tsv`; runs in `runs.tsv` under `mutant:<name>`. The failing test cases come from `cases.py` on gcc-debug (selections `mutant-cases:<name>`), for the first seven mutants; the four larger projection nudges were run with `mutate.py` only.

| Mutant | gcc-debug | clang-debug | Killed by |
| --- | --- | --- | --- |
| the dot's result one ulp further from zero | killed | killed | "the dot and the antidot round within their bounds" |
| the projection's result one ulp further from zero | survived | survived | Inside the bound: see below |
| the projection's result 2, 4, 8, or 16 ulps further from zero | killed | killed | `core.rounding` (the projection's family; each of the four) |
| the bulk's result one ulp further from zero | killed | killed | "the bulk, the weight, and the duals round within their bounds" and "the projection rounds within its forward error bound" (whose weight dual goes through the bulk) |
| the projection takes the bulk dual of b | killed | killed | "the projection rounds within its forward error bound" (the tracked value differs from the reference) |
| the kernels' products rounded through float (first group's mutant) | killed | killed | the products, the antiproducts, the duals (the random form's), the dot and antidot, and the projection |
| one ulp after every sum of the kernels (first group's mutant) | killed | killed | the products, the antiproducts, the sums, the dot and antidot, and the projection |
| the test's exact-value check removed | survived | survived | Equivalent: see below |

Every kill is a test failing at run time; no mutant stopped a build.

## What the check cannot see

- **One ulp on the projection's result.** Its coefficients come from three stages of products and sums, so their K is several and their bound γ_K M several ulps of the value; one ulp, at most 2u times the value, adds only a part of the bound, and the largest observed ratio is 0.42, so on every coefficient seen a one-ulp error stayed inside γ_K M, where a sound bound check must accept it. Two ulps are caught. The one-ulp nudge after every sum inside the kernels is caught, because it enters the earlier stages and grows.
- **The exact-value check.** Where G only moves blades, the bulk, the weight, and the duals count one rounding (the product by the unit antiscalar, exact in double), so the bound check already requires d == fl(e), which is e. Removing the explicit exact-value check therefore changes nothing; it stays as the direct statement that these results are exact.

## Observed ratios

The largest ratio of \|d − e\| to γ_K M per family, then over coefficients with K ≥ 2, at `c32c619`, gcc-debug, through `tools/timed` (selections `core_rounding_test seed <s>`). The first group's families are unchanged from `../0020-rounding/`.

| Family | Coefficients | Seed 0 | Seed 1 | Seed 2 | Seed 3 | Seed 20261104 |
| --- | --- | --- | --- | --- | --- | --- |
| bulk, weight, duals | 47,400 | 0.592324 | 0.587741 | 0.617270 | 0.629159 | 0.502610 |
| dot, antidot | 1,800 | 0.973371, 0.441031 | 0.933208, 0.585474 | 0.978435, 0.525918 | 0.961096, 0.554220 | 0.889147, 0.423335 |
| projection | 6,840 | 0.387428 | 0.347242 | 0.418538 | 0.368579 | 0.289043 |

Every rounded coefficient of the bulk, the weight, the duals, and the projection has K ≥ 2, so both figures are the same; the duals' ratios come from the random form alone, all other forms giving exact results. No ratio exceeds 1.
