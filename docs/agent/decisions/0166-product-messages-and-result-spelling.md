# 0166 The product messages name the product as written; result types are spelled through public layout names

- Status: accepted (plan 0019 section 1, question 5 (a) to (c); recorded by the WU10a implementer)
- Date: 2026-11-02
- Recorded by: Claude (agent, Opus 5.5), implementer of WU10a, cpp-expert role
- Bears on: the WU9 review (`records/reviews/2026-11-02-wu9-products.md`, F5, F12, F13), decision 0162 items 2 and 8, decision 0157 item 3, plan 0016 reading 21, `records/diagnostics/wu10a-messages.md` (the texts, chosen by the WU10a test author)

## Context

The WU9 review left three findings on the products' diagnostics to WU10:

- F5: a scalar too narrow for one of the geometric product's exact coefficients, written `a * b`, was reported by the kernel as "apply: the scalar type cannot hold ...", an internal name the user never wrote.
- F13: the mixed-operand fixes, "both must be in the same one" and "convert one explicitly first", restated the rule or pointed to a conversion that does not exist.
- F12: Clang printed a product's result type through the kernel's declared return type: `mv<rga3, product_plan<(galoot::core::detail::BinaryOperation)1, rga3, blade_set<4>{...}, blade_set<4>{...}>.result, double>`. GCC prints the value whatever the declaration says.

## Options weighed

F5:
1. Pass the spelling into `detail::apply` and let it choose the message. One check, but `apply` then knows about the public spellings, and its message table grows with every public product.
2. Each public spelling that can fail checks the condition itself, before it calls the kernel, and gives its own message (chosen). The condition is one named `constexpr bool`, `scalar_holds_every_coefficient_of_the_product` (or `_antiproduct`), so Clang prints the rule and not a mask, and the call to the kernel sits in the `if constexpr` branch the check guards, so a failed check never instantiates `apply` and the user sees one error. `a * b` checks for itself rather than calling `geometric_product` unguarded, so its message names `a * b`.

F13: the plan's two wordings (chosen), with the texts of the test author's catalogue copied verbatim: "<written>: the operands are in different algebras; give both types the same algebra tag" and "<written>: the operands have different scalar types; give both types the same scalar type", for all eight spellings. The likeliest cause of two algebras is two tags declared for one metric, and this names its fix. The scalar text is reworded again when a scalar conversion exists (decision 0162 item 2).

F12:
1. Leave the sugar. The type is correct, and sugar can change later without breaking anyone; but every error that involves a product shows `product_plan`, the enum cast, and `detail::`.
2. Variable templates in `detail` that hold each result set by value, used in the declared return types. Measured with Clang 23.1.4 on `(p ^ q) == line`: Clang prints `mv<rga3, detail::wedge_layout<rga3, ...>, double>`, so `detail::` stays on the first line, which plan 0019 criterion 4 forbids.
3. The same variable templates in `galoot::core` (chosen): `geometric_product_layout<Alg, A, B>`, `wedge_layout`, `geometric_antiproduct_layout`, `antiwedge_layout`, and in `unary.hpp` `right_complement_layout<Alg, S>` and `left_complement_layout<Alg, S>`. Clang then prints `mv<rga3, wedge_layout<rga3, blade_set<4>{...}, blade_set<4>{...}>, double>`. They hold the same value as the plan's result, so every result type is the same type as before; the tests pin `decltype(p ^ q)` and the other result types against layouts written from the definition. A program can also name a product's result type through them.
4. Alias templates (`template <...> using wedge_result = mv<...>`). Probed on Clang 23.1.4 with the same comparison: Clang prints the alias and then its expansion, `(aka 'core::mv<rga3, core::detail::product_plan<core::detail::BinaryOperation::wedge, ...>.result, ...')`, on the same first line, so the internal names stay.

## Decision

1. The four spellings of the two geometric products (`geometric_product`, `*`, `geometric_antiproduct`, `%`) check that the scalar holds every exact coefficient of their plan, each with its own message, and call the kernel only when it does. The wedge and the antiwedge have coefficients +1, −1, and 0, which every ring scalar holds, so they have no such check; their comments say so.
2. The mixed-operand messages are the two wordings above, from one rejecting function whose conditions compare the two algebras or the two scalar types themselves, so the compilers print both. Each catch-all returns the first operand's type, so `auto r = a % b;` adds no second error.
3. The products and complements declare their result types through the public layout variable templates of option 3. The masks of the operand layouts remain in the text, since deduction drops the name of a layout variable; names written in the type stay deferred (decision 0157 item 3).

## Consequences

- Six new public names. They are what users will write for a result type (`mv<rga3, wedge_layout<rga3, point_layout, point_layout>, double>`), so renaming them after release breaks code (W13).
- The kernel's own messages remain for a direct call of `detail::apply`, which only the library makes; the compile-fail tests forbid `apply:` in the output of each public spelling.
- The layout variable templates are tested through the result types they declare, not by `static_assert`s of their own; the review may ask for those.
