# Diagnostics of the three likeliest mistakes, before and after WU8a

Plan 0016, criterion 11b and test E17. Recorded by Claude (agent, Opus 5.5) in the cpp-expert role, implementing WU8a. This is a record of readability, not verification (V1 does not apply).

## What was compiled

- Before: research note 0019's committed spike texts, `docs/agent/records/spikes/api-design/out/`, spelling M (the blade set as masks, `blades<4, "...">`) in forms R (constraints only) and P (a catch-all with a literal message), because no `mv` existed before WU8a. Case numbers are the spike's.
- After: `program.cpp` here, the same user program over WU8a's `include/galoot/core/mv.hpp` as of `b123788`, after the review fixes (first run at `878b624`, again at `669f7e2`; each change of the header moved line numbers, so the texts were regenerated, and `summary.tsv` did not change); a later change to the header calls for a new run. Spelling L declares each layout as a named variable and each type as an alias, as the examples recommend; spelling I writes the layout inside the alias, as the spike's spelling M did. They differ only in Clang's `aka` text, so the table shows L and the summary has both.
- Compilers: GCC 16.2.1 and Clang 23.1.4 with libstdc++ 16, flags `-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -fsyntax-only -fno-diagnostics-color -I ../../../../../include`, from this directory, as the spike compiled.
- Command, from the worktree root: `tools/timed spike - wu8-diagnostics:run -- python3 -I docs/agent/records/diagnostics/wu8/run.py` (about 4 s). It writes every text to `out/<compiler>-<spelling>-case<N>.txt` and the counts to `out/summary.tsv`.
- Measures: lines of output; error lines; the line of the first error; the longest line in characters. The spike's texts name `fixture.hpp` beside the program, while the after texts name `../../../../../include/galoot/core/mv.hpp`; "installed" is the longest line with that prefix removed, as a user who installs the headers reads it, and is the fair comparison. No randomness is involved.

## The table

Lines / longest line (installed length for after). One error in every text that fails.

| Mistake | Spike case | Compiler | Before, R | Before, P | After (WU8a) | First error line, after |
| --- | --- | --- | --- | --- | --- | --- |
| Wrong number of coefficients, `const Point s{1, 2, 3};` | 1 | GCC | compiles silently, s = (1, 2, 3, 0) | compiles silently | 10 / 468 | 7 |
| | | Clang | compiles silently | compiles silently | 12 / 194 | 3 |
| Other orientation, `get<"e14">(l)` | 5 | GCC | 25 / 677 | 9 / 534 | 10 / 504 | 7 |
| | | Clang | 13 / 301 | 8 / 218 | 9 / 248 | 3 |
| Blade the type lacks, `get<"e1">(l)` | 6 | GCC | 25 / 676 | 9 / 533 | 10 / 503 | 7 |
| | | Clang | 13 / 300 | 8 / 217 | 9 / 247 | 3 |
| Assigning to a type that drops components, `const Point s = q;` | 8 | GCC | 4 / 799 | 9 / 892 | 4 / 811 | 2 |
| | | Clang | 10 / 301 | 8 / 268 | 16 / 282 | 1 |

The last column is the line of the first error in the output. GCC prints the inclusion context (two lines) and then, for a message from a template, the instantiation context and the line of the call (four lines); Clang prints the inclusion context (two lines) and then the error. In every after text the error line is the first error and holds the message.

## What each outcome says, and which routes it used

Routes are numbered as in plan 0016, section 5, "Other routes to readable diagnostics".

1. Wrong number of coefficients. Before, the aggregate took three values for four components and set w = 0 with no warning on either compiler: the worst outcome, a silent wrong value. After, one error on both compilers:
   - GCC: `error: static assertion failed: mv: give one value for each component, in the order the layout lists them`, then `• the comparison reduces to ‘(3 == 4)’`.
   - Clang: `error: static assertion failed due to requirement 'sizeof...(Values) == component_count': mv: give one value ...`, then `note: expression evaluates to '3 == 4'`.

   Routes 2 (a numeric catch-all, so the compiler prints the counts), 3 (one error), and 7 (the message style). The longest line is GCC's instantiation context, which prints the layout as masks through libstdc++'s `std::__array_traits<...>::_Type` wrapper (route 1 would shorten it; not adopted).
2. Other orientation and a blade the type lacks. One error with the cause's own message, on the first error line after the inclusion context:
   - `get: the type stores this blade with the other orientation; use the name its layout gives it and negate the value`;
   - `get: the type has no blade of this name; its layout lists the names it holds`.

   Before, form P gave one message for both causes. After, the line and character counts are within a line and 30 characters of form P's: GCC's longest line (the instantiation context, 504 against 534) is shorter, mostly because GCC drops the `galoot::core::` prefix of the names it prints inside that namespace, where the spike's fixture was outside it; Clang's (the "in instantiation of" note, 248 against 218) is longer because it names `galoot::core::get` and the scalar `double`. Routes 2, 3 (an invalid name gives `get`'s message alone, never the parser's), 5 (one overload on a forwarding reference, whose rejecting twin has the same return type, so no follow-on error), and 7.
3. Assigning a motor to a point. WU8a has no converting constructor (WU8b adds them and their catch-all), so this is the compiler's own message, as plan 0016 section 5 says to record:
   - GCC: one line, `conversion from ‘mv<[...],galoot::core::blade_set<8>{...}>’ to non-scalar type ‘mv<[...],galoot::core::blade_set<4>{...}>’ requested`, 811 characters, the sets as masks through libstdc++'s wrapper.
   - Clang: `no viable conversion from 'const mv<[...], blade_set<8>{...}, [...]>' to 'const mv<[...], blade_set<4>{...}, [...]>'`, then a note per candidate constructor; the notes name `'const Motor' (aka 'const gc::mv<rga3, motor_layout, double>')`, which is route 6 at work: with the layout inline (spelling I) that line is 39 characters longer.

   No message names the mistake in the algebra's terms yet; WU8b's row for this mistake replaces it with the conversion catch-all ("conversion drops blades of its input: it is lossy, so request it with truncate<Type>(x)").

Route 4 (the scalar checked in the class body) does not bear on these three mistakes; for `mv<rga3, line_layout, std::string>` it gives one error on both compilers (`core.mv_not_ring_scalar`).

## For the owner

Whether more is needed is the owner's call (plan 0016, section 16). What remains long is GCC's printing of a layout's value in every instantiation context: 400 to 800 characters for a line or a motor, the same before and after, because the type's template argument is the set of masks. Route 1, the names as written in the type, measured 265 characters for the same `get` mistake in note 0019 (spelling N); it is the one route left that shortens GCC's lines materially.
