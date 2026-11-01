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

None.
