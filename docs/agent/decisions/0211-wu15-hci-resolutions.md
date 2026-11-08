# 0211 WU15 after the hci reading: the scalar's formatter test and four messages

- Status: recorded. The coordinator directed the resolution on 2026-11-08: S1 by option (a), C1 to C4 as the reading words them, S3, and C5, C6, C8 as comments. S2 (the rga3 tag's names) is outside WU15 and left to the owner; C7 is a CONVENTIONS wording proposal for the owner (C10).
- Date: 2026-11-08
- Recorded by: Claude (agent, Opus 5.5), implementer of WU15
- Bears on: `format.hpp`, `mv.hpp`, `linear.hpp`, `blade_set.hpp`, `README.md`; supersedes the message texts of decision 0204 item 5 (the third) and decision 0206 items 1 (the second), 2, and 3 (the first); refines decision 0204 item 3
- Source: `records/reviews/2026-11-08-wu15-hci-reading.md`

## Context

The hci-expert reading of WU15 found nothing blocking. Its S1 was a false message: a scalar whose `std::formatter` takes `std::format_parse_context&` and `std::format_context&`, the form most users copy, prints with `std::format` but fails `std::formattable<T, char>`, which tries a context whose iterator the standard leaves unspecified. The formatter of a multivector decided by `std::formattable`, so such a scalar was told it "has no std::formatter". C1 to C4 proposed clearer texts for four messages.

## Options weighed

- S1 (a): decide by whether `std::formatter<T, char>` is enabled, through `std::semiregular`. A disabled specialization is not default constructible, copyable, or movable ([format.formatter.spec]); an enabled one meets BasicFormatter, which requires all three. Widens what prints; narrows nothing.
- S1 (b): keep `std::formattable` and make the message say so. Keeps rejecting a formatter that works everywhere else, and changes a literal.
- C1 to C4: reword, or keep the texts. Each rewording changes one pinned literal.

## Decision

1. **S1, option (a).** `detail::has_formatter<T>` is `std::semiregular<std::formatter<T, char>>`, and it replaces `std::formattable<T, char>` in `parse`, `format`, and the choice of the scalar member. The scalar without a formatter is still rejected by the one message of decision 0204 item 5, and `std::formattable` of a multivector still answers yes (decision 0204 item 3).
2. **The new message texts:**
   - "mv: the source has another scalar type, and a conversion keeps the scalar type; convert each coefficient to the target's scalar type and build the target from those values";
   - "blades: the tag is not an algebra; an algebra tag holds its metric in a static constexpr member named value, as in struct rga3 { static constexpr auto value = rigid<3>(); }";
   - "a + b: a multivector and a number have no sum; make the number a multivector on the scalar blade 1 and add that", and its "a - b" counterpart ending "subtract that";
   - "format: an entry of the algebra tag's names is not a blade of the algebra; write 1, or e and distinct indices from 1 to n, the number of generators, as in e41".
3. **No message for a `names` member that is not one text** (C5). Decision 0204 item 4's reading stands; the comment now says such a member is not read.

## Consequences

- A behavior change in `include/` after the readings (S1), which the W5 review covers; the accept test `A scalar whose formatter takes std::format_context& prints inside a multivector` pins it.
- Four expected literals in `test/CMakeLists.txt` changed, each in a commit of its own before its header change (W9).
- The misuse catalogue's scan output and the mutation script's anchors follow the new texts.
- If a later unit adds a named scalar conversion, the `mv` message should name it.
