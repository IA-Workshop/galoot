# Mutants of compose, transform, and antiproject (WU13a)

Plan 0021 section 6 and criterion 11. `mutate.py` applies one mutant at a time to `include/galoot/core/transform.hpp` or `dual.hpp`, builds the programs that judge it (`core_transform_test`, `core_transform_identities_test`, the two checking-mode programs `core_transform_mode_1` and `_0`, `core_dual_test`, `core_dual_identities_test`) with `ninja -k 0`, runs every test labeled `core.transform` or `core.dual` (the programs and every compile-fail test of both labels), and restores the header. Results: `out/gcc-debug.tsv` and `out/clang-debug.tsv` (mutant, build exit, each failed build with its first error line, the failed tests); every build and test is a `mutant:<name>` row in `runs.tsv`.

Run by the WU13a implementer (Claude, agent, Opus 5.5) in two detached worktrees of `feature/core-transform`, one per preset, so that no two runs edited one header. 29 mutants ran at `1ca4508`. The other six ran again at `f2d8322`: the first run of five of them (`parity_by_grade`, `odd_sign_minus_for_every_n`, `odd_sign_plus_for_every_n`, `reverse_for_antireverse`, `no_involution_for_odd`) failed to build only through an unused-variable warning made an error, which does not count, so their edits now keep the variable used; and `f9ad5bc` (clang-tidy) rewrote the lines that `x_orientation_dropped` and the odd sign edit. Between `1ca4508` and `f2d8322` the header changed in comments and in that clang-tidy rewrite, which keeps every term; the other 29 were not rerun.

## Result

35 mutants, each on gcc-debug and clang-debug: 34 killed on both compilers, each by a test, and one equivalent survivor.

| Mutant | Killed by (both compilers unless stated) |
| --- | --- |
| `parity_by_grade` | the parity trait's static_asserts (`transform_test.cpp`), and `core.transform_identities` |
| `parity_accepts_mixed` | the parity static_asserts; `core.compose_mixed_parity_first`, `_second`, `core.transform_mixed_parity` |
| `parity_rejects_empty` | the parity static_asserts |
| `odd_sign_minus_for_every_n` (−V ⟇ x̂ ⟇ V̰ for every n) | `core.transform`, `core.transform_identities` |
| `odd_sign_plus_for_every_n` (+V ⟇ x̂ ⟇ V̰ for every n) | `core.transform`, `core.transform_identities` |
| `reverse_for_antireverse` | `core.transform`, `core.transform_identities` |
| `no_involution_for_odd` | `core.transform`, `core.transform_identities` |
| `x_orientation_dropped` (the canonical copy without x's orientations) | `core.transform` |
| `second_stage_drops_a_kept_term` | the kernel test's static_asserts (GCC); a build error in the kernel's array size (Clang, through the empty versor's test); `core.transform` and the checking-mode programs |
| `first_stage_drops_a_read_term` | as the previous one; the unchecked programs (`core.transform_unchecked_*`) see it, since checking mode computes the whole first stage |
| `compose_operands_swapped` | `core.transform`, `core.transform_identities` |
| `exact_check_removed` | `core.transform_consteval_nonzero`, `_rational`, `core.transform_nonzero_int64`, `_rational`, `core.transform_checking_recorded` |
| `exact_check_reads_kept` | the checking-mode program's own constant evaluation; `core.transform` |
| `exact_check_twice` | `core.transform_checking_recorded` |
| `exact_check_truncate_message` | the constant-evaluation and abort-message tests |
| `transform_parity_check_removed` | `core.transform_mixed_parity` |
| `transform_check_removed` | `core.transform_narrow_scalar_first_plan`, `_second_plan` |
| `transform_checks_first_plan_only` | `core.transform_narrow_scalar_second_plan` |
| `transform_checks_second_plan_only` | `core.transform_narrow_scalar_first_plan` |
| `compose_parity_check_removed` | `core.compose_mixed_parity_first`, `_second` |
| `compose_parity_checks_first_only` | `core.compose_mixed_parity_second` |
| `compose_check_removed` | `core.compose_narrow_scalar` |
| `compose_layout_is_input` | the layout static_asserts; `core.compose_compare` |
| `transform_catch_alls_without_algebra_condition` | the catch-all probes; `core.compose_mixed_algebras`, `core.transform_mixed_algebras` |
| `transform_catch_alls_without_scalar_condition` | the catch-all probes; `core.compose_mixed_scalars`, `core.transform_mixed_scalars` |
| `antiproject_operands_swapped` | the layout static_asserts (`dual_test.cpp`) |
| `antiproject_uses_bulk_dual` | the narrow-scalar case that only the second operand's weight dual reaches: bulk_dual's own check rejects it, so `dual_test.cpp` does not build |
| `antiproject_is_project` (wedge and antiwedge swapped) | the layout static_asserts |
| `antiproject_check_removed` | `core.dual_narrow_scalar_antiproject`, `_first_operand` |
| `antiproject_checks_first_operand` | `core.dual_narrow_scalar_antiproject_first_operand`, and the second-operand case of `dual_test.cpp` |
| `antiproject_checks_bulk` | both narrow-scalar tests, and the second-operand case |
| `antiproject_layout_is_input` | the layout static_asserts; `core.dual_compare_antiproject` |
| `antiproject_catch_all_without_algebra_condition` | the catch-all probes; `core.dual_mixed_algebras_antiproject` |
| `antiproject_catch_all_without_scalar_condition` | the catch-all probes; `core.dual_mixed_scalars_antiproject` |

**Equivalent survivor:** `compose_checks_reversed_plan` (the narrow-scalar check reads the plan of Q1 ⟇ Q2 in place of Q2 ⟇ Q1). For basis blades, e_B ⟑ e_A is ± the reverse of e_A ⟑ e_B, since the reverse of a product is the product of the reverses in the other order, and the reverse only changes signs; through the complements the same holds for the antiproduct. So the two plans have the same coefficients up to sign, and holds_exactly reads only magnitudes (|c|), so the checks agree on every scalar type.

Not run as mutants: plan section 6's "the second stage keeps every term" and "transform drops nothing and returns the plan's layout". The first changes no value (the extra terms are dropped afterwards) and only the cost, which the kernel test's term counts and the assembly counts (`records/performance/transform-cost/`) pin; the second changes the result type, which every `decltype` static_assert of `transform_test.cpp` rejects by construction.

## After the blind review

At `34cd05d`, in the detached worktree `core-transform--mutants`:

- The reviewer's surviving mutant `first_keeps_all` (the restricted first stage keeps every term), run with the reviewer's script (`records/reviews/2026-11-07-wu13a-review/mut.py`), is now killed on both compilers by the degenerate case added to the kernel test (`transform_test.cpp`, "the first stage keeps only the terms that the kept terms read"): its static_assert fails on gcc-debug and clang-debug. Its rows are below rather than in the reviewer's `mutants.tsv`, which stays as the reviewer left it:

  | Mutant | Preset | Build exit | First failed build | Failed tests |
  | --- | --- | --- | --- | --- |
  | `first_keeps_all` | gcc-debug | 1 | `transform_test.cpp:1346`: static assertion failed | - |
  | `first_keeps_all` | clang-debug | 1 | `transform_test.cpp:1343`: static assertion failed (the first kernel's size) | - |

- `exact_check_truncate_message` ran again, since the message it replaces changed (`9fe1d3d`); it is killed as before, by the constant-evaluation and abort-message tests, on both compilers (`out/*.tsv`).
