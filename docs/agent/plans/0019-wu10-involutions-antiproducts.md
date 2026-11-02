# 0019 WU10: involutions, complements, antiproducts, sums and scaling

- Status: planned 2026-11-02 (planner: Claude, agent, Opus 5.5, planner role with cpp-expert and hci-expert judgement; implementers and reviewers to be named by the coordinator at branching). Not started.
- Branches, from `develop` at `19273a6` (the merge of WU9) or later, one after the other (section 1, departure 1):
  - WU10a: `feature/core-antiproducts`, worktree `.claude/worktrees/core-antiproducts`.
  - WU10b: `feature/core-linear`, worktree `.claude/worktrees/core-linear`, branched after WU10a merges.
- Decision numbers 0165 to 0169: 0165 and 0166 in WU10a, 0167 to 0169 in WU10b (section 2).
- Source of scope: plan 0001 (WU10, "Involutions, complements, antiproducts; De Morgan laws", size M, after WU9); plan 0016 section 15 "Deferred" (to WU9 and WU10) and readings 12, 17, and 21; plan 0018 section 1 items 1 and 2 and its Outcome; the WU9 blind review (`records/reviews/2026-11-02-wu9-products.md`, F5, F12, F13 and the usability checklist); decisions 0159, 0162 (items 2, 5, 6, 8, 9), 0164, 0140 and 0141.
- Goal: the public involutions (`reverse`, `antireverse`, `grade_involution`), complements (`right_complement`, `left_complement`), and antiproducts (`geometric_antiproduct`, `antiwedge`) on multivectors, with the operators `~`, `!`, `%`, and `&` of CONVENTIONS 7.1; sums, differences, and scaling by a scalar (`a + b`, `a - b`, `s * a`, `a * s`, `a / s`); the De Morgan laws and the involution and complement relations tested on multivectors; the never-checking projection beside `truncate`; and the messages that the WU9 review deferred to this unit.

## 1. Departures, ambiguities, and owner questions (C20)

### Departures

1. **Two parts, not one.** Plan 0001 lists WU10 as one unit of size M. It now holds about twice WU9's surface: seven public functions with four operators, five arithmetic operators with a new sum kernel, a projection, a concept rename, and the rewording of eleven messages. WU7 and WU8 ran as two parts for the same reason (decisions 0117, 0158). Split along the dependency, so that each review reads one part in one sitting (W2):
   - WU10a, the roadmap row proper: the antiproducts with `%` and `&`; the involutions and complements with `~` and `!`; the De Morgan, involution, and complement relations on multivectors; the product messages (F5, F13); the result-type spelling (F12).
   - WU10b, the arithmetic: sums, differences, and scaling; negation if ratified (question O1); the projection `part` if ratified (O1); `has_blade` renamed `has_name`; the ring-operations message split by cause.

   WU10b changes `mv.hpp`, which WU10a does not touch, and uses WU10a's message wording, so it branches after WU10a merges. If the owner prefers one branch, the commits of section 4 are the same, in the same order.
2. **No conjugation.** Neither the roadmap nor CONVENTIONS 2.1 names a Clifford conjugation (the reverse composed with the grade involution), so none is built (C12). A user writes `grade_involution(reverse(x))`.
3. **The plan-level De Morgan proofs are not repeated.** WU7b proved the four De Morgan laws and the involution and complement relations exhaustively on plans and maps (plan 0013, reading 3; its D4 and D6, n up to 6 and 10). WU10 tests the same laws on multivectors through the public functions, on blade pairs of the golden forms (exhaustive) and on seeded random multivectors over the signature set of plan 0001 section 5 (DoD-10's second layer). The trace row of 4.2 is already `yes`.
4. **Figures that disagree between records.** The antiproduct's growth at fixed use is +268% (GCC) and +463% (Clang) in decisions 0116 and 0140, +265% and +464% in decision 0141 (quoting note 0018), and +266% and +461% in the 2026-11-02 handoff. They come from different runs of the same measurement; section 1, question 3 quotes 0141's as the brief asks. Not traced further; the difference does not bear on the recommendation.
5. **Planning probes, not reproducible as committed (V10).** Three `-fsyntax-only` probes in the planner's scratch area, on g++ 16.2.1 and clang++ 23.1.4, back statements below. Two ran against `include/` of the WU9 worktree just before its merge, and the third (`demorgan_types.cpp`) against `develop` at `19273a6`. The headers differ between those two revisions only in lint suppressions and one inlined condition in `product.hpp`. The probes are: `scaling.cpp` (question 4), `spelling.cpp` (question 5c), and `demorgan_types.cpp` (reading R3). They were not timed with `tools/timed` (a P13 lapse, reported). The implementer repeats each as a test or a recorded probe before relying on it.

### Readings

R1. **Scope by part.** Plan 0018 section 1 item 1 put in WU10 exactly: the antiproduct, the antiwedge, `%`, `&`, `~`, `!`, scaling (`s * a`, `a * s`, `a / s`), sums, and differences. Confirmed against plan 0016 section 15, which adds "what blade set the unary operations return (reading 17)". `antireverse`, `grade_involution`, and `left_complement` are the remaining public names of plan 0016 section 15's "and the rest"; their operations exist as `UnaryOperation` enumerators since WU7b, so their public functions cost one line each and join WU10a. Bulk, weight, duals, dot, and antidot stay in WU11.

R2. **Operators without a named function.** STYLE S13 says each overloaded operator is a one-line alias of a named function. CONVENTIONS 7.1 leaves the named-function column empty for `a + b`, `a - b`, and the three scalings. Reading: those five are the arithmetic of a number type, as `std::complex` has it, and 7.1 gives them no public name, so none is invented (C12). Each operator's body is one call of a function in `detail`. S13's rule is read as covering the rows that name a function. If the owner wants public names, they need a 7.1 amendment; none is proposed.

R3. **Result layouts (plan 0016 reading 17).** One rule, stated in each `///` comment:
   - An operation that sends every blade of its input to the same blade times a sign keeps the input's layout. So `reverse`, `antireverse`, `grade_involution`, negation, and scaling return the input's type, and `~line` is a `Line`. `detail::apply` already has the target form for this (`apply<Op, Target>(x)` with Target the input's layout).
   - `a + b` and `a - b` of one type return that type.
   - Every other operation returns the canonical set of the blades it reaches: the products and antiproducts (as WU9's do), the complements, and a sum or difference of two different layouts, whose result holds the union of the two layouts' blades.

   Why: the sign maps and same-type sums hold exactly the input's blades, so returning the input's type loses nothing (C6). It also matches what users of every geometric-algebra library expect (`~R` has the type of `R`), and it lets `~~x == x` compile without a conversion. The complements reach other blades, so they follow the products: a canonical set the user converts from, `Plane{!point}`. Probe `demorgan_types.cpp`: both sides of each De Morgan law have one type for six pairs of rigid(3) and conformal(3) layouts, because both are the canonical set of the same masks. The tests still compare on `full<n>` wherever the types differ, so that they do not rely on this.

R4. **Sums of two layouts need a small kernel (C4).** A sum on the union layout is the identity maps of the two inputs onto one target. `identity_kernel` does each half. Their combination needs one new piece of straight-line code: every blade held by one operand is assigned, not added to zero, so that a −0.0 survives, as `accumulate` already does (plan 0016 reading 18), and a blade held by both is added. This is per-basis data (positions and signs) composed with no new generation, so C4 admits it without a decision of its own. Decision 0167 records it.

R5. **Floating-point.** WU10's operations round only through the scalar's own operations: one per coefficient for scaling and division, and the products' rounding as in WU9. Double values in the tests are exact (integers below 2^24, or values chosen so a rounding difference is visible, section 5). M3's "V12 double tests with forward bounds" are not planned in any unit yet; this plan leaves them out and notes the gap for the coordinator.

R6. **Name collisions.** `reverse` meets `std::reverse` (two iterators, so no ambiguity) and `std::ranges::reverse`, a function object that turns off argument-dependent lookup when a using-directive brings it into scope. A test calls `reverse(x)` unqualified in a unit that has `using namespace std;` and includes `<algorithm>`. The `std::ranges` case is documented, not worked around.

### The questions decision 0162 fixes in this plan

1. **The never-checking projection beside `truncate` (decision 0162 item 6).** Required by the ruling: `truncate` keeps its check (decision 0161), and a second, plainly named function takes the same components and never checks.

   | Name | For | Against |
   | --- | --- | --- |
   | `part<Line>(motor)` | The field's own word: "the scalar part", "the bivector part", "the grade-k part"; CONVENTIONS 3.2 and 6.3 already call a subset of components a part ("the bulk is the part without e4", "four parts"). Says that the rest is left behind on purpose, and suggests no check. Short, in no standard header | `part<Motor>(line)`, a target with more blades, reads less well (it gives the line with zeros on the rest) |
   | `project<Line>(x)` | Common in linear algebra | CONVENTIONS 4.3 already uses projection for b ∨ (a ∧ b☆), which WU13 builds as `project`. Rejected |
   | `truncate_unchecked<Line>(x)` | States the relation to `truncate` | Long; presents the deliberate case as the unsafe variant of the checked one, when a grade part is the ordinary operation |
   | `keep<Line>(x)`, `select<Line>(x)` | Plain verbs | `keep` does not say the rest goes; `select` has other meanings (POSIX `select`, ranges) |

   Recommended: **`part<Type>(x)` and `part<Blades>(x)`**, mirroring `truncate`'s two forms, with the same result as `truncate` and never a check, in any mode or constant evaluation. Its catch-all rejects a target that is not an `mv` type of x's algebra and scalar, with `truncate`'s wording ("part: the target must be an mv type of the same algebra and scalar type, not a class derived from one; part changes only which blades are held").

   Two ratified texts stand in the way, so `part` waits for the owner (question O1):
   - CONVENTIONS 4.5 says a conversion that omits blades "is named `truncate`" and "in checking mode verifies that what it drops is zero". A second name with no check needs 4.5 amended (C10).
   - Constitution C8 says "a checking mode verifies that whatever an operation discarded was zero". Read literally, that sentence forbids `part`, and also the bulk and the weight (WU11) and any grade part, which discard nonzero components by definition. Reading proposed: C8's checking covers what an operation assumes is zero, as `truncate` and `transform` do. An operation whose purpose is to discard, and that says so in its name, assumes nothing. Ruling 0162 item 6 implies this reading. The plan asks the owner to confirm it, not to amend C8.

   `truncate`'s Requires paragraph is reworded to point to `part` ("to drop coefficients that are not zero on purpose, use part"). The lossy-conversion message, which the owner ruled (decision 0162 item 8), is unchanged. None of WU10's unary operations uses either function, since the sign maps keep their layout (R3).

2. **`has_blade` and the other orientation (HCI F11 of the WU8a reading; decision 0162 item 9).** The question, restated: `has_blade<Line, "e14">` is false, though a line holds the blade e14 (as −e41). What the concept tests is narrower than its name: whether the oriented name, with the orientation the layout gives it, is one that `get` accepts. It is `get`'s constraint, and `get`'s messages already draw the distinction ("stores this blade with the other orientation"). The reading proposed two remedies: rename it to say what it tests, or keep the name and document it. The ruling took "the recommendation" without saying which; this plan reads it as the first remedy, because the name is the part a user reads, and both remedies are equally small.

   The change: rename `has_blade<X, Name>` to **`has_name<X, Name>`**, with its meaning unchanged: true exactly when `get<Name>(x)` is accepted, and false, never an error, otherwise. Its comment says that the other orientation of a held blade is false, and that the question "does the layout hold this blade in either orientation" has no concept. That question has no present caller (S16). In one listed commit: `mv.hpp` (the concept, `get`'s constraint and its two comments, "ask has_name instead"), `mv_test.cpp` and the compile-fail sources that name it, with no expected value changed. Decision 0169 supersedes the name in decision 0033 and plan 0016 reading 20. Alternatives weighed in 0169: `holds_name` (reads as well, longer), `has_coefficient` (misleads: the line does have a coefficient on e14, negated).

3. **Decision 0140, the antiproduct inside the coupling closure, raised again (decision 0162 item 5).** The numbers from decision 0141 and the records it rests on:

   | | GCC | Clang |
   | --- | --- | --- |
   | Four typed rigid(3) pairs in R(n−1,0,1), n = 4 to 16, antiproduct as built (conjugation over all n) | +265% | +464% |
   | The same through the geometric product | +6% | +4% |
   | Option B of 0140 (conjugate inside the closure), components per plan | +9.0% | +10.8% |
   | Option B, components once per algebra: the plans / the per-algebra data | +0.7% / +30.6 M instructions | +4.3% / +44.1 M |
   | The sparse sets of R(15,0,1) through the antiproduct (`plan_cost_antigp_sparse16.cpp`, the canary): operations (GCC) and steps (Clang), against 18,252 and 3,136 at every n through the product | 443,211 at n = 16 (49,347 at n = 4) | 77,513 at n = 16 (9,137 at n = 4) |
   | At n = 4, the size of rigid(3), the four pairs: antiproduct against product (decision 0116) | 308.8 M against 306.2 M | 702.5 M against 547.6 M (+28%) |

   Recommended: **build the public antiproducts on the conjugation as it stands**, and keep 0140 proposed. Reasons:
   - WU10 changes no generation. `geometric_antiproduct` and `antiwedge` are one-line callers of `detail::apply`, so 0140 can replace the generator later without changing their interface or their results. 0140's own test plan compares the closure route with the present one.
   - For the models planned (n from 3 to 6), the gain is tens of milliseconds per translation unit (0141). At n = 4 GCC's cost is the product's within 1%, and Clang's is 28% above it.
   - None of 0141's triggers fires. No production report exists. No model needs more than six generators while using few of them: rigid(3) has 4 and conformal(3) has 5. The date, 2027-11-01, has not come. WU10's own sparse test at n = 16 (section 5) is a test, not a use.

   The triggers stay as 0141 sets them, and WU10 adds nothing to them. WU10 records current figures for the review of the exception: the four rigid(3) pairs through `%` at n = 4 and n = 16 on both compilers, and the canary's counts unchanged. This is not required by 0141 item 4, since WU10 does not touch generation. It keeps the figures through the public path current. Question O2 asks the owner to confirm.

4. **The two `operator*` families.** WU9's family takes two `mv`s, `operator*(const mv<Alg, A, T>&, const mv<Alg, B, T>&)`, with a catch-all constrained to two multivectors that differ in algebra or scalar. Scaling adds:
   - `operator*(const T& s, const mv<Alg, A, T>& x)` and `operator*(const mv<Alg, A, T>& x, const T& s)`, with T deduced from both parameters, and `operator/(const mv<Alg, A, T>& x, const T& s)`, which also requires that T has `/`. A scalar of another type gives a deduction conflict, so this overload is not a candidate. An `mv` in the scalar's place deduces T as an `mv` type, which conflicts with the coefficient type, so it is not a candidate either.
   - One catch-all per spelling (`s * a`, `a * s`, `a / s`) for a scalar of another type, constrained by `multivector<X> && !multivector_or_derived<S> && ring_scalar<S> && S is not X's scalar type`.

   The three families' constraints are disjoint. Two `mv`s exclude the scalar overloads (deduction) and the scalar catch-all (`!multivector_or_derived`). A scalar of the right type excludes WU9's catch-all (`multivector<S>` is false) and the scalar catch-all (same type). So no call is ambiguous, whatever partial ordering would say. Probe `scaling.cpp` builds `p * q`, `2.0 * p`, `p * 2.0`, `3.0 * derived_line`, and `derived_line * derived_line` with no ambiguity on both compilers, and `2 * p` for a double point gives one error. Clang's text: "static assertion failed due to requirement 'std::is_same_v<int, double>': s * a: ...".

   The scalar must have the multivector's scalar type exactly. `2 * x` for a double multivector is rejected with a message that says to write 2.0. Reasons:
   - Converting to T would admit `2.5 * x` for an integer multivector, which drops digits silently (C8).
   - A rule of conversions that keep every value (int into double, float into double, but not int into float) works, but the user must know the rule to predict a result (C8's last sentences).
   - `std::complex<double>` rejects `2 * c` for the same reason of deduction, so C++ users know the pattern.
   - It matches WU9's rule for two operands of different scalar types.
   - Widening later is additive: every call that compiles now still compiles. Decision 0167 records the value-keeping rule as the alternative.

   `a / s` divides each coefficient by s with T's own division: exact for the test rational, correctly rounded per coefficient for double (never multiplication by 1/s, which rounds twice), and truncating for an integer type, which the comment states (C7: rounding comes from the chosen scalar). For an integer T, s must not be zero. Built-in division by zero is undefined, so the precondition is checked by `GALOOT_ASSERT` (S9). A T without `/` gets the compiler's own error: no present scalar lacks it, and the comment says so. `s * a` multiplies each coefficient on the left and `a * s` on the right, so that a scalar ring that does not commute keeps its order. A test-local 2×2 integer matrix scalar checks this (section 5).

5. **Messages deferred to this unit.**
   - (a) **F5: `apply:` reaches the user through a public product.** A narrow scalar in `geometric_product`, `*`, `geometric_antiproduct`, or `%` gets "apply: the scalar type cannot hold one of the product's exact coefficients; ...". Fix: each public product checks that condition itself, before it calls `apply`, in an `if constexpr` whose other branch holds the one `static_assert` with the product's spelling, so the user sees one error. Example: "a * b: the scalar type cannot hold one of the product's exact coefficients; use a scalar type that holds larger integers exactly". The condition is a named `constexpr bool`, so Clang prints the rule, not a mask. The wedge and the antiwedge have coefficients ±1 only, which every ring scalar holds, so they need no check, and their comments say so. The sum kernel's coefficients are ±1 too.
   - (b) **F13: the mixed-operand wording.** The messages begin with the operation as written (`a * b:` since WU9). The fixes change. "Both must be in the same one" restates the rule, and "convert one explicitly first" points to a conversion the library does not have:
     - mixed algebras: "a * b: the operands are in different algebras; give both types the same algebra tag". The likeliest cause is two tags declared for one metric, and this names the fix for it.
     - mixed scalars: "a * b: the operands have different scalar types; give both types the same scalar type". Reworded again when the scalar conversion of question 7 exists.

     The same pair of wordings serves the eight product spellings (four functions, four operators) and `a + b` and `a - b`. Each `static_assert` keeps WU9's condition on the two types, so the compilers print both algebras or both scalars.
   - (c) **F12: the spelling of a result type.** WU9 gave the operation enums underlying type `int`, which removed the raw byte. Clang still prints a product's result type through the declared return type of `apply`: `mv<rga3, product_plan<(galoot::core::detail::BinaryOperation)1, rga3, blade_set<4>{...}, blade_set<4>{...}>.result, double>`. GCC prints the value whatever the declaration says. The type itself is correct and stays: the canonical set by value, which WU9's tests pin. Only its sugar is at issue, and sugar can change later without breaking anyone. A small change removes the internal names from Clang's line. Each public function that returns a canonical set declares its return type as `mv<Alg, detail::wedge_layout<Alg, A, B>, T>` and calls `apply<Op, Target>`, with one variable template per result (`geometric_product_layout`, `wedge_layout`, `geometric_antiproduct_layout`, `antiwedge_layout`, `right_complement_layout`, `left_complement_layout`; `sum_layout` in WU10b). Probe `spelling.cpp`: Clang then prints `mv<rga3, wedge_layout<...>, double>`, without `product_plan`, `.result`, the enum cast, or `detail::`. Recommended in WU10a, because it changes the four products' declarations at the same time as F5's check. The masks of the operand layouts remain in the text: they come from deduction, which drops the name of a layout variable. Names written in the type stay deferred (decision 0157 item 3).
   - (d) **The ring-operations message (decision 0152; decision 0162 item 8).** WU9 gave it a fix after the semicolon ("...; use a number type such as double or an integer type"). The rest of the reviews' proposal, naming the requirement that failed, is still open. WU10b splits `ScalarFault::no_ring_operations` into the five requirements of `ring_scalar`, tested in its order, each with its own named rule and message (plan 0016 reading 21: each cause its own message):
     - "mv: the scalar type has no addition; give it a + b, or use a number type such as double"
     - "mv: the scalar type has no subtraction; give it a - b, or use a number type such as double"
     - "mv: the scalar type has no multiplication; give it a * b, or use a number type such as double"
     - "mv: the scalar type has no unary minus; give it -a, which the products use for a term of coefficient -1"
     - "mv: the scalar type cannot be made from an integer coefficient; give it a constructor from std::int64_t, or use a number type such as double"

     `ring_scalar` itself stays one concept, and generic code tests it as before. The five checks are its five clauses, so no sixth rule can drift from it. A static_assert in the test pins each clause against a scalar that lacks it.
   - All new messages follow plan 0016 reading 21: one error, the operation as written, the mistake in the algebra's terms, the fix after a semicolon, no final period or quotes. Each rejecting overload returns a value of the first operand's type, or of the target, so that `auto r = ...;` adds no second error (WU9 F1).

6. **Headers (decision 0164 leaves this to the plan).** Recommended: three headers, all included by `core.hpp`.
   - `product.hpp` gains the antiproduct and antiwedge with `%` and `&`. They share the products' operand checks, catch-alls, and messages, so a reader finds all four products in one file.
   - A new `unary.hpp` holds the involutions and complements with `~` and `!`. They need no operand checks and include only `mv.hpp` and `plan.hpp`. WU11 decides whether bulk, weight, and duals join it.
   - A new `linear.hpp` holds sums, differences, scaling, and negation: the operations of the vector space, with the sum kernel.
   - The operand checks that `product.hpp` and `linear.hpp` share (`same_algebra_v`, `same_scalar_v`, and the two concepts) move to `mv.hpp`'s `detail`, beside `multivector_of`, which tests the same members. That way neither new header includes the other. The move is a mechanical commit of WU10b.

   Alternatives weighed in decision 0165: everything in `product.hpp` (one file, but `+` and `~` in a file named for products); sums and scaling in `mv.hpp` (beside the type, but `mv.hpp` is the largest header and every edit to it rebuilds every multivector test, P8). The new headers add parse cost to every includer of `core.hpp`, which is measured (section 2).

7. **Scalar conversion is not in WU10 (decision 0162 item 2).** It goes to a small unit of its own, planned after WU10b merges and before WU16's model aliases. It must:
   - choose a name consistent with `truncate` and `part`, since a conversion from double to float is lossy in digits as `truncate` is in components;
   - change the assertion `!std::is_constructible_v<Line<double>, Line<float>>` in `mv_test.cpp` by a recorded decision (W9);
   - reword the mixed-scalar messages of 5(b) to name it.

   Nothing in WU10 needs it. Each mixed-scalar message tells the user to give both types one scalar type.

### Owner questions (section 8)

- O1. Ratify CONVENTIONS version 1.8: 4.5 names `part` beside `truncate`; 7.1 adds negation `-a`; and confirm the reading of C8 in question 1. WU10a does not wait; WU10b's two commits that need it wait (W8).
- O2. Decision 0140: keep it proposed and build the antiproducts on the present conjugation (recommended).

## 2. Definition of done

WU10a:
- `include/galoot/core/product.hpp`: `geometric_antiproduct(x, y)`, `antiwedge(x, y)`, `x % y`, and `x & y`, each a short caller of `detail::apply`, with `///` comments that say what each computes (the definitions of CONVENTIONS 4.2, restated), its result layout, that the antiwedge does not depend on the metric, the cost a user can predict (one plan per pair of layouts, and the antiproduct's growth with n for low-grade blades in a large algebra), and that `&` binds below `==`. Rejecting overloads for mixed algebras and scalars in the four new spellings. The F5 check in the two geometric products, the F13 wording in all eight spellings, and the F12 return types in all four products.
- `include/galoot/core/unary.hpp`, new: `reverse`, `antireverse`, `grade_involution`, `right_complement`, `left_complement`, `~x`, and `!x`, each a one-line caller of `detail::apply`, the sign maps on the input's layout and the complements on their canonical result (R3). Comments say: the sign of each map in terms of grade and antigrade (2.1); the complement's defining relation (3.1) and that `!!x` is ±x by grade, not x; that `!x` is a multivector, so `if (!x)` does not compile; that `std::ranges::reverse` under a using-directive hides `reverse` (R6). `core.hpp` includes it.
- Tests (doctest-expert role), in `test/core/product_test.cpp` and a new `test/core/unary_test.cpp` (section 5). Compile-fail tests for the eight new rejections, for F5 in two spellings, and for each reworded WU9 message.
- Trace table: rows for 2.1 (the antireverse and the grade involution), 3.1 (left of right is the identity), and 7.1 (`a ^ b`, `a % b`, `a & b`, `~a`, `!a`), each set to `yes` with a test that states its fact.
- Decisions 0165 (split, headers, result layouts) and 0166 (product messages and result spelling).
- Records: mutants (`records/mutations/0019-antiproducts/`), the diagnostics table (each new message, first-line length, line count, both compilers), W10 before and after for the affected suites, P2 for the new includer of `core.hpp` (the smoke test, with a usable instruction count; WU9's failed), the figures of question 3. The plan's outcome for WU10a.

WU10b:
- Mechanical commits: the operand checks move to `mv.hpp` (no behavior changes); `has_blade` becomes `has_name` (question 2).
- `include/galoot/core/linear.hpp`, new: `a + b`, `a - b` (same algebra and scalar; result per R3), `s * a`, `a * s`, `a / s` (question 4), and `-a` if O1 ratifies it, with their rejecting overloads and comments: the layout rule, the order of multiplication, division by T's own `/` with the integer case and its zero precondition, and that a requires-expression asking whether `2 * x` is valid answers yes. `core.hpp` includes it.
- `mv.hpp`: `part<Type>(x)` and `part<Blades>(x)` with their catch-all, if O1 ratifies 4.5; `truncate`'s Requires paragraph reworded; the ring-operations fault split (5d).
- Tests (doctest-expert role) in a new `test/core/linear_test.cpp`, with `part` in `mv_test.cpp` beside `truncate`, and compile-fail tests for every new rejection.
- Trace table: 7.1 rows for the sum and difference and the scaling; 4.5 and 7.1 rows for `part` and negation once ratified.
- Decisions 0167 (scaling, sums, the operator families, negation), 0168 (`part` and the C8 reading, with the owner's ruling on O1), and 0169 (`has_name`, the ring-operations messages).
- Records as for WU10a (`records/mutations/0019-linear/`); the plan's outcome closed.

Both parts: W5 blind review with the usability checklist (each part changes a public interface), requested by the coordinator; merge by the coordinator after P9's checks. The independent audit that decision 0163 item 2 placed after WU10 follows WU10b.

## 3. Acceptance criteria

1. Values against independent references.
   - Every public involution and complement of every blade of `full<n>`, n = 3, 4, 5, equals the golden `grade_signs` and `complements`.
   - Every antiproduct and antiwedge of a pair of blades of the four golden forms equals the composition from golden values.
   - The randomized identities of section 5 hold on every form of the signature set, by exact equality.
2. Layouts. The sign maps, negation, scaling, and same-type sums return the input's type, by `static_assert` on `Line`, `Motor`, and a conformal(3) layout. The complements, products, antiproducts, and two-layout sums return the canonical set of the reached blades, by `static_assert` against an expected layout written in the test from the definition (not from `detail`).
3. Operators. Each of `%`, `&`, `~`, `!` returns the type and value of its named function.
4. Diagnostics. Each rejection in the units' catalogue gives one error, also in the form `auto r = ...;`, and the message is on the first error line on GCC and Clang, pinned by `expect_output` compile-fail tests. No message begins with `apply:` for a public spelling. Clang's first line for `(p ^ q) == line` contains no `product_plan`, `detail::`, or enum cast. Sizes are recorded beside the WU9 table.
5. Detection. Each catch-all has an `is_invocable` or requires-expression probe in the tests, showing the documented "yes" for a rejected call. Each also has a probe showing that it captures no valid call and no other type's operator, in a scope where `galoot::core`'s operators are visible (WU9 F8a).
6. Mutants. Each mutant of section 6 is killed on gcc-debug and clang-debug by a test, never by a warning; an equivalent survivor is recorded with its reason.
7. Checks. P8 and P9: only the tests a change can affect, widened at each merge. Every build and test is timed with `tools/timed`. All seven presets pass from new, empty trees at the final revision of the build inputs. The trace gate passes. Builds draw from the shared job pool, whose status is read before and after each heavy run.
8. No citation of project records in code, comments, tests, or build files (STYLE S11).
9. Only after O1: `part` never checks, also on an exact scalar and in a constant evaluation, while `truncate` on the same input still fails; and `-a` returns the input's type.

## 4. Approach

Tests before the code they judge where they can be, each such commit saying it fails (W2); expected-value changes to existing tests in their own listed commits (W9). Small commits, each building and passing apart from those.

WU10a, on `feature/core-antiproducts`:

0. Claim: this plan committed to `develop`. Branch and worktree as above.
1. Test commit: the reworded expectations of WU9's eight compile-fail tests (F13), listed for review; fails until commit 2.
2. The F13 wording in `product.hpp`.
3. Test commit: compile-fail tests for F5 in `a * b` and `geometric_product(a, b)` with a narrow scalar (WU9 review's probe: `std::int8_t` under diag(100, 100)); fails until commit 4.
4. F5 and F12 in `product.hpp`: the coefficient check in the public geometric product, and the four products' declared return types through the per-operation layout variable templates. A test asserts that `decltype(p ^ q)` is unchanged.
5. Test commit: the antiproduct tests of section 5 (golden composition, operators, associativity, 𝟙 as identity, compile-fail for the eight new rejections).
6. `geometric_antiproduct`, `antiwedge`, `%`, `&`, with their catch-alls, in `product.hpp`.
7. Test commit: `unary_test.cpp` (golden signs and complements, layouts, relations, the unqualified-call test of R6).
8. `unary.hpp` and its line in `core.hpp`; CTest registration.
9. Test commit: the De Morgan laws and the involution product rules on multivectors (section 5, identities), over the signature set.
10. Trace rows of section 2, with the trace gate run.
11. Records: mutants, diagnostics, W10, P2, question 3's figures, decisions 0165 and 0166, the WU10a outcome.

WU10b, on `feature/core-linear`, after WU10a merges:

0. The claim line for WU10b in this plan, on `develop`.
1. The operand checks move from `product.hpp` to `mv.hpp`'s `detail` (mechanical; product tests unchanged).
2. `has_blade` becomes `has_name`, every use migrated, no expected value changed (listed).
3. Test commit, then code: the ring-operations fault split (5d).
4. Test commit, then code: sums and differences in `linear.hpp`, with the sum kernel and `sum_layout`.
5. Test commit, then code: scaling and division with their catch-alls.
6. After O1 only: negation (test, then code).
7. After O1 only: `part` (test, then code), and `truncate`'s comment reworded.
8. Trace rows; records; decisions 0167 to 0169; the outcome. If O1 is still open here, the unit closes without commits 6 and 7, which are recorded as waiting, and a later small unit adds them (W8).

## 5. Evidence

Independent oracles (V1 kind 3) are the golden header, generated by `test/reference/golden.py` by routes that share no code with the library. Identities (kind 2) are checked exactly, on seeded random multivectors over the integers or the test rational. Plan-level proofs (kind 1) exist from WU7b and are not repeated (departure 3).

| Operation | Golden values that exist (`test/reference/golden.hpp`) | How the test uses them |
| --- | --- | --- |
| `reverse`, `antireverse`, `grade_involution` | `grade_signs::n3`, `n4`, `n5` (fact 2.1) | Each blade of `full<n>` as a one-blade multivector, on its own layout and on a rigid(3) oriented layout (`oriented_names::rigid_3`, fact 1.6), so the orientation's sign is in play |
| `right_complement`, `left_complement` | `complements::n3`, `n4`, `n5` (fact 3.1); `result_order::n4`, `n5` (fact 1.7) for the canonical result | As above; n = 3 and 5 are odd (left and right agree) and n = 4 is even (they differ), so a swap is caught |
| `geometric_antiproduct` | `gp_tables` for rigid(3), conformal(3), the degenerate 3-form, and random form 1, with `complements` | For every pair of blades: left(golden gp(right e_A, right e_B)), composed in the test from golden values alone, as WU7b's D5 did at plan level |
| `antiwedge` | the same tables; the wedge as their top-grade part, which WU9 already uses | As above, with the test's own wedge |
| 𝟙 as the identity of ⟇ (validation for rigid(3), a statement Lengyel makes, read in note 0005) | `units::antiscalars` (fact 1.5) | `antiscalar % x == x` and `x % antiscalar == x` on random multivectors |
| Sums, differences, scaling, negation, `part` | none needed | Hand values over the integers, stated in the test; vector-space identities |

`golden.py` needs no new values. One addition was weighed and rejected: tables of the antiproducts themselves. The test composes them from golden values, which is the same independence at no cost to the generator (C12). `conventions_check.py` is unchanged: it already checks the De Morgan laws and the antiproduct's associativity for rigid(3), which is where 4.2's Checked label comes from.

Identities on multivectors (DoD-10's second layer), each on seeded random integer or rational multivectors with the seed printed (V7). Coverage: every form of plan 0001 section 5 with n ≤ 5 on `full<n>`; n = 6 on grade subsets, where Clang's limit on `full<6>` products applies; sparse sets at n = 8, 12, and 16:
- the four De Morgan laws, and their converses right(a ⟇ b) = right(a) ⟑ right(b) and right(a ∨ b) = right(a) ∧ right(b), with left likewise;
- associativity of `%` and `&`;
- `~~x == x`, `grade_involution(grade_involution(x)) == x`, and `antireverse(antireverse(x)) == x`; `left_complement(right_complement(x)) == x` and the reverse order;
- `right_complement(right_complement(x))` equal to x times (−1)^(gr·ag) grade by grade;
- `antireverse(x) == left_complement(reverse(right_complement(x)))`;
- the product rules: reverse(a ⟑ b) = reverse(b) ⟑ reverse(a), reverse(a ∧ b) = reverse(b) ∧ reverse(a), antireverse(a ⟇ b) = antireverse(b) ⟇ antireverse(a), and grade_involution(a ⟑ b) = grade_involution(a) ⟑ grade_involution(b).

The n = 16 cases compile the antiproduct of the 0141 exception. If one makes the edit-and-test cycle slow, it carries the `slow` label (P9) and is never omitted.

WU10b's own tests:
- vector-space laws: a + b = b + a, (a + b) + c = a + (b + c), a − b = a + (−1)·b, s(a + b) = sa + sb, (st)a = s(ta);
- distributivity of each product over sums;
- `(x * s) / s == x` over the rational;
- hand values for two-layout sums on oriented layouts (a rigid(3) line plus the antiscalar gives a motor-shaped canonical result, with each sign from the oriented names);
- −0.0 kept on a blade only one operand holds, over double;
- `3.0 / 10.0`, which is not `3.0 * 0.1` in binary64, against multiplication by the reciprocal;
- the left and right order of scaling, with a test-local 2×2 integer matrix scalar, which also checks that products keep the order x_a y_b;
- the integer zero-divisor precondition, aborting in checking mode under `test/expect_output.cmake`, as `truncate`'s run-time check is tested;
- `part` against `truncate` on inputs whose dropped part is zero (equal) and nonzero (`part` succeeds where `truncate` fails a constant evaluation).

V10, to fill in at each outcome. Verification by the above. Validation: for rigid(3), 𝟙 as the identity of ⟇; for non-diagonal antiproducts, not performed, as the owner accepted at M2 (decision 0163 item 3).

## 6. Mutants

WU10a, each on gcc-debug and clang-debug:
- `reverse` calls the antireverse map;
- `grade_involution` calls the reverse map;
- `!` calls `left_complement` (killed only in even n);
- a sign map returns the canonical layout instead of the input's;
- `%` calls `geometric_product`;
- `antiwedge` uses the geometric-antiproduct plan;
- `&` calls `wedge`;
- the operands of `geometric_antiproduct` are swapped;
- the F5 check is dropped (`apply:` returns);
- the algebra and scalar constraints are dropped from the new catch-alls;
- one sign of `left_complement_sign` is flipped in `bits.hpp`.

WU10b:
- the sum kernel adds its first term to zero (the −0.0 test);
- a difference takes y with +1;
- a two-layout sum drops the blades only y holds;
- `s * a` computes x_i * s (the matrix scalar);
- `/` multiplies by the reciprocal (`3.0 / 10.0`);
- the integer zero assertion is removed;
- the scalar catch-all's constraint is widened to every type;
- `part` calls `truncate`;
- one ring-operations fault reports the next clause's message.

## 7. Risks

1. **`!` and `~` on a class type.** `!x` is a multivector, so `if (!x)` does not compile, which is the safe failure. `!!x` is ±x, not x, and the comment says so.
2. **Precedence.** `%` binds as `*`, left to right; `&` and `^` bind below `==`. CONVENTIONS 7.1 requires parentheses for `^` and `&`, and the tests also parenthesize every mix of `*` and `%`.
3. **The scalar catch-all is broad.** A requires-expression for `int * Point` answers yes, as WU9's catch-alls do for mixed operands. Constraining it to `ring_scalar` keeps it from capturing other types' operators; criterion 5 probes this.
4. **Antiproduct build cost at large n.** The 0141 exception applies. The tests at n = 16 pay it, and users of models with n ≤ 6 see the figures of question 3.
5. **Compile cost of the new tests.** `mv_test.cpp` is the largest unit (64.5 G instructions on GCC). The identity tests over the signature set may approach it. If `product_test.cpp` passes `mv_test.cpp`'s cost, the identities move to their own unit under the same label, by the implementer's measurement.
6. **The layout rule is a rule.** R3 is stated in every comment. The hci reading of the review checks that a first-time user predicts `~line`, `!point`, and `line + antiscalar` from the comments alone.
7. **Owner-dependent commits.** `part` and negation wait for O1. If O1 is answered against them, WU10b closes without them and the trace gains no rows for them.
8. **Classes derived from `mv`.** Valid calls work by deduction and mismatched ones get the compiler's error (WU9 F6), until WU16.
9. **Shared files with other lanes.** `test/CMakeLists.txt` blocks and `runs.tsv` rows: whichever unit merges second merges `develop` in first (W7).

## 8. Open questions for the owner

- **O1. CONVENTIONS version 1.8, and a reading of C8.**
  - 4.5, after "It is never implicit.": "`part<Blades>(x)` and `part<Type>(x)` keep the same components and never check: what they drop is meant to be nonzero, as in a grade part."
  - 7.1, a row: "`-a` | Negation | " (no named function, like `+` and `-`).
  - Confirm that C8's "a checking mode verifies that whatever an operation discarded was zero" covers what an operation assumes is zero, not an operation whose purpose is to discard and whose name says so (question 1).

  Recommended: yes to all three. Without them, `part` and `-x` are not built, and `-x` stays the compiler's error.
- **O2. Decision 0140.** Keep it proposed, and build WU10's antiproducts on the present conjugation, with 0141's triggers unchanged (question 3). Recommended: yes.

For the coordinator, not the owner: M3's "V12 double tests with forward bounds" have no unit yet (R5).

## 9. Agent cost estimate

Recorded in `docs/agent/records/agent-cost.tsv` as each agent finishes. Figures for comparison: WU7b's implementer used 674k tokens and WU8b's 865k; their blind reviews used 280k to 360k.

| Part | Role | Model | Estimate |
| --- | --- | --- | --- |
| WU10a | implementer (cpp-expert role) | Opus, high | 550k to 700k tokens |
| WU10a | test author (doctest-expert) | Opus, high | 250k to 350k |
| WU10a | blind reviewer (W5, with the usability checklist) | Opus, high | 300k to 380k |
| WU10a | delta reviewer, if the review asks for fixes | Opus, high | 200k to 270k |
| WU10b | implementer (cpp-expert role) | Opus, high | 450k to 600k |
| WU10b | test author (doctest-expert) | Opus, high | 200k to 300k |
| WU10b | blind reviewer (W5, with the usability checklist) | Opus, high | 250k to 330k |
| WU10b | delta reviewer, if needed | Opus, high | 180k to 250k |

Size M overall, as plan 0001 has it: about 2.2M to 3.2M tokens in all. The mechanical commits (WU10b 1 and 2) are small enough for Sonnet if the coordinator prefers.

## 10. Outcome

Not started.
