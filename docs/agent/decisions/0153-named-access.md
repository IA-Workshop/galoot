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

## Implementation, 2026-11-01 (WU8a)

Recorded by Claude (agent, Opus 5.5), cpp-expert role, implementing WU8a of plan 0016.

1. `has_blade<X, Name>` is a concept over data: `multivector<X>` and a lookup of the name in X's layout, a variable template evaluated once per type and name. The lookup reuses `blade_set.hpp`'s `name_count`, `next_name`, and `parse_name`, and returns why a name fails as a value, so the concept is false, never an error, for every name.
2. `get<Name>(x)` is one function template on a forwarding reference, constrained by `has_blade`, returning `std::forward_like<X>(x.coefficients[at])` with `at` a constant.
3. The rejection is one overload, constrained by `multivector<X> && !has_blade<X, Name>`, whose body selects the message of the cause by an `if constexpr` chain of `static_assert(false, ...)`. One overload with a branch per cause reads more plainly than seven overloads with seven constraints, and gives the same diagnostic, since each compiler prints the instantiation of the function and then the one failed assertion.
4. Departure from decision 0033's form P: the rejecting overload declares get's return type rather than deducing `auto` and returning a placeholder. With a deduced return type a `requires`-expression that asks whether `get<"e1">(line)` is valid instantiates the body and becomes a hard error (research note 0010, detection table); with the declared type it answers yes without compiling the message, which is the documented consequence plan 0016 test E10 expects. The return statement after the assertions keeps the declared type, so a use of the result, as in `get<"e1">(line) = 2.0`, raises no second error.
5. A seventh cause, beyond the six of item 3 above: a name that lists one generator twice, such as `e11`, is not a blade name in the sense of the other six messages, so it has its own.
6. The final messages, each tested on both compilers by a compile-fail test that requires it, forbids the other six and the parser's `blades:` messages, and requires one error:

| Cause | Message | Test |
| --- | --- | --- |
| An algebra of more than nine generators | get: names have one digit per index, so they are read only for n up to 9; reach the coefficient by its position | `core.mv_get_wide_algebra` |
| Not one name | get: give one blade name | `core.mv_get_two_names` |
| Not a name | get: a name is 1 or e followed by generator indices, as in e41 | `core.mv_get_not_a_name`, `core.mv_get_index_zero` |
| A repeated generator | get: the name lists one generator twice; a blade names each generator once | `core.mv_get_repeated_generator` |
| An index above n | get: the name has an index above n, the number of generators | `core.mv_get_above_n` |
| A blade the layout lacks | get: the type has no blade of this name; its layout lists the names it holds | `core.mv_get_absent` |
| The other orientation | get: the type stores this blade with the other orientation; use the name its layout gives it and negate the value | `core.mv_get_opposite` |

The wide-algebra message gained its fix after a semicolon ("reach the coefficient by its position"), following the message style of plan 0016 reading 21; the others are the plan's wording.

## WU8b, 2026-11-01: the absent message for a layout in canonical order

Recorded by Claude (agent, Opus 5.5), cpp-expert role, implementing WU8b of plan 0016, for HCI reading F7 of WU8a: "its layout lists the names it holds" points at nothing when the layout was computed by `full`, `of_grade`, `merge`, or a product, as WU8b's `apply` results are.

1. A layout whose blades all have sign +1 and come in canonical order, as every computed layout does, gets its own absent message: "get: the type has no blade of this name; its layout lists blades in order of grade, each named by its indices in increasing order". Other layouts keep "get: the type has no blade of this name; its layout lists the names it holds". A hand-written layout in canonical order, such as a point's e1 e2 e3 e4, gets the first, which is true of it too.
2. Not built: a function returning a layout's names as text, which the message could name. It would be a new public interface with no other caller; printing by oriented names is `format`'s (WU15), and the message can name it then.
3. Tested by `core.mv_get_absent_ordered` (one error, forbidding the other seven messages of get and the parser's) and `core.mv_get_absent`, unchanged.

### Review fixes, 2026-11-01

Recorded by Claude (agent, Opus 5.5), cpp-expert role, after the hci-expert reading of WU8b (S2), at `ef579e2`. It replaces item 1 of the WU8b section above. "Absent" means that the layout holds the mask in neither orientation, so the naming rule cannot lead the user to a name that works there; on the other-orientation path it is the fix. For a layout in canonical order:

- absent: "get: the type has no blade of this name, in either orientation; its layout does not hold it";
- the other orientation: "get: the type stores this blade with the other orientation; its layout names each blade by its indices in increasing order, as e12 for e21; use that name and negate the value".

Hand-written layouts keep the two messages of WU8a. Tested by `core.mv_get_absent_ordered` and `core.mv_get_opposite_ordered`, each forbidding the other eight messages of get.
