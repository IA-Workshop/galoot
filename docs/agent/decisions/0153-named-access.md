# 0153 Named access to coefficients

- Status: accepted by the owner, 2026-11-01 ("proceed as recommended")
- Date: 2026-11-01
- Decided by: the owner, on plan 0016 question 9 (note 0012, open question 1); recorded by Claude (agent, Opus 5.5)

## Context

`get<"e41">(x)` reads and writes a coefficient by its oriented name (Spike F, note 0012). A Line stores e41 = −e14. `get<"e14">` on it could return −c by value, or be rejected.

## Options weighed

1. Return −c by value. It could not be written through, so `get` would have two behaviors, and it would hide CONVENTIONS 1.6.
2. Reject with its own message. Chosen.

## Decision

1. `get<Name>(x)` is a free function template on a forwarding reference, one overload, returning the coefficient with the value category of its argument, found unqualified by argument-dependent lookup. The name type is `blade_set.hpp`'s structural string, parsed by its parser, so a name means in `get` what it means in `blades`.
2. A name that the layout holds only in the other orientation is rejected with its own message, distinct from the message for a blade the layout lacks. Proposed wording: "get: the type stores this blade with the other orientation; use the name its layout gives it and negate the value".
3. Each cause has one message and gives one error: a blade the layout lacks, the other orientation, an index above n, a text that is not a name, more than one name, and an algebra of more than nine generators. An invalid name makes `has_blade` false without a second error from the parser.

## Consequences

Plan 0016 tests E6, E7, E9, and E10. The implementer records the final messages in an appended section here.
