# 0050 Range of the bits functions, and their assertion messages

- Status: accepted
- Date: 2026-10-31
- Decided by: Claude (agent, Opus 5.5), implementing WU2 (plan 0004). The owner may reverse it.

## Context

Decision 0004 accepts 1 <= n <= 16 in v1 and asks that the limit be "a template parameter, not a hard-coded constant, so it can be raised without an interface change". Plan 0004 gives the bits functions n as a function argument (`antigrade(mask, n)`), and they are the lowest layer: no algebra or template parameter exists at that level. Plan 0004 criterion 5 asks that the functions reject out-of-range input by `GALOOT_ASSERT`.

`GALOOT_ASSERT`'s message reaches a compile-time diagnostic only through the source line the compiler echoes, so it must start on the line of `GALOOT_ASSERT(` (plan 0002, risk 8). Checked here: with the message on the next line, neither GCC 16.2.1 nor Clang 23.1.4 shows it. At 80 columns, a message that names the function (`right_complement_sign: ...`) does not fit beside its condition.

## Options

Range:

1. Assert n <= 16 in bits. That hard-codes the limit at the lowest layer, which is what decision 0004 asks not to do.
2. Accept every n the mask type can hold, 1 <= n <= 32 (`mask_bits`), and leave the v1 limit to the place where an algebra is declared (the metric, WU3), where it can be a template parameter.

Messages:

1. Name the function in each message and reword until it fits, which forces terse text and short helper names.
2. State only the precondition (`n must be from 1 to 32`, `blade has a generator beyond e_n`), and let the compile-time call trace or the run-time file and line name the function. Only `antigrade` and `complement_mask` assert; the sign functions call one of them before anything else, so they reject the same input with the same message.

## Decision

Range option 2 and message option 2. `n = 0` is rejected, as decision 0004 says. Masks are checked for bits at or above n. `grade`, `reorder_sign`, `reverse_sign`, and `grade_involution_sign` take no n and accept every mask. A `static_assert(mask_bits == 32)` keeps the literal "32" in the messages in step with the mask type.

## Consequences

WU3 enforces 1 <= n <= 16 (or its template parameter) when an algebra is declared. Bits is tested exhaustively to n = 8 and on a sample at n = 12, 16, and 32, so its stated range is covered at both ends. A test that expects a message cannot tell which function's assertion fired; the run-time tests also require `bits.hpp:` in the report, and each public function that takes n has its own rejection tests.
