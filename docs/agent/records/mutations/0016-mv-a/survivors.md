# WU8a mutation runs: outcome and survivors

Driver: `mutate.py` here, 28 mutants, each built and run against the whole suite on gcc-debug and clang-debug, in the normal mode and in the run-time mode (the static_asserts of `mv_test.cpp`, `rational_test.cpp`, and `blade_set_test.cpp` set aside). A build that fails only on warnings counts as a survivor; none did. CTest is not run after a failed build. Job pool: 23 of 23 slots free before and after each run.

## Runs

| File | Revision | Mutants | Result |
| --- | --- | --- | --- |
| `results-152514a-normal.tsv` | `152514a` | all 28 | killed on both presets except `shared_zero_initializer` (clang-debug) and `rational_no_sum_guard` (clang-debug) |
| `results-152514a-runtime.tsv` | `152514a` | all 28 | as above, and also `ring_scalar_no_unary_minus` (both presets) and `shared_zero_initializer` (gcc-debug) survived |
| `results-bc1493e-rerun-normal.tsv`, `-runtime.tsv` | `bc1493e` (tests of `a6037b0`) | the three | `rational_no_sum_guard` and `ring_scalar_no_unary_minus` killed on both presets in both modes; `shared_zero_initializer` killed on gcc-debug in both modes, survives on clang-debug |

## The survivors and what was done

1. `rational_no_sum_guard`, clang-debug, both modes, at `152514a`. Without the sum's guard, `max + 1` and `2^62 + 2^62` wrap to the minimum of `std::int64_t`, which the constructor's own check rejects with the same message, so the run-time cases still abort with it. In the constant-evaluation case, Clang 23.1.4 still printed the call of `rational_failed` with the message after the overflow, so `support.rational_consteval_add` passed too (observed only; GCC 16.2.1 stopped at the overflow and the test failed). Fixed by a case whose wrapped sum no check catches: `support.rational_rejects_add_far_above`, `max + max`, which wraps to -2 (`a6037b0`). Killed at `bc1493e`.
2. `ring_scalar_no_unary_minus`, both presets, run-time mode. Every check of the concept was a static_assert. `a6037b0` repeats the accepted and rejected types in run-time CHECKs. Killed at `bc1493e`.
3. `shared_zero_initializer`, gcc-debug, run-time mode, at `152514a`: the constant evaluation that triggers GCC's defect was a static_assert, which the mode sets aside. `a6037b0` makes it a constexpr variable. Killed at `bc1493e`.
4. `shared_zero_initializer`, clang-debug, both modes: equivalent. The mutant restores `coefficients{}`, which gives the same value as `coefficients{zero_at(I)...}` by the language rules; only GCC 16.2.1 miscompiles it (`docs/agent/records/spikes/gcc-shared-array-initializer/`). Clang compiles both correctly, so no test on Clang can tell them apart.

## Kills by the plan's list

Each of the plan's mutants (section 8, WU8a) is in the driver; the test that kills it, from the normal-mode records:

| Plan's mutant | Driver name | Killed by |
| --- | --- | --- |
| each set check removed | `no_range_check`, `no_sign_check`, `no_repeat_check` | `core.mv_beyond_n*`, `core.mv_bad_sign*`, `core.mv_repeated_*` |
| the repeat scan starting two places on | `repeat_scan_two_on` | `core.mv_repeated_mask`, `core.mv_repeated_orientations` |
| the repeat test comparing (mask, sign) | `repeat_compares_orientation` | `core.mv_repeated_orientations` |
| a sign of 0 accepted | `sign_zero_accepted` (in `is_sign`, shared with merge and index_of) | `core.mv_bad_sign`, and the merge and index_of sign tests |
| get ignoring the sign | `get_ignores_sign` | core.mv's has_blade assertions; run-time mode: `core.mv_get_opposite` |
| get one position off | `get_one_place_off` | core.mv's named-access assertions and CHECKs |
| get returning the first blade with the mask regardless of orientation | `get_ignores_sign` (the lookup takes the first blade with the mask) and `get_orientation_as_absent` | as above; `core.mv_get_opposite` |
| has_blade raising an error for an invalid name | `has_blade_errors_on_invalid_name` | the build of core.mv, at its has_blade assertions for x1 and e0 |
| equality skipping the last coefficient | `equality_skips_last` | core.mv, the equality test at the last place |
| the count catch-all accepting K - 1 | `count_accepts_one_short` | `core.mv_one_too_few` (and `mv_too_few`, `mv_too_many`, whose required "3 == 6" and "7 == 6" no longer print once the condition is not a single comparison) |
| the K-parameter constructor not explicit for K = 1 | `one_value_implicit` | core.mv's `!is_convertible` assertion; run-time mode: `core.mv_scalar_implicit` |
| the storage's default member initializer removed | `no_default_initializer` | core.mv's zero assertions; run-time mode: the zero CHECKs over the 0xA5 pattern |
| the rational without normalization, with a negative denominator kept, or with the overflow guard removed | `rational_not_normalized`, `rational_negative_denominator_kept`, `rational_no_sum_guard`, `rational_no_product_guard`, `rational_min_accepted` | support.rational's hand values; the `support.rational_rejects_*` and `support.rational_consteval_*` tests |
| ring_scalar without its unary-minus requirement | `ring_scalar_no_unary_minus` | core.mv's concept assertion; run-time mode (from `a6037b0`): its CHECK |
| blades<Alg> reading n from the wrong place (a fixed 4) | `blades_fixed_four` | blade_set_test.cpp and mv_test.cpp, at names of n other than 4 |

Beyond the plan's list: `no_scalar_check`, `range_ignores_algebra`, `exact_scalar_ignores_limits`, `multivector_keeps_reference`, `shared_zero_initializer`, `absent_message_lost`, and `one_error_inert` (the compile-fail helper's one-error option made inert, killed by `core.mv_two_errors`).

## After the review fixes (`9b42298`)

`results-9b42298-review-{normal,runtime}.tsv`: the driver updated for the reviewed header (36 mutants, the eight new ones from the reviews' findings), on gcc-debug and clang-debug in both modes. Every mutant is killed by a test on both presets in both modes, except `shared_zero_initializer` on clang-debug, equivalent there as before. No build failed on warnings alone. The review's survivors, now killed:

| Mutant | Killed by (normal mode; the run-time mode agrees or kills at run time) |
| --- | --- |
| `mv_alg_unconstrained` (mv's `metric_like Alg` made `class Alg`, at its three declarations) | `core.mv_not_metric_like`, which requires the compiler's "does not satisfy 'metric_like'" or "‘metric_like<Alg>’", not a word of its path |
| `blades_unconstrained` | `core.blade_set_blades_not_metric_like` and `core.blade_set_names_zero` |
| `ring_requires_equality` | core.mv's two-lane scalar (`static_assert(core::ring_scalar<two_lanes>)` and mv's scalar check) |
| `equality_requires_removed` | core.mv's `!std::equality_comparable<Line<two_lanes>>`; run-time mode: its CHECK |
| `catchall_k1_not_explicit` | core.mv's `!std::is_convertible_v<T, Line<T>>`; run-time mode: its CHECK |
| `catchall_counts_ignored_for_one` | `core.mv_one_value` ("1 == 6") |
| `rational_product_no_cancel` | support.rational's (2^62 / 3)(3 / 2^62) == 1 |
| `literal_in_path_check_inert` | `tools.compile_fail_literal_in_path_rejects` |

## After the storage base took the algebra, layout, and scalar (`af447ae`)

`results-af447ae-storage-normal.tsv`: the mutants of the storage base and the class head (`mv_alg_unconstrained`, `no_default_initializer`, `shared_zero_initializer`, `one_value_implicit`, `equality_skips_last`, `equality_requires_removed`), normal mode, gcc-debug and clang-debug: all killed by tests, except `shared_zero_initializer` on clang-debug, equivalent there.
