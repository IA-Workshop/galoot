# 0016 WU8: the multivector type, its scalars, named access, and applying plans

- Status: claimed 2026-11-01 (planner: Claude, agent, Opus 5.5, cpp-expert role, with the hci-expert and math-expert role files for the interface and the mathematics). Revised 2026-11-01 for the owner's rulings ("proceed as recommended") on this plan's questions and on research note 0019's (section 2). Not started.
- Unit: WU8 of plan 0001 (`mv.hpp`), in two parts run one after the other: WU8a on branch `feature/core-mv-a` (worktree `.claude/worktrees/core-mv-a`), then WU8b on branch `feature/core-mv-b` (worktree `.claude/worktrees/core-mv-b`), branched from `develop` after WU8a merges. Each part has its own W5 review and merge. WU8b is claimed by a dated line in this plan when WU8a has merged, as plan 0013 did for WU7b (decision 0158).
- Decision numbers 0150 to 0159, all assigned (section 5, "Both parts"). The owner's rulings are recorded in 0150, 0151, 0152, 0153, 0155, 0157, 0158, and 0159 (commit `daf69d8`); 0154 and 0156 are the implementers'.
- Depends on: WU7 (plan 0013, merged: `product_plan` for the four binary operations and `unary_map` for the five unary maps, typed onto canonical result sets), Spike F (research note 0012), Spike C (note 0010) with decision 0033, and research note 0019 (API design, its spike under `docs/agent/records/spikes/api-design/`). Sequenced after plan 0015 for one commit (section 1, item R4).
- Goal: the type a user holds. `mv<Alg, Blades, T>` stores one coefficient of a scalar type T on each oriented blade of a blade set; it is constructed from exactly one value per component and is zero by default; the scalar concepts say which T are accepted; the type rejects a blade set that is wrong for its algebra; `get<"e41">(x)` reads and writes a coefficient by its oriented name; `==` compares exactly; a plan or map is applied to stored coefficients with straight-line code whose coefficients are exact in the scalar; a conversion between blade sets is explicit, and the lossy one is `truncate<Type>(x)` or `truncate<Blades>(x)`, checked in checking mode for exact scalars. DoD-7 holds across five scalars. Blade sets print shorter in diagnostics and are spelled with the algebra, `blades<rga3, "...">`.

## 1. Departures, ambiguities, and owner questions (C20)

### Departures and open items after the owner's rulings

R1. **A ruling that conflicts with ratified text, not edited (owner question O1).** The ruling on question 7 (`truncate` checks exact scalars only, decision 0155) conflicts with the clause of CONVENTIONS 4.5 that says checking mode verifies the dropped part "within the operation's forward error bound for a rounding scalar". The ruling's 4.5 edit covers only `truncate<Type>`, so that clause stands. Proposed wording, for the owner to ratify: "... and in checking mode verifies that what it drops is zero when the scalar is exact. An operation that drops part of its own result, such as `transform`, verifies on a rounding scalar that the dropped part is within that operation's forward error bound (decision 0010)." Until it is ratified, WU8b implements the exact-scalar check and does not merge the rounding-scalar behavior of criterion 16 (it halts on that item under W8 if the wording is still open when everything else is done).

R2. **A second text the ruling does not cover (owner question O2).** PERFORMANCE P5 says "Values are trivially copyable aggregates (STYLE S8)". After the S8 amendment, multivectors are not aggregates. Not edited. Proposed: "Values are trivially copyable (STYLE S8)".

R3. **A precision in the S8 amendment.** The ruling's words, "trivially copyable structural values", cannot hold for every scalar DoD-7 requires: `std::complex<double>` is trivially copyable but not structural (its members are private). The applied text says "whenever their scalar type is trivially copyable and structural" (decision 0150 item 5). Nothing else in the ruling's text was changed.

R4. **The C array inside `blade_set` (ruling H2) has three costs, none a reason to refuse it.** An array of size zero is not C++, so `blade_set<0>` becomes a specialization holding `std::array<oriented_blade, 0>`. Tests that use the `std::array` interface of the member (`.at(i)`, about 30 places; `blades == expected`, 6 places) are migrated. And constant evaluation reads a C array without libstdc++'s checked subscript, so the step counts of plans may move, which plan 0015's canary (`core.plan_cost_model`, exact thresholds) is built to detect. So the C-array commit lands only after plan 0015 has merged into `develop`, and if the canary fails on it, the moved thresholds are reported with their cause and refitted by plan 0015's documented procedure in a separate, listed commit (W9), not silently.

R5. **`blades<Alg, "...">` replaces `blades<N, "...">` (ruling H2) rather than standing beside it.** A variable template cannot be overloaded on whether its first argument is a type or a number, so keeping both would need a second name, which C12 and "one name, one meaning" argue against. About 50 uses in the tests and compile-fail sources move to algebra tags with no expected value changed, plus whatever new files have landed by then (plan 0015's canary units use `full<4>` and no `blades<N>`, as read at its branch head `26cc06e`). `full<N>` and `of_grade<N, K>` keep the count: they take no names and need no algebra. `blade_set.hpp` then includes `metric.hpp` (decision 0157 supersedes decision 0090 item 4).

R6. **The rename of ruling H3 is split between now and WU9 (decision 0159).** Now: CONVENTIONS 7.1 names `geometric_product` and `geometric_antiproduct` (version 1.6), and the trace table's two unimplemented facts that spelled `gp` and `antigp` use the new names. In WU9: the enumerators `BinaryOperation::gp` and `::antigp` become `::geometric_product` and `::geometric_antiproduct`, with every use migrated in one listed commit, in the unit that creates the public functions. WU8 renames nothing in `plan.hpp`, which plan 0015 edits; WU8's own tests use the present enumerator names and are migrated in WU9 with the rest. Test-only helper names and the golden namespaces (`gp_terms`, `gp_tables`) are not renamed.

R7. **Consequences of the rulings for the design, made here.**
   - The counted construction of ruling H1 is written as K parameters of type T, one per component, inherited from a small storage base, because a constructor template cannot both accept integer literals and reject a narrowing variable (planning probe 2, section 11). Brace initialization then keeps exactly the narrowing rule an aggregate had.
   - Because the type is no longer an aggregate, the conversion between blade sets becomes an `explicit` constructor of the target. So `Motor{line}` now works beside `Motor(line)` and `static_cast<Motor>(line)`, and the brace-form pitfall of the first version (a message about `double`) is gone.
   - Implicit conversions, lossy or not, are rejected with the compiler's own message, because S8 keeps every single-argument constructor `explicit`. A non-explicit catch-all would give a Galoot message there, but would break S8.
   - Under ruling H4, `mv` constrains its scalar by `std::semiregular` and asserts `ring_scalar` in its body, so a bad scalar gives one error with a literal message, not GCC's four.

R8. **Not ruled on, left as a proposal.** Research note 0019's reviewer checklist (its section 7) is not adopted. The hci-expert reading (section 12) may use it as a guide; the W5 reviews are not judged against it.

R9. **Planning evidence, uncommitted.** Probe 2 of this revision (section 11, item 5) appended 14 rows to the main checkout's `runs.tsv`. They are not part of the revision's commits, because that file also holds other workstreams' uncommitted rows. Probe 1's 44 rows were committed by the coordinator in `4a7898f`. Both probes' figures are planning values, not reproducible as committed (V10).

### The first version's owner questions, as answered

1. Two parts, with named access in WU8a: ruled as recommended (decision 0158). The blade-set changes of ruling H2 join WU8a.
2. WU8 writes the catch-alls of its own public functions: ruled as recommended (decision 0158). WU15 keeps `format.hpp` and the catalogue-wide review. For the type itself, under ruling H4, a bad scalar is reported by a `static_assert` in the body with a literal message, so DoD-7's "rejected with a message in the terms of the algebra" is met with a literal.
3. An explicit conversion that drops nothing: ruled as recommended (decision 0155). It is now a constructor of the target (R7).
4. No default scalar in the core: ruled as recommended (decision 0150).
5. Zero by default: ruled as recommended (decision 0150), together with the counted construction of ruling H1.
6. Coefficient exactness through `std::numeric_limits`: ruled as recommended (decision 0151).
7. `truncate` checks exact scalars only: ruled as recommended (decision 0155); see R1 for the conflict with CONVENTIONS 4.5.
8. Exactness by `std::numeric_limits<T>::is_exact`: ruled as recommended (decision 0151).
9. `get` rejects the opposite orientation with its own message: ruled as recommended (decision 0153).
10. `truncate<Blades>` only: superseded by ruling H5; both `truncate<Type>(x)` and `truncate<Blades>(x)` exist (CONVENTIONS 4.5, version 1.6).
11. Keep `blades<N, ...>`: superseded by ruling H2; `blades<Alg, ...>` replaces it (R5).

### Readings

12. `apply` stays in `detail` under the roadmap's name; WU8 adds no public product. WU9 and WU10's public functions are one-line callers of it. It shares a spelling with `std::apply`, so every call is qualified (note 0019, section 6.5, agrees).
13. Equality. A defaulted `operator==` compares two multivectors of the same type coefficient by coefficient. By the language rule for defaulted comparisons, it is deleted when T has no `==`. STYLE S13 ("each overloaded operator is a one-line alias of a named function") is read as covering the operators of CONVENTIONS 7.1, which `==` is not. Multivectors on different blade sets, algebras, or scalars are not comparable without an explicit conversion. With a floating scalar, `==` is IEEE equality (−0.0 equals +0.0; a NaN equals nothing), and the comment says so. A tolerance comparison, when V12 needs one, will be a named function with a named tolerance, never an overload of `==` (note 0019, section 6.8).
14. DoD-7 in M2. WU8b runs the same plans over all five scalars with inputs for which every product and every partial sum is an integer of magnitude below 2^24, a bound proved in the test's comment. So every scalar's result is exact and is compared with `==` against the exact reference. Tests with nonzero tolerances wait for M3.
15. The test rational lives in `test/support/`, outside the core (DoD-5, decision 0151), separate from WU5's core rational (M5).
16. Repeated masks. `mv` rejects a blade set that lists one mask twice, in either orientation, and checks signs and range, once per set, in its body (decision 0092, option 3). The direct form comes first: compare every pair, O(K²). If measurement (criterion 9) shows it lowers a ceiling a present use reaches, test canonical order first and compare pairs only otherwise, by a decision. `index_of` keeps its own checks (decision 0093).
17. Result sets. `apply` writes onto the plan's or map's canonical result set (C6, DoD-4). A second form writes onto a given target that holds every blade of that result; the conversions and `truncate` use it as the identity map, and WU10 can use it so that `reverse` of a Line is a Line. What the public unary operations return is WU10's choice.
18. Evaluation order and the first term (V14). Each term contributes c · (x_a · y_b), with c = ±1 applied as addition or subtraction. Each output's first term is assigned rather than added to zero, because under IEEE semantics 0.0 + x cannot be simplified (planning probe 1). `ring_scalar` requires unary minus for it. The `///` comment of `apply` states both.
19. M2's "first assembly read" is taken in WU8b (criterion 24), not DoD-24 in full, which stays with WU17.
20. Names, for the hci-expert reading to confirm or change in decision 0150's appended section:
   - the type `mv`, its storage `coefficients`, and the members `algebra_type`, `scalar_type`, and `layout`;
   - the concepts `ring_scalar` and `has_blade` (named in decision 0033), `exact_scalar`, and `multivector`;
   - the functions `get` and `truncate`.

   Each member exists only if a present caller uses it (S16).
21. Message style, adopted from note 0019 section 6.9 as this plan's practice (not a ruling):
   - one mistake gives one error;
   - the message starts with the operation as written, then a colon (`mv:`, `get:`, `truncate:`, `conversion:`);
   - the mistake is stated in the algebra's terms, never in masks or positions;
   - the fix comes after a semicolon;
   - a number is printed through a numeric condition (decision 0152);
   - each cause has its own message;
   - no message has a final period or quote characters.
22. Overlap with work in flight (W7). Plan 0015 (`feature/plan-limits-certified`, closed on its branch at `26cc06e`, not yet merged) changes `plan.hpp`'s `///` comments, adds blocks to `test/CMakeLists.txt` and canary units under `test/core/`, moves scripts into `docs/agent/snippets/`, and appends to `runs.tsv`.
   - WU8 does not edit `plan.hpp` or `docs/agent/snippets/`.
   - WU8a edits `blade_set.hpp`, which plan 0015 does not touch, but the edit may move the canary's thresholds (R4).
   - Both units add blocks to `test/CMakeLists.txt` and rows to `runs.tsv`; those are adjacent edits, resolved by merging `develop` in before review.
   - WU8 adds `mv.hpp` to `CMakeLists.txt`'s header set and to `include/galoot/core.hpp`, which plan 0015 does not touch.

## 2. Owner rulings (2026-11-01)

The owner ruled "proceed as recommended" on this plan's eleven questions and on research note 0019's five (H1 to H5). Each ruling, its record, and what it changed here:

| Ruling | Record | Governance edit | Changed in this plan |
| --- | --- | --- | --- |
| Questions 1, 2: two parts, named access in WU8a; WU8 writes its own catch-alls | 0158 | none | Section 5; the blade-set changes join WU8a |
| Questions 4, 5 and H1: no default scalar; zero by default; one value per component, a wrong count is one error printing "3 == 4" | 0150 | STYLE S8 amended, status line added | Sections 4, 5, criteria 1 to 3, tests E1 to E3 and E15; R3, R7 |
| Questions 6, 8: exactness through `std::numeric_limits` | 0151 | none | Unchanged from the first version |
| H4: numeric catch-alls; a class template may check in its body under a weaker constraint | 0152 | STYLE S7 amended | `mv`'s scalar check; the count constructor; test E8 |
| Question 9: opposite orientation rejected with its own message | 0153 | none | Message wordings in sections 4 and 8 |
| Questions 3, 7 and H5: explicit lossless conversion; `truncate` checks exact scalars only; `truncate<Type>` beside `truncate<Blades>` | 0155 | CONVENTIONS 4.5 (version 1.6) | Conversion as a constructor; criteria 15, 16; tests F9, F10; R1 |
| H2: blade sets in a C array; `blades<Alg, "...">` | 0157 | none | New WU8a commits and criteria 11a, 11b, tests E16, E17; R4, R5 |
| H3: `geometric_product`, `geometric_antiproduct` | 0159 | CONVENTIONS 7.1 (version 1.6); two trace facts | R6; deferred list |
| Note 0019's reviewer checklist | not ruled | none | R8 |

## 3. Background for the reader

A multivector of a named type stores one coefficient per blade of its blade set, in the set's order, and a coefficient on an oriented blade s e_A multiplies s e_A (CONVENTIONS 1.6). For example, a rigid(3) line stores six numbers on (e41, e42, e43, e23, e31, e12) = (−e14, −e24, −e34, +e23, −e13, +e12). The coefficient written on e41 multiplies −e14, so a published formula can be copied without a hidden negation.

A plan (plan 0013, section 2) is the list of terms (a, b, out, c) of a product of two blade sets, typed onto a canonical result set R. Applying it to x and y computes, for each output position, the sum over its terms of c · x_a · y_b. A worked example in rigid(3), with p = (1, 2, 3, 1) and q = (4, 5, 6, 1) on (e1, e2, e3, e4):

- The geometric product lands on R = (1, e12, e13, e14, e23, e24, e34).
- The scalar part is p1 q1 + p2 q2 + p3 q3 = 32, because e4 squares to 0.
- Each bivector e_ij gets p_i q_j − p_j q_i, so the product is (32, −3, −6, −3, −3, −3, −3).
- Converted to a Line, the bivectors read (e41, e42, e43) = (3, 3, 3), which is the direction q − p, and (e23, e31, e12) = (−3, 6, −3), which is the cross product (p1, p2, p3) × (q1, q2, q3).

The reordering and the signs of the Line's layout come from the oriented names alone. These are planning values; the test recomputes them (section 8, F3).

`truncate<Line>(x)` keeps the blades of x that a Line holds and drops the rest. In checking mode, for an exact scalar, it verifies that each dropped coefficient is zero (CONVENTIONS 4.5). For example, the motor (s, l, a) on (1, e41, ..., e12, e1234) truncated to a Line keeps l and is checked to have s = a = 0.

## 4. The user-facing surface

These snippets show what a first-time user meets. They are examples in this plan only; when user documentation is written, its examples are compiled and run as tests (W12). The diagnostics in comments are the proposed literals; the counted-constructor and conversion lines are as planning probe 2 printed them for the probe's wording.

Declaring a type, making a value, and reading and writing it by name:

```cpp
#include <galoot/core.hpp>

namespace gc = galoot::core;

// An algebra is a tag type that carries its metric: rigid(3) is R(3,0,1).
struct rga3 {
  static constexpr auto value = gc::rigid<3>();
};

// A line stores its direction on e41, e42, e43 and its moment on e23, e31,
// e12; each name is the product of its generators in the order written.
inline constexpr auto line_layout = gc::blades<rga3, "e41 e42 e43 e23 e31 e12">;
using Line = gc::mv<rga3, line_layout, double>;

Line axis{0.0, 0.0, 1.0, 0.0, 0.0, 0.0};  // the z axis through the origin
double vz = gc::get<"e43">(axis);         // 1.0
gc::get<"e23">(axis) = 2.0;               // write a coefficient by its name
Line none;                                // every coefficient zero
bool moved = axis != Line{0.0, 0.0, 1.0, 0.0, 0.0, 0.0};  // exact comparison

// Line partial{0.0, 0.0, 1.0};
//   error: static assertion failed: mv: give one value for each component,
//   in the order the layout lists them
//     • the comparison reduces to ‘(3 == 6)’       (GCC)
//   note: expression evaluates to '3 == 6'          (Clang)
```

Moving between blade sets:

```cpp
inline constexpr auto motor_layout =
    gc::blades<rga3, "1 e41 e42 e43 e23 e31 e12 e1234">;
using Motor = gc::mv<rga3, motor_layout, double>;

Motor as_motor{axis};                       // drops nothing; also Motor(axis)
Line back = gc::truncate<Line>(as_motor);   // drops 1 and e1234
Line same = gc::truncate<line_layout>(as_motor);  // the same, by blade set

// Line lost{as_motor};
//   error: static assertion failed: conversion drops blades of its input:
//   it is lossy, so request it with truncate<Type>(x)
// Motor implicit = axis;
//   error from the compiler: no viable conversion (conversions are explicit)
```

What other misuse looks like:

```cpp
gc::get<"e14">(axis);  // get: the type stores this blade with the other
                       // orientation; use the name its layout gives it and
                       // negate the value
gc::get<"e1">(axis);   // get: the type has no blade of this name; its layout
                       // lists the names it holds
gc::get<"e5">(axis);   // get: the name has an index above n, the number of
                       // generators
gc::mv<rga3, line_layout, std::string> s;
                       // mv: the scalar type must support +, -, *, unary -
                       // and construction from an integer coefficient
float f = 0.1f;
double d = 0.1;
gc::mv<rga3, gc::blades<rga3, "e1 e2">, float> v{d, f};
                       // the compiler's narrowing error for d, as for any
                       // brace initialization
```

How each choice reads to a first-time user, judged by the hci-expert role file's criteria (the names a user types, the examples copied, the errors read):

| Element | What the user sees | Judgment, and what remains |
| --- | --- | --- |
| `mv<Alg, Blades, T>` | Three arguments, algebra first, no default scalar. Clang prints `mv<rga3, line_layout, double>` through the alias; GCC prints the set's value, after ruling H2 as `oriented_blade [6]{...}` without libstdc++'s wrapper | The algebra comes first in mixed-algebra diagnostics (note 0019, case 9). Names as written in the type are not adopted (decision 0157 item 3) |
| `blades<rga3, "...">` | The algebra, not a count; a name beyond the algebra's n is rejected by the parser's message | No repeated n. `full<N>` still takes a count, because it names no blades |
| Construction | `Line{...}` with exactly six values, in layout order; integer literals accepted; a narrowing variable rejected; a wrong count is one error that prints the counts | The silent zero fill of the aggregate is gone (note 0019, section 5.2). `std::is_constructible_v<Line, double, double, double>` is true because of the catch-all (decision 0033's documented consequence) |
| `Line l;`, `Line{}` | Zero | No indeterminate values |
| `get<"e43">(x)` | A free function, found unqualified; reads and writes; six distinct messages | Names serve n up to 9; above that, coefficients are reached by position |
| `==`, `!=` | Exact, same type only | Across layouts the compiler's "no match for operator==" names both types |
| `Motor{line}` | Explicit; all three spellings work | Implicit conversion is the compiler's error, which names both types |
| `truncate<Line>(m)` | The target by its type, as `static_cast` and `gsl::narrow` take it; or by its blade set | "truncate" drops components, not digits; the `///` comment says so (note 0019, section 6.6) |
| A wrong scalar | One error with a literal message (ruling H4) | Generic code asking whether `mv<A, B, std::string>` names a type is told yes; the comment says so |

## 5. Content

New header `include/galoot/core/mv.hpp`, in `galoot::core` with its helpers and the kernel in `galoot::core::detail`. It includes `plan.hpp` and standard headers only, and none of `<complex>`, `<string>`, or `<algorithm>`. It joins the header set in `CMakeLists.txt` and `include/galoot/core.hpp`. Names as in reading 20, messages as in reading 21.

### WU8a

Blade sets (ruling H2, decision 0157), first, each in its own commit with the migration of its uses listed and no expected value changed:

- `blades<Alg, Names>` in place of `blades<N, Names>`, with `Alg` constrained by `metric_like`; `blade_set.hpp` includes `metric.hpp`. The parser's messages are unchanged. One new rejection: a first argument that is not `metric_like`.
- `blade_set<K>` holds `oriented_blade blades[K]`, with the clang-tidy suppression and its reason beside it. `blade_set<0>` is a specialization holding `std::array<oriented_blade, 0>`. The `///` comment states what the member supports: range-for, subscripts, conversion to `std::span`, and `==` of whole sets. This commit lands after plan 0015 has merged (R4); `blades<Alg>` does not wait.

The type (decisions 0150, 0151, 0152):

- `ring_scalar<T>`: `std::semiregular<T>`; `a + b`, `a - b`, `a * b`, and `-a` convertible to T; then T constructible from `coefficient_type`. `T{}` being zero and the ring laws are documented requirements. Equality is not required, so that a lane-pack scalar is accepted.
- `exact_scalar<T>`: `ring_scalar<T>` and `std::numeric_limits<T>::is_exact`.
- `detail::coefficient_storage<T, std::index_sequence<I...>>`: public `std::array<T, sizeof...(I)> coefficients{}`, a defaulted default constructor, a constructor `explicit(sizeof...(I) == 1)` taking `component<T, I>...` (an alias of T), declared only when `sizeof...(I) > 0`, and a defaulted `==`.
- `mv<Alg, Blades, T>`, `template <metric_like Alg, blade_set Blades, std::semiregular T>`, deriving publicly from the storage and inheriting its constructor; `algebra_type`, `scalar_type`, and `layout` as present callers need them.
  - A constructor template for any other number of values, except none and except one multivector argument, whose only content is `static_assert(sizeof...(A) == component_count, "mv: give one value for each component, in the order the layout lists them")`. A named `component_count` makes Clang print "'3 == 6'" rather than the blade set (probe 2).
  - In its body, `static_assert`s with literal messages: `ring_scalar<T>` ("mv: the scalar type must support +, -, *, unary - and construction from an integer coefficient").
  - Also in the body, each reading a variable template computed once per (n, set): every mask names only e1 to en ("mv: the blade set names a generator beyond e_n"); every sign is +1 or −1 ("mv: the blade set holds a sign other than +1 or -1"); no mask appears twice ("mv: the blade set holds one blade twice, perhaps in both orientations").
- `multivector<X>` and `has_blade<X, Name>` (data only, decision 0033); `has_blade` is false, with no error, for an invalid name.
- `get<Name>(x)` (decision 0153): one function template on a forwarding reference, returning the coefficient with the value category of its argument; the position is a `constexpr` constant inside the function. The name parameter reuses `detail::name_list` and `parse_name`. Form-P catch-alls, one per cause:
  - absent: "get: the type has no blade of this name; its layout lists the names it holds";
  - other orientation: "get: the type stores this blade with the other orientation; use the name its layout gives it and negate the value";
  - index above n: "get: the name has an index above n, the number of generators";
  - not a name: "get: a name is 1 or e followed by generator indices, as in e41";
  - more than one name: "get: give one blade name";
  - wide algebra: "get: names have one digit per index, so they are read only for n up to 9".
- `test/support/rational.hpp` (decision 0151): `test_support::rational`, int64 numerator and positive denominator in lowest terms, the ring and field operations and `==`, all `constexpr`, every intermediate guarded against overflow (a non-`constexpr` call fails a constant evaluation; a message and `std::abort` at run time); `std::numeric_limits` specialized with `is_specialized` and `is_exact` true. Its tests are `test/support/rational_test.cpp`, CTest `support.rational`.
- `test/core/bare_user.cpp` gains a Line declared with `blades<Alg, ...>`, constructed, read by `get`, and compared (DoD-6).

### WU8b

- `detail::apply<Op>(x, y)` for a `BinaryOperation` and `detail::apply<Op>(x)` for a `UnaryOperation`, for one algebra and one scalar type. The result is an `mv` on the plan's or map's result set.
  - The body expands an index pack over the plan's terms, one statement per term, with the coefficient, the positions, and the first-term flag as constants. The flags are computed in one pass with a "seen" array.
  - The direct loop over the terms is the reference the tests compare with (C3).
  - Because `mv` now has a K-parameter constructor, the result is built default-initialized (zero) and then written; the assembly read checks that the zero fill folds away (criterion 24).
- The coefficient check (decision 0151): a variable template per (plan, T), and a `static_assert` in `apply` with the message "the scalar type cannot hold one of this product's exact coefficients".
- `detail::apply` onto a given target set (reading 17).
- The conversions (decision 0155):
  - a constructor `template <blade_set From> explicit mv(const mv<Alg, From, T>&)`, constrained to sources whose blades the target all holds; each coefficient takes the sign s_source · s_target;
  - an `explicit` form-P catch-all constructor for the other sources, with the message "conversion drops blades of its input: it is lossy, so request it with truncate<Type>(x)";
  - a source of another algebra or scalar reaches neither, and the compiler reports no matching constructor.
- `truncate<To>(x)` for a blade set and `truncate<Target>(x)` for a type (CONVENTIONS 4.5, version 1.6), overloaded on the kind of the template parameter. Behavior:
  - each blade of the target that x holds takes its coefficient with the sign s_source · s_target, and every other blade of the target is zero;
  - for an `exact_scalar`, each dropped coefficient is checked by `GALOOT_ASSERT` ("truncate: a dropped coefficient is not zero"), in every constant evaluation and at run time when `GALOOT_CHECKING` is 1;
  - for a rounding scalar nothing is checked, and the comment says why (R1 bears on this);
  - a type that is not a multivector of x's algebra and scalar is rejected by a catch-all: "truncate: the target must be a multivector type of the same algebra and scalar type; truncate changes only which blades are held".
- Trace table: the 4.5 row ("truncate checks that the part it drops is zero") is set to `yes` in the commit that implements it, with a test whose marker states the fact.

### Both parts

- Every public declaration has a `///` comment stating what it requires, guarantees, and preserves, with the measured limits where they bind, and cites no record (S11).
- Decisions: 0150 to 0153, 0155, 0157 to 0159 record the rulings (written). The implementers write 0154 (the kernel: expansion, first term, folding, evaluation order, the exactness check, the target-set form) and 0156 (limits and costs of both parts, WU8b in an appended section). Implementation detail of a ruled topic goes in a dated section appended to that topic's record: the set checks to 0150, the final messages to 0153, the conversion details to 0155.
- Mutation scripts and results under `docs/agent/records/mutations/0016-mv-a/` and `0016-mv-b/`, keeping lessons 0060 to 0064.

## 6. Definition of done

For each part:

- The header compiles alone and in the umbrella header.
- Every test of section 8 for that part passes on all seven presets, each test case with its reference marker.
- The trace gate, markers, golden facts, citations, include scan, format, and lint pass.
- Every rejection has a compile-fail test on both compilers that requires its literal message, or the compiler's text where the plan says so, and forbids the messages of the other rejections.
- Every test case and every `static_assert` the part adds has a recorded mutant that it kills, or is recorded as equivalent with the reason.
- The part's limits are measured and stated in the `///` comments.
- Its decisions, the W10 before-and-after record, the per-use costs, the P2 record (WU8b), and the agent-cost row are written.
- This plan records the part's outcome with its V10 line.

The unit is done when both parts are merged into `develop` and this plan is closed. WU8b's rounding-scalar behavior of `truncate` merges only when owner question O1 is settled.

## 7. Acceptance criteria

WU8a:

1. Type properties (STYLE S8 as amended, P5, DoD-4), each a `static_assert`, for at least these instances:
   - rigid(3) point, line, plane, and motor layouts with each of the five scalars;
   - a conformal(3) vector;
   - the sparse set {1, e1, e16, e1 ∧ e16} by mask in R(15,0,1);
   - an empty set.

   The properties:
   - not an aggregate; trivially copyable and standard-layout;
   - for K ≥ 1, `sizeof == K × sizeof(T)`;
   - for `double` and the rational, usable as a class-type template argument;
   - for K = 1, `std::is_convertible_v<T, mv>` false.
2. A default-initialized `mv` and `mv{}` hold T{} in every place, for the five scalars, at compile time and at run time.
3. Construction:
   - Exactly K values, in layout order, land in positions 0 to K − 1, with integer literals accepted for a floating scalar.
   - Fewer values and more values are each one error with the count message and both counts printed. Compile-fail tests on both compilers require the literal and the text "3 == 6" or "7 == 6".
   - A narrowing variable (`int` into `double`, `double` into `float`) is the compiler's narrowing error. The compile-fail tests require "narrow", which both compilers print.
   - A single scalar for K = 1 does not convert implicitly.
4. Equality is true exactly when every coefficient is equal. For each position of a line and a motor, two values that differ there alone compare unequal. For `double`, −0.0 equals +0.0, and a NaN coefficient makes `==` false.
5. Concepts, with the expected answers taken from the stated requirements (S7):
   - `ring_scalar` accepts `std::int64_t`, `int`, `short`, `float`, `double`, `long double`, `std::complex<double>`, and the test rational;
   - it rejects a pointer, `std::string`, a type with `+` and `-` but no `*`, a type with no conversion from `coefficient_type`, a type without unary minus, and a type that is not copyable;
   - `exact_scalar` is true for `std::int64_t`, `int`, and the rational, and false for `float`, `double`, and `std::complex<double>`;
   - `multivector` is true for an `mv` with any cv-qualification and reference, and false for `std::array<double, 4>` and for a blade set.

   `mv` with a scalar that fails `ring_scalar` is one error with its literal, on both compilers.
6. The set checks: each fires alone on a hand-written set, with its message, as a compile-fail test on both compilers (E8). Every set made by `full`, `of_grade`, `merge`, and `blades`, for n from 1 to 8, is accepted.
7. Named access by definition, for the rigid(3) layouts and a conformal(3) layout with e45:
   - `get` with each name as the layout lists it reads and writes exactly that position, through an lvalue, a const lvalue, and an rvalue (whose result is an rvalue);
   - the name's mask and sign, computed in the test by its own inversion count, equal the layout's entry;
   - `has_blade` is true for each listed name, and false for its opposite orientation, for an absent blade, and for an invalid name, with no error.
8. Named access rejections: each of the six messages of section 5 is a compile-fail test on both compilers. Each produces exactly one error, with the message on the first error line after the inclusion context; an invalid name gives the parser's message alone, or `get`'s alone, never both.
9. The test rational:
   - the field identities hold on seeded random values (one seed source, printed on failure, V7);
   - the hand values hold;
   - equality agrees with cross-multiplication;
   - an overflow aborts at run time with its message (`WILL_FAIL`) and fails a constant evaluation (compile-fail);
   - `std::numeric_limits<rational>::is_exact` is true.
10. Limits and costs, measured on both compilers (track-only, C14):
    - the largest n for which `mv<Alg, full<n>, double>` builds at the default limits with the direct repeated-mask check, stated in the `///` comment, and reported if it is below 6;
    - the cost of the counted constructor per type (instantiations of the storage base and of the inherited constructor);
    - W10 before and after;
    - the per-use cost of a type, a construction, and a `get` (note 0012's method).
11. The blade-set changes:
    - 11a. `blades<Alg, ...>`: every migrated test and compile-fail source passes with its expected values and messages unchanged; a first argument that is not `metric_like` is rejected (compile-fail, both compilers).
    - 11b. The C array: every test of `blade_set`, `plan`, and `blade_product` passes with expected values unchanged, including empty sets (`blade_set<0>`, the empty results of `typed_plan<0, 0>`). The longest GCC line of the rejection E9's `get_opposite` is recorded before and after; note 0019 measured 534 against 265 characters with names, and the C array alone is expected to give part of that, not all. `core.plan_cost_model` is run on both compilers and its result reported (R4).
12. The seven presets from fresh trees at the final revision; `all_verify_interface_header_sets` on gcc-debug and clang-debug; `core.bare_compile` with the extended `bare_user.cpp`.

WU8b:

13. Kernel equals plan, by definition, exhaustive over the stated range:
    - forms rigid(3), R(4,0,0), R(1,3,0), and conformal(2) with `full<4>`, and the degenerate form and seeded random form 1 with `full<3>`;
    - the four binary operations and the five unary maps;
    - `apply` on every pair of basis inputs, and on seeded random integer inputs, equals the test's loop over the same plan's terms.

    The range may be reduced by a recorded choice if the edit cycle slows, but never below rigid(3) and conformal(2) for all nine operations.
14. Products against independent values:
    - the hand values F3 to F5 hold in `static_assert`s and at run time;
    - for each of the four golden forms, `apply<gp>` of two seeded random multivectors on `full<n>` equals their product composed in the test from the golden pair tables, over the test rational with non-integer inputs.
15. DoD-7: point ⟑ point, line ⟑ plane, and motor ⟑ point in rigid(3), vector ⟑ vector in conformal(3), and one antiproduct, over `std::int64_t`, the test rational, `float`, `double`, and `std::complex<double>`. The inputs are bounded as reading 14 states; the results equal the exact reference converted to each scalar, by `==`; the bound is derived in the test before the values are seen.
16. The coefficient check, each bound on both sides, each rejection a compile-fail test on both compilers (F8):
    - −2^24 (diag(4096, 4096)) compiles with `float`;
    - −(2^24 + 1) (diag(24929, 673)) is rejected with `float` and compiles with `double`, `int`, and `std::int64_t`;
    - 2^53 (diag(8192, 8192, 8192, 16384)) compiles with `double`;
    - 32767^4 is rejected with `double` and compiles with `std::int64_t` and the rational;
    - the diag(4096, 4096) plan is rejected with `short`.
17. The conversions:
    - for every pair of rigid(3) layouts where the target holds the source's blades, and for a canonical result into a named layout, each coefficient lands on its blade with the sign s_source · s_target, over all five scalars;
    - `Motor{line}`, `Motor(line)`, and `static_cast<Motor>(line)` give equal values;
    - the canonical result of point ∧ point as a Line reads (3, 3, 3, −3, 6, −3) for section 3's p and q;
    - every lossy target, in all three spellings, is rejected with the message (compile-fail, both compilers);
    - an implicit conversion, lossy and lossless, gives the compiler's own error, recorded by compile-fail tests that require each compiler's present wording and are labeled as records of compiler behavior.
18. `truncate`:
    - `truncate<Line>(m)` and `truncate<line_layout>(m)` are equal;
    - kept blades are as for the conversion, and blades of the target that the input lacks are zero;
    - for `std::int64_t` and the rational, a nonzero dropped coefficient fails a constant evaluation (compile-fail) and aborts at run time in a checking build (`WILL_FAIL`, registered where `GALOOT_CHECKING` is ON);
    - a build with checking off returns the kept blades without evaluating the check;
    - for `double`, a nonzero dropped coefficient does not abort, merged only when O1 is settled (R1);
    - a target that is not a multivector of x's algebra and scalar is rejected with its message;
    - the 4.5 trace row is `yes` with its fact in a marker.
19. Compile time equals run time for `apply`, the conversions, and `truncate`, in rigid(3) and conformal(3), with inputs read through `volatile`.
20. Every compile-time check WU8b adds fires on a mutant, and each bound of the coefficient check is tested on both sides.
21. Limits, on both compilers, in the three configurations of decision 0103:
    - `apply` builds for every plan that builds;
    - any plan above 2048 terms that builds on Clang is checked against Clang's fold-nesting limit, and the limit is stated in the comment, or the expansion is chunked by a decision.
22. P2: decision 0103's fixed use through `apply`, at n = 4, 8, 12, 16, before and after, as plan 0013 did (decision 0107's practice), under `docs/agent/records/performance/mv/`.
23. W10 before and after from clean revisions; the per-use costs of `apply`, the conversions, and `truncate`; the seven presets from fresh trees at the final revision; the header sets on gcc-debug and clang-debug; `bare_user.cpp` converts and truncates a Line and a Motor (DoD-6).
24. The assembly read, under `docs/agent/records/performance/mv/assembly/`, for rigid(3) point ⟑ point, point ∧ point, and motor ⟑ point over `double` at -O2 on both compilers:
    - the kernel has one multiplication per term, no multiplication by ±1, no integer-to-floating conversion, no load of plan data, no branch on blades, and terms − outputs additions and subtractions;
    - whether it is inlined is recorded and compared with a hand-written straight-line function compiled the same way;
    - the zero fill of the result is not emitted, for the kernel and for a constructed-then-written `mv`.

## 8. Test list, with expected values and reference sources

Markers as in use: `proof` (exhaustive over a finite range, V1 kind 1), `identity` (kind 2), `independent value` (kind 3), `contract` (a property the header promises). Every exact comparison is `==` (V11). Files:

- `test/core/mv_test.cpp` (CTest `core.mv`), for both parts (S14);
- `test/support/rational_test.cpp` (CTest `support.rational`);
- the migrated `test/core/blade_set_test.cpp` and `test/core/plan_test.cpp`.

Rigid(3) layouts, with `rga3` the test's tag:

- P = `blades<rga3, "e1 e2 e3 e4">`
- L = `"e41 e42 e43 e23 e31 e12"`
- G = `"e423 e431 e412 e321"`
- M = `"1 e41 e42 e43 e23 e31 e12 e1234"`

### WU8a

| # | Test case | Reference (kind) | Expected |
| --- | --- | --- | --- |
| E1 | Type properties | contract (S8 as amended, P5) | Criterion 1. `sizeof`: L over `double` 48, over `float` 24; M over `std::int64_t` 64. The empty set's `coefficients.size()` is 0. `mv<rga3, L, double>` is a template argument, read back equal |
| E2 | Zero initialization | contract | Criterion 2 |
| E3 | Construction by count | contract | `Line{1, 2, 3, 4, 5, 6}` holds 1 to 6 at positions 0 to 5; integer and floating literals give equal values; for a float layout, `{0.5f, 1.0f, ...}` |
| E4 | Equality | contract | Criterion 4, with one mutant per position of the comparison killed |
| E5 | Scalar concepts | contract, from the stated requirements | Criterion 5 |
| E6 | Named access by definition | independent value: the test's inversion count on the name; the CONVENTIONS 1.6 table | Criterion 7. On L: e41 at 0 (mask 0b1001, sign −1), e42 at 1 (0b1010, −1), e43 at 2 (0b1100, −1), e23 at 3 (0b0110, +1), e31 at 4 (0b0101, −1), e12 at 5 (0b0011, +1). On G: e423 (0b1110, +1), e431 (0b1101, −1), e412 (0b1011, +1), e321 (0b0111, −1). On M: "1" at 0 (mask 0, +1) and e1234 at 7 (0b1111, +1). The marker states "an oriented name lists generators in the order of their product", the 1.6 fact |
| E7 | `has_blade` | contract | True for every listed name of E6. False, with no error, for e14, e24, e34, e13 on L, for e1 on L, for "e5" in rigid(3), for "x1", and for "e1 e2" |
| E8 | Set checks and scalar | contract (V4) | Compile-fail tests:<br>- `core.mv_beyond_n` (e5 last in a set of rigid(3))<br>- `core.mv_beyond_n_31` (e32 in R(31,0,0))<br>- `core.mv_bad_sign` (sign 0)<br>- `core.mv_bad_sign_second` (sign 2, middle)<br>- `core.mv_repeated_mask` (e1 twice)<br>- `core.mv_repeated_orientations` (e14 and e41)<br>- `core.mv_repeated_apart` (e2 e1 e2)<br>- `core.mv_repeated_after` (e1 e2 e3 e2)<br>- `core.mv_not_metric_like`<br>- `core.mv_not_ring_scalar` (`std::string`; requires the scalar literal; one error)<br>- `core.mv_scalar_without_multiplication` (Spike C's case 5)<br><br>Accepted: the sets of criterion 6 |
| E9 | Named access rejections | contract (C9) | Compile-fail tests, each requiring its message and forbidding the others:<br>- `core.mv_get_absent` (e1 on L)<br>- `core.mv_get_opposite` (e14 on L)<br>- `core.mv_get_above_n` (e5)<br>- `core.mv_get_not_a_name` ("x1"; "e0" as a second case)<br>- `core.mv_get_two_names` ("e1 e2")<br>- `core.mv_get_wide_algebra` (a set in R(10,0,0)) |
| E10 | Detection | contract (decision 0033) | `requires { get<"e1">(line); }` is true (the documented consequence); `has_blade<Line, "e1">` is false; `std::is_constructible_v<Line, double, double, double>` is true (the count catch-all), and the comment of `mv` says so |
| E11 | Rational, hand values | independent value, by hand | 1/3 + 1/6 = 1/2; (−2)/4 = 1/(−2) = −1/2, with denominator 2; (2/3)(9/4) = 3/2; 1/2 − 1/2 = 0, with denominator 1; (3/7) / (6/7) = 1/2 |
| E12 | Rational, identities | identity, seeded (V7) | Criterion 9, with numerators and denominators drawn below 2^15 |
| E13 | Rational, overflow | contract | `support.rational_overflow` (`WILL_FAIL`, requires the message) for 2^62 + 2^62; `support.rational_consteval_overflow` (compile-fail) for the same in a `static_assert` |
| E14 | Compile time equals run time | contract | E3, E6, and E11 also through `volatile` at run time |
| E15 | Construction rejections | contract (decisions 0150, 0152) | Compile-fail tests:<br>- `core.mv_too_few` (three values for L; requires the count literal and "3 == 6")<br>- `core.mv_too_many` (seven; "7 == 6")<br>- `core.mv_narrowing_int` (an `int` variable for `double`; requires "narrow")<br>- `core.mv_narrowing_float` (a `double` variable for `float`; requires "narrow")<br>- `core.mv_scalar_implicit` (`mv<rga3, blades<rga3, "1">, double> s = 2.0;`; requires the compiler's conversion error, labeled as compiler behavior)<br><br>Each wrong count gives one error |
| E16 | `blades<Alg, ...>` | contract, and the migrated tests' own references | Criterion 11a. `blades<rga3, "e41 e23 e31">` equals the set {(0b1001, −1), (0b0110, +1), (0b0101, −1)}. Compile-fail `core.blade_set_blades_not_metric_like` |
| E17 | Blade sets in a C array | contract, and the migrated tests' own references | Criterion 11b. `blade_set<0>` and `blade_set<K>` both: range-for, subscript, `std::span` conversion, `==`, a template argument, trivially copyable |

### WU8b

| # | Test case | Reference (kind) | Expected |
| --- | --- | --- | --- |
| F1 | Kernel equals plan | proof over the stated range | Criterion 13. The reference is the test's loop over `plan.terms`: out += c · x_a · y_b, from zero |
| F2 | First term and folding | contract | For P ⟑ P, each output's first term in plan order has the lowest a, then the lowest b. An input of −0.0 in one place gives the same values as +0.0, except possibly the sign of a zero output |
| F3 | Rigid(3) hand values | independent value, by hand from e1² = e2² = e3² = 1, e4² = 0 (planning values) | p = (1, 2, 3, 1), q = (4, 5, 6, 1).<br>- p ⟑ q = (32, −3, −6, −3, −3, −3, −3) on (1, e12, e13, e14, e23, e24, e34)<br>- p ∧ q = (−3, −6, −3, −3, −3, −3) on the six bivectors<br>- As a Line, (3, 3, 3, −3, 6, −3); the moment equals the cross product (−3, 6, −3) |
| F4 | Conformal(3) hand values | independent value, by hand from e4 · e5 = −1 (planning values) | v = (1, 2, 3, 4, 5), w = (1, 0, 0, 1, 1) on (e1, ..., e5).<br>- v ⟑ w = (−8, −2, −3, −3, −4, 0, 2, 2, 3, 3, −1) on (1, e12, e13, e14, e15, e23, e24, e25, e34, e35, e45)<br>- The scalar is 1 + (−1)(4 + 5)<br>- e45 gets v4 w5 − v5 w4 = −1<br>- e23 is in the result set and is zero for these inputs |
| F5 | Unary maps on a Line | independent value, from plan 0013's D2 terms and the 2.1 sign rule | l = (1, 2, 3, 4, 5, 6) on L. The reverse on the canonical set (e12, e13, e14, e23, e24, e34) is (−6, 5, 1, −4, 2, 3); onto L itself, it is −l |
| F6 | Golden composition | independent value (golden `gp_tables`, LDLᵀ route) | Criterion 14; rational inputs with denominators 2, 3, and 5 |
| F7 | Five scalars (DoD-7) | independent value (the golden composition in `std::int64_t`) | Criterion 15. The test states the bound: inputs are integers of magnitude at most 2^4 (both parts, for complex); every coefficient of these plans has magnitude at most 2 (asserted in the test); each output has at most 48 terms. So every partial sum is below 48 · 2 · 2^8 · 2 < 2^16 < 2^24 |
| F8 | Coefficient exactness | independent value, by hand (planning values, checked in Python while planning) | Criterion 16.<br>- e12 e12 = −g11 g22 in a diagonal form, and e1234 e1234 = +g11 g22 g33 g44<br>- 24929 · 673 = 16,777,217 = 2^24 + 1, with 24929 = 97 · 257<br>- 4096² = 2^24, and 8192³ · 16384 = 2^53<br>- 32767^4 = 1,152,780,773,560,811,521, odd and between 2^53 and 2^63 |
| F9 | Conversions | contract and independent value | Criterion 17. Compile-fail tests:<br>- `core.mv_conversion_lossy_brace`, `_paren`, `_cast` (each requires the conversion literal)<br>- `core.mv_conversion_implicit_lossy` and `_implicit_lossless` (compiler wording, labeled) |
| F10 | `truncate` | contract (CONVENTIONS 4.5) | Criterion 18.<br>- The motor (0, 1, 2, 3, 4, 5, 6, 0) truncated to L, by type and by set, is (1, ..., 6)<br>- (7, 1, ..., 6, 0) aborts in checking mode for `std::int64_t` (`core.mv_truncate_nonzero`, `WILL_FAIL`) and fails a constant evaluation (`core.mv_truncate_consteval_nonzero`)<br>- With checking off it returns (1, ..., 6)<br>- Over `double` it returns (1, ..., 6) and does not abort (merged with O1)<br>- `core.mv_truncate_other_algebra` and `core.mv_truncate_not_a_type` require the truncate literal<br><br>The marker states "truncate checks that the part it drops is zero", the 4.5 trace fact |
| F11 | Target-set form | proof over the rigid(3) layouts | The canonical result of every rigid(3) pair of WU7a's C2, written onto a named layout that holds it, equals the canonical result converted |
| F12 | Compile time equals run time | contract | Criterion 19 |
| F13 | Empty results | contract | L ∧ G gives an `mv` with no coefficients, default-constructed, and two of them compare equal |

Mutants to be killed, each recorded with the test that kills it, on both compilers, in both modes of the mutation driver.

WU8a:

- each set check removed;
- the repeat scan starting two places on;
- the repeat test comparing (mask, sign) instead of mask;
- a sign of 0 accepted;
- `get` ignoring the sign;
- `get` one position off;
- `get` returning the first blade with the mask regardless of orientation;
- `has_blade` raising an error for an invalid name;
- equality skipping the last coefficient;
- the count catch-all accepting K − 1;
- the K-parameter constructor not `explicit` for K = 1;
- the storage's default member initializer removed;
- the rational without normalization, with a negative denominator kept, or with the overflow guard removed;
- `ring_scalar` without its unary-minus requirement;
- the `blade_set<0>` specialization holding one element;
- `blades<Alg>` reading n from the wrong place (a fixed 4).

WU8b:

- the first-term flag inverted;
- the first term added to zero (equivalent in value; killed by the assembly criterion; recorded as a cost mutant);
- the ±1 branches swapped;
- the coefficient applied as (c · x) · y (equivalent on the exact inputs of every test; recorded as equivalent, the order being declared under V14);
- the exactness bound off by one on each side;
- the conversion's sign s_source · s_target replaced by s_target;
- the covering test reversed;
- the dropped-coefficient check run for a rounding scalar, or skipped for the rational;
- `truncate<Type>` accepting a type of another algebra.

## 9. Approach

Direct forms first (C3, W4), in the style of `plan.hpp` and `blade_set.hpp`: `constexpr` functions over values, variable templates computed once per argument list (S7, P3), `static_assert`s with literal messages, catch-alls of decision 0033's forms and decision 0152's numeric variant, and `GALOOT_ASSERT` for run-time preconditions. Every standard header `mv.hpp` includes is one the core already includes, unless the include-cost record shows it costs nothing measurable.

Each commit builds and passes on what it was checked on (W2, W9 as amended by decision 0094). Test scope per commit: the affected tests and the cheap gates, checked at the commit on a clean tree. The full gcc-debug suite runs at the end of each test-and-code pair, and all seven presets once at the final revision.

Order of commits for WU8a, on `feature/core-mv-a`:

1. The W10 before rows from fresh gcc-debug and clang-debug trees at the claim revision.
2. `blades<Alg, ...>` and the migration of its uses, listed, no expected value changed (E16).
3. `test/support/rational.hpp` and its tests (E11 to E13), with the CMake registration of `support.rational`.
4. `mv.hpp`: the concepts, the storage, the type, the count constructor, the set and scalar checks; `core.hpp` and the header set. Tests E1 to E5, E8, E14, E15.
5. Named access and its catch-alls; tests E6, E7, E9, E10; `bare_user.cpp`.
6. After plan 0015 has merged and `develop` is merged in: the C array in `blade_set` and the migration of its uses, listed (E17); `core.plan_cost_model` on both compilers. If it fails, the cause is recorded, and any refit is a separate listed commit (R4).
7. Mutation records; limits and per-use costs; W10 after; appended sections of decisions 0150 and 0153; decision 0156.
8. Outcome with the V10 line; W5 review; fixes in their own commits; a delta review of any core fix (decision 0094 item 3); merge after `develop` is merged in and P9 passes.

WU8b, on `feature/core-mv-b`, after WU8a merges:

1. The claim line here; W10 before.
2. The kernel and the coefficient check (F1 to F8, F12, F13).
3. The target-set form, the conversions, and `truncate` (F9 to F11), with the 4.5 trace row.
4. The assembly read; limits, P2, per-use costs, W10 after; mutation records.
5. Decision 0154, the appended section of 0155, and 0156's appended section.
6. Outcome, review, merge. The rounding-scalar case of `truncate` waits for O1.

The hci-expert reads the interface before each W5 review: the WU8a header with its rejection outputs on both compilers, and the conversion and `truncate` messages of WU8b. The reading is recorded under `docs/agent/records/reviews/` as advice. It may use note 0019's checklist as a guide (R8).

Compile-time cost to every includer (decision 0113: `core.hpp` alone is about 0.91 G instructions on GCC and 0.92 G on Clang). The budget approach, with no bound (C14):

- measure `core.hpp` alone and `mv.hpp` alone, before and after each part, on both compilers;
- if a rise exceeds the spread of three repeats, attribute it with a Clang `-ftime-trace` and report it with its cause;
- keep `mv.hpp` from instantiating anything at namespace scope and from adding standard headers;
- measure separately what falls on programs that use the type (criteria 10 and 23): the instantiations of the storage base and inherited constructor per type, the catch-alls, `get`, and `apply`.

The C array and `blades<Alg>` change what every includer parses (`blade_set.hpp` now includes `metric.hpp`, already in `core.hpp`), and their W10 rows say so.

## 10. Evidence

- Test output per preset for `core.mv`, `support.rational`, and the migrated suites, with the exhaustive ranges stated (F1, F11).
- Compile-fail output on both compilers for every rejection, with the line on which each message appears.
- The trace gate output in each merge message, with the 4.5 row pinned (WU8b).
- Mutation records for both parts.
- Limits, per-use costs, P2, W10, and the canary result of R4, each as `tools/timed` runs in `runs.tsv` with their commands, and summaries under `docs/agent/records/performance/mv/`.
- The assembly read, with the commands and the hand-written comparison.
- V10 line, to fill in the outcome:
  - Verification: E1 to E17 and F1 to F13 (kinds 1 to 3 over the stated ranges).
  - Validation, rigid(3): the line through two points, with direction q − p and moment p × q (F3), and the oriented names against CONVENTIONS 1.6 (Checked, Confirmed).
  - Validation, conformal(3): the hand value F4.
  - Validation, scalars: the exactness of each scalar's conversions rests on IEEE 754's binary formats (background).
  - The non-diagonal antiproducts remain unvalidated (plan 0013, departure 8).

## 11. Evidence gathered while planning

Planning values, not reproducible as committed (R9). Probe sources were throwaway files over `develop`'s headers, compiled with `-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -I include` on GCC 16.2.1 and Clang 23.1.4. Every compile went through `tools/timed`, in shared mode, one run each. Selections: `wu8-plan-probe:*` at `7da09f1`, committed in `4a7898f`; `wu8-plan-probe2:*` at `daf69d8`'s parent `37a562d`, not committed.

1. Kernel form, rigid(3) motor ⟑ point (28 terms on 8 outputs, coefficients ±1), `double`, -O2:
   - A loop over the terms: GCC kept 3 multiplications, 1 integer-to-double conversion, and 1 branch (a loop); Clang 4, 2, and 2.
   - An index-pack expansion with ±1 folded: 28 multiplications and no conversion on both. GCC had no call and no branch. Clang called the expanded lambda out of line (one call from the caller to a straight-line body).
   - At -O0 the expansion made about 200 calls on both.
   - Compile cost of the unit at -O0, in retired instructions on performance cores: GCC 3.82 G with the expansion against 3.93 G with the loop; Clang 4.87 G against 4.86 G.
2. First term:
   - Accumulating from zero gave 28 additions and subtractions; assigning each output's first term gave 20, on both compilers.
   - A first-term pass comparing every term with every earlier one failed Clang's step limit at 1,024 terms and GCC's operation limit at 4,096; a linear pass did not.
   - With the linear pass, `full<n>` × `full<n>` in R(n,0,0) compiled at -O0 on GCC in 0.54 s (n = 4), 0.94 s (n = 5), and 3.37 s (n = 6, 4,096 terms, 0.76 GiB peak); on Clang in 0.60 s and 0.95 s for n = 4 and 5. On Clang, n = 6 stops in the plan itself.
3. Aggregate behavior (first version, superseded by item 5): `Line{motor}` failed with a message about `double`, while `static_cast<Line>(motor)` and `Line l(motor)` compiled. A `float` layout needs `0.1f` literals under `-Wconversion -Werror`.
4. A bad scalar under a constraint whose first failing conjunct is constructibility: 174 lines on GCC and 99 on Clang, listing `std::string`'s constructors. Note 0019 measured 43 lines and four errors on GCC with the arithmetic conjunct first, and one error with a `static_assert` in the body.
5. The counted design (probe 2): K parameters of type T inherited from a storage base, a count catch-all, explicit converting constructors, and a defaulted `==` on the base. Findings on both compilers:
   - The type is trivially copyable, standard-layout, not an aggregate, usable as a class-type template argument (`first<Line{1, 2, 3, 4, 5, 6}>` is 1.0), and not implicitly convertible from `double` for one blade; `mv<rga3, P, std::complex<double>>` is trivially copyable.
   - Integer literals into a `double` layout compile.
   - Three values for a four-blade point give one error: GCC "static assertion failed: mv: give one value for each component, in the order the layout lists them • the comparison reduces to ‘(3 == 4)’"; Clang "... due to requirement 'sizeof...(A) == component_count'", then "expression evaluates to '3 == 4'". Five give "5 == 4".
   - An `int` variable for a `double` coefficient is GCC's "narrowing conversion of ‘w’ from ‘int’ to ‘component<double, 3>’ {aka ‘double’}" and Clang's "non-constant-expression cannot be narrowed from type 'int' to 'component<double, 3UL>' (aka 'double')". A `double` variable into a `float` layout is rejected the same way.
   - `Motor{line}`, `Motor(line)`, and `static_cast<Motor>(line)` compile and agree.
   - A lossy `Line{motor}` reached the lossy catch-all. Its probe condition was a function of the two sets, so both compilers printed the sets in the message, which is why section 5 uses form P there.
   - The implicit `const Line l = motor;` is the compiler's "conversion from ‘mv<...>’ to non-scalar type ... requested" (GCC) or "no viable conversion from ..." (Clang).
   - Without a defaulted `==` on the base, the derived type's defaulted `==` is deleted.
   - With a size of zero, the K-parameter constructor would redeclare the default constructor, hence its constraint.

## 12. Review

W5 blind review of each part before its merge, by a reviewer independent of the author, given this plan's criteria, section 8, and the author's departures, not the author's view that the change is correct.

The WU8a reviewer also:

- reads every rejection output on both compilers as a first-time user would (C9);
- checks E6's masks and signs against CONVENTIONS 1.6 by hand;
- reruns the repeat-scan, `get`-orientation, and count-catch-all mutants;
- checks the rational's overflow guard at its bounds;
- checks that the C-array and `blades<Alg>` migrations changed no expected value, line by line against the listed commits.

The WU8b reviewer also:

- checks F7's bound argument and the hand values of F3, F4, and F8;
- checks the assembly read against the hand-written form;
- reruns the first-term and exactness-bound mutants;
- runs `truncate`'s checking-mode tests in a checking and a non-checking preset.

A fix to core code after a review gets a delta review (decision 0094 item 3). The hci-expert reading (section 9) precedes each review and is advice, not the review. Note 0019's checklist is not a review criterion (R8).

## 13. Agent cost estimate (for information only, decision 0019)

From `records/agent-cost.tsv`: the WU7a implementer with review fixes used 631k tokens and 194 tool calls, WU7b's 674k and 286. Their reviewers used 300k and 79, and 358k and 112.

| Role | Tokens | Tool calls |
| --- | --- | --- |
| WU8a implementer, with review fixes (blade-set migrations included) | 520k to 700k | 200 to 270 |
| hci-expert reading of WU8a | 120k to 180k | 40 to 60 |
| WU8a blind reviewer, plus a delta review | 330k + 150k | 100 + 50 |
| WU8b implementer, with review fixes (assembly read and P2 included) | 500k to 650k | 190 to 250 |
| hci-expert reading of WU8b | 80k to 120k | 30 to 40 |
| WU8b blind reviewer, plus a delta review | 330k + 150k | 110 + 50 |
| Total | about 2.2M to 2.8M | about 770 to 980 |

No bound applies. The outcome of each part compares these with the actuals. This revision used about 45 tool calls; its token count is not visible to me.

## 14. Risks

1. Common-mode error between the tests and the header, both written from CONVENTIONS by agents. Each family has a route that shares no code: golden tables, hand values, the test's own inversion count, the loop form of the kernel, and cross-multiplication for the rational.
2. The test rational is a test oracle. It has its own identities, hand values, and mutants, and DoD-7 also compares it with `std::int64_t`.
3. Diagnostics are compiler text. The compile-fail tests require Galoot's literals. The few that record compiler wording (narrowing, implicit conversion) are labeled and are expected to change with a compiler.
4. The C array may move plan 0015's canary thresholds (R4). It is sequenced after plan 0015 and reported, not refitted silently.
5. The `blades<Alg>` migration touches other units' test files when they merge. The unit that merges second migrates them, in a listed commit.
6. The counted constructor costs an instantiation of the storage base and its constructor per type, and a catch-all per call shape (criterion 10 measures it). Programs that declare many types pay more than with the aggregate.
7. The repeated-mask check is O(K²) and may lower the largest `full<n>` an `mv` accepts on Clang; present results reach 64 blades.
8. Clang leaves the expanded kernel out of line (section 11, item 1). It is compared with a hand-written function; no vendor attribute is used without a decision.
9. Clang's fold-nesting limit of 2048 caps one expansion; no plan that builds on Clang is known to exceed it (criterion 21).
10. Exactness is unchecked for scalars without a `numeric_limits` specialization, such as `std::complex<float>` with a coefficient above 2^24; the comment and decision 0151 say so.
11. Name clashes: `get` with `std::get`, `apply` with `std::apply`, and `truncate` with POSIX `truncate`. A test calls `get` and `truncate` unqualified in a unit that includes `<tuple>` and `<unistd.h>` and uses `std::complex` scalars.
12. `std::is_constructible_v` and `requires`-probes answer yes for a wrong count and for `mv<A, B, std::string>` (decisions 0033 and 0152); the comments say so.
13. Lessons 0060 to 0064 apply.

## 15. Deferred

- To WU9 and WU10:
  - the public `geometric_product`, `wedge`, `geometric_antiproduct`, `antiwedge`, `reverse`, `right_complement`, and the rest, as one-line callers of `apply`, with their operators (CONVENTIONS 7.1);
  - the enumerator rename of decision 0159 (in WU9);
  - the `same_algebra` concept and its message;
  - what blade set the unary operations return (reading 17);
  - scaling, sums, and differences.
- To WU11: bulk, weight, duals, dot, antidot.
- To WU13: `transform` and `compose` with their declared result sets, and their checking against their own plan's bound on rounding scalars.
- To WU15: `format.hpp` and the DoD-19 catalogue review.
- To WU16: model aliases (with or without a default scalar) and the documentation's examples, compiled as tests (W12).
- To M3: tests with nonzero tolerances (reading 14).
- To M5: WU5's core rational.
- Not built, noted only (C12):
  - names as written in the type (decision 0157 item 3);
  - conversion between scalar types;
  - structured bindings;
  - names for n ≥ 10;
  - a Galoot exactness trait;
  - chunked expansion;
  - the canonical-order fast path of the repeated-mask check;
  - renaming the test helpers and golden namespaces that say `gp`.

## 16. Owner questions, open

- O1. CONVENTIONS 4.5's clause on rounding scalars conflicts with the ruling on question 7 (R1). Ratify the proposed wording? Recommended: yes.
- O2. PERFORMANCE P5 says "trivially copyable aggregates" (R2). Ratify "trivially copyable"? Recommended: yes.
- Not a question, recorded: note 0019's reviewer checklist is not ruled on and stays a proposal (R8).
