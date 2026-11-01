# 0152 Catch-alls that print numbers, and class templates checked in their body

- Status: accepted by the owner, 2026-11-01 ("proceed as recommended"); the STYLE S7 amendment below is ratified by the same ruling
- Date: 2026-11-01
- Decided by: the owner, on research note 0019 question 4; recorded by Claude (agent, Opus 5.5)
- Adds to: decision 0033 (form P), which stands

## Context

Decision 0033's form P rejects a misuse with a constrained catch-all whose body is `static_assert(false, "literal")`. Research note 0019 found two refinements. Where the help is a number, a `static_assert` that tests the numeric condition (`count == expected`) prints the values on both compilers in one error ("the comparison reduces to '(3 == 4)'", "expression evaluates to '3 == 4'"); where the condition is a concept, testing it doubles GCC's output, so `false` stays better. And a class template whose parameter fails a constraint cascades on GCC (four errors, 43 lines, for a `std::string` scalar), while a `static_assert` in the body under a weaker constraint gives one error.

## Decision

1. A catch-all whose help is a number asserts the numeric condition itself, with its literal message; any other catch-all keeps form P's always-false condition.
2. A class template may constrain a parameter by a weaker named concept and check the rest of its requirement with a `static_assert` in its body, with a literal message, so that a misuse gives one error.
3. STYLE S7 is amended. After "Every template parameter is constrained by a named concept; `std::enable_if` is not used." it reads "A class template may constrain a parameter by a weaker named concept and check the rest of its requirement with a `static_assert` in its body, so that a misuse gives one error with a literal message.", and after the sentence that ends "(C9)." it reads "A constrained catch-all overload that rejects a misuse asserts a condition that is always false, with a literal message (decision 0033); where the help is a number, such as a count of values, its `static_assert` tests the numeric condition itself, so that the compiler prints the values."

## Consequences

- `mv` constrains its scalar by `std::semiregular` and asserts `ring_scalar` in its body ("mv: the scalar type must support +, -, *, unary - and construction from an integer coefficient"). Generic code that asks whether `mv<A, B, std::string>` names a type is told yes; it fails when the type is used. `ring_scalar` itself stays a named concept generic code can test.
- The constructor of decision 0150 that rejects a wrong count is a numeric catch-all.
