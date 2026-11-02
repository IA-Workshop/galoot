# WU9 products: mutants and diagnostics

Revision: the head of `feature/core-products` when this note was written. Mutants were applied by hand to `include/galoot/core/product.hpp` one at a time, then reverted; each was built and run on gcc-debug and clang-debug (`core.product*` tests).

| Mutant | gcc-debug | clang-debug | Killed by |
| --- | --- | --- | --- |
| operands swapped inside `geometric_product` | killed | killed | the table check and the hand-checked vector products (run-time) |
| `wedge` calls the geometric-product plan | killed | killed | a `static_assert` of the test file (the result type differs from the plan's) and the table check |
| `operator^` calls `geometric_product` | killed | killed | the same-type `static_assert` of the operators test |
| `operator*` calls `wedge` | killed | killed | the same-type `static_assert` of the operators test |
| algebra constraint dropped from the catch-all | killed | not run | the four `product_mixed_algebras_*` compile-fail tests and `can_wedge<R, C>` |
| scalar constraint dropped from the catch-all | killed | not run | the four `product_mixed_scalars_*` compile-fail tests and `can_multiply<R, F>` |

The plan also listed "flip one table sign for a shared-generator pair". That sits in the blade-product rule, not in this unit's code; the unit's table check covers every blade pair of rigid(3) and conformal(3) against the reference and would fail on it, but this mutant was not run here. Where a mutant shows "killed" for a compile error of the test file, the kill is the test's own `static_assert`, never a warning.

## Diagnostics

Each mixed call gives one error, the message of the product as written. Sizes of the whole output, measured with `-fsyntax-only` on the compile-fail sources (longest line in characters, line count):

| Source | GCC 16.2.1 | Clang 23 |
| --- | --- | --- |
| mixed algebras, `a * b` | 1962, 12 | 495, 14 |
| mixed algebras, `geometric_product(a, b)` | 1974, 14 | 507, 14 |
| mixed scalars, `wedge(a, b)` | 1369, 12 | 493, 14 |

After WU10a, which reworded these messages and added the antiproducts and the narrow-scalar messages (`../0019-antiproducts/diagnostics.py`, `out/diagnostics.tsv` there): the length of the first error line, which holds the message, then the longest line and the line count of the whole output, `-fsyntax-only`, g++ 16.2.1 and clang++ 23.1.4. Every source gives one line that holds `error:`.

| Source | GCC first, longest, lines | Clang first, longest, lines |
| --- | --- | --- |
| mixed algebras, `a * b`, `a ^ b`, `a % b`, `a & b` | 155, 2412 to 2416, 14 | 249, 495 to 499, 11 |
| mixed algebras, `geometric_product(a, b)` | 166, 2424, 14 | 260, 507, 11 |
| mixed algebras, `wedge(a, b)` | 155, 2412, 14 | 249, 495, 11 |
| mixed algebras, `geometric_antiproduct(a, b)` | 171, 2428, 14 | 265, 511, 11 |
| mixed algebras, `antiwedge(a, b)` | 159, 2416, 14 | 253, 499, 11 |
| mixed scalars, `a * b`, `a ^ b`, `a % b`, `a & b` | 157, 1819 to 1823, 14 | 208, 493 to 497, 11 |
| mixed scalars, `geometric_product(a, b)` | 169, 1831, 14 | 220, 505, 11 |
| mixed scalars, `wedge(a, b)` | 157, 1819, 14 | 208, 493, 11 |
| mixed scalars, `geometric_antiproduct(a, b)` | 173, 1835, 14 | 224, 509, 11 |
| mixed scalars, `antiwedge(a, b)` | 161, 1823, 14 | 212, 497, 11 |
| narrow scalar, `a * b` | 200, 654, 9 | 267, 294, 8 |
| narrow scalar, `geometric_product(a, b)` | 212, 662, 9 | 279, 314, 8 |
| narrow scalar, `a % b` | 204, 658, 9 | 275, 296, 8 |
| narrow scalar, `geometric_antiproduct(a, b)` | 220, 670, 9 | 291, 322, 8 |
| `(p ^ q) == line`, a compiler error, not a library message | 966, 968, 29 | 404, 520, 8 |

The longest GCC line is now the "In instantiation of" line of the rejecting function, with both operand types written out in full; it is about 450 characters longer than the WU9 figure above. The WU9 figures were taken at a revision before that review's fixes, and the difference was not traced further. On Clang the first line begins "static assertion failed due to requirement" and names the two types compared, or `scalar_holds_every_coefficient_of_the_product` (or `_antiproduct`) for a narrow scalar.

The long GCC lines hold the compiler's expansion of the operand types, whose layouts print as masks; the message is the first thing on the first line, as with the earlier messages of the core.

## Behavior to know

The rejecting overloads make a requires-expression that asks whether a mixed call is valid answer yes, as `get` does; the tests pin that (`can_multiply<R, F>`). A scalar operand (`x * 2.0`) has no overload, so the compiler gives its own error until scaling arrives.

## Cost

`core.hpp` now includes `product.hpp`. The smoke test, which includes only `core.hpp`, compiled in 0.261 s before and 0.247 s after on gcc-debug; its instruction count is not usable (the first row's counter reads 101,752, which is a failed measurement, not a figure). No larger includer was measured separately.

The independent review ran the sixth mutant (a shared-generator sign flip in the blade product) and two operand swaps on gcc-debug; the table checks and the associativity checks killed all of them at run time.
