# Mutants of dual.hpp (WU11a)

Plan 0020 section 6, run by the WU11a implementer (Claude, agent, Opus 5.5) on the real header, after the test author's runs on a scratch stand-in (which are not reproducible as committed). `mutate.py` applies each mutant to `include/galoot/core/dual.hpp`, builds `core_dual_test` and `core_dual_identities_test` with `ninja -k 0`, runs the tests labeled `core.dual` (both programs and the 24 dual compile-fail tests), and restores the header. Every build and test is a row of `runs.tsv` with the selection `mutant:<name>`. Output: `out/gcc-debug.tsv` and `out/clang-debug.tsv` (mutant, build exit, first error line of each program that failed to build, failed tests).

Run at `6837785` to `4720980` (commits made during the run touched only records and the golden comments, not the header or the tests' code): gcc-debug first, then clang-debug. After the run, `505737e` rewrote how the antiscalar's layout is built (a function of n in place of a nested braced list, for clang-tidy) and `48172c4` linted the tests; neither touches a line a mutant edits or a value a test checks, so the mutants were not run again.

A mutant is killed by a test: a run-time check or a compile-fail test that fails, or a test's own code that stops compiling (a `static_assert` of the test, a comparison the test makes, or a call the test makes that the header then rejects). Where a build failed, the first error line is in the record; every one is in `test/core/dual_test.cpp`, or, for `dot_checks_whole_product`, the dot's own message raised by the test's call that must compile (the test of a scalar that holds the dot's coefficients but not the whole product's). No kill comes from a warning.

The mutants:

| Mutant | What it changes |
| --- | --- |
| `bulk_dual_uses_left_complement` | bulk_dual is left(bulk(x)), the left bulk dual |
| `bulk_dual_omits_reverse` | bulk_dual is x 𝟙 |
| `bulk_dual_antiscalar_first` | bulk_dual is 𝟙 x~ |
| `weight_dual_is_bulk_dual` | weight_dual is right(bulk(x)), with its layout |
| `weight_calls_bulk` | weight is bulk(x), with its layout |
| `weight_right_complement_outside` | weight is right(bulk(right(x))) |
| `left_weight_dual_uses_right_complement` | left_weight_dual is right(weight(x)) |
| `dot_omits_reverse` | the dot's kernel is applied to b, not reverse(b) |
| `kernel_keeps_antiscalar_terms` | the kernel keeps the terms on the antiscalar |
| `kernel_keeps_every_term` | the kernel keeps every term |
| `kernel_adds_first_term_to_zero` | no term is marked first |
| `antidot_without_complements` | antidot is left(dot(a, b)), with its layout |
| `antidot_right_complement_outside` | antidot is right(dot(right(a), right(b))) |
| `project_operands_swapped` | a ∨ (b ∧ weight_dual(a)), with its layout |
| `project_uses_bulk_dual` | b ∨ (a ∧ bulk_dual(b)), with its layout |
| `<f>_check_dropped` (9) | the narrow-scalar check of f is `true` |
| `<f>_checks_<g>` (10) | f checks another operation's coefficients: bulk and the bulk duals the weight's, weight and the weight duals the bulk's, dot the antidot's and the whole product's, antidot the dot's, project the bulk dual's |
| `catch_alls_without_{algebra,scalar}_condition` | the three catch-alls lose one half of their constraint |
| `<name>_layout_is_input` (9) | the layout name is the input's layout |
| `project_antiwedge_order_swapped` | (a ∧ weight_dual(b)) ∨ b, with its layout (blind review F1) |
| `kernel_check_any_held` | the kernel's check accepts when any kept coefficient fits (F2) |
| `project_checks_first_operand` | project checks the weight dual of its first operand (F3) |

## Results

45 mutants on each of gcc-debug and clang-debug. 44 are killed on both, each by a test; the record names the killing tests for each.

- The narrow-scalar mutants (19) are each killed by the matching `core.dual_narrow_scalar_<function>` test alone, on both compilers, except `dot_checks_whole_product`, which is killed by the accept test in `dual_test.cpp` (it stops compiling with the dot's own message).
- The catch-all mutants are killed by the requires-expression probes of `dual_test.cpp` and by the six `core.dual_mixed_*` tests.
- The layout-name mutants are killed by the layout static_asserts and, on Clang, also by the `core.dual_compare_*` tests, which need the layout's name on the first error line; on GCC, which prints values and not names, the compare tests cannot tell them apart.
- The value mutants are killed at run time by `core.dual` and, for most, `core.dual_identities`, or by the test's static_asserts. `bulk_dual_antiscalar_first` also makes the identities program exceed Clang's step limit (in the plan of 𝟙 with `full<5>` in reverse order), so on Clang it is caught by the build there and by `core.dual` at run time.
- `kernel_adds_first_term_to_zero` is killed by the kernel's own static_asserts, so `dual_test.cpp` does not build; with those asserts removed, "the dot keeps a negative zero" would be the run-time check (the test author's stand-in run found it kills this mutant; not rerun here).

One survivor, equivalent as plan 0020 section 6 expected: `antidot_right_complement_outside`. The right and the left complement of the scalar are both 𝟙 (the antiscalar's sign is +1 either way, since the scalar has grade 0), so right(dot(...)) and left(dot(...)) are the same value on the same layout.

No kill comes from a warning made an error: every first error line in the records is a `static_assert` or a comparison in the test's own code, or a test's call that the header rejects with its message.

## After the blind review

The review's own run of nine mutants found three survivors, added above as the last three rows and run at `9bd09dd`, against the tests added there, on gcc-debug and clang-debug: each is killed on both. `project_antiwedge_order_swapped` by "a line projected onto the plane z = 2" at run time; `kernel_check_any_held` by `core.dual_narrow_scalar_dot_mixed_kernel` and `core.dual_narrow_scalar_antidot_mixed_kernel`; `project_checks_first_operand` by `core.dual_narrow_scalar_project_first_operand` and by the accept test "project checks the coefficients of its second operand only", which then stops compiling with project's message. So 48 mutants in all, 47 killed on both compilers, one equivalent survivor.
