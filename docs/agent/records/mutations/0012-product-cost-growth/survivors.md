# Mutants of the set-bit loops, and the reruns of the earlier sets

Plan 0012 changed three loops to visit only the set bits of a mask: `reorder_sign` in `bits.hpp` (`77ed567`), and the contraction of `add_generator_product` and the correction of `add_blade_product` in `blade_product.hpp` (`4e69ce8`). `mutate.py` holds the 16 mutants of their bounds, written from the loops and not from a list of earlier mutants.

## The loop mutants (`results-20dc2ea-*.tsv`)

| Mutant | Change |
| --- | --- |
| `rs_skip_lowest`, `ct_skip_lowest`, `cr_skip_lowest` | the lowest set bit is cleared before the loop starts |
| `rs_skip_highest`, `ct_skip_highest`, `cr_skip_highest` | the loop stops while one bit, the highest, is left |
| `rs_top_bit_ignored`, `ct_top_bit_ignored`, `cr_top_bit_ignored` | bit 31, e32 at N = `mask_bits`, is cleared before the loop |
| `ct_index_plus_one`, `cr_index_plus_one` | the index j is one above the bit visited |
| `ct_highest_first`, `cr_highest_first` | the bits are visited from the highest down, so the contraction sign alternates from the other end |
| `rs_below_inclusive` | the generators of B at or below e_i are counted, not only those below |
| `rs_below_one_short` | the generator of B just below e_i is not counted |
| `rs_highest_index` | the index is the highest set bit of what is left, not the lowest |

Every mutant is killed on gcc-debug and clang-debug in both modes. In normal mode each is killed in the build: by a `static_assert` of the tests (`bits_test.cpp:281-284`, `blade_product_test.cpp:532`, `:834`, `:856`, `plan_test.cpp`), except `rs_top_bit_ignored`, which builds and fails `core.bits` and `core.plan` at run time (the samples at n = 32 and the widest-algebra plan), and the two `index_plus_one` mutants, whose read past the end of the Gram matrix ends a constant evaluation. In run-time mode, with the `static_assert`s of the three doctest files off, the mutants are killed by doctest cases, or in the build by checks the mode leaves on: a `static_assert` of `blade_set_test.cpp` (name parsing calls `reorder_sign`), a plan of another size whose comparison in `plan_test.cpp` no longer compiles, and the read past the end of the Gram matrix.

`cr_top_bit_ignored` is killed only by the test case "blade_product reads a pairing with the top generator of the widest algebra" (`971bc9e`): with that case removed (`--without-top-pairing`), it survives in both modes on both compilers (`results-20dc2ea-*-without-top-pairing.tsv`). In R(32,0,0) every pairing off the diagonal is zero, so the correction never reads g(e_a, e32) with a nonzero value there. `ct_top_bit_ignored` is killed by the older widest-algebra case, e32 e32 = 1.

No survivor is left unresolved.

The first run-time runs were discarded: the driver wrote `static_assert(true || ...)`, and `true || a && b` ended the build with `-Wparentheses` under `-Werror` (`plan_test.cpp:471`), so every mutant was killed by the warning. The driver now writes `static_assert(true ? true : ...)`.

## Reruns of the WU2, WU6a, and WU6b sets

Each earlier driver was rerun unchanged at `52ed97f` (code of `77ed567`), on gcc-debug and clang-debug, in normal and run-time mode (`rerun-52ed97f-*.tsv`), and `compare_results.py 52ed97f` compares every mutant's status, killed or survived, with its latest recorded one:

| Set | Driver | Mutants | Recorded results | Result |
| --- | --- | --- | --- | --- |
| WU6a, `blade_product.hpp` | `0011-blade-product/mutate.py` | 42 | `results-b142837-*`, `results-398463d-*-range` (normal only), `results-65bd869-*-fixes*` | same in all four runs; 3 survivors, the equivalent ones of `0011-blade-product/survivors.md` |
| WU6b, `plan.hpp` | `0011-gp-plan/mutate.py` | 49 | `results-4e5ea90-*` | same in all four runs; 6 survivors in normal mode and 1 in run-time mode, as recorded |
| WU6b review | `reviews/2026-11-01-wu6b-gp-plan/mutate_review.py` | 27 | `results-4e5ea90-*-review` | same in all four runs; 5 and 7 survivors, as recorded |

Every anchor of those drivers still occurs once, because the changed loops keep the text the earlier mutants name (the contraction and correction signs, `rest & ~(mask_t{1} << j)`, the Gram reads).

The WU2 snippet `docs/agent/snippets/bits_fault_injection.py`, with its two `reorder_sign` mutations moved to the new bound in `77ed567`, catches all 13 mutations on gcc-debug in both modes and on clang-debug in normal mode (`rerun-52ed97f-bits-snippet-*.txt`). On clang-debug its `--runtime-only` mode refuses to run, before any mutation: with the `static_assert` lines removed, `all_agree` in `bits_test.cpp` is unused and Clang's `-Wunused-function` fails the build under `-Werror`. That is a defect of the snippet's run-time mode on Clang, independent of this change (`bits_test.cpp` is unchanged since `0f1039a`, and the snippet's index records it as checked on gcc-debug only); it is reported, not fixed here.
