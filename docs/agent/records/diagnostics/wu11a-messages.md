# WU11a: the messages of dual.hpp and their sizes

Plan 0020 reading R11 and criterion 6; decision 0173. The texts are pinned by the `core.dual_*` compile-fail tests of `test/CMakeLists.txt` (block `core.dual`), each with `ONE_ERROR` and with `apply:`, `geometric_product:`, `left_complement`, `product_plan`, and `detail::` forbidden on the first error line. Each source writes its result into a variable (`const auto r = ...;`), which adds no second error.

| Function | Narrow scalar | Mixed algebras | Mixed scalar types |
| --- | --- | --- | --- |
| bulk | bulk: the scalar type cannot hold one of the bulk's exact coefficients; use a scalar type that holds larger integers exactly | | |
| weight | weight: ... the weight's ... | | |
| bulk_dual | bulk_dual: ... the bulk dual's ... | | |
| weight_dual | weight_dual: ... the weight dual's ... | | |
| left_bulk_dual | left_bulk_dual: ... the left bulk dual's ... | | |
| left_weight_dual | left_weight_dual: ... the left weight dual's ... | | |
| dot | dot: ... the dot's ... | dot: the operands are in different algebras; give both types the same algebra tag | dot: the operands have different scalar types; give both types the same scalar type |
| antidot | antidot: ... the antidot's ... | antidot: (as for dot) | antidot: (as for dot) |
| project | project: ... the projection's ... | project: (as for dot) | project: (as for dot) |

The `core.dual_compare_<function>` tests compare a result with a value of a named layout. On Clang the first error line names the operation's layout (`bulk_layout<rga3, blade_set<6>{...}>`), with no `product_plan` or `detail::`; GCC prints the value of the layout, as it does for the products.

## Sizes

`wu11a/diagnostics.py` compiles each `test/core/dual_*_fail.cpp` with the test flags and `-fsyntax-only`, at `69d6dcc`, with g++ 16.2.1 and clang++ 23.1.4; rows in `wu11a/sizes.tsv`, with the first error line of each. Columns: the length of the first error line, the longest line, the number of lines, and the number of lines that hold `error:` (one in every case).

| Source | GCC first / longest / lines | Clang first / longest / lines |
| --- | --- | --- |
| `dual_compare_antidot_fail.cpp` | 683 / 683 / 29 | 349 / 484 / 8 |
| `dual_compare_bulk_dual_fail.cpp` | 835 / 836 / 29 | 323 / 496 / 8 |
| `dual_compare_bulk_fail.cpp` | 828 / 834 / 29 | 313 / 491 / 8 |
| `dual_compare_dot_fail.cpp` | 678 / 678 / 29 | 341 / 480 / 8 |
| `dual_compare_left_bulk_dual_fail.cpp` | 840 / 840 / 29 | 333 / 501 / 8 |
| `dual_compare_left_weight_dual_fail.cpp` | 840 / 840 / 29 | 337 / 503 / 8 |
| `dual_compare_project_fail.cpp` | 941 / 968 / 29 | 374 / 549 / 8 |
| `dual_compare_weight_dual_fail.cpp` | 835 / 835 / 29 | 327 / 498 / 8 |
| `dual_compare_weight_fail.cpp` | 832 / 836 / 29 | 317 / 493 / 8 |
| `dual_mixed_algebras_antidot_fail.cpp` | 154 / 2411 / 14 | 248 / 497 / 11 |
| `dual_mixed_algebras_dot_fail.cpp` | 150 / 2407 / 14 | 244 / 493 / 11 |
| `dual_mixed_algebras_project_fail.cpp` | 154 / 2255 / 14 | 248 / 475 / 11 |
| `dual_mixed_scalars_antidot_fail.cpp` | 156 / 1455 / 14 | 207 / 462 / 11 |
| `dual_mixed_scalars_dot_fail.cpp` | 152 / 1451 / 14 | 203 / 458 / 11 |
| `dual_mixed_scalars_project_fail.cpp` | 156 / 1226 / 14 | 207 / 423 / 11 |
| `dual_narrow_scalar_antidot_fail.cpp` | 199 / 507 / 9 | 266 / 266 / 8 |
| `dual_narrow_scalar_bulk_dual_fail.cpp` | 203 / 351 / 9 | 272 / 272 / 8 |
| `dual_narrow_scalar_bulk_fail.cpp` | 193 / 341 / 9 | 257 / 257 / 8 |
| `dual_narrow_scalar_dot_fail.cpp` | 191 / 499 / 9 | 254 / 254 / 8 |
| `dual_narrow_scalar_left_bulk_dual_fail.cpp` | 213 / 361 / 9 | 287 / 287 / 8 |
| `dual_narrow_scalar_left_weight_dual_fail.cpp` | 217 / 365 / 9 | 293 / 293 / 8 |
| `dual_narrow_scalar_project_fail.cpp` | 202 / 507 / 9 | 272 / 272 / 8 |
| `dual_narrow_scalar_weight_dual_fail.cpp` | 207 / 355 / 9 | 278 / 278 / 8 |
| `dual_narrow_scalar_weight_fail.cpp` | 197 / 345 / 9 | 263 / 263 / 8 |

GCC's first lines for the comparisons (678 to 941 characters) hold the masks of both layouts, as for the products in WU10a; its longest lines for the mixed-operand rejections (up to 2,411) are the "required from" lines that print the operand types. Clang stays within 8 to 11 lines.
