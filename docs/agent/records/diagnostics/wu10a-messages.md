# WU10a: the texts of the product messages

Plan 0019, section 1 question 5 (a) and (b), and section 2. Chosen by the WU10a test author (Claude, agent, Opus 5.5, doctest-expert role) before the implementation, so that the compile-fail tests and the header use the same texts. The implementer copies each text verbatim into `include/galoot/core/product.hpp`. A text changes only with its pinned expectation in `test/CMakeLists.txt`, in a commit listed for review (W9).

Style (plan 0016, reading 21): the operation as the user wrote it, then a colon; the mistake in the algebra's terms; the fix after a semicolon; no masks, positions, or internal names; no final period or quotes. Where the plan gives the text (5a's example for `a * b`, 5b's two wordings), it is used as given. The other texts follow the same pattern.

## How the tests read them

Each text is pinned by a `galoot_add_compile_fail_test` in `test/CMakeLists.txt`, block `core.product_*`, with `ONE_ERROR`: the compile must fail, the text must appear, and every line of the output that holds `error:` must hold the text. So the text must sit on the first line of the `static_assert` diagnostic on both compilers (a `static_assert` whose message is the string literal does this), and the source writes the result into a variable (`const auto r = a * b;`), which must add no second error. `;` in the texts below is the semicolon; CMake holds it as `${semicolon}`.

The spelling as written is the prefix: `geometric_product`, `wedge`, `geometric_antiproduct`, `antiwedge` for the functions, and `a * b`, `a ^ b`, `a % b`, `a & b` for the operators.

## Mixed operands (F13; the same two wordings for all eight spellings)

| Cause | Text, with `<written>` the spelling | FORBID |
| --- | --- | --- |
| Two algebras | `<written>: the operands are in different algebras; give both types the same algebra tag` | `different scalar types` |
| Two scalar types, one algebra | `<written>: the operands have different scalar types; give both types the same scalar type` | `different algebras` |

When both differ, the algebra message is the one given (as in WU9). Each `static_assert` keeps WU9's condition on the two types (`std::is_same_v` of the two `algebra_type`s, or of the two `scalar_type`s), so that the compilers print both.

Pinned for the four WU9 spellings by `core.product_mixed_{algebras,scalars}_{geometric_product,wedge,times,caret}` (sources unchanged; only the expected text changes), and for the four new spellings by `core.product_mixed_{algebras,scalars}_{geometric_antiproduct,antiwedge,percent,ampersand}`.

## A scalar that cannot hold a coefficient of the product (F5)

| Spelling | Text | Test |
| --- | --- | --- |
| `geometric_product(a, b)` | `geometric_product: the scalar type cannot hold one of the product's exact coefficients; use a scalar type that holds larger integers exactly` | `core.product_narrow_scalar_geometric_product` |
| `a * b` | `a * b: the scalar type cannot hold one of the product's exact coefficients; use a scalar type that holds larger integers exactly` | `core.product_narrow_scalar_times` |
| `geometric_antiproduct(a, b)` | `geometric_antiproduct: the scalar type cannot hold one of the antiproduct's exact coefficients; use a scalar type that holds larger integers exactly` | `core.product_narrow_scalar_geometric_antiproduct` |
| `a % b` | `a % b: the scalar type cannot hold one of the antiproduct's exact coefficients; use a scalar type that holds larger integers exactly` | `core.product_narrow_scalar_percent` |

Each of these tests also forbids `apply:`, so the message of `detail::apply` must not appear: the public product checks the condition itself and does not instantiate `apply` when it fails (plan section 1, 5a). The input is `mv<diag(100, 100), full<2>, std::int8_t>`: e12 e12 = −10000, and for the antiproduct the right complement of 1 is e12, so both plans hold a coefficient of magnitude 10000.

"antiproduct's" for the antiproduct is the test author's choice; the plan gives only the product's example. The wedge and the antiwedge have coefficients ±1 only and get no such message.

## Not pinned here

- The unary functions and operators (`reverse`, `antireverse`, `grade_involution`, `right_complement`, `left_complement`, `~`, `!`) have no rejecting overloads in the plan, so they have no message. A call on a non-multivector is the compiler's own error.
- Criterion 4's statement about Clang's first line for `(p ^ q) == line` (no `product_plan`, `detail::`, or enum cast) is a compiler text, not a library message; it belongs in the diagnostics table of the records commit, not in a compile-fail test.
