# 0167 Sums, differences, scaling, and negation: exact scalar types, the sum kernel, and three disjoint operator families

- Status: accepted (plan 0019 section 1, readings R2 to R5 and question 4; CONVENTIONS 7.1 row for `-a` ratified by the owner on 2026-11-02, ruling O1; recorded by the WU10b implementer)
- Date: 2026-11-02
- Recorded by: Claude (agent, Opus 5.5), implementer of WU10b, cpp-expert role
- Bears on: decision 0165 (the result-layout rule and `linear.hpp`), decision 0166 (the mixed-operand wordings), plan 0016 readings 18 and 21, `records/diagnostics/wu10b-messages.md` (the texts, chosen by the WU10b test author)

## Context

CONVENTIONS 7.1 lists `a + b`, `a - b`, `s * a`, `a * s`, and `a / s`, and since version 1.8 `-a`, with no named function. Decision 0165 put them in a new header, `linear.hpp`, and fixed their result layouts: a sign map, negation, scaling, and a sum of one type keep the input's type; a sum or difference of two layouts gives the canonical set of the blades either holds. What remained was how a sum of two layouts is computed without new generation (C4), which scalars the scalings accept, how division rounds, how three families of `operator*` stay apart, and what negation is.

## Options weighed

Named functions (R2):
1. Public names (`add`, `scale`, ...). S13 asks for each operator to alias a named function, but 7.1 gives these rows none, and a name would need a 7.1 amendment.
2. None (chosen): the operators are the arithmetic of a number type, as `std::complex` has it. Each operator body is the computation itself or one call into `detail`.

The scalar of `s * a`, `a * s`, `a / s`:
1. Any type that converts to T. Admits `2.5 * x` for an integer multivector, which drops digits silently (C8).
2. Conversions that keep every value (int into double, but not int into float). Works, but a user must know the rule to predict a result.
3. Exactly T (chosen). `2 * x` for a double multivector is rejected with a message that says to write 2.0. `std::complex<double>` rejects `2 * c` by deduction in the same way, it matches the rule for two operands of different scalar types, and widening later is additive: every call that compiles now still compiles. Option 2 is the alternative on record.

The rejection of another scalar type: one catch-all per spelling, constrained by `multivector<X> && !multivector_or_derived<S> && ring_scalar<S> && S is not X's scalar type` (`detail::scalar_of_another_type`), returning X so that `auto r = 2 * x;` adds no second error. Its `static_assert` compares `S` with `X::scalar_type`, the scalar's type first, so both compilers print the two types in the order the user wrote them. A `volatile double` scalar deduces S as `volatile double`, which `ring_scalar` accepts, so it reaches this catch-all and is told it has another type. Stripping cv-qualification in the constraint was weighed and not done: the call would then have no candidate at all and get the compiler's longer error, while the catch-all's message names both types; the comment says to copy a volatile value into a T first.

Three families of `operator*`. The products take two `mv`s with one T; the scalings take a `const T&` and an `mv<Alg, A, T>`, with T deduced from both; the catch-alls of each take what the others exclude. Two multivectors deduce T as an `mv` type from the scalar's place, a conflict, and are not a scalar for the scalar catch-all (`!multivector_or_derived`). A scalar of the right type is not a multivector for the products' catch-all and is the scalar type for the scalar catch-all. So no call is ambiguous, whatever partial ordering would say, and the tests probe each pair, also for a class derived from `mv` (it deduces as its base for the valid overloads and is no `multivector` for any catch-all).

Division:
1. Multiply by the reciprocal 1/s. Rounds twice on a floating scalar (`3.0 / 10.0` is not `3.0 * (1.0 / 10.0)`), and for an integer scalar the reciprocal is 0.
2. T's own `/` on each coefficient (chosen): exact for the test rational, correctly rounded per coefficient for double, and truncating toward zero for a built-in integer type, which the comment states. `operator/` requires that `a / b` converts to T, so a scalar without division has no `x / s` and gets the compiler's own error; no present scalar lacks it.
3. For an integer T, a zero divisor is undefined behavior, so it is a precondition checked by `GALOOT_ASSERT(s != T{}, "a / s: an integer scalar cannot divide by zero")` in `linear.hpp` (S9). "Integer" is read through `detail::integer_limits<T>`, which is false rather than an error for a specialization without `is_integer` (the test rational's has only `is_specialized` and `is_exact`), and `std::equality_comparable<T>` guards the comparison. A floating T is not checked: IEEE 754 defines its division by zero, and a test divides by 0.0 in checking mode.

The sum of two layouts (R4, C4):
1. A new generated plan for the sum. New machinery, which C4 admits only by a decision showing composition cannot do it.
2. Per-basis data composed (chosen). x's terms are `identity_kernel<A, Target>`, the copy that conversions already use. y's terms are the same identity terms of B onto the target, times +1 for a sum and −1 for a difference, each marked first where A lacks the blade. One straight-line function, `run_sum`, applies x's terms and then y's with the existing `accumulate`. A blade that one operand alone holds is assigned (or assigned negated), never added to zero, so a −0.0 survives, as `accumulate` already does for products (plan 0016 reading 18); a blade both hold is x's coefficient plus or minus y's, each times its orientation. Two values of one type go through the same function onto their own layout, where every coefficient sign is +1.

The result layout of two layouts: `sum_layout<Alg, A, B>`, a public variable template in `galoot::core` beside the products' `*_layout` names (decision 0166 option 3, for the same reason: a `detail` name prints `detail::` on Clang's first line). It is `merge` of the two layouts with every orientation set to +1, so its blades are in canonical order and no mask can conflict by sign. Its cost is merge's, quadratic in the sizes, and the comment says so. Same-type sums and two-layout sums are two overloads, the second constrained by `!detail::same_layout_v<A, B>`, so that the reader sees the two cases are disjoint rather than relying on partial ordering; `sum_layout<Alg, A, A>` is the canonical reordering of A and is not what `x + x` returns.

Negation:
1. `0 - x`, through the sum kernel. Turns +0.0 into +0.0 under IEEE 754.
2. `T`'s unary minus on each coefficient (chosen), in a loop over the input's layout, so −(+0.0) is −0.0. It needs no rejecting overload: every ring scalar has a unary minus.

Scaling and negation are loops over the coefficients, not kernels: every blade maps to itself, so there are no positions or signs to precompute.

## Decision

1. `linear.hpp` holds `a + b`, `a - b` (two overloads each: one type, and two layouts on `sum_layout`), their catch-alls with the products' two wordings, `s * a`, `a * s`, `a / s` with T exactly the multivector's scalar type, their catch-alls, and `-a`. It includes `mv.hpp`, `blade_set.hpp`, `metric.hpp`, and `assert.hpp`, and `core.hpp` includes it.
2. The sum kernel composes `identity_kernel` and a second identity kernel with the sign of the operation, assigning each blade's first term.
3. `sum_layout<Alg, A, B>` is a public name, merged with orientation +1.
4. Division uses T's `/`; an integer zero divisor is checked; a floating one is not.

## Consequences

- Seven new public operator overloads with five catch-alls, and one new public name, `sum_layout`, which users may write in a type (W13 if renamed).
- A requires-expression that asks whether `2 * x` is valid for a double multivector answers yes, as for the mixed products; asking whether the two types are the same is the reliable test. The comments say so.
- `a / s` for an integer scalar truncates; users who want exact quotients choose an exact rational scalar (C7).
- The sum of two large layouts is bounded by merge's limits on constant evaluation, as the comment of `sum_layout` states.

## Correction after the blind review (2026-11-02)

Recorded by the WU10b implementer after the W5 review of `feature/core-linear`; the text above is kept as first recorded.

1. **The constraint's stated reason was wrong.** The paragraph on `sum_layout` says the two-layout overloads were constrained by `!detail::same_layout_v<A, B>` "so that the reader sees the two cases are disjoint rather than relying on partial ordering". The review showed the constraint did more: without it, partial ordering still chose the same-type overload, but substituting the two-layout overload's return type evaluated `merge` of the layout with itself, which a same-type sum of `full<8>` on Clang 23 and of `full<10>` on GCC 16 cannot build. The header's promise that a sum of one type merges nothing rested on that constraint, and no test pinned it.
2. **What replaces it.** `sum_layout<Alg, A, A>` is now A, by a partial specialization, so `sum_layout` is the result layout of `x + y` for every pair of layouts, as every other `*_layout` name is for its operation; generic code that declares the result type through it agrees with the call. With that, `+` and `-` each have one overload returning `mv<Alg, sum_layout<Alg, A, B>, T>`, and the same-type overloads and the constraint are gone. A same-type sum evaluates no merge because the specialization chooses A. A test sums two `full<8>` multivectors of R(8, 0, 0), the largest full layout Clang 23 builds, on both compilers, and asserts `sum_layout<Alg, A, A> == A`.
3. **What the vector-space operators do not include.** There is no compound assignment (`+=`, `-=`, `*=`, `/=`) and no unary `+`. CONVENTIONS 7.1 lists neither, so they are not built (C12); `x = x + y` is the spelling. A user who writes `x += y` gets the compiler's own error, which on GCC prints the layout as masks. Adding them later is additive and needs a 7.1 row.
4. **The scalar-type message** now names the general fix ("convert it to that type first, as 2.0 for 2 over double or std::int64_t{2} over std::int64_t", pointing to the second type of the printed condition), since the first text fitted only double (review F5).
