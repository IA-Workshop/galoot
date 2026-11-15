# 0015 A cheaper metric_like that checks the same things

Date: 2026-11-01. Written by: Claude (agent, Opus 5.5), as cpp-expert in the researcher role, on branch `research/metric-like-proof` at `e1af2ac`. Not yet reviewed by anyone else. The proposal that follows from it is decision 0121 (status proposed).

How each claim was checked, by tag used below:

- **Derivation**: an argument from the definitions, written out step by step here. No one else has checked it.
- **Standard**: a clause of the C++ working draft read at eel.is/c++draft on 2026-11-01 by this author (a substitute for ISO C++23, N4950; the clauses quoted have the same content in C++23 as far as this author recalls, which is not checked).
- **Solver**: z3 5.3.3 (math-tools `py/` project), `equivalence_z3.py`.
- **Exhaustive**: a C++ program or a set of `static_assert`s that tries every case of a finite set that is shown to cover all cases.
- **Compiled**: GCC 16.2.1 and Clang 23.1.4 with libstdc++ 16, Galoot's test flags, both compilers; outputs under `docs/agent/records/performance/metric-like-check/out/`.
- **Measured**: pinned retired-instruction counts (below).
- **Inferred**: a reading of measurements or of compiler behavior that was not confirmed in the compiler's source.

## Question

`metric_like<Alg>` (`include/galoot/core/metric.hpp`, the concept at line 274) reads all n² entries of the tag's Gram matrix three times in one evaluation: `entries_in_range()` inside a `std::bool_constant` guard, `is_symmetric()`, and `entries_in_range()` again. Decision 0107 measured the cost of that: in a unit that uses an algebra, about 18 M instructions on GCC and 20 to 24 M on Clang more at n = 16 than at n = 4, and Clang pays it twice when the unit also writes `static_assert(metric_like<alg>)`. The owner asked whether something can be proved that lets Galoot rely on a cheaper mechanism.

## Answer in short

1. The three conditions can be replaced by one pass that reads each entry once (for each pair i ≥ j it reads g_ij and g_ji), and the replacement accepts exactly the same matrices. Proved for every n (Theorem 1), checked by the solver for every n from 1 to 32, and exhaustively on the compiled code for n ≤ 3.
2. The verdict can be held in a variable template, one per tag, behind a guard that reads a single entry. A single entry decides whether the value can be read at compile time exactly as reading every entry does (Theorem 2), so the concept stays false, never an error, for every tag the library's tests reject. Each algebra is then checked once per unit on both compilers (measured).
3. Reading the matrix through a plain copy made by `std::bit_cast`, so that each read is a built-in subscript rather than a call of `std::array::operator[]`, cuts the remaining cost by about two thirds. The copy holds the same values at the same indices (Theorem 3).
4. Together (variant `cached-flat`) these keep every guarantee of today's concept and bring the decision-0107 growth from n = 4 to 16 down from +17.7 M to +2.5 M on GCC and from +20.8 M to +5.2 M on Clang, and with the `static_assert` in the unit from +42.8 M to +2.1 M on Clang (Measured, table below).
5. Removing the n² reads entirely (trusting the factories, or a branded type) cannot be done soundly with `metric<N>` as it is: a hand-written aggregate, a changed copy, and a `std::bit_cast` each produce an invalid metric that a type-only test accepts (Compiled). Closing those three needs a non-aggregate `metric` with a const matrix and a pointer member; that breaks STYLE S8 ("descriptors are aggregates"), the tested "aggregate" property, and on Clang the "trivially copyable" one, and it saves at most the last 2 to 5 M, and nothing for algebras built by `from_gram` (Derivation, Compiled). Not recommended.

## What the concept guarantees today

Read from the header and its tests (`test/core/metric_test.cpp:181` to `:202`):

- G1. `Alg::value` has type `metric<N>` (after removing cv and reference), so a look-alike type is rejected.
- G2. The concept is false, not an error, when `Alg` has no `value`, when `value` is not a metric, and when `value` cannot be read at compile time: a non-constexpr static, a declared but undefined static, a non-static member. This is what "the guard reads every entry, so that it fails for a value that is not usable at compile time even when n = 1" in the header's comment secures: at n = 1, `is_symmetric()` reads nothing, and in C++23 a call that reads nothing can be a constant expression even on an object that is not constexpr, so some read is needed.
- G3. n ≥ 1.
- G4. The Gram matrix is symmetric.
- G5. Every entry lies within ±`max_form_entry` (32767).

Not guaranteed today either: anything about a program that specializes `galoot::core::metric` itself. `holes/specialized.cpp` specializes `metric<2>` with member functions that report a valid form over an asymmetric matrix, and today's concept accepts it on both compilers, because it calls those member functions (Compiled). So the guarantees hold for programs that use Galoot's templates and do not specialize them. That is the threat model of this note:

- A careless user writes a metric by hand, changes a copy of one, or makes one from raw integers, and expects a mistake to be caught. Every check must catch these.
- A hostile user specializes Galoot's templates or declares names in its namespaces. No check written in the library can stop that, because the specialization replaces the very definitions the check reads; such a program is outside every guarantee, today and under every option here.

## Options weighed

| Option | Invariant relied on | G1 to G5 | Cost at n = 16 (one check, GCC / Clang) | Interface change |
| --- | --- | --- | --- | --- |
| Today | none | all | 21.4 / 25.6 M, twice on Clang with a `static_assert` | none |
| (a) one pass | Theorem 1 | all | 14.4 / 16.1 M, twice on Clang with a `static_assert` | none |
| (d) cache of today's three conditions | Theorems 2, 4 | all | 21.5 / 30.1 M, once | none |
| (a) + (d) | Theorems 1, 2, 4 | all | 14.5 / 16.4 M, once | none |
| (a) + (d) + flat reads (recommended) | Theorems 1 to 4 | all | 5.2 / 6.2 M, once | none |
| (b) trust the factories | Theorems 5, 6, and a sealed `metric` | all, if sealed | about the type floor, 0.2 / 0.8 M | `metric` stops being an aggregate; S8 departure |
| (c) branded wrapper `signature<M>` | Theorem 7 | all | one pass per distinct metric per unit, as (a) + (d) | every tag is spelled differently |
| type test only (floor, not an option) | none | G1 only | 0.2 / 0.8 M | none, but unsound |

Costs are the concept units below (`c-use1` minus `c-base`).

### (a) One pass

The pass, as prototyped in `variants.py` (trees `single`, `cached`):

```cpp
template <std::size_t N>
constexpr bool is_valid_form(const metric<N>& m) noexcept {
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j <= i; ++j) {
      const entry_type a = m.gram[i][j];
      const entry_type b = m.gram[j][i];
      if (a != b || a < -max_form_entry || a > max_form_entry) {
        return false;
      }
    }
  }
  return true;
}
```

**Theorem 1.** For every n ≥ 0 and every n × n matrix G of `int`, `is_valid_form` returns true exactly when `is_symmetric()` and `entries_in_range()` both return true.

Write in(x) for −M ≤ x ≤ M, with M = `max_form_entry`. Reading the three loops, they compute:

- Sym(G): g_ij = g_ji for every pair with j < i (`is_symmetric`, which skips the diagonal, where the equation holds trivially).
- Rng(G): in(g_ij) for every i and j (`entries_in_range`).
- P(G): g_ij = g_ji and in(g_ij), for every pair with j ≤ i (`is_valid_form`; the `if` returns false as soon as either part fails, so the loop returns true exactly when every pair passes).

Proof that P implies Sym and Rng. Sym asks for the equations with j < i; each is one of the equations P asks for, so it holds. For Rng, take any position (i, j). If j ≤ i, in(g_ij) is one of P's conditions. If j > i, swap the roles: the pair (j, i) has its second index below its first, so P gives g_ji = g_ij and in(g_ji); since the two numbers are equal, in(g_ij) holds as well.

Proof that Sym and Rng imply P. Take a pair with j ≤ i. If j < i, Sym gives g_ij = g_ji; if j = i, the equation says g_ii = g_ii. And Rng gives in(g_ij). So both parts of P hold for the pair.

The two directions together give the equivalence. Nothing in it depends on n, and every comparison is between `int` values, with −M = −32767 representable, so no step overflows. Every index is below N, so no read is out of bounds. ∎

The pass reads n(n + 1) entries; today's evaluation reads n² + n(n − 1) + n² = 3n² − n (752 at n = 16 against 272). GCC already shares the two `entries_in_range()` calls through its cache of constant calls, so it saves less than the count suggests.

Machine checks of Theorem 1:

- Solver: the three loops transcribed into formulas over 32-bit signed entries; for each n from 1 to 32 the solver finds no matrix on which today's conditions and the pass differ (`out/equivalence-z3-e1af2ac.txt`, all "unsat"). This checks the transcription, not the C++ text.
- Exhaustive, on the compiled C++: `exhaustive_3.cpp` runs `is_valid_form` and `is_symmetric() && entries_in_range()` on all 7⁹ = 40,353,607 matrices of size 3 with entries from S = {INT_MIN, −M − 1, −M, 0, M, M + 1, INT_MAX}: 729 accepted by each, 0 disagreements, on both compilers, for the plain and the flat pass (`out/exhaustive-3-e1af2ac.txt`). Why seven values cover every `int` matrix of that size: both predicates look at an entry only through two things, which of the three ranges it lies in (below −M, within, above M) and whether it equals its mirror entry. S holds two distinct values in each range, so for any int matrix one can replace each entry by a value of S in the same range, choosing equal values for equal mirror pairs and distinct values for distinct ones; the replaced matrix gets the same answer from each predicate. So a disagreement anywhere would show up as a disagreement on S.
- Exhaustive, on the concept itself: `verdict_check.cpp` compares `metric_like` with a predicate written from the requirement for every 1 × 1 and 2 × 2 matrix over S (7 + 2,401 tags), and checks the counts accepted (3 and 27), in each variant tree, on both compilers (`out/check-e1af2ac.tsv`). The `type` tree fails it, as it must.

The single atomic constraint. Tree `single` writes the value test as `std::bool_constant<detail::is_valid_form(Alg::value)>::value`, so the pass runs once per evaluation instead of once in a guard and once again. Standard, [temp.constr.atomic]/3: if substitution into an atomic constraint "results in an invalid type or expression in the immediate context", the constraint is not satisfied; otherwise "E shall be a constant expression of type bool". A template argument that is not a constant expression makes `std::bool_constant<...>` an invalid type, so the constraint is false rather than an error; once the type is formed, `::value` is always a constant. This is the same mechanism today's guard relies on, inside a requires-expression; that compilers treat a non-constant template argument as a failure in the immediate context, rather than a hard error, is behavior confirmed by the tests on both compilers, not a sentence of the standard this author found.

### (d) The verdict in a variable template

```cpp
template <class Alg>
inline constexpr bool valid_form_v = is_valid_form(Alg::value);

template <class Alg>
concept metric_like =
    detail::is_metric_v<std::remove_cvref_t<decltype(Alg::value)>> &&
    (std::remove_cvref_t<decltype(Alg::value)>::generator_count >= 1) &&
    requires {
      typename std::integral_constant<entry_type, Alg::value.gram[0][0]>;
    } && detail::valid_form_v<Alg>;
```

A variable template's initializer is not in the immediate context of the concept: if it is not a constant expression, the program is ill-formed, a hard error. So the guard must already have established that `is_valid_form(Alg::value)` is a constant expression, and it reads only one entry.

**Theorem 2.** For a tag whose `value` has type `metric<N>` with N ≥ 1, reading `Alg::value.gram[0][0]` in a constant expression succeeds exactly when reading every entry does.

Standard, [expr.const]: an lvalue-to-rvalue conversion (a read) is allowed in a constant expression only when applied to "a non-volatile glvalue that refers to an object that is usable in constant expressions", or to an object whose lifetime began within the evaluation (§7.7.2 ¶2.10). Which objects are usable is a list (¶8, ¶9): those declared by a variable usable in constant expressions (a constexpr variable), template parameter objects, string literal objects, some lifetime-extended temporaries, and "non-mutable subobjects or reference members of any of the above".

1. Every entry gram[i][j] is a subobject of the one complete object that `Alg::value` refers to (directly, or through a reference member). `metric` has no mutable member.
2. So, by the last item of the list, gram[0][0] is usable exactly when that complete object is one of the listed kinds, and the same is true of every other entry. Whether a read is allowed therefore depends on the complete object, not on which entry is read.
3. The read in a concept is not inside an evaluation that created the object, so the second way a read can be allowed does not apply.
4. Hence the read of one entry succeeds exactly when the reads of all succeed. N ≥ 1 makes gram[0][0] exist; the concept tests N ≥ 1 first, from the type alone. ∎

So the purpose of today's "the guard reads every entry" survives: some entry is read even when n = 1. Compiled: the library's own cases (`runtime_value`, `declared_value_1`, `member_value`, the `_1` versions at n = 1) and three more in `verdict_check.cpp` (a constexpr reference to a constexpr object, accepted; a constexpr reference to a non-constexpr object at n = 2 and at n = 1, rejected; a template parameter object, accepted) give the expected verdict in every variant tree on both compilers.

**Theorem 4.** The cached verdict equals the uncached one, and reaching it never turns a false into an error.

1. Order. Standard, [temp.constr.op]/2: "To determine if a conjunction is satisfied, the satisfaction of the first operand is checked. If that is not satisfied, the conjunction is not satisfied." So `valid_form_v<Alg>` is reached only after the type test, N ≥ 1, and the guard have passed.
2. A constant initializer. Then `Alg::value` is readable (Theorem 2), the loop reads only indices below N, compares `int`s without overflow (Theorem 1), and runs n(n + 1)/2 ≤ 528 iterations for n ≤ 32, far below either compiler's step limit. So the initializer is a constant expression and the instantiation cannot fail.
3. Same value. `Alg::value` is a constexpr object, so it is const, and a constant evaluation cannot modify it (modifying an object whose lifetime did not begin within the evaluation is not a constant expression, [expr.const] ¶2.18). So `is_valid_form(Alg::value)` has one value everywhere in the unit, and the variable holds it.
4. Nothing new is assumed about satisfaction. Standard, [temp.constr.atomic]/3: if "the satisfaction result is different for identical atomic constraints" at different points in the program, the program is ill-formed, no diagnostic required. So a valid program already has one verdict per tag; storing it cannot change the meaning of a valid program. ∎

That the compilers evaluate the initializer once and reuse it is behavior, not a requirement of the standard; it is measured below (`cached` and `cached-flat`, `c-sa-use2` costs the same as `c-use1` on both compilers).

What forces Clang's second evaluation, measured in the concept units of today's header at n = 16: one constrained use costs 25.6 M, two different constrained templates 26.3 M (they share one evaluation), a `static_assert` alone 26.0 M, and the `static_assert` with the two templates 50.6 M. So Clang shares the satisfaction of `metric_like<alg>` among constraints, but not between a concept-id written as an expression (in a `static_assert`) and the same concept checked as a constraint. GCC shares both (22.5 M with all three). Inferred, not read in Clang's source: Clang keeps the satisfaction of a template's constraints in a cache keyed by the constrained entity, and evaluates a concept-id expression on its own. With the variable template the question no longer matters: the second check reads a stored `bool` (17.1 M against 16.4 M, within the spread of 1.4 M).

Unexplained: `cached-orig` (today's three conditions in a variable template) costs 30.1 M on Clang for one evaluation, more than today's 25.6 M, although it reads fewer entries. It is not the recommended form and was not traced.

### (e) Reads that are not calls

Most of the pass's cost is the calls: each `m.gram[i][j]` calls `std::array::operator[]` twice, and in constant evaluation a call is an interpreted frame. Without `-D_GLIBCXX_ASSERTIONS` the figures are the same within the spread (`out/cost-no-assertions-e1af2ac/`), so it is the call, not the assertion in it. Tree `cached-flat` copies the matrix once into a struct holding a plain two-dimensional array and reads that:

```cpp
template <std::size_t N>
struct flat_gram {
  entry_type e[N][N];
};
...
  static_assert(sizeof(flat_gram<N>) == sizeof(m.gram));
  const flat_gram<N> g = std::bit_cast<flat_gram<N>>(m.gram);
```

`std::bit_cast` is a single built-in operation in constant evaluation, and the reads of `g.e[i][j]` are built-in subscripts.

**Theorem 3.** When the `static_assert` holds, g.e[i][j] == m.gram[i][j] for every i and j.

1. `std::bit_cast<To>(from)` gives a `To` whose value representation is the object representation of `from` (its bytes), when both are trivially copyable and of equal size, and it is a constant expression unless either type holds a union, pointer, pointer to member, volatile, or reference (Standard, [bit.cast]; the draft words the last part as "constexpr-unknown representation"; the list is recalled, not found on the page read). Both types here are trivially copyable, hold only `int`s, and the `static_assert` makes the sizes equal.
2. A row `std::array<int, N>` is a contiguous container of N `int`s, so it occupies at least N · sizeof(int) bytes; the outer array likewise holds N rows contiguously.
3. sizeof(`flat_gram<N>`) is N · N · sizeof(int) (a C array has no padding), so the `static_assert` says sizeof(m.gram) = N · N · sizeof(int). By step 2 the N rows already fill N · sizeof(int) bytes each, so there is no padding anywhere: row i starts at byte i · N · sizeof(int), and entry j of it at j · sizeof(int) after that.
4. The plain array puts e[i][j] at byte (i · N + j) · sizeof(int), the same place. So each `int` of the copy is the `int` of the original at the same indices. ∎

If a library ever lays out `std::array` with extra bytes, the `static_assert` fails to compile; it cannot pass silently. The flat pass passed every check above (verdict, exhaustive n = 3, the library's tests). It is the one place the proposal departs from the plainest code: STYLE S16 prefers the plain construction "wherever the plain one costs no more", and here the plain one costs about three times as much at n = 16 (14.5 M against 5.2 M on GCC, 16.4 M against 6.2 M on Clang). That trade is for the owner (decision 0121).

A side effect, not a reason: a specialization of `metric<2>` whose `gram` is a type with a lying `operator[]` (`holes/specialized_data.cpp`) defeats today's concept and the plain pass, but not the flat one, which reads the bytes. A specialization can defeat any check, so this is not a guarantee.

### (b) Check once, at the factory, and trust the type

The invariant this needs: every `metric<N>` object that can be read at compile time holds a value that a factory returned.

**Theorem 5.** The factories return only valid metrics.

- `R<P, Q, Z>()` value-initializes the matrix (all zeros) and writes +1 or −1 on the first P + Q diagonal entries. Every entry off the diagonal is 0, so the matrix is symmetric; every entry is −1, 0, or 1, within ±M because M ≥ 1. `check_generator_count` rejects n = 0 by `static_assert`. `rigid<D>()` is `R<D, 0, 1>()`.
- `conformal<D>()` starts from zeros, writes 1 on the first D diagonal entries and −1 at (D, D + 1) and at (D + 1, D). The only nonzero entries off the diagonal are that mirrored pair, so the matrix is symmetric, and every entry is −1, 0, or 1. n = D + 2 ≥ 2.
- `from_gram` asserts symmetry and range with `GALOOT_ASSERT`. It is `consteval`, so its call is evaluated at compile time, and a failed assertion makes the call not a constant expression and the program ill-formed. So a value it returns passed both checks, and n ≥ 1 was checked. ∎

But the invariant itself is false for today's `metric<N>`, which is a public aggregate (a template argument's members must be public: Standard, [temp.param]/12, "all base classes and non-static data members are public and non-mutable"). Compiled, both compilers, with the concept reduced to the type test (tree `type`), each of these asymmetric metrics is accepted, and each is rejected by today's concept and by every proposed variant:

- `holes/hand_aggregate.cpp`: `static constexpr core::metric<2> value{{{{0, 1}, {2, 0}}}};`
- `holes/mutated_copy.cpp`: `auto m = core::rigid<2>(); m.gram[0][1] = 7; return m;` in a constexpr function.
- `holes/forged_by_bit_cast.cpp`: `std::bit_cast<core::metric<2>>(std::array<int, 4>{0, 1, 2, 0})`.

**Theorem 6.** If `metric<N>` had (i) no constructor that user code can call other than copy and move, (ii) a const matrix, (iii) a member whose type keeps `std::bit_cast` out of constant evaluation (a pointer), and (iv) no specialization in the program, then every `metric<N>` object readable at compile time would hold a factory's value.

Argument (Derivation; the steps marked were checked by compiling `holes/sealed.cpp`, `holes/sealed_pointer.cpp`, `holes/forged_by_memcpy.cpp` on both compilers):

1. An object of a class with a user-declared constructor is initialized only by a constructor ([dcl.init], recalled). With (i), user code reaches only the copy and move constructors, and those copy an object that already exists. Aggregate initialization is gone, because the class is no longer an aggregate (checked: `TRY_AGGREGATE`, `TRY_CONSTRUCTOR` fail).
2. A copy holds the value of the object it copies. So, taking objects in the order they are created, each holds a factory's value if each earlier one did.
3. With (ii), assignment to the matrix and assignment of the whole object are ill-formed (checked: `TRY_MUTATION`, `TRY_ASSIGNMENT` fail), and a cast-away-const write is a modification of a const object, which a constant evaluation rejects ([expr.const] ¶2.18 and ¶2.8).
4. `std::bit_cast` and the compilers' constexpr `__builtin_memcpy` create or overwrite an object from bytes without a constructor. With a C-array matrix, `std::bit_cast` still forges a `sealed_metric` on both compilers (`sealed.cpp` compiles its forgery); with (iii) it is no longer a constant expression (checked: `sealed_pointer.cpp` `TRY_BIT_CAST` fails on both). `__builtin_memcpy` into such an object is not a constant expression on either compiler (checked).
5. With (iv), no other definition of `metric<N>` exists. ∎

Each of (i) to (iii) changes a stated property. `metric` stops being an aggregate, which STYLE S8 requires of descriptors and `metric_test.cpp:210` asserts; the tags the tests write by hand would have to use `from_gram`. On Clang 23 a class with a const member of class type, such as `const std::array<...>`, is not trivially copyable, though its copy and move constructors are trivial and its assignments deleted (`holes/const_class_member.cpp` compiles on GCC and fails on Clang); by [class.prop]/1 and [special]/6, which make deleted functions ineligible and ask only that the eligible ones be trivial, GCC's answer looks right, but this was not settled. So keeping "trivially copyable" on both compilers forces `gram` to become a C array, which changes the type of every row users read. The pointer member grows the type by eight bytes and has no meaning of its own.

What it would save: at most the gap between `cached-flat` and the type floor, 5 M (GCC) and 5 to 6 M (Clang) per algebra per unit at n = 16. And nothing for algebras made by `from_gram`: a header that defines `struct alg { static constexpr auto value = core::from_gram({...}); };` evaluates `from_gram`, with its own n² checks, in every unit that includes it. A check made where a value is created runs wherever the value is created, and in a header-only library that is every unit.

What a careless user could still do: nothing, once sealed. A hostile one: specialize `metric<N>`, as today.

### (c) A wrapper that only a validator produces

`holes/wrapped.cpp`: the tag derives from `signature<M>`, whose definition holds `static_assert(valid(M))` and `static constexpr auto value = M;`, and the concept asks whether the tag derives from some `signature<M>`.

**Theorem 7.** A tag that derives from `signature<M>` holds a valid M, unless the program specializes `signature`. Deriving needs the complete base class, so `signature<M>` is instantiated, and its `static_assert` is part of its definition; a failing assertion makes the program ill-formed (checked: `TRY_INVALID` fails on both compilers). An explicit specialization `template <> struct signature<skew>` has no assertion and is accepted (checked: `TRY_SPECIALIZED` compiles on both), which is the hostile case again.

Cost: the check runs once per distinct M per unit, the same as (a) + (d) (one class instantiation per specialization is how both compilers behave; inferred, not measured separately). It changes how every user writes a tag (`struct rga3 : signature<rigid<3>()> {};`, and a plain tag is no longer accepted, checked), and it gains nothing in cost over (d). Its one advantage is the place of the message: an invalid metric fails at the tag's definition, the earliest point (C9), where (d) fails at the first constrained use. A variant with a `consteval` constructor that validates (`checked_metric<N>`) is the same trade: it validates wherever it is constructed, so in every unit, and `std::bit_cast` forges it unless it, too, holds a pointer.

### Other mechanisms considered

- Checking only in checking builds (`GALOOT_CHECKING`): the concept decides overload resolution and template validity, which must not change between build modes. Rejected without measurement.
- Leaving symmetry to the generator: the generator reads only the entries a product needs, so an asymmetric form would give a silently wrong algebra (the products read g_ij for one order of i and j). Rejected.
- A precomputed verdict stored in `metric` itself (a `bool` set by the factories): public like every member, so a hand-written aggregate sets it to true. Rejected.
- C++20 modules: a module interface that defines a tag and instantiates `valid_form_v<tag>` would hold the result in the compiled interface, so importers might not recheck it. Untested; Galoot's headers must work with no build system (C13), so this could only be an addition.

## Measurements

`measure.py` (concept units and plan units) and `measure_no_assertions.py`, headers of `e1af2ac` with `metric.hpp` rewritten by `variants.py`; GCC 16.2.1 and Clang 23.1.4, libstdc++ 16; Galoot's test flags at -O0 (`-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -D_GLIBCXX_ASSERTIONS -DGALOOT_CHECKING=1`), `-c`. Every compile ran as `taskset -c 12 tools/timed measure - <selection> -- <compiler> ...`, pinned to one performance core (CPU 12, sibling 13), one row each in this branch's `runs.tsv` (selections `metric-like-cost:*`, from 18:21:21Z to 18:24:36Z, and `metric-like-cost-no-assertions:*`, 18:25:36Z to 18:25:47Z). Three repeats, interleaved. Other work on CPU 12 at most 0.02 s per compile; the sibling thread was busy at most 0.31 s during one compile. The machine was shared with other sessions (a CTest and Ninja builds; the pool showed 3 of 23 slots free at the end); retired instructions of the compile itself do not count that work, wall times are not used. Summaries: `out/cost-e1af2ac/summary.txt`, `out/cost-no-assertions-e1af2ac/summary.txt`; every row in `rows.tsv` beside them.

An earlier attempt of the same script stopped in its third repeat, because another compile of mine appended to `runs.tsv` while it read the last row; those rows remain in `runs.tsv` under the same selections, and the script now finds its row by selection.

### Concept units

`metric.hpp` alone; the tag `R(n−1, 0, 1)`; millions of instructions above the same unit with no use of the concept; spread (largest minus smallest of three) at most 0.7 M on GCC and 3.8 M on Clang except where given.

| Tree | GCC one use, n = 4 / 16 / 32 | GCC `static_assert` + two uses, n = 16 | Clang one use, n = 4 / 16 / 32 | Clang `static_assert` + two uses, n = 16 |
| --- | --- | --- | --- | --- |
| base (today) | 3.7 / 21.4 / 77.3 | 22.5 | 2.4 / 25.6 / 96.1 | 50.6 |
| single | 2.7 / 14.4 / 50.3 | 15.7 | 1.8 / 16.1 / 58.4 | 30.5 |
| single-guarded | 3.0 / 14.2 / 50.4 | 15.3 | 2.9 / 29.5 / 115.9 (spread 7.4) | 59.3 |
| cached-orig | 3.5 / 21.5 / 76.8 | 22.4 | 3.2 / 30.1 / 118.4 (spread 4.5) | 30.6 |
| cached | 2.8 / 14.5 / 49.9 | 15.4 | 2.2 / 16.4 / 57.2 | 17.1 |
| cached-flat | 2.6 / 5.2 / 13.0 | 6.4 | 1.5 / 6.2 / 22.2 | 8.6 (spread 2.8) |
| type (floor) | 0.3 / 0.2 / 0.3 | 1.3 | 0.0 / 0.8 / 0.1 | 1.5 |

The second constrained template alone adds about 1.1 M on GCC and 0.7 M on Clang in every tree, the floor included: that is the template, not the check.

### Plan units (the measure of decision 0107)

The four typed rigid(3) pairs in R(n−1, 0, 1), four plans minus the same unit with none; growth from n = 4 to n = 16, millions of instructions. Spread at most 1.3 M on GCC and 4.8 M on Clang.

| Tree | GCC, no `static_assert` | Clang, no `static_assert` | GCC, plans with the `static_assert` minus the unit without | Clang, the same |
| --- | --- | --- | --- | --- |
| base (today) | 300.8 → 318.5 (+17.7) | 539.3 → 560.1 (+20.8) | +18.5 | +42.8 |
| single | +11.3 | +12.1 | +11.3 | +27.2 |
| cached | +11.4 | +11.5 | +11.5 | +14.5 |
| cached-flat | +2.5 | +5.2 | +3.0 | +2.1 |
| type (floor) | −0.1 | −1.7 | −0.4 | −2.5 |

Today's row reproduces decision 0107 (about 18 M on GCC, 20 to 24 M on Clang). The header's own cost does not move: the concept unit with no use of the concept is within 1 M of today's in every tree, at every n (`c-base` in `out/cost-e1af2ac/summary.txt`).

What is confirmed and what is not:

- Confirmed (measured): the costs above on this machine, these compilers, these flags; one evaluation per unit for the cached trees on both compilers; two on Clang for the uncached ones when the concept is also evaluated as an expression.
- Inferred: that the per-call cost of `std::array::operator[]` in constant evaluation is what the flat pass saves (the counts fit; no compiler profile was taken); why Clang does not share an expression's evaluation with a constraint's; the extra 4 M of `cached-orig` on Clang is unexplained.
- Not measured: wall time (shared machine), peak memory beyond what `rows.tsv` records, other standard libraries (libc++), and the cost of the sealed type of option (b), which was not built into the library.

## What the proposal keeps of the header's comments

- "The concept tests the data only, and is false rather than an error for a type that has no such member": kept.
- "The guard reads every entry, so that it fails for a value that is not usable at compile time even when n = 1": becomes "the guard reads one entry, which decides whether the value can be read at compile time at all" (Theorem 2).
- "an aggregate written by hand is checked by metric_like before an algebra can use it" (the `metric` comment): kept, and checked by `holes/hand_aggregate.cpp`, `mutated_copy.cpp`, `forged_by_bit_cast.cpp` in every proposed tree.

## Files

All in `docs/agent/records/performance/metric-like-check/`:

| File | Use |
| --- | --- |
| `variants.py` | Writes the variant include trees from a Galoot `include/` |
| `check.py` | The correctness checks of each tree: `verdict_check.cpp`, `metric_test.cpp`, `plan_test.cpp`, the `plan_not_metric_like` compile-fail test → `out/check-e1af2ac.tsv` |
| `verdict_check.cpp` | Exhaustive verdict at n = 1 and 2 and the reference cases |
| `exhaustive_3.cpp` | Exhaustive comparison at n = 3, built at -O2 and run → `out/exhaustive-3-e1af2ac.txt` |
| `equivalence_z3.py` | Solver proof of Theorem 1's transcription for n = 1 to 32 → `out/equivalence-z3-e1af2ac.txt` |
| `holes.py`, `holes/` | What compiles, per tree and sketch → `out/holes-e1af2ac.tsv` |
| `measure.py`, `measure_no_assertions.py` | The pinned measurements → `out/cost-*` |

Reproduce, from this worktree with the pool exported (`eval "$(tools/jobpool env)"`):

```
R=docs/agent/records/performance/metric-like-check
python3 -I $R/variants.py include /tmp/v
python3 -I $R/check.py $PWD /tmp/v $R/out/check.tsv
python3 -I $R/holes.py $PWD /tmp/v $R/out/holes.tsv
python3 -I $R/measure.py $PWD /tmp/v $R/out/cost 12 3
tools/timed check - z3 -- uv run --frozen --project <math-tools>/py python -I $PWD/$R/equivalence_z3.py
```

## Correction, 2026-11-01 (W5 review of plan 0014, finding F5)

Added by Claude (agent, Opus 5.5); the text above is kept. Both conclusions stand; two steps said more than is true.

- Theorem 2, step 1 and 2. Usability of an entry in a constant expression is decided by the path from the object that `Alg::value` designates, not by the complete object alone: a reference to a `mutable` subobject of a constexpr object cannot be read, although its complete object is constexpr (the reviewer's case 14). The argument holds because every entry of `gram` is reached from that object by the same kind of path, through `metric` and `std::array`, which have no mutable member and no union; so one entry is readable exactly when every entry is.
- Theorem 4, step 3. "`Alg::value` is a constexpr object" should read "a const object usable in constant expressions": it may also be a template parameter object, or the object a constant reference names. Each is const, so a constant evaluation cannot modify it, and the step stands.
