# 0022 WU15: `format.hpp`, the misuse catalogue, and the remaining catch-alls

- Status: planned 2026-11-07 (planner: Claude, agent, Opus 5.5, planner role with cpp-expert and hci-expert judgement; implementers and reviewers to be named by the coordinator at branching). Not claimed until this plan reaches `develop` (W7); it was written on `records/plan-wu15`, branched from `develop` at `d68881f`. Nothing in section 4 starts before the owner answers the questions marked "owner" in section 10.
- In progress from 2026-11-08: the owner answered section 10 in decision 0196 item 5 (O1 to O5 as recommended, O6 to O9 the defaults). Parts 1 and 2 on `feature/core-format`, branched from `develop` at `bd60589`; implementer Claude (agent, Opus 5.5, cpp-expert role, also the coordinator of parts 1 and 2); test author a separate Claude agent (Opus 5.5, doctest-expert role) on `feature/core-format--tests`. Part 3 waits for the coordinator.
- Branches, named by W7 as amended (decision 0188): `feature/core-format`, worktree `.claude/worktrees/core-format`, from `develop` after this plan merges; the test author on `feature/core-format--tests`, worktree `.claude/worktrees/core-format--tests`, branched from `feature/core-format` at its first commit, never merged, its commits cherry-picked (decision 0185).
- Decision numbers: the next free numbers when each record is written (at this commit the highest used is 0195). This plan reserves none, because two other planners may be writing at the same time (risk 6).
- Source of scope: plan 0001 (section 3, "`format.hpp` outside the umbrella header"; WU15, "`format.hpp`; catch-all overloads with literal messages", size S, after WU8 and Spike C; M4; DoD-2, DoD-19, DoD-6); decision 0158 item 2 (WU8 writes the catch-alls of its own functions, the products' catch-alls belong to the units that add the products, and "WU15 keeps `format.hpp` and the review of the DoD-19 catalogue as a whole"); decision 0033 (form P, and "WU15 implements this form"); research note 0010 (Spike C); research note 0019, sections 6.9 and 6.10 ("print blades by their oriented names as the layout declares them"); decision 0153 item 2; the WU8a hci reading (F7, F9, F14) and the WU8b blind review (F10); decisions 0166, 0169, 0173, 0179 (message style); CONVENTIONS 1.1, 1.6, 1.7, 7.1.
- Goal: (1) a written misuse catalogue for DoD-19 that maps every rejection the core makes to its compile-fail test on both compilers, for the owner to ratify; (2) `include/galoot/core/format.hpp`, a `std::formatter` for multivectors that prints every coefficient on its blade's name, outside the umbrella header; (3) catch-alls with literal messages for the likely misuses the catalogue finds without one.

## 1. Departures, ambiguities, and readings (C20)

### Departures and ambiguities

1. **Most of the roadmap's "catch-all overloads with literal messages" already exist.** Plan 0001 gave WU15 the catch-alls as a whole; decision 0158 then moved each unit's catch-alls into the unit. At `d68881f` the core has form P catch-alls (decision 0033) for every binary operation (`product.hpp:252-461`, `linear.hpp:200-277`, `dual.hpp:241-679`, `transform.hpp:168-824`), for `truncate`, `part`, and `get` (`mv.hpp:1029-1291`), and constructor rejections in `mv` (`mv.hpp:606-674`), each pinned by compile-fail tests in `test/CMakeLists.txt` (236 sources in `test/core/*_fail.cpp`). Reading followed: WU15's catch-all work is the catalogue review that decision 0158 assigns, plus the few gaps it finds (R7), not a rewrite.

2. **DoD-19 says "matching the literal message", and three catalogue items are met by the compiler's own text instead.** An implicit conversion between layouts reaches no constructor and is the compiler's error, pinned on "to non-scalar type" (GCC) and "no viable conversion from" (Clang) (`test/CMakeLists.txt:1130-1137`; the WU8b review accepted this design, its departure 1). A tag that is not an algebra fails `metric_like` in the template head, pinned on the compiler's account of the concept (`test/CMakeLists.txt:275-278`, `:1027`). Narrowing in brace initialization is the language's own error. The catalogue states each of these as it is, and the owner's ratification (O3) decides whether DoD-19 accepts it.

3. **"Graded signature" has no spelling to reject.** DoD-19 lists it, and DoD-1 says graded signatures are rejected at compile time. `metric<N>` holds a symmetric integer Gram matrix and nothing else (`metric.hpp:74`), and no interface of the core takes a phase. A tag whose `value` is not a `metric<N>` fails `metric_like`. Reading followed: the catalogue records the item as met by `metric_like`'s rejection, with the tests that pin it; the owner rules on it with O3.

4. **The layout keeps no names.** A blade set stores a mask and a sign per blade (`blade_set.hpp:46-56`); the text the user gave `blades<Alg, "...">` is gone once the set is built, and decision 0157 item 3 left names in the type deferred. So `format` cannot print "the oriented names as the layout declares them" (note 0019, 6.10) from the type alone. A planning probe (departure 6) shows what a rule can do: a plane of rigid(3) declared on e423, e431, e412, e321, the names of CONVENTIONS 1.6, prints as e234, e314, e124, e213 under the rule "canonical name for sign +1, first two indices swapped for sign −1". This is O1, the main question of the unit.

5. **CONVENTIONS fixes basis names (C10), and `format` is where users read them.** The printed text names blades, so the rule that chooses a name for a stored orientation is a naming convention under C10. Section 11 proposes a CONVENTIONS 7.2 for the owner to ratify; this plan does not edit CONVENTIONS.

6. **Planning probes, not reproducible as committed (V10).** These statements rest on probes in the planner's scratch area, against the headers of `d68881f`, on g++ 16.2.1 and clang++ 23.1.4 with libstdc++ 16, flags `-std=c++23 -pedantic-errors -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -DGALOOT_CHECKING=1`, each compile through `tools/timed --log` into a scratch log (not `runs.tsv`):
   - `probe.cpp`: a `std::formatter<mv<Alg, B, T>, char>` partial specialization written with public names only (`canonical_name`, `mv`, `blade_set`) compiles and runs on both compilers, and prints a rigid(3) line as `{e41: 1, e42: 2, e43: 3, e23: 4, e31: 5, e12: 6}`, the plane above as `{e234: 0.50, e314: 0.00, e124: 1.00, e213: -2.00}` under `{:.2f}`, and the wedge of two points over `int` as `{e12: 1, e13: 0, e14: 1, e23: 0, e24: -1, e34: 0}`. Its `parse` and `format` must be templates on the context: with `std::format_context&` as the parameter, `std::formattable<Line, char>` is false on both compilers, because the concept checks the formatter against another context type. A scalar without a formatter (`std::complex<double>`, which has none in C++23's library) gives at least three errors from inside `<format>` on both compilers, the first a `static_assert` of libstdc++ ("std::formatter must be specialized for each type being formatted"), none naming the library's own terms.
   - `probe2.cpp`: `std::format("{}", std::map<int, double>{{1, 2.5}, {3, 4.0}})` prints `{1: 2.5, 3: 4}` on both compilers, the standard library's own form for a map. A class derived from `mv` formats once it has `template <> struct std::formatter<Line, char> : std::formatter<mv<...>, char> {};`, and `std::formattable<Line, char>` is then true. A `static_assert` with a literal in `parse` puts the literal on the first error line on both compilers, but the naive form gives four errors (the message twice, from the compile-time and the run-time format string checks, and a deleted formatter of the scalar), so one error is a criterion to meet, not yet shown.
   - `core_only.cpp` against `core_format.cpp` (an includer of `core.hpp`, with and without `<format>`), three compiles each at `-O0`, unpinned on a shared machine: GCC about 0.94 G retired instructions and 76 MiB peak memory without `<format>`, about 3.17 G and 158 MiB with it; Clang about 0.97 G and 116 MiB without, 4.04 G and 194 MiB with it. This is the measured reason for keeping `format.hpp` out of `core.hpp` (P3, and `assert.hpp:34-35`, which already avoids `<format>`).
   - `gaps.cpp`: ten misuses outside the present catalogue, one per compile, lines / error lines and the first error, in R7's table.

   The test author and the implementer repeat each claim they rely on as a test or a committed probe.

7. **WU16 is planned at the same time** (`records/plan-wu16`, no commits at `d68881f`). WU16 depends on WU15 (plan 0001), and M4's exit evidence asks for a zero core diff from the `rga3` model. Anything the model needs from the core to print its objects under CONVENTIONS 1.6's names must therefore land in WU15 (O1). The two plans should be read together before either branches; this plan does not know WU16's choice between type aliases and derived named classes (the WU8a hci reading, F6), which bears on R5.

### Readings

R1. **The catalogue (part 1).** A record, `docs/agent/records/diagnostics/misuse-catalogue.md`, with one row per cause the core rejects at compile time or in checking mode: the misuse as a user writes it, the operation, the message (or the compiler text a test requires), the compile-fail or run-time test names, and whether the test runs on both compilers (each compile-fail test runs in every preset, so GCC presets and Clang presets each run it; the record names the tests that branch on the compiler, as `not_metric_like` does). It groups the rows under DoD-19's nine items and lists the rejections outside them. It is generated by a committed script, `docs/agent/records/diagnostics/catalogue/scan.py`, that reads every `static_assert` literal and `GALOOT_ASSERT` message in `include/galoot/core/` and every `MESSAGE` and `REQUIRE` value in `test/CMakeLists.txt` (with `${semicolon}` read as `;` and the `set(...)` variables of that file resolved), and prints the messages that no test requires. A message with no test is either given one or listed with its reason (an internal invariant, such as `blade_set.hpp:87`). The script is a record tool, run at this unit, at the M4 gate, and at the release audit; it is not a CTest or a merge gate (C12; O9).

R2. **What `format` prints (part 2, after O1 and O2).** `std::format("{}", x)` writes `{name: value, name: value, ...}`: every component of x, zero or not, in the order of x's layout, each as its blade's name, a colon and a space, and its coefficient as the scalar's own formatter writes it. An empty layout writes `{}`. The format specification, as in `{:.3f}`, is handed to the scalar's formatter and applies to each coefficient. Nothing is dropped and nothing is rounded beyond what the scalar's formatter does (C8). The form is the standard library's form for a map (departure 6), which a C++ reader knows, and it cannot be misread the way `2e3` can, which C++ reads as 2000.

R3. **Names (after O1).** Recommended (O1 option B with fallback E): a blade is printed by the name the algebra tag declares for it, when the tag declares one with the layout's orientation; otherwise by its canonical name (1.1), with a leading `-` when the layout stores the blade with orientation −1, as CONVENTIONS 1.6 itself writes "e41 = −e14". A blade a product or `full` generates has orientation +1 and canonical order, so a result prints exactly as `get`'s messages describe it ("names each blade by its indices in increasing order"). The tag declares names by an optional static member, a list in the form `blades<Alg, "...">` reads (proposed `names`, a constant convertible to `std::string_view`), checked at compile time, once per algebra, when `format` first uses it: each entry a valid name in the algebra, no blade twice (V4). A test-local rigid(3) tag that declares CONVENTIONS 1.6's names prints a plane as `{e423: ..., e431: ..., e412: ..., e321: ...}`.

R4. **Where and how.** `include/galoot/core/format.hpp`, in `galoot_core`'s header set (`CMakeLists.txt:18-31`), not included by `core.hpp` (plan 0001 section 3; departure 6's cost). It includes `<format>`, `<string_view>`, `mv.hpp`, and `blade_set.hpp`, and holds the partial specialization `std::formatter<galoot::core::mv<Alg, B, T>, char>` with `parse` and `format` templated on the context (departure 6), the name choice in `galoot::core::detail`, and the rejecting branches. `char` only; `std::print` works through it with nothing added. No `operator<<`, no `wchar_t` formatter, no formatter for `blade_set`, and no doctest `StringMaker` in WU15 (O8). The comment of `mv` (`mv.hpp:484-485`, "The type has no output operator or formatter") changes in the same unit (W12).

R5. **Classes derived from `mv`.** A `std::formatter` specialization matches one type exactly, so a class derived from `mv` gets no formatter from `format.hpp`. A model that uses derived classes gives each one its formatter by inheriting the base's, as departure 6's probe does, which is a line in the model and no core change. `format.hpp`'s comment says so (O7).

R6. **`format`'s own rejections.** Each is a literal on the first error line on both compilers, with one error (the criterion of decisions 0166 and 0173), also when the call is `auto s = std::format("{}", x);`, with `detail::`, `__format`, and `basic_format_arg` forbidden on the first error line:
   - a scalar with no `std::formatter`, tested with a test-local scalar type that has none (not `std::complex<double>`, which gains a formatter in a later standard library);
   - an algebra of more than nine generators, where names have no single-digit form (`blade_set.hpp:80-83`), mirroring `get`'s message;
   - after O1, each fault of a tag's `names`: an entry that is not a name in the algebra, and a blade named twice.

   Proposed texts, for the test author to finalize under the style of note 0019 section 6.9 (no quote characters, the fix after a semicolon): "format: the scalar type has no std::formatter; give it one, as double has", "format: names have one digit per index, so a multivector prints by name only for n up to 9; format its coefficients instead", "format: the algebra tag's names hold a text that is not a blade of the algebra; list names as blades reads them, as in e41", "format: the algebra tag's names name one blade twice; name each blade once". With the scalar's rejection in place, `std::formattable<mv<Alg, B, S>, char>` is true for a scalar S with no formatter, as decision 0033 accepted for functions; the comment says so (O6).

R7. **The gaps (part 3, after O3 to O5).** From `gaps.cpp` (departure 6), lines / error lines, and the first error line in short:

   | Case | Misuse | GCC | Clang | First error |
   | --- | --- | --- | --- | --- |
   | 1 | `Motor{float_line}`, another scalar type | 114 / 1 | 43 / 1 | no matching constructor, with the masks |
   | 3 | `Motor{line_of_another_algebra}` | 114 / 1 | 43 / 1 | as case 1 |
   | 6 | `p + 1.0`, a multivector and a number | 47 / 1 | 19 / 1 | no match for `operator+` |
   | 9 | `mv<notag, layout, double>`, a tag without `value` | 20 / 2 | 10 / 1 | the constraint of `mv` |
   | 2 | `truncate<5>(motor)` | 52 / 5 | 13 / 1 | no matching function |
   | 4, 8 | `line == motor`, `line == float_line` | 30 / 1 | 8 / 1 | no match for `operator==` |
   | 5, 7 | `reverse(3.0)`, `transform(p, 2.0)` | 17 to 41 / 1 | 7 to 19 / 1 | no matching function |
   | 10 | control: `reverse(fl) + l`, catalogued | 13 / 1 | 12 / 1 | the library's literal |

   Recommended (O5): catch-alls for cases 1 and 3 (one rejecting constructor in `mv` for a source of another scalar type or another algebra, with two messages, distinct from the lossy-conversion one at `mv.hpp:613-618`), and for case 6 (`x + s`, `s + x`, `x - s`, `s - x` for a ring scalar s, in `linear.hpp`, whose comment already says "A multivector and a number have no sum", `linear.hpp:40-41`). The WU8b review named case 1 "a common mistake once the public products exist" (F10). Case 9 is O4. Not recommended: case 2 (Clang already says it in 13 lines; GCC's 5 errors are one mistake of a rare kind), cases 4 and 8 (a catch-all `==` would make `std::equality_comparable_with` true for pairs that do not compare, which standard algorithms and test frameworks read, and Clang's 8 lines already name both types by their aliases; the `core.dual_compare_*` tests pin those names), and cases 5 and 7 (one operand, one error, short on both compilers). Proposed texts: "mv: the source has another scalar type; a conversion between layouts keeps the scalar type, so convert the coefficients one by one", "mv: the source is in another algebra; a conversion between layouts keeps the algebra; give both types the same algebra tag", "a + b: a multivector and a number have no sum; make the number a multivector on the blade 1 and add that" (and `a - b:`). Each new catch-all makes a requires-expression or `std::is_constructible_v` answer yes for the misuse; each comment says so, as the existing ones do (`product.hpp:248-251`).

R8. **Interaction with the existing catch-alls.** A new rejecting constructor must not take a call that an existing constructor or catch-all takes: a multivector of the same algebra and scalar on another layout still reaches the converting or the lossy constructor (`mv.hpp:594-618`), one coefficient still reaches the count rules (`mv.hpp:626-657`), and a class derived from `mv` is still copied or converted. The test author pins each with a compile test that must build (accept) beside each compile-fail test (reject), and the mutants of section 6 include constraints made too wide and too narrow.

R9. **`get`'s messages stay as they are.** Decision 0153 item 2 said `get`'s "its layout lists the names it holds" could point at `format` once it exists. A literal cannot hold the call `std::format("{}", x)` (no quote characters, plan 0002 risk 8), and `format.hpp` is not included by `core.hpp`, so a message that points at it would point at a header the user may not have. Recorded, not changed.

### The W2 question

W2 is not amended (decision 0185). The test author commits each group on `feature/core-format--tests` and first compiles it against a scratch stand-in (like departure 6's probes) to show it is well formed and that the mutants of section 6 fail it, and runs `tests.reference_markers` and `tests.golden_facts` on the group (plan 0021 outcome, departure 16). The implementer cherry-picks each group with `git cherry-pick -x` immediately before the code commit it judges; that commit is the single failing commit W2 allows, and its message says so. A test author's commit that fails to build may be combined with its fix only on the terms of decision 0195 item 6 (a build failure, a commit nobody else has fetched, and the reviewer told).

## 2. Definition of done

- The misuse catalogue (R1) and its scan script committed, every message of the core either required by a test or listed with its reason, and the catalogue put to the owner for ratification (O3).
- `include/galoot/core/format.hpp` as R2 to R6 and the owner's O1 and O2 rulings describe, in the header set, not included by `core.hpp`, with `///` comments that state the form, the names, the order, the specification's reach, the rejections, that `std::formattable` answers yes for a scalar without a formatter, and how a derived class gets its formatter. `mv.hpp`'s comment updated (R4).
- The catch-alls of R7 that O4 and O5 admit, each with its comment and its detection consequence stated.
- Tests (doctest-expert role, independent of the implementer): `test/core/format_test.cpp` (labels `core;core.format`), compile-fail tests for every message of R6 and R7, accept tests for R8, and the pin that `core.hpp` alone gives no formatter (criterion 6).
- CONVENTIONS 7.2 and its trace rows, if the owner ratifies section 11's proposal; otherwise the form is stated in `format.hpp` alone and the trace table is unchanged.
- Decisions: the printed form and names (with O1 and O2's rulings), the catalogue and its ratification (O3), and the new catch-alls (O4, O5).
- Records: `records/diagnostics/wu15-messages.md` (each new rejection, before and after, both compilers, lines and error lines, the first error line, as `wu13a-messages.md`); mutants in `records/mutations/0022-format/`; W10 before and after (section 7); the agent costs; this plan's outcome.
- A W5 blind review with the usability checklist, fixes, and a delta review after any non-comment fix to `include/` (W5, decision 0183); the merge of `feature/core-format` into `develop` by the coordinator after P9's checks.

## 3. Acceptance criteria

1. **The catalogue.** Every DoD-19 item has a row with at least one test that runs in a GCC preset and in a Clang preset; the scan prints no message without a test or a listed reason; every row's test name exists in `test/CMakeLists.txt` (checked by the script). The three items of departure 2 and the item of departure 3 are stated as they are.
2. **Names against independent values.** For every oriented blade of every algebra R(n, 0, 0), n from 1 to 9 (2^n masks, each with sign +1 and −1, in a layout of that one blade), the printed name equals the name the test computes from the mask and sign by its own code, sharing none with the library: the canonical index list, with `-` for −1 under fallback E. For a rigid(3) tag that declares names for all sixteen blades, with 1.6's names for the bivectors and trivectors, each of 1.6's blades prints by its 1.6 name, a blade the layout stores with the other orientation prints by fallback E, and a blade the tag does not name prints by fallback E (O1). Exhaustive over a finite set, so V1 kind 1.
3. **The form.** Exact strings: a rigid(3) point, line, plane, and motor over `std::int64_t` and over `double` with values exact in binary (0.5, −2, 3); a product result over `int`, whose blades print in 1.7's order (e12, e13, e14, e23, e24, e34); an empty layout prints `{}`; zero coefficients print; `{:.2f}` and `{:>5}` apply to each coefficient, checked against the scalar's formatter applied to each coefficient alone (a contract test, not independent evidence); `std::print` and `std::format_to` give the same text.
4. **Rejections.** Each message of R6 and of the admitted R7 cases gives one error with its message on the first error line on GCC and Clang, also as `auto r = ...;`, with the internal names of R6 (or, for R7, `detail::`, `component<`, and the masks' `blade_set<` spelling) forbidden on the first error line (`FORBID_ON_FIRST_ERROR`). The R7 table is re-measured after the change and recorded.
5. **Accepted uses unchanged.** The accept tests of R8 build on both compilers; every existing test of `core.mv` and `core.linear` passes unchanged; no expected value or message of an existing test changes, except as O4 rules for `blades`, in its own listed commit (W9).
6. **Outside the umbrella.** A test that includes `<format>` and `core.hpp`, and not `format.hpp`, asserts `!std::formattable<Line, char>`; with `format.hpp` it asserts `std::formattable<Line, char>`. `core.include_scan`, `core.bare_compile`, and the header-set verification (`VERIFY_INTERFACE_HEADER_SETS`, `CMakeLists.txt:108`) pass with the new header.
7. **Derived classes.** A test-local class derived from `mv` formats with the one-line inherited formatter of R5, and the result equals its base's text.
8. **Mutants.** Each mutant of section 6 is killed on gcc-debug and clang-debug by a test, never by a warning; an equivalent survivor is recorded with its reason.
9. **Checks.** Section 7: P8 during work, P9 at each merge, every build and test timed by `tools/timed`, the job pool's status read before and after each heavy run, all seven merge presets and `clang-measure` from new, empty build trees at the final revision of the build inputs on a clean tree, the trace gate, `tools/runlog check`, and the format check passing; no citation of agent records in code, comments, tests, or build files (S11).

## 4. Parts and order

Tests come before the code they judge, with the mechanics of "The W2 question". Small commits; expected-value changes to existing tests in their own listed commits (W9).

0. Claim: this plan merged to `develop` by the coordinator; the owner's answers to section 10 recorded in a decision.
1. **Part 1, the catalogue** (implementer; no core change). The scan script and the catalogue record; any message it finds without a test gets a test from the test author (a test group that passes when committed, since the code exists) or a listed reason. The owner ratifies the catalogue (O3) before part 3 starts; parts 1 and 2 do not wait for it.
2. **Part 2, `format.hpp`** (after O1 and O2).
   1. Test group T2a (test author): criteria 2, 3, 6, 7 and the accept cases, against the ruled names and form.
   2. `format.hpp`, the header set, `mv.hpp`'s comment, CTest registration.
   3. Test group T2b: the compile-fail tests of R6.
   4. The rejecting branches of R6.
   5. If section 11 is ratified: CONVENTIONS 7.2 and its trace rows set to `no` before step 2, then `yes` after it (the trace table's rule), with the trace gate run.
3. **Part 3, the catch-alls** (after O3 to O5).
   1. Test group T3a: the compile-fail tests of the admitted R7 cases and the accept tests of R8.
   2. The rejecting constructor in `mv.hpp` and the scalar sums in `linear.hpp`.
   3. If O4 is yes: test group T3b (the `blades` literal), then the change to `blades` and its helper in `blade_set.hpp`, with the changed expected text of `core.blade_set_blades_not_metric_like` and `core.blade_set_names_zero` in its own listed commit.
   4. The catalogue regenerated with the new rows.
4. **Records**: decisions, diagnostics, mutants, W10, costs, the outcome. Then the blind review, fixes, delta review, and the merge.

Part 2 goes before part 3 because it touches only a new header, so its P8 runs are small, while part 3 changes `mv.hpp`, which every core test includes, and is measured once for W10 at the end. If O1 and O2 wait longer than O3 to O5, the coordinator may swap them; nothing in one part depends on the other.

## 5. Evidence (VVUQ)

| Claim | Evidence | Kind (V1) |
| --- | --- | --- |
| every printed name denotes the stored oriented blade | criterion 2: exhaustive over n ≤ 9, against the test's own name code | 1 (exhaustive) |
| a model's names print as CONVENTIONS 1.6 has them | criterion 2's rigid(3) tag, values copied from 1.6, which is Checked and Confirmed | 3 (independent values) |
| results print in 1.7's order | criterion 3's product case, against the order written in the test from 1.7 | 3 |
| the form, the specification, `std::print` | criterion 3 | contract (V2: a behavior stated where it is declared) |
| `format.hpp` is outside the umbrella | criterion 6 | contract |
| each rejection names its cause in one error | criterion 4, both compilers; `wu15-messages.md` | contract (C9) |
| valid uses still compile | criterion 5 and R8's accept tests | contract |
| the catalogue is complete | the scan script's output at the final revision, committed | mechanical check |
| `format.hpp` costs an includer only what it includes | W10 (section 7) | measurement (V15) |

Exact scalars are compared by exact equality of text (V11). For `double`, the text compared is the formatter's own and only values exact in binary are used, so no tolerance arises (V12 does not apply). Validation (V8): the names of a rigid(3) model against CONVENTIONS 1.6, whose names were read in Lengyel's pages (research note 0005); the readability of the messages and of the printed form has no outside referent beyond the usability checklist and the owner's sampling at the M4 gate (plan 0001 risk 3), and the report says so.

## 6. Mutants

Each on gcc-debug and clang-debug, by a script under `records/mutations/0022-format/` as earlier units did:

- names: a −1 blade printed without its `-`; a −1 blade printed by the tag's name though the tag gives the other orientation; the tag's names ignored; the canonical index order reversed; names printed for n = 10 without the rejection;
- form: zero coefficients skipped; the layout order replaced by canonical order (killed by a named layout that is not canonical); the specification not forwarded; the separator `: ` or `, ` changed; `{}` for an empty layout replaced by nothing;
- rejections: each `static_assert` of R6 and R7 removed (the misuse then compiles, or fails with the compiler's text, and the test that requires the literal fails); each message swapped with another's;
- catch-alls: the new constructor's constraint widened to the same scalar type (an accept test fails as ambiguous or rejected), narrowed to the scalar case only (the algebra case loses its message); the scalar-sum catch-all widened to multivector operands (an accept test of `a + b` fails);
- the scan script: `REQUIRE` values ignored; `${semicolon}` not translated (both make it report false gaps or miss real ones, checked by fixtures in the script's own directory).

## 7. Checks and presets (P8, P9, P13, W10)

Every build and test runs through `tools/timed` after `eval "$(tools/jobpool env)"`, and the pool's status (`tools/jobpool status`) is read before and after each heavy run. The pool is shared: no agent of this unit starts or stops it.

| Point | Selection | Configurations |
| --- | --- | --- |
| Part 1, before each commit | the scan script on the tree; `-L tests` (the `tests.*` checks) and the new or changed tests, if test files change | one (gcc-debug) |
| Part 2, before each commit | `-L 'core\.format\|core\.headers'`, the new and changed tests | one (gcc-debug) |
| Part 3, before each commit | `-L core`: `mv.hpp` is included by every core test (P8, "a component's header") | one (gcc-debug) |
| End of each part, before the next starts | P8's selection for everything the part changed (the same rule P9 gives a merge into a feature branch, should the coordinator split a part onto a subfeature branch) | gcc-debug and clang-debug, since messages and first error lines differ by compiler; lint when a header changed |
| Merge into `develop` | the full suite on gcc-debug, gcc-release, gcc-asan, clang-debug, clang-release, clang-asan, and lint, and `clang-measure`'s full suite (decision 0184), each from new, empty trees at the final revision of the build inputs on a clean tree; the trace gate; `tools/runlog check`; `clang-format --dry-run --Werror` on the changed C++ sources | as listed; P9 requires each compiler, one under sanitizers, `clang-measure`, and the style checks, and the other presets are run as WU13 ran them, so that the merge message reports every preset at one revision |

The merge message names the selection, configurations, outcome, wall time, and any check deferred (P13, decision 0182 item 2).

W10 (the unit changes the core): pinned, three repeats, as plan 0021's records did, before (the branch point) and after (the final revision), instructions, wall time, and peak memory on both compilers for an includer of `core.hpp` alone (`smoke_test.cpp`), `mv_test.cpp`, `linear_test.cpp`, and, after only, `format_test.cpp` and a translation unit that includes `format.hpp` and formats one point. The expectation stated beforehand: an includer of `core.hpp` alone changes by part 3's catch-alls only, within the history's spread; a `format.hpp` includer pays about `<format>`'s cost (departure 6). A change beyond the spread is reported with its cause (P7).

## 8. Review plan (W5)

- **Blind review** by the `blind-reviewer` agent at the Opus level with high effort, in a configuration that shares no context with the authors, given the diff from the branch point, section 3's criteria, and the departures of the outcome, and not the authors' view that it is correct. It applies the usability checklist, since `format.hpp` and the catch-alls are public interface (decision 0160). It reads the catalogue against the headers and reruns the scan, compiles each new compile-fail source on both compilers, and tries its own mutants. Its report is committed as written (decision 0182 item 13).
- **Delta review** of any fix to code in `include/` after the review; a comment-only fix is listed in the review record instead (decision 0183).
- **Optional hci-expert reading** (Opus, high) of the printed form and the new messages before the blind review, as WU8a and WU8b had: the printed text is the most visible output of the library, and the reading is cheap beside a later rename (W13). Recommended; the coordinator decides.
- The test author and the implementer are different agents; the reviewer is neither.

## 9. Risks

1. **The names question blocks part 2.** If the owner takes neither B nor E (O1), the form waits. Part 1 and part 3 do not depend on it.
2. **One error per rejection inside `<format>`.** The probe's naive form gave four errors (departure 6). If one error cannot be reached on both compilers, the departure is recorded with the measured counts and the owner told; the message must still be on the first error line.
3. **A standard library change.** C++26 adds formatters for types the tests might use (such as `std::complex`) and range formatting rules may evolve; the tests use test-local types (R6), and the toolchain record names the library.
4. **`std::formattable` answers yes** for a scalar without a formatter once R6's rejection exists, so generic code that probes it is told yes (decision 0033's trade, extended to a class specialization). Stated in the comment; O6.
5. **The extension point on the tag (O1 option B)** is a new optional member of the tag protocol, a core interface that the owner sets (C18, the M1 decision on signatures' interface). If the owner declines it, a rigid(3) plane prints by fallback E, `{e234: ..., -e134: ..., e124: ..., -e123: ...}`, and the model cannot change that without a core diff (M4).
6. **Shared files and numbers.** `test/CMakeLists.txt`, `CMakeLists.txt` (the header set), `mv.hpp`, `linear.hpp`, `docs/agent/records/convention-trace.md`, `runs.tsv`, and the decision and plan numbers are shared with WU16 and any other unit active at branching; whichever branch merges second merges its parent first (W7), and decision numbers are taken when written.
7. **clang-tidy on a specialization in `std`.** A check such as `cert-dcl58-cpp` may flag the formatter's specialization; a finding not fixed is suppressed at its place with the reason (S4).

## 10. Open questions, with recommendations

Marked **owner** where the constitution or an earlier ruling gives the choice to the owner; the others are the agents' to settle, and the owner may override any of them.

- **O1 (owner). How a blade's name is chosen when printed.** The layout keeps no names (departure 4), and the names are basis names under C10.
  - A. A rule from mask and sign: the canonical name for +1, the canonical name with its first two indices swapped for −1. Every printed name is one `get` reads. Lines of rigid(3) print as 1.6 writes them; planes print as e234, e314, e124, e213, which no published table uses. Needs a new naming rule in CONVENTIONS.
  - B. Names declared by the algebra tag, an optional member listing oriented names (R3), used when the tag names the blade with the layout's orientation; with A or E as the fallback. Prints a model's objects under its published names, which matters because M4 forbids the model a core diff later. Costs one optional member of the tag protocol, its compile-time checks, and two messages.
  - C. Names kept in the layout value. Every blade set would carry text, so two spellings of one blade become two types, every diagnostic that prints a layout grows, and decision 0157 item 3 deferred it. Not recommended.
  - D. The canonical name with the stored sign folded into the coefficient (3 on e41 printed as `e14: -3`). Prints a number the value does not hold. Not recommended.
  - E. The canonical name with a leading `-` for −1 (`-e14: 3`). Reads exactly as 1.6 defines a stored coefficient ("multiplies s e_A"); needs no new naming rule; a name with `-` is not one `get` reads.

  Recommended: **B with fallback E.** B gives users the names they declared and read in the literature, E keeps the fallback a plain reading of the stored data. If the owner prefers no change to the tag protocol now, E alone, with B recorded as a note for the model's plan, knowing that M4's zero core diff then keeps rigid(3) planes printing as `-e134` and the like.

- **O2 (owner). The text form.** Options: the map form `{e41: 1, e42: 0}` (R2); a sum `1 e41 + 0 e42`, which needs a sign test on the scalar to write `- 2 e43`, unavailable for every scalar (a complex number has no order), and reads `2e3` as a number when the space is lost; the coefficients alone, `[1, 0]`, which `std::format("{}", x.coefficients)` already prints and which would leave `format.hpp` without a purpose (C12). Recommended: **the map form**, every component in layout order, the specification applied to each coefficient, and the form ratified as CONVENTIONS 7.2 (section 11).

- **O3 (owner). Ratify the misuse catalogue** of part 1, as DoD-19 requires ("a ratified misuse catalogue"), including departure 2 (three items met by the compiler's own text, by earlier design) and departure 3 (graded signatures met by `metric_like`). Recommended: ratify once part 1's record exists; part 3's rows are added by the same rule and listed in the merge report.

- **O4 (owner). A literal message for a tag without a metric** (gap case 9; the WU8a hci reading, F9, routed to the owner). The first place a user writes a tag is usually `blades<Alg, "...">`, whose template head constrains `metric_like` (`blade_set.hpp:603`). Option: constrain the variable template by a weaker concept and assert the rest in its helper, with a literal such as "blades: an algebra tag holds its metric as static constexpr value, as in struct rga3 { static constexpr auto value = rigid<3>(); }". STYLE S7 allows that pattern for a class template only, so this needs section 11's S7 proposal; two existing tests change their expected text (W9). Recommended: **yes, for `blades` only**, if the owner ratifies the S7 sentence; the class template `mv` keeps its constraint, since a weaker head there would make `typename mv<int, ...>` well formed. Otherwise recorded as a note: model users never write a tag (WU16).

- **O5. The catch-alls of R7.** Recommended: cases 1, 3, and 6, for the measured diagnostics and the review's prediction; not cases 2, 4, 5, 7, 8, for R7's reasons.

- **O6. `format`'s rejection of a scalar without a formatter** gives a literal and makes `std::formattable` answer yes (risk 4). The alternative, constraining the specialization on `std::formattable<T, char>`, answers honestly and gives the library's errors (at least three, departure 6). Recommended: **the literal**, within decision 0033's trade, stated in the comment.

- **O7. Derived classes.** Recommended: no core support; a model inherits the base's formatter in one line (R5, probe). Revisit if WU16 adopts derived named classes and the line proves a burden.

- **O8. What `format.hpp` leaves out**: `operator<<` (an `<ostream>` include for a form `std::print` and `std::format` already give), `wchar_t`, a formatter for `blade_set`, and a doctest `StringMaker` for test failure output (which would add `<format>`'s cost to every test that includes it). Recommended: **all left out** (C12), each recorded as a note with its would-be user.

- **O9. The catalogue's scan as a record tool, not a CTest or a merge gate.** Recommended: a record tool, run at this unit, the M4 gate, and the release audit. The owner may promote it to a gate like the convention trace if the catalogue drifts.

## 11. Proposals for the owner (governance and conventions; not edited here)

1. **CONVENTIONS 7.2, text form** (with O1 B+E and O2), proposed text: "7.2 With `<galoot/core/format.hpp>` included, `std::format("{}", x)` writes `{name: value, ...}`: every component of x, zero or not, in the order of x's layout. A blade is named as the algebra tag names it when the tag declares a name with the layout's orientation, and otherwise by its canonical name (1.1), preceded by `-` when the layout stores it with orientation −1, as e41 = −e14 is written `-e14`. A format specification applies to each coefficient. (Proposed)" With trace rows such as "a blade stored with orientation -1 and no declared name prints as its canonical name preceded by -" and "format writes every component in the order of the layout".
2. **STYLE S7** (with O4): "A class template may constrain a parameter by a weaker named concept ..." becomes "A class template or a variable template may ...", so that `blades` can carry a literal for a tag without a metric.
3. **W7's branch table** has no row for a branch that holds only a plan or records (`records/plan-wu15`, `records/plan-wu16`, `records/m3-acceptance` are in use). Proposed row: "`records/<name>`: a plan, a decision record, or other records only, branched from `develop` and merged into it; any contributor, agents included."

## 12. Agent cost estimate

Recorded in `docs/agent/records/agent-cost.tsv` as each agent finishes. For comparison (agent-cost.tsv): WU8a's hci reading 333k tokens, WU8b's 222k; WU13a's test author 464k; blind reviews 210k to 400k; the WU13 planner 322k.

| Part | Role | Model | Estimate |
| --- | --- | --- | --- |
| 1 | implementer: scan script, catalogue record (cpp-expert role) | Opus, high | 120k to 200k tokens |
| 1 to 3 | test author (doctest-expert) | Opus, high | 300k to 420k |
| 2, 3 | implementer: `format.hpp`, catch-alls, records (cpp-expert role) | Opus, high | 350k to 500k |
| 2, 3 | hci-expert reading, optional | Opus, high | 180k to 250k |
| all | blind reviewer (W5, usability checklist) | Opus, high | 250k to 350k |
| all | delta reviewer, if needed | Opus, high | 150k to 220k |

Size S, as plan 0001 has it, at its upper end: about 1.2M to 1.7M tokens without the hci reading, 1.4M to 1.9M with it. This planner's own cost is recorded by the coordinator from the harness report.

## 13. Notes for later units

- The knowledge base has no note on writing a `std::formatter` for a library type (the context templates of departure 6, the map form, the one-error question). The implementer may draft one if the owner permits writing there.
- If decision 0157 item 3 (names in the type) is ever adopted, O1's option B becomes unnecessary for named types and the tag's names would serve only generated layouts.
- A `layout_names` function (the WU8a hci reading, F7) remains without a caller; `format` prints names with values, which covers the run-time need.

## 14. Outcome

In progress (2026-11-08). Parts 1 and 2 are done on `feature/core-format`; part 3 has not started; nothing is merged into `develop`.

Done:
- Part 1: `records/diagnostics/catalogue/scan.py` (with fixtures and `--self-test`), `records/diagnostics/misuse-catalogue.md`, and the scan's output. Of 143 messages, 123 are pinned on both compilers; the remaining 20 are maintainer guards or self-checks of generated plans, each with its reason. The one user-facing message without a test, `GALOOT_CHECKING must be 0 or 1`, got `core.assert_checking_value`.
- Part 2: `include/galoot/core/format.hpp`, with T2a (`core.format`, `core.format_outside_core`) and T2b (14 `core.format_*` compile-fail tests), each cherry-picked with `-x` right before the code it judges; the `mv.hpp` comment; CONVENTIONS 7.2 and the STYLE S7 sentence as drafts; and the four 7.2 trace rows, now `yes`.
- Checks: gcc-debug and clang-debug, `-L 'core\.format|core\.headers'`, 18 of 18 each; a targeted lint build (the core's header checks, both format test programs, format-check); the trace gate. Each rejection gives one error line on both compilers.

Departures:
- The scan reads CTest's JSON listing of a configured tree for each compiler, not `test/CMakeLists.txt` (R1). The listing has variables, loops, and compiler branches already resolved. Section 6's "`${semicolon}` not translated" mutant becomes "character 1 not read as `;`".
- Criterion 2 uses one layout per grade, with signs alternating, not one layout per blade. It still covers all 2044 oriented blades, each with both signs.
- `ONE_ERROR` does not catch a message printed twice. One error per case was checked by hand compiles, not by a test.

Readings the tests now pin:
- a scalar stored with orientation -1 prints as `-1`;
- tag names may be separated by runs of spaces;
- a `names` member that does not convert to `std::string_view` is not read;
- an empty `names` list is valid.

Next: the owner confirms the 7.2 and S7 wording and ratifies the catalogue (O3). Then part 3 (T3a, the catch-alls in `mv.hpp` and `linear.hpp`, T3b with the `blades` change), the catalogue regenerated, and step 4's records: section 6's mutation scripts under `records/mutations/0022-format/`, `wu15-messages.md`, W10, the decisions, and the costs. Then the hci reading, the blind review, and the merge.
