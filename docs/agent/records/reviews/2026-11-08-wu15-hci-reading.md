# hci-expert reading: WU15 (plan 0022), `feature/core-format` at `1ee9a54`

- Reader: hci-expert, Claude Opus 5.5 (`claude-opus-5-5`), 2026-11-08. Agent work. Role: reviewer. This is the optional reading of plan 0022 section 8, done before the W5 blind review: the printed form and the new messages as a first user meets them. I am independent of the implementer and the test author, and this reading is advice only. A blind reviewer checks correctness separately.
- Read as: a C++23 user who knows geometric algebra and has not seen Galoot's internals.
- Subject: `include/galoot/core/format.hpp`; the WU15 changes to `mv.hpp` (the rejecting constructor and its comment), `linear.hpp` (the four sums with a number), and `blade_set.hpp` (`blades` and its two messages); CONVENTIONS 7.2; plan 0022 sections 1, 2, 3, 8, R1 to R9, and its outcome; decisions 0204 to 0207; `records/diagnostics/wu15-messages.md`; `records/diagnostics/misuse-catalogue.md`. I did not edit the branch.
- Work: branch `records/review-wu15-hci` from `1ee9a54`, worktree `.claude/worktrees/hci-wu15`. I wrote the programs and ran them in the session's scratch directory, not in the repository.
- Compilers: g++ 16.2.1 and clang++ 23.1.4 with libstdc++ 16, `-std=c++23 -I include`. The printing programs were linked and run with `-Wall -Wextra`, and the header comment's example also with `-Wpedantic -Wconversion -Wshadow -Werror`. The mistake cases were compiled with `-Wall -Wextra -fsyntax-only -fno-diagnostics-color`.
- Verdict: **nothing blocks the merge.**
  - Every library message I reached is one error, on the first error line, on both compilers. CONVENTIONS 7.2 matches what the formatter does in every point I checked. The header's examples print what they say.
  - S1 is a false message. A scalar whose formatter takes `std::format_context&` prints with `std::println` and is still told it "has no std::formatter".
  - S2 is about the model. The rga3 tag declares no names, so every rga3 line, plane, and motor prints by the fallback names (`-e14`, `-e134`), not the names its own header uses.
  - Three findings should be acted on, S2 by the owner. Eight could be.
- Job pool: running, 23 of 23 slots free before the first run and after the last. I neither started nor stopped it.

## Departures, skipped checks, and ambiguities (C20)

1. Programs and run rows are not committed. The brief limits my commits to this record. Section 3 describes each case well enough to rewrite it. I ran 133 compiles through `tools/timed --log` into a scratch log, rows `hci-wu15:*`, totaling 86 s, none longer than 1.6 s. They are not in `runs.tsv`.
2. I ran no CMake build and no CTest. The compile-fail tests, the catalogue scan, and the mutants belong to the authors and the blind reviewer.
3. S1's fix was prototyped in a scratch copy of `include/`, outside the repository, and not on the branch. It is a code proposal, and a cpp-expert should confirm it (section 4).
4. Task first was not pure: the brief ordered the plan and the records read before the programs.
5. Only GCC and Clang with libstdc++ were used. No person was observed, so what a first user tries is my judgment.
6. Knowledge base: the brief limits commits to this record, so I wrote no note. One candidate is listed at the end.
7. Each message rewording in section 4 changes a literal that `test/CMakeLists.txt` pins: `mv_other_scalar` at line 1171, `blades_not_an_algebra` at 384, `linear_number_plus` and `linear_number_minus` at 1590 and 1591, and `format_not_a_blade` at 2143. Each rewording therefore needs a test commit of its own, before the header change (W9). S1, S2, and the comment findings change no pinned literal.

## 1. Task first

1. Print a point and a line of the rga3 model. `std::println("{}", p)` needs `<galoot/core/format.hpp>`, which neither the README nor `rga3.hpp` mentions (S3). With it, the point prints as `{e1: 0, e2: 0, e3: 0, e4: 1}`, as I hoped. The line prints as `{-e14: 1, -e24: 2, -e34: -0, e23: 0, -e13: -0, e12: 0}`, but `objects.hpp` taught me e41, e42, e43, and e31 (S2).
2. Print with three decimals: `{:.3f}` worked on every coefficient. `{:>40}`, meant to pad the whole value, padded every coefficient instead. That is documented, but a user may not expect it (C6).
3. Print over my own exact rational. A formatter written as cppreference writes one, with `parse(std::format_parse_context&)` and `format(const R&, std::format_context&)`, prints the rational alone. Inside a multivector it gets "format: the scalar type has no std::formatter" (S1). Once the formatter's members were templates on the context, it printed `{e1: 1/2, e2: -3/4, e3: 0, e4: 1}`.
4. Give my own tag the names of Lengyel's pages. `static constexpr auto names = "e41 e42 e43 e23 e31 e12 e423 e431 e412 e321";` worked, and so did `const char*`. The misspelled member `name` was ignored silently, which no library can catch.
5. Make the mistakes the new messages exist for: `Motor<double>{float_line}`, `p + 1.0`, and `blades` on a tag that is not an algebra. Each gave the library's message on the first line (section 3).

## 2. The checklist (decision 0160), in brief

| Item | Verdict | Evidence |
| --- | --- | --- |
| Field's terms or full words | Met | `names` on the tag; the blade names follow CONVENTIONS 1.1 and 1.5 |
| One meaning; no std clash | Met | the form is the standard library's form for a map |
| Similar things look alike | Met | `format:`, `mv:`, `a + b:`, `blades:` prefixes; mistake, `;`, then the fix |
| Shortest form is correct | Partly | `std::println("{}", x)` needs an include that nothing points to (S3) |
| Lossy conversions named | Met | the scalar and algebra conversions are now rejected with a message, and the implicit form stays the compiler's error (case 3) |
| Compile-fail test per catalogued misuse, both compilers | Met (read, not run) | the catalogue's rows; `test/CMakeLists.txt` |
| One error, message on the first error line | Met | every library message of section 3, on both compilers |
| Operation first, mistake, fix | Mostly met | S1 (false for one formatter form); C1, C2, C4 (wording) |
| Context lines in the user's terms | Not met on GCC (known) | GCC's "In instantiation of" lines spell the masks, up to 1,078 characters for case 1; Clang's notes are shorter and name the operator |
| Distinct causes, distinct messages | Met | two `mv` messages, two `blades` messages, four `format` messages |
| Probes documented | Met | each new overload's `///` says what a requires-expression or `is_constructible` is told |
| Assumptions stated | Mostly met | width per coefficient, and a specification the scalar rejects, are left unsaid (C6) |
| Examples compile | Met | `format.hpp`'s head comment, both tags, both compilers, prints as written |
| Maintainer finds each behavior | Met | name choice in `detail::make_printed_names`; rejections in one `if constexpr` chain |

## 3. The programs

`print.cpp`, linked and run on both compilers, gave identical output on each:

| What was printed | Output |
| --- | --- |
| rga3 `Point<double>{0, 0, 0, 1}` | `{e1: 0, e2: 0, e3: 0, e4: 1}` |
| rga3 `Line{p ^ q}`, q = (1, 2, 0, 1) | `{-e14: 1, -e24: 2, -e34: -0, e23: 0, -e13: -0, e12: 0}` |
| rga3 `Line{q ^ p}` | `{-e14: -1, -e24: -2, -e34: -0, e23: 0, -e13: -0, e12: 0}` |
| rga3 `Plane{p ^ q ^ r}` | `{e234: 0, -e134: -0, e124: 1, -e123: -0}` |
| `translation(1.0, 2.0, 3.0)` | `{-e14: 0, -e24: 0, -e34: 0, e1234: 1, e23: 0.5, -e13: 1, e12: 1.5, 1: 0}` |
| `p ^ q` unconverted | `{e12: 0, e13: 0, e14: -1, e23: 0, e24: -2, e34: 0}` |
| `{:.3f}`, `{:>6}`, `{:+}`, `{:g}` on a line or motor | applied to each coefficient, as 7.2 says |
| `{:#x}` on `Point<std::int64_t>` | `{e1: 0xa, e2: 0xff, e3: 0x3, e4: 0x1}` |
| `Point<int>{1, -2, 3, 1}` | `{e1: 1, e2: -2, e3: 3, e4: 1}` |
| point over a rational (templated formatter) | `{e1: 1/2, e2: -3/4, e3: 0, e4: 1}` |
| empty layout `blade_set<0>{}` | `{}` |
| a tag naming e41 ... e321: line, plane | `{e41: 1, e42: 0, ...}`, `{e423: 1, e431: 0, e412: 0, e321: 0}` |
| a tag naming only `e41 e24` | `{e41: 1, -e24: 0, -e34: 0, ...}`: e24 is the other orientation of the layout's e42, so the fallback applies, as documented |
| the join of two points of the named tag | `{e12: 1, e13: 0, e14: 1, ...}`: a product's blades have orientation +1 |
| R(3,0,0) on `1 e12 e31 e23` | `{1: 1, e12: 2, -e13: 3, e23: 4}` |
| NaN, infinity, -0.0 | `nan`, `inf`, `-0`, as the scalar's formatter writes them |
| `std::vector<Point>` with `{}` and `{::.2f}` | `[{e1: 0, ...}, {e1: 1, ...}]`, and the nested specification applies |
| `{:>40}` on a point | every coefficient padded to 40 (C6) |

`fail.cpp` with `-DCASE=N`. Each cell gives the total lines, the lines holding `error:`, and the longest line of the whole diagnostic.

| # | What the user wrote | GCC | Clang | First error line (both compilers unless marked) |
| --- | --- | --- | --- | --- |
| 1 | `rga3::Motor<double>{float_line}` | 10 / 1 / 1078 | 9 / 1 / 442 | `mv: the source has another scalar type; a conversion between layouts keeps the scalar type, so convert the coefficients one by one` |
| 2 | `rga3::Line<double>(float_line)`, same layout | 10 / 1 | 9 / 1 | as case 1 (C1: no layout changes here) |
| 3 | `rga3::Line<double> l = float_line;` | 12 / 1 / 224 | 31 / 1 / 344 | the compiler's: GCC `conversion from 'mv<[...],[...],float>' to non-scalar type 'mv<[...],[...],double>'`; Clang `no viable conversion from 'const Line<float>' to 'Line<double>'` |
| 4 | `static_cast<Line<double>>(Line<int>{...})` | 10 / 1 | 9 / 1 | as case 1 |
| 24 | `Motor<double>{Line<double>{float_line}}` | 10 / 1 | 9 / 1 | as case 1: the inner conversion |
| B | another algebra's `float` line into `Line<double>` | 1 error | 1 error | `mv: the source is in another algebra; ...`: when both differ, the algebra is named, as the comment says |
| 5 to 8, 27 | `p + 1.0`, `1 + p`, `p - 2`, `p + 1.0f`, `p + 1.0 * 2` | 12 / 1 | 11 / 1 | `a + b: a multivector and a number have no sum; make the number a multivector on the blade 1 and add that` (`a - b:` for 7) |
| 10 | `(p ^ p) + 1.0` | 16 / 1 | 11 / 1 | as 5 |
| 9 | `r += 1.0` | 4 / 1 / 516 | 4 / 1 | `no match for 'operator+='`: the core has no compound assignment at all (outside the unit) |
| 11 | `blades<zero_tag, "e1">`, `value = metric<0>{}` | 13 / 1 | 12 / 1 | `blades: the algebra tag's metric has no generators, and an algebra has at least one; give the tag a metric such as rigid<3>()` |
| 12, 13, 16, 25 | `blades` on a tag with no `value`, an `int` value, `int` as the tag, a `static const` value that is not constexpr | 13 / 1 | 12 / 1 | `blades: the tag is not an algebra; an algebra tag holds its metric as static constexpr value, as in struct rga3 { static constexpr auto value = rigid<3>(); }` |
| 14 | `static constexpr auto value = core::rigid<3>;` (no parentheses) | 16 / 2 | 25 / 3 | the language's: the tag itself is ill formed (`returns address of immediate function`), before `blades` is reached |
| 15 | `mv<no_value, blades<no_value, "e1">, double>` | 34 / 3 | 21 / 2 | the `blades` message first, then `mv`'s constraint |
| 17 | `println` of a point over a scalar whose formatter takes `std::format_context&` | 33 / 1 / 922 | 35 / 1 / 465 | `format: the scalar type has no std::formatter; ...`, which is false (S1) |
| 18 | a tag with `names = "e0 e41"` | 33 / 1 | 35 / 1 | `format: an entry of the algebra tag's names is not a blade of the algebra; write 1, or e and distinct indices from 1 to n, as in e41` |
| 19 | a tag with `names = "e41 e14"` | 33 / 1 | 35 / 1 | `format: the algebra tag's names name one blade twice, perhaps in both orientations; name each blade once` |
| 20 | `println` of `mv<R(10,0,0), full<10>, double>` | 33 / 1 / 78,181 | 87 / 4 | GCC: the n-above-9 message, after a 78,181-character context line. Clang: `mv`'s own layout check hits the constexpr step limit at 1024 blades, before `format`. This is outside the unit: `format_wide_algebra` uses a one-blade layout |
| 21, 28 | `{:d}` on a double point | 33 / 2 / 1286 | 37 / 1 | the standard library's: `call to consteval function basic_format_string<...>`, then on GCC `__failed_to_parse_format_spec` (C6) |
| 22, 23 | `{:.2f}`, `{:.3}` on `Point<int>` | 36 / 2 | 43 / 1 | as 21 (C6) |

`head.cpp`: both examples of `format.hpp`'s head comment compile with `-Werror` on both compilers. They print `{-e14: 0, -e24: 0, -e34: 1, e23: 0, -e13: 0, e12: 0}` and `{e41: 0, e42: 0, e43: 1, e23: 0, e31: 0, e12: 0}`, as written.

My counts are one line below `wu15-messages.md`'s on GCC for case 1 (10 against 11), with the same error count. My source differs from the record's probe, so this is no discrepancy.

## 4. Findings

Severity for users: **blocking** (should stop the merge), **should** (fix in this unit, or decide before release), **could** (worth doing; no harm in waiting). None is blocking.

### S1. The scalar message is false for a formatter that takes `std::format_context&` (should; this unit; code, no literal change)

- Evidence: case 17. The scalar's formatter has `parse(std::format_parse_context&)` and `format(const num&, std::format_context&) const`. That is the form of cppreference's `std::formatter` example, and the form most users copy. `std::println("{}", num{3})` works on both compilers, and `static_assert(!std::formattable<num, char>)` holds on both (`ctx.cpp`). The standard's `formattable` tries the formatter with an unspecified iterator's context (plan 0022 departure 6 found the same for the library's own formatter). `format.hpp` decides by `std::formattable<T, char>`, so this user is told the type "has no std::formatter", which is untrue. The `///` "Requires: ... a std::formatter<T, char> for the scalar type" is met, yet the call is rejected.
- Option (a), recommended: decide by whether the specialization is enabled, which is what "has a formatter" means. A disabled `std::formatter` specialization is not default constructible, copyable, or movable ([format.formatter.spec]). A scratch copy with `concept has_formatter = std::semiregular<std::formatter<T, char>>;` in place of the three uses of `std::formatter<T, char>` being formattable printed a point over `num`, `{:.2f}` on a double point, and a `std::vector` of points over `num` on both compilers. All three `format_scalar_without_formatter*_fail.cpp` sources still gave one error with the message on both compilers. The `///` sentence on `std::formattable` stays true. Cost: a change in `include/`, so a delta review; one accept test with a `std::format_context&` formatter; and a cpp-expert check that `semiregular` is the right test for an enabled specialization.
- Option (b): keep the check and make the message true. "format: the scalar type has no std::formatter that std::formattable accepts; give it one, with parse and format templated on the context, as double has." Cost: the literal at `test/CMakeLists.txt:2141` (W9), and the rejection of a formatter that works everywhere else stays.

### S2. The rga3 tag declares no names, so the model's objects print by names its documentation does not use (should; owner: this unit or the next model unit, before M4's gate)

- Evidence: section 3, rows 2 to 5. `objects.hpp` documents the line on e41, e42, e43, e23, e31, e12, and the plane on e423, e431, e412, e321. The printed text says `-e14`, `-e24`, `-e34`, `-e13`, and `-e134`, `-e123`. With the stored sign, a line from q to p prints `-e14: -1`, and a coordinate that is zero prints `-e13: -0`. Both are correct readings of fallback E, and both make a user stop and work out the signs. Decision 0204's consequence anticipates the fix: "The rga3 model can declare CONVENTIONS 1.6's names without a core change."
- Change (model only, no core diff, so M4's zero-core-diff exit is unaffected): in `rga3::algebra`, `static constexpr auto names = "1 e1 e2 e3 e4 e23 e31 e12 e41 e42 e43 e423 e431 e412 e321 e1234";`, the list `format_test.cpp`'s `rga3_named` already pins. Add an rga3 test that each of the five types prints by those names, and a sentence in the header comment.
- Why before release: printed text ends up in logs and in users' golden files. Renaming `-e14` to `e41` after release breaks every stored output.

### S3. Nothing a user reads says how to print a value (should; README, this unit or with S2)

- Evidence: `README.md` reads coefficients one by one with `get` (line 91) and never mentions `format.hpp`. `core.hpp` and `rga3.hpp` do not include it, by design. Without it, `std::println("{}", p)` gives libstdc++'s nine errors (the WU16 reading, case 25). Printing a value is usually a user's first debugging step.
- Change, after the second example: "To print a value, include `<galoot/core/format.hpp>`: `std::println(\"{}\", p)` prints `{e1: 1, e2: 3, e3: 3, e4: 1}`, each coefficient on its blade's name, and a specification such as `{:.3f}` applies to each coefficient. `core.hpp` leaves it out because `<format>` is costly to compile." Check the text against the README test (W12). Once S2 is in, a line example also shows the model's names.

### C1. The scalar conversion message's fix is vague, and it mentions layouts when none change (could)

- Evidence: cases 1, 2, 4. `Line<double>(float_line)` keeps the layout, yet the message explains "a conversion between layouts". "Convert the coefficients one by one" does not say what to build from them. An `int` to `double` conversion, which loses nothing, is likely the commonest case.
- Before: `mv: the source has another scalar type; a conversion between layouts keeps the scalar type, so convert the coefficients one by one`
- After: `mv: the source has another scalar type, and a conversion keeps the scalar type; convert each coefficient to the target's scalar type and build the target from those values`. The literal is at `test/CMakeLists.txt:1171`. Revisit if the planned scalar-conversion unit adds a named function; the message should then name it.

### C2. `blades`' tag message is telegraphic in one phrase (could)

- Evidence: cases 12, 13, 16, 25. The phrase "holds its metric as static constexpr value" reads as a word left out.
- After: `blades: the tag is not an algebra; an algebra tag holds its metric in a static constexpr member named value, as in struct rga3 { static constexpr auto value = rigid<3>(); }`. The literal is at line 384. Case 25 (`static const`, not constexpr) is helped by the same text.

### C3. The number-sum fix's "the blade 1" (could; low)

- Evidence: cases 5 to 8. "The blade 1" is the library's term, and a reader may take "1" for a number. After: "... make the number a multivector on the scalar blade 1 and add that" (and "subtract that"). The literals are at lines 1590 and 1591. The message cannot show the spelling `mv<Alg, blades<Alg, "1">, T>{s}`, because quote characters are excluded, so the `///` comment of the four overloads could give it.

### C4. `format`'s name message uses n without saying what it is (could)

- Evidence: case 18. `blades`' own message says "an index above n, the number of generators". After: `... write 1, or e and distinct indices from 1 to n, the number of generators, as in e41`. The literal is at line 2143. The likely case, `e0` from a PGA text, is already pointed at the fix by "from 1".

### C5. A `names` member that is not one text is ignored silently (could; comment)

- Evidence: decision 0204 item 4 accepts that a `names` that does not convert to `std::string_view` is not read. A user who writes an array, such as `static constexpr const char* names[] = {"e41", ...}`, gets fallback names with no hint. This is derived from the concept's text and was not compiled. Add to the `///` comment: "A names member that is not one text, such as an array of names, is not read." A message would change a pinned reading (decision 0204), so it is left to the owner.

### C6. Two behaviors of the specification are left unsaid (could; comment)

- Width and fill apply to each coefficient, so `{:>40}` pads every value and not the whole text. A specification the scalar's formatter rejects, such as `{:.2f}` for `int` (cases 21 to 23), is the standard library's error: two errors on GCC with a 1,277-character first line, one on Clang, neither naming the cause in Galoot's terms.
- Add to the `///` comment: "Width and fill apply to each coefficient; to pad the whole text, format it to a string first. A specification the scalar's formatter rejects, such as {:.2f} for int, is reported by the standard library."

### C7. CONVENTIONS 7.2 cites 1.1 for a name that 1.1 does not give (could; owner, C10)

- 7.2 says "otherwise by its canonical name (1.1)". 1.1 defines the canonical blade of a mask by its indices but names no blade for the empty mask. The scalar prints as `1` (from 1.5 and `canonical_name`), and as `-1` with orientation −1 (decision 0204 item 4). Proposed: "by its canonical name (1.1; the scalar is 1, 1.5)". Wording only. Every other clause of 7.2 matches the behavior in section 3: the order, the zeros, the declared names by orientation, the `-` fallback, `{}`, and the specification on each coefficient.

### C8. Negative zeros (could; comment; carried from the WU16 reading, C6)

- `-e34: -0` and `-e13: -0` appear in most printed lines and planes built by `^`. Under S2's names they become `e43: -0`. Add to the `///`: "Coefficients print as the scalar's formatter writes them, so a floating -0.0 prints as -0."

### Outside the unit (noted, no change asked here)

- The core has no `operator+=` or `operator-=` (case 9). `r += q` gives the compiler's error with a 516-character first line on GCC. Before release, either add them or give a catch-all message.
- `mv<R(10,0,0), full<10>, double>` fails on Clang at `mv.hpp:154`, where the constexpr step limit is reached in `masks_distinct`. This happens without `format` and is unrelated to WU15. On GCC, formatting it prints a 78,181-character context line before the message.

### What worked and is worth keeping

- One error, on the first line, on both compilers, for every library message I reached. The catch-alls take 9 to 13 lines, against 114 and 47 before (`wu15-messages.md`).
- The map form reads at once. `1: 0` for the scalar is consistent with `get<"1">`. The nested range specification `{::.2f}` works for a `std::vector` of points with nothing added.
- `Motor<double>{other_algebra_float_line}` names the algebra, the more basic mistake, as the comment says.
- Splitting `blades`' message in two (decision 0207 item 4) pays off: case 11's "has no generators" is exact where "not an algebra" would mislead.
- The head comment of `format.hpp` shows the fallback and the declared names side by side, which explains E better than any sentence could.

## 5. Before release (Hyrum's law)

These choices cost more after release:
- S2, the printed names of the model's objects, which users will store;
- S1, what counts as a scalar with a formatter, since a widening later is safe but a narrowing is not;
- the literal texts of C1 to C4, which break only the tests that pin them, though users search for messages.

## Open items for the owner

1. **S2, where the rga3 names land.**
   - Options: (a) in this unit, a model change of one line plus a test, before the merge; (b) in the next model unit, before M4's gate.
   - Tradeoff: (a) widens WU15 beyond the core and adds an rga3 test to its review. (b) leaves a merged `develop` printing `-e14` for model lines until then.
   - Recommendation: (a), since the cost is one line and a test, and M4's gate needs the model's printed form settled.
2. **C7, a wording change to CONVENTIONS 7.2** (C10, owner ratifies). Recommendation: take it with the next CONVENTIONS version; there is no urgency.
3. **C5, whether a non-text `names` member gets a message.** It would reverse an accepted reading of decision 0204. Recommendation: no message now; the comment line only.

Not owner items: S1 (recommend option a, with a delta review), S3, and C1 to C4, C6, C8. Each of C1 to C4 needs its own test commit (W9).

## Knowledge base

Not written (departure 6). Candidate: a `std::formatter` written with `std::format_context&` parameters works with `std::format` but fails `std::formattable`. A wrapper that checks "has a formatter" should test whether the specialization is enabled (`std::semiregular<std::formatter<T, char>>`), not `std::formattable`.

## Agent cost

About 30 tool calls. Token counts are not visible to me.
