# WU10b: the texts of the arithmetic, scalar, and part messages

Plan 0019, section 1 questions 1 and 4, question 5 (b) and (d), and section 2 (WU10b). Chosen by the WU10b test author (Claude, agent, Opus 5.5, doctest-expert role) before the implementation, so that the compile-fail tests and the headers use the same texts. The implementer copies each text verbatim into `include/galoot/core/mv.hpp` or `include/galoot/core/linear.hpp`. A text changes only with its pinned expectation in `test/CMakeLists.txt`, in a commit listed for review (W9).

Style (plan 0016, reading 21): the operation as the user wrote it, then a colon; the mistake in the algebra's terms; the fix after a semicolon; no masks, positions, or internal names; no final period or quotes. Where the plan gives the text (5(b)'s two wordings, 5(d)'s five texts, question 1's `part` text), it is used as given. The other texts follow the same pattern.

## How the tests read them

Each text is pinned by `galoot_add_compile_fail_test` (directly or through `mv_rejects`) in `test/CMakeLists.txt`, with `ONE_ERROR`: the compile must fail, the text must appear, and every line of the output that holds `error:` must hold the text or a `REQUIRE` literal. So the text must sit on the first line of the `static_assert` diagnostic on both compilers, which a `static_assert` whose message is the string literal does, and the sources write the result into a variable (`const auto r = a + b;`), which must add no second error: each rejecting overload declares a return type that is a value type (the multivector operand's type, or the target), not `void` and not `auto`. `;` in the texts below is the semicolon; CMake holds it as `${semicolon}`.

## The scalar's ring operations (5(d); `mv.hpp`)

`ScalarFault::no_ring_operations` becomes five faults, checked after `not_copyable` in this order, each with its own named rule and `static_assert`. A scalar with several faults gets the first one's message alone.

| Order | Requirement of `ring_scalar` | Text |
| --- | --- | --- |
| 1 | `a + b` convertible to T | `mv: the scalar type has no addition; give it a + b, or use a number type such as double` |
| 2 | `a - b` convertible to T | `mv: the scalar type has no subtraction; give it a - b, or use a number type such as double` |
| 3 | `a * b` convertible to T | `mv: the scalar type has no multiplication; give it a * b, or use a number type such as double` |
| 4 | `-a` convertible to T | `mv: the scalar type has no unary minus; give it -a, which the products use for a term of coefficient -1` |
| 5 | `std::constructible_from<T, coefficient_type>` | `mv: the scalar type cannot be made from an integer coefficient; give it a constructor from std::int64_t, or use a number type such as double` |

Tests (block `core.mv_<case>`, function `mv_ring_rejects`): each requires its text and forbids the other four and the semiregular message.

| Test | Scalar | Text |
| --- | --- | --- |
| `core.mv_scalar_lacks_addition` | `test_support::lacks_addition` | 1 |
| `core.mv_scalar_lacks_subtraction` | `test_support::lacks_subtraction` | 2 |
| `core.mv_scalar_lacks_multiplication` | `test_support::lacks_multiplication` | 3 |
| `core.mv_scalar_lacks_unary_minus` | `test_support::lacks_unary_minus` | 4 |
| `core.mv_scalar_lacks_integer_construction` | `test_support::lacks_integer_construction` | 5 |
| `core.mv_scalar_without_arithmetic` | a colour code with no arithmetic | 1 |
| `core.mv_not_ring_scalar` (existing source) | `std::string`: + but no - or * | 2 |
| `core.mv_scalar_without_multiplication` (existing source) | a length: + and -, no * | 3 |
| `core.mv_scalar_without_multiplication_or_negation` | +, -, a constructor; no * and no -a | 3 |
| `core.mv_scalar_without_negation_or_construction` | +, -, *; no -a and no constructor | 4 |

The five fault scalars are in `test/support/scalar_faults.hpp`; `mv_test.cpp` asserts that each lacks exactly its one requirement and is not a `ring_scalar`. The tests that used to forbid the old single text (`core.mv_scalar_reference`, `_const`, `_bool`, `_move_only`, `_const_member`, `_without_default`, `core.mv_not_metric_like`) now forbid all five.

## Sums and differences of mismatched operands (5(b); `linear.hpp`)

The products' two wordings, with the spelling `a + b` or `a - b`. When both the algebra and the scalar differ, the algebra message is the one given, as for the products. Each `static_assert` keeps the products' condition on the two types (`std::is_same_v` of the two `algebra_type`s, or of the two `scalar_type`s), so that the compilers print both.

| Test | Text | FORBID |
| --- | --- | --- |
| `core.linear_mixed_algebras_plus` | `a + b: the operands are in different algebras; give both types the same algebra tag` | `different scalar types` |
| `core.linear_mixed_scalars_plus` | `a + b: the operands have different scalar types; give both types the same scalar type` | `different algebras` |
| `core.linear_mixed_algebras_minus` | `a - b: the operands are in different algebras; give both types the same algebra tag` | `different scalar types` |
| `core.linear_mixed_scalars_minus` | `a - b: the operands have different scalar types; give both types the same scalar type` | `different algebras` |

## A scalar of another type (question 4; `linear.hpp`)

One text per spelling, from the three catch-alls constrained by `multivector<X> && !multivector_or_derived<S> && ring_scalar<S> && S is not X's scalar type`. The condition of each `static_assert` is `std::is_same_v<S, typename X::scalar_type>`, the scalar's type first, so that both compilers print the two types in that order; the tests require it for an `int` given to a `double` multivector.

| Spelling | Text |
| --- | --- |
| `s * a` | `s * a: the scalar is not of the multivector's scalar type; write it in that type, as 2.0 for 2 when the scalar type is double` |
| `a * s` | `a * s: the scalar is not of the multivector's scalar type; write it in that type, as 2.0 for 2 when the scalar type is double` |
| `a / s` | `a / s: the scalar is not of the multivector's scalar type; write it in that type, as 2.0 for 2 when the scalar type is double` |

Required beside the text in `core.linear_scale_left_int`, `core.linear_scale_right_int`, and `core.linear_divide_int`: on GCC 16 the note `‘int’ is not the same as ‘double’`, on Clang 23 `due to requirement 'std::is_same_v<int, double>'` (measured on a stand-in). `core.linear_scale_left_fraction` (2.5 times a `std::int64_t` line) requires only the `s * a` text. With `core.hpp` included, `core.linear_scale_with_products` (`a * 2`) must give the `a * s` text and not `a * b:`, and `core.linear_product_with_scaling` (a double line times a float motor) must give the products' `a * b: the operands have different scalar types; give both types the same scalar type` and neither `s * a:` nor `a * s:`.

## An integer divisor of zero (question 4; `linear.hpp`)

`a / s` checks, for an integer scalar type, `GALOOT_ASSERT(s != T{}, "a / s: an integer scalar cannot divide by zero")`. Text: `a / s: an integer scalar cannot divide by zero`.

- The text is short on purpose. A failed constant evaluation shows only the source line that holds `GALOOT_ASSERT(`, so the whole literal must be on that line (assert.hpp's own requirement), within 80 columns: at an indentation of four, `GALOOT_ASSERT(s != T{}, "...");` fits this text and no longer one. A fix after a semicolon did not fit.
- "Integer" is what `std::numeric_limits<T>` says: `is_specialized` and `is_integer`. The test rational's specialization defines only `is_specialized` and `is_exact`, so the test must read the limits in a way that compiles for it, such as `detail::integer_limits<T>`; `std::numeric_limits<T>::is_integer` written directly does not compile for the rational (seen on the stand-in). A floating scalar is not checked: `core.linear_divide_accepts_floating_zero` divides by 0.0 in checking mode and must get infinities and a NaN.
- Pinned at compile time by `core.linear_divide_consteval_zero` (requires `assertion_failed`) and at run time by `core.linear_divide_rejects_zero`, which requires `galoot: assertion failed: a / s: an integer scalar cannot divide by zero (` and `linear.hpp:`, so the check must be in `linear.hpp` itself.

## part's target (question 1; `mv.hpp`)

The plan's text, used as given, with truncate's form of catch-all:

`part: the target must be an mv type of the same algebra and scalar type, not a class derived from one; part changes only which blades are held`

Pinned by `core.mv_part_{other_algebra,not_a_type,other_scalar,derived,result_used}`, mirrors of truncate's five, each forbidding `truncate:`.

## Not pinned here

- Negation `-a` has no rejecting overload, so no message.
- A scalar type without `/` gets the compiler's own error for `a / s`, as the plan says; no test asks whether `a / s` is valid for such a type.
- Observation from the stand-in: a `volatile double` scalar (`const volatile double h; h * line`) deduces S as `volatile double`, which `ring_scalar` accepts, so it reaches the `s * a` catch-all, whose condition prints `std::is_same_v<volatile double, double>`. The tests copy volatile values into plain variables before scaling. Whether the catch-all should strip cv-qualification is the implementer's call; no test pins either way.
