# 0021 WU13: compose, transform, antiproject, and the rounding check of the transform

- Status: planned 2026-11-04 (planner: Claude, agent, Opus 5.5, planner role with math-expert, cpp-expert, hci-expert, and numerical-expert judgement; implementers and reviewers to be named by the coordinator at branching). Claimed by this commit. The owner answered O1 to O8 and departure 7 on 2026-11-07, "proceed with recommendations", with three changes to the 1.10 text (decision 0192); M3 is accepted (decision 0182), so WU13a may branch. WU13a implemented on `feature/core-transform`, 2026-11-07, ready for its blind review (section 11); WU13b not started.
- Branches, from `develop` at `535c91e` or later, after the M3 gate's acceptance (O1), named by W7 as amended (decision 0188):
  - WU13a: `feature/core-transform`, worktree `.claude/worktrees/core-transform`. The test author works on `feature/core-transform--tests`, worktree `.claude/worktrees/core-transform--tests`, branched from `feature/core-transform` at its first commit. The coordinator also created `feature/core-transform--math`, worktree `.claude/worktrees/core-transform--math`, for step 1.
  - WU13b: `feature/core-transform--rounding`, worktree `.claude/worktrees/core-transform--rounding`, branched from `feature/core-transform` after WU13a's review is resolved, and merged back into it. Its test author works on `feature/core-transform--rounding--tests`, worktree `.claude/worktrees/core-transform--rounding--tests`. Only `feature/core-transform` merges into `develop`, after both parts are reviewed (departure 4, O8).
- Decision numbers 0176 to 0180: 0176 to 0179 in WU13a, 0180 in WU13b (section 2). The owner's rulings on this plan are decision 0192, so 0176 records the 1.10 text as committed.
- Source of scope: plan 0001 (WU13, "`transform.hpp`: compose, transform, project, antiproject (exact rotor 3/5, 4/5 for V11)", size M, after WU11; M4, "zero core diff; 1.6 and 5.1 to 5.7 pinned; flector distinguishing test; rigid V8 cases"; DoD-19 "odd+even versor in `transform`"; DoD-20; DoD-24; D9; D10; risk 7); CONVENTIONS 1.9, items 2.1, 4.3, 4.4, 4.5, 5.4 to 5.6, 7.1, and section 8 ("Flector transform"); plan 0020 and its Outcome (`project` already exists in `dual.hpp`, decision 0174, and is not replanned); decisions 0009, 0010, 0159 to 0175; the post-WU10 audit (`records/gates/2026-11-02-post-wu10-audit.md`); the third handoff of 2026-11-02; research notes 0005 and 0020; the design study, "Bulk, weight, duals and norms" to "Lengyel versus Hestenes-style conventions".
- Goal: `compose(Q2, Q1)` and `transform(x, V)` of CONVENTIONS 4.4 in a new header `transform.hpp`, with the parity rule of decision 0009 rejecting a mixed versor at compile time and the checking mode of 4.4 and 4.5: an exact check for exact scalars in WU13a, and in WU13b the rounding-scalar check of decision 0010 that decision 0175 assigns to `transform`, verified against WU11b's tracking oracle; `antiproject(a, b)` beside `project`; a CONVENTIONS amendment (version 1.10) that defines the antiprojection in 4.3 and repairs 4.4 outside four generators; the flector distinguishing test and the exact rotor of V11.

## 1. Departures, ambiguities, and owner questions (C20)

### Departures and ambiguities

1. **WU13 belongs to M4, and the M3 gate is not yet submitted.** W11 says a module waits for the gates of the modules it depends on. WU9 to WU11 (M3) are merged, but no M3 gate report exists, and the post-WU10 audit (finding M2) reported the same pattern when WU9 ran ahead of M2. This plan is committed now because the brief asks for the claim and because a plan is not implementation. Reading followed: nothing of section 4 after commit 0 starts until the owner accepts M3 or says WU13 may start before it (O1).

2. **CONVENTIONS 4.4 is ambiguous for n = 4 and wrong for odd n.** The ratified text reads "an even V gives V ⟇ x ⟇ Ṽ, as in 5.4, and an odd V gives −V ⟇ x̂ ⟇ Ṽ, as in 5.6", and "by the parity of V's blades". Three problems, each found by the planning probes of departure 6:
   - **Ṽ.** The character is U+1E7C, V with a tilde above, which the design study uses for the reverse. Sections 5.4 and 5.6, which 4.4 cites, write Q̰ and F̰ (U+0330, tilde below), the antireverse. On a blade of grade k the reverse is the antireverse times (−1)^((n−1)(n−2k)/2): equal on every blade when n ≡ 1 (mod 4), opposite on every blade when n ≡ 3 (mod 4), and opposite on the odd grades when n = 4. Read as the reverse, 4.4 gives the negation of 5.6 for a flector in rigid(3) (a reflected point gets weight −1) and fails to be an outermorphism for an even versor in rigid(2) and R(3,0,0). Reading followed: Ṽ means the antireverse, as 5.4 and 5.6 have it.
   - **Parity.** "The parity of V's blades" does not say grade or antigrade. They agree when n is even and are opposite when n is odd. The transform lives in the antiproduct algebra, which the right complement maps from the geometric-product algebra grade for antigrade, so the parity that makes the sandwich an automorphism is the antigrade's. In conformal(3) (n = 5) a motor has odd grades and even antigrades, and in rigid(2) (n = 3) Lengyel's motor r_x e1 + r_y e2 + r_z e3 + r_w 𝟙 does too: by grade, every such motor would be rejected as odd or given the odd formula. Reading followed: parity of the antigrade.
   - **The odd sign.** For an odd V, the form that is the outermorphism of V's action on vectors and that fixes the vectors of a mirror is −V ⟇ x̌ ⟇ V̰, with x̌ the antigrade involution ((−1)^ag on a blade of antigrade ag). When n is even x̌ = x̂, the grade involution, which gives 5.6. When n is odd x̌ = −x̂, so the form is +V ⟇ x̂ ⟇ V̰, and 4.4's −V ⟇ x̂ ⟇ Ṽ gives the negated vector map (a reflected point of rigid(2) gets weight −1). Reading: this needs the owner, since it changes ratified text.

   Proposed text for version 1.10 is in O2. Implementation waits for the ruling: the n = 4 cases agree under every reading except "Ṽ is the reverse", so WU13a's rigid(3) tests do not depend on it, but the odd-n tests and the code's sign rule do.

3. **`antiproject` goes in `dual.hpp`, not `transform.hpp`.** Plan 0001's WU13 row names `transform.hpp` for compose, transform, project, and antiproject. `project` went to `dual.hpp` (decision 0174), and `antiproject` is its mirror: the same parts (`weight_dual`, `wedge`, `antiwedge`), the same rejecting function, the same message form. Putting it in `transform.hpp` would split the pair and make `transform.hpp` include `dual.hpp` for one function. Recommended: `dual.hpp` (O3).

4. **Two parts, one merge.** Plan 0001 lists WU13 as one unit of size M. It now holds three public functions with two layout names, eleven compile-time messages and two run-time ones, two restricted kernels, the parity rule, the exact check, and the rounding-scalar check of decision 0010, which puts forward-error-bound logic into the core for the first time (plan 0001 risk 7). The rounding check needs its own derivation and review (numerical-expert). So:
   - WU13a: CONVENTIONS 1.10, `antiproject`, `compose`, `transform` with the parity rule, the exact-scalar check, the layouts, and the messages.
   - WU13b: the rounding-scalar check of `transform`.

   CONVENTIONS 4.5 (ratified) says `transform` checks rounding scalars, so `develop` should never hold a `transform` without that check. WU13b therefore branches from `feature/core-transform` and merges back into it, and only then does `feature/core-transform` merge into `develop`. Each part gets its own W5 review. If the owner prefers WU13a to merge first with "a rounding scalar is not checked yet" stated, the commits are the same (O8).

5. **`transform` returns x's own type.** Decision 0165's rule gives an operation that moves blades the canonical layout of what it reaches. CONVENTIONS 4.4 says the result of `transform` has the blade set of x, so it returns `mv<Alg, S, T>` with x's own orientations: `transform(line, motor)` is a `Line`. This follows the ratified convention, which governs; it is not a new rule. `compose` follows decision 0165 (O2 and R6).

6. **Planning probes, not reproducible as committed (V10).** These statements rest on probes in the planner's scratch area, against the headers of `535c91e`, on g++ 16.2.1 and clang++ 23.1.4 and Python 3, each run through `tools/timed --log` into a scratch log, except one batch (below):
   - `parity.py`: exact, over `Fraction`, with `test/reference/clifford_reference.py` for the geometric product of any symmetric form. For rigid(2), R(3,0,0), rigid(3), R(2,2,0), conformal(2), rigid(4), conformal(3), and R(4,1,0), versors built as antiproducts of one, two, or three random hyperplanes (antigrade 1, each the wedge of n − 1 random vectors), and twelve candidate forms (sign ±, involution none, grade, or antigrade, antireverse or reverse), checked for: the outermorphism property λ^(k−1) T(a1 ∧ … ∧ ak) = T(a1) ∧ … ∧ T(ak) for k up to n, with λ the coefficient of V ⟇ V̰ on 𝟙; composition T_(W ⟇ V) = T_W ∘ T_V on all grades; V ⟇ V̰ = λ 𝟙; and, for one mirror, T(a) = λ a for a in the mirror. Results: an even V passes with the antireverse in every form, and with the reverse only where n ≡ 0 or 1 (mod 4); an odd V passes exactly with −V ⟇ x̌ ⟇ V̰ (and, where the reverse equals ± the antireverse, its equivalents), which is −V ⟇ x̂ ⟇ V̰ for n = 4 and +V ⟇ x̂ ⟇ V̰ for n = 3 and 5. Six to twelve trials per case: a sampled claim (V9), seed 20261104.
   - `antiproject.py`: exact, in rigid(3), with `test/reference/conventions_check.py`'s functions. b ∧ (a ∨ weight_dual(b)) equals, on 100 random cases each, the formulas the rigid wiki's Projections page gives for a plane onto a point, a line onto a point, and a plane onto a line (and the projection formula of a point onto a plane); the antiprojection of a plane onto a point contains the point, keeps the plane's normal when the point is unitized, and is unchanged when antiprojected again. The page was read through a fetch that a summarizing model converted to text, so it is a substitute source (V9): what it says must be read again from the page before 4.3 cites it (criterion 5).
   - `antiproject.py`, second part: in rigid(3), for V a motor or a flector layout and x a point, line, plane, motor, or flector layout, the composed plan V ⟇ x then ⟇ V̰ reaches 8 blades; it drops 4 for a point or a plane, 2 for a line, and none for a motor or a flector; and on 20 generic V of each layout, not normalized and not satisfying any constraint, every dropped coefficient is exactly zero. So in rigid(3), with these layouts, a dropped part is zero by a polynomial identity, whatever V is (R10).
   - `counts.py`: term counts of the two products, whole and restricted to the outputs on x's blades and the intermediate blades they read: motor or flector on a point 68 products, 48 restricted; on a line 84 and 72; on a plane 76 and 48.
   - `compose.cpp`: `transform` and `compose` written with today's public functions and `part` (`-std=c++23 -pedantic-errors -Wall -Wextra -Wconversion -Werror -ffp-contract=off -O2`). On both compilers: the exact rotor 3 e43 + 4 𝟙 over `std::int64_t` turns (1, 0, 0, 1) into (7, 24, 0, 25); the mirror e412 reflects (1, 2, 3, 1) into (1, 2, −3, 1) with −F ⟇ x̂ ⟇ F̰; `compose` applies its second operand first; in rigid(2) the mirror e23 reflects (1, 2, 1) into (−1, 2, 1) with +V ⟇ x̂ ⟇ V̰. All as `static_assert`s, so a constant evaluation. Assembly, one function per translation unit: transform(point, motor) 48 `mulsd`, 15 `addsd`, 21 `subsd` on GCC, no call, the restricted count of `counts.py`; 56 `mulsd` and 6 `mulpd` on Clang, no call; transform(line, motor) 72 `mulsd` on GCC, 59 `mulsd` and 13 `mulpd` on Clang; the odd composition for a point stays out of line on GCC (one call besides two to `__stack_chk_fail`); `compose` of two motors 48 `mulsd` on GCC, 35 `mulsd` and 7 `mulpd` on Clang. The first eight assembly compiles ran without `tools/timed`; they were repeated through it with the same results, and only the repeats have rows.

   The implementer and the math-expert repeat each as a test or a committed probe before relying on it.

7. **Trace rows of section 5 in the core.** CONVENTIONS marks 5.4 to 5.6 as model items and says they are instances of 4.4. Their facts (a composition order, a translator's value, a reflected point's weight) are values of the core's `transform` and `compose` on rigid(3) layouts, so WU13a pins 5.4, 5.5, and 5.6 with tests on layouts written in the test and sets those rows to `yes` (the trace table's rule: "in the change that implements the item in `include/galoot/core/` or in a model"). 5.1 to 5.3 and 5.7 stay for WU16, which adds the named types and constructors. If the owner reads 5.x as the model's alone, WU13a keeps the tests and leaves the rows `no` until WU16.

### Readings

R1. **Names and order.** `compose(Q2, Q1)` and `transform(x, V)` as 4.4 and 7.1 write them; `antiproject(a, b)`, the antiprojection of a onto b, object first like `project(a, b)` (decision 0174). No operator (7.1). One header for the first two, `include/galoot/core/transform.hpp`, which includes `product.hpp` and `unary.hpp`; `core.hpp` includes it; `galoot_core`'s header set lists it. `dual.hpp` gains `antiproject` and `antiproject_layout` and adds `antiproject_function` to its spelling enum. No new public involution: the antigrade involution of the odd rule is (−1)^n times the grade involution, so the sign folds into the kernel (R5).

R2. **The parity rule (D9, decision 0009).** A layout is even when every blade has an even antigrade n − gr, odd when every blade has an odd one, and mixed otherwise. `transform` rejects a mixed V at compile time with its own message (R11), pinned on both compilers: the DoD-19 catalogue item. An empty V, which has no blade of either parity, is accepted: both formulas give zero, so x's layout with zeros, and nothing is dropped. `compose` rejects an operand of mixed parity (O4), since its purpose is to build a versor that `transform` accepts; the antiproduct of two one-parity layouts has one parity, because antigrades add modulo 2 under ⟇, so `transform(x, compose(Q2, Q1))` never meets the parity rejection. A user who wants the antiproduct of anything writes `%`.

R3. **What `transform` computes.** Even V: V ⟇ x ⟇ V̰. Odd V: −V ⟇ x̌ ⟇ V̰, which is −V ⟇ x̂ ⟇ V̰ for even n and V ⟇ x̂ ⟇ V̰ for odd n, after O2. Then x's layout keeps its blades and the rest is dropped and checked (R8, R9). The comment says that `transform` does not unitize: a versor whose V ⟇ V̰ is λ𝟙 multiplies a transformed vector by λ, the same projective object with a scaled weight, and a unitized motor or flector has λ = 1. That is no assumption the code relies on, so nothing is checked about it (C8 is met by the statement).

R4. **`compose`.** Q2 ⟇ Q1, on `compose_layout<Alg, Q2, Q1>`, a public name that equals `geometric_antiproduct_layout` and exists, as for the duals (decision 0171), so that a mismatched comparison prints the operation's own layout on Clang's first line. Both are public interface, with the sixteen existing `*_layout` names (decision 0187). `compose(Q2, Q1)` of two motors gives a motor's eight blades, canonical and with orientation +1, so `Motor{compose(q2, q1)}` reads it under the motor's orientations (decision 0165). Its comment states the order: Q1 first.

R5. **Kernels (C4, P4).** `transform` is two plans the core already generates, the geometric antiproduct of V's layout with x's and of that result with V's layout, composed with per-basis data, as decision 0172 admitted the dot's scalar-part kernel:
   - the second stage keeps only the terms whose output is a blade of x, with the output moved to x's position and multiplied by x's orientation, as `make_product_kernel` does for a target;
   - the first stage keeps only the terms whose output the second stage reads;
   - the antireverse of V, the involution of x, and the odd sign are signs folded into the coefficients of the terms. Folding is exact under IEEE 754: negation is exact, and rounding to nearest is symmetric, so fl(−a − b) = −fl(a + b), and the result is bitwise the result of the unfolded composition;
   - in checking mode, a third set: the second-stage terms onto the dropped blades (R8, R9).

   The probes give the reason (departure 6): the plain composition with `part` already reaches the restricted count on GCC (48 multiplications for a point) but not on Clang (62), and GCC keeps the odd composition out of line. And WU13b's magnitude needs the same terms with absolute coefficients, which no composition of public functions provides. A kernel that collects the like terms of the sandwich (v_i v_k + v_k v_i), which would bring a point's transform near Lengyel's hand form, is plan 0001's D8 question (collected plans, Spike E, M7), not WU13's; WU13 records the counts for it. The kernels live in `transform.hpp`'s `detail`, with S7 accept and reject assertions in the tests (as decision 0173 placed the dot kernel's).

R6. **Result layouts.** `transform`: x's layout (departure 5). `compose`: `compose_layout`, canonical. `antiproject`: `antiproject_layout<Alg, A, B>` = `wedge_layout<Alg, B, antiwedge_layout<Alg, A, weight_dual_layout<Alg, B>>>`, canonical; a plane onto a point gives e123, e124, e134, e234 in canonical form (the plane's blades), read back with `Plane{antiproject(g, p)}`.

R7. **`antiproject` composition and convention.** b ∧ (a ∨ weight_dual(b)) = `wedge(b, antiwedge(a, weight_dual(b)))`, the design study's formula and the wiki's (departure 6, a substitute reading). Checked by the probe in rigid(3) against the wiki's three antiprojection formulas. The proposed 4.3 text (O2) states the definition and two facts: the antiprojection of a plane onto a unitized point is the plane through the point with the same normal, and that of a line onto a unitized point is the line through the point with the same direction. Messages as for `project` (R11). Never checks: it drops nothing.

R8. **The exact check (C8, 4.4, 4.5).** For an `exact_scalar` with `==`, every dropped coefficient must be zero: `GALOOT_ASSERT` in every constant evaluation and at run time when `GALOOT_CHECKING` is 1, with `transform`'s own message, not `truncate`'s. When can a dropped part be nonzero? Under the antireverse, the sandwich keeps a homogeneous x's antireverse sign, and antigrades k and k ± 2 have opposite signs, so only antigrades k and k ± 4 can mix:
   - In rigid(3), for the point, line, plane, motor, and flector layouts, nothing nonzero is ever dropped, for any V of one parity (probe, departure 6; the math-expert proves it for decision 0177).
   - Where n ≥ 5, or x holds antigrades 0 and 4 without both, a V that is not a versor (V ⟇ V̰ not a multiple of 𝟙) gives a nonzero dropped part, and the check reports it. The hand case for the tests: R(4,0,0), V = 1 + 𝟙 (antigrades 4 and 0, both even), x = 1 on {1}: V ⟇ 1 ⟇ V̰ = 2 + 2𝟙, and 2𝟙 is dropped (computed by hand from 1 ⟇ 1 = 𝟙 in R(4,0,0); the test author recomputes it in the reference).

   So for exact scalars the check verifies that V acts as a versor on x, which is what 4.4's "in checking mode every discarded part is verified to be zero" asks.

R9. **The rounding check (WU13b; decision 0010 as 0175 places it; 4.5).** For a rounding scalar, each dropped coefficient d must satisfy |d| ≤ its forward error bound, the tracked bound of decision 0175 with e = 0: γ_K M, with K the number of roundings on the coefficient's path and M the same computation on absolute values. Design, for the numerical-expert to derive and record in decision 0180:
   - **Which scalars.** Those whose `std::numeric_limits` say `is_iec559` and `round_style == std::round_to_nearest`: float and double are tested. `long double` meets the test on x86-64, but it is x87 there, which is out of scope, so it is neither tested nor claimed. A scalar that is neither exact nor such a type (`std::complex<double>`, a user type without limits, the test-only `tracked`) is not checked, and the comment says so, as `truncate`'s does for rounding scalars (O5).
   - **K.** Computed at compile time from the two restricted kernels in the order the kernel runs them, by the rules of `tracked` (an input 0; a product the sum plus 1; a sum the larger plus 1; a coefficient of ±1 no product), one K per dropped output. The tests require it to equal `tracked`'s K for every dropped coefficient.
   - **M.** Computed in checking mode by running the same kernels on the absolute values of V and x with the absolute values of the coefficients. Not from the absolute values of the computed intermediate, which cancellation can make smaller than the true magnitude.
   - **The comparison in T.** M itself is computed with up to K roundings, so M ≤ M'/(1 − γ_K); and forming γ and the product rounds. A margin, in the manner of decision 0175, makes the computed check pass for every d within the true bound. The planner's sketch gives a factor of at most γ_(2K+2) on the computed M'; the numerical-expert derives the exact form, beside the constant (S15).
   - **Underflow.** The γ bound assumes no underflow. Products of tiny inputs can lose absolute accuracy of the order of the smallest subnormal per operation, so the bound adds an absolute term (of the order of K times `denorm_min`, derived), and a test with inputs near 1e-300 shows no false abort.
   - **Not finite.** A dropped part that is NaN or infinite fails the check (O7): a checking build cannot verify it.
   - **When it runs.** Only where `GALOOT_ASSERT` checks (every constant evaluation; run time with `GALOOT_CHECKING` 1). With checking off, the magnitude kernels are not run, and the run code is R5's.
   - **What it cannot tell.** The bound covers this operation's roundings, taking V and x as exact. A V that carries rounding error from its own construction, and so is only nearly a versor, can drop more than this bound where a dropped part is not zero by identity: in n ≥ 5 (conformal(3) vectors, whose grade-5 part a non-versor leaves), or for x mixing antigrades 0 and 4. In rigid(3) with the five layouts every dropped part is zero by identity (R8), so a false abort cannot happen there. WU13b measures how often a rounded conformal(3) motor trips the check and records it; O6 asks whether that stands for version 1.

R10. **Evidence for the parity rule (V1 kind 1).** The probe is sampling. The math-expert writes the proof for decision 0177: the right complement maps the geometric-product algebra onto the antiproduct algebra, sending the reverse to the antireverse and the grade involution to the antigrade involution, so the sandwich forms are the images of v x ṽ and v x̂ ṽ; then the outermorphism of the induced vector map relates to the twisted adjoint by the determinant (−1 for a reflection), which gives the sign −V ⟇ x̌ ⟇ V̰; and the dropped-part claim of R8 for rigid(3). The proof is checked by a committed script in `records/spikes/transform-parity/`, the planner's `parity.py` made reproducible.

R11. **Messages (decisions 0166 and 0173).** One error, the operation as written, the mistake in the algebra's terms, the fix after a semicolon, no final period or quotes. The test author writes the final texts in the catalogue; proposed:
   - `transform`, mixed parity: "transform: the versor holds blades of both parities, even and odd antigrade; a versor is even, as a motor, or odd, as a flector"
   - `compose`, mixed parity (after O4): "compose: an operand holds blades of both parities, even and odd antigrade; compose versors that are each even or odd"
   - narrow scalar, one each for `transform`, `compose`, `antiproject`: "transform: the scalar type cannot hold one of the transform's exact coefficients; use a scalar type that holds larger integers exactly", and likewise (the composition's coefficients; `antiproject` checks `weight_dual`'s, as `project` does);
   - mixed operands, two each for `transform`, `compose`, `antiproject`: "<function>: the operands are in different algebras; give both types the same algebra tag" and "...have different scalar types; give both types the same scalar type";
   - the exact check (run time and constant evaluation): "transform: a coefficient outside the blades of x is not zero; V does not act as a versor on x"
   - the rounding check (WU13b): "transform: a coefficient outside the blades of x exceeds its rounding bound; V does not act as a versor on x, or it carries rounding error of its own"

   That is 11 compile-time messages (two parity, three narrow-scalar, six mixed-operand) and 2 run-time ones. Each compile-time one is pinned by an `expect_output` compile-fail test on both compilers, each also in the form `auto r = ...;`, with `apply:`, `geometric_antiproduct:`, `product_plan`, `detail::`, and `truncate:` forbidden on the first error line (`FORBID_ON_FIRST_ERROR`, decision 0173). `transform.hpp` has a rejecting function of its own for `transform` and `compose`, so `product.hpp` and `dual.hpp`'s enum gain nothing for them. Each narrow-scalar test uses a layout on which the checked plan differs from a mutant's (diag(100, 100, …) over `std::int8_t`, as decision 0173).

R12. **Limits.** `transform` costs two antiproduct plans and the restriction, so it inherits the antiproduct's limits (decision 0141; audit B1: the antiproduct of `full<5>` in conformal(3) fails on Clang without libstdc++'s assertions). The implementer measures the largest `full<n>` x and V that build for `transform` and `compose` on both compilers, with and without `_GLIBCXX_ASSERTIONS`, and in conformal(3), states them in the comment, and builds the new tests under `clang-measure`, as WU11a did (criterion 9).

R13. **The V11 rotor.** CONVENTIONS 5.5's rotor l sin φ + 𝟙 cos φ with sin φ = 3/5 and cos φ = 4/5 is exact over the test rational: transform((1, 0, 0, 1), (3/5) e43 + (4/5) 𝟙) = (7/25, 24/25, 0, 1), compared with `==` (V11); over `std::int64_t` the scaled rotor 3 e43 + 4 𝟙 gives (7, 24, 0, 25) (probe). Two such rotors composed turn by 4φ: cos 4φ = −527/625, sin 4φ = 336/625, exact.

### The W2 question

W2 is not amended (decision 0185, which makes these the standing practice), so the mechanics of plan 0020 apply: the test author commits each group on the tests branch, the implementer cherry-picks it with `-x` immediately before the code it judges, each cherry-picked test commit is the single failing commit W2 allows, and its message says so. The test author first compiles every group against a scratch stand-in (like the planner's `compose.cpp`) to show the tests are well formed and that the section 6 mutants fail them (lesson 0067).

### Owner questions (section 8)

O1 (M3 gate before WU13), O2 (CONVENTIONS 1.10), O3 (`antiproject` in `dual.hpp`), O4 (`compose` rejects mixed operands), O5 (scalars the rounding check covers), O6 (rounded versors in n ≥ 5), O7 (non-finite dropped parts), O8 (one merge after both parts).

## 2. Definition of done

WU13a:
- CONVENTIONS version 1.10 as ruled in decision 0192, with its revision row, and decision 0176 recording the text as committed and whether 4.3's antiprojection sentences have met their condition (decision 0192 item 2(c)).
- `include/galoot/core/transform.hpp`, new: `compose`, `transform`, `compose_layout`, the parity trait and the restricted kernels in `detail`, the rejecting functions, and the exact check. Its `///` comments state the definitions (4.4, restated), the parity rule and the odd sign, the result layouts with a rigid(3) example, that `transform` does not unitize (R3), what the check verifies (R8), the measured limits (R12), that a requires-expression asks a catch-all and is told yes, and `[[nodiscard]]` and `constexpr` on all. `core.hpp` includes it; the header set lists it.
- `include/galoot/core/dual.hpp`: `antiproject`, `antiproject_layout`, its catch-all, `antiproject_function`, and the head comment's table gains its row.
- `test/reference/conventions_check.py`: the antiprojection facts of 4.3 in rigid(3), and the parity rule for an odd mirror in rigid(2) (n = 3) beside the existing rigid(3) checks, so that the 1.10 labels can say Checked (math-expert role; the script's own forms, sharing no code with the library).
- `records/spikes/transform-parity/`: the committed check script of R10.
- Tests (doctest-expert role) in `test/core/transform_test.cpp` (labels `core;core.transform`), in `test/core/transform_identities_test.cpp` if the measured cost says so, and the `antiproject` group in `test/core/dual_test.cpp`; compile-fail tests for each message of R11; a `WILL_FAIL` run-time test and a constant-evaluation compile-fail test of the exact check (DoD-20).
- Trace table: 4.3 gains the antiprojection fact; 4.4 set to `yes` and gains rows for the parity and odd-sign facts and the composition order; 5.4, 5.5, and 5.6 set to `yes` (departure 7).
- Decisions 0177 (the parity rule and the dropped-part claim, with proofs, math-expert role), 0178 (split, header, layouts, names, kernels, `antiproject`'s home), 0179 (messages).
- Research: the rigid wiki's Projections page read under V9 (as LaTeX or raw wikitext, not only a summary) and recorded as an addendum to research note 0005 or a new note.
- Records: mutants (`records/mutations/0021-transform/`), diagnostics (`records/diagnostics/wu13a-messages.md`), W10 before and after for every suite that includes `core.hpp`, P2 for the smoke unit (pinned), the limits of R12, the assembly counts of criterion 10, and the plan's outcome for WU13a.

WU13b:
- The rounding check of R9 in `transform.hpp`, with its constants and derivation beside them (S15).
- Tests (doctest-expert role, independent of the implementer) in `test/core/transform_rounding_test.cpp` (labels `core;core.transform;core.rounding`), using `test/support/tracked.hpp` as the oracle; `WILL_FAIL` and constant-evaluation tests of the rounding check.
- Decision 0180 (the bound, the margin, the underflow term, the scalars covered, the characterization of rounded versors in conformal(3)), numerical-expert role.
- A 4.5 trace row: "transform checks a rounding scalar's dropped part against its forward error bound".
- Decision 0175's inventory gains `transform`, `compose`, `antiproject` (an appended section, since a recorded decision is not rewritten).
- Records: mutants (`records/mutations/0021-transform-rounding/`), the observed ratios of |d| to the core bound and to γ_K M, the cost of checking mode (instructions of a checking build of `transform_rounding_test.cpp` against an unchecked one), and the plan's outcome closed.

Both: a W5 blind review of each part (WU13a with the usability checklist, since it changes a public interface), a delta review after any header change that follows a review (decision 0094 item 3), and the merge of `feature/core-transform` into `develop` by the coordinator after P9's checks, with decision 0080's copy step of `compile.tsv`.

## 3. Acceptance criteria

WU13a:
1. **Values against independent references.** `transform` equals, coefficient by coefficient and exactly, the sandwich composed in the test from golden product tables and golden complement tables (as WU10's tests compose the antiproduct), for seeded V and x over the test rational, on every golden form with one-parity V layouts. `compose` equals the golden antiproduct. `antiproject` equals the golden composition b ∧ (a ∨ right(𝔾 b)) from golden G (WU11a's `exomorphisms`).
2. **Hand values in rigid(3)** (layouts written in the test):
   - the rotor of R13 over the test rational and over `std::int64_t`, and two rotors composed (4φ);
   - the translator e23 + 2 e31 − e12 + 𝟙 moves (5, 7, −1, 1) to (7, 11, −3, 1), and t_x e23 + t_y e31 + t_z e12 + 𝟙 moves by 2t in general (5.5);
   - the flector distinguishing test (section 8, M4): the mirror e412 reflects (1, 2, 3, 1) to (1, 2, −3, 1) with weight +1, where the single-sign form −F ⟇ x ⟇ F̰ would give weight −1; and a flector composed with a motor;
   - lines and planes move as the points that span them: transform(p ∧ q, V) = transform(p, V) ∧ transform(q, V), and likewise for planes, for a motor and both flectors (5.4, 5.6);
   - composition order: transform(x, compose(Q2, Q1)) == transform(transform(x, Q1), Q2), with Q1 and Q2 that do not commute, so a mutant that swaps them dies (5.4);
   - `antiproject`: the three formulas of R7's reading, containment, the normal and the direction kept for a unitized point, idempotence.
3. **The odd-n sign test.** In rigid(2) and R(3,0,0), a single mirror fixes the vectors in it with factor λ = (V ⟇ V̰)/𝟙, and the transform of a vector not in it is its reflection; in rigid(2), the mirror e23 sends (1, 2, 1) to (−1, 2, 1). In conformal(3), a motor built from two planes is accepted as even (its grades are odd) and moves a round point to the round point with the moved center and the same radius, with the expected value computed in the reference script. This test tells 4.4's old odd sign from the new one, and the grade parity from the antigrade parity.
4. **Identities over the signature set** (plan 0001 section 5), seeded and exact, for V the antiproduct of one to three random hyperplanes: the outermorphism property of departure 6 at every grade; composition; V ⟇ V̰ = λ 𝟙; dense five-generator forms only where the measured limits allow (R12); sparse cases at n = 8.
5. **Validation (V8).** The rigid(3) values of 5.4 to 5.6 against research note 0005 (A3, A4, F2), recomputed there. The antiprojection against the Projections page, read under V9 and recomputed (the reading is a definition of done item). Not validated: odd n and the non-diagonal forms, for which no published table was read; the report says so.
6. **Types and layouts.** `decltype(transform(x, V))` is x's type for every case; `compose_layout` and `antiproject_layout` each have an accepting and a rejecting `static_assert` and equal layouts written in the test from the definition, never one taken from `detail`.
7. **The parity rule.** `static_assert`s on the parity trait for an even, an odd, a mixed, and an empty layout in rigid(2), rigid(3), and conformal(3) (where grade parity and antigrade parity differ), each with its accept and reject cases (S7). A mixed V and, after O4, a mixed `compose` operand are rejected with their messages on both compilers; an empty V gives x's layout of zeros.
8. **The exact check.** The case of R8 (R(4,0,0), V = 1 + 𝟙, x on {1}) fails a constant evaluation with the message on the echoed line, and aborts at run time with `GALOOT_CHECKING` 1 (`WILL_FAIL`); with `GALOOT_CHECKING` 0 it returns 2 and does not abort; rigid(3) cases with nonzero coefficients on every input blade pass in both modes over `std::int64_t` and the test rational. A scalar that counts its comparisons shows the check compares exactly the dropped coefficients.
9. **Diagnostics and limits.** Each of the 11 compile-time rejections gives one error with its message on the first error line on GCC and Clang, with the forbidden names absent there, also as `auto r = ...;`. The R12 table is measured and stated in `transform.hpp`. `transform_test.cpp` builds and passes under `clang-measure` and `gcc-measure`.
10. **Run code (P4).** Read in the `-O2 -ffp-contract=off` assembly on both compilers, with checking off: transform(point, motor) and transform(point, flector) 48 multiplications each, transform(line, motor) 72, and transform(plane, motor) 48, the restricted counts of departure 6; compose of two motors no more multiplications than its plan has terms (the probe saw 48 on GCC; the plan's count is not yet confirmed); no call but the stack protector. Checking mode on is measured and reported, not bounded.
11. **Mutants.** Each mutant of section 6 is killed on gcc-debug and clang-debug by a test, never by a warning; an equivalent survivor is recorded with its reason.
12. **Checks.** P8 and P9; every build and test timed with `tools/timed`; the job pool's status read before and after each heavy run; all seven presets from new, empty trees at the final revision of the build inputs, on a clean tree; the trace gate, `tools/runlog check`, and the format check pass; no citation of records in code, comments, tests, or build files (S11); W2 as in section 1.

WU13b:
1. **Never a false abort on exact inputs.** For every dropped coefficient of `transform` over double, on seeded inputs p · 2^−27 (|p| < 2^27) for rigid(3) (point, line, plane by motor and by both flectors), conformal(2), and conformal(3) with V built exactly in double (dyadic hyperplanes whose antiproducts round nowhere, checked by `tracked`'s K = 0 for V): the core's tolerance is at least γ_K M with K and M from `tracked`, the core's K equals `tracked`'s K, and checking mode does not abort. Over float, the same with inputs sized for float.
2. **The check fires.** The R8 case over double aborts at run time in checking mode and fails a constant evaluation, with the rounding message. A dropped part of 10 times the bound, made with a V perturbed by a known amount, aborts; one at a tenth of the bound does not.
3. **Underflow and non-finite.** Inputs scaled near 1e-300 do not abort; a NaN or infinite input that reaches a dropped coefficient aborts (after O7).
4. **The derivation** is in decision 0180 and beside the constants, with the source pages of research note 0020 cited; a self-test pins the margin from both sides, as decision 0175's does.
5. **Characterization (O6).** For motors of conformal(3) built in double with rounding (products of planes with random double coefficients), the share of 10^4 seeded cases in which the check fires, and the largest ratio of the dropped part to the bound, are recorded. A share above zero is information for O6, not a failure.
6. **Mutants** of section 6 killed; **checks** as WU13a criterion 12.

## 4. Approach

Tests come before the code they judge, with the mechanics of "The W2 question". Expected-value changes to existing tests go in their own listed commits (W9). Small commits.

WU13a, on `feature/core-transform`:

0. Claim: this plan committed to `develop`.
1. Math-expert (on the tests branch, as test-side reference code, cherry-picked first): `conventions_check.py`'s new checks, including the bulk and the weight of diagonal forms with squares other than 0 and 1 for 3.2's label (decision 0190); `records/spikes/transform-parity/` with its script; the proof for decision 0177 drafted in the spike record; the V9 reading of the Projections page. The conventions check runs.
2. CONVENTIONS 1.10 as ruled (decision 0192: 2.1 defines the antigrade involution; 4.5 says which rounding scalars are checked; 4.3's antiprojection sentences only once the Projections page has been read directly and the new checks pass), decision 0176, and the new trace rows set to `no` (implementer, as WU11a's `cab40c0`). The labels "Checked" in the 1.10 text rest on step 1's checks, so step 1 comes first; if a step 1 check fails, the label stays Proposed and the owner is told before the text is committed.
3. Test group: `antiproject` (R7 values, containment, unitized cases, idempotence, layout, three compile-fail tests).
4. `antiproject` and its layout in `dual.hpp`.
5. Test group: `compose` (golden values, layout, parity trait `static_assert`s, the mixed-operand and narrow-scalar compile-fail tests, the mixed-parity test after O4).
6. `transform.hpp` with `compose`, `compose_layout`, the parity trait, the rejecting function; `core.hpp`; the header set; CTest registration.
7. Test group: `transform` (criteria 1 to 3, 6 to 8; the mixed-parity, mixed-operand, and narrow-scalar compile-fail tests; the exact check's `WILL_FAIL` and constant-evaluation tests; the restricted kernels' S7 assertions).
8. `transform`, the restricted kernels, the exact check.
9. Test commit: the identities over the signature set (criterion 4), passing when committed; the `slow` label only if measured.
10. Trace rows set to `yes`, with the trace gate run.
11. Records; decisions 0177 to 0179; the WU13a outcome. Blind review of WU13a; fixes; delta review.

WU13b, on `feature/core-transform-rounding`, from `feature/core-transform` after step 11:

0. A claim line for WU13b in this plan, on `develop`.
1. Numerical-expert: decision 0180's derivation, written before any rounding test runs (as decision 0175's record of the process asks).
2. Test group (test author): criteria 1 to 4, with the self-test of the margin.
3. The rounding check: absolute-value kernels, K at compile time, the comparison, the messages.
4. The characterization of criterion 5; mutants; records; the 4.5 trace row; decision 0175's appended inventory section; the outcome.
5. Blind review of WU13b; fixes; delta review; merge into `feature/core-transform`.

Then the coordinator merges `develop` into `feature/core-transform`, runs P9's `develop`-level checks (`gate:all`), and merges it into `develop`.

## 5. Evidence

Independent oracles (V1 kind 3) come from the golden header (`test/reference/golden.py`), whose tables the tests compose by routes that share no code with the library. `golden.py` itself needs no new namespace: the sandwich is a composition of golden product and complement tables, written in the test, as the antiproduct's expected values already are. Identities (kind 2) are checked exactly on seeded inputs over the signature set. The proof of decision 0177 (kind 1) covers the parity rule for every n and every form where V ⟇ V̰ is a nonzero multiple of 𝟙, and the dropped-part claim for rigid(3).

| Claim | Oracle |
| --- | --- |
| `transform`, `compose`, `antiproject` values | golden tables composed in the test; hand values from the rigid(3) complement table |
| 5.4 to 5.6 in rigid(3) | `conventions_check.py`'s `check_versors` (existing) and the C++ hand values |
| the odd-n sign | `conventions_check.py`'s new rigid(2) check, the proof, and the C++ mirror tests |
| 4.3's antiprojection | `conventions_check.py`'s new checks; the Projections page read under V9 |
| the exact check | the hand case of R8, recomputed in the reference |
| the rounding check | `test_support::tracked` (decision 0175), exact |

V10, to fill in at each outcome: verification as above; validation as criterion 5. Not validated: odd n, the non-diagonal forms, and the Terathon side of the flector disagreement, which stays untested under C15 and C16 (plan 0001 risk 9).

## 6. Mutants

WU13a, each on gcc-debug and clang-debug:
- parity by grade instead of antigrade (killed in conformal(3) and rigid(2));
- the odd sign −1 for every n (killed by the rigid(2) mirror), and +1 for every n (killed by the rigid(3) flector);
- the reverse in place of the antireverse (killed in rigid(2), even, and rigid(3), odd);
- no involution of x for an odd V (killed by a line under a flector);
- `compose` returns Q1 ⟇ Q2 (killed by the order test with non-commuting motors);
- `transform` drops nothing and returns the plan's layout; returns the canonical layout of x's blades;
- the second stage keeps every term, or drops one kept term; the first stage drops a term the second reads;
- the exact check removed; checking the kept coefficients instead of the dropped ones; using `truncate`'s message;
- the parity trait accepts a mixed layout, or rejects an empty one;
- `antiproject`'s operands swapped; `bulk_dual` in place of `weight_dual`; wedge and antiwedge swapped (which is `project`);
- each narrow-scalar check removed, and each checking another plan;
- the algebra and scalar constraints removed from each catch-all.

WU13b:
- the tolerance halved; the tolerance zero (an exact-zero check on double);
- K undercounted by one for the second stage;
- M from the absolute value of the computed intermediate;
- the margin removed (killed by the self-test at the edge);
- the check skipped for float;
- the underflow term removed (killed by the 1e-300 test, if the derivation shows it is needed; otherwise recorded as equivalent with the reason);
- a NaN dropped part passes.

## 7. Risks

1. **False aborts (plan 0001 risk 7).** A wrong bound aborts correct programs in checking mode. Mitigated by deriving before testing, by checking the core's K and bound against `tracked` coefficient by coefficient, and by the underflow test. The residual case, a versor with its own rounding error in n ≥ 5, is real and is O6.
2. **CONVENTIONS ruling.** WU13a cannot implement the odd sign until O2 is ruled. If the owner keeps 4.4's text, the plan halts under W8 before step 8 for odd n (the rigid(3) work does not depend on it).
3. **Antiprojection source.** The wiki page was read only through a summary. If the full reading differs from b ∧ (a ∨ b☆), section 8 gains a row and the owner decides (C11).
4. **Stacked limits** (R12): `transform` of large layouts in five-generator algebras on Clang. The models planned use typed layouts far below them.
5. **Two new public names** (`compose_layout`, `antiproject_layout`), public interface from the start (decision 0187).
6. **Compile cost.** Two plans per (V, x) pair plus the restricted kernels, and in checking builds the magnitude kernels. Measured (W10, P2); no bound (C14).
7. **Shared files.** `test/CMakeLists.txt`, `dual.hpp`, `dual_test.cpp`, `runs.tsv`, and this plan: whichever branch merges second merges its parent first (W7). No other unit is active on these files at this commit.
8. **M7 overlap.** The restricted kernel is not the collected form; DoD-24's "operation counts equal the ref script's collected-term counts" stays M7's (Spike E). WU13 records its counts for that comparison.

## 8. Open questions for the owner

- **O1. M3 before M4.** WU13 is in M4; M3 has no gate report yet (departure 1). Recommended: the coordinator submits the M3 gate report now (WU9 to WU11 are merged, with WU11b's inventory and bounds), and WU13a branches after the owner accepts it. Alternative: start WU13a now, declared as a W11 departure.
- **O2. CONVENTIONS version 1.10.** Recommended text:
  - 4.3: "The projection of a onto b is b ∨ (a ∧ b☆), and the antiprojection of a onto b is b ∧ (a ∨ b☆), written `project(a, b)` and `antiproject(a, b)` with the object first. For a point projected onto a unitized plane, the result has weight +1. The antiprojection of a plane onto a unitized point is the plane through the point with the same normal, and that of a line onto a unitized point is the line through the point with the same direction. (Checked)"
  - 4.4: "`transform(x, V)` applies a versor V to x by the parity of the antigrades of V's blades. An even V gives V ⟇ x ⟇ V̰, as in 5.4. An odd V gives −V ⟇ x̌ ⟇ V̰, where x̌ multiplies each blade of antigrade m by (−1)^m; when n is even x̌ is the grade involution x̂, as in 5.6, and when n is odd x̌ = −x̂, so an odd V gives V ⟇ x̂ ⟇ V̰. Either way the transform is the outermorphism of V's action on vectors, and a single reflection maps each vector of its mirror to itself times the coefficient of V ⟇ V̰ on 𝟙. A V with blades of both parities is rejected at compile time. The result has the blade set of x, and in checking mode every discarded part is verified to be zero, exactly for an exact scalar and within the operation's forward error bound for a rounding scalar (4.5). `compose(Q2, Q1)` is Q2 ⟇ Q1 for versors Q1 and Q2 each of one parity, and applies Q1 first: transform(x, compose(Q2, Q1)) = transform(transform(x, Q1), Q2). (Proposed; the odd sign Checked for n = 3 and n = 4)"
  - 7.1: "`transform(x, V)` applies 4.4" in place of "applies 5.4 or 5.6", and "the projections" added to the list of operations without an operator.

  Recommended: ratify, before WU13a's step 2, with the labels taking effect only if step 1's checks pass (section 4). The changes to 4.4 are the antireverse in place of Ṽ, the antigrade parity, the odd sign for odd n, and the one-parity condition on `compose` (O4); for rigid(3) none changes a value.
- **O3. `antiproject` in `dual.hpp`** beside `project` (departure 3). Recommended: yes.
- **O4. `compose` rejects an operand of mixed parity** (R2), the earliest point a mixed versor can be reported (C9). Recommended: yes. Otherwise `compose` accepts anything `%` does, and a mixed result is rejected when it reaches `transform`.
- **O5. Scalars the rounding check covers** (R9): IEEE binary types rounding to nearest, with float and double tested and `long double` neither tested nor claimed (x87 is out of scope); other inexact scalars, such as `std::complex<double>`, unchecked and documented. Recommended: yes. The alternative, a complex bound, has no present user (C12).
- **O6. Rounded versors in n ≥ 5** (R9, risk 1). D10 checks a dropped part against this operation's bound only, so in conformal(3) a motor that carries its own rounding error may abort a checking build where the dropped part is not zero by identity. Recommended: keep D10 as ruled for version 1, documented in the comment and characterized by WU13b (criterion 5), and revisit with the cga3 model (M6), whose rules on versor construction can then be measured. Alternatives: add a term for V's own defect (needs a versor norm, more core machinery), or check rounding scalars only where every dropped part is zero by identity (needs symbolic collection, M7's D8).
- **O7. Non-finite dropped parts fail the check** (R9). Recommended: yes; a checking build cannot verify a NaN. The alternative skips the check when M is not finite.
- **O8. One merge after both parts** (departure 4). Recommended: yes, so that `develop` never holds a `transform` without 4.5's rounding check. The alternative merges WU13a first with the gap stated.

Owner's answer, 2026-11-07 ("proceed with recommendations", decision 0192): O1 to O8 as recommended, and departure 7 approved. O2 with three changes to the text above: 2.1 defines the antigrade involution ("the antigrade involution multiplies a blade of antigrade m by (−1)^m"); 4.5 says which rounding scalars the check covers, those of O5, and that others, such as `std::complex<double>`, are not checked, since O5's exemption contradicts the ratified 4.5 otherwise; 4.3's antiprojection sentences are ratified only after the Projections page has been read directly under V9 and the new checks pass. The questions this plan listed as open with the owner are ruled: W2 (decision 0185), the public `*_layout` names (0187), B1 (0184: the `full<5>` cases are made configuration-independent and `clang-measure`'s full suite joins P9's merge checks before WU13 merges), subfeature branch names (0188), and F4 on 3.2's label (0190).

## 9. Agent cost estimate

Recorded in `docs/agent/records/agent-cost.tsv` as each agent finishes. For comparison, plan 0020 estimated WU11, both parts, at 2.1M to 2.9M tokens; the WU11a test author used about 440k to 470k per run (agent-cost.tsv), and blind reviews have used 210k to 370k.

| Part | Role | Model | Estimate |
| --- | --- | --- | --- |
| WU13a | test author (doctest-expert) | Opus, high | 400k to 550k tokens |
| WU13a | proof for decision 0177, `conventions_check.py`, spike script, V9 reading of the Projections page (math-expert) | Opus, high | 150k to 250k |
| WU13a | implementer (cpp-expert role) | Opus, high | 500k to 700k |
| WU13a | blind reviewer (W5, with the usability checklist) | Opus, high | 300k to 400k |
| WU13a | delta reviewer, if the review asks for fixes | Opus, high | 200k to 270k |
| WU13b | derivation for decision 0180 and implementer (numerical-expert role) | Opus, high | 350k to 500k |
| WU13b | test author (doctest-expert, with numerical-expert judgement) | Opus, high | 250k to 350k |
| WU13b | blind reviewer (W5) | Opus, high | 250k to 330k |
| WU13b | delta reviewer, if needed | Opus, high | 150k to 220k |

Size M overall, as plan 0001 has it, at the upper end: about 2.6M to 3.6M tokens in all. This planner's own cost is recorded by the coordinator from the harness report.

## 10. Notes for later units

- In rigid(2), the probe's rotor 3 e3 + 4 e123 turns (1, 0) to (7/25, −24/25): clockwise in the e1, e2 plane. A future rga2 model fixes its own sense of rotation in its section of CONVENTIONS; WU13 makes no claim about it.
- The restricted kernel's counts (48 multiplications for a point under a motor) are the baseline that M7's collected plans are compared against (Spike E).

## 11. Outcome

### WU13a

Implemented on `feature/core-transform` (implementer and coordinator: Claude, agent, Opus 5.5, cpp-expert role; test author: Claude, agent, Opus 5.5, doctest-expert role, on `feature/core-transform--tests`; step 1, decision 0177 and research note 0021: Claude, agent, Opus 5.5, math-expert role, on `feature/core-transform--math`). Reviewed (W5, with the usability checklist): approve with non-blocking findings (`records/reviews/2026-11-07-wu13a-review.md`); the findings are resolved below, and a delta review follows. Not merged. WU13b has not started.

Commits, in the order of section 4. Each test group was committed on `feature/core-transform--tests` and cherry-picked with `-x` immediately before the code it judges (decision 0185); each such commit is the single failing commit that W2 allows, and its message says so. The math-expert's commits were cherry-picked the same way.

| Step | Commit | What |
| --- | --- | --- |
| 1 | `3126146`, `89dcf2d`, `0fb27bd`, `48e090a` (from `db1a349`, `8f82ba5`, `dcb182a`, `960bdb4`) | `conventions_check.py`'s new checks; `records/spikes/transform-parity/`; research note 0021; decision 0177 |
| | `62c3f8d` | merge of `develop` (`51c18de`: M3 acceptance records, the test split, the B1 fix) |
| 2 | `549cdc8` | CONVENTIONS 1.10, decision 0176, the new trace rows set to `no` |
| 3 | `ddf4968` (from `9dbace5`) | tests of `antiproject` |
| 4 | `dc3fdd8` | `antiproject` and `antiproject_layout` in `dual.hpp` |
| 5 | `51436cf`, `371e8ff` (from `090dd32`, `8583fe3`) | tests of `compose` and the parity trait; the test author's fix of the reference markers of groups 1 and 2 |
| 6 | `7dba23f` | `transform.hpp` with `compose`, `compose_layout`, the parity trait; `core.hpp`; the header set |
| 7 | `e4d39d2` (from `ee2002c`) | tests of `transform` |
| 8 | `79eb30e` | `transform`, its kernels, the exact check |
| 9 | `7671e76` (from `558f8cd`) | the identities over the signature set (`transform_identities_test.cpp`), passing when committed |
| | `1ca4508` | the kernels' static_asserts (the implementer's own test, listed for review) |
| 10 | `914c28d` | trace rows of 4.3, 4.4, and 5.4 to 5.6 set to `yes` |
| | `b21650b` | the measured limits in the comments of `transform.hpp` and `dual.hpp` |
| | `f9ad5bc`, `1217145` | clang-tidy fixes: the header's second-stage kernel; the tests (listed for review: no check or expected value changes) |
| 11 | `f2d8322`, `8fcd06c`, and this commit | decisions 0178 and 0179, diagnostics, agent costs; mutants, run code, limits, build cost; this outcome and the final runs |

Departures and readings (C20):

1. **4.4 wording (decision 0176 item 3).** The committed 4.4 says that the transform of a versor with V ⟇ V̰ = λ𝟙 satisfies λ^(k−1) T(a1 ∧ … ∧ ak) = T(a1) ∧ … ∧ T(ak), and is the outermorphism of V's action on vectors when λ = 1, where O2's ratified sentence said it "is the outermorphism" without the factor. Decision 0177 (theorem 4) shows O2's sentence holds only for λ = 1, and needs the hypothesis that an antiproduct of hyperplanes meets. The owner is asked to confirm the corrected sentence; no value or code depends on it. The 2.1 clauses added after the ratified definition of the antigrade involution, "so it is (−1)^n times the grade involution, and the right complement takes it to the grade involution: right(x̌) is the grade involution of right(x)" with "its relations Checked", are also unratified (decision 0192 item 2(a) ratified the definition alone) and are put to the owner with it (decision 0176 addendum). CONVENTIONS' status line and its 1.10 revision row say that both passages await the owner.
2. **The odd sign is Checked for n = 3, 4, and 5**, not "n = 3 and n = 4" as O2 proposed: the math-expert's check covers rigid(4) and R(5,0,0) too.
3. **Plan 0021 R10's "for every n and every form where V ⟇ V̰ is a nonzero multiple of 𝟙" is false as stated** (decision 0177, 3.4: R(6,0,0), V = 1 + 𝟙). The theorem holds under hypothesis (H), which antiproducts of hyperplanes meet in every n, and which is automatic for n ≤ 4 and for n = 5 when the form has rank at least 2. `transform.hpp`'s comment states the result for versors, antiproducts of hyperplanes.
4. **Branch names (decision 0188).** `feature/core-transform--tests` and `feature/core-transform--math` in place of the plan's `feature/core-transform-tests`; the math-expert worked on its own branch rather than on the tests branch, so the two agents ran at once without sharing a worktree.
5. **The exact check's message** is "transform: v does not act as a versor on x" (after review finding 5; at first "transform: the result leaves the blades of x"), shorter than R11's proposal, so that it fits on the line of `GALOOT_ASSERT(` (decision 0179). It is 42 characters, of the 48 there; decision 0179's addendum and the message of `9fe1d3d` said 43, which the correction appended to decision 0179 fixes.
6. **The narrow-scalar check of `transform` reads both whole plans**, not only the restricted kernels' terms (decision 0178): checking mode computes every term, so a scalar accepted unchecked must hold them too.
7. **The kernels' S7 assertions are in `transform_test.cpp`, written by the implementer** (`1ca4508`), not by the test author, as WU11a did for the dot's kernel.
8. **The conformal(3) round point's expected value is computed in `transform_test.cpp`** from CONVENTIONS 6.1 and the moved center, not in `conventions_check.py` as criterion 3 says (the test author's departure 3); its planes use 6.2's layout, which 6.2 labels Study.
9. **The dense five-generator identities are reduced** to versors of at most two hyperplanes and grades up to 3 (the test author's departure 5): with the full set, Clang 23 hits its step limit. They build and pass under `clang-measure` with the real kernels.
10. **Criterion 10 on Clang:** Clang 23 keeps each `transform` out of line behind the probe's tail jump, and in `transform(line, motor)` calls the first stage's kernel; `compose` of two motors is a tail jump to its kernel, with 49 multiplications counted against 48 terms (not explained). GCC inlines everything and calls nothing. The multiplication counts equal the restricted counts on both compilers.
11. **No hci-expert or cpp-expert agent was spawned**: the implementer did the cpp-expert work; the messages were written by the test author under decisions 0166 and 0173, and the usability checklist is the blind reviewer's.
12. **The test author's `558f8cd` message** says "24 forms"; the file has 22. Not amended (no rewriting of history).
13. **Mutants:** 29 ran at `1ca4508` and 6 at `f2d8322` (records README); between them the header changed only in comments and in a clang-tidy rewrite that keeps every term.
14. **The math-expert lost four run-log rows** of intermediate runs (its departure 3) and timed some exploratory runs into a scratch log only.
15. **The job pool** was not running when this unit started; another agent's build started it a minute later (it was running when status was next read). This unit started nothing.
16. **W2: the suite failed at a code commit, and two failing test commits came in a row** (review finding 2). `tests.reference_markers` and `tests.golden_facts` fail at `dc3fdd8`, the antiproject code, on the reference comments that `ddf4968` added to `dual_test.cpp`: four marker errors and three golden facts not stated. `371e8ff`, the test author's fix, which does not build either (it precedes `compose`), came right after the failing test commit `51436cf`, so two failing test commits came in a row, which W2's single commit does not allow. History is not rewritten. The test author's check before hand-over compiled each group against a stand-in but did not run the `tests.*` checks; a later test author should run them too.

Evidence:

- Verification (V1 kinds 1 to 3): `transform` and `compose` against the sandwich and the antiproduct composed in the test from golden product and complement tables, on the five golden forms and the oriented rigid(3) layouts, exactly over the test rational; `antiproject` against b ∧ (a ∨ right(𝔾 b)) from golden G on the same forms; the rigid(3) hand values of criterion 2 (the rotor of R13 over the test rational and `std::int64_t`, 4φ, the translator, the flector distinguishing test, lines and planes as the points that span them, the composition order); the odd-n tests in rigid(2) and R(3,0,0); the conformal(3) motor on a round point; types and layouts; the parity trait; the exact check in both modes and by a scalar that records its comparisons; the identities of criterion 4 over 22 forms of two to five generators and sparse cases at n = 8, seeded and exact. Proofs: decision 0177, checked by `records/spikes/transform-parity/check.py` (1720 exact checks, seed 20261107, all passing). `conventions_check.py`: "all convention checks passed (seed 20261031)".
- Validation (V8): the antiprojection against the Projections page, read directly and recomputed (research note 0021: nine formulas agree as polynomial identities in rigid(3); one erratum, not used). The rigid(3) values of 5.4 to 5.6 equal those CONVENTIONS marks Confirmed from research note 0005, but were not recomputed against the note in this unit. Not validated: odd n and the non-diagonal forms (no published table read), and the Terathon side of the flector disagreement (C15, C16).
- Mutants (`records/mutations/0021-transform/README.md`): 35, each on gcc-debug and clang-debug; 34 killed on both by a test; one equivalent survivor, `compose_checks_reversed_plan`.
- Diagnostics (`records/diagnostics/wu13a-messages.md`): each of the 17 compile-fail sources (the 11 rejections of R11, the extra narrow-scalar and parity variants, the two comparisons, the two constant-evaluation checks) gives one error line with its message, on both compilers.
- Run code (`records/performance/transform-cost/`): `transform` of a point by a motor or a flector 48 multiplications, of a line 72, of a plane 48, `compose` of two motors 48 (49 on Clang), the restricted counts; calls as in departure 10.
- Limits (same record): `transform` up to n = 5 on Clang and 6 on GCC, `compose` up to 5 and 7, `antiproject` up to 5 and 6, all for conformal(3); the dense five-generator form fails on Clang for `transform` and `compose`. Stated in the headers.
- W10 (same record, pinned, three repeats): an includer of `core.hpp` +7.7 M instructions (+0.8%) on GCC and +7.9 M (+0.8%) on Clang; `smoke_test.cpp` +0.6% and +0.8%; `dual_test.cpp` +21.9% and +20.2%; `transform_test.cpp` 125.1 G and 111.7 G; `transform_identities_test.cpp` 402.3 G (25.7 s, 2.4 GB) and 365.6 G (22.7 s, 1.2 GB).
- Final runs: every preset from new, empty build trees at `8fcd06c` (the final revision of the build inputs is `1217145`; later commits change records only), with a clean tree: the run rows read `8fcd06c4034d` without `-dirty`. Configure, build, and test each exited 0 with no FAILED build step: gcc-debug and gcc-release 365 tests, gcc-asan 357, clang-debug, clang-release, clang-asan, and lint 367, all passing; `clang-measure`, its full suite (decision 0184), 359 of 359. Thirty tests are new (25 labeled `core.transform`, 5 in `core.dual`). The trace gate, `tools/runlog check`, and `clang-format --dry-run --Werror` on the 25 changed C++ sources exit 0. The job pool read 23 of 23 slots free before and after.

Resolution of the blind review's findings (2026-11-07): 1, the CONVENTIONS status line and revision row name the two passages that await the owner (decision 0176 addendum); 2, departure 16; 3, the first stage's restriction pinned by a degenerate case (`f57800a`; decision 0178 addendum), and the reviewer's mutant `first_keeps_all` now dies on both compilers; 4, the counting script committed (`records/performance/transform-cost/counts.py`); 5, the message (`9fe1d3d`; decision 0179 addendum: the reviewer's text holds a quote character, and no fix after a semicolon fits); 6, "about" in the header example (`34cd05d`); 7, the published sources of 3.3 and 3.4 (decision 0177 addendum); 8, trace rows for 4.4's λ^(k−1) relation and mirror-times-λ fact and 4.3's line onto a point; 9, `asm.cpp` formatted, its assembly unchanged. The commits listed for review are `f57800a` (the implementer's test) and `9fe1d3d` (the header and the tests' expected message together).

Open, for the owner or later units: item 1 above (the 4.4 sentence and the 2.1 clauses); WU13b (the rounding check, decision 0180); `transform_identities_test.cpp` is now the largest unit of the suite (402 G instructions on GCC), not labeled `slow`; Clang's out-of-line kernels (departure 10), which a `[[gnu::always_inline]]` or a different kernel split could address if measured to matter; the math-expert's draft knowledge-base note on versor sandwiches in the antiproduct algebra, not written, since the brief gave no permission to write there.

### WU13b

Implemented on `feature/core-transform--rounding`, branched from `feature/core-transform` at `01835b7` after WU13a's review was resolved (implementer, and the derivation of decision 0180: Claude, agent, Opus 5.5, numerical-expert role; test author: Claude, agent, Opus 5.5, doctest-expert role with numerical-expert judgement, on `feature/core-transform--rounding--tests`, branched at `0ba8f51`). Not reviewed yet (W5); not merged.

Commits, in the order of section 4:

| Step | Commit | What |
| --- | --- | --- |
| | `bf62954` | WU13a review follow-up: transform's comment says that the exact check's message also comes for a versor when x's layout holds only some blades of a grade, and that the cure is every blade of its grades (comment only) |
| | `c906ac2` | WU13a review follow-up: the exact check's message is 42 characters, not 43 (a correction appended to decision 0179, and the count in departure 5 above) |
| 1 | `0ba8f51` | decision 0180, the derivation, before any rounding test was written or run; research note 0020's addendum (Higham (2.8) and monotonicity, pp. 56-57) |
| 2 | `a4e43af` (from `d2902da`) | the rounding tests: the single failing commit W2 allows, its message says so |
| 3 | `c2af6a8` | the rounding check in `transform.hpp` |
| 4 | `ce5449a`, `47b3bfc`, `a8bbe97` (from `9db9a95`), `c6e1f3c`, and this commit | the mutation script; mutants, characterization, the 4.5 trace row, decision 0175's addendum; the test author's run-log rows; the cost script; costs, decision 0180's addendum, agent cost, this outcome |
| | `cb04901` | a clang-tidy suppression in the margin guard (comment only, listed for review) |

Departures and readings (C20):

1. **The tests branch is `feature/core-transform--rounding--tests`**, as decision 0188 and this plan name it; the coordinator's brief wrote `feature/core-transform--tests-rounding` while citing decision 0188, which governs.
2. **No claim line on `develop`** (section 4, WU13b step 0): the coordinator's brief forbids touching `develop`. This plan, committed to `develop`, already claims both parts.
3. **Decision 0180**, the number this plan reserved and which was still free, not the next number after the highest (0193).
4. **The underflow term and the margin differ from R9's sketch** (decision 0180): R9's additive "K times denorm_min" is unsound, since a later factor scales an underflowed product's error (a probe in exact rationals exceeds it by a factor of about 900); the check adds λ, the smallest normal number, after every rounded product in the magnitude instead. The margin is one rounding, γ_(K+1), not the planner's γ_(2K+2).
5. **A padded magnitude that overflows fails the check**, a choice the owner has not ruled on (O7 covers a non-finite dropped part); decision 0180 weighs it.
6. **`transform.hpp` stops with `#error` under `-ffast-math` in a checking build** (STYLE S15; decision 0180), which the plan did not list; `core.transform_fast_math` pins it.
7. **Criterion 1's inputs are p / 2^18 for double and p / 2^9 for float**, not p · 2^−27: the transform is cubic, and 2^−81 denominators overflow the test rational (the test author's sizing, as decision 0181 sized the projection's). Criterion 2 is built in R(4,0,0), not conformal(3). The underflow case's coefficients were found by a scratch search of the test author's, not committed; the test checks the property it relies on (a dropped coefficient nonzero and beyond γ_K M).
8. **`c2af6a8`'s message says gcc-debug 389 of 389**; the preset selects 388 (389 counted the run-log check, which presets leave out). Not amended.
9. **The gcc output of the characterization** came from a second, untimed run of the same binary with the same arguments (its README says so).
10. **Not run as a mutant:** "the check reads the kept coefficients", which does not compile in the header (the README of the mutants says why).
11. **Unexplained:** why rounded versors of flat planes trip the check in conformal(3) and those of general hyperplanes did not (characterization below). **Not profiled:** the cause of checking mode's run cost.

Evidence:

- Verification (V1 kinds 1 and 3): the derivation in decision 0180 (Lemmas A to C, from Higham's (2.8), Lemmas 3.1 and 3.3, and monotonicity, read under V9 in research note 0020 and its addendum). `core.transform_rounding` checks, over double and float, on 16 families of seeded exact inputs (rigid(3) point, line, and plane by the motor and both flectors; a non-diagonal form; conformal(2); conformal(3) with exact antiproducts of one to three hyperplanes), every dropped coefficient against `tracked`: the core's K equals the oracle's, γ_K M ≤ tol < γ_(K+2) M exactly (the margin pinned from both sides), |d| ≤ tol, |d| ≤ γ_K M, and `transform` returns the unchecked coefficients bit for bit; also products that underflow, a hand case in R(4,0,0) (K = 3, tol = 2 fl(γ_4)), constant evaluations, and the scalars covered. The checking-mode programs abort with the message for a non-versor over double and float, a dropped part ten times its tolerance, a NaN, an infinity, and an overflowing magnitude, and not at a tenth of the tolerance or for underflowing products; the constant-evaluation and `-ffast-math` tests pass. Largest ratios at seed 0: |d| / tol 0.089 (double) and 0.151 (float), |d| / (γ_K M) 0.098 and 0.181; underflow 0.111.
- Validation (V8): not applicable beyond verification; the bound is a theorem about this computation, and its tightness was observed, not validated against an outside referent.
- Mutants (`records/mutations/0021-transform-rounding/`): 17, each on gcc-debug and clang-debug, with the same outcome on both: 16 killed by a test, one equivalent survivor, the guard of the margin's hypothesis.
- Characterization (`records/spikes/transform-rounding/`, criterion 5, O6): 10,000 seeded cases per family, identical on both compilers; the check fires for 11 (two flat planes) and 15 (three) conformal(3) vectors, at most 3.76 times the tolerance, and never in the other six families.
- Cost (`records/performance/transform-rounding-cost/`, pinned, three repeats): +0.3% (GCC) and +0.4% (Clang) instructions for an includer of `core.hpp`; `transform_test.cpp` unchanged within its spread; `transform_rounding_test.cpp` 47.8 G and 33.5 G, the same in both checking modes; checking mode's run cost 10.7× (GCC) and 9.4× (Clang) the instructions of an unchecked run and about 27× its wall time.
- Final runs, every preset from new, empty build trees at `c6e1f3c` (build inputs final at `cb04901`), with a clean tree: configure, build, and test each exited 0 with no failed build step: gcc-debug and gcc-release 388 tests, gcc-asan 380, clang-debug, clang-release, clang-asan, and lint 390, clang-measure 382, all passing. Twenty-three tests are new, each labeled `core.transform` and `core.rounding`. The trace gate, `conventions_check.py`, `tools/runlog check`, and `clang-format --dry-run --Werror` on the changed C++ sources exit 0. The job pool read 23 of 23 slots free before and after each heavy run.

Listed for review (W9): `a4e43af` (the tests, the test author's); `c2af6a8` (the check); `cb04901` (a suppression comment); `bf62954` (a comment). The implementer judged no test of its own; no existing test or expected value changed.

Open, for the reviewer or the owner: departure 5 (overflow fails); checking mode's run cost, which computing the γ factors at compile time and unrolling the magnitude kernel would likely cut (inferred); the flat-plane characterization, for M6; WU13a's open items above.
