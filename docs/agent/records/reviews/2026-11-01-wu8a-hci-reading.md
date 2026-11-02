# hci-expert reading: WU8a (plan 0016), `feature/core-mv-a` at `1a5af96`

- Reader: hci-expert, Claude Opus 5.5 (`claude-opus-5-5`), 2026-11-01. Agent work. Role: the reading that plan 0016 section 12 places ahead of the W5 review. It is advice, not the review; a blind reviewer checks correctness in parallel.
- Read as: a user who knows geometric algebra and C++23 and has never seen Galoot's internals.
- Subject: the public surface WU8a adds or changes: `include/galoot/core/mv.hpp` (`mv`, `ring_scalar`, `exact_scalar`, `multivector`, `has_blade`, `get`) and `blades<Alg, "...">` in `include/galoot/core/blade_set.hpp`, at `1a5af96` (the header's code is `878b624`'s, its comments `669f7e2`'s). The branch was not edited.
- Work: branch `review/hci-wu8a` from `1a5af96`, worktree `.claude/worktrees/hci-mv-a`. Spike sources, scripts, and every diagnostic text quoted below are in `docs/agent/records/spikes/wu8a-hci-reading/`.
- Compilers: GCC 16.2.1 and Clang 23.1.4 with libstdc++ 16, `-fsyntax-only`, from the spike directory so that paths are relative. Two flag sets: "project" (`-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off`) and "plain" (`-std=c++23 -Wall`, as a user's own project might have).
- Verdict on the surface: **ready after fixes**. Nothing blocks the merge. The counted constructor does what it was built for: every wrong count I tried, in eight forms, gives one error that prints both counts. Named access names each of seven causes in the algebra's terms at compile time, which no library in note 0019's survey does. Four cheap fixes belong in WU8a because they touch the first error line or a name before anyone depends on it: F1 to F4. The rest is for WU8b, later units, or the owner.
- Job pool: not started or stopped. It showed 23 of 23 slots free at my first run, and 21 of 23 (two other workstreams' ninja clients) after the main run.

## Departures, skipped checks, and ambiguities (C20)

1. The brief says to read the knowledge-base notes "via branch kb/api-design worktree". That branch has been merged into the knowledge base's `develop` (`811cb70`) and has no worktree, so I read `api-design/` on `develop`, read-only.
2. "New empty build trees": no CMake build was needed. Every program is a single compile from the spike directory, and the one program that runs (`probes.cpp`) was built into a new empty directory in my scratch space. The full suites are the blind reviewer's and the author's (plan Outcome, criterion 12); I did not rerun them.
3. Not run through `tools/timed`: about a dozen single `-fsyntax-only` compiles of under a second each while writing the spike (scratch probes of `static_assert` printing, of the narrowing message, and of inherited `explicit(bool)`), and git commands. The spike runs went through `tools/timed`: `first_user` three times (93, 95, and 101 s; the last after `clang-format`, which also moved the routes program's includes and was undone there with `clang-format off`), `routes` twice (14 and 12 s), `probes` on both compilers (0.3 s each), case 0 built and run (0.8 s), and the formatting (0.1 s). The texts in `out/` are from the last runs. Their rows are in this branch's `runs.tsv`.
4. The scratch probes that found the GCC defect of F15 and the `static_assert` printing behind F2 are restated as committed programs only in part: the GCC defect's minimal form is quoted in F15, not committed as a file. The route experiments that measure F1, F2, and F6 are committed.
5. Only GCC 16.2.1 and Clang 23.1.4 with libstdc++. MSVC and libc++ were not tried.
6. No person was observed. The judgments about what a first-time user expects are mine (Judgment), backed by note 0019's sources where cited.
7. I read the plan's Outcome (WU8a) last, as briefed, after the checklist pass, the 49 cases, and the route experiments were done. Its claim that only route 1 would shorten GCC's lines is the one place the reading changed in response (F6 is the measured counterexample; it was measured before I read the claim).
8. Error counts: my first run counted GCC's nested "• error:" lines inside a note as errors. The committed `run.py` counts top-level error lines only, and the tables below are from that run.

## 1. Task first

Written from the examples only (plan 0016 section 4, the comment at the head of `mv.hpp`, `test/core/mv_test.cpp`, and the compile-fail sources), before reading the header:

1. Declare an algebra and the named types I work with: a tag `struct rga3 { static constexpr auto value = gc::rigid<3>(); };`, a layout `inline constexpr auto line_layout = gc::blades<rga3, "e41 e42 e43 e23 e31 e12">;`, and `using Line = gc::mv<rga3, line_layout, double>;`.
2. Make values and compare them: `Line axis{0, 0, 1, 0, 0, 0};`, `Line none;` (zero), `axis != Line{...}`.
3. Read and write a coefficient by the name in the published formula: `get<"e43">(axis)`, `get<"e23">(axis) = 2.0;`.

All three compiled first time on both compilers (`first_user.cpp`, case 0). Where I had to leave the examples:

- The tag's member must be named `value`. The examples show it, but the rule is only in `metric_like`'s comment, and a misnamed member (case 15) gives a constraint failure that GCC reports inside `<span>` (F9).
- The scalar type of a multivector type. `mv` has `algebra_type` but no `scalar_type`; the tests themselves spell it `decltype(x.coefficients)::value_type` (`mv_test.cpp:112`, `:606`). F4.
- Whether `get` needs a namespace: the plan qualifies it, the tests do not. `get`'s comment answers it (found by ADL).
- Which names a type holds when I have only the type: `Type::layout.blades` gives masks and signs, not names. The "absent" message says "its layout lists the names it holds", which works only when I wrote the layout myself (F7).
- How to print a value: nothing in WU8a; `std::cout << line` is 492 lines on GCC (case 27). `std::format("{}", line.coefficients)` works (case 0). F14.
- Whether parentheses check narrowing: the constructor comment says each value "converts to T as in brace initialization", which is true only for braces (F5).

## 2. The checklist (decision 0160), item by item

Evidence is a file and line at `1a5af96`, or a case of `first_user.cpp` (`out/<compiler>-<flags>-case<N>.txt`), or a route of `routes/program.cpp`.

### Names

| Item | Verdict | Evidence |
| --- | --- | --- |
| Field's terms or full words; abbreviations the field uses everywhere | Met, one reservation | `coefficients`, `layout`, `component_count`, `ring_scalar`, `exact_scalar`, `multivector`, `has_blade`, `get` are words or the field's. `mv` is an abbreviation, but the field's (Gaigen 2 generates `mv`) and STYLE S10 names it as vocabulary. The type is `mv` and its concept `multivector`, which is the `string`/`basic_string` kind of pair a C++ user knows. `has_blade` is false for a blade the type holds in the other orientation (F11) |
| One meaning across the library; nothing std means differently | Not met, one case | "Layout" and "blade set" name the same thing in one header's messages: `mv.hpp:193`, `:195`, `:197` say "the blade set", `:228` and `:377` say "the layout" (F3). `get` means what `std::get<T>` means, access by key, and coexists with it (case 47: `using namespace std;` still gives Galoot's one message). `truncate`: WU8b |
| Similar things look alike | Not met, one case | `algebra_type` exists, `scalar_type` does not (`mv.hpp:201`, F4). Minor: a blade set's count is `size()`, a multivector's `component_count`, and `Line::layout.size()` also works; acceptable because the member is static and a constant |

### Construction and conversion

| Item | Verdict | Evidence |
| --- | --- | --- |
| The shortest form is correct; wrong counts and narrowing are compile errors | Met for braces; not for parentheses | Braces: too few (1), too many (2), assignment of a short list (44), `emplace_back` short (8), an array (7) all give one error with "N == K". `int` and `double` variables narrow to an error (3, 4) under the project's flags. Parentheses: `FPoint(d, 0, 0, 1)` compiles under plain flags and holds 0.10000000149011612; `Point(2^53 + 1, ...)` holds 9007199254740992 (`out/probes-g++.txt`). GCC without `-Werror` only warns on a narrowing non-constant (3 and 4, plain: exit 0, one warning). Both are the language's rules, but the comment promises more (F5) |
| Default-constructed value is zero | Met | case 0 (`Line none;`), case 26 (`constexpr Line zero;` read by `get` in a `static_assert`), `mv_test.cpp:219-263` |
| Lossy conversions named, never implicit; lossless explicit | Not applicable yet (WU8b). What exists is right: `Motor m = l;` (22) and `Point s = q;` (23) are compile errors |
| No hidden default | Met | No default scalar (40: "wrong number of template arguments (2, should be 3)"); orientation comes from the names written; checking mode: WU8b |

### Misuse and diagnostics

| Item | Verdict | Evidence |
| --- | --- | --- |
| Each catalogued misuse has a compile-fail test on both compilers requiring the literal | Met for the plan's catalogue | `test/core/mv_*_fail.cpp`, 29 files; `test/CMakeLists.txt:891-895` for narrowing ("narrow"). Not in the catalogue and not tested: a named layout of a larger algebra (18; same message as `mv_beyond_n`), a `const` or reference scalar (37, 38), `bool` (39) |
| One error per misuse; the message on the first error line after the inclusion context | Met for every Galoot message | GCC: line 7, after the 2-line inclusion context and 4 lines of instantiation context; Clang: line 3. Cases 1, 2, 7, 8, 9, 11, 12, 18, 33, 36, 44, 47. Not met for compiler messages Galoot cannot shape: a `double` variable into `float` under `-Wconversion` is two errors on both compilers (4), a `const double` scalar is 4 errors on GCC (38) |
| Operation first, mistake in the algebra's terms, fix after a semicolon; no masks, positions, internal names | Partly met | The `get` messages are the model (`mv.hpp:362-382`). The three layout checks state no fix (`mv.hpp:193-198`); acceptable for hand-written sets, less so for case 18, a layout written for rigid(3) used in R(3,0,0). On the first error line itself: narrowing names `galoot::core::detail::component<double, 3>` (GCC) and `component<double, 3UL>` (Clang) where the aggregate said `double` (3, 4, 25; F1), and Clang's "due to requirement" prints `detail::blades_in_algebra<3, galoot::core::blade_set<6>{{{{9, -1}, ...}}}>` before the layout message (18; F2) |
| Context lines show the user's terms | Not met on GCC; partly on Clang | GCC prints the layout as masks through `std::__array_traits<...>::_Type` in every instantiation context (all Galoot messages; up to 1,247 characters for `emplace_back`, case 8). Clang shows `Line` (aka `gc::mv<rga3, line_layout, double>`) in its own errors (19, 20, 27, 31, 34, 42), but in the context note of a Galoot message it prints `mv<rga3, blade_set<6>{{{{9, -1}, ...}}}, double>` (1, 9, 11, 12). The comment at `mv.hpp:23-24` ("lets Clang print those names in its diagnostics") holds only for the first kind. Owner decision (section 4, F6) |
| Distinct causes, distinct messages | Met, one gap | `get`: seven causes, seven messages (1 per case; 9 is "not a name" for `e4l`). Gap: an array passed whole (7) gets the count message ("1 == 4"), which states a cause the user did not make (F10) |
| Probed concepts test data; documentation says which probes answer wrongly | Met, one omission | `has_blade` and `multivector` test data (`mv.hpp:83`, `:317`). The comments say `requires { get<"e1">(line); }` and `std::is_constructible_v<Line, double, double, double>` answer yes (`mv.hpp:222`, `:333-335`; case 33 gives one error with the message). Not said: `std::is_constructible_v<Line, std::array<double, 6>>` is also yes (probes), by the same constructor (F5) |

### Semantics and cost

| Item | Verdict | Evidence |
| --- | --- | --- |
| Discarding, re-representing, or assuming operations say so | Met for what exists | `get` documents the rvalue case; `truncate`: WU8b |
| `auto r = f(a, b);` is safe | Met | `auto c = get<"e43">(make_line());` copies. `const double& r = get<"e43">(make_line());` dangles (29): GCC catches it under `-Wextra` (`-Wdangling-reference`), Clang is silent with every warning on. `get`'s comment says so (`mv.hpp:336-338`), as `std::get` behaves |
| Exact comparison is `==`; tolerances named | Met | `mv.hpp:179-182`; IEEE behavior tested (`mv_test.cpp:337`) |
| Predictable cost in the `///` comment | Met | Layout limits (`mv.hpp:169-173`), `get`'s position at compile time (`:338-339`) |

### Examples and readability

| Item | Verdict | Evidence |
| --- | --- | --- |
| Every example compiles and is the idiom to copy | Met in the header; the plan's first snippet does not compile as written | The head of `mv.hpp` compiles as written. Plan 0016 section 4's first block puts `gc::get<"e23">(axis) = 2.0;` at namespace scope, where it is not a statement (`plan_snippets.cpp`, SNIPPET 1: GCC "‘galoot::core::get’ is not a type"); inside a function it compiles on both (SNIPPET 2). The third block's narrowing line is an error on Clang and only a warning on GCC under plain flags (SNIPPET 3) |
| A maintainer finds each behavior from its name; one rule, one place | Met | Each check is one `static_assert` beside its message; the seven `get` causes are one `if constexpr` chain (`mv.hpp:361-383`); the parser is shared with `blades`, so a name means the same in both. The `zero_at` comment says why the zero is written per place |

### Before release (Hyrum's law)

Interface changes this reading suggests that would break users later, cheapest now: `scalar_type` (F4, additive); the wording "layout" in messages (F3, breaks only tests that match the text); the name `has_blade` (F11); whether `mv` supports structured bindings and positional `get<I>` (F12); and, largest, whether model types are classes derived from `mv` or aliases of it (F6), since that decides type identity for every user.

## 3. First-time-user programs: 49 cases, two compilers, two flag sets

`first_user.cpp` with `-DCASE=N`, run by `run.py` (196 compiles, 95 s). Lines / top-level errors / longest line as installed, project flags; "ok" means it compiled. The first diagnostic line is quoted with its path removed; the full texts are in `out/`.

| # | Mistake or confusion | GCC | Clang | First diagnostic line (GCC unless marked) | Style |
| --- | --- | --- | --- | --- | --- |
| 1 | `Point s{1, 2, 3};` | 10/1/468 | 12/1/197 | `mv.hpp:227:37: error: static assertion failed: mv: give one value for each component, in the order the layout lists them` (then "(3 == 4)") | Good; context lines show masks |
| 2 | seven values for six | 10/1/566 | 12/1/239 | same message, "(7 == 6)" | Good |
| 3 | `int w` into a `double` place | 5/1/150 | 8/1/170 | `narrowing conversion of ‘w’ from ‘int’ to ‘galoot::core::detail::component<double, 3>’ {aka ‘double’}` | Internal name on the first line (F1). Plain flags, GCC: compiles with a warning |
| 4 | `double d` into a `float` place | 6/2/158 | 11/2/171 | the same with `component<float, 0>`, then `-Wfloat-conversion` | Two errors (compiler); F1 |
| 5 | the same with parentheses | 5/1/158 | 4/1/168 | `conversion from ‘double’ to ‘...component<float, 0>’ may change value [-Werror=float-conversion]` | Plain flags: compiles silently on both (F5) |
| 6 | `Point s{{1, 2, 3, 1}};` (the `std::array` habit) | 65/1/889 | 26/1/237 | GCC `no matching function for call to ‘galoot::core::mv<rga3, galoot::core::blade_set<4>{...}’`; Clang `no matching constructor for initialization of 'const Point' (aka 'const gc::mv<rga3, point_layout, double>')` | Compiler's; valid with the aggregate, now an error (F10) |
| 7 | `Point s{values};`, an array | 10/1/483 | 12/1/214 | the count message, "(1 == 4)" | Misstates the cause (F10) |
| 8 | `points.emplace_back(1, 2, 3);` | 21/1/1247 | 21/1/399 | the count message at line 18, inside `std::construct_at` | Message present; long |
| 9 | `get<"e4l">(l)` (letter l) | 10/1/504 | 9/1/252 | `get: a name is 1 or e followed by generator indices, as in e41` | Good |
| 10 | commas in `blades<rga3, "e41, e42, ...">` | 16/1/230 | 16/1/216 | `blades: after the e, a name lists generator indices, each a digit from 1 to 9`; Clang adds `due to requirement 'error != galoot::core::detail::NameError::bad_index'` | Wrong cause named (the separator); internal name on Clang (F16) |
| 11 | `get<"e14">(l)` | 10/1/504 | 9/1/252 | `get: the type stores this blade with the other orientation; use the name its layout gives it and negate the value` | Good |
| 12 | `get<"e1">(l)` | 10/1/503 | 9/1/251 | `get: the type has no blade of this name; its layout lists the names it holds` | Good (F7 for computed layouts) |
| 13 | `mv<rga3, "e41 e42 ...", double>` | 29/5/118 | 13/2/172 | `class template argument deduction failed:`; Clang `no viable constructor or deduction guide for deduction of template arguments of 'blade_set'` | Compiler's; the likeliest wrong guess after reading about names as written |
| 14 | `blades<4, "e1 e2 e3 e4">` (the old spelling) | 5/1/227 | 7/1/90 | `type/value mismatch at argument 1 ...`; Clang `template argument for template type parameter must be a type` | Compiler's; clear enough |
| 15 | tag member named `metric`, not `value` | 18/1/299 | 10/1/148 | `use of invalid variable template ‘blades<bad_tag, ...>’`, then GCC locates `‘value’ is not a member of ‘bad_tag’` in `/usr/include/c++/16/span:472` | Constraint text; GCC points into `<span>` (F9) |
| 16 | the metric's type as the algebra | 18/1/315 | 10/1/152 | as 15, `blades<galoot::core::metric<4>, ...>` | As 15 |
| 17 | `mv<cga3, line_layout, double>`, a layout written for rigid(3) | ok | ok | | Silent (F13) |
| 18 | `mv<r3, line_layout, double>`, R(3,0,0) | 12/1/434 | 11/1/240 | `mv: the blade set names a generator beyond e_n`; Clang first: `due to requirement 'detail::blades_in_algebra<3, galoot::core::blade_set<6>{{{{9, -1}, ...}}}>'` | Masks on Clang's first line (F2); no fix given |
| 19 | `l == q` (line, motor) | 29/1/1046 | 8/1/410 | `no match for ‘operator==’ (operand types are ‘Line’ {aka ‘galoot::core::mv<rga3, ...masks...>’} ...`; Clang `invalid operands to binary expression ('Line' (aka 'gc::mv<rga3, line_layout, double>') and 'const Motor' ...)` | Compiler's; Clang good, GCC long (WU9 catch-alls) |
| 20 | `Line<double> == Line<float>` | 29/1/972 | 8/1/385 | as 19 | As 19 |
| 21 | one-blade `s == 2.0` | 21/1/580 | 7/1/251 | as 19 | As 19 |
| 22 | `const Motor m = l;` | 4/1/893 | 16/1/307 | GCC `conversion from ‘mv<[...],...blade_set<6>...>’ to non-scalar type ‘mv<[...],...blade_set<8>...>’ requested` | Compiler's (WU8b adds `Motor{l}`) |
| 23 | `const Point s = q;` | 4/1/815 | 16/1/282 | as 22 | As in the diagnostics record |
| 24 | `const Motor m{l};` | 65/1/1215 | 25/1/347 | `no matching function for call to ...` | WU8b |
| 25 | `0.5` for an `int` scalar | 4/1/147 | 11/2/159 | `narrowing conversion of ‘5.0e-1’ from ‘double’ to ‘...detail::component<int, 0>’` | F1 |
| 26 | `constexpr Line zero;` read by `get` | ok | ok | | Correct |
| 27 | `std::cout << l` | 492/1/675 | 150/1/414 | `no match for ‘operator<<’` | Compiler's (F14) |
| 28 | `std::format("{}", l)` | 177/6/1011 | 70/4/473 | `std::formatter must be specialized for each type being formatted` | Compiler's (F14) |
| 29 | `const double& r = get<"e43">(make_line());` | 8/1/453 | ok | `possibly dangling reference to a temporary [-Werror=dangling-reference]` | GCC catches it under `-Wextra`; Clang silent; plain flags: silent on both |
| 30 | `double& r = get<"e43">(make_line());` | 4/1/116 | 4/1/117 | `cannot bind non-const lvalue reference of type ‘double&’ to an rvalue of type ‘double’` | Correct |
| 31 | `auto [x, y, z, w] = p;` | 5/1/377 | 4/1/143 | `4 names provided for structured binding`; Clang `type 'const Point' (aka ...) binds to 1 element, but 4 names were provided` | Compiler's (F12) |
| 32 | `auto [all] = p;` | ok | ok | binds the whole `std::array` | Silent (F12) |
| 33 | generic code asks `requires { get<"e1">(x); }` | 13/1/515 | 12/1/258 | `get: the type has no blade of this name; ...` | As documented |
| 34 | `for (double c : l)` | 39/2/91 | 4/1/149 | `‘begin’ was not declared in this scope` | Compiler's; `l.coefficients` is the range |
| 35 | a lane-pack scalar with no `==`, then `a == b` | 65/1/949 | 28/1/285 | `no match for ‘operator==’ ...` | As planned (compiler's) |
| 36 | `std::vector<double>` as scalar | 45/1/450 | 15/1/196 | `mv: the scalar type must support +, -, *, unary - and construction from an integer coefficient` | Good |
| 37 | `double&` as scalar | 34/3/605 | 19/2/192 | `template constraint failure for ‘template<...> requires (metric_like<Alg>) && (semiregular<T>) struct galoot::core::mv’` | Cascade (F8) |
| 38 | `const double` as scalar | 49/4/610 | 22/2/235 | as 37 | Cascade (F8) |
| 39 | `bool` as scalar | ok | ok | | Silent; `ring_scalar<bool>` is true (F8) |
| 40 | no scalar given | 19/4/177 | 16/2/81 | `wrong number of template arguments (2, should be 3)` | First line clear; follow-on errors |
| 41 | `Line::scalar_type` | 7/2/878 | 4/1/165 | `‘scalar_type’ in ‘using Line = struct galoot::core::mv<...masks...>’ does not name a type` | F4 |
| 42 | `l[0]` | 4/1/486 | 4/1/120 | `no match for ‘operator[]’` | Compiler's; storage is `l.coefficients` |
| 43 | `std::get<0>(l)` | 272/1/482 | 85/1/219 | `no matching function for call to ‘get<0>(Line&)’` | Compiler's (F12) |
| 44 | `p = {1, 2, 3};` | 10/1/468 | 12/1/197 | the count message, "(3 == 4)" | Good |
| 45 | template arguments swapped | 16/5/228 | 10/2/90 | `type/value mismatch at argument 1 ...` | Compiler's |
| 46 | `Point s{.coefficients = {1, 2, 3, 1}};` | 5/2/412 | 4/1/164 | `designated initializers cannot be used with a non-aggregate type` | Compiler's; clear |
| 47 | `get<"e14">(l)` under `using namespace std;` | 10/1/504 | 9/1/252 | as 11 | Good: `std::get` does not interfere |
| 48 | `get<0>(l)`, a position for a name | 67/1/198 | 10/1/190 | `no matching function for call to ‘get<0>(Line&)’` | Compiler's (F12) |

Under plain flags the texts are the same except: 3 and 4 compile on GCC with a narrowing warning; 5 compiles silently on both; 29 compiles silently on GCC; 25 is one error and one warning on Clang.

Probes (`probes.cpp`, built and run, both compilers agree): `std::regular<Line>` yes; `std::is_aggregate_v<Line>` no; `std::is_constructible_v<Line, double, double, double>` yes and `<Line, std::array<double, 6>>` yes; `std::is_convertible_v<double, Scalar>` no; `std::ranges::range<Line>` no, of its `coefficients` yes; tuple-like no; `requires { get<"e1">(line) }` yes while `has_blade<Line, "e1">` no; `ring_scalar<bool>` yes, `<char>` yes, `<int*>` no, `<std::string>` no; `mv<cga3, line_layout, double>` is the same type as `mv<cga3, blades<cga3, "...">, double>`; `blades<rga3, "e41  e42 ...">` with two spaces gives the same type as with one.

The last probe is worth stating for the owner: because a layout is a value of masks, the spelling of the names does not decide type identity. Route 1 (names as written in the type) would lose that unless it normalized the text (note 0019, section 6.2).

## 4. Naming, discoverability, and the changes of this unit

- **The aggregate-to-constructor change.** The evidence says it was right. Note 0019 found that an aggregate takes three values for four with no warning, and here every short or long list is one error with both counts, including the forms the plan did not list: assignment from a list (44), `emplace_back` (8, where the message survives being instantiated inside `std::construct_at`), and a whole array (7). It costs five things the plan did not foresee: the alias `detail::component<T, I>` now names the target type of every narrowing error (F1); double braces, valid with the aggregate, are a 65-line error (6); designated initializers are rejected (46, a clear message); parentheses never checked narrowing and still do not, which the comment hides (F5); and `std::is_constructible_v` answers yes for an array (probes). Only F1 and F5 need work now.
- **`blades<Alg, "...">`.** Better than `blades<N, "...">`: the user writes the algebra once, as in note 0019's alternative (b). The old spelling (14) and a non-type (16) give the compiler's short constraint errors. Two things to know. The set does not keep its algebra, so `mv<cga3, line_layout>` compiles (17) and is the very type `mv<cga3, blades<cga3, "...">>` (probes); the parameter reads as ownership but only n is used, which `blades`' comment says and `mv`'s does not (F13). And a tag without `value` gives a constraint failure that GCC places in `<span>` (F9).
- **Names.** Field terms throughout, except the two points F3 (layout or blade set) and F11 (`has_blade`). `get` is the right name: it is what `std::get<T>` does (access by key), it is found by ADL, and it coexists with `std::get` under `using namespace std;` (47). `component_count` beside `coefficients` is defensible (a component is a place, a coefficient its value; S8 uses both so), but `scalar_type` is missing (F4).
- **Discoverability.** The three tasks need four names (`blades`, `mv`, `get`, and the tag's `value`), all shown in the header's opening comment, which compiles. Gaps: no way to see which names a type holds when its layout was computed (F7), no printing (F14), and the tag protocol only in `metric_like`'s comment (F9).
- **The plan's snippets** (section 4). The judgments in its table hold, except "Clang prints `mv<rga3, line_layout, double>` through the alias": true for the compiler's own errors, not for Galoot's messages, whose Clang context note prints the masks (cases 1, 9, 11, 12). The first block does not compile at namespace scope as written (section 2), which matters only when it becomes documentation (W12).

## 5. The diagnostics table, and the routes

### Does it need more work now?

It can wait, with two small exceptions. Every Galoot message is on the first error line, alone, and says what to do. What is long on GCC is the instantiation context above it (4 lines, up to 566 characters for a seven-blade count error, 1,247 inside `emplace_back`), which a user skims once the message line is found. That is a cost in reading effort, not a cause of wrong fixes, and nothing in WU8a's catalogue is left without a message. The two exceptions touch the first error line itself, not the context: F1 (the internal alias in every narrowing error) and F2 (masks on Clang's first line for a layout check). Both are local to `mv.hpp`, measured below, and belong in WU8a.

The case for doing more later is GCC's context lines for `get` and for conversions (WU8b), and the comparison messages of WU9, where GCC prints up to 1,046 characters (19). Those are what route B below addresses.

### Ranking of the outer routes for the owner

The owner prefers consistent storage types and other routes to readable diagnostics. Ranked by measured gain per cost, with storage unchanged unless said:

1. **Named model classes** (not in plan 0016 section 5's list; storage unchanged). A user or model type declared as `struct Line : gc::mv<rga3, line_layout, double> { using mv::mv; };` prints as `Line` on both compilers in every context line. Measured (route `named_class`): `get<"e14">` GCC 499 to 257 characters, Clang 248 to 169; `get<"e1">` 498 to 256 and 247 to 149; `const Point s = q;` GCC 811 to 98 characters ("conversion from ‘const Motor’ to non-scalar type ‘const Point’ requested", 4 lines), Clang 279 to 164 ("no viable conversion from 'const Motor' to 'const Point'"); `l == q` GCC 1,046 to 865. Costs: `multivector` must accept a class derived from an `mv` (a member alias and `std::derived_from`, data only); a named type and the `mv` it derives from are two types for one layout, so results of products (which are `mv`s) need an explicit conversion into the named type; GCC's `explicit(bool)` defect (F15) must be worked around; the count error is unchanged (the inherited constructor is still `mv`'s). It fits STYLE S10's "model object type (`rga3::Point`)" and would be decided with the models (WU16's aliases). Owner decision.
2. **Readable first lines** (two local changes to `mv.hpp`; storage unchanged). Route `type_pack`: the constructor's parameters spelled as a pack of T instead of `detail::component<T, I>`. Narrowing GCC 147 to 96 characters, "narrowing conversion of ‘w’ from ‘int’ to ‘double’", Clang 167 to 136, "cannot be narrowed from type 'int' to 'double'": the aggregate's text back. Route `named_checks`: the three layout checks read named static members. Clang's first line for case 18 becomes "static assertion failed due to requirement 'layout_names_only_generators_of_the_algebra': mv: the blade set names a generator beyond e_n", 237 to 177 characters, the rule named instead of the masks; GCC unchanged in substance. Both keep every property (route program case 0 checks not-aggregate, trivially copyable, standard-layout, size, explicit from one scalar, `multivector`, `has_blade` on both compilers), and the compile-fail tests' required texts ("narrow" and the literals) are unaffected. Recommended for WU8a (F1, F2).
3. **Route 1, names as written in the type.** The largest gain where it applies (note 0019: 534 to 265 for `get`, 892 to 363 for a lossy conversion), but identity by text, a second family of types, and no names for computed results. Route B gives most of its gain for the types a user names, with no second representation. Keep it recorded.
4. **Route 9, `format`** (WU15): printing by oriented names, which also answers F7 and F14 for output, not for diagnostics.
5. **Route 8, mixed-algebra catch-alls** (WU9): one Galoot message for 19 to 21, where GCC now prints 29 lines.
6. **Route 10, computed messages**: C++26; not available under the project's flags.
7. **The count catch-all in a base that knows only K** (route `count_base`, tried and not recommended). The idea was that GCC's instantiation context would then name no layout. It does name none, but GCC then points "required from here" at the library's using-declaration (`mv_routes.hpp:362`) and adds an "in ‘constexpr’ expansion of ‘((galoot::core::mv<rga3, ...masks...>’" line at the user's code, so case 1 grows from 9 lines and 463 characters to 12 and 707. Clang: 194 to 179. Negative.
8. **Route 11, form T**: not recommended (note 0019).

### The two experiments, as run

`routes/mv_routes.hpp` is a copy of `mv.hpp` at `1a5af96` with the routes behind macros; it keeps the real include guard, so `routes/program.cpp` includes it before `<galoot/core.hpp>` and the real header is skipped. Nothing under `include/` changed. `routes/run.py` compiles 8 cases under 6 route settings on both compilers (96 compiles, 14 s, `routes/out/summary.tsv`). Case 0, the correct program with the property checks, compiles under every setting on both compilers, except `named_class` alone on GCC, where `std::is_convertible_v<double, Point>` is wrongly true (F15); with the split catch-all of the `all` setting it is false again.

## 6. Findings

Severity: **blocks** (WU8a merge), **fix in WU8a**, **defer** (named unit), **owner**. None blocks.

### F1. Narrowing errors name `detail::component<double, 3>` (fix in WU8a)

- Evidence: cases 3, 4, 25, both compilers, both flag sets; `mv.hpp:109-110`, `:139-140`. GCC: "narrowing conversion of ‘w’ from ‘int’ to ‘galoot::core::detail::component<double, 3>’ {aka ‘double’}". The aggregate this replaced said "to ‘double’" (note 0019, case 4). Every user who narrows sees a library-internal name and a position on the first error line, against the message style's "no positions or internal names".
- Change: spell the parameters as a pack of T, as `routes/mv_routes.hpp` does under `ROUTE_TYPE_PACK` (`packed_storage<Multivector, T, Ts...>` built from `template <std::size_t> using same = T;`). Before: `coefficient_storage(component<T, I>... values)`. After: `packed_storage(Ts... values)` with every `Ts` equal to T. Measured: "from ‘int’ to ‘double’" on both compilers, 147 to 96 and 167 to 136 characters, properties unchanged.

### F2. Clang prints masks on the first line of a layout check (fix in WU8a)

- Evidence: case 18 (a rigid(3) layout in R(3,0,0), a realistic mistake): "static assertion failed due to requirement 'detail::blades_in_algebra<3, galoot::core::blade_set<6>{{{{9, -1}, {10, -1}, ...}}}>': mv: the blade set names a generator beyond e_n". The same holds for the sign and repeated-mask checks (`mv.hpp:190-198`), which assert a variable template whose arguments Clang prints. A scratch probe showed the same for `bool(...)` and `== true`; only a named constant, or `static_assert(false)` inside a branch, avoids it.
- Change: give each check a named static member, as under `ROUTE_NAMED_CHECKS` (`layout_names_only_generators_of_the_algebra`, `layout_signs_are_plus_or_minus_one`, `layout_holds_each_blade_once`; private members would do). Measured 237 to 177 characters, the rule named. While there, give the range check a fix after a semicolon, since case 18 is a layout of another algebra, for example "mv: the layout names a generator beyond e_n; a layout written for a larger algebra cannot be used here".

### F3. "Layout" and "blade set" for one thing (fix in WU8a)

- Evidence: `mv.hpp:193`, `:195`, `:197` ("the blade set ...") against `:228` and `:377` ("the layout lists ..."); the user's code says `line_layout` and the member is `layout`.
- Change: "layout" in all of `mv`'s messages ("mv: the layout names a generator beyond e_n", "mv: the layout holds a sign other than +1 or -1", "mv: the layout holds one blade twice, perhaps in both orientations"). `blades:` messages keep "name". The compile-fail tests that match the literals change with them; cheaper now than after release.

### F4. `scalar_type` is missing (fix in WU8a)

- Evidence: case 41; `mv_test.cpp:112` and `:606` spell it `decltype(x.coefficients)::value_type`, so a present caller exists (S16); decision 0150 item 5 left it out for want of one. `algebra_type` beside no `scalar_type` breaks "similar things look alike".
- Change: `/// The scalar type of the coefficients.` `using scalar_type = T;`, and the two test lines use it.

### F5. The comments promise more than the language gives (fix in WU8a)

- Evidence: `mv.hpp:211-213` says each value "converts to T as in brace initialization, so ... a variable that would narrow is an error". With parentheses (`FPoint(d, 0, 0, 1)`, `emplace_back(d, ...)`) a value converts as in any call: 0.1 becomes 0.10000000149011612 and 2^53 + 1 becomes 2^53, silently under `-Wall` (case 5, probes). With braces, GCC treats a narrowing non-constant as a warning unless warnings are errors (cases 3, 4 plain). `mv.hpp:222` names only the three-`double` probe; `std::is_constructible_v<Line, std::array<double, 6>>` is also true. `mv.hpp:23-24` says naming the layout "lets Clang print those names in its diagnostics", which holds for the compiler's own errors only.
- Change, for example: "With braces, Line{...}, each value converts to T as in brace initialization: an integer literal is accepted for a floating scalar, and a variable that would narrow is rejected (an error on Clang; on GCC a warning unless -Werror). With parentheses, as for any constructor, values convert implicitly and may narrow." Add the array case to the probe sentence, and "in the compiler's own errors, such as a failed comparison" to the head comment.

### F6. Named model classes shorten GCC's lines without a storage change (owner)

- Evidence: section 5, route 1; `routes/out/`. This contradicts the Outcome's sentence that "only route 1 (names as written in the type) would shorten" GCC's lines.
- Proposal: when the models' types are designed (WU16, or WU11 if earlier), decide whether `rga3::Line` is an alias of `mv<...>` or a class derived from it. If a class: `multivector` accepts classes derived from an `mv` (data only), each named class has an explicit conversion from its base, and the count catch-all is split (F15).

### F7. "Its layout lists the names it holds" has no list to point at for a computed layout (defer to WU8b)

- Evidence: `mv.hpp:377-378`. True when the user wrote `blades<rga3, "...">`. For a type over `full<4>`, `of_grade<4, 2>`, a `merge`, or a product's result set (WU8b's `apply`), no names exist in the user's code, and the diagnostic shows only masks.
- Change: decide with WU8b, where result types first reach users. Options: a function that returns a layout's oriented names as text (also usable by `format`, WU15), named in the message ("...; layout_names(T::layout) lists them"); or a message that says the names are canonical for a generated layout.

### F8. Scalars that are references, const, or `bool` (defer to WU8b)

- Evidence: cases 37 and 38: `double&` and `const double` fail `std::semiregular` in `mv`'s template head, so the body's literal never fires: GCC 34 lines and 3 errors, 49 and 4; Clang 19/2, 22/2. Case 39: `bool` is accepted, because `ring_scalar<bool>` holds through integer promotion, though `true + true` is `true`.
- Change: constrain T by a weaker named concept that a reference and a const type meet (`std::destructible`), and assert in the body, first, "mv: the scalar type must be a plain object type, not a reference or const; use double, not const double", then `ring_scalar`. Exclude `bool` from `ring_scalar`, with a test. Both are cheap; neither is in the plan's catalogue.

### F9. A tag without `value` (owner, or defer to WU16)

- Evidence: cases 15, 16. Clang: "constraints not satisfied for variable template 'blades' [with Alg = bad_tag, ...]", then the unmet requirement. GCC says the same, then places "‘value’ is not a member of ‘bad_tag’" in `/usr/include/c++/16/span:472` (its own satisfaction cache matched `decltype(_Tp::value)` there), and opens with "In file included from /usr/include/c++/16/format:52". A first-time user declaring an algebra meets this before anything else.
- Proposal: a literal for the tag protocol ("blades: an algebra tag holds its metric as static constexpr value, as in struct rga3 { static constexpr auto value = rigid<3>(); }"), by the class-body pattern of decision 0152 applied to the variable template's helper, or a library helper that makes the tag (`struct rga3 : gc::algebra<gc::rigid<3>()> {};`). STYLE S7's allowance is written for class templates, so the first is the owner's call; WU16's model tags make the second moot for model users.

### F10. Double braces and whole arrays (defer to WU8b)

- Evidence: case 6, `Point s{{1, 2, 3, 1}}`, the habit of `std::array` and of the old aggregate: GCC 65 lines, the constructor candidates with masks. Case 7, `Point s{values}` with a `std::array<double, 4>`: the count message, "1 == 4", a cause the user did not commit.
- Change: rejecting both is right (a constructor from `std::array` would bring back the silent zero fill for `{{1, 2, 3}}`). Give the single non-multivector argument its own catch-all in WU8b, when the conversion catch-alls are written: "mv: give the coefficients one by one, Point{1, 2, 3, 1}, not as one array or braced list".

### F11. `has_blade` is false for a blade the type holds (owner, before release)

- Evidence: `has_blade<Line, "e14">` is false (`mv_test.cpp:701`), though a line holds the blade e14 (as -e41). The name says "blade"; the test is "this oriented name, as the layout lists it". `get`'s messages draw exactly that distinction ("stores this blade with the other orientation").
- Proposal: rename to say what it tests, for example `has_name<X, "e41">` or `holds_name`, or keep `has_blade` and add a sentence to its comment and to the user guide. The name is in decision 0033 and plan reading 20, so this is the owner's.

### F12. Structured bindings and positions (defer to WU16; owner)

- Evidence: `auto [x, y, z, w] = p;` fails ("binds to 1 element"), while `auto [all] = p;` compiles and binds the whole array (31, 32); `get<0>(l)` and `std::get<0>(l)` fail with the compiler's candidate lists (43, 48; GCC 272 lines). Plan 0016 section 15 defers structured bindings, but the one-name form exists today, silently.
- Proposal: decide before release whether `mv` is tuple-like (`std::tuple_size` and `get<I>`, by position, beside `get<"name">`, as `std::get` has both index and type forms). Until then, say in `mv`'s comment that `auto [x, y, z, w] = p.coefficients;` is the way to unpack.

### F13. A layout does not belong to the algebra it was written for (defer to WU9; comment now)

- Evidence: case 17 compiles; the probe shows `mv<cga3, line_layout>` is `mv<cga3, blades<cga3, "...">>`. `blades`' comment says only n is read; `mv`'s does not.
- Change: one sentence in `mv`'s comment ("A layout is a list of blades and is not tied to the algebra named in blades<Alg, ...>; it can serve any algebra with enough generators"). WU9's mixed-algebra messages compare the types' algebras, which is where a mix becomes harmful.

### F14. Printing (defer to WU15; comment now)

- Evidence: case 27, `std::cout << l`: GCC 492 lines; case 28, `std::format`: 177. Case 0, built and run with GCC: `std::format("{}", l.coefficients)` prints `[0, 0, 1, 2, 0, 0]`.
- Change: until `format.hpp`, the comment of `coefficients` can say that the array prints with `std::format`.

### F15. GCC 16 drops `explicit` from an inherited constructor template whose `explicit(bool)` reads its own parameters (defer; affects F6 and users who derive)

- Evidence (scratch probe, minimal): `template <int K> struct E { template <class... V> explicit(sizeof...(V) == 1) E(V&&...); }; struct D : E<1> { using E::E; };` gives `std::is_convertible_v<double, D>` true on GCC 16.2.1, false on Clang 23.1.4; `D d = 2.0;` compiles on GCC. A non-template constructor with `explicit(K == 1)`, and a template whose condition reads only the class's parameters, keep `explicit` on both. In WU8a this affects nobody using `mv` directly. It affects any class derived from `mv` with `using mv::mv;` (route `named_class`, case 0 fails its property check on GCC): `std::is_convertible_v<double, Point>` is true there, though the conversion itself still stops at the count message.
- Change, when classes derive from `mv`: split the count catch-all into an `explicit` one-argument template and an implicit template for two or more, as `mv_routes.hpp`'s count base does. Recorded in the knowledge base's upstream-awareness table; not reported upstream.

### F16. Commas between names (defer)

- Evidence: case 10, `blades<rga3, "e41, e42, e43, ...">`: "blades: after the e, a name lists generator indices, each a digit from 1 to 9". The cause is the separator. Clang's first line adds "due to requirement 'error != galoot::core::detail::NameError::bad_index'", the F2 pattern in WU4's parser.
- Change: a parser error for a punctuation separator ("blades: names are separated by spaces, as in e41 e42"), and the F2 treatment of its `static_assert`s. WU4's code, so a later unit.

### F17. The count catch-all in a layout-free base (not recommended)

- Evidence: section 5, route 7. Recorded so that it is not tried again.

## 7. What a user would trip on that the plan did not foresee

1. A library-internal alias in every narrowing error (F1).
2. Parentheses that narrow silently, and GCC's narrowing warning without `-Werror` (F5).
3. Double braces and whole arrays, valid or plausible with the aggregate (F10).
4. A layout used in another algebra, silently (F13), or in a smaller one, with masks on Clang's first line (F2).
5. A tag whose member is not `value`, reported inside `<span>` on GCC (F9).
6. `const` and reference scalars, which miss the literal message, and `bool`, which is accepted (F8).
7. `auto [all] = p;` binding the array, and no positional `get` (F12).
8. The "absent" message pointing at a list of names that a computed layout does not have (F7).
9. A dangling reference from `get` on a temporary that Clang does not warn about (case 29; documented, as for `std::get`).
10. Writing the names straight into the type, `mv<rga3, "e41 ...", double>`, the likeliest guess after reading about route 1: five errors on GCC about class template argument deduction (case 13). A catch-all is not possible for a class template's parameter kind; the user guide should show the layout variable.
11. GCC's dropped `explicit` for anyone who derives from `mv` (F15).

## 8. Knowledge base

Added on branch `kb/api-design-value-type-traps` (worktree `knowledge-base-worktrees/api-design-value-type-traps`), committed there, for the coordinator to merge: the note `api-design/value-type-construction-traps.md`, the exercise `exercise-archive/api-design-first-user-traps.md` (`cpp-api02-2026-11-01`), a row in `upstream-awareness.md` for F15, and the index line in `api-design/README.md`.

## Agent cost

About 90 tool calls. Token counts are not visible to me. The plan estimated 120k to 180k tokens and 40 to 60 tool calls for this reading.
