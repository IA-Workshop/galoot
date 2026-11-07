# 0179 WU13a: the messages of transform, compose, and antiproject

- Status: recorded
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), implementer of WU13a; the texts were written by the WU13a test author (Claude, agent, Opus 5.5, doctest-expert role) and copied into the headers
- Bears on: plan 0021 R11 and criterion 9; decisions 0166, 0173, 0178; C9

## Context

Plan 0021 R11 proposed eleven compile-time messages and two run-time ones, in the form of decisions 0166 and 0173: one error, the operation as written, the mistake in the algebra's terms, the fix after a semicolon, no final period. The test author pinned the final texts with compile-fail tests (`test/CMakeLists.txt`, blocks `core.transform` and `core.dual`), each with `ONE_ERROR` and with `apply:`, `geometric_antiproduct:`, `a % b:`, `product_plan`, `detail::`, and `truncate:` forbidden on the first error line (for `antiproject`, the `core.dual` block's internal names).

## Decision

The texts, as committed:

| Cause | Message |
| --- | --- |
| `transform`, V of both parities | transform: the versor holds blades of both parities, even and odd antigrade; a versor is even, as a motor, or odd, as a flector |
| `compose`, an operand of both parities | compose: an operand holds blades of both parities, even and odd antigrade; compose versors that are each even or odd |
| narrow scalar | transform: the scalar type cannot hold one of the transform's exact coefficients; use a scalar type that holds larger integers exactly (and "compose: ... the composition's ...", "antiproject: ... the antiprojection's ...") |
| two algebras | `<function>`: the operands are in different algebras; give both types the same algebra tag |
| two scalar types | `<function>`: the operands have different scalar types; give both types the same scalar type |
| the exact check (constant evaluation and run time) | transform: the result leaves the blades of x |

The exact check's message is shorter than R11's proposal ("transform: a coefficient outside the blades of x is not zero; V does not act as a versor on x"). A failed constant evaluation shows a `GALOOT_ASSERT` message only through the source line the compiler echoes, so the message must start on the line of `GALOOT_ASSERT(` (assert.hpp), and the 80-column layout leaves at most 48 characters there. It names the operation and what went wrong; the comment of `transform` says when it happens (a V that does not act as a versor on x). The rounding check's message is WU13b's.

Each condition of a `static_assert` is a named `bool` (`versor_has_one_parity`, `each_operand_has_one_parity`, `scalar_holds_every_coefficient_of_the_transform`, ...), so that Clang's first line, which prints the condition, shows no `detail::` name.

## Consequences

- Measured sizes, both compilers, in `records/diagnostics/wu13a-messages.md`: every rejection gives one error line, with the message on it.
- The narrow-scalar tests of `transform` have two cases, one whose large coefficient is only in the first plan and one only in the second, so that a check of one plan alone fails a test.

## Addendum, 2026-11-07: the exact check's message names the cause (review finding 5)

After the blind review, the exact check's message is "transform: v does not act as a versor on x" (43 characters) in place of "transform: the result leaves the blades of x", which named only the symptom. The reviewer suggested "transform: v is not a versor on x's blades"; it holds a quote character, which a GALOOT_ASSERT message must not (assert.hpp). No fix after a semicolon fits the 48 characters, so the usability checklist's "the fix after a semicolon" is not met for this message; the comment of transform says when it fires and what it means. The tests' expected text (`transform_dropped` in `test/CMakeLists.txt`) changed with the header in one commit.
