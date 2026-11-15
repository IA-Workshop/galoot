# hci-expert reading: WU8b (plan 0016), `feature/core-mv-b` at `6c9df63`

- Reader: hci-expert, Claude Opus 5.5 (`claude-opus-5-5`), 2026-11-01. Agent work. Role: reviewer, reading the public interface WU8b adds as a user meets it (plan 0016 section 9, the reading before the W5 review). Advice; a blind reviewer checks correctness separately.
- Read as: a user who knows geometric algebra and C++23 and has never seen Galoot's internals.
- Subject: what WU8b adds to `include/galoot/core/mv.hpp` at `6c9df63`: the explicit conversions between layouts (`Motor{line}`), `truncate<Blades>(x)` and `truncate<Type>(x)`, the scalar checks (reference or cv, `bool`, not semiregular, no ring operations), the rejecting constructors for one non-coefficient argument, the F7 message for a canonical layout, `scalar_type`, and the compile-time coefficient-exactness check of `detail::apply`. The branch was not edited.
- Work: branch `review/wu8b-hci` from `6c9df63`, worktree `.claude/worktrees/hci-mv-b`. Programs, scripts, and every diagnostic text quoted below are in `2026-11-01-wu8b-hci-reading/` beside this file. The programs were written and first run in the session's scratch directory, outside the repository, and copied here.
- Compilers: GCC 16.2.1 and Clang 23.1.4 with libstdc++ 16. "Project" flags `-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off`, "plain" flags `-std=c++23 -Wall`; `-fsyntax-only -fno-diagnostics-color` for the mistake cases. Plain and project flags gave the same status, line count, and error count in every case, so the tables give the project flags.
- Verdict: **nothing blocks the merge.** The unit does what its catalogue promised: every catalogued misuse that reaches a library constructor or function gives one error whose literal is the first error line after the inclusion context (GCC line 7, Clang line 3), on both compilers; F8 and F10 of the WU8a reading are fixed, including forms the record did not try (a `std::vector`, a pointer, a `std::array<float, 4>`, a short double-brace list, `volatile double`). The values are right: re-signing between `e41` and `e14` layouts, widening, truncation by type and by layout, and a grade-2 projection by `of_grade` all print what the formula says. Seven findings should be acted on, two of them owner questions before release (S1, S7); nine could be.
- Job pool: not started or stopped; 23 of 23 slots free before the first run and after the last.

## Departures, skipped checks, and ambiguities (C20)

1. Task first was not pure. I read plan 0016 section 4, decision 0155, the diagnostics record, and the declarations of `mv.hpp` (to learn the names) before writing the programs; I read the bodies of the conversion, `truncate`, and lookup code after the first full run. The tasks in section 1 are written from the plan's snippets.
2. No CMake build or CTest run; the suites are the author's and the blind reviewer's. Every program here is a single compile, and three of them (`semantics.cpp`, `derived.cpp`, `plan_snippet.cpp`) were built and run.
3. Not run through `tools/timed`: four `-fsyntax-only` compiles of `plan_snippet.cpp` with `-DLOST` and `-DIMPLICIT`, and two of `derived.cpp` with `-DLOSSY` (under a second each, run in the same loops as timed builds), and git and file commands. Every other compile and run went through `tools/timed` (selections `hci-wu8b:*`); the rows are in this branch's `docs/agent/records/performance/runs.tsv`, committed with this record. The full mistake run is about 120 s (two of its runs are logged; the texts in `out/` are from the second, which includes cases 32 to 35).
4. The coefficient-exactness check is reachable only through `detail::apply`, which no user calls yet. I compiled the author's three compile-fail sources for it (`test/core/mv_apply_inexact_*_fail.cpp`) unchanged and read their output (`out/inexact-*.txt`); my judgment of it (C6) is for when products become public (WU9).
5. Knowledge base: the role file says to commit nothing there unless the brief says so; the brief is silent, and the owner's knowledge-base workflow is to commit as knowledge lands on a branch per collector, which the WU8a reading also did. I followed the workflow (section 9).
6. Only GCC and Clang with libstdc++; MSVC is deferred and libc++ was not tried. No person was observed: what a first-time user expects is my judgment.
7. The variant experiments (section 4) change copies of the header in `variant/include/` and `echo/include/`, written by the `make_header.py` beside each; those copies are not committed. Nothing under `include/` changed.

## 1. Task first

The three tasks this part exists for, as a user would write them from the plan's snippets:

1. Widen a value into a larger type: `const Motor m{line};` (also `Motor(line)` and `static_cast<Motor>(line)`), including between layouts that orient a blade differently (`e41` against `e14`).
2. Take part of a value: the line part of a motor, `gc::truncate<Line>(motor)` or `gc::truncate<line_layout>(motor)`; the bivector part, `gc::truncate<gc::of_grade<4, 2>>(motor)`.
3. Choose a scalar type, and be told clearly when it is wrong: `const double`, `double&`, `bool`, a fixed-point class without a default constructor, a class missing `*`, `std::unique_ptr<double>`, a class with a `const` member.

Tasks 1 and 2 compiled first time on both compilers and printed the expected values (`semantics.cpp`, both checking modes, `out/` not needed: the output is quoted in section 5). Task 3 gives one literal per fault, with the two wrong fixes of S3. Places I had to leave the snippets:

- Whether `truncate` checks anything for `double`: only its comment says (it does not), and the plan's snippet does not show an exact scalar.
- What `truncate` does to an integer motor whose scalar part is not zero: the comment's Requires paragraph answers it, and the answer was a surprise (S1).
- How to change the scalar type, `FLine{line}` from a `double` line: there is no way, and the error is 113 lines on GCC (S7).
- Whether `truncate<MyLine>(m)` accepts a class derived from `mv`, as `Motor{l}` does: it does not, and says the target is not a multivector type (S4).

## 2. The checklist (decision 0160), item by item

Evidence is a case of `user.cpp` (`out/<compiler>-<flags>-case<N>.txt`), a line of `mv.hpp` at `6c9df63`, or a program beside this file.

### Names

| Item | Verdict | Evidence |
| --- | --- | --- |
| Field's terms or full words | Met | `truncate`, `scalar_type`, the messages' "layout", "blade", "component", "coefficient". The WU8a reading's F3 (layout or blade set) is fixed: every `mv:` message now says "layout" |
| One meaning across the library; nothing std means differently | Met, with S1's reservation | No `std::truncate` exists; `std::trunc` rounds, and the comment says "it drops components, not digits". POSIX `::truncate(const char*, off_t)` in scope does not interfere with the unqualified call (case 30), and ADL finds it (31). The reservation: for an exact scalar, `truncate` may not drop anything nonzero, which is not what the word says (S1) |
| Similar things look alike | Partly met | `truncate<Type>` and `truncate<Blades>` mirror `static_cast<Type>` and the layout NTTP; `scalar_type` beside `algebra_type` (WU8a F4 fixed). Not met: the conversion's message is the only one without its operation as a prefix (S5) |

### Construction and conversion

| Item | Verdict | Evidence |
| --- | --- | --- |
| The shortest form is correct | Met | `Motor{l}`; `Point{values}`, `Point(values)`, `Point{{1, 2, 3, 1}}`, `Point{{1, 2, 3}}`, `Point{vector}`, `Point{values.data()}`, `Point{array_of_float}` are each one error with the literal (19 to 21, 23 to 25, 27). `Point s = {{1, 2, 3, 1}};` is the compiler's "chosen constructor is explicit" on Clang and two errors on GCC (22; C1) |
| Default value zero | Met (WU8a) | |
| Lossy conversions named, never implicit; lossless explicit | Met | `Point{q}`, `Point(q)`, `static_cast<Point>(q)`, `emplace_back(q)` give the conversion's literal (2, 9, 8); every implicit form is the compiler's error (1, 3 to 7, 29). `std::is_convertible_v<Line, Motor>` is false (`semantics.cpp`) |
| No hidden default changes a result | Not met for `truncate` | Whether `truncate<ILine>(int_motor)` with a nonzero scalar part compiles, aborts, or drops depends on the scalar type, on constant evaluation, and on `GALOOT_CHECKING` (S1; case 17, `semantics.cpp`) |

### Misuse and diagnostics

| Item | Verdict | Evidence |
| --- | --- | --- |
| Each catalogued misuse has a compile-fail test on both compilers requiring the literal | Met | `test/CMakeLists.txt:931-1123`, with `ONE_ERROR` and `FORBID` of the neighboring messages. Not in the catalogue: a move-only or non-assignable scalar (47, 48), copy-list double braces (22), another scalar or algebra (10, 11) |
| One error per misuse, the message on the first error line | Met for every library literal; four exceptions | Line 7 on GCC, line 3 on Clang, one error, in cases 2, 9, 12, 13, 19 to 28, 33, 40 to 45, 47, 49, 60 to 65. Exceptions: `truncate<double>(q)` and `const FLine s = truncate<FLine>(q);` add a second error, because the catch-all returns x (14, 32; C2); a scalar with no default constructor or a `const` member adds one on GCC when the program uses a member of the coefficients (43, 48; known from the author's record); a constant evaluation of `truncate` that drops a nonzero coefficient puts `assertion_failed` on GCC's first error line and the message five lines later (17; C5) |
| Operation first, mistake in the algebra's terms, fix after a semicolon | Mostly met | Every scalar message has a fix (40 to 49) except the ring-operations one (44, 45; C3). The conversion message has no operation prefix (S5). The semiregular message's fix is wrong for two of its three causes (47, 48; S3). The F7 messages give the naming rule where it does not help and leave it out where it does (60, 61; S2) |
| Context lines show the user's terms | Not met on GCC (WU8a F6, unchanged) | GCC prints layouts as masks in every context line, up to 2,558 characters (17); Clang names `Motor` only where the type is spelled through its alias (29) |
| Distinct causes, distinct messages | Not met in three places | S3 (three causes, one message and one fix), C3 (five requirements, one message), C4 (a single value that does not convert to T gets the array message, 28 and 33) |
| Probed concepts test data; the documentation says which probes answer wrongly | Partly met | The count and array catch-alls document their `is_constructible` answers (`mv.hpp:515`, `:532`). The lossy catch-all does not, and `std::is_constructible_v<Line, Motor>` is true (`semantics.cpp`; S5) |

### Semantics and cost

| Item | Verdict | Evidence |
| --- | --- | --- |
| Operations that discard, re-represent, or assume say so | Met in the comment, not in the name for exact scalars | `truncate`'s comment states what is kept, negated, zero, and dropped; the Requires paragraph mixes checking mode with constant evaluation (S1) |
| `auto r = f(a, b);` is safe | Met | `truncate` and the conversions return values; `truncate<const Line&>` returns `Line` (`semantics.cpp`) |
| Exact comparisons `==` | Met (WU8a) | |
| Predictable cost | Met | The conversion's kernel runs at compile time; `truncate` documents no cost, but it is the same identity kernel |

### Examples and readability

| Item | Verdict | Evidence |
| --- | --- | --- |
| Every example compiles and is the idiom to copy | Met where examples exist | Plan section 4's "Moving between blade sets" compiles and runs on both (`plan_snippet.cpp`); its two commented errors are as stated on Clang ("no viable conversion" on GCC is "conversion from ... requested"). The header's opening comment, the only user documentation, shows no conversion or `truncate` (C8) |
| A maintainer finds each behavior; one rule, one place | Met | The scalar faults are one `consteval` function in a fixed order (`mv.hpp:127-140`) and four named constants; each catch-all is beside the constructor it complements |

## 3. The programs: 56 mistake cases, two compilers, two flag sets

`user.cpp` with `-DCASE=N`, compiled by `run.py` (224 compiles, about 120 s; `out/summary.tsv`). Lines / errors / longest line, project flags; "ok" means it compiled. Paths in the texts are relative to the worktree root; the longest line includes them.

| # | What the user wrote | GCC | Clang | First error line after the context (GCC unless marked) |
| --- | --- | --- | --- | --- |
| 0 | the correct program | ok | ok | |
| 1 | `const Point s = q;` (lossy, implicit) | 9/1/808 | 28/1/290 | `conversion from ‘mv<[...],galoot::core::blade_set<8>{...masks...}’ to non-scalar type ‘mv<[...],...blade_set<4>...>’ requested`; Clang `no viable conversion from 'const mv<[...], blade_set<8>{{{{0, 1}, ...}}}, [...]>' to ...` |
| 2 | `const Point s{q};` | 10/1/933 | 9/1/265 | `conversion drops blades of its input: it is lossy, so request it with truncate<Type>(x)` |
| 3 | `const Motor s = l;` (lossless, implicit) | 9/1/886 | 28/1/327 | as 1 |
| 4 | `return l;` from a function returning `Motor` | 11/1/871 | 28/1/327 | `could not convert ‘l’ from ...masks...`; Clang `no viable conversion from returned value of type ...` |
| 5 | `take_motor(l)`, a `const Motor&` parameter | 7/1/995 | 7/1/307 | `invalid initialization of reference of type ‘const Motor&’ {aka ...masks...}`; Clang `no matching function for call to 'take_motor'` |
| 6 | `take_point(q)` | 7/1/918 | 7/1/285 | as 5 |
| 7 | `motors.push_back(l)` | 23/1/1565 | 10/1/390 | `no matching function for call to ‘std::vector<...masks...>::push_back(const Line&)’` |
| 8 | `points.emplace_back(q)` | 21/1/1700 | 18/1/513 | the conversion's literal, at line 18 (GCC), inside `std::construct_at` |
| 9 | `static_cast<Point>(q)` | 10/1/933 | 9/1/266 | the conversion's literal |
| 10 | `const FLine f{l};` (double to float) | 113/1/1191 | 43/1/241 | `no matching function for call to ‘galoot::core::mv<...masks...>::mv(<brace-enclosed initializer list>)’`; Clang `no matching constructor for initialization of 'const FLine' (aka ...)`, then ten candidates, one of them "could not match 'float' against 'double'" (S7) |
| 11 | `const R3Vector v{p};` (another algebra) | 113/1/998 | 43/1/288 | as 10 (S7) |
| 12 | `truncate<FLine>(q)` | 10/1/700 | 9/1/321 | `truncate: the target must be a multivector type of the same algebra and scalar type; truncate changes only which blades are held` |
| 13 | `truncate<R3Vector>(p)` | 10/1/532 | 9/1/251 | as 12 |
| 14 | `truncate<double>(q)`, the scalar part as a number | 14/2/549 | 12/2/226 | as 12, then `r += s` fails, s being the Motor the catch-all returns (C2) |
| 15 | `truncate<"e41 e42 e43 e23 e31 e12">(q)` | 51/1/320 | 13/1/188 | `no matching function for call to ‘truncate<"e41 ...">(const Motor&)’` (C9) |
| 16 | `truncate(q)` | 34/1/257 | 13/1/110 | `no matching function for call to ‘truncate(const Motor&)’` |
| 17 | `constexpr auto s = truncate<ILine>(im);`, a nonzero dropped part | 24/1/2558 | 22/1/251 | `call to non-‘constexpr’ function ‘void galoot::core::detail::assertion_failed(const char*, const char*, int)’`, the message in the echoed `GALOOT_ASSERT` line; Clang `constexpr variable 's' must be initialized by a constant expression` (C5) |
| 18 | `truncate<Line>(l, 1e-12)`, a tolerance | 22/1/257 | 13/1/140 | `no matching function for call to ‘truncate<Line>(const Line&, double)’` |
| 19 | `const Point s{{1, 2, 3, 1}};` | 10/1/529 | 9/1/208 | `mv: give the coefficients one by one, not as one array or braced list; write one value for each component, in the order the layout lists them` |
| 20 | `const Point s{values};`, a `std::array` | 10/1/483 | 9/1/208 | as 19 |
| 21 | `const Point s(values);` | 10/1/483 | 9/1/208 | as 19 |
| 22 | `const Point s = {{1, 2, 3, 1}};` | 17/2/908 | 7/1/85 | `converting to ‘const Point’ ... from initializer list would use explicit constructor`, then the literal as a second error; Clang `chosen constructor is explicit in copy-initialization` (C1) |
| 23 | `const Point s{vector};` | 10/1/481 | 9/1/208 | as 19 |
| 24 | `const Point s{{1, 2, 3}};`, one short | 10/1/529 | 9/1/208 | as 19 (no silent zero) |
| 25 | `const Point s{values.data()};` | 10/1/468 | 9/1/208 | as 19 |
| 26 | `const Point s{1};` | 10/1/466 | 12/1/198 | the count message (WU8a) |
| 27 | `const Point s{array_of_float};` | 10/1/482 | 9/1/208 | as 19 |
| 28 | `const Point s{std::complex<double>{1, 0}};` | 10/1/475 | 9/1/208 | as 19 (C4) |
| 29 | `const std::optional<Point> s = q;` | 13/1/889 | 27/1/369 | the compiler's; Clang names `'const Motor' (aka 'const gc::mv<rga3, motor_layout, double>')` |
| 30 | `truncate<Line>(q)` unqualified, `<unistd.h>` included | ok | ok | |
| 31 | `truncate<Line>(q)` unqualified | ok | ok | |
| 32 | `const FLine s = truncate<FLine>(q);` | 14/2/888 | 36/2/356 | as 12, then the compiler's conversion error (C2) |
| 33 | a one-blade type from `std::complex<double>` | 10/1/367 | 9/1/208 | as 19, a cause the user did not commit (C4) |
| 34 | `const Scalar s = 2.0;`, one blade | 9/1/307 | 31/1/212 | the compiler's; the one-value constructor is explicit, as documented |
| 35 | `Line{truncate<Line>(q)}`, a round trip | ok | ok | |
| 40 | scalar `const double` | 10/1/379 | 9/1/238 | `mv: the scalar type is a reference or is const or volatile; give the type itself, as in double for const double` |
| 41 | scalar `double&` | 10/1/374 | 9/1/238 | as 40 |
| 42 | scalar `bool` | 10/1/364 | 9/1/201 | `mv: bool cannot be the scalar type, since true + true is true; use an integer type such as int` |
| 43 | scalar without a default constructor | 14/2/1336 | 9/1/236 | `mv: the scalar type must be default constructible, copyable, and assignable; give it a default constructor that makes its zero`; GCC's second error is the program's use of a member |
| 44 | scalar `std::string` | 10/1/393 | 9/1/209 | `mv: the scalar type must support +, -, *, unary - and construction from an integer coefficient` (C3) |
| 45 | scalar with `+` and `-` only | 10/1/367 | 9/1/209 | as 44 (C3) |
| 46 | an aggregate scalar with a ring's operators and no constructor | ok | ok | accepted: parenthesized aggregate initialization makes it constructible from `std::int64_t` |
| 47 | scalar `std::unique_ptr<double>` | 10/1/384 | 9/1/236 | as 43, whose fix (a default constructor) the type already has (S3) |
| 48 | scalar with a `const` member | 14/2/1333 | 9/1/236 | as 43; the fault is assignment (S3) |
| 49 | scalar `volatile double` | 10/1/382 | 9/1/238 | as 40 |
| 50 to 52 | `std::complex<double>`, `unsigned`, `char` | ok | ok | accepted |
| 53 | a pointer to `mv<..., later>` with `later` incomplete | 22/2/533 | 10/1/161 | `template constraint failure ... requires (metric_like<Alg>) && (destructible<T>)`: `std::destructible` in the template head rejects an incomplete scalar even where only a pointer is formed. Rare; noted, not a finding |
| 60 | `get<"e1">(b)`, `b` over `of_grade<4, 2>` | 10/1/519 | 9/1/247 | `get: the type has no blade of this name; its layout lists blades in order of grade, each named by its indices in increasing order` (S2) |
| 61 | `get<"e21">(b)` | 10/1/520 | 9/1/248 | `get: the type stores this blade with the other orientation; use the name its layout gives it and negate the value` (S2) |
| 62 | `get<"e12">(p)`, a hand-written layout in canonical order | 10/1/474 | 9/1/230 | as 60 |
| 63 | `get<"e14">(l)` | 10/1/524 | 9/1/252 | as 61, and here the layout's name `e41` is in the user's code |
| 64 | `get<"e1">(l)` | 10/1/523 | 9/1/251 | `get: the type has no blade of this name; its layout lists the names it holds` |
| 65 | `get<"e12">` on a `merge<point_layout, of_grade<4, 3>>` | 10/1/565 | 9/1/265 | as 60 |

Clang prints the named constant for each scalar fault ("due to requirement 'scalar_is_not_bool'"), which reads as the rule; the conversion and `get` messages have no such clause.

## 4. The implicit-conversion question

### Why `const Point s = q;` gives the compiler's error and `Point{q}` the literal

`Point{q}`, `Point(q)`, and `static_cast<Point>(q)` are direct-initialization, which considers every constructor, so overload resolution chooses the lossy catch-all and its `static_assert` fires. `const Point s = q;` is copy-initialization, which considers only converting constructors, those not declared `explicit` ([over.match.copy]). Every conversion constructor of `mv` is `explicit`, as STYLE S8 requires, so no constructor is a candidate and the compiler reports that no conversion exists. Passing an argument (5, 6), returning (4), and `push_back` (7) are copy-initialization too. The comment on the lossy catch-all says so (`mv.hpp:499-502`). From the user's side the GCC message names neither `Point` nor `Motor`, only masks (808 characters); GCC adds "note: explicit conversion function was not considered" pointing at the line of the constructor it skipped. Clang's names neither type in its first line either (template diffing prints `mv<[...], blade_set<8>{...}, [...]>`), then lists eight candidates with 'const Motor' (aka ...) in each.

### Would a non-explicit lossy catch-all be better for users? No: measured

`variant/` holds a program of eleven cases and a copy of the header with the catch-alls behind macros (`variant/make_header.py`, `variant/run.py`, 88 compiles, 22 s, `variant/out/summary.tsv`). Settings: the branch; the lossy catch-all without `explicit` (the author's measured alternative); that plus a non-explicit lossless catch-all ("a conversion between layouts is explicit, though this one drops nothing; write Type{x}"), which a consistent design would need; and the array catch-all without `explicit`.

| Case | Branch (GCC / Clang lines) | Lossy non-explicit | Both non-explicit |
| --- | --- | --- | --- |
| `const Point s = q;` | 9 / 28, the compiler's | 10 / 9, the literal | the literal |
| `const Motor s = l;`, `return l;`, `take_motor(l)`, `push_back(l)` | 7 to 23 / 7 to 28, the compiler's | unchanged | 10 / 9, the lossless literal |
| `f(q)` with `f(const Point&)` and `f(const Line&)` | 21 / 10, "no matching function" | 13 / 10, "call is ambiguous" | the same |
| `g(l)` with `g(const Motor&)` and `g(const Point&)` | 21 / 10, "no matching function" | 10 / 9, **the lossy literal**: it tells the user to call `truncate`, when the fix is `g(Motor{l})` | "call is ambiguous" |
| `const std::variant<Point, Line> v = q;` | 8 / 22 | 8 / 104, four errors on Clang | the same |
| generic `as_point(x)`: `if constexpr (std::is_convertible_v<X, Point>) return x; else return truncate<Point>(x);`, called with a Motor | **compiles and is correct** | **fails**: `is_convertible_v<Motor, Point>` is now true, so it takes the first branch and stops at the literal | fails |
| control: `Motor{cl}`, `Motor(l)`, `static_cast<Motor>(Line{...})`, `emplace_back(l)`, `Point p = {1, 2, 3, 1}` | compiles; `Motor{l}` correct | compiles | compiles; direct-initialization still picks the real conversion for a const, a non-const, and an rvalue source |

So the non-explicit lossy catch-all buys the literal for one spelling, and pays with a correct generic idiom that no longer compiles, a message that gives the wrong fix in an overload set, an ambiguity where there was "no matching function", a longer `std::variant` error, and traits that lie (`std::is_convertible_v<Motor, Point>` true). The author's decision not to adopt it (decision 0155, WU8b section, item 3) is right, and these measurements are the evidence the owner would need if the question is raised again. The array setting changed only `const Point s = {{1, 2, 3, 1}};`, to one error with the literal on both compilers (C1).

### A cheaper way to put the fix next to the compiler's message (S6)

`echo/` (`make_header.py`, texts in `echo/`): a comment at the end of each converting constructor's declaration line. When copy-initialization skips an explicit constructor, GCC echoes the line of the constructor it would have chosen, and Clang echoes every explicit constructor it did not consider. Measured, `const Point s = q;` on GCC:

```
include/galoot/core/mv.hpp:505:22: note: explicit conversion function was not considered
  505 |   constexpr explicit mv(const mv<Alg, From, T>& /*source*/) { // lossy: truncate<Type>(x)
```

and `const Motor s = l;` echoes line 490 with `// explicit: Type{x}`. Clang's notes for both show both lines. The first error line is unchanged; the fix is now on the screen. Argument passing (case 5) gets no such note. This is the same technique the project already relies on for `GALOOT_ASSERT`, whose message is read from the echoed source line.

## 5. `truncate`'s semantics

What it does (`semantics.cpp`, both compilers, both checking modes):

```
Motor{l}        [0, 1, 2, 3, 4, 5, 6, 0]
Line14{l}       [-1, -2, -3, 4, 5, 6]          (e14 = -e41)
truncate<Line>  [1, 2, 3, 4, 5, 6]
grade 2 of m    [6, -5, -1, 4, -2, -3]         (e12 e13 e14 e23 e24 e34 of e41..e12 = 1..6)
truncate<Point>(m) [0, 0, 0, 0]                (no blade in common: all zero, no diagnostic)
is_convertible<Line, Motor> 0; is_constructible<Motor, Line> 1;
is_constructible<Line, Motor> 1 (lossy: an error when used);
is_constructible<Point, std::array<double, 4>> 1; constructible_from<Line, ILine> 0
GALOOT_CHECKING=0: truncate<ILine>(int motor, nonzero scalar) [1, 2, 3, 4, 5, 6]
GALOOT_CHECKING=1: galoot: assertion failed: truncate: a dropped coefficient is not zero (include/galoot/core/mv.hpp:776), abort
```

Every value is right. What surprises is the last two lines with case 17: for an exact scalar, the same call `truncate<ILine>(im)` on a motor whose scalar part is 7

- fails to compile in a constant evaluation, in every checking mode;
- aborts at run time in a checking build, naming `mv.hpp:776`, not the caller;
- drops the 7 silently at run time in an unchecked build, which is what a user's project gets by default (`assert.hpp:11-13`);

while for `double` it drops the 7 in all three. The ratified rule (CONVENTIONS 4.5, version 1.7) is exactly this, and the comment states it. The user-facing consequence is S1: the only operation in core v1 that removes components (plan 0001's list has no grade selection) cannot be used to take the line part of an integer or rational motor, though the word "truncate" and the first sentence of its comment say that is what it does.

## 6. Against the author's diagnostics table

`docs/agent/records/diagnostics/wu8/wu8b/README.md` (measured at `942a2d7`; the header changed after it only in the kernel's return and comments). My numbers for the same mistakes, at `6c9df63`, agree within the length of the paths: author case 8 / mine 1 (GCC 9/811 against 9/808, Clang 28/282 against 28/290), 9 / 2 (10/925 and 9/268 against 10/933 and 9/265), 10 / 60, 11 / 40, 12 / 41, 13 / 42, 14 / 43 (GCC two errors in both), 15 / 19, 16 / 20, 17 / 3. The claim that every library message is on the first error line after the inclusion context holds for every case of mine that reaches one, except `truncate`'s constant-evaluation failure, which is not a `static_assert` (17).

What the table does not show, and a user meets: copy-list double braces (22, two errors on GCC); a conversion to another scalar or algebra (10, 11; 113 lines on GCC); the constant-evaluation failure of `truncate` (17, 2,558 characters on GCC); the second error after `truncate`'s catch-all (14, 32); `truncate<Derived>` (S4); the semiregular message's wrong fix (47, 48); and the F7 message on the other-orientation path (61). Its item 3 says the F7 message "says how its blades are named"; it does, on the path where naming cannot help (S2).

## 7. Findings

Severity for users: **blocking** (should stop the merge), **should** (fix in this unit, or decide before release), **could** (worth doing; no harm in waiting). None is blocking.

### S1. `truncate` means two things, chosen by the scalar type (should; owner, before release)

- Evidence: section 5; case 17; `mv.hpp:790-818`; CONVENTIONS 4.5 version 1.7; plan 0001's list of core operations (no grade selection besides `truncate`). For an exact scalar `truncate` is a checked lossless conversion (it may drop only zeros); for a rounding scalar it is a projection. A user who writes `truncate<Line>(motor)` to take the line part, the natural reading of the name and of the comment's first sentence, has a correct program for `double` and an incorrect one for `std::int64_t` or a rational, whose outcome then depends on constant evaluation and `GALOOT_CHECKING`. The checklist's "no hidden default changes a result: scalar type, ..., checking mode" is not met. It matters most to the users who choose exact scalars to verify their geometry.
- Proposal, for the owner (the rule is ratified text): give the two meanings two names. Either (a) keep `truncate` as ruled and add, before release, a projection that never checks (`part<Blades>(x)`, or `grade<k>(x)` for the common case), and say in `truncate`'s first line "for an exact scalar, only for a value whose dropped part is zero"; or (b) make `truncate` the projection for every scalar (what its name says) and give the checked form its own name. (a) is additive and keeps the ratified text. Either way, rewrite the Requires paragraph, which now reads as if the check ran only in checking mode: "Requires, for an exact scalar with ==: every coefficient that it drops is zero. Checked in every constant evaluation, and at run time when GALOOT_CHECKING is 1; with GALOOT_CHECKING 0 the coefficients are dropped unchecked."

### S2. The F7 messages give the naming rule on the wrong path (should; cheap, this unit)

- Evidence: cases 60, 61, 65; `mv.hpp:905-916`, `:1005-1015`. "Absent" means the mask is not in the layout in either orientation, so the rule "each named by its indices in increasing order" cannot lead the user to a name that works (60, 65). The other-orientation path (61, `get<"e21">` on a canonical bivector) is where the rule is the fix, and there the message says "use the name its layout gives it", a name a generated layout never showed the user.
- Change: two messages. Absent, either kind of layout: "get: the type has no blade of this name, in either orientation; its layout does not hold it" (or keep "its layout lists the names it holds" for hand-written layouts). Other orientation, canonical layout: "get: the type stores this blade with the other orientation; a generated layout names each blade by its indices in increasing order, as e12 for e21; use that name and negate the value". Only the literals of two tests change.

### S3. The semiregular message gives a wrong fix for two of its causes (should; cheap, this unit)

- Evidence: case 47 (`std::unique_ptr<double>`, which has a default constructor and cannot be copied) and case 48 (a class with a `const` member, which cannot be assigned) get "give it a default constructor that makes its zero". Three causes, one message, and the fix is right for one.
- Change: split the check, in the same first-fault order: not `std::default_initializable` gives the present message; otherwise not `std::copyable` gives "mv: the scalar type must be copyable and assignable, as a number is; a move-only type or a class with a const or reference member is not". One more compile-fail test (a move-only scalar).

### S4. `truncate<Derived>(x)` says the target is not a multivector type (should; this unit or with WU8a's F6)

- Evidence: `derived.cpp` with `-DDERIVED_TARGET` (`struct Line : gc::mv<...> { using mv::mv; };`): "truncate: the target must be a multivector type of the same algebra and scalar type", which is false as the user reads it. This unit made the constructors accept classes derived from `mv` (decision 0155, WU8b section, item 4): `Motor{line}` between two derived classes compiles and is correct. `truncate`'s target test uses `multivector`, which is exact `mv` specializations only. A derived target also cannot be made from its own base with the same layout, `Line{truncate<line_layout>(m)}`, since [over.match.funcs]/9 excludes the inherited constructor (`derived.cpp` as committed fails there, both compilers).
- Change, cheap: say it, in `truncate`'s comment and message ("the target must be an mv type, not a class derived from one, of the same algebra and scalar type"). The full answer, accepting derived targets, belongs with the owner's decision on named model classes (WU8a reading F6), since such a class must then declare its own constructor from its base.

### S5. The conversion's message lacks its operation, and its probe answer is undocumented (should; cheap, this unit)

- Evidence: cases 2, 8, 9: "conversion drops blades of its input: ..." is the only library literal without `mv:`, `get:`, or `truncate:` in front. `std::is_constructible_v<Line, Motor>` is true (`semantics.cpp`) because of the catch-all; the count and array catch-alls document their equivalent answers (`mv.hpp:515`, `:532`), this one does not.
- Change: "mv: the conversion drops blades of its input: it is lossy, so request it with truncate<Type>(x)" (three tests' literal), and in the catch-all's comment: "Because this constructor exists, std::is_constructible_v<Line, Motor> is true; std::is_convertible_v is false."

### S6. Put the fix on the echoed declaration line (should; free, this unit)

- Evidence: section 4, `echo/`. GCC's "explicit conversion function was not considered" note and Clang's "explicit constructor is not a candidate" notes echo the converting constructors' lines.
- Change: end the lossless constructor's declaration line with a comment such as `// explicit: write Type{x}` and the lossy catch-all's with `// lossy: use truncate<Type>(x)`, within the formatter's 80 columns (for example by moving `{` or shortening the parameter comment), so `clang-format` does not move it to the next line. No trait, test, or first line changes.

### S7. A conversion to another scalar type or algebra is 113 lines on GCC, and the scalar cannot be changed at all (should; owner or a later unit, before release)

- Evidence: cases 10 (`FLine{line}`, double to float) and 11 (another algebra): GCC 113 lines, longest 1,191 characters, ten candidates with masks; Clang 43 lines, the cause ("could not match 'float' against 'double'") in its fourth note. Changing the scalar of a value is a common need (double for building, float for upload), and nothing in the interface does it. Decision 0155 item 4 and the plan accept the compiler's message here, and `mv_test.cpp:2047` asserts `!std::is_constructible_v<Line<double>, Line<float>>`.
- Proposal: a scalar conversion under its own name (a later unit; it is lossy for double to float, so S8's rule asks for a name, for example `scalar_cast<float>(line)`), and then an explicit catch-all for a multivector of the same layout family with another scalar: "mv: the source has another scalar type; a conversion between layouts keeps the scalar; use scalar_cast". The catch-all makes `is_constructible` true for that pair, which changes the test at `:2047`; that is the owner's trade, and it is cheaper to decide before release.

### C1. Drop `explicit` from the C-array catch-all (could)

- Evidence: case 22 and `variant/` setting "array": `const Point s = {{1, 2, 3, 1}};` goes from GCC 17 lines and two errors, Clang "chosen constructor is explicit in copy-initialization", to one error with the literal on both; no other case, trait, or the control program changed. Only a braced list or a real C array binds to that parameter, so nothing converts silently. Mention in its comment why it alone is not explicit.

### C2. `truncate`'s catch-all returns x, which makes a second error (could)

- Evidence: cases 14 and 32 (two errors each, both compilers): the catch-all's `auto` return is the source, so `const FLine s = truncate<FLine>(q);` then fails to convert a Motor. Change: return `std::remove_cvref_t<Target>{}` when that type is default-constructible (it is for every case tried: `double`, `int`, an `mv` of another scalar or algebra), otherwise x.

### C3. The ring-operations message lumps five requirements and gives no fix (could)

- Evidence: cases 44, 45. A user's own scalar missing only `*` is told it "must support +, -, *, unary - and construction from an integer coefficient", and "integer coefficient" is the library's term for `std::int64_t`. Change: name the type ("from std::int64_t, the type of the products' integer coefficients") and add a fix after a semicolon ("define the missing operators"), or split construction from the operators, as S3 splits semiregular.

### C4. One value that does not convert to T gets the array message (could)

- Evidence: cases 28 and 33 (`std::complex<double>` into a `double` type, a one-blade type in 33): "give the coefficients one by one, not as one array or braced list", a cause the user did not commit. Change: when the single argument is not a range or array, "mv: the value does not convert to the scalar type".

### C5. `truncate`'s check in a constant evaluation, and its abort at run time (could; with `GALOOT_ASSERT`)

- Evidence: case 17: GCC's first error line names `galoot::core::detail::assertion_failed`, its message is in the echoed source line five lines below, and its context is 2,558 characters; Clang's first line is "constexpr variable 's' must be initialized by a constant expression". The run-time message names `mv.hpp:776`, which does not tell the user which of their calls failed. Both follow from `GALOOT_ASSERT`'s design. Possible later: a `std::source_location` default argument on `truncate` and the run-time report, so the abort names the caller.

### C6. The coefficient-exactness message, for when products are public (could; WU9)

- Evidence: `out/inexact-*.txt`: "the scalar type cannot hold one of this product's exact coefficients; use a scalar type that holds larger integers exactly", one error, first line, on both. Clang's clause is "due to requirement 'exact'"; a name such as `scalar_holds_every_coefficient_of_the_product` would read as the rule. No operation prefix: when a user writes `a * b` (WU9) the message should start with the product's name. For `double` there is rarely a larger scalar to take, so the fix could also say "or scale the metric".

### C7. Document the conversions in the header's opening comment (could)

- Evidence: the opening comment of `mv.hpp` is the only user documentation, and it shows construction only. Plan section 4's conversion snippet compiles and runs (`plan_snippet.cpp`). Add three lines: `Motor m{axis};`, `Line back = truncate<Line>(m);`, and that `Motor m = axis;` does not compile.

### C8. `truncate` with names written as text (could)

- Evidence: case 15, `truncate<"e41 e42 e43 e23 e31 e12">(q)`, the guess after learning `blades<rga3, "...">`: GCC 51 lines. A catch-all for a string target is possible with a `name_list` overload, if users are seen to try it.

### C9. Things that worked and are worth keeping

- Unqualified `truncate<Line>(q)` with POSIX `::truncate` in scope (30) and by ADL (31); `truncate<const Line&>` returning `Line`; the round trip (35); a short double-brace list rejected, not zero-filled (24); `volatile`, `std::complex`, `unsigned`, and `char` scalars handled as documented (49 to 52).

## 8. Before release (Hyrum's law)

Interface changes this reading suggests that would break users later, cheapest now: the meaning of `truncate` for exact scalars, and a projection beside it (S1); the literal texts of S2, S3, and S5 (they break only tests that match them, but users grep for messages); whether `truncate` and the constructors accept classes derived from `mv` (S4, with WU8a's F6); a scalar conversion and whether `std::is_constructible_v<Line<double>, Line<float>>` stays false (S7).

## 9. Knowledge base

Added on branch `kb/api-design-implicit-rejection` (worktree `knowledge-base-worktrees/api-design-implicit-rejection`), committed there (`1f01ebf`), not merged: the note `api-design/rejecting-implicit-conversions.md` (the measured costs of a non-explicit rejecting constructor, the echoed-line hint, copy-list double braces, and a check whose strictness depends on the scalar), the exercise `exercise-archive/api-design-implicit-rejection.md` (`cpp-api02b-2026-11-01`), and its line in `api-design/README.md`. The copy-initialization rule itself is in the implementer's unmerged note `cpp-language/lang-rejecting-constructors-and-kernel-results.md` (branch `kb/cpp-catch-alls-and-kernel-results`), which this note does not repeat.

## Agent cost

About 50 tool calls. Token counts are not visible to me. The plan estimated 80k to 120k tokens and 30 to 40 tool calls for this reading.
