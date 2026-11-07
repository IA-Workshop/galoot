# hci-expert reading: WU16 (plan 0023), `feature/rga3` at `da49bca`

- Reader: hci-expert, Claude Opus 5.5 (`claude-opus-5-5`), 2026-11-07. Agent work. Role: reviewer, reading the rga3 model as a first user meets it (plan 0023 section 8, the reading before the W5 review). Independent of the implementer and the test author; advice only. A blind reviewer checks correctness separately.
- Read as: a user who knows C++23, and a second user who also knows projective geometric algebra, neither of whom has seen Galoot's internals.
- Subject: `README.md`, `include/galoot/rga3.hpp`, `include/galoot/rga3/objects.hpp`, `include/galoot/rga3/motions.hpp` at `da49bca`; `docs/agent/records/diagnostics/wu16-messages.md`; plan 0023 sections 2, 3, 8, R2, R6; decisions 0196 to 0201. `feature/rga3` moved to `613cab0` during the reading (lint commits by another agent); `git diff da49bca 613cab0 -- include README.md` is empty, so the reading holds at `613cab0`. The branch was not edited.
- Work: branch `records/review-wu16-hci` from `da49bca`, worktree `.claude/worktrees/hci-wu16`. Programs were written and run in the session's scratch directory, not in the repository.
- Compilers: g++ 16.2.1 and clang++ 23.1.4 with libstdc++ 16, `-std=c++23 -I include`; the README examples with `-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror`, the mistake cases with `-Wall -Wextra -fsyntax-only -fno-diagnostics-color` (a second batch adds `-Wconversion` and links).
- Verdict: **nothing blocks the merge.** The five messages of `motions.hpp` each come as one error on the first error line on both compilers, are plain ASCII, start with the operation, and give a fix. The README's two examples compile and run on both compilers and print what they say. The serious finding is not a message: `rotation` about a line whose direction is not of length 1 gives a wrong turn silently, and the README's idiom `Line{p ^ q}` produces such lines for most pairs of points (S1). Seven findings should be acted on, three of them owner questions before release (S1, S2, S3); seven could be.
- Job pool: running, 23 of 23 slots free before the first run and after the last; not started or stopped.

## Departures, skipped checks, and ambiguities (C20)

1. File name. The brief named `reviews/wu16-hci-reading.md`; W6 asks for a sortable prefix and every earlier review has a date, so this file is `2026-11-07-wu16-hci-reading.md`.
2. Programs and run rows are not committed. The brief says to commit only this record and to keep scratch sources out of the repository; the WU8b reading committed its programs beside its record. The cases are described in section 3 with enough text to rewrite them. The `tools/timed` rows (selections `hci-wu16:*`, six runs, 0.2 s to 32 s) are in a scratch log, not in `docs/agent/records/performance/runs.tsv`.
3. No CMake build or CTest run; the compile-fail tests and the README check are the authors' and the blind reviewer's. I compiled the README blocks myself, extracted by hand, not through `tools/readme_examples.py`.
4. Task first was not pure: I read the plan and the diagnostics record before writing programs, as the brief ordered.
5. Only GCC and Clang with libstdc++. No person was observed; what a first user tries is my judgment.
6. Knowledge base: the brief limits commits to this record, so no note was written; two candidates are listed at the end.
7. The message proposals in section 4 change literals that `test/CMakeLists.txt:2177-2181` pins, so each needs a test commit of its own (W9) before the header change.

## 1. Task first

Five tasks, written from the README alone:

1. Make points and the line through two of them: `Line{p ^ q}`. Worked; the README's comment says why the braces are needed.
2. Turn a point about an axis I chose, by an angle: `rotation(Line{p ^ q}, theta)`. Compiled, and gave a wrong result for an axis through (0, 0, 0) and (0, 0, 2) (S1). Nothing in the README says the axis must have length 1; the header comment does.
3. Chain two motions and keep the result as a `Motor`: `const Motor m = compose(a, b);`. Did not compile, with a mask-heavy message (S2). The README writes `Motor motion{...}` with braces but explains braces only for the join.
4. Move a point: `transform(motor, point)`, in the order the sandwich Q ⟇ x ⟇ antireverse(Q) is written. Compiled silently and returned a motor (S3).
5. A motion over `int`, which the README says to build "from its coefficients": I had to read `motor_layout`'s comment to learn the order of the eight values and that they hold half the distance (S4).

## 2. The checklist (decision 0160), in brief

| Item | Verdict | Evidence |
| --- | --- | --- |
| Field's terms or full words | Met | `Point`, `Line`, `Plane`, `Motor`, `Flector` are Lengyel's; `*_layout` is the core's word; `rotation`, `translation` full words (section 5) |
| One meaning; no std clash | Met | `transform` and `std::ranges::transform` is stated in the README |
| Similar things look alike | Met | All five messages: operation, mistake, `;`, fix. Both constructors return `Motor<T>` |
| Shortest form is correct | Not met for `rotation` | S1; and `Motor m = compose(...)` fails (S2) |
| Default zero | Met | the aliases' comments; the core |
| Lossy conversions named | Met (core) | case 4 gives `truncate`'s literal |
| No hidden default changes a result | Met | no default scalar; `Point<>` is the compiler's error (case 10) |
| Narrowing rejected | Partly (core, before this unit) | `Point<int>{0.5, ...}` is an error; `Motor<float>{0.1, ...}` only warns under `-Wconversion` (C4) |
| Compile-fail test per catalogued misuse, both compilers | Met | `test/CMakeLists.txt:2171-2208`, `ONE_ERROR`, `detail::` and `apply:` forbidden |
| One error, message on the first error line | Met | every case of section 3 whose first error is a library message, both compilers |
| Operation first, mistake in the algebra's terms, fix | Mostly met | S5 (a claim that is false for `int`), S6 (masks on Clang's first line), S7 (the likely cause, degrees, unnamed) |
| Context lines in the user's terms | Not met on GCC (known, plan 0023 R3) | GCC's "required from" lines spell `mv<...blade_set...>`; Clang names `galoot::rga3::rotation<int, int>` |
| Distinct causes, distinct messages | Met | the five catch-alls split by cause |
| Probes documented | Met | each catch-all's comment says what a requires-expression is told |
| Assumptions stated | Met in `///`, not in the README | `rotation`'s unitized axis (S1); half angles in `motor_layout` |
| `auto r = f(...)` safe | Met | values throughout |
| Examples compile and are the idiom to copy | Compile: met. Idiom: S1, S2 | README blocks, both compilers |
| Maintainer finds each behavior | Met | one header per concern; each catch-all beside its function |

## 3. The programs

`user.cpp` with `-DCASE=N` (35 cases, 70 compiles, 32 s) and `user2.cpp` (10 cases, linked and run where they compile, 11 s). Lines and longest line of the whole diagnostic; "first line" is the first `error:` line.

| # | What the user wrote | GCC lines / longest | Clang lines / longest | First error line (both unless marked) |
| --- | --- | --- | --- | --- |
| 1 | `Motor{1, 2, 3}` | 13 / 644 | 15 / 354 | `mv: give one value for each component, in the order the layout lists them`, then `3 == 8` on both |
| 2 | `Motor{1, ..., 9}` | 13 / 674 | 15 / 384 | as 1, `9 == 8` |
| 3 | `Point{1, 2, 3}` (no w) | 13 / 493 | 15 / 315 | as 1, `3 == 4` |
| 4 | `Line{turn}` (lossy) | 13 / 1028 | 12 / 411 | `mv: the conversion drops blades of its input; it is lossy, so request it with truncate<Type>(x)` |
| 5 | `const Line r = origin ^ up;` | 12 / 916 | 28 / 439 | the compiler's: `conversion from 'mv<[...],blade_set<6>{...masks...}' to non-scalar type ...` |
| 6 | `get<"e5">(point)` | 13 / 490 | 12 / 353 | `get: the name has an index above n, the number of generators` |
| 7 | `get<"e14">(line)` | 13 / 541 | 12 / 376 | `get: the type stores this blade with the other orientation; use the name its layout gives it and negate the value` |
| 8 | `get<"e12">(point)` | 13 / 491 | 12 / 354 | `get: the type has no blade of this name, in either orientation; its layout does not hold it` |
| 9 | `rotation(Line<int>{...}, 1)` | 21 / 1015 | 15 / 242 | message 1 of R6 (S5) |
| 10 | `Point<> r{...}` | 13 / 452, 2 errors | 9 / 183 | GCC `wrong number of template arguments (0, should be 1)`; Clang `too few template arguments for alias template 'Point'` |
| 11 | `rga3::Point r{0.0, 0.0, 0.0, 1.0}` (CTAD) | 50 / 905, 2 errors | 20 / 617 | GCC `class template argument deduction failed`; Clang `no viable constructor or deduction guide`, with notes naming `detail::` (C2) |
| 12 | `translation(&x, &y, &z)` | 95 / 329 | 38 / 176 | `no matching function for call to 'translation(double*, double*, double*)'` |
| 13 | `translation(1.0, 0, 2)` | 10 / 645 | 9 / 269 | message 5, with `'double' is not the same as 'int'` (GCC note) or `std::is_same_v<double, int>` (Clang) |
| 14 | `translation(1.0f, 2.0, 3.0)` | 10 / 649 | 9 / 271 | message 5, float against double |
| 15 | `translation(1, 2, 3)` | 10 / 600 | 9 / 229 | message 4 |
| 16 | `translation(1.0, 2.0, 3.0)` unqualified | 10 / 217 | 8 / 215 | `'translation' was not declared in this scope; did you mean 'galoot::rga3::translation'?` (Clang the same in its words) |
| 17 | `translation(std::array{1.0, 2.0, 3.0})` | 22 / 243 | 13 / 176 | `no matching function for call to 'translation(std::array<double, 3>)'`; Clang "requires 3 arguments, but 1 was provided" |
| 18 | `translation(point)` | 22 / 243 | 13 / 176 | as 17 |
| 19 | `rotation(z_axis, 90)` | 10 / 1024 | 9 / 277 | message 2, `int` against `double` (S7) |
| 20 | `rotation(pi / 2, z_axis)` | 56 / 693 | 22 / 173 | `no matching function for call to 'rotation(double, const Line&)'`; Clang "could not match 'Line<T>' (aka 'core::mv<algebra, line_layout, T>')" |
| 21 | `rotation(origin ^ up, pi / 2)` | 10 / 1083 | 9 / 462 | message 3; Clang's first line is 462 characters of masks (S6) |
| 22 | `rotation(z_axis)` | 27 / 225 | 16 / 173 | `no matching function for call to 'rotation(const Line&)'` |
| 23 | `rotation(Line<float>{...}, pi / 2)` | 10 / 1024 | 9 / 279 | message 2, `double` against `float` |
| 24 | `transform(turn, origin)` (swapped) | compiles | compiles | (S3) |
| 25 | `std::println("{}", origin)` | 189 / 934, 9 errors | 76 / 474, 4 errors | `std::formatter must be specialized for each type being formatted` (C5) |
| 26 | `const Motor r = compose(turn, turn);` | 12 / 1062 | 28 / 536 | the compiler's, as 5 (S2) |
| 27 | `Motor<float>{0.0, ..., 0.5, ...}` (exact doubles) | compiles | compiles | |
| 28 | `rotation(Line<std::int64_t>{...}, pi / 2)` | 21 / 1033 | 15 / 242 | message 1 (the scalar is reported before the angle) |
| 29 | `translation(&x, 2.0, 3.0)` | 60 / 327 | 22 / 176 | `no matching function`; Clang "deduced conflicting types ... ('double *' vs. 'double')" |
| 30 | `rotation(0.0, 0.0, 1.0, pi / 2)` | 27 / 225 | 16 / 173 | `no matching function`, "requires 2 arguments, but 4 were provided" |
| 31 | `const Line r = transform(z_axis, turn);` | compiles | compiles | transform keeps x's layout, as documented |
| 32 | `const Plane r = origin ^ up ^ p;` | 12 / 772 | 28 / 440 | the compiler's, as 5 |
| 33 | `auto l = origin ^ up; rotation(l, pi / 2)` | 10 / 1083 | 9 / 462 | message 3 |
| 34 | `translation(1, std::int64_t{2}, 3)` | 10 / 644 | 9 / 267 | message 5, whose fix suggests double; correct, though the user may have wanted integers (message 4 would follow) |
| u2-1 | `get<"e0">(point)` (the null generator's usual name in PGA) | 13 | 12 | `get: a name is 1 or e followed by generator indices, as in e41` (S4, C3) |
| u2-2 | `Motor<float>{0.1, ...}` | warning only | warning only | `-Wfloat-conversion`, `-Wimplicit-float-conversion` (C4) |
| u2-3 | `Point<int>{0.5, 0, 0, 1}` | 1 error | 1 error | the compiler's narrowing error |
| u2-4 | `auto l = p ^ q; get<"e41">(l)` | 13 | 12 | `get: the type stores this blade with the other orientation; its layout names each blade by its indices in increasing order, as e12 for e21; use that name and negate the value` |
| u2-5 | `std::println("{}", p.coefficients)` | runs | runs | `[0, 0, 0, 1]` |
| u2-7 | `auto m = translation(1.0, 0.0, 0.0); auto r = transform(m, origin);` | runs | runs | prints `[-0, -0, -0, -1, 0.5, 0, 0, 0]`, a motor (S3) |
| u2-8 | `get<"e0123">(motor)` | 13 | 12 | as u2-1 |

`unit.cpp`: `rotation(Line{P{0, 0, 0, 1} ^ P{0, 0, 2, 1}}, pi / 2)` moves (1, 0, 0, 1) to `[-1.5, 2, 0, 2.5]`, the point (-0.6, 0.8, 0), a turn of about 127 degrees, where a quarter turn gives (0, 1, 0) (S1).

`check.cpp`: the head comments of `objects.hpp` and `motions.hpp` compile and run on both compilers; `Line l{p ^ q}` for the x axis prints `[1, -0, -0, 0, -0, 0]` (C6). The integer motors proposed in S4 are checked there by `static_assert` on both compilers.

## 4. Findings

Severity for users: **blocking** (should stop the merge), **should** (fix in this unit, or decide before release), **could** (worth doing; no harm in waiting). None is blocking.

### S1. `rotation` about a line of the README's idiom turns by the wrong angle, silently (should; owner, before release)

- Evidence: `unit.cpp` above. `motions.hpp:39-44` states that l is assumed unitized and that a violation "scales or distorts what it moves"; the README says nothing, and its example works only because `origin ^ up` happens to have a direction of length 1. `Line{p ^ q}` has direction q - p, so almost every axis a user builds from two points is not unitized, and the model has no `unitize` (out of v1, plan 0023 R2), so the user must divide six coefficients by a norm they compute by hand.
- Options: (a) `rotation` divides the direction and the moment by the length of the direction, one `std::sqrt` and a division per coefficient. T is already a floating type there. For an axis-aligned unit line the computed length is exactly 1, so the result is unchanged bit for bit; for another unit line it can differ in the last bit, so R11's bit-for-bit test would need its expected value computed the same way (not checked against the test's lines). The comment then says "l of any positive length", and the assumption left is that l is a line (a join always is). Cost: a square root per call, and a comment change to C8's statement. (b) Keep the function and state the assumption in the README, next to the example: "rotation assumes the axis has a direction of length 1; origin ^ up has direction (0, 0, 1). For other points, divide the line's six coefficients by the length of q - p." (c) Both a and a named `unitize` later.
- Recommendation: (a), before release; the call is a run-time function with sine and cosine already, and an axis built from two points is the common case. If the owner prefers (b), make it now, since the README is the first thing copied.

### S2. `const Motor m = compose(a, b);` does not compile, and the README does not say why (should; README now, owner on the core later)

- Evidence: case 26: GCC 1,062-character line of masks, Clang 28 lines; `compose` of two `Motor`s returns the canonical layout (`blade_set<8>{0, 3, 5, 9, 6, 10, 12, 15}`), not `motor_layout`, so copy-initialization is the compiler's error. Joins and meets do the same (cases 5, 32). The README explains braces for the join only (line 69-72) and writes `const Motor motion{core::compose(move, turn)};` (line 78) without comment.
- Change, this unit: README, after "compose(second, first) chains them into one motor.": "Like the join, its result has a layout of its own, so it converts to a Motor with braces; `const Motor motion = core::compose(move, turn);` does not compile."
- For the owner, before release: whether `compose` of two operands of one layout returns that layout when the result fits it (a motor of two motors, a flector of a motor and a flector on `flector_layout`). Gain: the shortest form compiles. Cost: a core change outside this unit, and a return type that depends on the operands' layouts. Recommendation: a later core unit, before release.

### S3. `transform(motor, point)` compiles and returns a motor (should; documentation, this unit)

- Evidence: cases 24 and u2-7. The sandwich is written Q ⟇ x ⟇ antireverse(Q), motor first, in `objects.hpp:65`, in `transform.hpp`, and on Lengyel's pages, and many libraries put the motor first; `transform(x, Q)` puts it second. A point is a valid odd versor, so the core cannot reject the swap. With `const Point p = ...` the user gets a lossy-conversion error that does not point at the order; with `auto` the result is a wrong value.
- Change: README line 52: "and `transform(x, motor)` moves the object x by the result (the object comes first)". `objects.hpp:65`: "transform(x, Q), with the object first, moves x to Q ⟇ x ⟇ antireverse(Q)".

### S4. README for a reader who knows C++ but not geometric algebra (should; this unit)

- Evidence: the README uses blade, generator, metric, weight, join, meet, wedge, antiwedge, motor, flector, layout, and R(3, 0, 1) without saying what they are; the second example's result (-2, -2, 0, -2) is "at (1, 1, 0)" only if the reader divides by w, which the README does not say; a reader who knows PGA expects e0 for the projective generator, and `get<"e0">` gives only "a name is 1 or e followed by generator indices" (u2-1); "build such a motor from its coefficients" (line 121-122) gives neither the order nor the halving.
- Change: after "Points are written as (x, y, z, w)": "as in homogeneous coordinates, so (2, 4, 6, 2) is also the point (1, 2, 3). The blades are named by their generators: e1, e2, e3 are the axes and e4 is the projective one, which many texts call e0." After line 122: "A motor's eight coefficients follow `motor_layout` and hold half of each distance and angle: `Motor<int>{0, 0, 0, 1, 1, 2, 3, 0}` moves points by (2, 4, 6), and `Motor<int>{0, 0, 1, 1, 0, 0, 0, 0}` is a quarter turn about the z axis that also doubles a point's weight." (Both checked on both compilers in `check.cpp`.) One sentence for motor and flector: "A motor is a rotation and translation together; a flector is a motion with a reflection in it."

### S5. `rotation`'s scalar message says `int` lacks `std::sin` (should; cheap, this unit)

- Evidence: cases 9, 28. `<cmath>` has `std::sin(int)`, returning double, so "std::sin and std::cos, which this scalar type lacks" is untrue for the most likely scalar, and a C++ reader stops at it. The fix "the line times sin(theta/2) and cos(theta/2) on e1234" does not say the user can scale an exact rotor (S4's quarter turn).
- Before: `rotation: the angle needs std::sin and std::cos, which this scalar type lacks; build the Motor from its coefficients, the line times sin(theta/2) and cos(theta/2) on e1234`
- After: `rotation: sin and cos of the angle do not give values of this scalar type, which is not a floating type; use Line<double> or Line<float>, or build the Motor from its coefficients, the line times sin(theta/2) and cos(theta/2) on e1234`

### S6. The axis message's first line on Clang is 462 characters of masks (should; cheap, this unit)

- Evidence: cases 21, 33, the diagnostics record: Clang prints `due to requirement 'std::is_same_v<galoot::core::mv<..., blade_set<6>{{{{3, 1}, ...}}}, double>, ...>'`, and GCC adds a note of the same types. The two types help nobody: the message already says what the axis must be. A scratch copy with `static_assert(false, ...)`, the form `mv.hpp:616` already uses, gave one error, a first line of about 150 characters with no masks on either compiler, and no type note on GCC. The fix "convert a join" also does not fit a `Point` axis, the case of `rotation_axis_point_fail.cpp`.
- Before: `static_assert(std::is_same_v<X, Line<typename X::scalar_type>>, "rotation: the axis must be an rga3 Line; convert a join such as p ^ q with Line<T>{p ^ q}");`
- After: `static_assert(false, "rotation: the axis is not an rga3 Line; give a Line<T>, and convert a join such as p ^ q with Line<T>{p ^ q}");` (tried on both compilers). Keep the dependent conditions of messages 2 and 5, where the printed types are the help.

### S7. The angle message does not name the likely cause, degrees (should; cheap, this unit)

- Evidence: case 19. `rotation(z_axis, 90)` is how the message is most often reached; the fix "as 0.5 over double" leaves the user with `90.0`, which then compiles and turns by 90 radians.
- Before: `...the second of the two types shown; convert it to that type first, as 0.5 over double or 0.5f over float`
- After: `rotation: the angle is not of the line's scalar type, the second of the two types shown; give it in radians as that type, as std::numbers::pi / 2 over double or std::numbers::pi_v<float> / 2 over float` (tried on both compilers: one error, first line, type note unchanged).

### C1. The integer translation message names d, which the user did not write (could)

- Evidence: case 15. Before: `translation: halving d would truncate in an integer scalar type; use a floating or exact rational scalar type`. After: `translation: halving dx, dy, and dz would truncate in an integer scalar type; use a floating or exact rational scalar type, or build the Motor from its coefficients, which hold half of each`.

### C2. Class template argument deduction on the aliases (could)

- Evidence: case 11, `rga3::Point r{0.0, 0.0, 0.0, 1.0}`: GCC 50 lines and two errors, Clang notes that spell `detail::multivector_or_derived` and the core's deduction guides. Nothing can deduce the scalar without a core change. Change: one README sentence, "The scalar type is always written: `Point<double>`, never `Point`."

### C3. Core messages the model's user meets (could; for WU15 or a later core unit, no change here)

- `mv:` prefixes the count and conversion messages where the user wrote `Motor` or `Line` (cases 1, 4); Clang shows the alias in its context, GCC does not (known, plan 0023 R3). `get: the name has an index above n` does not give n (case 6: "above 4"). `e0` and `e0123`, the usual PGA spellings, get the general name rule (u2-1, u2-8); "generators are numbered from 1" would point to e4.

### C4. `Motor<float>{0.1, ...}` is accepted with a warning only (could; core, before this unit)

- Evidence: u2-2; `Point<int>{0.5, ...}` is an error (u2-3). The checklist's "a value cannot be made ... with a narrowing conversion" is met for integers, not for double to float. Plausibly a recorded choice of the core; not checked.

### C5. Printing a value (could; WU15)

- Evidence: case 25, nine errors on GCC; `p.coefficients` prints (u2-5). Until `format.hpp` exists, one README line: "`std::println(\"{}\", p.coefficients)` prints the coefficients in layout order."

### C6. Negative zeros in printed coefficients (could)

- Evidence: `check.cpp`, the head comment of `objects.hpp`: the x axis prints `[1, -0, -0, 0, -0, 0]`. The re-signing of e14 to e41 negates zeros. Harmless under `==`; worth a sentence in `format.hpp`'s documentation when it lands.

### C7. The head comment of `motions.hpp` writes the angle as a literal (could)

- `motions.hpp:13`: `rotation(z_axis, 1.5707963267948966)`; the README writes `std::numbers::pi / 2`. Use the README's form, which also shows the user where to get pi in the line's type.

### What worked and is worth keeping

- All five messages are ASCII (theta, not θ; e1234, not 𝟙), which reads in every terminal and log, against R6's draft.
- An unqualified `translation` gets "did you mean 'galoot::rga3::translation'?" from both compilers (case 16), so the ADL asymmetry the README explains is also caught at the call.
- Swapped or missing arguments of `rotation` and `translation` (cases 17, 18, 20, 22, 29, 30) get short, clear compiler messages; Clang's "could not match 'Line<T>' (aka 'core::mv<algebra, line_layout, T>')" names the user's type.
- The count message prints `3 == 8` on both compilers, and `Point{1, 2, 3}`, the most likely point mistake, is caught (case 3).
- The README's ADL paragraph and its `std::ranges::transform` warning, and both examples' expected values, which are correct.

## 5. Names

`algebra`, `point_layout` to `flector_layout`, and `Point` to `Flector` need no change: Lengyel's object names, the core's word "layout", and a tag that diagnostics print as `galoot::rga3::algebra`, which reads well. `rotation` and `translation` are full words for what the caller asks for; the field's names for the results, rotor and translator, are what the comments call them, and that split (the function names the motion, the comment names the object) is clear. The owner ruled the names (decision 0196, O4); I see no reason to reopen them.

## 6. Before release (Hyrum's law)

Interface choices this reading touches that cost more after release: whether `rotation` normalizes its axis (S1); whether `compose`, the join, and the meet return a model type when the operands share one (S2); the literal texts of S5 to S7 and C1 (they break only the tests that pin them, but users search for messages).

## Knowledge base

Not written (departure 6). Candidates: an angle-axis constructor whose example uses a unit axis hides the unit-length assumption from everyone who copies it; a catch-all whose `static_assert` condition is a type comparison makes Clang print both types on the first line, so use `static_assert(false, ...)` where the types are not the help.

## Agent cost

About 30 tool calls. Token counts are not visible to me.
