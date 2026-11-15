# 0220 The change of basis: its interface, its messages, and its term lists

- Status: recorded, as a proposal. Part D of plan 0030 (cpp-expert with hci-expert judgement). The shape is the owner's ruling on O3 (decision 0219 item 3: a tag carrying source, target and value; `change_basis`, `change_basis_back`, `conformal_change<D>()`, `from_rows`; the inverse computed by the core). The owner confirms the names after this record and before part B starts (decision 0219 item 3). The details below that O3 did not rule are this record's choices, open to the owner and to the blind review (C18).
- Date: 2026-11-09
- Recorded by: Claude (agent, Opus 5.5), cpp-expert in the designer role, on `records/wu12-part-d` (worktree `.claude/worktrees/wu12-part-d`) from `develop` at `a6307ba`.
- Number: 0220, as plan 0030 reserved it; free on every branch at `a6307ba`.
- Bears on: plan 0030 sections 4 (criteria 1 to 8), 6, 9 part D, R4 to R6, risk 2, O2, O3; plan 0001 DoD-3, DoD-7, DoD-12, DoD-19; decisions 0033, 0151, 0187, 0205, 0219; CONVENTIONS 1.1, 1.4, 1.7; STYLE S7 to S11, S16; `records/diagnostics/misuse-catalogue.md`.

## 1. Departures, skipped checks, and readings (C20)

1. **Two public names that section 6 did not list.** Section 6 names the functions, the factories, and `change_basis_layout`. A complete interface also needs the value type and the concept the tag must meet, as `metric<N>` and `metric_like` serve an algebra tag. This record adds `basis_change<N>` and `basis_change_like`, and `change_basis_back_layout` beside `change_basis_layout` (decision 0187's rule that each result layout has a public name). The owner's confirmation of names covers them.
2. **`from_rows` checks nothing beyond its shape, against plan 0030 criterion 1's "checked when the change is generated" and C9's "earliest point".** Every value check runs once per tag, when the tag is first used (section 3.4). A consteval `GALOOT_ASSERT` in `from_rows` would report a fault at the tag's definition, one line earlier in practice, but the prototype shows that Clang then gives two errors with the message only in a note, as `from_gram` does today (`core.metric_asymmetric`). A `static_assert` in the per-tag check gives one error with the message on the first line, on both compilers, and puts each rule in one place. Reading followed: the earliest point at which C9's message can be the first error line is the tag's first use. Open item OI2 asks the owner to confirm.
3. **The tag-level messages start with `basis_change:`, not with the operation as written.** The usability checklist asks that a message start with the operation as written. The checks of a tag run once per tag, whichever of `change_basis`, `change_basis_back`, or a layout first uses it, so one message cannot name the operation the user wrote without doubling every message. The prefix names the value type, as `metric:` names the metric factories and `blade_set:` the blade sets. The messages that depend on the call (the operand's algebra, the scalar) start with the function's name. Open item OI3.
4. **Sixteen messages, not plan 0030's estimate of eight to twelve** (section 12's compile-fail row). Section 3.5 lists them. At the plan's 0.5 to 1 s per message and compiler, the compile-fail lane grows by about 16 to 32 s of slot time per preset pair (inferred from the plan's figure, not measured).
5. **The determinant and the inverse are computed by fraction-free elimination, not by the wedge fold.** Plan 0030 section 6 says "adj(M) Δ over det M", without a method, and R4 maps the term lists to the wedge's fold. The fold could also give the cofactors and the determinant, as the images of the blades of grades n − 1 and n, but that costs, for a dense change, about Σ_m C(n, m) term insertions whatever layout the program uses, which breaks C6 for a change used only on vectors. Elimination costs O(n³). Section 4.4 gives the reasoning; open item OI5 asks whether the owner reads it as per-basis data (C4) or as machinery that needs a decision of its own.
6. **The prototype ran outside the pool and outside the run log.** I compiled four single-file prototypes (section 6) with `g++` and `clang++` directly, each through `tools/timed --log` into a log in my scratch directory, with step `prototype`, so this records branch carries no run rows and `tools/timed` did not ask for the pool. No CMake build or CTest ran, and the job pool was not started (it read "not running" and stayed so). The prototype is not committed; its paths are in the report.
7. **Part A's results are not yet available.** The header comment of section 3.1 lists what a change preserves, from plan 0030 section 5.2's hypotheses, with the factors that part A must prove marked "[part A]". Part C writes the final text from research note 0026.

## 2. Context

Plan 0030 gives the core an exact change of basis between two algebras of the same dimension whose forms are congruent over the rationals (C2, decision 0003). The owner ruled that the change is integer rows over one positive denominator per row, with no rational type in the core (O2), and that the interface is a tag carrying source, target and value, applied by `change_basis` and `change_basis_back`, built by `from_rows` or `conformal_change<D>()`, with the inverse computed by the core (O3). Part D settles the spelling, writes the declarations and their `///` comments as text, lists the messages the misuse catalogue gains, and maps `change_basis` to the term-list mechanism (R4, DoD-3), before any test is written.

## 3. Decision

### 3.1 The spelling, as a user writes it

```cpp
#include <galoot/core.hpp>

struct cga3 { static constexpr auto value = galoot::core::conformal<3>(); };
struct r410 { static constexpr auto value = galoot::core::R<4, 1, 0>(); };

// A change of basis is a tag, as an algebra is: diagnostics and symbols name
// it, and its checks run once per tag in each translation unit.
struct cga3_to_r410 {
  using source = cga3;
  using target = r410;
  static constexpr auto value = galoot::core::conformal_change<3>();
};

constexpr auto point_layout = galoot::core::blades<cga3, "e1 e2 e3 e4 e5">;
using Point = galoot::core::mv<cga3, point_layout, double>;

Point p{1, 2, 3, 1, 7};
auto q = galoot::core::change_basis<cga3_to_r410>(p);       // in r410
auto back = galoot::core::change_basis_back<cga3_to_r410>(q);  // in cga3
Point p2 = galoot::core::truncate<Point>(back);              // p2 == p

// A change written by hand: the scaled Witt form against R(1, 1, 0), with
// e1 = e'1 + e'2 and e2 = (e'1 - e'2) / 2.
struct witt1 {
  static constexpr auto value = galoot::core::from_gram({{0, 1}, {1, 0}});
};
struct r110 { static constexpr auto value = galoot::core::R<1, 1, 0>(); };
struct witt1_to_r110 {
  using source = witt1;
  using target = r110;
  static constexpr auto value =
      galoot::core::from_rows({{1, 1}, {1, -1}}, {1, 2});
};
```

`change_basis` and `change_basis_back` are found by argument-dependent lookup through `x` (C++20 allows an explicit template argument there), so `change_basis<cga3_to_r410>(p)` works unqualified, as `truncate<Point>(m)` does.

The header comment of `include/galoot/core/basis_change.hpp`, as text (part C finalizes the bracketed parts from research note 0026 and its own measurements):

```cpp
// Exact changes of basis between two algebras with the same number of
// generators, whose forms are congruent over the rationals, such as
// conformal(3) and its diagonal counterpart R(4, 1, 0):
//
//   struct cga3 { static constexpr auto value = galoot::core::conformal<3>(); };
//   struct r410 { static constexpr auto value = galoot::core::R<4, 1, 0>(); };
//   struct cga3_to_r410 {
//     using source = cga3;
//     using target = r410;
//     static constexpr auto value = galoot::core::conformal_change<3>();
//   };
//
//   constexpr auto point_layout = galoot::core::blades<cga3, "e1 e2 e3 e4 e5">;
//   using Point = galoot::core::mv<cga3, point_layout, double>;
//   Point p{1, 2, 3, 1, 7};
//   auto q = galoot::core::change_basis<cga3_to_r410>(p);  // in r410
//   Point p2 = galoot::core::truncate<Point>(
//       galoot::core::change_basis_back<cga3_to_r410>(q));  // p2 == p
//
// A change is a matrix P whose row i writes the source's generator e_i in
// the target's generators e'_1 to e'_n: e_i = P_i1 e'_1 + ... + P_in e'_n.
// It is held exactly, as integer rows over one positive denominator per row,
// P_ij = rows[i][j] / denominators[i], so no entry is ever rounded. It must
// keep every inner product, P G' P^T = G for the Gram matrices G of the
// source and G' of the target, and it must be invertible; both are checked,
// in integers, the first time a program uses the change.
//
// The change sends a blade e_A = e_a1 ∧ ... ∧ e_ak to the wedge of the
// images of its generators, which is the sum over the target's blades e'_B
// of A's grade of det P[A, B] e'_B, the minor of P on the rows of A and the
// columns of B. So it keeps grades, sends the scalar 1 to 1, and sends the
// antiscalar of the source to det(P) times the antiscalar of the target.
// change_basis_back applies the inverse change, P^-1, which the library
// computes exactly from the rows.
//
// Because P keeps the inner products, the change is an isomorphism of the
// two algebras. It preserves the geometric product, the wedge, the dot, the
// reverse, the antireverse, and the grade involution: change_basis(x ⟑ y) is
// change_basis(x) ⟑ change_basis(y), and likewise for the others. Through
// the antiscalar it scales [part A: the antiwedge by 1 / det(P) and the bulk
// dual by det(P)]. The geometric antiproduct, the weight dual, the antidot,
// and transform are preserved [part A: with these factors] only when
// [part A's condition], as for conformal_change. The complements, the bulk,
// and the weight are not preserved in general, because they are defined on
// the coordinates of one basis: in conformal(3) the bulk of the origin e4 is
// -e5, while [part A: computed by way of R(4, 1, 0) it is -e5 / 2]. So a
// result of these depends on the basis it was computed in.
//
// Result layouts: the image of a layout is the set of the target's blades
// that the images of its blades reach, each with orientation +1, in order of
// grade and then of index list, as for a product; change_basis_layout<C, S>
// names it. Convert it to a named type, Type{q}, to read it under that
// type's orientations. A round trip can hold more blades than it started
// with: the origin e4 alone maps onto e4' and e5', and those map back onto
// e4 and e5, with zero on e5 in exact arithmetic. truncate<Point> reads it
// back and, for an exact scalar, checks that what it drops is zero. Nothing
// here drops a component, so nothing here checks one.
//
// Scalars: each result coefficient is an integer combination of the input's
// coefficients, divided once by a positive integer where the denominators
// reach that blade. A scalar with exact division, such as a rational type,
// gives the exact result; a floating scalar rounds as one sum and one
// division per coefficient; an integer scalar is accepted only on a layout
// where no division is needed.
//
// Compile cost: [part C: the measured limits on both compilers at the
// default limits, as dual.hpp states them; the checks of a change cost
// O(n^3) to O(n^4) integer operations once per change, and the terms of a
// layout cost what its blades' images cost, which for a dense P grows with
// the binomial coefficient C(n, k) of each grade k the layout uses].
```

### 3.2 The declarations, with their `///` comments

These are text for part C. Names and signatures are the proposal; the comments are drafts that part C may tighten and must keep true. `[part A]` and `[part C]` mark what those parts supply.

```cpp
namespace galoot::core {

/// An exact change of basis between two algebras of N generators: row i
/// writes the source's generator e_(i+1) in the target's generators e'_1 to
/// e'_N as
///   e_(i+1) = (rows[i][0] e'_1 + ... + rows[i][N-1] e'_N) / denominators[i],
/// so the matrix P of the change has entries rows[i][j] / denominators[i],
/// held exactly as integer rows over one denominator per row. from_rows and
/// conformal_change build one. A tag whose `value` is a basis_change, beside
/// its `source` and `target` algebra tags, is a change that change_basis and
/// change_basis_back apply; basis_change_like says what the tag requires and
/// what is checked.
///
/// Requires: N <= mask_bits, checked at compile time with a message that
/// names it. The values are checked only when a tag that holds them is first
/// used.
/// Guarantees: a structural type, so a change can be a template argument;
/// trivially copyable, and never allocates.
template <std::size_t N>
struct basis_change {
  /// rows[i][j] is the numerator of P_ij, the coefficient of e'_(j+1) in the
  /// image of e_(i+1).
  std::array<std::array<entry_type, N>, N> rows;

  /// denominators[i] divides every entry of row i. Each must be positive.
  std::array<entry_type, N> denominators;

  /// The number of generators of the source and of the target, n.
  static constexpr std::size_t generator_count = N;

  /// Two changes are equal when their rows and their denominators are equal
  /// entry by entry, so a row scaled together with its denominator gives an
  /// unequal value for the same matrix P.
  [[nodiscard]] constexpr bool operator==(const basis_change&) const = default;
};

/// The change whose row i is rows[i] over denominators[i]:
/// e_(i+1) = (rows[i][0] e'_1 + ... + rows[i][N-1] e'_N) / denominators[i].
/// For the form [[0, 1], [1, 0]] against R(1, 1, 0), with e1 = e'1 + e'2 and
/// e2 = (e'1 - e'2) / 2: from_rows({{1, 1}, {1, -1}}, {1, 2}). The parameters
/// are C arrays so that N is deduced from the braces. A row need not be
/// reduced: {{2, 2}, ...} over {2, ...} is the same matrix as {{1, 1}, ...}
/// over {1, ...}, and the library reduces what it generates.
///
/// Requires: N rows of N entries and N denominators, or the call does not
/// match; N <= mask_bits, checked as for basis_change. Nothing else is
/// checked here, because the algebras are not known here: that every
/// denominator is positive, that the rows are independent, and that P keeps
/// the inner products of the two algebras are checked when a tag that holds
/// the result is first used, each with a message that names it.
/// Guarantees: rows and denominators equal the arguments entry by entry.
template <std::size_t N>
[[nodiscard]] consteval basis_change<N>
// NOLINTNEXTLINE(*-avoid-c-arrays): a C array lets N be deduced from braces.
from_rows(const entry_type (&rows)[N][N], const entry_type (&denominators)[N]);

/// The change of basis from conformal(D) to its diagonal counterpart
/// R(D+1, 1, 0). The generators e1 to eD are kept, e_i = e'_i; the origin
/// e(D+1) is (e'(D+2) - e'(D+1)) / 2; and the point at infinity e(D+2) is
/// e'(D+2) + e'(D+1), where e'(D+1) squares to +1 and e'(D+2) to -1. Its
/// determinant is -1, so e(D+1)(D+2) maps to -e'(D+1)(D+2) and the antiscalar
/// to minus the target's antiscalar.
///
/// Requires: D + 2 <= mask_bits, checked as for basis_change. A tag that
/// holds it names an algebra of conformal<D>() as its source and one of
/// R<D+1, 1, 0>() as its target; any other pair is rejected when the tag is
/// first used, since the change then does not keep the inner products.
/// Guarantees: the rows of that change, with every denominator 1 but the
/// origin's, which is 2.
template <std::size_t D>
[[nodiscard]] consteval basis_change<D + 2> conformal_change();

/// A tag type that names a change of basis: its source algebra, its target
/// algebra, and the change between them.
///
/// Requires of C: member types `source` and `target` that are metric_like,
/// and a static data member `value`, usable in a constant expression, of type
/// basis_change<N> with N >= 1. The concept tests these data only, and is
/// false rather than an error for a type that lacks one of them, such as an
/// algebra tag. The rest is checked once per tag in each translation unit,
/// when change_basis, change_basis_back, or one of the layouts below first
/// uses it, each with a message that names it: the source and the target
/// each have N generators; every denominator is positive; the rows are
/// linearly independent, so the change has an inverse; and P G' P^T = G for
/// the Gram matrices G of the source and G' of the target, so that the
/// change keeps every inner product. The last is computed exactly in
/// integers, as M G' M^T = D G D for the integer rows M and the diagonal D of
/// the denominators. A value of these checks, of the inverse, or of the
/// generated terms that does not fit coefficient_type fails the constant
/// evaluation with a message, so it cannot pass silently.
template <class C>
concept basis_change_like = /* [part C], as metric_like reads one entry */;

// clang-tidy reads the operation, once substituted, as a C-style cast of its
// value, which the source does not have.
// NOLINTBEGIN(modernize-avoid-c-style-cast)

/// The layout of change_basis<C>(x) for x on the layout S of C's source
/// algebra: the target's blades that the images of S's blades reach, each
/// with orientation +1, in order of grade and then of index list, as for a
/// product. Each image keeps its blade's grade. For conformal_change<3>(),
/// the point e1 e2 e3 e4 e5 gives e1 to e5 of the target, and the origin e4
/// alone gives e4 and e5.
template <basis_change_like C, blade_set S>
inline constexpr auto change_basis_layout = /* [part C] */;

/// The layout of change_basis_back<C>(y) for y on the layout S of C's target
/// algebra, as for change_basis_layout, through the inverse change. For
/// conformal_change<3>(), e4 alone gives e4 and e5 of the source.
template <basis_change_like C, blade_set S>
inline constexpr auto change_basis_back_layout = /* [part C] */;

// NOLINTEND(modernize-avoid-c-style-cast)

/// x written in the generators of C's target: the image of x under the
/// change, which sends each generator e_i to row i of P and each blade
/// e_A = e_a1 ∧ ... ∧ e_ak to the wedge of the images of its generators,
/// the sum of det P[A, B] e'_B over the target's blades e'_B of A's grade.
/// It keeps grades and the scalar, and sends the antiscalar to det(P) times
/// the target's antiscalar. It is an algebra isomorphism: the header's
/// comment lists the operations it preserves, with their factors, and those
/// it does not, such as the bulk.
///
/// Each result coefficient is computed by straight-line code, as for a
/// product, as one integer combination of x's coefficients divided once by a
/// positive integer where the change's denominators reach that blade:
/// y_B = (c_1 x_A1 + ... + c_m x_Am) / d_B, with the c and d_B exact integers
/// that share no factor, and no division where d_B is 1. Over float or
/// double that is one sum of products and one division, so the coefficient
/// rounds as they do; for conformal_change every d_B is 1 or 2, and the
/// division by 2 is exact unless it underflows.
///
/// Requires: C is basis_change_like and passes its checks; x is in C's
/// source algebra; the scalar holds every c and d_B exactly where its
/// numeric_limits can tell; and, where some d_B of this layout is not 1, the
/// scalar has a division and is not an integer type, so that no quotient is
/// truncated. Otherwise the call is rejected at compile time with a message
/// that names change_basis. An integer scalar is accepted on a layout that
/// needs no division, as for a permutation of the generators, or for
/// conformal_change on the Euclidean blades. A scalar whose division
/// truncates and whose numeric_limits do not say it is an integer, such as
/// some big-integer classes, is not detected.
/// Guarantees: on change_basis_layout<C, S>, in C's target algebra. The
/// result is exact for an exact scalar with exact division. Never checks: it
/// drops nothing. The terms are generated once in each translation unit for
/// each tag and layout it uses.
template <basis_change_like C, blade_set S, ring_scalar T>
[[nodiscard]] constexpr mv<typename C::target, change_basis_layout<C, S>, T>
change_basis(const mv<typename C::source, S, T>& x);

/// y written in the generators of C's source: the image of y under the
/// inverse change P^-1, which the library computes exactly from C's rows
/// when the tag is first used. change_basis_back<C>(change_basis<C>(x)) is x
/// on x's blades, and zero on any other blade it holds, in exact
/// arithmetic; truncate<Type> reads it back as x's type. Otherwise as for
/// change_basis, with source and target exchanged and P^-1 for P: it sends
/// the target's antiscalar to 1 / det(P) times the source's.
///
/// Requires: as for change_basis, with y in C's target algebra, and the
/// message naming change_basis_back.
/// Guarantees: on change_basis_back_layout<C, S>, in C's source algebra.
template <basis_change_like C, blade_set S, ring_scalar T>
[[nodiscard]] constexpr mv<typename C::source, change_basis_back_layout<C, S>,
                           T>
change_basis_back(const mv<typename C::target, S, T>& y);

/// Rejects a change_basis whose x is not in C's source algebra, with one
/// error that names the cause; the message says to use change_basis_back
/// when x is in the target. Because it exists, a requires-expression that
/// asks whether such a call is valid is told yes; compare x's algebra_type
/// with C::source instead.
template <basis_change_like C, multivector X>
  requires(!std::same_as<typename std::remove_cvref_t<X>::algebra_type,
                         typename C::source>)
constexpr std::remove_cvref_t<X> change_basis(const X& x);

/// Rejects a change_basis_back whose y is not in C's target algebra, as for
/// change_basis.
template <basis_change_like C, multivector X>
  requires(!std::same_as<typename std::remove_cvref_t<X>::algebra_type,
                         typename C::target>)
constexpr std::remove_cvref_t<X> change_basis_back(const X& y);

/// Rejects a change_basis whose template argument is not a change tag, such
/// as an algebra tag, with one error whose message shows a change tag.
/// Because it exists, a requires-expression that asks is told yes; ask
/// basis_change_like instead.
template <class C, multivector X>
  requires(!basis_change_like<C>)
constexpr std::remove_cvref_t<X> change_basis(const X& x);

/// Rejects a change_basis_back whose template argument is not a change tag,
/// as for change_basis.
template <class C, multivector X>
  requires(!basis_change_like<C>)
constexpr std::remove_cvref_t<X> change_basis_back(const X& y);

} // namespace galoot::core
```

`basis_change.hpp` joins `galoot/core.hpp`, after `transform.hpp`. It includes `<array>`, `<concepts>`, `<cstddef>`, `<type_traits>`, `<utility>`, and the core headers it uses (`assert.hpp`, `bits.hpp`, `blade_product.hpp`, `blade_set.hpp`, `metric.hpp`, `mv.hpp`, `plan.hpp`).

### 3.3 What the names say

| Name | Kind | Why this name |
| --- | --- | --- |
| `basis_change<N>` | the value type | a noun for the data, as `metric<N>` is; the header is `basis_change.hpp` |
| `basis_change_like` | the concept a tag meets | parallel to `metric_like` |
| `source`, `target`, `value` | the tag's members | the terms for a map's domain and codomain; `value` as in every algebra tag |
| `rows`, `denominators` | the value's members | row i is the image of e_i, which is what a user writes; one denominator per row |
| `from_rows(rows, denominators)` | factory | parallel to `from_gram(gram)` |
| `conformal_change<D>()` | factory | parallel to `conformal<D>()`; CONVENTIONS 1.4's change |
| `change_basis<C>(x)`, `change_basis_back<C>(y)` | the two maps | O3's names; a verb, as `transform` and `truncate` are; `back` is plain English for the inverse direction |
| `change_basis_layout`, `change_basis_back_layout` | result layouts | `<function>_layout`, as every result layout is named (decision 0187) |

### 3.4 When each check runs

Every check of a tag's value is a `static_assert` in one consteval function per tag, held in a variable template (`detail::checked_change<C>`, a name of part C's choosing), so it runs once per tag and translation unit, and each fault stops the checks after it through an `if constexpr` chain, so a tag with several faults gives one error. The function returns, beside the verdict, the forward rows and the inverse rows as `coefficient_type`, which the term generation reads. The order is the order of section 3.5's rows B1 to B5: counts, denominators, independence, isometry. Independence comes before the isometry so that a degenerate source form, where the isometry can hold for a singular P, gets the message that names the real fault.

The use-site checks are `static_assert`s in `change_basis` and `change_basis_back`, after the tag's checks have run through the return type. A tag that fails gives the empty change map, so the function adds no second error.

### 3.5 The misuse rows the catalogue gains

Each is one error on GCC and Clang, its literal message on the first error line, also through `auto r = …;` (the catch-alls return the operand's type, as `reject_dual_operands` does). B6 is a `GALOOT_ASSERT` in a constant evaluation, whose text both compilers show in the note that quotes the failing line, as for `core.blade_product_overflow`. The message texts are the proposal; the test names are suggestions for part B.

| Row | Misuse, as a user writes it | Rejected by | Message | Tests (suggested) |
| --- | --- | --- | --- | --- |
| B1 | a tag whose source has another number of generators than the change, as `source = rga3` with `conformal_change<3>()` | per-tag check; the condition is the comparison of the two counts as written (`C::source::value.generator_count == C::value.generator_count`), so that both compilers print them | "basis_change: the source algebra has a different number of generators than the change has rows; a change maps between algebras of one dimension" | `core.basis_change_source_count` |
| B2 | the same for the target | per-tag check | "basis_change: the target algebra has a different number of generators than the change has rows; a change maps between algebras of one dimension" | `core.basis_change_target_count` |
| B3 | `from_rows({{1, 0}, {0, 1}}, {1, -2})`, or a zero denominator, or a hand-written `basis_change` with one | per-tag check | "basis_change: every denominator must be positive; put a row's sign in its entries" | `core.basis_change_denominator_negative`, `core.basis_change_denominator_zero` |
| B4 | rows that are linearly dependent, as `{{1, 2}, {2, 4}}` | per-tag check | "basis_change: the rows are linearly dependent, so the change has no inverse; the images of the source's generators must be independent" | `core.basis_change_dependent`, and one with a degenerate source form where P G′ Pᵀ = G holds for the singular P |
| B5 | rows that do not keep the inner products, including a tag with source and target swapped, and P transposed | per-tag check | "basis_change: the rows do not carry the target's form to the source's (P G' P^T is not G); check the rows, and that source and target are not swapped" | `core.basis_change_not_isometry`, `core.basis_change_swapped` |
| B6 | rows whose minors, inverse, or isometry check do not fit `coefficient_type` | `GALOOT_ASSERT` in the generation | "basis_change: an exact value the change computes does not fit coefficient_type; use rows with smaller entries" | `core.basis_change_overflow` (at least one route; the others are listed in `test/internal_messages.txt` if no input reaches them separately) |
| U1 | `change_basis<cga3>(x)`, an algebra tag or any type that is not a change | catch-all | "change_basis: the template argument is not a change of basis; give a tag with source, target, and value, as in struct cga3_to_r410 { using source = cga3; using target = r410; static constexpr auto value = conformal_change<3>(); }" | `core.change_basis_not_a_change` |
| U1′ | the same for `change_basis_back` | catch-all | the same text with `change_basis_back:` | `core.change_basis_back_not_a_change` |
| U2 | `change_basis<C>(w)` with w in the target, or in a third algebra | catch-all | "change_basis: x is not in the change's source algebra; give x the source's algebra tag, or use change_basis_back to map from the target" | `core.change_basis_target_operand`, `core.change_basis_other_algebra`, `core.change_basis_other_algebra_auto` |
| U2′ | `change_basis_back<C>(x)` with x in the source, or a third algebra | catch-all | "change_basis_back: y is not in the change's target algebra; give y the target's algebra tag, or use change_basis to map from the source" | `core.change_basis_back_source_operand`, `core.change_basis_back_other_algebra` |
| U3 | a scalar that cannot hold a term coefficient or a denominator exactly, as `float` for 2^24 + 1 | `change_basis` | "change_basis: the scalar type cannot hold one of the change's exact coefficients; use a scalar type that holds larger integers exactly" | `core.change_basis_narrow_scalar` |
| U3′ | the same through `change_basis_back` | `change_basis_back` | the same text with `change_basis_back:` | `core.change_basis_back_narrow_scalar` |
| U4 | `std::int64_t` on a layout that needs a division, as the origin e4 under `conformal_change<3>()`; or a ring scalar without `/` there | `change_basis` | "change_basis: the change divides by an integer on this layout, which the scalar type cannot do exactly; use a rational or floating scalar type" | `core.change_basis_integer_division`, `core.change_basis_no_division` |
| U4′ | the same through `change_basis_back` | `change_basis_back` | the same text with `change_basis_back:` | `core.change_basis_back_integer_division` |
| G1 | a hand-written blade set naming a generator beyond e_n, given to `change_basis_layout` or `change_basis_back_layout` directly (`mv` rejects such a layout before a call can reach here) | the term generation, as the plans do | "basis change map: the blade set names a generator beyond e_n" | `core.basis_change_layout_beyond_n` |
| G2 | the same with a sign other than +1 or −1 | the term generation | "basis change map: the blade set holds a sign other than +1 or -1" | `core.basis_change_layout_bad_sign` |

Catalogue placement: U2 and U2′ join DoD-19's "Mixed algebras" item; B1 to B5 a new group "Change of basis" beside "Asymmetric form"; U3 and U3′ the "Narrow scalar" group; U4 and U4′ a new group "Division"; G1 and G2 the "Plans and products" group; U1 and U1′ the "Graded signature" item's neighbor, a tag that is not what the function takes. Part C may add internal invariant checks of the generated map (as `make_product_plan` has), whose messages go to `test/internal_messages.txt` with their reason.

Accepting and rejecting `static_assert`s for the concept (S7): `basis_change_like<cga3_to_r410>` holds; it fails for an algebra tag, for a tag without `target`, for one whose `value` is a `metric`, and for one whose `value` is not usable in a constant expression.

### 3.6 Scalars and rounding (for part N)

- The integer term coefficients c and the output denominators d_B are reduced so that, for each output blade, the gcd of d_B and all its c is 1. An output with d_B = 1 has no division.
- The sum is formed by the existing straight-line kernel (`run_map` over `kernel_term`s, the first term assigned, a coefficient of ±1 an addition or a subtraction), then each output with d_B ≠ 1 is divided once: `out = static_cast<T>(out / static_cast<T>(d_B))`. The division is the scalar's own `/`, never a multiplication by a reciprocal, which would round twice.
- So the rounding model per coefficient is that of one sum of m products followed by one division, as plan 0030 criterion 8 states, with m the number of terms on that output. Part N writes the bound from `test/support/tracked.hpp`'s counts.
- For `conformal_change` every c is ±1 or ±2 and every d_B is 1 or 2; on dyadic inputs every intermediate is exact unless it underflows or overflows.

## 4. The change's term lists and the one mechanism (R4, DoD-3)

### 4.1 What is reused, and what is per-basis data

The image of a blade e_A, A = {a1 < … < ak}, is the wedge of the images of its generators, φ(e_a1) ∧ … ∧ φ(e_ak), with φ(e_i) = (Σ_j M_ij e′_j) / Δ_i for the integer rows M and the denominators Δ. Its term list is the wedge's term list folded over the rows:

1. Start with the term list {1 · e′_∅}.
2. For each generator a_m of A in increasing order, replace the list L by Σ_{t ∈ L} Σ_j M_(a_m, j) · (t ∧ e′_j), where t ∧ e′_j is the wedge's pair rule (`pair_terms<wedge>`: zero when t's blade holds e′_j, otherwise `reorder_sign(t.blade, e′_j)` on the blade of the union). The target's metric is not read, because the wedge does not read it.
3. The result is Σ_B det M[A, B] e′_B, by the Leibniz expansion of each minor, and the image of e_A is that over Δ_a1 ⋯ Δ_ak.

So the generation composes the mechanism's wedge rule with per-basis integer coefficients, the entries of M, and per-basis denominators. A layout's map is then typed exactly as a unary map is: the image terms of each position a of the layout, with the layout's sign s_a folded in, become `plan_term`s with b = 0, and `collect_result` and `type_terms` put them on the canonical result set. One step is new: each output blade B gets the denominator d_B, the least common multiple of Δ_A = Π_{i ∈ A} Δ_i over the input blades A whose image reaches B, and each term's coefficient becomes c_AB = s_a det M[A, B] · d_B / Δ_A, after which d_B and its c are divided by their gcd. The kernel is a list of `kernel_term`s, applied by the existing `run_map`, followed by one division per output with d_B ≠ 1. Nothing else of the run-time path is new.

`change_basis_back` is the same generation over the inverse rows.

### 4.2 The cost the fold has, and the capacity of its lists

For a blade of grade k, the fold's partial wedges have grades 0 to k, so its term lists need room for C(n, min(k, ⌊n/2⌋)) terms, not C(n, k): the prototype's first build overflowed a capacity of C(5, 5) = 1 for the image of the antiscalar. With s_i nonzero entries in row i, the list after m steps holds at most min(C(n, m), s_a1 ⋯ s_am) terms. For `conformal_change` every row has one or two entries, so an image has at most two terms per null generator it holds. For a dense P, the image of a grade-k blade costs about Σ_{m ≤ k} n · C(n, m) insertions into a sorted list; part C measures the limits and states them (criterion 15). This is the only part of the change whose cost grows with the layout, and it grows only with the blades the layout holds (C6).

### 4.3 The checks and the inverse are per-tag data

The isometry check computes M G′ Mᵀ and Δ G Δ, O(n⁴) checked integer operations with the obvious loop (O(n³) with a stored product). The determinant and the inverse come from fraction-free Gauss–Jordan elimination of [M | I] (Bareiss's method): with a row swap where a pivot is zero, each step replaces a_ij by (a_kk a_ij − a_ik a_kj) / p for the previous pivot p, a division that is exact because every intermediate is a minor of [M | I] (a standard property of the method, recalled and not re-read; the prototype's cases of section 6, one with a row swap, agree). It ends at [d I | d M⁻¹] with d = ± det M, whatever rows were swapped, so the inverse is exact without tracking the swaps' sign. Then P⁻¹ = M⁻¹ Δ has row i equal to (d M⁻¹)_ij Δ_j over d, held with the sign of d moved into the row and reduced by the row's gcd. It costs O(n³) checked operations once per tag, whatever the layouts. Part C may verify the elimination in the same pass, M · (d M⁻¹) = d I, as an internal invariant.

### 4.4 Why elimination and not the fold for the inverse

The fold can give the adjugate (the images of the n blades of grade n − 1 are the cofactors) and the determinant (the image of the antiscalar), so the inverse would use one code path. It was rejected because its cost does not follow use. A tag is checked once per translation unit whatever layouts the program maps, and for a dense P the image of the antiscalar alone costs about Σ_m n · C(n, m) = n 2ⁿ insertions into lists of up to C(n, ⌊n/2⌋) terms: for n = 10, about 10⁴ insertions into lists of up to 252 terms, a few million steps, inferred from the insertion cost and not measured, which is past Clang's default limit of 1,048,576. A program that maps only vectors through such a change would fail to compile. Elimination is about 2n³ = 2,000 operations at n = 10. It is per-basis data, the inverse matrix, computed once and then composed with the same fold; it generates no term (C4). Open item OI5 asks the owner whether that reading holds or whether it needs a decision of its own.

### 4.5 What the direct minor expansion would be

The alternative R4 names, a direct expansion of each minor det M[A, B] by its own determinant, generates the same terms with a second code path and visits every pair (A, B) of one grade, C(n, k)² of them, where the fold visits only the B that are reached. It was not chosen. If part C finds the fold costlier, R4 asks for the C3 measurement and a decision.

## 5. The usability checklist, applied to this proposal

Applied by me to my own proposal, so it is not independent; the blind review applies it again (plan 0030 section 11).

**Task first.** The three tasks, written from the examples before the declarations: (1) take a conformal(3) point to R(4, 1, 0) coordinates and back; (2) compute a product in the diagonal basis and bring the result back; (3) define a change for a form of one's own, such as the scaled Witt form. Writing them, I had to look up two things in the declarations: which way the rows go (row i is the image of e_i, not column i), and why the round trip needs `truncate` (the layout grows; section 3.1's comment now says so in the header).

**Names**

- [x] Field terms: "change of basis", "source", "target", "rows", "denominators", "conformal". No abbreviation.
- [x] One meaning across the library: `change_basis` and `basis_change` clash with nothing in the standard library. **Finding (minor):** `basis_change` (the value) and `change_basis` (the map) differ only in word order. A user who writes `basis_change<C>(x)` gets the compiler's own error, one error on each compiler ("type/value mismatch at argument 1 in template parameter list for ‘template<long unsigned int N> struct galoot::core::basis_change’" on GCC, "template argument for non-type template parameter must be an expression" on Clang, prototype case 11), which names `basis_change` but not the fix. Not silent; the alternatives are in section 7.
- [x] Similar things look alike: `from_rows` beside `from_gram`, `conformal_change<D>()` beside `conformal<D>()`, `change_basis_layout` beside the other `_layout` names, the tag beside the algebra tag, and the template argument first as in `truncate<Type>(x)` and `get<"e1">(x)`.

**Construction and conversion**

- [x] The shortest way is a correct way: `from_rows` with the wrong number of rows, entries, or denominators does not match. A narrowing entry (a `double` in braces) is the language's own error.
- [x] A default-constructed `basis_change<N>` is all zero; it fails B3 when used, so it cannot pass silently.
- [x] No conversion loses information: both maps are exact for an exact scalar, and the result has its own layout. Reading under a named type is the existing explicit conversion or `truncate`, which keep their checks.
- [x] No hidden default: the denominators are always written (no overload defaults them to 1); the direction is in the function's name; there is no scalar default.

**Misuse and diagnostics**

- [ ] Each misuse has a compile-fail test on both compilers: owed by part B (section 3.5 lists them).
- [x] One error each, message first: shown by the prototype for U1, U2, U2′, U4, B1, B4, B5 on both compilers (section 6). **Not met by design for B6**, a `GALOOT_ASSERT` in a constant evaluation, as for every overflow in the core today; and not met by the rejected alternative of checking in `from_rows` (departure 2).
- [ ] The message starts with the operation as written: met by U1 to U4; **not met by B1 to B6, G1, G2**, which name the value type (`basis_change:`) or the generated map, because they run once per tag (departure 3, OI3).
- [x] Context lines show the user's terms: the instantiation context names the tag (`cga3_to_r410`) and the algebra tags, and Clang shows the failed requirement (`'isometry'`). B1 and B2 must compare the counts themselves, `C::source::value.generator_count == C::value.generator_count`, not two named locals: then GCC adds "the comparison reduces to ‘(4 == 5)’" and Clang names `rga3::value.generator_count` and "expression evaluates to '4 == 5'", while the comparison of named locals printed no value on either (prototype case 8, both forms tried).
- [x] Distinct causes give distinct messages, with one deliberate merge: U4 covers an integer scalar and a scalar without `/`, which have the same fix (OI4). U2 covers an operand in the target and one in a third algebra, with the fix for each in one message.
- [x] Concepts test data: `basis_change_like` reads members and one entry, never a call. The catch-alls make a requires-expression say yes for a rejected call (decision 0033's form P); each `///` comment says what to ask instead.

**Semantics and cost**

- [x] What is assumed is stated: the comment says the change is an isomorphism and that the bulk, the weight, and the complements are not preserved, with the example; nothing is dropped.
- [x] `auto r = change_basis<C>(x);` is safe: by value, no proxy.
- [x] Exact comparisons are `==`; there is no tolerance in the interface.
- [x] The comment states the result's blades and that the terms and checks run at compile time, once per tag and layout. The measured limits are owed by part C.

**Examples and readability**

- [ ] Every example compiles: section 3.1's examples compiled in the prototype (with `use.cpp`'s names); part C makes the header's example a test, as the other headers' examples are.
- [x] A maintainer finds each rule in one place: the tag's rules in the per-tag check, the scalar rules in the two functions, the term generation in one function.

**Before release.** The interface choices that would break users later are: the names (OI1); the order of the arguments of `from_rows`; the meaning of a row (the image of e_i, so the matrix acts on row vectors of coordinates, x′ = x P); and the tag's member names. Each is cheap to change now and costly after M5 ships.

## 6. The prototype (not committed)

To check the spelling, the overload set, and the single-error behavior on both compilers, I wrote a prototype of sections 3 and 4, a header of 568 lines and three programs of 248, in namespace `galoot::core` over the core at `a6307ba`, compiled with GCC 16.2.1 and Clang 23.1.4 at `-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -D_GLIBCXX_ASSERTIONS -O0`. It lives in my scratch directory only. It is evidence for the design choices, not for the library (V10: not reproducible from the repository).

| Program | Result on both compilers |
| --- | --- |
| `use.cpp`: conformal(3) hand values e4 ↦ −½e′4 + ½e′5, e5 ↦ e′4 + e′5, e45 ↦ −e′45, 𝟙 ↦ −𝟙′, 1 ↦ 1 (as `static_assert`s); the round trip and the homomorphism of ⟑ on full<5> over `double` with dyadic values; the Witt change (cc† + c†c on 1 is 2 in the scaled form; the round trip of c†); a change with det P = 2 (e12 ↦ 2 e′12, and back e′1 ↦ ½ e1) | builds, all pass; 1.5 s and 1.4 s to build |
| `dense.cpp`: a dense 4 × 4 change with a zero leading pivot and det M = 2, against R(4, 0, 0), over `test_support::rational`: the round trip and the homomorphism on full<4>, and 𝟙 ↦ 2𝟙′ | builds, all pass |
| `misuse.cpp`, ten cases (case 8 in two forms of B1's condition) | U1, U2, U2′, U4, B1, B4, B5: one error each, the literal first; a `float` on the origin's layout is accepted; a `from_rows` that checks with `GALOOT_ASSERT` gave one error on GCC and two on Clang with the message only in a note, as `from_gram` does today (the reason for departure 2) |
| case 11, `basis_change<C>(x)` | one error each, the compiler's own (section 5) |

## 7. Alternatives rejected

| Alternative | Why rejected |
| --- | --- |
| The change as a value template argument, `change_basis<conformal_change<3>()>(x)`, or a class template `change<cga3, r410, P>` | the symbols and diagnostics would spell the whole matrix, the checks would run per value rather than per named tag, and the algebras would have to be passed separately; a tag gives a name, as S7 asks for algebras (O3) |
| A function taking the matrix directly, `change_basis(x, P)` | as above, and the matrix would have to be a constant anyway (O3) |
| A user-supplied inverse that the core verifies | every user inverts P by hand; ruled out by O3 |
| A fraction literal for input only, as `from_entries({{frac{-1, 2}, frac{1, 2}}, …})` or `std::ratio` entries | a second public type and a second spelling with no present user that the integer rows do not serve; the user still writes a matrix, and the library would reduce it to the same rows. Plan 0030 risk 2 asked this record to weigh it; the checklist found no task that needs it. A user with rows such as (½, ⅓) writes (3, 2) over 6 |
| A `from_rows(rows)` overload with every denominator 1 | one more spelling of one thing, and a default (C8); the permutations and integer changes that would use it write `{1, …, 1}` |
| `from_rows` checking its denominators and independence itself | departure 2: two errors on Clang with the message in a note, and each rule in two places |
| Tag checks spelled per operation (`change_basis:` and `change_basis_back:` for each of B1 to B5) | doubles those messages and their tests, with no difference in what the user fixes (departure 3) |
| `MaxN` on `conformal_change<D, MaxN>()`, as `conformal<D, MaxN>()` has | the algebras carry their own bound, which their factories check, and the per-tag check compares the change with them; a `MaxN` here would be a second bound with no present user |
| Determinant and inverse from the wedge fold | section 4.4: its cost grows as n 2ⁿ for a dense change whatever the program maps (C6) |
| A direct minor expansion per (A, B) | section 4.5 |
| One denominator for the whole matrix | O2 ruled (a) |
| Division by multiplying with a stored reciprocal | rounds twice, and needs the reciprocal in the scalar |
| Distinct messages for an integer scalar and a scalar without `/` | the fix is the same; the merged message is true of both (OI4) |
| A target-type parameter, `change_basis<C, Type>(x)` | composes from the existing conversion, `Type{change_basis<C>(x)}`, which keeps its own checks (C12) |
| Value names `change_of_basis<N>`, `basis_map<N>`, or `change_rows<N>` | `change_of_basis` is as close to `change_basis` as `basis_change` is and longer; `basis_map` hides that it is a change of basis; `change_rows` names the representation. `basis_change` matches the header and parallels `metric` |
| Tag members `from` and `to` | read well (`using from = cga3;`) but O3 named source and target, and `from` is a common word for a function |
| Map names `to_target<C>` and `to_source<C>`, or `change_basis<inverse<C>>` | O3 ruled the names; `inverse<C>` adds a public template for one use |

## 8. Consequences

- Part B writes its tests against sections 3.2 and 3.5, after the owner confirms the names (OI1). Part C implements them and records its own choices in decision 0221.
- Part N writes the rounding rule from section 3.6.
- The misuse catalogue gains the rows of section 3.5 when part C regenerates it, listed in the merge report (decision 0205 item 2).
- Plan 0030's section 12 estimate for compile-fail tests rises to about sixteen messages (departure 4).

## 9. Open items for the owner

- **OI1. Confirm the names (decision 0219 item 3).** `basis_change<N>` with members `rows` and `denominators`; the concept `basis_change_like`; the tag members `source`, `target`, `value`; `from_rows(rows, denominators)`; `conformal_change<D>()`; `change_basis<C>(x)` and `change_basis_back<C>(y)`; `change_basis_layout<C, S>` and `change_basis_back_layout<C, S>`. The cost of a later change grows with every test and user. The one weak spot is the near-homonym of `basis_change` and `change_basis` (section 5); the alternatives are in section 7. Recommended: confirm as written.
- **OI2. Where the value checks run (departure 2).** Options: (a) once per tag at first use, with `static_assert`s, one error with the message first, each rule in one place; (b) also in `from_rows` at the tag's definition, with `GALOOT_ASSERT`, one line earlier but two errors on Clang with the message in a note, and two messages per rule. Recommended: (a).
- **OI3. The `basis_change:` prefix for tag-level messages (departure 3).** Options: (a) one prefix naming the value type, as `metric:` and `blade_set:` do; (b) one message per operation, doubling five messages and their tests. Recommended: (a).
- **OI4. One message for an integer scalar and a scalar without division (U4).** Options: (a) one message, since the fix is the same; (b) two, as the checklist's "distinct causes" reads strictly, one more test each way. Recommended: (a).
- **OI5. Elimination for the determinant and the inverse (departure 5, section 4.4).** Options: (a) read it as per-basis data composed with the fold, recorded here (C4 needs no further decision); (b) treat it as new core machinery needing its own decision with the C3 measurement against the fold. (a) keeps the checks O(n³) whatever the program maps; (b) costs a measurement of a route whose failure at n = 10 is predictable. Recommended: (a).
