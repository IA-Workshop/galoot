# 0016 WU8: the multivector type, its scalars, named access, and applying plans

- Status: claimed 2026-11-01 (planner: Claude, agent, Opus 5.5, cpp-expert role, with the hci-expert and math-expert role files for the interface and the mathematics). Not started.
- Unit: WU8 of plan 0001 (`mv.hpp`), in two parts run one after the other: WU8a on branch `feature/core-mv-a` (worktree `.claude/worktrees/core-mv-a`), then WU8b on branch `feature/core-mv-b` (worktree `.claude/worktrees/core-mv-b`), branched from `develop` after WU8a merges. Each part has its own W5 review and merge. WU8b is claimed by a dated line in this plan when WU8a has merged, as plan 0013 did for WU7b.
- Decision numbers 0150 to 0159. In use or claimed elsewhere: 0110 to 0118 (WU7), 0120 to 0125, 0130, 0131 and later (plan 0015, open-ended from 0131), 0140 and 0141. If plan 0015 reaches 0150, the later of the two claims renumbers before it merges.
- Depends on: WU7 (plan 0013, merged: `product_plan` for the four binary operations and `unary_map` for the five unary maps, typed onto canonical result sets) and Spike F (research note 0012). Spike C (note 0010) and decision 0033 fix the diagnostic form.
- Goal: the type a user holds. `mv<Alg, Blades, T>` stores one coefficient of a scalar type T on each oriented blade of a blade set, as a trivially copyable aggregate; the scalar concepts say which T are accepted; the type rejects a blade set that is wrong for its algebra; `get<"e41">(x)` reads and writes a coefficient by its oriented name; `==` compares exactly; a plan or map is applied to stored coefficients with straight-line code whose coefficients are exact in the scalar; a conversion between blade sets is explicit, and the lossy one is `truncate<Blades>(x)`, checked in checking mode. DoD-7 holds across five scalars.

## 1. Departures, ambiguities, and owner questions (C20)

Each item states the reading this plan follows. An owner answer that differs changes the named section and nothing else. Items 1 to 11 are owner questions with a recommended answer; items 12 to 21 are readings the work follows unless the owner says otherwise.

### Owner questions

1. Two parts, and where the line falls (owner question 1). Plan 0001 lists WU8 as one unit of size M; WU6 and WU7 each ran as two, at 550k to 670k tokens per implementer. WU8 holds a public type, three concepts, set checks, named access with its messages, a test-only rational, the kernel that applies plans, two conversions, checking mode, and DoD-7. The brief proposed WU8a = concepts, aggregate, exact equality, test exact scalar, repeated-mask check, and WU8b = `apply`, `truncate`, named access, DoD-7. This plan moves named access into WU8a. Reasons: reading and writing a coefficient by name is part of the type a user holds, and WU8a's tests and its review read far better with it (a test that checks a Line's e43 slot by `get<"e43">` says what it means; one that reads `coefficients[2]` does not); named access depends only on the type, not on `apply`; and the split then follows the two reviews' different emphases, WU8a on the interface and its messages (hci-expert reading), WU8b on numerics and generated code (exactness, evaluation order, assembly). Recommended: this split. The owner may take the brief's split instead; the criteria move with the content.
2. Catch-all messages for WU8's own public functions are written in WU8, not in WU15 (owner question 2). Plan 0001 gives WU15 "catch-all overloads with literal messages". Decision 0033 has fixed the form (P for functions, plain for conversion functions). A function and its misuse messages are one interface, and writing the compile-fail tests twice (raw now, rewritten when WU15 adds catch-alls) costs more than writing them once. Reading followed: WU8 adds the catch-alls of `get` (WU8a), the conversion and `truncate` (WU8b); WU15 keeps `format.hpp` and the catalogue-wide review of DoD-19, and adds the catch-alls of the public products (WU9 to WU13) unless those units add their own. For the type itself a class template cannot have a catch-all: an inadequate scalar is rejected by the constraint, whose report names `ring_scalar` and the first failing requirement (section 3). This is read as meeting DoD-7's "rejected with a message in the terms of the algebra" through the concept's name; a literal message would need a `static_assert` in an unconstrained template, which S7 forbids. Recommended: yes.
3. An explicit conversion between blade sets that drops nothing (owner question 3). Plan 0001's WU8 line names `truncate` only; STYLE S8 says "Conversions between blade sets ... are `explicit`; a lossy conversion has its own name", DoD-20 asks that conversions not be implicit, and DoD-19 lists "implicit lossy conversion" as a misuse. A user who forms p ∧ q (stored on the canonical bivectors) and wants a Line (stored on e41, e42, e43, e23, e31, e12) needs a reorder and sign change that loses nothing; `truncate` would do it, but its name says something is dropped. Reading followed: WU8b adds `static_cast<Target>(x)` (an `explicit` conversion function, the only conversion an aggregate can have), allowed when the target holds every blade of the source, in either orientation; a target that drops a blade is rejected with the message of note 0010 pointing to `truncate`. Planning probe (section 10, item 3): `static_cast<Line>(m)` and `Line l(m)` work on both compilers, `Line{m}` does not, and its message ("no viable conversion ... to 'double'") does not lead to the fix; documentation shows the cast. Recommended: yes.
4. No default scalar type (owner question 4). The design study writes `scalar T = double`. A default is friendlier to type, but C8 asks that behavior be declared rather than left to defaults, and the scalar decides rounding, exactness, and what checking mode checks. Models (WU16) may give their named types a default if the owner wants one there. Recommended: no default in the core.
5. A default-initialized multivector is zero (owner question 5). `Line l;` with a plain `std::array` member leaves the coefficients indeterminate, as `std::array` does, and reading one is undefined behavior that no sanitizer of the matrix detects. A default member initializer `coefficients{}` keeps the type an aggregate and trivially copyable (STYLE S8, P5), costs a zero fill only where an object is default-initialized, and is folded away by the optimizer when the object is then written (to be confirmed in the assembly read, criterion 24). It does make the type not trivially default-constructible. Recommended: zero.
6. Exactness of the coefficient in the scalar (owner question 6; plan 0011 section 12, "the conversion of `coefficient_type` into the scalar"). A plan coefficient is an exact `std::int64_t`. In `float` it is exact only up to 2^24 in magnitude, in `double` up to 2^53, in `short` up to 32767. A coefficient that rounds would make a structure constant inexact (C7) by an implicit lossy conversion (C8). Planning finding: with Gram entries up to `max_form_entry` = 32767, such coefficients exist: e12 e12 = −g11 g22 in diag(24929, 673) is −(2^24 + 1), and e1234 e1234 = 32767^4 in diag(32767, 32767, 32767, 32767) is an odd integer above 2^53. Reading followed: applying a plan to a scalar T checks at compile time that every coefficient of the plan is exact in T whenever `std::numeric_limits<T>` is specialized: |c| at most `max()` for an integer type, |c| at most 2^digits for a floating type of radix 2 (2^digits itself is exact). For a scalar without a specialization (`std::complex`, a user's type), exactness is a documented requirement of the scalar, not checked. Alternatives: a Galoot trait as an extension point (C5) that `std::complex` and user types can specialize, which is one more published interface; or no check at all. Recommended: the `numeric_limits` rule, with the gap for `std::complex` stated in the comment and in decision 0151.
7. Checking mode of `truncate` on a rounding scalar (owner question 7, a C8 provision, so the owner's). Decision 0010: exact scalars check for exact zero; rounding scalars check "that the discarded magnitude is within the plan's forward error bound". A standalone `truncate<Blades>(x)` has no plan: it cannot know the error of its input, so an exact-zero test would abort on correct rounded inputs (V13), and there is no bound to test against. Options: (A) `truncate` checks exact scalars only, says so in its `///` comment, and the bound check for rounding scalars comes with the fused operations that know their plan (`transform`, WU13; M3 "checking mode"); (B) exact-zero checks for every scalar (false aborts, against V13); (C) a second overload `truncate<Blades>(x, bound)` for rounding scalars, with the one-argument form on a rounding scalar rejected at compile time; it needs a magnitude and an order on T, which `ring_scalar` does not require. Recommended: A, recorded as the owner's ruling on how C8 applies to a conversion that has no plan.
8. How a scalar is known to be exact (owner question 8). Reading followed: `std::numeric_limits<T>::is_exact`, the standard's own published interface (C5): true for the integer types, false for floating types and for any type without a specialization, such as `std::complex<double>`. The test rational specializes `std::numeric_limits`, which the standard allows for a program-defined type. A user's exact type without a specialization is treated as rounding: never checked, never falsely aborted. The named concept is `exact_scalar<T>`. Recommended: yes.
9. `get` on the opposite orientation (owner question 9; note 0012, open question 1). `get<"e14">` on a Line that stores e41 = −e14 could return −c by value, but then it could not be written through, and it would hide CONVENTIONS 1.6. Reading followed: rejected, with its own message distinct from "no blade of this name". Recommended: reject.
10. `truncate`'s argument (owner question 10). CONVENTIONS 4.5 fixes `truncate<Blades>(x)`, a blade set. Once models exist, `truncate<rga3::Line>(m)` would read better; taking a type is a conventions change (C10). Reading followed: `truncate<Blades>(x)` as specified; the type form is noted for WU16. Recommended: keep 4.5 as written now.
11. `blades<N, "...">` repeats n beside the algebra (owner question 11). A user writes `mv<rga3, blades<4, "e41 ...">, double>` although `rga3` knows n = 4. Decision 0090 left `blades<Alg, ...>` for a unit that uses it. A second spelling now would give one concept two names (C12); the mismatch is caught (a name beyond the algebra's n is rejected by `mv`, with its message). Reading followed: keep `blades<N, ...>`; revisit when WU16 writes the models' layouts. Recommended: keep.

### Readings

12. `apply` stays in `detail`. Plan terms and maps are in `detail` (decision 0015), so the kernel that applies them is too, under the roadmap's name `apply`; WU8 adds no public product. WU9 and WU10's `gp`, `wedge`, `reverse`, and the rest are one-line callers of it. The name shares a spelling with `std::apply`; every call is qualified, and the hci-expert reading may rename it.
13. Equality. A defaulted `operator==` compares two multivectors of the same type coefficient by coefficient; it is deleted, by the language rule for defaulted comparisons, when T has no `==`. STYLE S13 ("each overloaded operator is a one-line alias of a named function") is read as covering the operators of CONVENTIONS 7.1, which `==` is not; a named `equal` would be a layer with no user (C12). Multivectors on different blade sets, algebras, or scalars are not comparable without an explicit conversion. With a floating scalar `==` is IEEE equality (−0.0 equals +0.0; a NaN equals nothing), and the comment says so.
14. DoD-7 in M2. DoD-7 says "one plan passes" with each of the five scalars; DoD-9 asks tolerances from forward error bounds, which M3 builds ("V12 double tests with forward bounds"). Reading followed: WU8b runs the same plans over all five scalars with inputs for which every product and every partial sum is an integer of magnitude below 2^24 (a bound proved in the test's comment from the input range and the term counts), so that every scalar's result is exact and is compared with `==` against the exact reference (V11, and V12 with the tightest tolerance, zero, justified by that bound). Tests with nonzero tolerances wait for M3.
15. The test rational is not WU5. Decision 0011 asks for an owned, test-only, overflow-trapping int64 rational; DoD-5 asks that it be added "with no diff under `include/galoot/core/`", as evidence for C5. WU5 (`detail/rational.hpp`, moved to M5 by decision 0094) is core arithmetic for the change of basis, with other requirements (compile-time only, form entries). Reading followed: WU8a writes the test scalar under `test/support/`, outside the core; whether WU5 later shares any code with it is WU5's question.
16. Repeated masks (decisions 0092, 0093, 0102 item 10, 0111 item 7). `mv` rejects a blade set that lists one mask twice, in either orientation, and checks signs and range, once per set, in the type (decision 0092, option 3). Direct form first (W4): compare every pair, O(K²) for K blades. It is measured (criterion 9); if it lowers a ceiling that a present use reaches, the candidate is to test canonical order first (a set strictly increasing in canonical order cannot repeat a mask, O(K), which covers every generated result set) and compare pairs only otherwise, by a decision with the measurement. `index_of` keeps its own checks (decision 0093); simplifying it is not part of this unit.
17. Result sets. `apply` writes a binary product onto the plan's result set and a unary map onto the map's result set, both canonical with sign +1, so that the result holds exactly the blades that can be nonzero (C6, DoD-4). It also has a form that writes onto a given target set that holds every blade of that result, which the conversion and `truncate` use (as the identity map) and which WU10 can use so that `reverse` of a Line is a Line. What the public unary operations return is WU10's choice; the hci-expert's view is recorded there as a note: a user expects `~line` to be a Line.
18. Evaluation order and the first term (V14). Each term contributes c · (x_a · y_b): the inputs are multiplied first, then the coefficient, with c = +1 and −1 applied as addition and subtraction. Each output's first term in plan order is assigned rather than added to zero, because under IEEE semantics 0.0 + x cannot be simplified to x and the compiler must keep the addition (planning probe, section 10, item 2). The `///` comment of `apply` states both, so that a difference from another implementation has a declared cause. The negation needs unary minus on T, which `ring_scalar` therefore requires.
19. M2's "first assembly read" is taken here, because `apply` is the first code Galoot generates for run time. It is a read of the -O2 code of a few rigid(3) pairs (criterion 24), not DoD-24 in full, which stays with WU17.
20. Names. Proposed, for the hci-expert reading and the implementer to confirm or change in decision 0150: `mv`, its member `coefficients`, `algebra_type`, `scalar_type`, and `layout` (the blade set, the word `blade_set.hpp`'s opening comment uses: "the storage layout of a named type"); concepts `ring_scalar` and `has_blade` (named in decision 0033), `exact_scalar`, `multivector`; functions `get`, `truncate`. Message prefixes follow the headers' practice ("mv:", "get:", "truncate:", "conversion:"). Each member exists only if a present caller uses it (S16).
21. Overlap with work in flight (W7). Plan 0015 (`feature/plan-limits-certified`) changes `plan.hpp`'s `///` comments, adds blocks to `test/CMakeLists.txt`, moves scripts into `docs/agent/snippets/`, and appends to `runs.tsv`. WU8 does not edit `plan.hpp` (it reads `product_plan` and `unary_map` only; any helper it needs, such as the first-term flags or the largest coefficient of a plan, lives in `mv.hpp`), and does not touch `docs/agent/snippets/`. Both add blocks to `test/CMakeLists.txt` and rows to `runs.tsv`: adjacent edits, resolved by merging `develop` into the branch before review (W7), keeping both sides' rows. WU8 adds `mv.hpp` to `CMakeLists.txt`'s header set and to `include/galoot/core.hpp`, which plan 0015 does not touch. Plan 0015's canary counts constant-evaluation work of plan units; if its units include `core.hpp`, `mv.hpp` adds parsing but no constant evaluation to them (inferred, to be checked against the canary when both have merged).

### Planning evidence

22. The probe results of section 10 come from throwaway files in the planner's scratch directory, compiled with `tools/timed` against `develop`'s headers at `7da09f1`; their 44 rows were appended to `runs.tsv` in the main checkout and are not committed, because this commit holds the plan alone. They are planning values, not reproducible as committed (V10). The unit recomputes every value it relies on from committed references. Hand-derived expected values in section 7 are marked "planning value" and are checked by the implementer against the reference named beside each before a test relies on them.

## 2. Background for the reader

A multivector of a named type stores one coefficient per blade of its blade set, in the set's order, and a coefficient on an oriented blade s e_A multiplies s e_A (CONVENTIONS 1.6). A rigid(3) line stores six numbers on (e41, e42, e43, e23, e31, e12) = (−e14, −e24, −e34, +e23, −e13, +e12), so the coefficient written on e41 multiplies −e14 and needs no hidden negation when it matches a published formula.

A plan (plan 0013, section 2) is the list of terms (a, b, out, c) of a product of two blade sets, typed onto a result set R in canonical order. Applying it to x and y computes, for each output position, the sum over its terms of c · x_a · y_b. For the rigid(3) points p = (1, 2, 3, 1) and q = (4, 5, 6, 1) on (e1, e2, e3, e4), the geometric product lands on R = (1, e12, e13, e14, e23, e24, e34): the scalar is p1 q1 + p2 q2 + p3 q3 = 32 (e4 squares to 0), and each bivector e_ij gets p_i q_j − p_j q_i, giving (32, −3, −6, −3, −3, −3, −3). Converted to a Line, the bivectors read (e41, e42, e43) = (3, 3, 3), the direction q − p, and (e23, e31, e12) = (−3, 6, −3), the cross product (p1, p2, p3) × (q1, q2, q3): the reordering and the signs of the Line's layout come from the oriented names alone. (Planning values; the test recomputes them as in section 7, F3.)

`truncate<Blades>(x)` keeps the blades of x that Blades holds and drops the rest; in checking mode, for an exact scalar, it verifies that each dropped coefficient is zero (CONVENTIONS 4.5). The motor (s, l, a) on (1, e41, ..., e12, e1234) truncated to a Line keeps l, and is checked to have s = a = 0.

## 3. The user-facing surface

These snippets show the surface WU8 gives a first-time user. They are examples in this plan only; when user documentation is written, its examples are compiled and run as tests (W12). Comments on the error lines quote the proposed messages.

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
inline constexpr auto line_layout = gc::blades<4, "e41 e42 e43 e23 e31 e12">;
using Line = gc::mv<rga3, line_layout, double>;

Line axis{0.0, 0.0, 1.0, 0.0, 0.0, 0.0};  // the z axis through the origin
double vz = gc::get<"e43">(axis);         // 1.0
gc::get<"e23">(axis) = 2.0;               // write a coefficient by its name
Line none{};                              // every coefficient zero
bool moved = axis != Line{0.0, 0.0, 1.0, 0.0, 0.0, 0.0};  // exact comparison
```

Moving between blade sets:

```cpp
inline constexpr auto motor_layout =
    gc::blades<4, "1 e41 e42 e43 e23 e31 e12 e1234">;
using Motor = gc::mv<rga3, motor_layout, double>;

auto as_motor = static_cast<Motor>(axis);          // drops nothing: allowed
Line back = gc::truncate<line_layout>(as_motor);   // drops 1 and e1234
// Line lost = static_cast<Line>(as_motor);
//   error: conversion drops blades of its input: this is a lossy
//   conversion, request it with truncate<Blades>(x)
```

What misuse looks like:

```cpp
gc::get<"e14">(axis);  // error: get: the blade set holds this blade with the
                       // opposite orientation; use the name the set gives it
gc::get<"e1">(axis);   // error: get: no blade of this name is in the blade set
gc::get<"e5">(axis);   // error: get: the name has an index above n, the
                       // number of generators
gc::mv<rga3, line_layout, std::string> s;
                       // error: constraints not satisfied: std::string does
                       // not satisfy ring_scalar, because a - b is invalid
```

How each choice reads to a first-time user, judged by the hci-expert role file's criteria (the names a user types, the examples copied, the errors read):

| Element | What the user sees | Judgment, and what remains |
| --- | --- | --- |
| `mv<Alg, Blades, T>` | Three arguments, algebra first; no default scalar (question 4). Clang prints `mv<rga3, line_layout, double>` with the variable names as written; GCC prints the set's full value with libstdc++ internals (`std::__array_traits<...>::_Type`) | The order matches the design study and puts the algebra first in diagnostics. The GCC rendering of blade sets is WU4's (note 0010, open question 4) and is recorded as a risk, not fixed here |
| `blades<4, "...">` | n written twice, once in the tag and once here | A papercut, not a trap: a wrong n is caught with a message (question 11) |
| Construction | `Line{...}` in layout order; fewer values leave the rest zero; under `-Wconversion` a float type needs `0.1f` literals (planning probe) | Plain aggregate rules, as for `std::array`. The order is the layout's, so the layout should be declared next to the type, as in the snippet |
| `Line{}` and `Line l;` | Both zero (question 5) | No indeterminate values |
| `get<"e43">(x)` | A free function, so no `x.template get<...>()` in generic code; reads and writes; the name is the oriented name the layout uses | Four distinct messages for four mistakes (absent, opposite orientation, index above n, not a name). Names have one digit per index, so `get` serves n up to 9; above that, coefficients are reached by position |
| `==`, `!=` | Exact, same type only | A comparison across layouts fails to compile with the compiler's "no match for operator==", naming both types; documented, not given its own message |
| `static_cast<Motor>(x)` | Explicit; the brace form `Motor{x}` fails with a message about `double` (question 3) | The one confusing message of the surface; the documentation shows the cast, and a compile-fail test records what the brace form prints so that a change is noticed |
| `truncate<line_layout>(m)` | The name says that blades are dropped; checking mode verifies they were zero for an exact scalar (question 7) | `truncate<Line>(m)` would read better once models exist (question 10) |
| A wrong scalar | The compiler's constraint report, naming `ring_scalar` and the first failing requirement | The requirements are ordered so that the arithmetic ones, which read briefly, fail before constructibility, whose explanation lists every constructor of the type (174 lines on GCC, 99 on Clang for `std::string` with the opposite order, planning probe) |

## 4. Content

New header `include/galoot/core/mv.hpp`, in `galoot::core` with the kernel in `galoot::core::detail`; it includes `plan.hpp` and standard headers only, and none of `<complex>`, `<string>`, `<algorithm>`. It joins the header set in `CMakeLists.txt` and `include/galoot/core.hpp`. Names as in departure 20.

### WU8a

- `ring_scalar<T>`: `std::semiregular<T>`; `a + b`, `a - b`, `a * b` convertible to T; `-a` convertible to T; `T` constructible from `coefficient_type`; the arithmetic requirements first (section 3). Value-initialization `T{}` is the zero of T, a documented requirement, as is that `+` and `*` form a ring. Equality is not required, so that a scalar whose `==` returns a mask, such as a lane pack, is accepted (it then has no `==` on `mv`). Each requirement stated in the `///` comment (S7).
- `exact_scalar<T>`: `ring_scalar<T>` and `std::numeric_limits<T>::is_exact` (question 8).
- `mv<Alg, Blades, T>`, `template <metric_like Alg, blade_set Blades, ring_scalar T>`: one public member `std::array<T, Blades.size()> coefficients{}` (question 5); `algebra_type`, `scalar_type`, `layout` as present callers need them; a defaulted `operator==` (departure 13). In its body, `static_assert`s with literal messages, each reading a variable template computed once per (n, set): every mask names only e1 to en ("mv: the blade set names a generator beyond e_n"), every sign is +1 or −1 ("mv: the blade set holds a sign other than +1 or -1"), and no mask appears twice ("mv: the blade set holds one blade twice, perhaps in both orientations"). The value checks are `static_assert`s rather than constraints so that the messages are literal and in the algebra's terms (C9); the type-level requirements (`metric_like`, `ring_scalar`) are constraints, so that generic code can detect them.
- `multivector<X>`: X, after removing references and cv-qualifiers, is an `mv`.
- `has_blade<X, Name>`: X is a multivector, Name is one valid name in X's algebra, and X's layout holds the blade of that name with its orientation. It tests data only (decision 0033).
- `get<Name>(x)`: one function template on a forwarding reference, returning the coefficient with the value category of its argument, as `std::get` does; the position is a `constexpr` constant inside the function, so no search runs at run time at any optimization level (note 0012, finding 4). The name parameter reuses `blade_set.hpp`'s structural string `detail::name_list` and its `parse_name`, so a name means in `get` exactly what it means in `blades` (CONVENTIONS 1.6). Four catch-alls of form P (question 2), mutually exclusive by constraint, one per mistake, with the messages of section 3, plus one for a list of more than one name ("get: give one blade name") and one for an algebra of more than nine generators ("get: names have one digit per index, so they are read only for n up to 9").
- `test/support/rational.hpp` (question 15, decision 0011): `test_support::rational`, numerator and positive denominator in `std::int64_t`, always in lowest terms; `+`, `-`, unary `-`, `*`, `/`, `==`, construction from `std::int64_t`; every operation `constexpr`; every intermediate product and sum guarded against overflow, failing a constant evaluation by a call to a non-`constexpr` function and aborting at run time with a message (decision 0012's form, written in the test support, not with `GALOOT_ASSERT`, so that the core gains nothing); a specialization of `std::numeric_limits` with `is_specialized` and `is_exact` true. Its own tests are in `test/support/rational_test.cpp`, CTest `support.rational`.
- `test/core/bare_user.cpp` gains a Line declared, read by `get`, and compared (DoD-6).

### WU8b

- `detail::apply<Op>(x, y)` for a `BinaryOperation` and `detail::apply<Op>(x)` for a `UnaryOperation`: x and y of one algebra and one scalar type (deduced, so a mismatch fails deduction; WU9 adds the public message). The result is an `mv` on the plan's or the map's result set. The body expands an index pack over the plan's terms, one statement per term, with the coefficient, the positions, and whether the term is its output's first as constants (section 10, items 1 and 2); the first-term flags are computed in one pass with a "seen" array, O(terms + outputs), not by comparing terms pairwise, which exceeded both compilers' limits in planning. The direct form, a loop over the terms, is the reference the tests compare it with (C3); the expansion is chosen because the loop does not give straight-line code (P4, section 10).
- The coefficient check (question 6): a variable template, computed once per (plan, T), that every coefficient is exact in T when `std::numeric_limits<T>` is specialized; a `static_assert` in `apply` with a literal message in the user's terms (proposed: "the scalar type cannot hold one of this product's exact coefficients").
- `detail::apply` onto a given target set (departure 17): the result on Target, which must hold every blade of the plan's result set, each output position and sign mapped at compile time (an output on −e14 in Target receives the negated sum). Used by the conversion and by `truncate` as the identity map, and later by WU10.
- The conversion: `template <blade_set To> explicit operator mv<Alg, To, T>() const`, constrained to targets that hold every blade of the source; a catch-all of the plain form for the others (decision 0033: "conversion functions get the plain catch-all"), with the message of section 3. Both orientations of a blade count as the same blade; the coefficient takes the sign s_source · s_target.
- `truncate<To>(x)`: the result on To; each blade of To that x holds takes its coefficient with the sign s_source · s_target, each other blade of To is zero; for an `exact_scalar`, each dropped coefficient is checked by `GALOOT_ASSERT` ("truncate: a dropped coefficient is not zero"), in every constant evaluation and at run time when `GALOOT_CHECKING` is 1 (decision 0030). For a rounding scalar nothing is checked, and the `///` comment says why (question 7). To is checked by `mv`'s own set checks when the result type is formed. Catch-alls: a target set beyond the algebra's n reaches `mv`'s message, and no other is added unless a test shows a misuse that reads badly.
- Trace table: the 4.5 row ("truncate checks that the part it drops is zero") is set to `yes` in the commit that implements it, with a test whose marker states the fact.

### Both parts

- Every public declaration has a `///` comment stating what it requires, guarantees, and preserves, with the measured limits where they bind, and cites no record (S11).
- Decisions: 0150 (the type: parameters, layout, members, zero initialization, equality, names), 0151 (the scalar concepts, `numeric_limits` for exactness, the test rational's place), 0152 (the set checks in the type, their form and cost), 0153 (named access: `has_blade`, orientation, the messages, the catch-all boundary with WU15), 0154 (the kernel: expansion, first term, coefficient folding, evaluation order, the exactness check, the target-set form), 0155 (the conversion and `truncate`, and checking mode), 0156 (WU8a limits and costs), 0157 (WU8b limits, costs, and the assembly read), 0158 (the split and the boundaries with WU9, WU10, and WU15). The owner's answers to section 1 are recorded in these. 0159 stays with this plan.
- Mutation scripts and results under `docs/agent/records/mutations/0016-mv-a/` and `0016-mv-b/`, in the form of plan 0013's drivers, keeping lessons 0060 to 0063 (read the build's status first, assert anchors, before-and-after costs from clean revisions, no CTest after a failed build in a tree whose programs were deleted).

## 5. Definition of done

For each part: the header compiles alone and in the umbrella header; every test of section 7 for that part passes on all seven presets, each test case with its reference marker; the trace gate, markers, golden facts, citations, include scan, format, and lint pass; every rejection has a compile-fail test on both compilers that requires its literal message and forbids the messages of the other rejections; every test case and every `static_assert` the part adds has a recorded mutant that it kills, or is recorded as equivalent with the reason; the part's limits are measured and stated in the `///` comments; its decisions, the W10 before-and-after record, the per-use costs, the P2 record (WU8b), and the agent-cost row are written; and this plan records the part's outcome with its V10 line. The unit is done when both parts are merged into `develop` and this plan is closed.

## 6. Acceptance criteria

WU8a:

1. Type properties (STYLE S8, P5, DoD-4): for at least these instances, `std::is_aggregate_v`, `std::is_trivially_copyable_v`, `std::is_standard_layout_v`, and, for K ≥ 1, `sizeof == K × sizeof(T)`, each a `static_assert`: rigid(3) point, line, plane, and motor layouts with each of the five scalars; a conformal(3) vector; the sparse set {1, e1, e16, e1 ∧ e16} by mask in R(15,0,1); an empty set (no coefficients).
2. `mv{}` and a default-initialized `mv` hold T{} in every place, for the five scalars, at compile time and at run time.
3. Equality is true exactly when every coefficient is equal: for each position of a line and a motor, two values that differ there alone compare unequal; for `double`, −0.0 equals +0.0 and a NaN coefficient makes `==` false.
4. `ring_scalar` accepts `std::int64_t`, `int`, `short`, `float`, `double`, `long double`, `std::complex<double>`, and the test rational; it rejects a pointer, `std::string`, a type with `+` and `-` but no `*` (Spike C's case 5), a type with no conversion from `coefficient_type`, a type without unary minus, and a type that is not copyable; expected answers from the stated requirements (S7). `exact_scalar` is true for `std::int64_t`, `int`, and the rational, and false for `float`, `double`, and `std::complex<double>`. `multivector` is true for an `mv` with any cv-qualification and reference, false for `std::array<double, 4>` and for a blade set.
5. The set checks: each fires alone on a hand-written set, with its message, as a compile-fail test on both compilers (section 7, E8), including a repeat in both orientations, a repeat not adjacent, a repeat after an insertion point, a stray blade last in the set, and e32 in R(31,0,0); every set made by `full`, `of_grade`, `merge`, and `blades`, for n from 1 to 8, is accepted.
6. Named access by definition: for the rigid(3) point, line, plane, and motor layouts and a conformal(3) layout with e45, `get` with each name as the layout lists it reads and writes exactly that position, through an lvalue, a const lvalue, and an rvalue (whose result is an rvalue); the name's mask and sign, computed in the test by its own inversion count, equal the layout's entry. `has_blade` is true for each listed name and false for its opposite orientation and for an absent blade.
7. Named access rejections: each of the six mistakes of section 4 is a compile-fail test on both compilers requiring its message; each produces one error, with the message on the first error line after the inclusion context (as note 0010 measured for form P).
8. The test rational: the field identities on seeded random values (one seed source, printed on failure, V7): associativity and commutativity of + and *, distributivity, additive and multiplicative inverses, normalization (lowest terms, positive denominator); hand values; equality agrees with cross-multiplication on every pair of a seeded sample; an overflow aborts at run time with its message (a test with `WILL_FAIL`) and fails a constant evaluation (a compile-fail test); `std::numeric_limits<rational>::is_exact` is true.
9. Limits and costs, measured on both compilers (track-only, C14): the largest n for which `mv<Alg, full<n>, double>` builds at the default constant-evaluation limits with the direct repeated-mask check, stated in the `///` comment; if it is below the largest result set a plan can reach (`full<5>` and `full<6>` products, 32 and 64 blades), the drop is reported and departure 16's faster form is decided by measurement; W10 before and after; the per-use cost of a type and of a `get` (note 0012's method: added uses from 64 to 256 repetitions, instructions and peak memory).
10. The seven presets from fresh trees at the final revision; `all_verify_interface_header_sets` on gcc-debug and clang-debug; `core.bare_compile` with the extended `bare_user.cpp`.

WU8b:

11. Kernel equals plan, by definition (exhaustive over the stated range): for the forms rigid(3), R(4,0,0), R(1,3,0), conformal(2) with `full<4>`, and the degenerate form and seeded random form 1 with `full<3>`, for the four binary operations and the five unary maps, `apply` on every pair of basis inputs (one coefficient 1, the rest 0) and on seeded random integer inputs equals the test's loop over the same plan's terms (the direct form). The range is reduced if the unit's compile time slows the edit cycle, by a recorded choice, never below rigid(3) and conformal(2) for all nine operations.
12. Products against independent values: the hand values of section 7 (F3 to F5) hold in `static_assert`s and at run time; for each of the four golden forms, `apply<gp>` of two seeded random multivectors on `full<n>` equals their product composed in the test from the golden pair tables (`gp_tables`, LDLᵀ route, V1 kind 3), over the test rational with non-integer inputs.
13. DoD-7: the same rigid(3) and conformal(3) plans (point ⟑ point, line ⟑ plane, motor ⟑ point, vector ⟑ vector, and one antiproduct) over `std::int64_t`, the test rational, `float`, `double`, and `std::complex<double>` (integer real and imaginary parts), with inputs bounded as departure 14 states, equal the exact reference (the golden composition in `std::int64_t`) converted to each scalar, by `==`; the bound is derived in the test's comment from the input range and the plan's term counts before the values are seen.
14. The coefficient check (question 6): applying the e12 ⟑ e12 plan of diag(4096, 4096) (coefficient −2^24) compiles with `float`; of diag(24929, 673) (−(2^24 + 1)) is rejected with `float` and compiles with `double`, `int`, and `std::int64_t`; e1234 ⟑ e1234 of diag(8192, 8192, 8192, 16384) (2^53) compiles with `double`, and of diag(32767, 32767, 32767, 32767) (32767^4, odd and above 2^53) is rejected with `double` and compiles with `std::int64_t` and the rational; the diag(4096, 4096) plan is rejected with `short`. Each value is computed in the test from the form's entries by hand (planning values, section 7, F8), and each rejection is a compile-fail test on both compilers.
15. The conversion: for every pair among the rigid(3) layouts where one holds the other's blades (line into motor, point into a set of all vectors in another order, a canonical result set into a named layout of the same blades), every coefficient lands on its blade with the sign s_source · s_target, over all five scalars; the canonical result of point ∧ point converted to a Line reads (3, 3, 3, −3, 6, −3) for section 2's p and q; implicit conversion and every lossy target are rejected with the message (compile-fail, both compilers); the brace form's output is recorded by a compile-fail test that requires its present first error line, labeled as a record of compiler behavior, not as a Galoot message.
16. `truncate`: kept blades as for the conversion; blades of the target that the input lacks are zero; for `std::int64_t` and the rational, a nonzero dropped coefficient fails a constant evaluation (compile-fail) and aborts at run time in a checking build (`WILL_FAIL`, registered in the presets with `GALOOT_CHECKING` ON), while a build with checking off returns the kept blades and does not evaluate the check (a program in the form of `core_assert_mode_0`); for `double`, a nonzero dropped coefficient does not abort (contract of question 7, labeled). The 4.5 trace row is `yes` with its fact in a marker.
17. Compile time equals run time: `apply`, the conversion, and `truncate` give the same values in a `static_assert` and at run time with inputs read through `volatile`, for rigid(3) and conformal(3).
18. Every generation-time or compile-time check WU8b adds fires on a mutant, and each bound of the coefficient check is tested on both sides (criterion 14).
19. Limits, both compilers, three configurations (decision 0103: Galoot's flags at -O0 and -O2, bare `-std=c++23 -pedantic-errors -O2`): `apply` builds for every plan that builds (the typed rigid(3) pairs, conformal(3) vectors, sparse pairs at n = 8, 12, 16, and the largest `full<n>` × `full<n>` of each operation); any plan above 2048 terms that builds on Clang is checked against Clang's fold-nesting limit of 2048 (knowledge-base note on constant-evaluation limits), and if one exists the limit is stated in `apply`'s comment, or the expansion is split into chunks by a decision with the measurement.
20. P2: decision 0103's fixed use (four typed rigid(3) pairs in R(n−1,0,1)) through `apply`, at n = 4, 8, 12, 16, pinned, three repeats, both compilers, without the concept `static_assert` (decision 0107); before (the plans alone) and after (applied to `mv` values), recorded under `docs/agent/records/performance/mv/`. Growth is reported with its cause and blocks nothing (decision 0005).
21. W10 before and after from clean revisions, and per-use costs of `apply` (note 0012's method, for the four rigid(3) pairs).
22. The seven presets from fresh trees at the final revision; header sets on gcc-debug and clang-debug.
23. DoD-6: `bare_user.cpp` applies nothing (the kernel is `detail`) but converts and truncates a Line and a Motor.
24. The assembly read (departure 19), recorded under `docs/agent/records/performance/mv/assembly/`: for rigid(3) point ⟑ point, point ∧ point, and motor ⟑ point over `double` at -O2 on both compilers, the kernel's body has one multiplication per term, no multiplication by a coefficient of ±1, no integer-to-floating conversion, no load of plan data, no branch on blades, and terms − outputs additions and subtractions; whether the kernel is inlined into its caller is recorded and compared with a hand-written straight-line function of the same terms, written from the mathematics in the record and compiled the same way; a default-initialized `mv` that is then fully written shows no separate zero fill.

## 7. Test list, with expected values and reference sources

Markers as in use: `proof` (exhaustive over a finite range, V1 kind 1), `identity` (kind 2), `independent value` (kind 3), `contract` (a property the header promises). Every exact comparison is `==` (V11). File: `test/core/mv_test.cpp` (CTest `core.mv`) for both parts, one file per component (S14); `test/support/rational_test.cpp` (CTest `support.rational`). Rigid(3) layouts: P = `blades<4, "e1 e2 e3 e4">`, L = `"e41 e42 e43 e23 e31 e12"`, G = `"e423 e431 e412 e321"`, M = `"1 e41 e42 e43 e23 e31 e12 e1234"`.

### WU8a

| # | Test case | Reference (kind) | Expected |
| --- | --- | --- | --- |
| E1 | Type properties | contract (S8, P5) | Criterion 1. For L over `double`, `sizeof` is 48; over `float`, 24; M over `std::int64_t`, 64; the empty set holds `coefficients.size() == 0` |
| E2 | Zero initialization | contract | Criterion 2 |
| E3 | Construction order | contract | `Line{1, 2, 3, 4, 5, 6}` holds 1 to 6 at positions 0 to 5; `Line{{1, 2, 3, 4, 5, 6}}` is equal to it; `Line{1, 2}` holds 0 at positions 2 to 5 |
| E4 | Equality | contract | Criterion 3, with one mutant per position of the comparison killed |
| E5 | Scalar concepts | contract, from the stated requirements | Criterion 4 |
| E6 | Named access by definition | independent value: the test's inversion count on the name; CONVENTIONS 1.6 table | Criterion 6. On L: e41 at 0 with (mask 0b1001, sign −1), e42 at 1 (0b1010, −1), e43 at 2 (0b1100, −1), e23 at 3 (0b0110, +1), e31 at 4 (0b0101, −1), e12 at 5 (0b0011, +1). On G: e423 (0b1110, +1), e431 (0b1101, −1), e412 (0b1011, +1), e321 (0b0111, −1). On M: "1" at 0 (mask 0, +1) and e1234 at 7 (0b1111, +1). The marker states "an oriented name lists generators in the order of their product", the 1.6 fact |
| E7 | `has_blade` | contract | true for every listed name of E6; false for e14, e24, e34, e13 on L (opposite orientation), for e1 on L (absent), for "e5" in rigid(3), and for "e1 e2" (two names) |
| E8 | Set checks | contract (V4) | Compile-fail `core.mv_beyond_n` (e5 last in a set of rigid(3)), `core.mv_beyond_n_31` (e32 in R(31,0,0)), `core.mv_bad_sign` (sign 0), `core.mv_bad_sign_second` (sign 2, middle), `core.mv_repeated_mask` (e1 twice), `core.mv_repeated_orientations` (e14 and e41), `core.mv_repeated_apart` (e2 e1 e2), `core.mv_repeated_after` (e1 e2 e3 e2), `core.mv_not_metric_like`, `core.mv_not_ring_scalar` (requires "ring_scalar" and, on each compiler, the text naming the failing requirement); accepted: the sets of criterion 5 |
| E9 | Named access rejections | contract (C9) | Compile-fail `core.mv_get_absent` (e1 on L), `core.mv_get_opposite` (e14 on L), `core.mv_get_above_n` (e5), `core.mv_get_not_a_name` ("x1", and "e0" as a second case), `core.mv_get_two_names` ("e1 e2"), `core.mv_get_wide_algebra` (a set in R(10,0,0)); each requires its message and forbids the others |
| E10 | Detection | contract (decision 0033) | `requires { get<"e1">(line); }` is true (the catch-all's documented consequence); `has_blade<Line, "e1">` is false |
| E11 | Rational, hand values | independent value, by hand | 1/3 + 1/6 = 1/2; (−2)/4 = 1/(−2) = −1/2 with denominator 2; (2/3)(9/4) = 3/2; 1/2 − 1/2 = 0 with denominator 1; (3/7) / (6/7) = 1/2 |
| E12 | Rational, identities | identity, seeded (V7) | Criterion 8, numerators and denominators drawn below 2^15 so no product overflows |
| E13 | Rational, overflow | contract | `support.rational_overflow` (`WILL_FAIL`, requires the message) for (2^62)/1 + (2^62)/1; `support.rational_consteval_overflow` (compile-fail) for the same in a `static_assert` |
| E14 | Compile time equals run time | contract | The values of E3, E6, and E11 also read through `volatile` at run time |

### WU8b

| # | Test case | Reference (kind) | Expected |
| --- | --- | --- | --- |
| F1 | Kernel equals plan | proof over the stated range | Criterion 11: the test's loop over `plan.terms` (out += c · x_a · y_b, from zero) is the reference |
| F2 | First term and folding | contract | For P ⟑ P, each output's first term in plan order is the one with the lowest a, then b; an input of −0.0 in one place gives the same values as +0.0 except possibly the sign of a zero output; with every coefficient ±1 the kernel performs no conversion (checked in the assembly read, criterion 24) |
| F3 | Rigid(3) hand values | independent value, by hand from e1² = e2² = e3² = 1, e4² = 0 (planning values) | p = (1, 2, 3, 1), q = (4, 5, 6, 1): p ⟑ q = (32, −3, −6, −3, −3, −3, −3) on (1, e12, e13, e14, e23, e24, e34); p ∧ q = (−3, −6, −3, −3, −3, −3) on the six bivectors; as a Line, (3, 3, 3, −3, 6, −3), the moment equal to the cross product (−3, 6, −3) |
| F4 | Conformal(3) hand values | independent value, by hand from e4 · e5 = −1 (planning values) | v = (1, 2, 3, 4, 5), w = (1, 0, 0, 1, 1) on (e1, ..., e5): v ⟑ w = (−8, −2, −3, −3, −4, 0, 2, 2, 3, 3, −1) on (1, e12, e13, e14, e15, e23, e24, e25, e34, e35, e45): the scalar is 1 + (−1)(4 + 5), and e45 gets v4 w5 − v5 w4 = −1; e23 is in the result set and zero for these inputs |
| F5 | Unary maps on a Line | independent value, from plan 0013's D2 terms and the 2.1 sign rule | l = (1, 2, 3, 4, 5, 6) on L: the reverse on the canonical set (e12, e13, e14, e23, e24, e34) is (−6, 5, 1, −4, 2, 3); onto L itself it is −l |
| F6 | Golden composition | independent value (golden `gp_tables`, LDLᵀ route) | Criterion 12, inputs over the test rational with denominators 2, 3, and 5 |
| F7 | Five scalars (DoD-7) | independent value (the golden composition in `std::int64_t`) | Criterion 13. Bound stated in the test: inputs are integers of magnitude at most 2^4 (for complex, both parts), each coefficient of these plans has magnitude at most 2 (asserted in the test), each output has at most 48 terms, so every partial sum is an integer of magnitude below 48 · 2 · 2^8 · 2 < 2^16 < 2^24, and every scalar computes it exactly |
| F8 | Coefficient exactness | independent value, by hand (planning values) | Criterion 14. e12 e12 = −g11 g22 in a diagonal form; e1234 e1234 = +g11 g22 g33 g44 (the reverse of a grade-4 blade has sign +1). 24929 · 673 = 16,777,217 = 2^24 + 1 (24929 = 97 · 257); 4096² = 2^24; 8192³ · 16384 = 2^53; 32767^4 = 1,152,780,773,560,811,521, odd and between 2^53 and 2^63 |
| F9 | Conversion | contract and independent value | Criterion 15; compile-fail `core.mv_conversion_lossy` (explicit), `core.mv_conversion_implicit`, `core.mv_conversion_brace` (records the compilers' text) |
| F10 | `truncate` | contract (CONVENTIONS 4.5) | Criterion 16: the motor (0, 1, 2, 3, 4, 5, 6, 0) truncated to L is (1, ..., 6); (7, 1, ..., 6, 0) aborts in checking mode for `std::int64_t` (`core.mv_truncate_nonzero`, `WILL_FAIL`), fails a constant evaluation (`core.mv_truncate_consteval_nonzero`), and returns (1, ..., 6) with checking off; over `double` it returns (1, ..., 6) and does not abort. The marker states "truncate checks that the part it drops is zero", the 4.5 trace fact |
| F11 | Target-set form | proof over the rigid(3) layouts | The canonical result of every rigid(3) pair of WU7a's C2 written onto a named layout that holds it equals the canonical result converted, coefficient by coefficient |
| F12 | Compile time equals run time | contract | Criterion 17 |
| F13 | Empty results | contract | L ∧ G gives an `mv` with no coefficients, and two of them compare equal |

Mutants to be killed (each recorded with the test that kills it, both compilers, both modes of the mutation driver): WU8a: each set check removed; the repeat scan starting two places on; the repeat test comparing (mask, sign) instead of mask; a sign of 0 accepted; `get` ignoring the sign; `get` one position off; `get` returning the first blade with the mask regardless of orientation; equality skipping the last coefficient; the rational without normalization, with a negative denominator kept, or with the overflow guard removed; `ring_scalar` without its unary-minus requirement. WU8b: the first-term flag inverted; the first term added to zero (an equivalent mutant in value, killed by the assembly criterion, recorded as a cost mutant); the ±1 branches swapped; the coefficient applied as (c · x) · y (equivalent on every test input, because the inputs are exact by construction; recorded as equivalent, the order being a declared property under V14 rather than a tested value); the exactness bound off by one on each side; `truncate`'s sign s_source · s_target replaced by s_target; the dropped-coefficient check run for a rounding scalar; the check skipped for the rational; the conversion's covering test reversed.

## 8. Approach

Direct forms first (C3, W4), in the style of `plan.hpp` and `blade_set.hpp`: `constexpr` functions over values, variable templates computed once per argument list (S7, P3), `static_assert`s with literal messages, catch-alls of decision 0033's forms, `GALOOT_ASSERT` for run-time preconditions. Every standard header `mv.hpp` includes is one the core already includes, unless the include-cost record (criterion 9) shows one costs nothing measurable.

Order of commits, each building and passing on what it was checked on (W2, W9 as amended by decision 0094; test scope as the handoff states: the affected tests and the cheap gates per commit, checked at the commit on a clean tree; the full gcc-debug suite at the end of each test-and-code pair; all seven presets once at the final revision):

WU8a, on `feature/core-mv-a`:

1. The W10 before rows from fresh gcc-debug and clang-debug trees at the claim revision.
2. `test/support/rational.hpp` and its tests (E11 to E13), with the CMake registration of `support.rational`.
3. `mv.hpp`: the concepts, the type, the set checks; `core.hpp` and the header set. Tests E1 to E5, E8, E14.
4. Named access and its catch-alls; tests E6, E7, E9, E10; `bare_user.cpp`.
5. Mutation records; limits and per-use costs; W10 after; decisions 0150 to 0153, 0156, 0158.
6. Outcome with the V10 line; W5 review; fixes in their own commits; a delta review of any core fix (decision 0094 item 3); merge after `develop` is merged in and P9 passes.

WU8b, on `feature/core-mv-b`, after WU8a merges: the claim line here; W10 before; the kernel and the coefficient check (F1 to F8, F12, F13); the target-set form, the conversion, and `truncate` (F9 to F11), with the 4.5 trace row; the assembly read; limits, P2, per-use costs, W10 after; mutation records; decisions 0154, 0155, 0157; outcome, review, merge.

The hci-expert reads the interface twice (section 11): the WU8a header and its rejection outputs before the W5 review, and the conversion messages of WU8b. The reading is advice to the implementer, not the W5 review.

Compile-time cost to every includer (decision 0113: `core.hpp` alone is about 0.91 G instructions on GCC and 0.92 G on Clang, after `plan.hpp`'s span instantiations). Budget approach, with no bound (C14): measure `core.hpp` alone and `mv.hpp` alone before and after each part on both compilers; if the rise is beyond the spread of three repeats, attribute it with a Clang `-ftime-trace` as decision 0113 did and report it with its cause; keep `mv.hpp` from instantiating anything at namespace scope and from including headers the core does not already include; measure per-use cost separately (criteria 9 and 21), because the cost of a type, a `get`, and an `apply` falls on the programs that use them, not on every includer.

## 9. Evidence

- Test output per preset for `core.mv` and `support.rational`, with the exhaustive ranges stated (F1, F11).
- Compile-fail output on both compilers for every rejection, with the line on which each message appears.
- The trace gate output in each merge message, with the 4.5 row pinned (WU8b).
- Mutation records for both parts.
- Limits, per-use costs, P2, and W10, each as `tools/timed` runs in `runs.tsv` with their commands, and summaries under `docs/agent/records/performance/mv/`.
- The assembly read (criterion 24), with the commands and the hand-written comparison.
- V10 line, to fill in the outcome. Verification: E1 to E14 and F1 to F13 (kinds 1 to 3 over the stated ranges; the golden tables by the LDLᵀ route). Validation: for rigid(3), the line through two points with direction q − p and moment p × q (F3), a known property of Plücker coordinates checked by hand, and the oriented names against Lengyel's table as CONVENTIONS 1.6 records it (Checked, Confirmed); for conformal(3), the hand value F4 from the null basis; for scalars, the exactness of each scalar's conversions (F7, F8) rests on IEEE 754's binary formats (background, not read in this unit). The non-diagonal antiproducts remain unvalidated (plan 0013, departure 8).

## 10. Evidence gathered while planning

Planning values, not reproducible as committed (departure 22). Probe sources were throwaway files: a minimal `mv` over Galoot's `product_plan`, compiled with `-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -I include` against `develop` at `7da09f1`, GCC 16.2.1 and Clang 23.1.4, each compile through `tools/timed` (selections `wu8-plan-probe:*` in the main checkout's `runs.tsv`, not committed), shared mode, one run each.

1. Kernel form, rigid(3) motor ⟑ point (28 terms on 8 outputs, coefficients ±1), `double`, -O2, counted in the function's assembly. A loop over the terms: GCC 3 multiplications, 1 integer-to-double conversion, 1 branch (a loop); Clang 4, 2, and 2. An index-pack expansion with ±1 folded: 28 multiplications and no conversion on both; GCC no call and no branch; Clang called the expanded lambda out of line (one call from the caller to a straight-line body) and added the distribution's stack-protector check. At -O0 the expansion made about 200 calls (per-term helper and subscripts) on both. Compile cost of the probe unit at -O0, in retired instructions as `tools/measure` records them on performance cores: GCC 3.82 G (expansion) and 3.93 G (loop); Clang 4.87 G and 4.86 G.
2. First term. Accumulating from a zero-initialized result gave 28 additions and subtractions on both compilers (one per term, the 8 additions of zero kept, as IEEE semantics require); assigning each output's first term gave 20, on both. A first-term pass comparing every term with every earlier one failed Clang's default step limit at 1,024 terms and GCC's operation limit at 4,096; a linear pass with a "seen" array did not. With the linear pass, the expansion of `full<n>` × `full<n>` in R(n,0,0) compiled at -O0 in 0.54 s (n = 4, 256 terms), 0.94 s (n = 5, 1,024 terms), and 3.37 s (n = 6, 4,096 terms, 0.76 GiB peak) on GCC, and 0.60 s and 0.95 s for n = 4 and 5 on Clang, where n = 6 stops in the plan itself (decision 0113's ceiling); plan generation included.
3. Aggregate behavior. `std::is_aggregate_v`, trivial copyability, and `sizeof(Line) == 6 · sizeof(double)` hold with a constrained `template <metric_like Alg, blade_set Blades, ring_scalar T>` and an `explicit` conversion function template. Brace elision (`Line{0, 0, 1, 0, 0, 0}`) and the full form agree, with no warning. `{0.1, 0.2, 0.3, 1.0}` into a `float` layout is rejected under `-Wconversion -Werror` on both compilers (`-Wfloat-conversion`, `-Wimplicit-float-conversion`); `0.1f` passes; a `double` variable is a narrowing error. `static_cast<Line>(motor)` and `Line l(motor)` compile; `Line{motor}` fails with "cannot convert 'const Motor' ... to 'double' in initialization" (GCC) and "no viable conversion from 'const Motor' (aka 'const mv<rga3, M, double>') to 'double'" (Clang).
4. Diagnostics of a bad scalar. `mv<rga3, P, std::string>` with a concept whose first conjunct to fail is `std::constructible_from<T, coefficient_type>`: GCC 174 lines, Clang 99, both naming `ring_scalar` and the failing conjunct first, then listing `std::string`'s constructors. Clang named the blade set by its variable (`mv<rga3, M, double>`); GCC printed its value with `std::__array_traits<...>::_Type`. Hence section 4's order of requirements.

## 11. Review

W5 blind review of each part before its merge, by a reviewer independent of the author, given this plan's criteria, section 7, and the author's departures, not the author's view that the change is correct. The WU8a reviewer also reads every rejection output on both compilers as a first-time user would (C9) and checks E6's expected masks and signs against CONVENTIONS 1.6 by hand; reruns the repeat-scan and `get`-orientation mutants; and checks the rational's overflow guard at its bounds. The WU8b reviewer checks the bound argument of F7, the hand values of F3, F4, and F8 (each derivation is short and stated in the test), and the assembly read against the hand-written form; reruns the first-term and exactness-bound mutants; and runs `truncate`'s checking-mode tests in a checking and a non-checking preset. A fix to core code after the review gets a delta review (decision 0094 item 3).

An hci-expert reading (section 8) precedes each review and is recorded under `docs/agent/records/reviews/` as advice, with what the implementer changed in response.

## 12. Agent cost estimate (for information only, decision 0019)

From `records/agent-cost.tsv`: the WU7a implementer with review fixes, 631k tokens and 194 tool calls; WU7b's, 674k and 286; their reviewers, 300k and 79, and 358k and 112; the WU7 planner, 322k and 61.

| Role | Tokens | Tool calls |
| --- | --- | --- |
| WU8a implementer, with review fixes | 450k to 600k | 170 to 230 |
| hci-expert reading of WU8a | 120k to 180k | 40 to 60 |
| WU8a blind reviewer, plus a delta review | 300k + 150k | 90 + 50 |
| WU8b implementer, with review fixes (assembly read and P2 included) | 500k to 650k | 190 to 250 |
| hci-expert reading of WU8b | 80k to 120k | 30 to 40 |
| WU8b blind reviewer, plus a delta review | 330k + 150k | 110 + 50 |
| Total | about 2.1M to 2.6M | about 730 to 930 |

No bound applies. The outcome of each part compares these with the actuals.

## 13. Risks

1. Common-mode error between the tests and the header, both written from CONVENTIONS by agents. Mitigation: each family has a route that shares no code: golden tables (LDLᵀ over fractions), hand values with their derivations, the test's own inversion count for names, the loop form of the kernel, and cross-multiplication for the rational.
2. The test rational is a test oracle; a defect in it can mask a defect in the core. It has its own identities, hand values, and mutants (E11 to E13), and DoD-7 also compares it with `std::int64_t` on integer inputs.
3. Diagnostics are compiler text, and a compiler update can change their line counts or wording. The compile-fail tests require Galoot's literal messages, not the surrounding text; the one test that records compiler wording (the brace conversion) is labeled as such and is expected to need updating with a compiler.
4. GCC prints blade sets through libstdc++ internals in every diagnostic that names an `mv` (section 10, item 4). That is `blade_set`'s representation (WU4), and a fix (a C array member) would change a merged interface; noted for the owner, not done here.
5. The repeated-mask check is O(K²) in its direct form and may lower the size of `full<n>` an `mv` accepts on Clang (criterion 9). The generated result sets reach 64 blades at most at the present plan ceilings, far below where the check costs much; large full layouts are not a present use.
6. Clang leaves the expanded kernel out of line at -O2 (section 10, item 1). If a hand-written function of the same terms is inlined where the kernel is not, the difference is reported (P4) and a form that inlines is sought; a vendor attribute that forces inlining is an extension (S2) and is not used without a decision.
7. Clang's fold-nesting limit of 2048 caps a single expansion; no plan that builds on Clang at its default limits is known to exceed it (criterion 19 checks).
8. The zero default (question 5) adds a fill where an object is default-initialized and not written; the assembly read checks the common case, and the owner may prefer indeterminate values for arrays of multivectors.
9. Unchecked exactness for scalars without a `numeric_limits` specialization (question 6): a `std::complex<float>` with a coefficient above 2^24 rounds without a diagnostic. The comment and decision 0151 say so; the cases need Gram entries in the thousands.
10. Name clashes: `get` with `std::get`, `apply` with `std::apply`, and `truncate` with POSIX `truncate`. A test calls `get` and `truncate` unqualified in a unit that includes `<tuple>` and `<unistd.h>` and has `std::complex` scalars (whose namespace ADL also searches).
11. Lessons 0060 to 0063 apply, as in plan 0013 (risk 9 there).

## 14. Deferred

- To WU9 and WU10: the public `gp`, `wedge`, `antigp`, `antiwedge`, `reverse`, `right_complement`, and the rest as one-line callers of `apply`, their operators (CONVENTIONS 7.1), the `same_algebra` concept and its message for operands of two algebras or two scalars, and what blade set the unary operations return (departure 17); scaling by a scalar, sums, and differences.
- To WU11: bulk, weight, duals, dot, antidot.
- To WU13: `transform` and `compose` with their declared result sets, and the checking of what they discard against their plan's forward error bound on rounding scalars (question 7).
- To WU15: `format.hpp` and the DoD-19 catalogue review (question 2).
- To WU16: `truncate<Type>` (question 10), `blades<Alg, ...>` (question 11), model aliases with or without a default scalar (question 4), and the documentation's examples, compiled as tests (W12).
- To M3: rounding-scalar tests with tolerances from forward error bounds (departure 14).
- To M5: WU5's core rational for the change of basis (departure 15).
- Not built, noted only (C12): conversion between scalar types (a change of representation, C8, with no present user); structured bindings for `mv` (a tuple protocol); names for n ≥ 10 (a CONVENTIONS 1.6 question, note 0012 open question 2); a Galoot exactness trait for scalars without `numeric_limits` (question 6); chunked expansion for plans above 2048 terms (risk 7); the canonical-order fast path of the repeated-mask check (departure 16).

## 15. Owner questions, collected

1. The split, with named access in WU8a (departure 1).
2. Catch-alls of WU8's public functions written in WU8 (departure 2).
3. An explicit conversion between blade sets that drops nothing (departure 3).
4. No default scalar (departure 4).
5. A default-initialized `mv` is zero (departure 5).
6. Exactness of coefficients checked through `std::numeric_limits` (departure 6).
7. `truncate` checks exact scalars only, by ruling on how C8 applies without a plan (departure 7).
8. Exactness decided by `std::numeric_limits<T>::is_exact` (departure 8).
9. `get` rejects the opposite orientation (departure 9).
10. `truncate<Blades>` as CONVENTIONS 4.5 has it (departure 10).
11. `blades<N, ...>` kept (departure 11).
