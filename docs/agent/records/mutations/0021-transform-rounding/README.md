# Mutants of transform's rounding check (WU13b)

Plan 0021 section 6 (WU13b) and criterion 6. `mutate.py` applies one mutant at a time to `include/galoot/core/transform.hpp`, builds the programs that judge it (`core_transform_rounding_test`, the rounding checking-mode programs `core_transform_rounding_mode_1` and `_0`, `core_transform_test`, the exact checking-mode programs, and `core_transform_identities_test`) with `ninja -k 0`, runs every test labeled `core.transform` (the programs and every compile-fail test, the rounding tests among them, which also carry `core.transform`), and restores the header. Results: `out/gcc-debug.tsv` and `out/clang-debug.tsv` (mutant, build exit, each failed build with its first error line, the failed tests); every build and test is a `mutant:<name>` row in `runs.tsv`.

Run by the WU13b implementer (Claude, agent, Opus 5.5) at `ce5449a` (the script's commit; the header is that of `c2af6a8`), in the branch's own worktree, gcc-debug first and then clang-debug, so that no two runs edited one header. `cb04901` later added a clang-tidy suppression comment to the header, which changes no code and touches no line a mutant edits.

## Result

17 mutants, each on gcc-debug and clang-debug, with the same outcome on both: 16 killed by a test, one equivalent survivor. A build failure counts only when it is a test's own `static_assert` or a constant evaluation the test asks for; none is a warning made an error.

| Mutant | Killed by (both compilers) |
| --- | --- |
| `tolerance_halved` | the hand case's `static_assert` (tolerance = 2 fl(γ_4), `transform_rounding_test.cpp`) |
| `tolerance_zero` | the hand case's `static_assert`; `core.transform_rounding_checking_tenth`, `_underflow_double`, `_underflow_float` |
| `margin_0` (γ_K) | the hand case's `static_assert` |
| `margin_2` (γ_(K+2)) | the hand case's `static_assert` |
| `k_undercounted_second_stage` | the hand case's count `static_assert` (K = 3) |
| `k_coefficient_product_uncounted` (a coefficient other than ±1 adds no rounding) | `core.transform_rounding` (inferred: the core's K against the oracle's, in the forms with coefficients other than ±1; the run records only the failing test) |
| `k_first_term_counted` | the count `static_assert`; `core.transform_rounding` |
| `magnitude_from_computed_middle` (the second stage reads \|computed middle\|) | `core.transform_rounding` (tolerance below γ_K M) |
| `padding_removed` | the constant evaluation of the underflow case in `transform_rounding_test.cpp`; `core.transform_rounding`, `core.transform_rounding_checking_underflow_double`, `_float` |
| `magnitude_coefficient_ignored` (\|c\| not applied in the magnitude) | `core.transform_rounding` |
| `nan_dropped_passes` (a NaN dropped coefficient skipped) | `core.transform_rounding_checking_nan` |
| `tolerance_finite_removed` (no `tol <= max`) | `core.transform_rounding_checking_infinite`, `_overflow` |
| `rounding_check_removed` | both constant-evaluation tests and every mode-1 abort test |
| `float_unchecked` | `core.transform_rounding_consteval_float`, `core.transform_rounding_checking_not_a_versor_float` |
| `exact_check_removed` (the exact path, rearranged by this change) | the exact check's constant-evaluation, abort, and recorded-comparison tests |
| `fast_math_error_removed` | `core.transform_fast_math` |

**Equivalent survivor:** `margin_hypothesis_removed`, the `static_assert` that (K + 1)(2K + 1)u < 1 for every dropped coefficient. No transform that builds comes near it (decision 0180: the largest K is about 130 at n = 6, and the limit is 2,895 for float), so no test can make it fire; it states the derivation's hypothesis in the code.

Notes. Plan 0021's "K undercounted by one for the second stage" is `k_undercounted_second_stage`, one rounding fewer for each second-stage term. The test author's other form of "a NaN passes", `!(|d| > tol) && tol <= max`, survives against its stand-in and is equivalent by its argument: the padded magnitude bounds every computed value, so a NaN dropped coefficient brings a NaN or infinite tolerance, which `tol <= max` rejects (not checked here). A mutant that checks the kept coefficients in place of the dropped ones does not compile in the header, since the two arrays differ in size, so a build failure would not be a test's kill; it was not run here (the test author ran a form of it against its stand-in).

## After the blind review

The review (`records/reviews/2026-11-07-wu13b-review.md`) ran 15 mutants of its own; two survived as test gaps (`size_signed`, `middle_magnitude_restricted`). After the fixes (`e188522`, tests `fbd8a11`), the script was extended and every mutant ran again at `f3c6b23`, gcc-debug and then clang-debug, in this worktree. `out/*.tsv` now hold these runs; the rows of the first run are in the history of this directory. `margin_hypothesis_removed` is gone: the guard became `transform_rounding_checked`, which decides whether a transform is checked, and its mutants now die.

22 mutants, each on gcc-debug and clang-debug, with the same outcome on both: 21 killed by a test, one equivalent survivor. The 16 mutants of the first run die as before (the hand case's `static_assert`s, now at `transform_rounding_test.cpp:865` and `:874`, and the same CTest tests, plus the new float and negative cases where they apply). The new ones:

| Mutant | Killed by (both compilers) |
| --- | --- |
| `size_signed` (the check compares d, not \|d\|; the reviewer's survivor) | `core.transform_rounding_consteval_negative_double`, `_float`, `core.transform_rounding_checking_negative_double`, `_float` |
| `magnitude_middle_restricted` (the magnitude's first stage restricted to the terms the kept outputs read; the reviewer's `middle_magnitude_restricted`) | `core.transform_rounding` (the degenerate form with e1 · e2 = 1, V on e1, e2, e3, e123, x on e123: tolerance below γ_K M) |
| `finite_math_error_removed` | `core.transform_finite_math_only` |
| `fast_math_error_removed` (now: the `-ffast-math` branch removed, so `-ffast-math` reaches the `-ffinite-math-only` error) | `core.transform_fast_math`, which forbids the other error's text |
| `checked_ignores_margin` | the test's `static_assert(!transform_rounding_checked<rga3, motor_layout, point_layout, coarse>)` (`transform_rounding_test.cpp:1175`), for the test-only scalar of 8 significand bits |
| `margin_holds_always` | the same |

**Equivalent survivor:** `padding_lambda`, the padding λ of the first version in place of λ/u. Both paddings are sound (decision 0180's addendum, item 3); they differ in run time and in the tolerance's floor below about K λ W, which no test pins, since no requirement fixes either.
