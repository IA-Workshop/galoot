# WU13a: the messages of transform, compose, and antiproject and their sizes

Plan 0021 reading R11 and criterion 9; decision 0179, which gives the texts. Each is pinned by a compile-fail test of `test/CMakeLists.txt` (blocks `core.transform` and `core.dual`) with `ONE_ERROR` and with `apply:`, `geometric_antiproduct:`, `a % b:`, `product_plan`, `detail::`, and `truncate:` forbidden on the first error line (the `core.dual` block's internal names for `antiproject`). Each source writes its result into a variable (`const auto r = ...;`), which adds no second error.

The `core.compose_compare` and `core.dual_compare_antiproject` tests compare a result with a value of a named layout; on Clang the first error line names `compose_layout<...>` or `antiproject_layout<...>`. The exact check's constant-evaluation tests (`transform_consteval_nonzero_*`) show the message on the echoed source line and require `assertion_failed`.

## Sizes

`wu13a/diagnostics.py` compiles each source with the test flags and `-fsyntax-only` on g++ 16.2.1 and clang++ 23.1.4, against the headers of `914c28d` (later header commits change comments only); rows in `wu13a/sizes.tsv`, with the first error line of each. Columns: the length of the first error line, the longest line, and the number of lines. Every source gives exactly one line holding `error:`. `transform_assert_fail.cpp` is a run-time program, not a compile-fail source, and is left out.

| Source | GCC first / longest / lines | Clang first / longest / lines |
| --- | --- | --- |
| `compose_compare_fail.cpp` | 1083 / 1131 / 28 | 422 / 633 / 8 |
| `compose_mixed_algebras_fail.cpp` | 159 / 1581 / 14 | 253 / 482 / 11 |
| `compose_mixed_parity_first_fail.cpp` | 190 / 691 / 9 | 239 / 310 / 8 |
| `compose_mixed_parity_second_fail.cpp` | 190 / 691 / 9 | 239 / 311 / 8 |
| `compose_mixed_scalars_fail.cpp` | 161 / 1694 / 14 | 212 / 511 / 11 |
| `compose_narrow_scalar_fail.cpp` | 208 / 515 / 9 | 279 / 279 / 8 |
| `dual_compare_antiproject_fail.cpp` | 802 / 809 / 29 | 362 / 498 / 8 |
| `dual_mixed_algebras_antiproject_fail.cpp` | 158 / 2259 / 14 | 252 / 479 / 11 |
| `dual_mixed_scalars_antiproject_fail.cpp` | 160 / 1230 / 14 | 211 / 427 / 11 |
| `dual_narrow_scalar_antiproject_fail.cpp` | 210 / 515 / 9 | 284 / 284 / 8 |
| `dual_narrow_scalar_antiproject_first_operand_fail.cpp` | 210 / 515 / 9 | 284 / 284 / 8 |
| `transform_consteval_nonzero_fail.cpp` | 154 / 1462 / 21 | 131 / 240 / 21 |
| `transform_consteval_nonzero_rational_fail.cpp` | 154 / 1588 / 21 | 140 / 258 / 21 |
| `transform_mixed_algebras_fail.cpp` | 161 / 1285 / 14 | 255 / 449 / 11 |
| `transform_mixed_parity_fail.cpp` | 201 / 574 / 9 | 244 / 269 / 8 |
| `transform_mixed_scalars_fail.cpp` | 163 / 1394 / 14 | 214 / 474 / 11 |
| `transform_narrow_scalar_first_plan_fail.cpp` | 208 / 493 / 9 | 277 / 277 / 8 |
| `transform_narrow_scalar_second_plan_fail.cpp` | 208 / 493 / 9 | 277 / 277 / 8 |
