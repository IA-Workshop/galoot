# Plan 0039 mutants: `same_rows`, the reduced rows, and `.fits = fits`

Plan 0039 section 8. Run by Claude (agent, Opus 5.5, implementer of plan 0039) on 2026-11-10 with `tools/mutate`, on gcc-debug (g++ 16.2.1) and clang-debug (clang++ 23.1.4), at `fa86535` (the header of `38925ad`, the records only dirty) in the unit's worktree, and at the branch point `3ddc371` (whose `include/` and `test/` are those of `ca6939d`) in a worktree detached there, `.claude/worktrees/basis-change-rows-bp`, which no other workstream used. The job pool read 23 of 23 slots free before and after each run. Rows: `out/<spec>-<preset>.tsv`, and `branch-point/out/`; the branch point's run log is `branch-point/runs.tsv`, the tip's rows `mutant:<name>` of `records/performance/runs.tsv`. The header changed after these runs only in comments (`42b2c22`), so they were not repeated.

`witnesses.py` is a transcription of `check_change` written from the header, with the reduction as an option; `witnesses-out.txt` is its output, the stage at which each input of the overflow tests is rejected as written and reduced (plan 0039 section 5.4).

## Summary

| Spec | Where | Mutants | gcc-debug | clang-debug |
| --- | --- | --- | --- | --- |
| `rows.toml` | tip | 7 | 6 killed, 1 equivalent | 6 killed, 1 equivalent |
| `gaps.toml` | tip | 1 | killed | killed |
| `branch-point/gaps.toml` | `3ddc371` | 1 | survived, as declared | survived, as declared |
| `rerun-0030.toml` | tip | 13 | 12 killed, 1 equivalent | 12 killed, 1 equivalent |

No mutant survived where it was expected to die, and none was unjudged.

One fault of the spec, not of the tests: the first run of `rows.toml` and `gaps.toml` declared `killers = ["core.basis_change"]`. A kill at compile time is named by the place of the failed assertion, not by a CTest test, so every killed row also lists "declared killer core.basis_change did not kill it" in its problems column and `tools/mutate` exited 1. The declarations were dropped from the specs afterwards (the blind review's N2 found the same); the verdicts are as the rows say.

## Each mutant and the test it was written for (W3)

| Mutant | Killed by, both presets (first error) | Test it was written for |
| --- | --- | --- |
| `rows_not_reduced` | the overflow `static_assert` of the tag checks, `basis_change.hpp:740`, in `basis_change_test.cpp`'s object, from the scaled changes | criterion 3, "rows with a large common factor" |
| `reduction_keeps_denominator` | `basis_change_test.cpp:292`, the reduction's hand values | criterion 2 |
| `reduction_gcd_without_denominator` | `basis_change_test.cpp:292`; also the isometry check of the random forms and four compile-fail tests | criterion 2 |
| `forward_rows_as_written` | the term generation's guard (`change_multiply`), from the image of e123 under m e'_i over m: GCC at `assert.hpp:71`, Clang at `basis_change.hpp:1002` (`count`) | criterion 3 |
| `same_rows_ignores_rows` | `basis_change_test.cpp:183` | criterion 1, other rows over equal denominators |
| `equality_restored` | `basis_change_test.cpp:187`, `!std::equality_comparable` | criterion 1 |
| `elimination_fits_hard_coded` | equivalent, as declared: the loop returns on `!fits` before either later return | N2 |
| `same_rows_ignores_denominators` (`gaps.toml`) | `basis_change_test.cpp:181`, equal rows over other denominators | criterion 1 |

Every new test is among the killers of a mutant written for it. Line numbers are those of `fa86535`.

## The gap

`core.basis_change` before the unit compared changes whose rows differed (`{{2, 2}, {1, -1}}` over `{4, 1}` against the Witt change) or whose rows and denominators were both equal, so an `==` that ignored the denominators passed it: `equality_ignores_denominators` survived at `3ddc371` on both presets. The same fault in `same_rows` dies at the tip on both, at the new `static_assert` of equal rows over other denominators. By the W3 rule that mutant counts for the gap.

## The 0030 mutants run again (V16)

`rerun-0030.toml` copies, with their names and faults, the mutants of `0030-basis-change/change.toml`, `review_fixes.toml`, and `delta-review/mutants.toml` whose code the unit edits (`check_change`, `eliminate`) or whose killers it changes (the new inputs of `core.basis_change_overflow` and `_overflow_elimination`). Two anchors were rewritten because `check_change` now passes the reduced rows (`isometry_sides_swapped`, `isometry_check_removed`). All are killed as before except `elimination_overflow_not_stopping`, declared equivalent and found so: once the later returns of `eliminate` state `fits`, an overflow that does not stop the loop still reaches the result as an overflow, since `fits` stays false, each divisor is a pivot the search found nonzero, and the noting arithmetic keeps every value in range. So the early exit and `.fits = fits` each make the other redundant; the unit keeps both, the exit to stop work and the field so that each return states its own truth. `isometry_overflow_holds` is killed by `core.basis_change_overflow` with its new input, and `isometry_diagonal_only` by `core.basis_change_pairing_not_kept`, the declared killers.

The other mutants of the 0030 specs edit code or tests that this unit does not change, and were not rerun.

## The seed campaign (criterion 8)

`core_basis_change_test` and `core_basis_change_identities_test`, each with `--rand-seed=1` to `50`, on gcc-debug at `42b2c22`, each run through `tools/timed --log seeds/runs.tsv` with the step `seed`, on 2026-11-10 from 13:43:43 to 13:43:57 UTC: all 100 runs passed. Median wall times 0.031 s and 0.191 s. No failure, so nothing became a fixed test.
