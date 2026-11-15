# Survivors of the blade_product mutation runs

State after the W5 review (`records/reviews/2026-11-01-wu6a-blade-product.md`) and its fixes. The author's mutants are in `mutate.py`, the reviewer's in `../../reviews/2026-11-01-wu6a-blade-product/mutate_review.py`.

## Equivalent: the change cannot alter any result the header promises

| Mutant | Change | Why equivalent |
| --- | --- | --- |
| `contraction_transposed`, `correction_transposed` (author) | g(i, j) read as g(j, i) | `blade_product` requires a symmetric form |
| `sum_guard_branch` (author) | `y >= 0` for `y > 0` in `add_checked` | at y = 0 both branches accept every x within -max to max, which `add_checked` requires |
| `r_unguarded_wedge_sign` (review) | `reorder_sign(...) * c` without `multiply_checked` | ±1 times a value within -max to max stays within it |
| `r_unguarded_inner_contraction`, `r_unguarded_inner_correction` (review) | `sign * entry` without `multiply_checked` | ±1 times an `int` widened to 64 bits always fits |

## Unresolved: not shown equivalent, and no killing input known

| Mutant | Change | What is known |
| --- | --- | --- |
| `r_unguarded_scale` (review) | `x.terms[k].coefficient * scale` without `multiply_checked`, in `add_generator_product` | The reviewer's exact-integer port of the recursion (`first_overflow.py`, 52 dense forms at n = 5 and 6 with entries up to 2^15 - 1, and the overflow test's form with both signs of its off-diagonal) found the first operation to leave ±max always at the contraction product or at a sum, never here. The scale's degree in the Gram entries grows with the depth of the corrections, so a killing input may exist at larger n. |
| `r_unguarded_correction` (review) | `(sign * entry) * scale` without `multiply_checked`, in `add_blade_product` | As above. |

The guards stay: both multiplications can overflow in principle, and the guard's message is the one a user would see.

## Killed after the review's fixes

`range_second_unchecked` (the review's `r_range_b_unchecked`), `range_skip_below_top` (`r_range_skip_below_top`), and `partial_sum_excursion` (`r_partial_sum_excursion`) were added to `mutate.py` and are killed on gcc-debug and clang-debug, in both modes, by `core.blade_product_beyond_n_second` and `_rejects_beyond_n_second`, by `core.blade_product_beyond_n_31` and `_rejects_beyond_n_31`, and, for the excursion, by the widest-algebra `static_assert` at capacity 1 in the build and, with the static_asserts off, by the test case "Every product fits a term list of exactly its capacity bound" (`results-65bd869-*-fixes*.tsv`). The test case comparing compile time with run time (A11) kills the review's `r_runtime_diverges`, which closes the one case the author's run-time record left without a kill.

2026-11-08: the 42 mutants and the run-time edits, with a fourth for the `static_assert` of the top-generator test case, are in `mutants.toml`, a spec for `tools/mutate` (plan 0026), whose outputs are `out/mutants-<preset>.tsv` and `out/mutants-<preset>-runtime.tsv`; `../0026-tools/README.md` compares them with the tables above.
