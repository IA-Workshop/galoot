# WU12 mutants: the change of basis

Plan 0030 section 8 and criterion 12. Run by Claude (agent, Opus 5.5, implementer of plan 0030 part E) on 2026-11-09 with `tools/mutate`, on gcc-debug (g++ 16.2.1) and clang-debug (clang++ 23.1.4), at the tip `752fdea` (whose `include/` and `test/` are those of `404f2fc`) in the unit's worktree, and at the branch point `1818efe` (the unit with part A merged, before part B's tests and part C's header) in a worktree detached at that revision, `.claude/worktrees/core-basis-change-bp`, which no other workstream used and which was removed afterwards. The job pool read 23 of 23 slots free before and after each campaign. Each run's rows are in `out/<spec>-<preset>[-runtime].tsv`; the branch point's in `branch-point/out/`, its run log in `branch-point/runs.tsv`; the tip's builds and tests are rows `mutant:<name>` of `records/performance/runs.tsv`.

## Summary

| Spec | Judged by | Mutants | gcc-debug | clang-debug |
| --- | --- | --- | --- | --- |
| `change.toml` | `^core\.basis_change` (both programs and the 25 compile-fail tests) | 15 | 15 killed | 15 killed |
| `change.toml --runtime` | the same, the layout `static_assert`s of `basis_change_test.cpp` turned off | 15 | 15 killed | 15 killed |
| `naturality.toml` | `^core\.basis_change_identities$` (factors), `^core\.basis_change$` (inequalities) | 19 | 19 killed | 19 killed |
| `ldlt.toml` | `^support\.diagonalize$` | 3 | 3 killed | 3 killed |
| `ldlt.toml --runtime` | the same, the Witt form's `static_assert`s turned off | 3 | 3 killed | 3 killed |
| `rounding.toml` | `^core\.basis_change$` | 3 | 3 killed | 3 killed |
| `branch-point/gaps.toml`, at `1818efe` | `^core\.(identities\|identities_rounding\|dual_identities\|transform_identities)$` | 3 | 3 survived | 3 survived |
| `gaps.toml`, at the tip | the same suites and `^core\.basis_change_identities$` | 3 | 3 killed | 3 killed |
| `gaps.toml --runtime` | the same, the kernel sources' `static_assert`s of det P and the contragredient condition turned off | 3 | 3 killed | 3 killed |
| `new_cases.toml` | as each mutant's selection says | 14 | 14 killed | 14 killed |
| `new_cases.toml --runtime` (three diagonalize mutants) | `^support\.diagonalize$`, the Witt form's `static_assert`s turned off | 3 | 3 killed | 3 killed |

No mutant survived where it was expected to die, none was unjudged, and no run-time edit leaked. The three branch-point survivals are the expected result (a gap mutant must survive there to count); `tools/mutate` exits 1 on them because its spec expects a kill, which is the expectation of the tip.

## The gaps (criterion 12)

The predictions are `gap-predictions.md`, written by part A before any run. Each mutant counts: it survives the four identity suites at the branch point on both presets and dies at the tip on both.

| Mutant | Predicted, branch point / tip | Observed, branch point | Observed, tip | How the tip kills it |
| --- | --- | --- | --- | --- |
| `conformal_pairing_positive` | survives / killed (assert) | survived, both | killed, both | first error: the kernel sources' own `static_assert(bc::dual_isometry<...>)` of the conformal changes (`basis_change_conformal3_test.cpp:51`, `basis_change_conformal_test.cpp:30`), which the tests compute from the tag's rows and the stored forms; with those turned off (`--runtime`), the core's isometry check, `basis_change.hpp:571`, on both presets |
| `diagonal_negative_first` | survives / killed (assert) | survived, both | killed, both | the same, and `basis_change_witt_test.cpp:49`; under `--runtime`, the core's isometry check |
| `witt_pairing_doubled` | survives, uncertain / killed (assert) | survived, both: no overflow of the test rational in the identity suites | killed, both | first error: the tests' `static_assert`s of det P for the random forms (`basis_change_random_five_test.cpp:39`, `basis_change_random_test.cpp:63`) and of the Witt changes; under `--runtime`, the core's isometry check (`:571`) and, first in the source of the dense forms, the catch-all `change_basis: the template argument is not a change of basis` (`:1071`), so that tag is not `basis_change_like` under the mutant (inferred, not traced: most likely its diagonal counterpart's squares leave the range of a Gram entry, so the target is not `metric_like`) |

So each prediction held, with one difference of mechanism: at the tip the first error is the tests' own compile-time statement of det P or of the contragredient condition, which reads the mutated stored form, before the core's check is reached. The `--runtime` runs show that the core's check kills each mutant without them. Every kill lies in `core.basis_change_identities`, the suite the gaps extend.

The fourth row of the coverage table, the test-side LDLᵀ, has no counting mutant, as `gap-predictions.md` argues (an LDLᵀ test derives its counterpart and its change from the same stored form, so a consistent mutant keeps every assertion true). No diagonal counterpart is built with `R()`: `basis_change_forms.hpp`'s `to_diagonal` uses the diagonal of `diagonalize` through `diagonal_metric`, so the exception that prediction names does not arise. This is the expected result, not a failure.

## What each spec showed

- **`change.toml`.** Every check of decision 0220 section 3.4 is load-bearing: removing the counts, the denominators' sign, the independence, or the isometry check, or swapping its two forms, fails the compile-fail tests of those messages; `checked_arithmetic_unchecked` fails `core.basis_change_overflow` (the overflow is then the compiler's own "not a constant expression", without the message). The term generation's mutants (the minor's sign, rows for columns, the denominators, the grade-0 image, the inverse as a transpose) fail the golden images, the hand values, and the isomorphism at both layers. Both elimination mutants are stopped by the core's own checks: `elimination_swap_skipped` by a division by a zero pivot (`0 / 0`, first error in the independence check's constant), `elimination_division_dropped` by the self-check that the inverse inverts the rows (`basis_change.hpp:577`).
- **`--runtime` of `change.toml`.** Two mutants (`minor_sign_dropped`, `grade_zero_image_dropped`) are still first stopped at `basis_change_test.cpp:256`: the layout comparison there becomes `true || a == b`, but `a` and `b` then have different types, so the expression does not compile. Their run-time kills are in `core.basis_change_identities` in the same run.
- **`naturality.toml`.** Each factor of the agreement checks is load-bearing, and so is each inequality of the basis-dependent operations. 1/d against d is told apart for the antiwedge and the bulk dual by the random forms (det P of 1/2, 2/7755, 5/18, …), and for the antiproduct family and the weight duals only by rigid(3) with e4 = 2 e′4, as research note 0026 section A4 predicts: `antiproduct_factor_d` and `left_weight_dual_factor_d` fail only "the Witt, degenerate, and rigid changes preserve what they must".
- **`ldlt.toml`.** All three rules of the elimination are load-bearing. Without `--runtime`, `pairing_skipped` and `squarefree_scaling_skipped` are first stopped by the Witt form's `static_assert`s in `diagonalize_test.cpp`; with them off, "two null vectors are paired, and the radical stays" and "each square is scaled to the squarefree part of p q" fail at run time.
- **`rounding.toml`**, against the tolerance record's predictions (section "rounding.toml"):
  - `division_per_term`: predicted killed by the small dyadic inputs of a change whose denominators are not powers of two, surviving the dyadic changes and the wide inputs. Killed, and "a diagonalizing change over double rounds only its division on small dyadic inputs" is among the failed cases, as predicted. The wide-input case is also listed; that listing is not evidence either way (see "Reading the failed cases").
  - `division_rounding_uncounted` (the record's "division term dropped"): the record's correction 2 predicted rejection where an output has one or two roundings. Killed by "a change of basis over double is within its forward error bound on wide integer inputs" alone, as predicted; the small dyadic cases check correct rounding directly, not through the count, so they cannot see it.
  - `double_results_scaled` (× (1 + 2⁻⁴⁸)): killed by "conformal_change over double, float, and complex is exact on small dyadic inputs", which stops the program: such a value has a denominator above 2⁶², which `exact_rational` refuses, so the later cases did not run.
  Decision 0222 item 12 asks that predicted survivors here not be counted as gaps; none survived.
- **`new_cases.toml`.** Written after the first runs, so that every test case and test that parts B and C added kills a mutant on both presets (criterion 12's first clause). With the other specs, every new `TEST_CASE` and every new CTest test does, on both presets. One case is killed only at compile time: "change_basis over std::int64_t where no division is needed" fails to build under `denominator_unreduced`, the integer-division rejection firing on its own call (the failing object is `basis_change_scalars_test.cpp`'s), because the reduction of q_B with its terms is exactly what lets e45 and e12345 map without a division; no run-time behavior of an integer scalar differs otherwise. The case "The products of five algebras equal their matrix representations" (`blade_product_test.cpp`) is killed by `diagonal_negative_first` judged by `core.blade_product`.

## Reading the failed cases

`tools/mutate` lists as failed cases every `TEST CASE:` header in the output of a failed CTest test. doctest prints that header for a `MESSAGE` too, and "a change of basis over double is within its forward error bound on wide integer inputs" always prints its largest ratios with `MESSAGE`. So whenever `core.basis_change` fails for another reason and gets that far, that case is listed. Its own kill is `division_rounding_uncounted`, where it is the only case listed and the program failed, so a case of its own must have failed.

## Not done

- No mutant of `basis_change.hpp`'s concept `basis_change_like`; its accepting and rejecting `static_assert`s in `basis_change_test.cpp` pin it, as for the core's other concepts.
- The branch point ran the gap mutants only; the other specs mutate code or tests that do not exist there.

## The seed campaign (criterion 14)

Both new randomized programs, `core_basis_change_test` (its scalar cases draw from doctest's seed) and `core_basis_change_identities_test` (the kernel groups), ran with `--rand-seed=1` to `50` on gcc-debug at `752fdea`, each run through `tools/timed --log seeds/runs.tsv` with the step `seed`, on 2026-11-09 from 15:22:19 to 15:22:34 UTC. All 100 runs passed (exit 0), so no failure became a fixed test or a defect report. Median wall times 0.056 s and 0.172 s. The draws are integers from −3 to 3 for the kernel groups, inside the overflow bound of research note 0026 section F2 (provably safe up to ±9 for every traced computation), so no seed is expected to overflow the test rational; the campaign supports that, as a sample (V9).

## After the blind review

The blind review (`records/reviews/2026-11-09-wu12-blind-review.md`) found two survivors of its own spec, F1 and F2, and noted untested claims, N4. Four test commits answer them: `0c5f0e0` (the compile-fail test `core.basis_change_pairing_not_kept`), `cc923ac` (`change_basis_back` over `std::int64_t` on e′1 e′2 e′3), `a83eec3` (transform by a flector, and the projection and antiprojection, with 1/d², in `check_transform`), and `9cd86f0`, which corrects `a83eec3`: the antiprojection of a vector onto a hyperplane is zero once n ≥ 3 (x ∨ H☆ has grade 2 − n), so it now takes the hyperplane's antiprojection onto the vector. A first run of the antiprojection's factor mutant at `a83eec3` was killed only by the one-mode Witt change (n = 2), which showed the zero; its rows were replaced by the run at `9cd86f0`.

`review_fixes.toml` runs the reviewer's two survivors unchanged and three edits of the new expected factors, at `9cd86f0` (the records only dirty) on both presets: all 5 killed on each (`out/review_fixes-<preset>.tsv`).

| Mutant | Killed by, both presets |
| --- | --- |
| `isometry_diagonal_only` (F1) | `core.basis_change_pairing_not_kept`, the spec's declared killer |
| `inverse_sign_not_moved` (F2) | the rejection `change_basis_back: the change divides by an integer …` firing at compile time in `basis_change_scalars_test.cpp`, from the new integer call |
| `flector_transform_factor_one_over_d`, `project_factor_one_over_d`, `antiproject_factor_one_over_d` | the three cases that call `check_transform` (every change there has d = −1 or 2, where 1/d and 1/d² differ) |

## After the review rulings (decision 0223)

The header change `30c2275` (one error per tag, the tag checks noting an overflow instead of stopping, the reworded messages, the value-as-tag catch-alls) moved or reshaped the code that eight mutants of `change.toml` and one of `review_fixes.toml` edit, so their anchors were rewritten for the new form, each keeping its fault: `columns_for_rows` (the two reads in `blade_image`, since the inverse's rows now also contain `rows.rows[i][j]`), `inverse_as_transpose`, `isometry_sides_swapped` and `isometry_check_removed` (in `check_change`), `independence_check_removed`, `count_checks_removed` (both counts and both guards, the target's now nested), `denominator_check_removed`, `checked_arithmetic_unchecked` (the product's overflow test of `multiply_noting`, which both the tag checks and the term generation use), `elimination_division_dropped`, and `isometry_diagonal_only`. `change.toml` (also `--runtime`), `new_cases.toml`, and `review_fixes.toml` then ran again at `0fa98ac` (the records only dirty) on both presets: 15, 15, 14, and 5 killed on each, nothing survived or unjudged. The rows replace those of the earlier runs in `out/`. `count_checks_removed` is now also killed by `core.basis_change_both_counts`, and `checked_arithmetic_unchecked` and `isometry_check_removed` by `core.basis_change_overflow`, which now asks for one error with the message first. `naturality.toml`, `ldlt.toml`, `rounding.toml`, and the gap specs were not rerun: they edit tests, the test support, `run_change`, and `metric.hpp`, none of which the rulings changed.
