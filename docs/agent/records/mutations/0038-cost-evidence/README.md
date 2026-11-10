# Plan 0038 part C's mutants: the counting scalar and the operation counts

Run on 2026-11-10 (UTC) by Claude (agent, Opus 5.5, in the test-author and implementer roles of plan 0038 part C) with `tools/mutate`, every build and test through `tools/timed` with the shared job pool. Compilers: g++ 16.2.1 (gcc-debug), clang++ 23.1.4 (clang-debug). Both presets build with checking on; the counting tests are built in both modes in every preset (`rga3.op_counts_checking`, `rga3.op_counts_unchecked`), so both sets of counts judge every mutant here.

- **Gap:** `tools/mutate gap.toml`, from 13:48 to 14:00, pool 23 of 23 free before, load average 1.5. Rows: `out/gap-<preset>.tsv`; builds and tests are the rows `mutant:<name>` of `records/performance/runs.tsv` on this branch.
- **Tip:** `tools/mutate counting.toml`, from 14:01 to 14:14, pool 22 of 23 free before and 23 of 23 after, load average 10.0 at the start (other work on the machine). Rows: `out/counting-<preset>.tsv`. `tools/mutate` exited 0: every verdict as declared, every declared killer among the killers.
- **Revision:** both runs at `feature/cost-evidence--counting` `286a322`.

## How the gap is judged

The gap mutants are judged by the suites that check values: `core.product`, `core.transform`, and the rga3 components other than `op_counts` (`selection.values` of `gap.toml`, 128 tests). The suite at the branch point (`feature/cost-evidence` `5444671`) is that suite: this unit adds test files and CMake registrations and changes no existing test source and nothing under `include/`, and `golden.hpp` only gains a namespace. So the gap run uses the tip's tree with the new tests left out of the selection, rather than a second tree at the branch point. `counting.toml` runs the same mutants with `rga3.op_counts` added (130 tests).

Before the implementation, the test author built the two counting programs by hand with GCC against scratch stand-ins of the scalar and of the golden counts (the counts typed from the spike's tables), in both modes, and applied the four gap mutants to the headers one at a time. Three failed the tests in both modes, and `first_stage_all` did not. Those builds used a scratch script and the scratch log, so that step is not reproducible from the repository; the runs below are.

## Verdicts

| Mutant | Written for | Gap (both presets) | Tip (both presets) | Killers at the tip |
| --- | --- | --- | --- | --- |
| `product_kernel_zero_term` | the gap: a product kernel keeps one extra term with coefficient zero | survived | killed | `rga3.op_counts_checking`, `rga3.op_counts_unchecked` |
| `sign_multiplied` | the gap: `accumulate` multiplies by +1 and -1 instead of folding the sign | **killed** by `core.transform_rounding` | killed | `core.transform_rounding`, both `op_counts` |
| `first_stage_all` | the gap: transform's first stage keeps every term with checking off | survived | survived | none |
| `cancelling_pair` | the gap: a product kernel adds a term t and its negation -t to one output | survived | killed | both `op_counts` |
| `counting_without_multiplications` | the new tests | (not run) | killed | both `op_counts`, `support.counting` |
| `counting_without_additions` | the new tests | (not run) | killed | both `op_counts`, `support.counting` |
| `counting_without_subtractions` | the new tests | (not run) | killed | both `op_counts`, `support.counting` |
| `counting_without_negations` | the new tests | (not run) | killed | both `op_counts`, `support.counting` |
| `counting_without_comparisons` | the new tests | (not run) | killed | `rga3.op_counts_checking`, `support.counting` |
| `counting_without_integer_constructions` | the new tests | (not run) | killed | `support.counting` |
| `counting_without_default_constructions` | the new tests | (not run) | killed | `support.counting` |
| `golden_sign_as_multiplication` | the new tests: `golden.py` counts a term of coefficient +1 or -1 as two multiplications | (not run) | killed | `reference.golden` |

## Findings

- **The gap.** `product_kernel_zero_term` and `cancelling_pair` survive the value suites and are killed by the counting tests, on both compilers: the counting tests close that gap. Neither changes a value over an exact scalar, and over double neither moved a value beyond the value tests' tolerances.
- **`sign_multiplied` is not a gap mutant.** The value suites already kill it, through `core.transform_rounding`: its cases "transform's tolerance counts the middle blades that only the dropped terms read", "transform's tolerance holds dropped coefficients whose products underflow", and "transform's tolerance of each dropped coefficient is between gamma_K M and gamma_(K+2) M" fail, because a product by +1 or -1 is one more rounding for the tracking oracle, which the library's bound does not count. `gap.toml` declared it `survived` before the run, as the plan listed it, and the declaration is kept, so `tools/mutate gap.toml` exits 1 on this mutant only. The counting tests kill it as well.
- **`first_stage_all` is not a gap mutant either, and no test of this unit can kill it.** In rigid(3), the second stage of a motor acting on a point, a line, or a plane reads every middle blade, so the restricted first stage already holds every term (20, 36, and 28 terms; the reference computes both lists and they are equal, and `transform_test.cpp`'s kernel counts state the same). Keeping every term then changes nothing in these operations. A transform whose restriction drops terms, such as the degenerate form of `transform_test.cpp`, would be needed to kill it, which is outside the operations this part counts.
- **Each kind the scalar counts has a test that sees it.** The integer and default constructions are seen only by `support.counting`: the rga3 operations form no integer constant, and the default constructions are not pinned.
- **The reference.** `golden_sign_as_multiplication` changes every multiplication count that `golden.py` writes, and `reference.golden` fails on the committed header. With the header regenerated, both `op_counts` programs would fail too; that was not run.
