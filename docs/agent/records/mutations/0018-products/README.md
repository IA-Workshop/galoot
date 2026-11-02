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

The long GCC lines hold the compiler's expansion of the operand types, whose layouts print as masks; the message is the first thing on the first line, as with the earlier messages of the core.

## Behavior to know

The rejecting overloads make a requires-expression that asks whether a mixed call is valid answer yes, as `get` does; the tests pin that (`can_multiply<R, F>`). A scalar operand (`x * 2.0`) has no overload, so the compiler gives its own error until scaling arrives.

## Cost

`core.hpp` now includes `product.hpp`. The smoke test, which includes only `core.hpp`, compiled in 0.261 s before and 0.247 s after on gcc-debug; its instruction count is not usable (the first row's counter reads 101,752, which is a failed measurement, not a figure). No larger includer was measured separately.

The independent review ran the sixth mutant (a shared-generator sign flip in the blade product) and two operand swaps on gcc-debug; the table checks and the associativity checks killed all of them at run time.
