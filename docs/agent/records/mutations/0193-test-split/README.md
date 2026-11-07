# The test split and the B1 fix: the antiproduct mutants before and after

Decision 0193. Toolchain: g++ 16.2.1 and clang++ 23.1.4, on ds-001.

## What ran

`../0019-antiproducts/mutate.py <preset>`, unchanged, on gcc-debug and clang-debug, at three revisions of `feature/test-split`:

| Revision | Tree | Files in `out/` |
| --- | --- | --- |
| `b504aa7` | before: one `identities_test.cpp`, one `product_test.cpp` | `b504aa7-gcc-debug.tsv`, `b504aa7-clang-debug.tsv` |
| `c28fa89` | the split and the B1 fix, with `develop` merged, shared checks with external linkage | `c28fa89-gcc-debug.tsv`, `c28fa89-clang-debug.tsv` |
| `4efe4d0` | the same with the shared checks given internal linkage, the branch's final code | `4efe4d0-gcc-debug.tsv`, `4efe4d0-clang-debug.tsv` |

It is the only mutation driver in the records whose programs include the identities or product tests: it applies each of its 17 mutants to `product.hpp`, `unary.hpp`, or `bits.hpp`, builds `core_product_test`, `core_unary_test`, and `core_identities_test` with `ninja -k 0`, and runs the tests labeled `core.product` or `core.unary`. Its target names did not change, since the new sources belong to the same two programs. The driver writes `../0019-antiproducts/out/<preset>.tsv`; each run's table was copied here and the committed tables there were restored. The runs are in `runs.tsv` under the selections `mutant:<name>` and `mutant:restore`.

## Result

On both presets and at all three revisions, the columns `mutant`, `build_exit`, and `failed_tests` are identical: all 17 mutants are killed, by the same tests. Before and after, by mutant:

| Mutant | Killed by, on both presets, before and after |
| --- | --- |
| `reverse_calls_antireverse`, `grade_involution_calls_reverse` | `core.unary`, `core.identities` |
| `bang_calls_left_complement` | `core.unary` |
| `reverse_returns_canonical_layout` | a `static_assert` of `unary_test.cpp`; `core.unary` |
| `percent_calls_geometric_product` | a `static_assert` of `product_test.cpp`; `core.identities`, `core.product_narrow_scalar_percent`, `core.product_narrow_scalar_on_scalars_percent` |
| `antiwedge_uses_antiproduct_plan`, `ampersand_calls_wedge` | a `static_assert` of `product_test.cpp`; `core.identities` |
| `antiproduct_operands_swapped` | `core.identities`, `core.product` |
| the four `check_dropped_*` and the two `*_checks_product_coefficients` | their `core.product_narrow_scalar_*` tests |
| `new_catch_alls_without_algebra`, `new_catch_alls_without_scalar` | a `static_assert` of `product_test.cpp`; the four `core.product_mixed_*` tests of the antiproduct spellings |
| `left_complement_sign_flipped_for_e12` | `core.product` at run time |

The column `build_failures` differs in two ways only. The `product_test.cpp` line numbers moved (626, 544, 741, and 740 before; 419, 337, 511, and 510 after), because two test cases and the shared helpers left that file. And for `left_complement_sign_flipped_for_e12`, the library's own check of the complement signs (`plan.hpp:937`) now stops each of the seven identities group sources instead of the one identities source. As in the WU10a record, a program that does not build leaves CTest running its previous build, so that mutant's kill is `core.product`'s at run time, before and after.

No mutant was added: the change is to the tests' files, not to what they check.
