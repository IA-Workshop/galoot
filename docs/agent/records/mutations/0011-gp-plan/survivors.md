# Survivors of the gp_plan mutation runs

The mutants are in `mutate.py`, run at `163ab01` on gcc-debug and clang-debug, each in the normal mode and with `--runtime` (results `results-163ab01-*.tsv`). A mutant is killed when the build of `core_plan_test` fails or a `core.plan` test fails; the record gives, for a failed build, the `static_assert` lines that fired.

## Equivalent: the change cannot alter any result the header promises

| Mutant | Change | Why equivalent |
| --- | --- | --- |
| `zero_check_removed` | the `static_assert` on a zero coefficient after generation made `true \|\| ...` | `generate_gp` cannot write a zero coefficient: `blade_product` drops zero sums and a sign of ±1 keeps a coefficient nonzero. The check fires on the generator mutants `sign_by_sum` and `count_plus_one` (`plan.hpp:213`) |
| `first_check_removed` | the check that every `a` lies in the first set | `a` is the loop index over the first set. Fires on `a_off_by_one`, `positions_swapped`, `fill_sets_swapped` (`plan.hpp:215`) |
| `second_check_removed` | the check that every `b` lies in the second set | as above for `b`. Fires on `b_off_by_one`, `positions_swapped`, `fill_sets_swapped` (`plan.hpp:217`) |
| `beyond_n_check_removed` | the check that every `out` names only e_1 to e_n | every term of `blade_product` lies on a subset of A ∪ B, which the set check keeps within e_1 to e_n. Fires on `out_shifted` (`plan.hpp:219`) |
| `order_check_removed` | the check of the order a, b, canonical `out` | the loops write in that order and `blade_product` keeps canonical order. Fires on `loops_swapped`, `positions_swapped`, `count_plus_one` (`plan.hpp:221`) |
| `locals_plain` | `make_gp_plan`'s fill as a plain `constexpr` local | the result is the same; the difference is cost, which `records/performance/plan-growth/out/locals.tsv` measures (Clang +26% to +43% of a unit for all locals plain; GCC unchanged) |

None of these can be killed without a faulty generator, and a test hook in the header to inject one would be code for the tests' sake (C12). Each check's condition is tested instead: `defects_of` is called on hand-written plans at each bound, which kills `first_bound_strict`, `second_bound_strict`, `zero_test_wrong`, `beyond_n_test_one_more`, `order_allows_repeat`, `order_same_b`, and `order_same_a`.

In `--runtime` mode the five checks of a generated plan are off, so their removal mutants are reported as skipped.

## Unresolved

None at `163ab01`.

## After the W5 review (2026-11-01)

Both lists were rerun at `4e5ea90`, the code after the review fixes, on gcc-debug and clang-debug in both modes. The author's list gives `results-4e5ea90-<preset>[-runtime].tsv`, with the same counts as at `163ab01`: 43 killed and the six survivors above, or in run-time mode 43 killed, 5 skipped, and `locals_plain`. The review's list (`../../reviews/2026-11-01-wu6b-gp-plan/mutate_review.py`, run with this driver) gives `results-4e5ea90-<preset>[-runtime]-review.tsv`: 22 of 27 killed in normal mode, the same on both compilers.

- Killed now, not before: `r_make_capacity_one` and `r_make_capacity_two`. They fail the build at the conformal(2), degenerate, and golden random-form plans of `plan_test.cpp`, in both modes (`results-accdb24-*-review-f1.tsv`, `results-d75d491-*-runtime-review-f1-f2.tsv`). `r_gp_plan_unconstrained` fails `core.plan_not_metric_like`, which now forbids `make_gp_plan` (`results-d75d491-*-review-f2.tsv`).
- Equivalent, as the review found: `r_sign_by_equality` (for signs of ±1, which are checked first), `r_capacity_ge` (`>=` in a maximum), `r_defects_n_widest` (the generated-plan range check at n = `mask_bits`, which a correct generator never trips), and `r_count_plain` and `r_all_locals_plain_count` (cost only).
- In run-time mode only, two more survive, because the checks that kill them are off: `r_guard_range_only` and `r_defects_sizes_swapped`. Both are killed in normal mode.

Unresolved: none.
