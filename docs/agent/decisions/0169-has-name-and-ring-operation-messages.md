# 0169 `has_blade` becomes `has_name`; a scalar's missing ring operation gets its own message

- Status: accepted (plan 0019 section 1, questions 2 and 5 (d); recorded by the WU10b implementer)
- Date: 2026-11-02
- Recorded by: Claude (agent, Opus 5.5), implementer of WU10b, cpp-expert role
- Bears on: decision 0033 (which named `has_blade`), plan 0016 reading 20, decision 0153 (named access), decision 0162 items 8 and 9, decision 0152, plan 0016 reading 21, `records/diagnostics/wu10b-messages.md`
- Supersedes: the name `has_blade` in decision 0033 and plan 0016 reading 20; their meaning is unchanged

## Context

`has_blade<Line, "e14">` is false, though a line holds the blade e14, as −e41. The concept tests something narrower than its name: whether an oriented name, with the orientation the layout gives it, is one `get` reads. It is `get`'s constraint, and `get`'s messages already draw the distinction ("stores this blade with the other orientation"). The WU8a reading proposed renaming it or documenting it; decision 0162 item 9 took "the recommendation" without saying which, and plan 0019 read it as the rename, because the name is what a user reads.

The multivector's message for a scalar without the ring operations listed all five requirements ("must support +, -, *, unary - and construction from an integer coefficient"), so a user had to find which one the type lacks. The reviews asked for the requirement that failed to be named (plan 0016 reading 21: each cause its own message); WU9 added only the fix after the semicolon.

## Options weighed

The name:
1. Keep `has_blade` and document the narrower meaning. As small, but the name keeps misleading.
2. `has_name<X, Name>` (chosen): true exactly when `get<Name>(x)` is accepted; false, never an error, otherwise.
3. `holds_name`. Reads as well, and is longer.
4. `has_coefficient`. Misleads: the line does have a coefficient on e14, negated.

The question "does the layout hold this blade in either orientation" gets no concept of its own: it has no present caller (S16). The comment says so.

The ring-operations messages:
1. One message that lists the five requirements (the old form).
2. Five messages, one per requirement of `ring_scalar` after `semiregular`, checked in the concept's order: addition, subtraction, multiplication, unary minus, construction from `coefficient_type` (chosen). A scalar with several faults gets the first one's message alone, as the earlier faults already work.

How the five checks stay equal to the concept:
1. Five separate requires-expressions in the multivector beside an unchanged `ring_scalar`. A sixth copy of the rule that can drift from it.
2. `ring_scalar` written as the conjunction of five named `detail` concepts (`ring_addition`, `ring_subtraction`, `ring_multiplication`, `ring_unary_minus`, `ring_from_coefficient`), which the multivector's fault function tests in the same order (chosen). `ring_scalar` stays one public concept, and generic code tests it as before. Each requirement is tested by a `static_assert` in `mv_test.cpp` against a scalar that lacks it alone.

## Decision

1. `has_blade<X, Name>` is renamed `has_name<X, Name>`, meaning unchanged, in `mv.hpp` (the concept, both of `get`'s constraints and their comments, "ask has_name instead") and in `mv_test.cpp`, in one commit that changes no expected value. No compile-fail source named it.
2. `ScalarFault::no_ring_operations` becomes `no_addition`, `no_subtraction`, `no_multiplication`, `no_unary_minus`, and `not_from_coefficient`, each with a named rule and its message, with the texts of the test author's catalogue copied verbatim:
   - "mv: the scalar type has no addition; give it a + b, or use a number type such as double"
   - "mv: the scalar type has no subtraction; give it a - b, or use a number type such as double"
   - "mv: the scalar type has no multiplication; give it a * b, or use a number type such as double"
   - "mv: the scalar type has no unary minus; give it -a, which the products use for a term of coefficient -1"
   - "mv: the scalar type cannot be made from an integer coefficient; give it a constructor from std::int64_t, or use a number type such as double"

## Consequences

- `has_blade` is gone with no alias. No release has shipped it, so no user is broken (W13); the release notes of the first release need not mention it.
- A user who passes `std::string` is told it has no subtraction, the first requirement it fails, rather than given the whole list.
- The order of the five checks is part of the interface: the compile-fail tests pin which message a scalar with several faults gets.
