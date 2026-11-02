# 0016 WU8: the multivector type, its scalars, named access, and applying plans

- Status: claimed 2026-11-01 (planner: Claude, agent, Opus 5.5, cpp-expert role, with the hci-expert and math-expert role files for the interface and the mathematics). Revised 2026-11-01 for the owner's rulings ("proceed as recommended") on this plan's questions and on research note 0019's (section 2), and again on 2026-11-01 for the owner's later ruling that `blade_set` keeps its `std::array` (section 2). Not started.
- Unit: WU8 of plan 0001 (`mv.hpp`), in two parts run one after the other: WU8a on branch `feature/core-mv-a` (worktree `.claude/worktrees/core-mv-a`), then WU8b on branch `feature/core-mv-b` (worktree `.claude/worktrees/core-mv-b`), branched from `develop` after WU8a merges. Each part has its own W5 review and merge. WU8b is claimed by a dated line in this plan when WU8a has merged, as plan 0013 did for WU7b (decision 0158).
- Decision numbers 0150 to 0159, all assigned (section 5, "Both parts"). The owner's rulings are recorded in 0150, 0151, 0152, 0153, 0155, 0157, 0158, and 0159 (commit `daf69d8`); 0154 and 0156 are the implementers'.
- Depends on: WU7 (plan 0013, merged: `product_plan` for the four binary operations and `unary_map` for the five unary maps, typed onto canonical result sets), Spike F (research note 0012), Spike C (note 0010) with decision 0033, and research note 0019 (API design, its spike under `docs/agent/records/spikes/api-design/`). Not sequenced after plan 0015 (section 1, reading 22).
- Goal: the type a user holds. `mv<Alg, Blades, T>` stores one coefficient of a scalar type T on each oriented blade of a blade set; it is constructed from exactly one value per component and is zero by default; the scalar concepts say which T are accepted; the type rejects a blade set that is wrong for its algebra; `get<"e41">(x)` reads and writes a coefficient by its oriented name; `==` compares exactly; a plan or map is applied to stored coefficients with straight-line code whose coefficients are exact in the scalar; a conversion between blade sets is explicit, and the lossy one is `truncate<Type>(x)` or `truncate<Blades>(x)`, checked in checking mode for exact scalars. DoD-7 holds across five scalars. Blade sets are spelled with the algebra, `blades<rga3, "...">`, and the diagnostics of the three likeliest mistakes are recorded before and after, for the owner to judge whether more is needed (section 5, "Other routes to readable diagnostics").

## 1. Departures, ambiguities, and owner questions (C20)

### Departures and open items after the owner's rulings

R1. **A ruling that conflicts with ratified text, not edited (owner question O1).** The ruling on question 7 (`truncate` checks exact scalars only, decision 0155) conflicts with the clause of CONVENTIONS 4.5 that says checking mode verifies the dropped part "within the operation's forward error bound for a rounding scalar". The ruling's 4.5 edit covers only `truncate<Type>`, so that clause stands. Proposed wording, for the owner to ratify: "... and in checking mode verifies that what it drops is zero when the scalar is exact. An operation that drops part of its own result, such as `transform`, verifies on a rounding scalar that the dropped part is within that operation's forward error bound (decision 0010)." Until it is ratified, WU8b implements the exact-scalar check and does not merge the rounding-scalar behavior of criterion 16 (it halts on that item under W8 if the wording is still open when everything else is done).

R2. **A second text the ruling does not cover (owner question O2).** PERFORMANCE P5 says "Values are trivially copyable aggregates (STYLE S8)". After the S8 amendment, multivectors are not aggregates. Not edited. Proposed: "Values are trivially copyable (STYLE S8)".

R3. **A precision in the S8 amendment.** The ruling's words, "trivially copyable structural values", cannot hold for every scalar DoD-7 requires: `std::complex<double>` is trivially copyable but not structural (its members are private). The applied text says "whenever their scalar type is trivially copyable and structural" (decision 0150 item 5). Nothing else in the ruling's text was changed.

R4. **The C array inside `blade_set` is dropped (owner ruling, 2026-11-01).** The first revision put `blade_set`'s blades in a C array to shorten GCC's printing (ruling H2, first half). The owner then ruled that `blade_set<K>` keeps `std::array<oriented_blade, K>`: consistency is preferred over the printing gain, and diagnostics are to be improved by other routes (section 5, "Other routes to readable diagnostics"). So nothing of the C array remains: no `blade_set<0>` specialization, no migration of `.at(i)` and `blades ==`, no commit sequenced after plan 0015, and no canary run for it. Decision 0157 carries the ruling in an appended section. The fact itself stands, as research note 0019 measured it: a C array would shorten GCC's line for a two-blade set from 194 to 136 bytes (it answers note 0010's open question 4).

R5. **`blades<Alg, "...">` replaces `blades<N, "...">` (ruling H2) rather than standing beside it.** A variable template cannot be overloaded on whether its first argument is a type or a number, so keeping both would need a second name, which C12 and "one name, one meaning" argue against. About 50 uses in the tests and compile-fail sources move to algebra tags with no expected value changed, plus whatever new files have landed by then (plan 0015's canary units use `full<4>` and no `blades<N>`, as read at its branch head `26cc06e`). `full<N>` and `of_grade<N, K>` keep the count: they take no names and need no algebra. `blade_set.hpp` then includes `metric.hpp` (decision 0157 supersedes decision 0090 item 4).

R6. **The rename of ruling H3 is split between now and WU9 (decision 0159).** Now: CONVENTIONS 7.1 names `geometric_product` and `geometric_antiproduct` (version 1.6), and the trace table's two unimplemented facts that spelled `gp` and `antigp` use the new names. In WU9: the enumerators `BinaryOperation::gp` and `::antigp` become `::geometric_product` and `::geometric_antiproduct`, with every use migrated in one listed commit, in the unit that creates the public functions. WU8 renames nothing in `plan.hpp`, which plan 0015 edits; WU8's own tests use the present enumerator names and are migrated in WU9 with the rest. Test-only helper names and the golden namespaces (`gp_terms`, `gp_tables`) are not renamed.

R7. **Consequences of the rulings for the design, made here.**
   - The counted construction of ruling H1 is written as K parameters of type T, one per component, inherited from a small storage base, because a constructor template cannot both accept integer literals and reject a narrowing variable (planning probe 2, section 11). Brace initialization then keeps exactly the narrowing rule an aggregate had.
   - Because the type is no longer an aggregate, the conversion between blade sets becomes an `explicit` constructor of the target. So `Motor{line}` now works beside `Motor(line)` and `static_cast<Motor>(line)`, and the brace-form pitfall of the first version (a message about `double`) is gone.
   - Implicit conversions, lossy or not, are rejected with the compiler's own message, because S8 keeps every single-argument constructor `explicit`. A non-explicit catch-all would give a Galoot message there, but would break S8.
   - Under ruling H4, `mv` constrains its scalar by `std::semiregular` and asserts `ring_scalar` in its body, so a bad scalar gives one error with a literal message, not GCC's four.

R8. **The reviewer checklist, adopted after the first revision.** The first revision recorded note 0019's checklist (its section 7) as not ruled on. The owner has since adopted it (decision 0160, commit `91344d6`): it is in the `blind-reviewer` role file, and W5 says that a review of a change to a public interface applies it, each unchecked item a finding. So both W5 reviews of this unit apply it (section 12). Decision 0158 item 3, which says it was not ruled, is superseded on that point by 0160 and is not rewritten here.

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
22. Overlap with work in flight (W7), re-checked 2026-11-01 against plan 0015's branch head `d89806d` (reviewed, not yet merged). Plan 0015 changes `plan.hpp`'s `///` comments, adds blocks to `test/CMakeLists.txt` and six canary units `test/core/plan_cost_*.cpp`, adds `test/tools/plan_cost_model.py`, moves scripts into `docs/agent/snippets/`, and appends to `runs.tsv`.
   - WU8 does not edit `plan.hpp`, `docs/agent/snippets/`, or the canary units.
   - WU8a's one change outside its own files is `blades<Alg, ...>` in `blade_set.hpp`, which plan 0015 does not touch. It changes no constant evaluation of a plan, so it cannot move the canary's thresholds, and no canary unit uses `blades<N, ...>` (all use `full<n>` or sets by mask, read at `d89806d`). So WU8a no longer waits for plan 0015's merge.
   - Still shared: blocks in `test/CMakeLists.txt` and rows in `runs.tsv`. These are adjacent textual edits. Whichever unit merges second merges `develop` in first, keeps both sides' blocks and rows, and runs the P9 checks on the result (W7). That is the only sequencing left.
   - If plan 0015 merges first, WU8a's merge of `develop` brings the canary in, and the canary then runs in WU8a's P9 checks like any other test; a failure there would be a defect to trace, not an expected effect.
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
| H2: blade sets in a C array; `blades<Alg, "...">` | 0157 | none | `blades<Alg>`: a WU8a commit, criterion 11a, test E16; R5 |
| Later ruling: `blade_set` keeps `std::array`; diagnostics improved by other routes | 0157, appended section | none | The C-array parts removed (R4); section 5's design task "Other routes to readable diagnostics"; criterion 11b and test E17 now the diagnostics record |
| H3: `geometric_product`, `geometric_antiproduct` | 0159 | CONVENTIONS 7.1 (version 1.6); two trace facts | R6; deferred list |
| Note 0019's reviewer checklist (ruled after the first revision) | 0160 | WORKFLOW W5 and the `blind-reviewer` role file | R8; section 12 |

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
| `mv<Alg, Blades, T>` | Three arguments, algebra first, no default scalar. Clang prints `mv<rga3, line_layout, double>` through the alias; GCC prints the set's value through libstdc++'s wrapper (`std::__array_traits<...>::_Type{...}`) | The algebra comes first in mixed-algebra diagnostics (note 0019, case 9). GCC's rendering is the main cost the owner asked to reduce by other routes (section 5); names as written in the type are one of them, not adopted now (decision 0157 item 3) |
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

Blade sets (ruling H2, first half as amended, decision 0157), first, in its own commit with the migration of its uses listed and no expected value changed:

- `blades<Alg, Names>` in place of `blades<N, Names>`, with `Alg` constrained by `metric_like`; `blade_set.hpp` includes `metric.hpp`. The parser's messages are unchanged. One new rejection: a first argument that is not `metric_like`. `blade_set<K>` keeps `std::array<oriented_blade, K>` (R4).

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


Other routes to readable diagnostics (a WU8a design task; the owner's ruling of 2026-11-01 asks for diagnostics to be improved without changing storage types). Each route below is available now or later without touching `blade_set`'s or `mv`'s storage; those marked "in WU8a" are part of this unit, the others are recorded here so that the owner can choose among them after seeing the record that follows. Sources are research note 0019, sections 4 to 6, and note 0010.

1. Names as written in the type, `mv<rga3, "e41 e42 e43 e23 e31 e12", double>` (ruling H2, second half; not adopted now, decision 0157 item 3). It is the largest gain measured: GCC's longest line for a bad name fell from 534 to 265 characters, for a lossy conversion from 892 to 363. Its costs are type identity by text unless the text is normalized, results computed by the library that have no written names, and a second family of types. Not in WU8a; a later unit by the owner's decision.
2. Catch-alls with literal messages, one per cause, with a numeric condition where the help is a number (decisions 0033 and 0152). In WU8a: the count constructor ("3 == 6") and the six `get` messages.
3. One error per mistake. A more specific check suppresses the catch-all: `has_blade` is false, without its own error, for an invalid name, so the parser's message is the only one. In WU8a.
4. The scalar checked in the class body under a weaker constraint (decision 0152), which removes GCC's four-error cascade, and the requirements of `ring_scalar` ordered so that a short one fails first. In WU8a.
5. One `get` overload on a forwarding reference, which shortens GCC's raw failure (25 lines against 34). In WU8a.
6. User-facing type aliases and named layouts in every example. Clang keeps an alias in its "aka" text (`'Point' (aka 'mv<rga3, ...>')`) and printed the variable name of a named layout in the first planning probe (`mv<rga3, M, double>`), so a user who writes `using Line = ...;` and `inline constexpr auto line_layout = ...;` reads those names in Clang's messages. The examples of this plan, of the tests that serve as examples, and later of the documentation always declare both. In WU8a, for the tests; the documentation follows in WU16 (W12).
7. The message style of reading 21: the operation as written first, the mistake in the algebra's terms, the fix after a semicolon, no masks or positions. In WU8a.
8. Catch-alls for mixed algebras on the products (note 0019, section 6.8), where mixing algebras becomes the most likely mistake. WU9.
9. `format` printing blades by their oriented names and the algebra by its tag name, so that output and messages share words (note 0019, section 6.10). WU15.
10. Computed messages (C++26, form G of note 0010), which could print blade names in the message itself, once the language baseline moves (a decision under STYLE S2). Not available in C++23 under the project's flags.
11. Not recommended: a catch-all that names an undefined template (form T), which puts the user's terms on the first line but reads as a defect of the library (note 0019, section 5.2).

The record, by the WU8a implementer: for each of the three likeliest mistakes of note 0019 section 6.1, the longest diagnostic line and the line count, on GCC and on Clang, before and after WU8a, in a table under `docs/agent/records/diagnostics/wu8/` with the compile commands and the full texts:

- giving the wrong number of coefficients (`Point p{1, 2, 3};`);
- naming a component with the other orientation (`get<"e14">(line)`) or one the type does not hold (`get<"e1">(line)`);
- assigning to a type that drops some of the source's components (`const Point s = motor;`).

"Before" is note 0019's committed spike texts for the mask spelling (`docs/agent/records/spikes/api-design/out/`, spelling M, forms R and P), because no `mv` exists before WU8a; "after" is WU8a's own `mv.hpp`. For the third mistake, WU8a records what its type gives without a converting constructor (the compiler's own message), and WU8b adds a row once its conversion constructors exist. The record states, for each mistake, which of the routes above the outcome used, and it is quoted in WU8a's outcome so that the owner can see whether more is needed.

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
11. `blades<Alg, ...>` and the diagnostics record:
    - 11a. `blades<Alg, ...>`: every migrated test and compile-fail source passes with its expected values and messages unchanged; a first argument that is not `metric_like` is rejected (compile-fail, both compilers).
    - 11b. The diagnostics record of section 5 ("Other routes to readable diagnostics") exists for the three likeliest mistakes on both compilers, before and after, with the commands and full texts, and is quoted in the outcome; each of the routes marked "in WU8a" is in the header, or its absence is reported with the reason.
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
| E17 | Diagnostics record | measurement, not a test (V1 does not apply; it judges readability) | Criterion 11b. The three likeliest mistakes on both compilers, before (note 0019's texts) and after; the compile-fail tests E9 and E15 and `core.mv_conversion_implicit_lossy` (WU8b) carry the messages themselves |

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
6. The diagnostics record (E17, criterion 11b).
7. Mutation records; limits and per-use costs; W10 after; appended sections of decisions 0150 and 0153; decision 0156.
8. Outcome with the V10 line; W5 review; fixes in their own commits; a delta review of any core fix (decision 0094 item 3); merge after `develop` is merged in and P9 passes.

WU8b, on `feature/core-mv-b`, after WU8a merges:

1. The claim line here; W10 before.
2. The kernel and the coefficient check (F1 to F8, F12, F13).
3. The target-set form, the conversions, and `truncate` (F9 to F11), with the 4.5 trace row.
4. The assembly read; limits, P2, per-use costs, W10 after; mutation records.
5. Decision 0154, the appended section of 0155, and 0156's appended section.
6. Outcome, review, merge. The rounding-scalar case of `truncate` waits for O1.

The hci-expert reads the interface before each W5 review: the WU8a header with its rejection outputs on both compilers, and the conversion and `truncate` messages of WU8b. The reading is recorded under `docs/agent/records/reviews/` as advice. It uses the checklist the W5 reviews apply (decision 0160), so that the implementer can act on its findings before the review.

Compile-time cost to every includer (decision 0113: `core.hpp` alone is about 0.91 G instructions on GCC and 0.92 G on Clang). The budget approach, with no bound (C14):

- measure `core.hpp` alone and `mv.hpp` alone, before and after each part, on both compilers;
- if a rise exceeds the spread of three repeats, attribute it with a Clang `-ftime-trace` and report it with its cause;
- keep `mv.hpp` from instantiating anything at namespace scope and from adding standard headers;
- measure separately what falls on programs that use the type (criteria 10 and 23): the instantiations of the storage base and inherited constructor per type, the catch-alls, `get`, and `apply`.

`blades<Alg>` changes what every includer of `blade_set.hpp` alone parses (it now includes `metric.hpp`, which `core.hpp` already includes), and the W10 rows say so.

## 10. Evidence

- Test output per preset for `core.mv`, `support.rational`, and the migrated suites, with the exhaustive ranges stated (F1, F11).
- Compile-fail output on both compilers for every rejection, with the line on which each message appears.
- The trace gate output in each merge message, with the 4.5 row pinned (WU8b).
- Mutation records for both parts.
- The diagnostics record (E17).
- Limits, per-use costs, P2, and W10, each as `tools/timed` runs in `runs.tsv` with their commands, and summaries under `docs/agent/records/performance/mv/`.
- The assembly read, with the commands and the hand-written comparison.
- V10 line, to fill in the outcome:
  - Verification: E1 to E16 and F1 to F13 (kinds 1 to 3 over the stated ranges). E17 is a record of readability, not verification.
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
- checks that the `blades<Alg>` migration changed no expected value, line by line against the listed commit;
- checks the diagnostics record against the compile-fail outputs it quotes.

The WU8b reviewer also:

- checks F7's bound argument and the hand values of F3, F4, and F8;
- checks the assembly read against the hand-written form;
- reruns the first-term and exactness-bound mutants;
- runs `truncate`'s checking-mode tests in a checking and a non-checking preset.

Both reviews apply the usability checklist for public interfaces (decision 0160, WORKFLOW W5), each unchecked item a finding. A fix to core code after a review gets a delta review (decision 0094 item 3). The hci-expert reading (section 9) precedes each review and is advice, not the review.

## 13. Agent cost estimate (for information only, decision 0019)

From `records/agent-cost.tsv`: the WU7a implementer with review fixes used 631k tokens and 194 tool calls, WU7b's 674k and 286. Their reviewers used 300k and 79, and 358k and 112.

| Role | Tokens | Tool calls |
| --- | --- | --- |
| WU8a implementer, with review fixes (the `blades<Alg>` migration and the diagnostics record included) | 480k to 650k | 185 to 250 |
| hci-expert reading of WU8a | 120k to 180k | 40 to 60 |
| WU8a blind reviewer, plus a delta review | 330k + 150k | 100 + 50 |
| WU8b implementer, with review fixes (assembly read and P2 included) | 500k to 650k | 190 to 250 |
| hci-expert reading of WU8b | 80k to 120k | 30 to 40 |
| WU8b blind reviewer, plus a delta review | 330k + 150k | 110 + 50 |
| Total | about 2.15M to 2.75M | about 755 to 960 |

No bound applies. The outcome of each part compares these with the actuals. This revision used about 45 tool calls; its token count is not visible to me.

## 14. Risks

1. Common-mode error between the tests and the header, both written from CONVENTIONS by agents. Each family has a route that shares no code: golden tables, hand values, the test's own inversion count, the loop form of the kernel, and cross-multiplication for the rational.
2. The test rational is a test oracle. It has its own identities, hand values, and mutants, and DoD-7 also compares it with `std::int64_t`.
3. Diagnostics are compiler text. The compile-fail tests require Galoot's literals. The few that record compiler wording (narrowing, implicit conversion) are labeled and are expected to change with a compiler.
4. GCC keeps printing blade sets through libstdc++'s wrapper (R4). The diagnostics record shows how much the other routes recover; whether more is needed is the owner's call.
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
  - names as written in the type (decision 0157 item 3; route 1 of section 5's list);
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
- Not a question, recorded: note 0019's reviewer checklist was adopted after the first revision (decision 0160, R8).
- For the owner after WU8a: whether the diagnostics record (criterion 11b) calls for more, such as names as written in the type.

## Owner rulings on O1 and O2 (2026-11-01)

The owner ratified both proposals ("proceed as recommended"; decision 0161). CONVENTIONS 4.5 (version 1.7) now says that a conversion which drops blades verifies the dropped part is zero when the scalar is exact, and that the rounding-scalar bound belongs to operations that drop part of their own result. PERFORMANCE P5 says "trivially copyable". So R1 and the conditions that WU8b's rounding-scalar behavior of `truncate` merges only when O1 is settled no longer apply: WU8b implements criterion 16 as ruled.

## Outcome, WU8a (2026-11-01, Claude, agent, Opus 5.5, cpp-expert, implementer)

Status: done on `feature/core-mv-a`, awaiting the hci-expert reading and the W5 blind review. Code and tests final at `669f7e2` (the header last changed there, in a comment; its code last changed at `878b624`); `develop` (with plan 0015) merged in at `b89b9ba`, whose full suites pass on gcc-debug (195 of 195) and clang-debug (197 of 197). This section is committed after them; the seven presets from new, empty trees and the W10 after rows run at this section's commit and are recorded in the commit that follows it.

### Departures, skipped checks, and ambiguities (C20)

1. One expected message changed, listed in `59c1057`: `core.blade_set_names_zero` (`blades<0, "e1">`) gave "blade_set: an algebra has at least one generator". A tag with no generators is not `metric_like`, so it now fails `blades`' constraint; the test requires "metric_like" and forbids `make_blades` and the index message. Every other migrated expected value and message is unchanged.
2. `59c1057`'s message miscounts the migration: blade_set_test.cpp has 21 migrated uses, not 18, and the compile-fail sources are 22 files with 23 uses (21 by the script, plus `names_zero`). History is not rewritten.
3. Named access has seven causes, not six: a name that lists one generator twice (`e11`) has its own message and test (`core.mv_get_repeated_generator`). The wide-algebra message gained a fix after a semicolon. Final wording in decision 0153's appended section.
4. The rejection of `get` is one overload with an `if constexpr` chain, not one catch-all per cause, and it declares `get`'s return type instead of form P's deduced `auto` and placeholder (decision 0153 appendix, item 4): with a deduced type, test E10's `requires`-probe would be a hard error.
5. `==` is written out and constrained by `std::equality_comparable<T>`, not defaulted (decision 0150 appendix, item 3), and the storage base takes the multivector type as a parameter so that no two types compare (item 2).
6. A GCC 16.2.1 wrong-code defect was found and worked around: the zero initializer is `coefficients{zero_at(I)...}`, not `{}` (decision 0150 appendix, item 4; lesson 0065; reproducer in `docs/agent/records/spikes/gcc-shared-array-initializer/`, no Galoot code). Not reported upstream (agents do not contact maintainers); the reproducer is kept for the owner.
7. `scalar_type` is not provided (no caller in WU8a, S16); WU8b adds it if its conversions need it.
8. Build tooling: `galoot_add_compile_fail_test` gained `ONE_ERROR` (every output line with "error:" must hold a required literal), with `core.mv_two_errors` (WILL_FAIL) showing it can fail. Messages with a `;` are passed to it with character 1, as `galoot_expected_findings` already does.
9. Lint of compile-fail sources, run by hand at each commit with clang-tidy (`.clang-tidy`, warnings as errors), because no build compiles them: a tidy check counts; the compiler's own diagnostics of the deliberate mistake (`clang-diagnostic-*`) are the failure under test and do not. Four suppressions, each with its reason beside it: `misc-include-cleaner` in `blade_set_names_zero_fail.cpp`, `blade_set_blades_not_metric_like_fail.cpp`, and `mv_not_metric_like_fail.cpp` (the failed constraint hides the header's use), and the narrowing checks in `mv_narrowing_float_fail.cpp` (the narrowing is the mistake). Pre-existing findings in touched files were fixed (six hand-written sets moved to `std::to_array` for `readability-trailing-comma`, `59c1057`). Not fixed, outside this unit's files (W9, reported): `plan_not_metric_like_fail.cpp` and `plan_map_not_metric_like_fail.cpp` have the same include-cleaner artifact.
10. Commits that fail by design, each saying so (W2): `380a355` and `15dd5a0` (tests before their code; the gates passed on their trees, extracted with `git archive`), and `461e3dd` (the GCC regression test before its fix; fails core.mv on gcc-debug only). `669f7e2`'s gates ran at `12e0e22`, a records-only commit, because untracked records were present at `669f7e2`.
11. `b94e50a` committed the diagnostics record's `program.cpp` unformatted (it is under `docs/`, outside `format-check`); `ce9d15e` formats it and regenerates the texts.
12. P13 lapses: `clang-format` runs, the preprocessing check of `<concepts>` (`-E`, line counts), and `git archive` extractions were not run through `tools/timed`. Every build, test, probe compile, lint, and measurement was.
13. The header-cost rise (+0.45 % and +0.53 % for `core.hpp`) exceeds the spread of three repeats; its attribution to parsing `mv.hpp` and to `metric.hpp` in `blade_set.hpp` is by construction. A Clang `-ftime-trace` was taken and was too noisy to attribute 4 to 5 M instructions.
14. Measurements ran on the shared machine with other workstreams active (load average 4 to 5 at the start), pinned to CPU 10 with an idle sibling (at most 0.01 s busy); instruction counts are compared, not times.
15. Observed, not explained: with the rational's sum guard removed, Clang 23.1.4 still printed the call of `rational_failed` with its message after the overflow of `2^62 + 2^62` in constant evaluation, so `support.rational_consteval_add` passed on clang-debug with that mutant; GCC stopped at the overflow. `support.rational_rejects_add_far_above` (`max + max`) kills the mutant on both (`a6037b0`).

### Commits

| Commit | Content |
| --- | --- |
| `0782a8e` | W10 before rows (fresh gcc-debug and clang-debug trees at `965059e`; 93 and 95 compile rows; 140 of 140 and 142 of 142) |
| `59c1057` | `blades<Alg, "...">` and the migration (W9 list in the message; see departures 1 and 2) |
| `42eed2f` | `test/support/rational.hpp` and `support.rational` with its rejections |
| `70cacfd` | merge of `develop` (decision 0161, O1 and O2) |
| `380a355`, `d6a8a27` | tests of the type (E1 to E5, E8, E14, E15), then `mv.hpp`'s concepts, storage, type, count constructor, and checks |
| `461e3dd`, `284c7e7` | the GCC regression test, then the zero initializer written per place |
| `15dd5a0`, `878b624` | tests of named access (E6, E7, E9, E10, `bare_user.cpp`), then `get` and `has_blade` |
| `b94e50a`, `ce9d15e` | the diagnostics record (E17) |
| `4150039`, `a6037b0` | test-only additions for mutants: `core.mv_one_too_few`; `add_far_above`, run-time concept checks, the constexpr variable of the regression test |
| `152514a`, `bc1493e`, `0acef07` | the mutation driver, its runs, the survivors; decisions 0150 and 0153 appended; lesson 0065; the GCC reproducer |
| `669f7e2`, `12e0e22` | the measured limit in `mv`'s comment; the cost records and decision 0156 |
| `b89b9ba` | merge of `develop` (plan 0015) |

### Criteria

1. Met at `d6a8a27` and after: E1 over five scalars for the point, line, plane, motor, the scalar "1", a conformal(3) vector, the sparse set in R(15,0,1), and the empty set; sizes 48, 24, 64; template arguments for double and the rational, and the empty and sparse sets; `!is_convertible<T, mv>` for one blade.
2. Met: zero by default at compile time and over a 0xA5 pattern at run time, five scalars, also after a constant evaluation wrote into a zero value (from `284c7e7`).
3. Met: one value per component in layout order, five scalars, compile time and through volatile; `core.mv_too_few` ("3 == 6"), `core.mv_one_too_few` ("3 == 4"), `core.mv_too_many` ("7 == 6"), each one error on both compilers; narrowing (`int` into double, `double` into float) is the compiler's narrowing error, "narrow" on both; `core.mv_scalar_implicit` records the compiler's conversion error.
4. Met: a change at each of 6 and 8 places is unequal, five scalars; -0.0 == +0.0 and a NaN unequal for double; no `==` across layouts, algebras, or scalars.
5. Met: the concepts on every type of the criterion, at compile time and (from `a6037b0`) at run time; `core.mv_not_ring_scalar` and `core.mv_scalar_without_multiplication`, one error with the literal.
6. Met: each layout check fires alone on its hand-written set, one error, both compilers (eight tests); every set from `full`, each `of_grade`, a merge, and named sets for n = 1 to 8 accepted.
7. Met at `878b624`: 20 names of the rigid(3) point, line, plane, motor and a conformal(3) layout with e45, over five scalars, by the test's own inversion count; lvalue, const lvalue, and rvalue categories; `has_blade` false without error for every case of E7.
8. Met: eight compile-fail tests (seven causes, two for "not a name"), each one error, its message, none of the other six nor `blades:`, on both compilers.
9. Met at `42eed2f` (and `a6037b0`): hand values; field identities on 2,000 seeded triples (seed from doctest's `rand_seed`, printed); equality against cross-multiplication; six run-time rejections and two compile-fail; `is_exact`.
10. Met: limits n = 10 (GCC) and 8 (Clang), stated in the `///` comment, not below 6; per-type, per-construction, and per-get costs; header costs (decision 0156, `docs/agent/records/performance/mv/`). W10 after: in the commit that follows this section.
11. 11a met at `59c1057` (departure 1 for `names_zero`); `core.blade_set_blades_not_metric_like` on both compilers. 11b met at `b94e50a` and `ce9d15e`, regenerated at `12e0e22` for `669f7e2`: the record is `docs/agent/records/diagnostics/wu8/` and is quoted below; every route marked "in WU8a" is in the header (2, 3, 4, 5, 6 in the tests' declarations, 7).
12. Met at `c3d4e45` (recorded in the commit after it): the seven presets from new, empty trees each built `all` and passed every test: gcc-debug 195 of 195, gcc-release 195 of 195, gcc-asan 187 of 187, clang-debug 197 of 197, clang-release 197 of 197, clang-asan 197 of 197, lint 197 of 197 (its clang-tidy clean over every built unit), `core.bare_compile` among them. `all_verify_interface_header_sets` built on gcc-debug, clang-debug, and lint, and `format-check` passed on all three. The W10 after rows are from the gcc-debug and clang-debug trees (`docs/agent/records/performance/mv/README.md`, W10). The job pool had 23 of 23 slots free before and after.

Mutation (DoD): 28 mutants on gcc-debug and clang-debug in both modes; every one killed by a test except `shared_zero_initializer` on clang-debug, which is equivalent there (`docs/agent/records/mutations/0016-mv-a/survivors.md`).

### The diagnostics record (criterion 11b), quoted

Lines / longest line in characters (installed path length for after), GCC 16.2.1 and Clang 23.1.4:

| Mistake | Compiler | Before, form R | Before, form P | After, WU8a |
| --- | --- | --- | --- | --- |
| `const Point s{1, 2, 3};` | GCC | compiles silently, w = 0 | compiles silently | 10 / 468, one error, "(3 == 4)" |
| | Clang | compiles silently | compiles silently | 12 / 194, one error, "'3 == 4'" |
| `get<"e14">(l)` (other orientation) | GCC | 25 / 677 | 9 / 534 | 10 / 504, its own message |
| | Clang | 13 / 301 | 8 / 218 | 9 / 248 |
| `get<"e1">(l)` (absent) | GCC | 25 / 676 | 9 / 533 | 10 / 503, its own message |
| | Clang | 13 / 300 | 8 / 217 | 9 / 247 |
| `const Point s = q;` (lossy) | GCC | 4 / 799 | 9 / 892 | 4 / 811, the compiler's message (WU8b adds the catch-all) |
| | Clang | 10 / 301 | 8 / 268 | 16 / 282, the compiler's message; the notes name `Motor` (aka `mv<rga3, motor_layout, double>`) |

The wrong count is now caught; named access names each cause; GCC's lines stay long because it prints the layout's masks through libstdc++'s wrapper in every instantiation context, which only route 1 (names as written in the type) would shorten. For the owner (section 16): whether this calls for more.

### V10

- Verification: E1 to E16 passed (contract, independent-value, and identity tests) on gcc-debug and clang-debug at `b89b9ba`; the seven presets follow. E17 is a record of readability, not verification.
- Validation: the oriented names of rigid(3) against CONVENTIONS 1.6 (E6, the test's own inversion count against the table's values); conformal(3)'s e45. The products are WU8b's.
- Scalars: the rational's exactness rests on its overflow checks (tested at their bounds); the IEEE behavior of `==` on double is checked for signed zero and NaN.

### Open items

1. The hci-expert reading and the W5 blind review (section 12), with the usability checklist.
2. The owner's question on the diagnostics record (section 16).
3. The GCC defect is unreported upstream; the reproducer is ready.
4. Knowledge base: note `cpp-language/lang-gcc-shared-array-initializer.md` and exercise `exercise-archive/cpp-shared-array-initializer.md` on branch `kb/cpp-shared-array-initializer` (worktree `knowledge-base-worktrees/cpp-shared-array-initializer`, commit `2919f6c`), for the coordinator to merge.
5. Pre-existing lint artifact in the two plan not-metric-like sources (departure 9).
6. WU8b, after this part merges.

### Agent cost

About 220 tool calls by the implementer; the token count is not visible to me. The plan estimated 480k to 650k tokens and 185 to 250 tool calls with review fixes.

### Review fixes (2026-11-01)

The W5 blind review (`docs/agent/records/reviews/2026-11-01-wu8a-mv.md`, merged at `e541ab4`) and the hci-expert reading (`docs/agent/records/reviews/2026-11-01-wu8a-hci-reading.md`, merged at `d9e1521`) were fixed in one round, each change in its own commit, tests before the code they judge.

Departures of this round (C20):

1. Blind F4 (a scalar that is not semiregular) is met by a test of the compiler's wording (`core.mv_scalar_without_default`), not by a literal message; the alternative (a weaker constraint and an assertion in the body) was not tried, for the reason in decision 0150's review section, item 5.
2. `ring_scalar<bool>` stays true and is documented in the concept's comment, not rejected (blind F10).
3. Blind F10's `[[nodiscard]]` on `zero_at` no longer applies: the pack-of-T storage (H1) needs no `zero_at`.
4. The layout messages say "the algebra does not have" rather than "beyond e_n", which also answers blind F10's point that messages wrote e_n where comments write e1 to en.
5. `11feaf6`'s tests were run on the working tree before the commit (core.mv, both compilers); its full suites ran at `b123788`, whose tests are the same.
6. Probes outside a build or test, all through `tools/timed`: `h5:p10`, `h5:p11` (the claims of the reworded comments), `review-f7:p9:*` (which written places trigger the GCC defect).
7. **The lint preset failed from `ed8be5a` to `f066f02`, unseen at the time.** clang-tidy's `bugprone-crtp-constructor-accessibility` read the storage base, whose first parameter was then the `mv` type, as a CRTP base with public constructors. The per-commit lint step built the lint preset and then printed `$?` after a command substitution in the same `echo`, so it printed 0 for a failed build: the builds' own logs show the failure at `ed8be5a`, `3d8df77`, `0b316df`, and `b123788`. The new lint tree of the final check at `f066f02` found it; `af447ae` fixes it (the base takes the algebra, layout, and scalar) and the lint preset passes there. The earlier lint steps of this unit printed the exit status directly or read the logs, and their logs show no failure (`d6a8a27`, `284c7e7`, `878b624`, `669f7e2`). The per-commit step now prints the exit status before anything else.
8. Lint by hand of every compile-fail source at the final revision, not only the touched ones, finds tidy-check findings (trailing commas, include-cleaner) in 22 sources that this unit did not change (`git diff 965059e` is empty for each): `blade_product_overflow_fail.cpp`, `blade_product_sum_overflow_fail.cpp`, `blade_set_full_wide_fail.cpp`, `blade_set_index_of_bad_sign_fail.cpp`, `blade_set_index_of_repeated_adjacent_fail.cpp`, `blade_set_index_of_repeated_mask_fail.cpp`, `blade_set_merge_repeated_in_second_empty_first_fail.cpp`, and fifteen `plan_*_fail.cpp`. A defect outside the unit (W9), reported and not fixed here; every source this unit added or changed is clean.
9. `develop` has not moved since `b89b9ba` merged it (`ebdd06d`), so the P9 checks run on this branch's head as it is. (Superseded: `develop` moved to `eb20336` during the delta review; see "Delta-review fixes" below.)

| Finding | Fix | Commits |
| --- | --- | --- |
| B1 (blind F1): four compile-fail tests found their literal in their own path | `galoot_add_compile_fail_test` stops configuring when MESSAGE or REQUIRE occurs in the source path (`galoot_check_literals_outside_path`), shown to fail by `tools.compile_fail_literal_in_path_rejects`; the tests require each compiler's words. Defect outside the unit (W9), fixed because the check catches it: `core.metric_like_no_generators`, `core.plan_not_metric_like`, `core.plan_map_not_metric_like` | `bf1ac75` |
| B1b (blind F2): a double into a float layout is two errors | The intended error is the language's narrowing error; the second is the compiler's -Wconversion warning made an error on the same line. `core.mv_narrowing_float` requires both, and ONE_ERROR allows exactly those two | `bf1ac75`, `162cd96` |
| B3 (blind F3) | two-lane scalar without == (ring_scalar true, Line made and read, not equality_comparable); `!is_convertible_v<T, Line<T>>`; `core.mv_one_value` ("1 == 6") | `14bff60` |
| Blind F4 | `core.mv_scalar_without_default`, compiler wording, labeled | `14bff60` |
| Blind F9 | (2^62 / 3)(3 / 2^62) == 1 | `14bff60` |
| HCI H1 (F1) | constructor parameters a pack of T; narrowing says "from 'int' to 'double'" | `162cd96` (test), `ed8be5a` |
| HCI H2, H3 (F2, F3), blind F10 | named layout rules; "layout" in every message; a fix after each semicolon | `294d265` (test), `3d8df77` |
| HCI H4 (F4) | `scalar_type` | `7d46580` (test), `0b316df` |
| HCI H5 (F5), blind F6, HCI F13, F14, blind F10 | comments say what is shown: braces against parentheses, the array probe, where Clang names aliases, a layout does not keep its algebra, printing through `coefficients`, bool, the forward declaration's comment | `b123788` |
| Blind F7 | the defect comment says what was observed; the regression test writes every place; lesson 0065 corrected by a dated section; the knowledge-base note narrowed (`kb/cpp-shared-array-initializer`, `227c20f`, `ad3ae5b`) | `ed8be5a`, `11feaf6`, this record |
| Blind F10, outcome count | "Four suppressions" in departure 9 above; the helper's comment line within 80 columns | this record, `bf1ac75` |

Header changes after the review, for the delta review (decision 0094 item 3): `ed8be5a` (the storage base, its constructor's parameter list, the zero initializer's comment), `af447ae` (the storage base's template parameters: algebra, layout, and scalar in place of the `mv` type), `3d8df77` (the three layout checks, their members and messages), `0b316df` (`scalar_type`), and `b123788` (comments only: the head comment, `ring_scalar`, the forward declaration, `mv`, the K-value constructor, the count catch-all). `blade_set.hpp` did not change.

Mutation after the fixes (`9b42298`, `docs/agent/records/mutations/0016-mv-a/survivors.md`): 36 mutants on gcc-debug and clang-debug in both modes; all killed by tests except `shared_zero_initializer` on clang-debug, which is equivalent there. `mv_alg_unconstrained` and `blades_unconstrained` are killed on both compilers by `core.mv_not_metric_like` and by `core.blade_set_blades_not_metric_like` and `core.blade_set_names_zero`.

The diagnostics record was regenerated for the reviewed header (`b123788`); `summary.tsv` is unchanged.

Deferred, with the unit that takes them (recorded, not changed):

- Blind F2 (two errors for a double into a float layout): recorded as compiler behavior above; nothing in the library can remove the compiler's -Wconversion error.
- Blind F5 (a copy from a class derived from `mv` reaches the count message), blind F8 (positional `get<0>` gives a deduction failure in internal terms): WU8b, with the conversions and the catch-alls for single arguments.
- HCI F7 (the absent message's list of names for a computed layout), F8 (reference, const, and bool scalars), F10 (double braces and whole arrays): WU8b.
- HCI F9 (a tag without `value`): the owner, or WU16 with the model tags.
- HCI F11, F12: owner decisions (below).
- HCI F13 and F14: commented now (`b123788`); WU9's mixed-algebra messages and WU15's `format` take the rest.
- HCI F15 (GCC 16 drops `explicit` from an inherited constructor template whose `explicit(bool)` reads its own parameters): affects only classes derived from `mv`; with F6.
- HCI F16 (commas between names in `blades`): WU4's parser, a later unit.
- HCI F17: recorded as not recommended.
- The plan's first usage snippet (section 4) puts `gc::get<"e23">(axis) = 2.0;` at namespace scope, where it is not a statement; inside a function it compiles. It matters when it becomes documentation (W12, WU16).

Owner decisions, not implemented:

1. HCI F6: named model classes deriving from `mv` (`struct Line : mv<rga3, line_layout, double> { using mv::mv; };`). Measured by the reading: GCC's line for the lossy assignment falls from 811 to 98 characters and `get`'s from 499 to 257; the cost is two types per layout (results of products are `mv`s and need a conversion into the named class), `multivector` accepting derived classes, and a workaround for GCC 16 dropping `explicit(bool)` from an inherited constructor template (F15).
2. HCI F11: `has_blade<Line, "e14">` is false though the line holds the blade e14 in the other orientation; rename to say "this oriented name" (`has_name`), or keep the name and document it.
3. HCI F12: whether `mv` is tuple-like, with a positional `get<0>` beside `get<"e41">`, and structured bindings; today `auto [all] = p;` binds the whole array.

Checks at the end of the review fixes, at `9d6bb48` (build inputs those of `af447ae`; recorded in the commit after it): the seven presets from new, empty trees each built `all` and passed every test, gcc-debug 199 of 199, gcc-release 199 of 199, gcc-asan 191 of 191 (plan 0015's canary is not registered there), clang-debug 201 of 201, clang-release 201 of 201, clang-asan 201 of 201, lint 201 of 201 with clang-tidy clean over every built unit (no failed step in its log); `all_verify_interface_header_sets` and `format-check` passed on gcc-debug, clang-debug, and lint; `core.bare_compile` passed in each. The job pool had 23 of 23 slots free before and after. The delta review of the header changes listed above is pending.

### Delta-review fixes (2026-11-01)

The delta review (`docs/agent/records/reviews/2026-11-01-wu8a-delta.md`, merged at `d203322`) accepted the review fixes after one test-only fix.

Correction, dated 2026-11-01 (D10): the review-fixes section above says the diagnostics record "was regenerated for the reviewed header (`b123788`)". The texts committed at `63513f1` were regenerated for `af447ae`, as the record's README says; `b123788` was the regeneration before it.

| Finding | Fix | Commits |
| --- | --- | --- |
| D1 (blocks): the distinct storage base had no test that could fail | core.mv tests the expression `a == b` in a concept, `has_equality_with`, for a rigid(3) point against an R(4,0,0) point, a line against rigid(3)'s bivectors, and a line over double against one over float; the `equality_comparable_with` assertions stay as a record of the standard concept's answer | `b2edac9` (test-only) |
| D2: the scalar's constraint in the template head | `names_a_line<without_default>` is false, `names_a_line<double>` and `<std::string>` true | `b2edac9` |
| D3: decision 0150 named the superseded base | a dated addendum for `af447ae` | this record |
| D4: the range message gave no fix | "...; use a layout whose blades are all in this algebra" | `067f53c` (test), `9f7ea99` |
| D6: ONE_ERROR's comment overstated the check | "allows only error lines that hold one of those two texts; it does not count the lines" | `067f53c` |
| D7: get at n = 9 | `get<"e19">`, `"e9"`, `"e123456789"` in R(9,0,0) | `b2edac9` |
| D8: the rational sum's common-factor step | 1/2^62 + 1/2^62 = 1/2^61 | `b2edac9` |
| D10 | the correction above | this record |

Deferred or recorded as nits (not changed):

- D5: GCC names the rule too, in its "evaluates to false" note after the masks, and the GCC side is untested (the mutant `repeat_rule_renamed` survives on GCC). A nit; the rule names are tested on Clang.
- D9: the configure-time check finds a literal only when it lies wholly inside the source path; a literal straddling the path and the compiler's decoration, or naming an included header's path, would pass it. No present literal is of either kind (the reviewer's decoys fail all 103 compile-fail tests). A decoy run registered as a test would close it; later work.
- D11: the K-value constructor copies each by-value parameter into the array; `std::move(values)...` would save one copy per coefficient for a scalar that allocates. When such a scalar has a user.
- D12: the 22 untouched compile-fail sources with lint findings, as departure 8 above (W9, reported).

Header change after the delta review, for a delta review (decision 0094 item 3): `9f7ea99`, the range message's text only.

`develop` moved to `eb20336` (the run-log compaction, merged there at `3f6bf9e`) and is merged into this branch in the commit that carries this paragraph: `runs.tsv` and `test/CMakeLists.txt` merged without conflict, and `tools/runlog check` on the merged tree reports the one era matching the history, 1,727 kept rows present once, and no removed row present, so no `tools/runlog prune` was needed. This replaces departure 9 of the review fixes.
