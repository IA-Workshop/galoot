# Diagnostics of the mistakes left for WU8b, before and after WU8b

Plan 0016, the WU8b row of the diagnostics record (section 5, "Other routes to readable diagnostics") and the items the WU8a reviews deferred to WU8b: the lossy assignment `const Point s = q;`, HCI reading F7 (the "absent" message for a computed layout), F8 (reference, const, and bool scalars), and F10 (double braces and whole arrays). Recorded by Claude (agent, Opus 5.5) in the cpp-expert role, implementing WU8b. A record of readability, not verification (V1 does not apply).

## What was compiled

- `program.cpp` here, one mistake per `WU8B_CASE`; case 0 is the correct program and compiles before and after.
- Before: the headers of `8f341c2` (develop's merge of WU8a), extracted by `git archive` into a temporary directory. After: this worktree's headers, as of `942a2d7` (the header's last change in this unit's code commits).
- Compilers and flags as in the WU8a record beside this directory: GCC 16.2.1 and Clang 23.1.4 with libstdc++ 16, `-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -fsyntax-only -fno-diagnostics-color`.
- Command, from the worktree root: `tools/timed spike - wu8b-diagnostics:run -- python3 -I docs/agent/records/diagnostics/wu8/wu8b/run.py 8f341c2` (about 7 s). Texts in `out/<compiler>-<before|after>-case<N>.txt`, counts in `out/summary.tsv`. The temporary tree's path is written `<before>/` in the texts.
- Measures as in the WU8a record: lines of output, error lines, the line of the first error, and the longest line with the include path removed ("installed").

## The table

Lines / longest installed line, with the number of errors where it is not one.

| Case | Mistake | GCC before | GCC after | Clang before | Clang after | Message after |
| --- | --- | --- | --- | --- | --- | --- |
| 8 | `const Point s = q;`, an implicit conversion that drops blades | 4 / 811 | 9 / 811 | 16 / 282 | 28 / 282 | the compiler's: GCC "conversion from ... to non-scalar type ... requested", Clang "no viable conversion from 'const Motor' ... to 'const Point'" |
| 9 | `const Point s{q};`, the same conversion written explicitly | 65 / 1,090 | 10 / 925 | 25 / 331 | 9 / 268 | conversion drops blades of its input: it is lossy, so request it with truncate<Type>(x) |
| 10 | `get<"e1">(b)` for `b` over `of_grade<4, 2>` | 10 / 511 | 10 / 511 | 9 / 249 | 9 / 249 | get: the type has no blade of this name; its layout lists blades in order of grade, each named by its indices in increasing order |
| 11 | scalar `const double` | 43 / 532, 2 errors | 10 / 379 | 19 / 235 | 9 / 230 | mv: the scalar type is a reference or is const or volatile; give the type itself, as in double for const double |
| 12 | scalar `double&` | 31 / 527, 2 errors | 10 / 374 | 16 / 165 | 9 / 230 | the same |
| 13 | scalar `bool` | compiles | 10 / 356 | compiles | 9 / 193 | mv: bool cannot be the scalar type, since true + true is true; use an integer type such as int |
| 14 | a scalar without a default constructor | 45 / 525, 3 errors | 14 / 1,338, 2 errors | 29 / 163, 2 errors | 9 / 228 | mv: the scalar type must be default constructible, copyable, and assignable; give it a default constructor that makes its zero |
| 15 | `const Point s{{1, 2, 3, 1}};` | 65 / 791 | 10 / 517 | 26 / 216 | 9 / 200 | mv: give the coefficients one by one, not as one array or braced list; write one value for each component, in the order the layout lists them |
| 16 | `const Point s{values};`, a `std::array<double, 4>` | 10 / 483, "1 == 4" | 10 / 475 | 12 / 210, "1 == 4" | 9 / 210 | the same |
| 17 | `const Motor s = p;`, an implicit conversion that drops nothing | 4 / 811 | 9 / 811 | 16 / 321 | 28 / 321 | the compiler's, as for case 8 |

In every after text that carries a Galoot message, that message is on the first error line after the inclusion context (line 7 on GCC, line 3 on Clang).

## What each outcome says

1. Cases 8 and 17, implicit conversions. Every conversion between layouts is an `explicit` constructor (STYLE S8), so an implicit one reaches no constructor and stays the compiler's error, as decision 0155 item 2 rules. The texts grew: GCC adds "note: explicit conversion function was not considered", pointing at the declaration of the conversion's catch-all (whose message is on the next lines, which GCC does not show), and Clang's notes list the new candidate constructors (28 lines against 16). The WU8a record's sentence that this row would be "replaced with the conversion catch-all" does not hold: an explicit constructor is not a candidate in copy-initialization. Measured alternative, not adopted (decision 0155 item 2 is the owner's): with the lossy catch-all declared without `explicit`, case 8 gives the conversion's own message, GCC 10 lines / 925 and Clang 9 / 268 (scratch copy of the header, selections `wu8b-diagnostics:implicit-catch-all:*` in the run log); the cost is a non-explicit single-argument constructor, against S8's wording, and `std::is_convertible_v<Motor, Point>` answering yes.
2. Case 9, the explicit lossy conversion: one error with the literal on both compilers (before, GCC printed 65 lines of candidates). The longest GCC line is the instantiation context, which prints both layouts as masks.
3. Case 10 (F7): same counts, new words. A layout that full, of_grade, or a product computes is in canonical order with every sign +1, and lists no names in the user's code; for such a layout the message now says how its blades are named, instead of pointing at a list the user never wrote. A hand-written layout in canonical order gets the same message, which is true of it too.
4. Cases 11 to 14 (F8): the scalar is now constrained in the template head by `std::destructible` only and checked in the class body in a fixed order, so each fault gives one error with its own literal. `bool` is now rejected (before it compiled; `ring_scalar<bool>` is false). Case 14's second error on GCC is the program's own use of a member of the scalar, `s.coefficients[0].sixteenths`: the storage of a rejected scalar that is not semiregular is a placeholder, so that making the value raises no second error; a use of a member of its coefficients then fails. The declaration alone gives one error on both compilers (`core.mv_scalar_without_default`).
5. Cases 15 and 16 (F10): one error with its own literal, instead of 65 lines of candidates on GCC (case 15) or the count message, whose cause the user did not commit (case 16).

Routes used (plan 0016, section 5): 2 (literal catch-alls), 3 (one error per mistake: the scalar's faults are reported first-only, and a single non-coefficient argument no longer reaches the count catch-all), 4 (the scalar checked in the class body under a weaker constraint), and 7 (message style).
