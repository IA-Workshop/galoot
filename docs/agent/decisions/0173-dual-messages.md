# 0173 The messages of `dual.hpp`: each public function checks its own composed coefficients, and the binary functions reject mixed operands through one rejecting function

- Status: accepted (plan 0020 reading R11; recorded by the WU11a implementer)
- Date: 2026-11-04
- Recorded by: Claude (agent, Opus 5.5), implementer of WU11a, cpp-expert role. The message nouns and the compile-fail tests are the WU11a test author's (Claude, agent, Opus 5.5, doctest-expert role).
- Bears on: decision 0166 (the product messages and their form), plan 0016 reading 21, review F5 of WU9 and F2 of WU10a, `records/diagnostics/wu11a-messages.md`

## Context

Each function of `dual.hpp` calls public functions that have their own messages: a narrow scalar given to `bulk` would otherwise be reported as "geometric_product: ...", a name the user did not write (WU9 review F5). The dot, the antidot, and the projection take two operands, which may come from two algebras or have two scalar types. Decision 0166 set the form: one error, the operation as written, the mistake in the algebra's terms, the fix after a semicolon.

## Options weighed

The narrow-scalar check:
1. Let the inner call report it. One check fewer per function, but the first error names `geometric_product`, which the user never wrote.
2. Each public function checks, before it calls anything, that the scalar holds every exact coefficient of its own composed plan, and calls the composition only inside the `if constexpr` its check guards (chosen). The condition is a named `constexpr bool`, `scalar_holds_every_coefficient_of_the_bulk` and likewise, so Clang prints the rule and not a mask or a `detail::` name.

What each checks:
- the bulk, the bulk dual, and the left bulk dual: the coefficients of the product of x's layout with 𝟙, the only plan of their composition whose coefficients can exceed 1;
- the weight, the weight dual, and the left weight dual: those of the product of right(x)'s layout with 𝟙;
- the dot: only the terms its kernel keeps, so a scalar that holds the dot's coefficients but not the whole product's is accepted (pinned: `std::int8_t` in diag(100, 100, 0, 0), where the product holds −10000 and the dot 100);
- the antidot: only the terms the dot of the complements keeps;
- the projection: the weight dual's of b, since the wedge and the antiwedge have coefficients +1, −1, and 0.

Mixed operands:
1. Reuse `product.hpp`'s `reject_product`, adding three spellings to its enum. `product.hpp` would change and its spelling list would name operations it does not hold.
2. A rejecting function of `dual.hpp`'s own, `detail::reject_dual_operands`, for its three spellings (chosen), with the products' two wordings and the conditions on the two types, so the compilers print both. Each catch-all is constrained by `detail::mismatched_operands<X, Y>`, which holds only for two multivectors, and returns the first operand's type, so `auto r = dot(a, b);` adds no second error and a user's own `dot` for other types is left alone.

## Decision

The 15 messages, pinned by `expect_output` compile-fail tests on GCC and Clang (`core.dual_narrow_scalar_<function>` and `core.dual_mixed_{algebras,scalars}_{dot,antidot,project}`), each with one error, the text on the first error line, and `apply:`, `geometric_product:`, `left_complement`, `product_plan`, and `detail::` forbidden on that line:

- "<function>: the scalar type cannot hold one of the <noun>'s exact coefficients; use a scalar type that holds larger integers exactly", for bulk (the bulk's), weight (the weight's), bulk_dual (the bulk dual's), weight_dual (the weight dual's), left_bulk_dual (the left bulk dual's), left_weight_dual (the left weight dual's), dot (the dot's), antidot (the antidot's), and project (the projection's);
- "<function>: the operands are in different algebras; give both types the same algebra tag" and "<function>: the operands have different scalar types; give both types the same scalar type", for dot, antidot, and project.

Each narrow-scalar test uses a layout on which the checked plan differs from the plans a mutant could check instead (diag(100, 100) over `std::int8_t`: the bulk of 1 is 1 and its weight 10,000, and on {e12} the reverse).

## Consequences

- A requires-expression that asks whether a mixed dot, antidot, or projection is valid is told yes, as for the products; the comments say so.
- To check only the first error line, the test author added `FORBID_FIRST` to `test/expect_output.cmake` and `FORBID_ON_FIRST_ERROR` to `galoot_add_compile_fail_test`, with three tests of their own (`tools.expect_output_forbid_first_*`). Existing tests do not use them.
- The message sizes on both compilers are in `records/diagnostics/wu11a-messages.md`.
