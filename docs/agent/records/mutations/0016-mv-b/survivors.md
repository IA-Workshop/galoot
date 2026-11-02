# WU8b mutation runs: outcome and survivors

Driver: `mutate.py` here, 31 mutants of `include/galoot/core/mv.hpp`, each built (`all`) and run against the whole suite on gcc-debug and clang-debug, in the normal mode and in the run-time mode (the static_asserts of `mv_test.cpp` set aside). A build that fails only on warnings counts as a survivor. CTest is not run after a failed build. Job pool: 23 of 23 slots free before and after each run. Lessons 0060 to 0064 are kept: the build's status is read before the tests, each anchor must occur once, and no CTest runs after a failed build.

## Runs

| File | Revision | Mutants | Result |
| --- | --- | --- | --- |
| `results-7d5f643-normal.tsv` | `7d5f643` (header of `be1b209`) | all 31 | all killed on both presets (62 of 62) |
| `results-7d5f643-runtime.tsv` | `7d5f643` | all 31 | 58 killed; `integer_bound_one_above` survived on both presets; `truncate_checks_rounding` and `truncate_check_removed` gave "warning" on clang-debug; `truncate_skips_rational` was killed by a compile error of the mutant itself |
| `results-b79716c-rerun-normal.tsv`, `-runtime.tsv` | `b79716c` (tests changed, header unchanged) | those four, `truncate_skips_rational` rewritten | all killed on both presets in both modes |

## The survivors and what was done

1. `integer_bound_one_above`, both presets, run-time mode: only a `static_assert` (`!e12_exact<128, 256, short>`) saw the accepted coefficient 32768. `b79716c` repeats every answer of the coefficient check in a run-time CHECK. Killed at `b79716c`.
2. `truncate_checks_rounding` and `truncate_check_removed`, clang-debug, run-time mode, "warning": with truncate's check gone or moved, the counting scalar of `mv_truncate_assert_fail.cpp` had an unused `==` and `is_exact`, and Clang's `-Wunused` stopped the build before any test ran. `b79716c` marks both `[[maybe_unused]]`. Killed at `b79716c` by core.mv and by the truncate programs.
3. `truncate_skips_rational`: the first form of the mutant read `std::numeric_limits<T>::is_integer`, which the test rational's specialization does not declare, so the mutant did not compile. Rewritten to skip any scalar that `detail::integer_limits` does not describe as an integer type, which skips the rational and keeps `std::int64_t`. Killed at `b79716c` by `!truncates_in_a_constant_evaluation<rational, 7>`, `core.mv_truncate_consteval_nonzero_rational`, and `core.mv_truncate_nonzero_rational`.

Kills by a compile error in the header: in the run-time mode, `float_bound_one_below` and `integer_bound_one_below` stop at apply's own check (`mv.hpp:708`), `covering_reversed` at the lossy catch-all, and `derived_reaches_count` at the count catch-all. Each is reached from a test's own call (the apply over float in diag(4096, 4096), the conversions of the conversion test, the copy of a derived line), so the test kills them; none is a warning.

## Kills by the plan's list

| Plan's mutant (section 8, WU8b) | Driver name | Killed by |
| --- | --- | --- |
| the first-term flag inverted | `first_flag_inverted` | the first-term assertions of "apply assigns the first term ..."; run-time: that test and "apply equals the loop ..." |
| the first term added to zero (the plan expected it equivalent in value) | `first_term_added_to_zero` | the signbit assertions on -0.0; run-time: the same test's CHECK |
| the ±1 branches swapped | `plus_minus_swapped` | the hand values; run-time: F1, F3 to F5 |
| the coefficient applied as (c x) y (the plan expected it equivalent) | `coefficient_times_first_input` | "apply multiplies the inputs before the coefficient": (2 max) 0.25 overflows |
| the exactness bound off by one on each side | `float_bound_one_below`, `float_bound_one_above`, `integer_bound_one_below`, `integer_bound_one_above` | the bounds of "apply checks that the scalar holds ..." (2^24 and 2^24 + 1 for float, 32767 and 32768 for short), and its run-time CHECKs from `b79716c` |
| the conversion's sign s_source s_target replaced by s_target | `conversion_sign_of_target_only` | the conversion test over five scalars |
| the covering test reversed | `covering_reversed` | the conversion test (its lossless conversions reach the lossy catch-all) |
| the dropped-coefficient check run for a rounding scalar | `truncate_checks_rounding` | `truncates_in_a_constant_evaluation<double, 7>`, and the run-time truncation over double |
| ... or skipped for the rational | `truncate_skips_rational` | as item 3 above |
| truncate<Type> accepting a type of another algebra | `truncate_other_algebra` | the `multivector_of` assertions, `core.mv_truncate_other_algebra` |

The other mutants are the checks this unit adds, each removed or weakened: the coefficient check and both target checks (`core.mv_apply_inexact_*`, `core.mv_apply_target_lacks_blade`, `core.mv_apply_map_target_lacks_blade`), the sign of the target in the product and map kernels (F11, F5), the lossy catch-all's message (`core.mv_conversion_lossy_*`), truncate's check removed and the other-scalar target (core.mv, `core.mv_truncate_*`), bool in `ring_scalar`, each scalar check, the template head, the placeholder storage (`core.mv_scalar_without_default`, on both compilers), the array and braced-list catch-alls (`core.mv_coefficients_as_*`), the derived-class exclusion, and the ordered absent message (`core.mv_get_absent_ordered`).

## Review fixes, 2026-11-01

After the W5 blind review and the hci-expert reading of WU8b, the driver gained mutants for the code that the fixes changed or added (`ef579e2`), and the mutants whose anchors moved were rewritten (44 mutants in the driver now). The 24 changed or new ones ran on gcc-debug and clang-debug in both modes:

| File | Revision | Result |
| --- | --- | --- |
| `results-71d040a-review-normal.tsv`, `-runtime.tsv` | `71d040a` (header of `ef579e2`) | all killed except `float_exponent_ignored` (survived on both presets in both modes); `float_bound_on_magnitude` was killed by an unused-variable warning of the mutant itself, and `integer_min_one_below` and `integer_bound_one_above` on GCC by an overflow diagnostic of the mutant's own `min() - 1` and `max() + 1` (a warning in the run-time mode) |
| `results-13b53ee-review-rerun-normal.tsv`, `-runtime.tsv` | `13b53ee` (one test added) | those four, the three flawed forms rewritten (`odd_bits` kept in use; `c + 1 >= min()` and `c - 1 <= max()`): all killed on both presets in both modes |

1. `float_exponent_ignored`: no present scalar has an exponent range small enough for |c| < 2^max_exponent to matter (float's is 128). `13b53ee` adds a test-only scalar whose limits describe IEEE half precision (11 digits, exponent to 2^16): 2^15 is held and 2^16 is not. Killed at `13b53ee`.
2. The three rewritten forms: a mutant must change the code under test and nothing that a compiler diagnoses by itself; the first forms did not meet that and are not counted.

The review's own surviving mutants are killed: the check stopping after the first dropped place (`truncate_checks_first_only`, by the antiscalar cases of the constant-evaluation test and `core.mv_truncate_nonzero_*_antiscalar`), and `>= 64` changed to `> 64`, whose code is gone with F1's fix (the bound shifts by no type's digits; the long double products and checks stay in the tests).
