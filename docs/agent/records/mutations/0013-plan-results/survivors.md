# Survivors of the typed-plan mutation runs

The mutants are in `mutate.py`, run with the header and tests of `11ad0aa` (driver at `36e4f0f`) on gcc-debug and clang-debug, each in the normal mode and with `--runtime`: `results-36e4f0f-<preset>[-runtime].tsv` (78 mutants), and `results-36e4f0f-<preset>[-runtime]-added.tsv` for four mutants rewritten or added after reading the first results (below), run with the driver as committed beside this file. A mutant is killed when the build of `core_plan_test` fails or a `core.plan` test fails (the doctest program, the 15 compile-fail tests, and on Clang `core.plan_evaluated_once`); for a failed build the record gives the `static_assert` lines that fired and the first error.

Totals over the 81 distinct mutants: 68 killed on both compilers in both modes; ten equivalent survivors (below); `locals_plain` and `locals_plain_twice` killed on Clang and surviving on GCC by design; `twice_static_locals` surviving everywhere, as it must. No CTest ran after a failed build, and the pool held 23 of 23 free slots before and after every run.

## Equivalent: the change cannot alter any result the header promises

| Mutants | Change | Why equivalent, and what fires the check |
| --- | --- | --- |
| `zero_check_removed`, `first_check_removed`, `second_check_removed`, `beyond_n_check_removed`, `order_check_removed` | one of the five checks of a generated plan made `true \|\| ...` | as recorded for WU6b: a correct generator cannot trip them. Generator mutants fire them: `sign_by_sum` and `zero_test_wrong` (`plan.hpp:450`), `a_off_by_one` (`:452`), `b_off_by_one` (`:454`), `out_shifted` (`:456`), `out_xor`, `loops_swapped`, `terms_reversed` (`:458`) |
| `typing_order_check_removed`, `typing_sign_check_removed`, `typing_beyond_check_removed`, `typing_unused_check_removed`, `typing_terms_check_removed` | one of the five checks of the typing made `true \|\| ...` | `collect_result` and `type_terms` cannot produce what they reject. Typing mutants fire them: `result_by_mask`, `typed_result_short` (`plan.hpp:465`), `result_sign_from_coefficient` (`:468`), `position_off_by_one` (`:470`, `:472`), `result_extra_scalar` (`:476`), `result_repeats_kept` (`:462`, `:469`), and `typed_result_order_by_mask` (`:465`: the check, made to compare masks, rejects a correct result) |

Each check's condition is tested directly: `defects_of` and `typed_defects_of` on hand-written plans at each bound, which kill `first_bound_strict`, `second_bound_strict`, `order_allows_repeat`, `order_same_b`, `order_same_a`, `typed_result_repeat_allowed`, `typed_sign_test`, `typed_beyond_inclusive`, `typed_unused_ignored`, `typed_order_allows_equal`, `typed_order_same_b`, and `typed_order_same_a`. In `--runtime` mode the ten checks are off, so their removal is reported as skipped.

## The form of the locals and of the initializer

| Mutant | Clang | GCC | Reading |
| --- | --- | --- | --- |
| `locals_plain` (every local of `make_product_plan` plain `constexpr`) | killed by `core.plan_evaluated_once`, both modes | survives | GCC has no evaluation check (decision 0112); the result is the same, the difference is cost |
| `locals_plain_twice` (plain locals, and an initializer that calls the generator twice) | killed by `core.plan_evaluated_once`, both modes | survives | as above |
| `twice_static_locals` (the initializer calls the generator twice; static locals kept) | survives | survives | required to pass: the second call reads the static locals, so nothing is generated again |

## Rewritten or added after the first run

- `wedge_capacity_gp` (the wedge's capacity sized as the product's): its first form, `return product_capacity(a, b);`, failed the build only by `-Wunused-parameter` under `-Werror` on both compilers (`results-36e4f0f-<preset>[-runtime].tsv`), which is not a kill by a test. It was rewritten to keep reading the operation and rerun: killed in both modes on both compilers by the case "Repeated blades share a result position, and empty sets give empty typed plans", which checks `plan_capacity(wedge, ...) == 1`. In output it is equivalent; it is the plan's cost mutant, and the capacity's own check is what sees it.
- `typed_plan_not_structural` (a `mutable` member): killed in every configuration by the first use of a plan in a constant expression (`plan.hpp:364`, the member is not usable there). No mutant was found that makes the typed plan non-structural while leaving it usable in constant expressions, so the `terms_of` static_assert (`plan_test.cpp:1286`, a plan as a template argument) has no mutant of its own.
- `plan_term_not_trivially_copyable` and `plan_term_not_aggregate`: WU6b's mutants of `plan_term`, rerun on the migrated static_asserts (`plan_test.cpp:506`, `:507`): killed in both modes on both compilers.

## Static_asserts and test cases without a mutant of this unit

- `plan_test.cpp:491`, WU6b's `e123 e123 = 0` in the degenerate form: a value of `blade_product`, judged by WU6a's mutants of that function, not by mutants of the plan.
- The test case "plan_capacity is the largest bound over the pairs" (WU6b): no mutant here fails it at run time; WU6b's records kill it with its capacity mutants (this driver keeps `capacity_start_zero` and `capacity_halved`, killed by other cases and by the capacity assertion).

## Unresolved

None.
