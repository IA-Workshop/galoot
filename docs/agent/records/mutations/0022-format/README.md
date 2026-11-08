# Mutants of the formatter, the catch-alls of WU15, and the catalogue's scan

Plan 0022 section 6 and criterion 8. `mutate.py` applies one mutant at a time to a header and builds that header's test programs with `ninja -k 0`. The groups are:
- `format.hpp`: `core_format_test`, `core_format_outside_core_test`;
- `mv.hpp`: `core_mv_test`;
- `linear.hpp`: `core_linear_test`;
- `blade_set.hpp`: `core_blade_set_test`.

It then runs every test of the group's label (`core.format`, `core.mv`, `core.linear`, `core.blade_set`), the compile-fail tests included, and restores the header. Results are in `out/gcc-debug.tsv` and `out/clang-debug.tsv`; each build and test is a `mutant:<name>` row in `runs.tsv`. `scan_mutants.py` runs mutants of `records/diagnostics/catalogue/scan.py` against its `--self-test`; its result is `out-scan.txt`.

Run by the WU15 implementer (Claude, agent, Opus 5.5) in the worktree of `feature/core-format`, which no other build used:
- the 18 format mutants at `ca81991`, whose format.hpp is that of `02a7580`, the final one;
- the 19 part 3 mutants at `17c9baa`.

After the hci reading, at `1d494bb` (the scalar's formatter judged by `std::semiregular`, and four messages reworded), the same implementer ran again on both presets every format mutant, a new one, `scalar_formatter_by_formattable`, which restores the `std::formattable` test, and the four part 3 mutants whose anchors or messages changed: `other_scalar_rejection_removed`, `other_source_messages_swapped`, `plus_and_minus_messages_swapped`, and `not_an_algebra_rejection_removed`. Their rows in `out/` are from that run; the other part 3 rows are from `17c9baa`. `zeros_skipped` first failed to build, because its `==` does not compile for the new test's scalar, `ratio`, which has none. That was a fault of the mutant, so it now skips zeros only for a scalar with `==`, and was run again on both presets.

After the blind review, at `29fb0a6`, the same implementer added the review's two surviving mutants, `parse_skip_removed` (format.hpp) and `number_operand_arithmetic_only` (linear.hpp), with the two tests that kill them, `core.format_scalar_without_formatter_spec` and `core.linear_plus_number_rational`, and ran both mutants on both presets. Each was killed by its new test alone, which confirms the gap the review found. The other rows were not rerun, so they do not list the two new tests.

One format mutant, `canonical_index_order_reversed`, first failed to build through a dangling view in the mutant itself. That was a fault of the mutant, not a kill, so it was rewritten and run again on gcc-debug; clang-debug ran only the rewritten form.

## Result

| Mutants | gcc-debug | clang-debug |
| --- | --- | --- |
| format.hpp (20) | 20 killed | 20 killed |
| mv.hpp, linear.hpp, blade_set.hpp (20) | 19 killed, 1 equivalent | 19 killed, 1 equivalent |
| scan.py (9) | 9 killed (Python) | — |

Format mutants and what killed them:

| Mutants | Killed by |
| --- | --- |
| names (`minus_dropped`, `tag_name_for_either_orientation`, `tag_names_ignored`, `canonical_index_order_reversed`) | `core.format` |
| `wide_algebra_printed` | `core.format_wide_algebra`, `_auto` (the source then compiles) |
| form (`zeros_skipped`, `canonical_order_for_layout_order`, `specification_not_forwarded`, `colon_changed`, `comma_changed`, `empty_layout_writes_nothing`) | `core.format` |
| each rejection removed, and the two message swaps | the compile-fail tests of that rejection |
| `names_checked_as_none` | every `core.format_names_*` test |
| `parse_skip_removed` | `core.format_scalar_without_formatter_spec`: `{:.2f}` then fails inside `<format>` |
| `scalar_formatter_by_formattable` | `core_format_test` fails to build: its accept test of a scalar whose formatter takes `std::format_context&` meets the rejection's message |

Part 3 mutants and what killed them:

| Mutants | Killed by |
| --- | --- |
| `other_source_scalar_only`, `other_source_algebra_only` | the accept test's probes of a derived source (`mv_test.cpp` static_asserts), and every compile-fail test of the lost cause |
| each `mv` rejection removed, and the swapped messages | the `core.mv_other_*` tests |
| `number_operand_any_type` | `linear_test.cpp`'s probes of sums of two multivectors (the catch-all for a number then also takes them, so the call is ambiguous), and `core.linear_mixed_*` |
| `number_operand_arithmetic_only` | `core.linear_plus_number_rational`: a rational then gets the compiler's error |
| `number_operand_not_ring` | `linear_test.cpp`'s probe that a type of the user's without arithmetic has no sum with a multivector |
| `number_operand_multivectors_too` | `core_linear_test` fails to build: the constraint then depends on itself in the check of `ring_scalar` (`mv.hpp:70`), a hard error for valid code |
| each sum rejection removed, and the swapped messages | the `core.linear_*_number_*` tests |
| `blades_constrained_by_metric_like`, each `blades` rejection removed, `no_generators_case_lost`, `blades_messages_swapped` | `core.blade_set_blades_*`, `core.blade_set_names_zero` |

## The equivalent survivor

`other_source_widened_to_same_types` drops the constraint `!same_as<OtherAlg, Alg> || !same_as<U, T>` from the rejecting constructor, and nothing changes. A source of the same type reaches the copy constructor, which is not a template and wins. A source of the same algebra and scalar on another layout reaches the converting constructor or the lossy constructor, whose parameter fixes Alg and T, so partial ordering makes them more specialized than the rejecting one. The constraint is kept because it states the constructor's purpose where it is declared, and the code does not lean on partial ordering to read correctly.
