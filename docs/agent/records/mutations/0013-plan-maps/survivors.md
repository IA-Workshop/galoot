# Mutation results for the unary maps and the antiproduct plans (plan 0013, WU7b)

Driver: `mutate.py` in this directory, at `e631c11` for the four passes and `10ec452` for the rerun of one mutant; 68 mutants of `include/galoot/core/plan.hpp` and `include/galoot/core/bits.hpp`. Each pass built `core_plan_test` and ran the `core.plan` selection (the doctest program, the compile-fail tests, and on Clang `core.plan_evaluated_once` and `core.plan_map_evaluated_once`); no CTest ran after a failed build. GCC 16.2.1 and Clang 23.1.4, gcc-debug and clang-debug, each in normal mode and in `--runtime` mode (the test's static_asserts and the header's generation-time checks turned off). Results: `results-e631c11-<preset>[-runtime].tsv`, and `results-10ec452-<preset>[-runtime]-rerun.tsv` for the rerun. Recorded by Claude (agent, Opus 5.5), cpp-expert.

## Summary

| Pass | Killed | Survived | Skipped (check off in this mode) |
| --- | --- | --- | --- |
| gcc-debug | 56 | 12 | 0 |
| gcc-debug --runtime | 55 | 6 | 7 |
| clang-debug | 59 | 9 | 0 |
| clang-debug --runtime | 58 | 3 | 7 |

No mutant is unresolved. Every survivor is one of the classes below, each explained.

## Survivors

1. `left_right_swapped`, every pass: equivalent. The conjugation with the left complement on the inputs and the right on the outputs computes the same antiproduct as the definition, for every pair of every form, in even and in odd n. Proof: left(a) left(b) = σ(a)σ(b) right(a) right(b) and right(e_D) = σ(D) left(e_D), with σ(u) = (−1)^(gr·ag), so each term carries σ(a)σ(b)σ(D); the product and the wedge keep the parity of grades, so gr D ≡ gr a + gr b, and the factor is 1 (for odd n every σ is 1; for even n σ(u) = (−1)^gr u). This is the parity argument of the right De Morgan law (decision 0115). The plan expected this mutant to survive odd n only; it survives even n too, because it is not a defect. The swaps that are defects are killed in every pass: the inputs alone (`input_left_complement`), the outputs alone (`output_right_complement`), the map's two signs (`unary_complement_signs_swapped`, by the closed-form check), and the two signs of `bits.hpp` (`bits_left_right_swapped`, by the closed-form check).
2. The seven removals of a map check (`map_order_check_removed`, `map_sign_check_off`, `map_beyond_check_removed`, `map_unused_check_removed`, `map_per_input_check_removed`, `map_blade_check_removed`, `map_closed_form_check_removed`), normal passes: equivalent, since a correct generator trips none of them. Each check fires on a generator mutant (next section). In `--runtime` mode they are skipped, the checks being off.
3. `pair_terms_unconstrained`, `--runtime` passes: the constraint is judged only by a compile-time `static_assert` (`has_pair_rule`), which `--runtime` turns off; killed in both normal passes.
4. `map_locals_plain`, `map_locals_plain_twice`, `plan_locals_plain`, GCC passes: by design. GCC's cache of constant calls serves a repeated generation, and the evaluation checks are registered for Clang only; killed on clang-debug in both modes, by `core.plan_map_evaluated_once` and `core.plan_evaluated_once`.
5. `map_twice_static_locals`, all passes: by design. The initializer calls the generator twice with static locals, which generates nothing again, so the evaluation check must pass it.

## Each generation-time check fires on a mutant (normal mode, both compilers)

| Check | Mutant whose build it fails |
| --- | --- |
| product plan: generated terms out of order (the run reversal) | `run_reversal_dropped`, `output_not_complemented` |
| unary map: result out of canonical order | `map_result_reversed`, `map_result_order_by_mask`, `map_result_extra_blade` |
| unary map: result sign other than +1 | `map_result_sign_negative`, `map_result_extra_blade` |
| unary map: a term points past the result | `map_position_off_by_one` |
| unary map: a result blade no term uses | `map_position_off_by_one`, `map_result_extra_blade` |
| unary map: not one term per input | `map_position_zero` |
| unary map: a term not on its operation's blade | `right_complement_keeps_mask`, `map_blade_kinds_swapped`, `map_complement_right_only`, `map_generated_in_wider_algebra` |
| unary map: a complement's sign differs from its closed form | `unary_complement_signs_swapped`, `left_complement_as_right`, `map_sign_not_folded`, `map_coefficient_negated`, `map_left_factor_dropped`, `map_left_factor_by_grade`, `map_closed_form_unsigned`, `closed_form_k_minus_one`, `closed_form_index_from_zero`, and the `bits.hpp` mutants `reorder_sign_skips_lowest` and `bits_left_right_swapped` |

The `checks_fired` column of each result file lists the messages seen.

## Kills by a warning

`map_left_factor_by_grade` was killed in the gcc-debug `--runtime` pass at `e631c11` by `-Werror=unused-variable` (its edit dropped the only use of the antigrade), not by a test. It was rewritten to keep the antigrade read (`10ec452`) and rerun in all four modes: killed in normal mode by the closed-form check, and at run time by `core.plan`. No other kill in any pass came from a warning.

## Mutants of the plan's list (section 6, WU7b)

- The run reversal dropped: killed by the generated-plan order check, both compilers.
- The output complemented on the right: killed (`output_right_complement`).
- One input not complemented: killed (`first_not_complemented`, `second_not_complemented`).
- The left and right complement signs swapped: in the map, killed by the closed-form check; in `bits.hpp`, killed by it; in the conjugation as a whole, equivalent (survivor 1).
- The reverse sign with k(k+1)/2 (`reverse_sign_k_plus_one`, in `bits.hpp`): killed by `core.plan` at run time (the map tests at n = 1 to 10).
- The antireverse with the grade in place of the antigrade: killed (`antireverse_with_grade`).
- Plain locals in each new generator: killed on Clang by the evaluation checks (survivor 4 on GCC).
