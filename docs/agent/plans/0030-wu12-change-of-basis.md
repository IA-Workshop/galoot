# 0030 WU12: the exact change of basis, and the non-diagonal and Witt forms through it (M5)

- Status: planned 2026-11-08 by Claude (agent, Opus 5.5), planner role with math-expert judgement and cpp-expert, numerical-expert, doctest-expert, and build-expert considerations, on `records/plan-wu12` (worktree `.claude/worktrees/plan-wu12`) from `develop` at `774ff66`. Not merged by its author. The coordinator claims the unit by cherry-picking this plan into `develop` (W7). Only step 0 of section 9 starts before the owner has answered O1 to O9 (section 15) or has said to proceed as recommended. After that answer, parts D, A, and N may start at once. Parts B and C wait for the merge of plan 0031 (departure 7).
- Branches, from `develop` after the claim (W7, decision 0188):
  - `feature/core-basis-change`, worktree `.claude/worktrees/core-basis-change`: the unit. Only this branch merges into `develop`. It branches after plan 0031's merge.
  - `feature/core-basis-change--math`, worktree `.claude/worktrees/core-basis-change--math`: the math-expert's reference work (`test/reference/` additions, `conventions_check.py`, the derivation, the predictions). It may branch from `develop` at the claim, since it touches no file of plan 0031. It merges into the unit.
  - `feature/core-basis-change--tests`, worktree `.claude/worktrees/core-basis-change--tests`: the test author's commits. It is never merged; each group is cherry-picked with `-x` into the unit (decision 0185).
- Record numbers, free on every branch at this commit (plan 0031 took research note 0025 and the mutation record 0031): decision 0219 (the owner's rulings on this plan), 0220 (part D's interface), and 0221 (the implementer's choices); research note 0026 (part A's readings, derivation, and the Sage cross-check); the mutation record `records/mutations/0030-basis-change/`. A number taken in the meantime moves to the next free one.
- Source of scope: plan 0001 (section 8, WU12 "`basis_change.hpp` and its V4 check", size M, depending on WU5 and WU11; section 6, M5 "Non-diagonal forms and change of basis", exit evidence "Non-diagonal and Witt signatures in DoD-10; 1.4 pinned; isomorphism and round trip; conformal-duals distinguishing test; core V8 matrix", owner decides "Conformal-duals confirmation; wording of 1.4"; section 5, the signature set; DoD-1, DoD-3, DoD-7, DoD-10, DoD-11, DoD-12, DoD-16, DoD-19). Also: decisions 0003 (non-diagonal forms through exact change of basis), 0094 item 2 (WU5 moved to M5 beside its first user), 0151 item 4 (the test rational is not WU5's), 0216 items 1 and 8, 0217 item 6, and 0218 items 4 and 5; plan 0029's coverage table (its three "WU12" rows) and outcome; plan 0031; research notes 0009 and 0014; the knowledge base's `change-of-basis-and-diagonalization.md` and `structures/universal-property-and-functoriality.md`; and math-tools' `docs/agent/math-expert/sage-route-a.md` and `nlab-exercises.md` (ga-N6, ga-N13).

## 1. Departures, ambiguities, and readings (C20)

### Departures and ambiguities

1. **WU12 is wider than plan 0001's line for it.** Plan 0001 gives WU12 "`basis_change.hpp` and its V4 check". But M5 holds WU12 alone, and M5's exit evidence also asks for the core V8 matrix (DoD-16), the owner's confirmation of the conformal duals, and the wording of 1.4. Reading followed: WU12 produces all of M5's exit evidence, and the parts of DoD-16 that are not yet done are part of this unit. O1 asks whether to split them off instead.
2. **WU5, the core rational, may have no user.** Decision 0094 moved WU5 to M5 "beside the change of basis, its first user". Section 6 shows that the change of basis can keep every generated coefficient an integer: the rows of P are integers over a positive denominator per row, and each result coefficient is divided once by an integer. No rational arithmetic is then needed in the core. The plan recommends closing WU5 by decision (O2). Closing it changes the roadmap, so the owner decides.
3. **"Sage Route A is WU12's independent oracle" (decision 0218 item 4) against C15 and V1.** C15 says that other implementations "serve only as black-box cross-checks after Galoot's own evidence stands, and never supply an expected value". V1 kind 4 says the same. Sage's `CliffordAlgebra` is another implementation. Reading followed: Sage Route A with `lift_isometry` is the independent cross-check of the change of basis. Its outputs are compared with Galoot's and with the reference script's, after the kind 1 to 3 evidence passes, and the comparison is recorded in research note 0026. No Sage value enters `test/` as an expected value. The expected values come from `test/reference/`, which shares no code with Galoot or Sage. This is how research note 0014 used Sage for the product tables. O6 asks the owner to confirm the reading.
4. **DoD-1's "symmetric rational forms" against the integer Gram matrix.** DoD-1 says the core accepts "symmetric rational forms given with an exact change of basis". Decisions 0060 and 0101 made the Gram matrix integral, and decision 0003 says "rationals appear in the change of basis and form entries". Reading followed: Gram matrices stay integral. A rational form, such as the canonical Witt form with a · a† = ½, is reached by clearing denominators, which is itself a change of basis that rescales generators: the scaled Witt form [[0, 1], [1, 0]] with c = e1 and c† = ½ e2 satisfies c c† + c† c = 1. O7 asks the owner to confirm this.
5. **The probe's findings are sampled, not proven.** Section 5.2 rests on a planning probe: nine changes of basis and 20 seeded draws each (`records/spikes/wu12-planning-probe/`). Under V9 it supports hypotheses only. Part A derives what each operation does under a change of basis before any test states it.
6. **Planning probes outside `tools/timed` and the pool.** I ran two scripts once each, `naturality.py` (standard library, under a second) and `sage_lift_isometry.py` (about 1.6 s), plus two one-off checks of hand values. They build and test nothing of the library, and I ran them outside `tools/timed` so that this branch carries no run-log rows. The first Sage run went through `uv run --frozen --project …/py-sage`, which recreated math-tools' `py-sage` virtual environment from its frozen lock with CPython 3.14.9 in place of the 3.14.8 its record names; the locked packages are unchanged (passagemath-modules 10.8.13). The commands and outputs are committed in `records/spikes/wu12-planning-probe/`. No build or test of the library was run for this plan.
7. **Plan 0031 runs now and shares files.** Plan 0031 (the WU14 follow-ups) changes `test/core/signatures.hpp`, the identity sources, `dual_identities_test.cpp`, `test/CMakeLists.txt`, and plan 0029's coverage table. W7 asks units that run at the same time to change different files. Reading followed: the unit branch and its test branch start after plan 0031 merges. Parts D, A, and N touch none of those files and may start at the claim. WU12 adds no form to the shared lists. Its structure layer iterates `for_each_signature`, so the two forms plan 0031 appends are covered once they are there.

### Readings

R1. **"Non-diagonal and Witt signatures in DoD-10" is mostly done.** WU14 runs every DoD-10 family that does not involve a change of basis over conformal(1) to conformal(4), the Witt forms of one and two modes, the degenerate non-diagonal form, rigid(3) with its null generator first, and the random forms (plan 0029's coverage table). WU12 adds the three families left to it: the isomorphism and round trip, the agreement of results across bases, and the LDLᵀ of the random forms.

R2. **The conformal-duals distinguishing test exists.** CONVENTIONS 1.16 names "the bulk dual of a conformal sphere is a round point" (`test/core/dual_test.cpp`), which the wiki's Duals table fails (research note 0024). What M5 still owes is the owner's confirmation, which section 8's row defers to "phase 3". That is O5.

R3. **The identity suites cannot see a change that keeps each algebra consistent with its own metric.** Every kernel identity reads g from `Alg::value.gram`, and since plan 0031 the generator rule pins each product to the Clifford product of that g. A factory that builds the wrong form, such as conformal(d) with e(d+1) · e(d+2) = +1, or R(p, q, r) with its negative squares first, passes them all. Only a statement that ties two algebras together catches it. Today that is `metric_test.cpp`'s hand values. With this unit it is also the change of basis, which checks P G′ Pᵀ = G. This is the gap that WU12's families extend, and it is what the gap mutants of section 8 target.

R4. **A change of basis is per-basis data composed with the one mechanism (C4, DoD-3).** The image of e_A is the wedge of the images of its generators, and each generator's image is a row of P. So the term list of `change_basis` for a layout is the wedge's term list folded over the rows, an exact composition of the existing mechanism with per-basis integer coefficients and one denominator per output blade. Part D records this in decision 0220. If part C finds that composition costs more than a direct minor expansion, the direct form needs its own C3 measurement and a decision.

R5. **The test-side LDLᵀ is test support, not library code.** Plan 0001 section 5 asks for "a test-side rational LDLᵀ supplying the diagonalizing basis". It lives in `test/support/` and uses `test_support::rational`. Its output is checked by the core's own V4 check, P G′ Pᵀ = G, and by its own tests. It needs no independent route, because a wrong diagonalization is rejected rather than trusted. The diagonal counterpart must be an integer form within `max_form_entry`. Each square d = p/q in lowest terms is scaled to the squarefree part of p·q (−31/2 becomes −62), and its row of P is scaled to match.

R6. **Nothing is dropped, so checking mode has nothing new to check.** The image of a layout is the set of blades the map reaches, and no coefficient is discarded (C8). Converting the image to a named layout goes through `truncate` or the explicit conversion, which keep their checks.

## 2. Goal

Give the core its exact change of basis between two algebras of the same dimension whose forms are congruent over the rationals (C2, decision 0003). This covers the conformal null basis against its diagonal counterpart (CONVENTIONS 1.4), the Witt basis against R(m, m, 0), and every random form against its diagonal LDLᵀ counterpart. Show that it is an algebra isomorphism with an exact inverse, state and pin which operations it preserves and with what factor, and close M5's exit evidence: 1.4 pinned, the core V8 matrix of DoD-16, and the owner's confirmation of the conformal duals.

## 3. Definition of done

- `include/galoot/core/basis_change.hpp` holds the interface of decision 0220. Every precondition of the change that a computation can test is checked when the change is generated (V4, DoD-12), and each rejection has a compile-fail test on both compilers that `tests.messages_pinned` counts as pinned.
- The three "WU12" rows of plan 0029's coverage table are covered at the structure layer (V1 kind 1) and at the kernel layer (V1 kind 2), in `records/mutations/0030-basis-change/coverage.md`, and plan 0029's table points to it.
- CONVENTIONS 1.17 (O4), as ratified, is pinned. The trace gate passes with its new facts, and the only items without a test are still 6.1 to 6.4.
- DoD-16's remaining items (the matrix representations, the Witt CAR relations, conformal(d) ≅ R(d+1, 1, 0)) have tests whose referents a research note records as read and recomputed (V8, V9), unless O1 moves the matrix representations out.
- Research note 0026 records part A's readings, the derivation of section 5.2's statements, and the Sage cross-check of section 7.
- The mutation specs of section 8 have run as each says, and every survivor is explained.
- The build-cost records of W10 exist. The blind review has run and its findings are resolved, and the P9 checks for a merge into `develop` pass.

## 4. Acceptance criteria

Each criterion names its evidence and the mutation spec that judges it. Section 7 gathers them.

1. **Declared and checked at generation (DoD-12, C9).** A change of basis is declared as data under decision 0220's spelling: its source and target algebras and the rows of P as integers, with one positive denominator per row. It is accepted only when source and target have the same number of generators, every denominator is positive, P G′ Pᵀ = G holds exactly (computed in integers as M G′ Mᵀ = Δ G Δ, with M the integer rows and Δ the diagonal of denominators), and det M ≠ 0. A minor or an inverse entry that does not fit `coefficient_type` fails the constant evaluation with a message. Each rejection gives one error on GCC and Clang, its literal message on the first error line and in the terms of the algebras (C9), as `static_assert` or `GALOOT_ASSERT` as S7 asks, and also through `auto r = …;`. The catalogue gains these rows and `tests.messages_pinned` passes. Each concept or trait has an accepting and a rejecting `static_assert` (S7). Judged by `change.toml` (each check removed, the isometry's two sides swapped).
2. **Blade images (definition, 1.4).** For the conformal change of 1.4 with d = 1 to 4, and for its inverse, the image of every basis blade equals `golden.hpp`'s new `basis_changes` namespace. That namespace is computed by `golden.py` from `clifford_reference.py`'s `change_of_basis`, minors over `Fraction`, which shares no code with the library (V1 kind 3). Hand values from the definitions (V1 kind 1): in conformal(3), e4 ↦ ½(e5′ − e4′), e5 ↦ e4′ + e5′, e45 ↦ −e45′, and 𝟙 ↦ −𝟙′. Judged by `change.toml` (a minor's sign, P transposed, a denominator dropped, the grade-0 image dropped).
3. **An algebra isomorphism, at the structure layer (V1 kind 1, per form).** For every signature of `for_each_signature`, paired with its test-side LDLᵀ counterpart (R5), and for the named conformal and Witt changes: φ(e_A ⟑ e_B) = φ(e_A) ⟑′ φ(e_B) for every pair of blades, φ(1) = 1′, every image has its source blade's grade, and φ⁻¹(φ(e_A)) = e_A and φ(φ⁻¹(e′_B)) = e′_B for every blade. The terms come from run-time calls of the change's term generation, so that no template is instantiated per form. Judged by `change.toml` and `gaps.toml`.
4. **An algebra isomorphism, at the kernel layer (V1 kind 2).** On seeded random multivectors over `test_support::rational` through the public functions: `change_basis(x ⟑ y) == change_basis(x) ⟑ change_basis(y)` and the same for ∧, together with the round trip `truncate<S>(change_basis_back(change_basis(x))) == x`. The groups are conformal(1) to conformal(4) against R(d+1, 1, 0); the Witt forms of one and two modes against R(1, 1, 0) and R(2, 2, 0); the degenerate form [[0, 1, 0], [1, 0, 0], [0, 0, 0]] and rigid(3) with its null generator first, each against a diagonal counterpart; and the ten random forms against their LDLᵀ counterparts, on the layouts part A's cost prediction allows (full layouts up to n = 4, conformal(3) full under the raised Clang step limit in its own source as decision 0193 did, and vectors and bivectors for the dense forms of five). Draws are bounded so that the test rational cannot overflow (part A's bound, criterion 12). Judged by `change.toml`, `gaps.toml`, and the seed campaign.
5. **Agreement across bases: what is preserved (V1 kind 1 for the statement, kind 2 for the test).** For each operation that part A's derivation (criterion 13) says a change of basis preserves, with its factor, the test checks φ(op(x, y)) = f · op′(φ(x), φ(y)) on the groups of criterion 4. Section 5.2's hypotheses are the starting point: ⟑, ∧, the dot, the reverse, the antireverse, and the grade involution with f = 1; the antiwedge with 1/det P; the bulk dual with det P; and, only where P⁻ᵀ is also an isometry, as for 1.4's change and the Witt change, ⟇, the weight dual, the antidot, and `transform` with the factors part A states. For conformal(1) to conformal(3) this also covers `transform` of a point by a motor built in the null basis, against the same in R(d+1, 1, 0). Judged by `gaps.toml` and by `naturality.toml`, which drops a factor or replaces det P by its inverse in the test's expected rule, so that each factor is shown to be load-bearing.
6. **Agreement across bases: what is not preserved (C8, V2).** One distinguishing test per basis-dependent operation, with hand values: in conformal(3), the bulk of the origin e4 is −e5, while by way of R(4, 1, 0) it is −½ e5. The right complement and the weight get one value each, and so do ⟇ and the antidot for an LDLᵀ change where P⁻ᵀ is not an isometry. The header comment of `basis_change.hpp` states this list in the terms a user can read, and so does CONVENTIONS 1.8 (O4). Each test fails if the operation were preserved. Judged by `naturality.toml` (the expected inequality turned into an equality).
7. **The test-side LDLᵀ (plan 0001 section 5, R5).** `test/support/diagonalize.hpp`, constexpr over `test_support::rational`, returns T and an integer diagonal D with T G Tᵀ = D after the squarefree scaling of R5, every entry within `max_form_entry`. It handles a zero pivot by a swap, a zero diagonal with a nonzero coupling by pairing f_k + f_j, and the radical. Its tests: the identity on every form of `for_each_signature` (V1 kind 1 per form, exact), and one case of each pivot rule. Judged by `ldlt.toml` (the swap skipped, the pairing skipped, the scaling skipped, which would put a non-integer square in D).
8. **Scalars (DoD-7, V11, V12).** `change_basis` works over `std::int64_t` for a change whose coefficients on the layout are integers, such as a permutation; over the test rational; over `double` and `float`; and over `std::complex<double>`. For a change with a fractional coefficient on the layout, an integer scalar is rejected at compile time with a message (decision 0151's rule extended to division). Over `double`, part N's rule, written before any rounding test runs, fixes each tolerance. With a dyadic change (1.4's) and inputs that are small integers times a power of two, every intermediate is exact, so the bound is zero and the comparison is `==`, with the derivation beside the constant. With an LDLᵀ change whose denominators are not powers of two, the tolerance is the forward bound of one sum of m products and one division, as `test/support/tracked.hpp` counts it (S15). Judged by `change.toml` (the integer rejection removed) and `rounding.toml` (the division moved before the sum, the tolerance's division term dropped).
9. **V8 validation (DoD-16).** (a) conformal(d) ≅ R(d+1, 1, 0) for d = 1 to 4: criterion 3 for 1.4's change, against the published relation of the null pair to the orthonormal pair, read under V9 by part A from Lengyel's conformal wiki (the preferred source for conventions) and from Colapinto's thesis, eqs. 2.39 to 2.42, and Hestenes's "Old Wine in New Bottles", eqs. 1 to 7, with the translation between their normalizations written out. (b) The Witt CAR relations for one and two modes: with c_j = e_(2j−1) and c_j† = ½ e_(2j) in the scaled Witt form, {c_j, c_k†} = δ_jk, {c_j, c_k} = 0, and {c_j†, c_k†} = 0 over the test rational. The same relations hold for the images of the Majorana pairs of R(m, m, 0) under the Witt change, against a published statement of the CAR read under V9. (c) If O1 keeps them: the matrix representations of R(0,1,0) as ℂ, R(0,2,0) as ℍ, R(1,1,0) as M₂(ℝ), R(3,0,0) through the Pauli matrices, and R(1,3,0) through the Dirac matrices. `golden.py` emits each algebra's product table computed by multiplying the published matrices, and checks faithfulness by the rank of the 2ⁿ blade matrices. A C++ test compares `blade_product` with it exactly. The matrices are read under V9 with their sign and index conventions (the knowledge base's `structures/classification-and-periodicity.md` asks that the index convention be fixed first). Judged by `reference.golden` (the regenerate-and-diff test) and by `gaps.toml`.
10. **The Sage cross-check (V1 kind 4, after criteria 1 to 9 pass).** Research note 0026 records the comparison of Sage Route A with `lift_isometry`, at passagemath-modules 10.8.13 with its `uv.lock` hash, against two things. One is `golden.hpp`'s `basis_changes` namespace. The other is a run-time dump of the library's maps for conformal(1) to conformal(3), both Witt changes, the degenerate form, rigid(3) with its null generator first, and the ten random forms' LDLᵀ changes. The dump program lives in math-tools, outside the repository, as research note 0014's did; it is built against the unit's headers through `tools/timed`. The comparison checks that Sage's codomain Gram matrix m⁻ᵀ G m⁻¹ equals the target's, with m = Pᵀ. The expected result is zero mismatches. A disagreement is investigated and recorded (C11), never settled by Sage's value. No Sage value or script path enters `test/`, `include/`, or a comment (C15, S11).
11. **Conventions (DoD-11, O4).** CONVENTIONS 1.17 as ratified: 1.4's wording and the new item 1.8 of section 14. `conventions_check.py` gains checks for 1.4's blade map and antiscalar sign and for 1.8's factors, which carry the label Checked. The trace table gains the new facts with their tests, and the trace gate passes. Section 8's "Conformal duals" row says what O5 rules.
12. **Mutants (amended as decision 0218 item 5 asks).** `records/mutations/0030-basis-change/` holds the specs of section 8, run on gcc-debug and clang-debug. Each new test case kills at least one mutant on both presets. Mutants of `basis_change.hpp` and of the test support are judged by the unit's own labels. For each of the three gaps of section 5.1, a gap mutant counts when the suite the gap extends lets it survive at the branch point and kills it at the tip. That suite is the identity suites of the coverage table at the branch point (`^core\.(identities|identities_rounding|dual_identities|transform_identities)$`), and the same suites plus this unit's new identity programs (`^core\.basis_change_identities`) at the tip. It is not the whole core suite, whose golden tables and `metric_test.cpp` hand values kill every product and factory mutant at the branch point (the reason decision 0217 item 6 gives). Part A predicts, before any run, which gaps can have a counting mutant. Where R3's characterization means none can exist (the LDLᵀ row is the likely case), the criterion records that prediction and its argument as the expected result, not as a failure, and the owner sees it in the outcome.
13. **The derivation first (V1 kind 1).** Before part B writes criteria 5 and 6, research note 0026 holds part A's proof of three statements: that the outermorphism of an isometry is an algebra map of the wedge-basis tables (the knowledge base's functoriality note gives the argument); that φ(𝟙) = det(P) 𝟙′; and of each factor of criterion 5 and each failure of criterion 6, together with the condition under which ⟇, the weight dual, and the antidot are preserved. Part A states the condition after its derivation; the probe suggests "P⁻ᵀ G′ P⁻¹ = G". The blind reviewer recomputes two of the factors by its own route.
14. **Seeds (decision 0216 item 6).** The new randomized programs run with `--rand-seed=1` to `50` on gcc-debug into a campaign log, every run passing. A failure becomes a fixed test (V7) and a defect report (W9). This is the overflow lesson of plan 0029, where seven of fifty seeds overflowed the test rational.
15. **Cost (W10, P1, C14).** The unit changes the core, so it records the build time and memory of the affected sources before and after the change with `tools/compiles cost` on gcc-debug and clang-debug. Those are every source that includes `galoot/core.hpp` (the header-set check, the rga3 preset's tests, the README examples) and the new sources. It also records the test time of the new programs. `basis_change.hpp`'s comment states the measured limits on both compilers at the default limits, as `dual.hpp` does. No bound applies.
16. **Checks.** P8 and P9; every build and test through `tools/timed`; the pool's status read before and after each heavy run; the trace gate, `tools/runlog check`, `format-check`, and lint; no citation of agent records or provisions in code, tests, comments, or build files (S11); and W2's labelled failing commits as section 9 orders them.

## 5. Survey

### 5.1 What WU12 owes, and what exists

| Item | State at `774ff66` | WU12 |
| --- | --- | --- |
| Change of basis is an algebra isomorphism and round-trips (DoD-10) | none (coverage table row "WU12") | criteria 3 and 4 |
| Diagonal and null-basis results agree (DoD-10) | none (row "WU12") | criteria 5 and 6 |
| Test-side rational LDLᵀ for the random forms (plan 0001 section 5) | none (row "WU12") | criterion 7, used by 3 to 6 |
| Exactness of the change of basis checked at generation (DoD-12) | none | criterion 1 |
| 1.4 pinned (M5) | `conventions_check.py` checks the change in Python; no C++ test, since no library code implements it | criteria 2, 3, and 11 |
| Non-diagonal and Witt forms in DoD-10 (M5) | covered for every other family (R1) | the new families run over them |
| Conformal-duals distinguishing test (M5) | named in CONVENTIONS 1.16 (R2) | O5, the owner's confirmation |
| DoD-16: R(0,0,n) product equals wedge; grade counts C(n,k); pseudoscalar square | `blade_product_test.cpp`, `blade_set_test.cpp` | none |
| DoD-16: conformal(d) ≅ R(d+1,1,0); Witt CAR | none | criterion 9 (a), (b) |
| DoD-16: matrix representations | hand products only ("Hand-derived products in … the classical algebras"), no matrices | criterion 9 (c), O1 |

### 5.2 What a change of basis preserves (the probe's hypotheses)

The probe (`records/spikes/wu12-planning-probe/naturality.py`) drew 20 seeded random multivector pairs for each of nine changes: 1.4's conformal change for d = 1 and 2, the same with the other scaling (origin e′3 − e′2, infinity ½(e′3 + e′2)), the Witt change of one mode, a rotation of R(2,0,0) by (3/5, 4/5), a boost of R(1,1,0) by (5/3, 4/3), the null shear e1 ↦ e1 + e2 of R(1,0,1), the permutation from rigid(3) with its null generator first to rigid(3), and the reference's own LDLᵀ of random form 1. Every case had det P = ±1.

| Operation | Preserved, as φ(op) = f · op′(φ) | Cases |
| --- | --- | --- |
| ⟑, ∧, dot, reverse, antireverse, grade involution | f = 1 | all nine |
| ∨ (antiwedge) | f = ±1, equal to det P; predicted 1/det P | all nine |
| bulk dual | f = ±1, equal to det P; predicted det P | all nine |
| ⟇, weight dual, antidot | f = ±1, equal to det P | the seven where P⁻ᵀ G′ P⁻¹ = G; not the shear or the LDLᵀ change |
| right and left complements, bulk, weight | f = 1 (rotation) or det P (permutation) | only the rotation and the permutation (orthogonal P) |

Every case had det P = ±1, so the probe cannot tell det P from its inverse. The predictions are a short argument of mine, not yet checked (marked so under C20). φ(𝟙) = det(P) 𝟙′, because the top minor of P is det P. Then 𝟙 ∨ 𝟙 = 𝟙 gives φ(a ∨ b) = det(P)⁻¹ φ(a) ∨′ φ(b). And a ∧ b★ = (a • b)𝟙, with the wedge and the dot preserved, gives φ(b★) = det(P) φ(b)★′, since the wedge pairing determines b★. Part A proves these and finds the factors of the antiproduct family (criterion 13), and part B tests a change with |det P| ≠ 1 (risk 1).

So the complements, the bulk, and the weight are properties of the coordinates, not of the algebra. In conformal(3) the bulk of e4 is −e5, but computed by way of R(4,1,0) it is −½ e5 (checked by hand and by the reference's route). A user who computes in the diagonal basis and maps back gets a different bulk. C8 asks the interface to say so (criterion 6). 1.4's change preserves the antiproduct family, so `transform` in conformal(d) agrees with `transform` in R(d+1,1,0) (criterion 5). The LDLᵀ changes of the random forms do not, so for those forms only the first three rows can be tested as agreement.

### 5.3 The Sage side

The planning run of `sage_lift_isometry.py` confirmed, for conformal(1) to conformal(3), the translation that research note 0014 and math-tools' `sage-route-a.md` and ga-N13 give. `CliffordAlgebra(QuadraticForm(QQ, 2G))` has e_i e_j + e_j e_i = 2 G_ij. `lift_isometry(m)` with m = Pᵀ sends e_i to Σ_j m[j, i] e′_j, and its codomain has Gram matrix m⁻ᵀ G m⁻¹, here that of R(d+1, 1, 0). The images of the wedge monomials, converted to wedge coordinates by Route A, equal the minors of P, with 0 mismatches over 64, 256, and 1,024 entries, and the lift is a homomorphism on every pair of wedge monomials. The traps the notes record still apply: passing G for 2G, reading grades from Sage's ordered-product coordinates, and the private attribute `_quadratic_form`.

## 6. Interface outline (for part D)

This is the planner's recommendation, to be settled by part D (cpp-expert with hci-expert judgement) and recorded in decision 0220 before any test is written (O3).

```cpp
struct cga3 { static constexpr auto value = galoot::core::conformal<3>(); };
struct r410 { static constexpr auto value = galoot::core::R<4, 1, 0>(); };

// A change of basis is a tag, as an algebra is: diagnostics name it, and its
// checks run once per tag and translation unit.
struct cga3_to_r410 {
  using source = cga3;
  using target = r410;
  static constexpr auto value = galoot::core::conformal_change<3>();
};

auto y = galoot::core::change_basis<cga3_to_r410>(x);       // an mv of r410
auto z = galoot::core::change_basis_back<cga3_to_r410>(y);  // an mv of cga3
```

- The value is an aggregate of integer rows M and positive row denominators Δ, so P = Δ⁻¹ M (O2). A general change is written `from_rows({{…}, …}, {d1, …, dn})`. The 1.4 change is the factory `conformal_change<D>()`, because 1.4 is a core convention and its pinning needs library code.
- The inverse is computed by the core, exactly and in integers (adj(M) Δ over det M, reduced by row), so a user who knows P need not invert it by hand. The round trip of DoD-10 then tests that computation.
- Each term coefficient is an integer, and each output blade has one positive denominator, the least common multiple of its terms' denominators. The output coefficient is the integer combination of the inputs divided once by that denominator. A scalar needs `/` only when some output denominator on the layout is not 1. An integer scalar is rejected then.
- The result layout is the canonical set of blades reached, with orientation +1, as for a product (`dual.hpp`'s rule). It is named `change_basis_layout<C, S>`, a public name under decision 0187's rule.
- `basis_change.hpp` joins `galoot/core.hpp`.

## 7. Evidence and judges (V1, V10, V12)

| Criterion | Claim | Evidence (V1 kind) | Tolerance (V12) | Judged by |
| --- | --- | --- | --- | --- |
| 1 | invalid changes are rejected at generation | compile-fail tests, both compilers; `tests.messages_pinned` | none (compile time) | `change.toml` |
| 2 | blade images are the minors of P | golden `basis_changes` from `clifford_reference.py` (3); hand values (1) | exact (V11) | `change.toml` |
| 3 | φ is an isomorphism with inverse φ⁻¹, every form of `for_each_signature` | exhaustive over blade pairs and blades (1, per form) | exact | `change.toml`, `gaps.toml` |
| 4 | the same through `mv` on random multivectors | seeded identities (2); 50-seed campaign (V7, a sample) | exact | `change.toml`, `gaps.toml` |
| 5 | the listed operations agree across bases with their factors | part A's proof (1); seeded identities (2) | exact | `gaps.toml`, `naturality.toml` |
| 6 | complements, bulk, weight (and, for non-dual-isometric P, the antiproduct family) differ across bases | hand values (1); part A's proof (1) | exact | `naturality.toml` |
| 7 | the test-side LDLᵀ diagonalizes every listed form | exact check per form (1); pivot cases | exact | `ldlt.toml` |
| 8 | scalars: exact where exact, bounded where rounding | rejection test; part N's derivation, written first | zero for dyadic changes on dyadic inputs; γ-bound of one sum and one division otherwise | `change.toml`, `rounding.toml` |
| 9 | V8: conformal ≅ diagonal, Witt CAR, matrix representations | referents read under V9 and recomputed (V8); golden matrix tables (3) | exact | `reference.golden`, `gaps.toml` |
| 10 | Sage agrees | black-box comparison of outputs (4), after 1 to 9 | exact | none: a record, not a test |
| 11 | 1.4 and 1.8 as ratified | `conventions_check.py` (3); trace gate | exact | trace gate |
| 12 | the tests can fail | mutation records | n/a | itself |
| 13 | the statements are true | paper proof in note 0026 (1), reviewed | n/a | blind review |

Validation (V8): criterion 9 is the unit's validation. Section 5.2's statements for the forms not covered by criterion 9 are verification of the specification's own mathematics, and the V10 table says so. Results from an uncommitted step are labeled not reproducible (V10).

## 8. Mutants (`tools/mutate`)

Specs under `records/mutations/0030-basis-change/`, run on gcc-debug and clang-debug. Where a `static_assert` of the tests could catch a mutant, they also run in `--runtime` mode. Every gap mutant is confirmed as a survivor at the branch point before it counts.

- **`change.toml`**, judged by `^core\.basis_change`: a minor computed with one row swap uncounted (a lost transposition sign); columns of P used for rows; the denominator never applied; an output's denominator taken from its first term only; the inverse computed as the transpose; the isometry check written as P G Pᵀ = G′; the isometry check removed; the invertibility check removed; the grade-0 image dropped; the integer-scalar rejection removed; the checked arithmetic of the minors made unchecked.
- **`gaps.toml`**, judged by the suite each gap extends (criterion 12): `conformal_pairing_positive` (`conformal()` sets e(d+1) · e(d+2) = +1); `diagonal_negative_first` (`R()` puts its −1 squares before its +1 squares); `witt_pairing_doubled` if part A finds a factory or a test form it can hit. Each is invisible to the identity suites by R3 and should be killed by criteria 3 and 4, through the core's isometry check or through the homomorphism. For the LDLᵀ row, part A writes its prediction (criterion 12) before any run.
- **`naturality.toml`**, edits of the test's own expected rules, as the WU14 blind review's `test_antiproduct_sign_dropped` was: each factor of criterion 5 set to 1, det P replaced by its inverse in the factor of ∨, and each inequality of criterion 6 made an equality. A correct library must fail each wrong rule, which shows the rule is load-bearing.
- **`ldlt.toml`**, judged by `^support\.diagonalize$`: the pivot swap skipped, the pairing step skipped, the squarefree scaling skipped.
- **`rounding.toml`**, judged by the floating cases of criterion 8: the division applied to each term before the sum, the division term dropped from the tolerance, and every floating result times 1 + 2⁻⁴⁸ (the WU14 reviewer's probe of tightness).

## 9. Parts and order

Tests come before the code they judge. Under W2 as widened by decision 0216 item 8, a run of test commits that do not yet build or pass may precede the one commit that makes them pass, each saying so and naming its failing test. Changes to existing tests' expected values go in their own listed commits (W9).

0. **Claim** (coordinator): cherry-pick this plan into `develop` after the owner's answers, and record them in decision 0219.
1. **Part D, the interface** (cpp-expert with hci-expert judgement, Opus high, on `--math` or a records branch, in parallel with A): decision 0220 with the spelling of section 6 as O3 rules, the declarations with their `///` comments as text, and the misuse rows the catalogue will gain. It also maps `change_basis` to the term-list mechanism (R4, DoD-3). It applies the usability checklist of the `blind-reviewer` role file to the proposal. The owner sees decision 0220 before part B starts, if O3 asks for that.
2. **Part A, the mathematics** (math-expert, Opus high, `feature/core-basis-change--math`), in this order:
   - (a) The derivation of criterion 13, written in note 0026.
   - (b) The V9 readings of criterion 9: Lengyel's conformal wiki for 1.4; Colapinto and Hestenes as the knowledge base cites them; a published statement of the CAR; and the Pauli and Dirac matrices with their conventions, if O1 keeps them.
   - (c) `golden.py`'s `basis_changes` namespace, and `representations` if O1 keeps it, regenerated into `golden.hpp` with `// Fact:` lines.
   - (d) `conventions_check.py`'s checks for 1.4 and 1.8.
   - (e) The draft text of CONVENTIONS 1.17 with labels earned by (d).
   - (f) The cost prediction of each test layout from `snippets/plan_cost_model`, and the overflow bound of the test rational for each group's draws (criterion 4).
   - (g) The gap-mutant predictions of criterion 12, committed before part E runs `gaps.toml`.
   - (h) Then, after criteria 1 to 9 pass on the unit, the Sage cross-check of criterion 10.

   Every brief for a part that fetches a web source says that the owner's email address is never sent (not as a contact address, in a user agent, or in a query), and that no third party is contacted. The math-expert adds to the knowledge base a note on what a change of basis preserves (with the status tag its derivation earns), commits nothing there unless its brief says to, and lists the note in its report.
3. **Part N, the tolerance rule** (numerical-expert, Opus high, beside A): criterion 8's rule, derived from `tracked`'s counts and written in the record before any floating test runs.
4. **Part B, the tests** (test author, doctest-expert, Opus high, `feature/core-basis-change--tests`), after plan 0031's merge and after D, A (a) to (f), and N. One commit per group, each labelled failing until part C lands:
   - (i) the test-side LDLᵀ and its tests (criterion 7), which pass when committed;
   - (ii) the V4 rejections (criterion 1) and the blade images (criterion 2);
   - (iii) the structure layer (criterion 3);
   - (iv) the kernel groups (criterion 4), in sources split by cost as part A predicts;
   - (v) agreement and its failures (criteria 5 and 6);
   - (vi) scalars (criterion 8);
   - (vii) V8 (criterion 9).

   The test author picks layouts within part A's prediction. A layout that does not build goes to the implementer rather than being weakened.
5. **Part C, the implementation** (implementer, cpp-expert judgement, Opus high, `feature/core-basis-change`): cherry-pick B's commits with `-x`; `basis_change.hpp`; `core.hpp`; the CTest registrations and labels (`core.basis_change`, `core.basis_change_identities`, `support.diagonalize`; `slow` only if measured); the trace rows; the catalogue regenerated; the coverage table. Then decision 0221 for the implementer's choices. The commit that adds the header makes B's commits pass.
6. **Part E, the evidence** (implementer): the mutants of section 8 at the branch point and the tip, the seed campaign (criterion 14), the costs (criterion 15), and the records. Then part A (h).
7. **Review** (section 11), then fixes, a delta review if core code changed after it, `develop` merged in, and the P9 checks.
8. **Merge** (coordinator), with P13's line in the message, and CONVENTIONS 1.17 committed as the owner ratified it.

## 10. Checks (P8, P9, P13, W10)

| Point | Checks | Presets |
| --- | --- | --- |
| Part A, before each commit | `reference.golden`, `tests.golden_facts`, `conventions_check.py`, the trace gate | gcc-debug |
| Part B, each group | the group's labels, `-L '^tests\.'`, lint of the changed targets; a labelled failing commit runs what can build | gcc-debug, then clang-debug for the sources under the raised step limit |
| Part C, before each commit | `-L '^(core\.basis_change|support\.diagonalize|tests\.)'`, the compile-fail tests of the new messages, the header-set check | gcc-debug and clang-debug |
| Merge of `--math` into the unit | the P8 selection of everything it changed | gcc-debug, clang-debug |
| Merge into `develop` | the full suite on each preset from new trees (gcc-debug, gcc-release, gcc-asan, clang-debug, clang-release, clang-asan, lint, clang-measure), `format-check`, the trace gate, `tools/runlog check` | all eight, as `gate:all` |

Every build and test runs through `tools/timed` (P13), with `eval "$(tools/jobpool env)"` first. `tools/jobpool status` is read before and after each heavy run: the mutants, the seed campaign, the cost campaign, and the P9 matrix. The pool is started only if it reads "not running", never twice, and never stopped. W10: the unit changes the core, so criterion 15's before-and-after record is owed.

## 11. Review plan (W5)

- **Blind review** by the `blind-reviewer` agent at the Opus level with high effort, in a configuration that shares no context with the authors. It gets the diff from the branch point, section 4, and the outcome's departures, but not the authors' view that the change is correct, and it reads the outcome only after its own checks (decision 0215 item 6). It applies the usability checklist, since a public interface is added. It recomputes, by its own route, two factors of criterion 5, the hand values of criteria 2 and 6, and one LDLᵀ change. It runs the catalogue scan on both listings, writes and runs its own `tools/mutate` spec of at least five mutants against the new tests, and reads the new sources' compile cost and the note's Sage rows. Its record is cherry-picked from `records/review-wu12`.
- **The math-expert's check** of the test author's markers against note 0026, before the review, as a reader and not a co-author.
- A fix to code after the review gets a delta review. A comment-only change is listed in the review record instead (decision 0183).

## 12. Compile and run cost: estimate and risk

The estimates are inferred from the WU14 cost campaign (`records/performance/wu14-identities-cost/`) and the comments of `dual.hpp`. None is measured for this unit.

| Item | Estimate | Risk |
| --- | --- | --- |
| Generating a change's terms | Σ_k C(n, k)² minors at most: 252 at n = 5, 924 at n = 6, each a k × k integer determinant. The 1.4 change has at most two terms per blade. Small beside a full<5> product plan (1,024 pairs) | low; a dense P at n = 16 (C(16, 8)² minors at grade 8) is not tested, and the header states its measured limit |
| The inverse | one integer adjugate, n ≤ 16 | low; overflow fails compilation with its message |
| `basis_change_test.cpp` (V4, images, scalars, V8) | 3 to 6 s and 0.4 to 0.6 GB per compiler, like `dual_test.cpp` (3.3 to 3.8 s) | low |
| Kernel identities (criteria 4 and 5) | each group instantiates the products of two algebras; like `dual_identities_test.cpp`, 10.5 to 10.8 s and 0.5 to 0.8 GB | **medium**: conformal(3) full needs Clang's raised step limit (decision 0193), and the dense forms of five do not build full products on Clang (`dual.hpp`). Mitigation: per-group sources and layouts from part A's prediction; split any source over about 10 s, as decision 0189 did |
| Structure layer (criterion 3) | run-time loops over the 42 forms of `for_each_signature` (44 after plan 0031), 2²ⁿ pairs each; under 1 s at -O0, a few seconds under ASan | low; `slow` only if measured |
| Compile-fail tests | about 8 to 12 messages × 2 compilers × 0.5 to 1 s | low |
| Parse cost added to every `core.hpp` includer | one header of a few hundred lines; expected under 50 ms per source | low; measured by criterion 15 |
| Test rational overflow | dense LDLᵀ denominators (up to −875/48 at n = 5) times random draws | **medium**; plan 0029's seed campaign found 7 of 50 seeds overflowing. Mitigation: part A's bound (criterion 4) and the campaign (criterion 14) |
| Mutants | about 25 mutants; those of `basis_change.hpp` rebuild the unit's sources (about 1 to 2 minutes each per preset); the gap mutants rebuild the identity suites as well (about 3 to 5 minutes each per preset) | 1.5 to 3 hours of pool time on two presets, spread over part E |
| Seed campaign | 50 runs × the new programs, about 1 s each | low, about 5 minutes |
| Sage cross-check | about 2 s for conformal(1) to (3) and under a minute for every form; 107 MiB | low |
| P9 matrix | about 20 to 25 minutes, as for WU14 | low |

## 13. Risks

1. **The probe's hypotheses are wrong in a case it did not try.** Every probed change had det P = ±1, so it cannot tell det P from its inverse for the antiproduct family. Mitigation: part A's derivation comes first (criterion 13), and part B tests at least one change with |det P| ≠ 1 (an LDLᵀ change with scaled rows, or 1.4's change with a rescaled Euclidean generator).
2. **The integer-with-denominators form proves awkward to write.** Users think of P with rational entries. Mitigation: part D weighs it against a small structural fraction literal in the interface, not in the arithmetic (O2), and the usability checklist judges it.
3. **Common-mode error.** The math-expert writes the golden values and the derivation, and the test author writes the tests. Sage (criterion 10) is the independent route, and the blind reviewer recomputes by a third. Plan 0001 risk 5 still applies.
4. **Compile cost** (section 12). It is mitigated by the predictions and the splits. A source that cannot build at the default limits gets its own source under a stated limit, never a narrower test.
5. **A defect found in existing code.** The change of basis is the first test that ties two algebras' factories together (R3). A wrong factory found this way is reported (W9) and fixed here only if small and in scope.
6. **Plan 0031 slips.** Parts B and C wait for it (departure 7). If it stalls, the coordinator may let WU12 register its sources in `test/CMakeLists.txt` after agreeing the hunk with plan 0031's implementer. That is still a W7 departure, which the owner would hear about.

## 14. Proposals for the owner (not edited here)

CONVENTIONS 1.17. Part A finalizes the text after its checks. The labels below are the planner's targets and are earned only by part A's checks.

- 1.4, reworded (the owner decides its wording at M5): "`conformal(d)` has e1 to ed Euclidean, e(d+1) the origin, and e(d+2) the point at infinity, with both of the last two null and e(d+1) · e(d+2) = −1. Its diagonal counterpart is R(d+1, 1, 0), whose generators are written e′1 to e′(d+2): e′(d+1) squares to +1 and e′(d+2) to −1. The change of basis from conformal(d) to R(d+1, 1, 0) (1.8) keeps e_i = e′_i for i ≤ d and writes the origin as ½(e′(d+2) − e′(d+1)) and the point at infinity as e′(d+2) + e′(d+1). Its determinant is −1, so e(d+1)(d+2) maps to −e′(d+1)(d+2) and 𝟙 to −𝟙′. (Checked)" The change is that the two bases' generators get distinct names, and that the blade map and the antiscalar's sign are stated.
- 1.8, new: "A change of basis from an algebra with Gram matrix G to one with Gram matrix G′ on the same number of generators is an invertible rational matrix P whose row i writes e_i in the generators e′_j, with P G′ Pᵀ = G. It sends e_A to Σ_B det P[A, B] e′_B over the blades B of A's grade, so it keeps grades, and its inverse is the change by P⁻¹. It preserves the geometric product, the wedge, the dot, the reverse, the antireverse, and the grade involution; it sends 𝟙 to det(P) 𝟙′, the antiwedge to det(P)⁻¹ times the antiwedge of the images, and the bulk dual to det(P) times the bulk dual of the image. The geometric antiproduct, the weight dual, the antidot, and the transform are preserved, with factors [part A], exactly when [part A's condition; the probe suggests P⁻ᵀ G′ P⁻¹ = G], as for 1.4's change. The complements, the bulk, and the weight depend on the basis and are preserved only by an orthogonal P: in conformal(3) the bulk of e4 is −e5, while by way of R(4, 1, 0) it is −½ e5. (Checked for the listed forms; the condition Proposed until part A's proof)"
- Section 8, "Conformal duals": "confirmed by the owner in phase 3" becomes the date and words of O5's ruling.
- No governance amendment. Criterion 12's reading is the one plan 0031's O1 asks the owner to word into the governance; this plan follows that reading and does not edit it.

## 15. Open questions for the owner, with recommendations

- **O1. The matrix representations of DoD-16.** M5's exit evidence lists "core V8 matrix". The Pauli, Dirac, quaternion, complex, and M₂(ℝ) representations are test-only work: a golden namespace, one C++ test, and a V9 reading, sharing no file with the change of basis beyond `golden.py`. Options:
  - (a) Keep them in WU12 (criterion 9 (c)). M5 then closes with all its evidence. It costs part A about 40k to 60k tokens and part B one test group.
  - (b) Move them to a small unit of their own after WU12. WU12 stays narrower, but M5 cannot close until that unit merges.

  Recommended: (a).
- **O2. How exact fractions are represented, and WU5.** Options:
  - (a) Integer rows with a positive denominator per row; integer term coefficients with one denominator per output blade; no rational type in the core. Close WU5 by decision.
  - (b) A core rational type (WU5) for the change's coefficients.
  - (c) One common denominator for the whole matrix.

  (a) keeps every generated coefficient an integer (C7, decision 0101), adds no arithmetic type (C12), and makes the V4 checks integer identities. Its cost is a less familiar input form; part D may add a fraction literal for input only. (b) is the most familiar to write and adds about 150 lines of core code and tests, with a present user. (c) is the simplest data, but a grade-k image then carries the denominator d^k before reduction. Generation forms larger intermediates than under (a), and it still needs the per-blade reduction that (a) does. Recommended: (a), with WU5 closed.
- **O3. The interface of section 6.** A change of basis as a tag carrying source, target, and value; `change_basis<C>(x)` and `change_basis_back<C>(y)`; the inverse computed by the core; `conformal_change<D>()` and `from_rows` as factories; `change_basis_layout`; the header in `core.hpp`. The alternatives are a function taking the matrix directly (no tag, so diagnostics show the whole matrix and checks repeat), a user-supplied inverse verified by the core (less code, but every user inverts P by hand), and no named conformal factory (1.4 then has no library code to pin). Recommended: section 6 as written, with the final spelling from part D's decision 0220. You confirm the names after part D, before part B starts, as you did for the operation names (decision 0187).
- **O4. CONVENTIONS 1.17 (section 14).** It covers 1.4's wording (yours to decide at M5) and the new 1.8. The alternative is to leave 1.4 as it stands, which uses one set of names for two bases, and to state the preserved operations only in the header comment. C10 wants one written specification per concept, and the basis-dependence of the bulk is a convention a user can trip on. Recommended: ratify 1.17 after part A's checks earn its labels, with the bracketed parts filled in by part A.
- **O5. Confirm the conformal duals.** Section 8 of CONVENTIONS records the choice "3.3, the definition; confirmed by the owner in phase 3". The poster agrees with the definition, the wiki's Duals table negates four rows, and a C++ test tells the two apart (R2). Confirming closes M5's item. Declining would mean adopting the table's signs, against the wiki's own definition and the poster, and changing 3.3. Recommended: confirm.
- **O6. The reading of "Sage Route A is WU12's independent oracle"** (departure 3). Options:
  - (a) Sage as the kind-4 black-box cross-check, with outputs compared and recorded in note 0026, and expected values coming only from `test/reference/`.
  - (b) Sage's values committed as golden values.

  (b) would make Sage supply expected values, which C15 forbids; it would need your recorded departure from C15. Its gain over (a) is small, since the Python route already computes the same minors from the definitions. Recommended: (a).
- **O7. Rational forms in DoD-1** (departure 4). Options:
  - (a) Integer Gram matrices only, with rational forms reached through a rescaling change of basis, and DoD-1's release statement written that way.
  - (b) Rational Gram entries in `metric`.

  (b) touches every header and every plan, and its present user is only the canonical Witt normalization, which (a) handles with c† = ½ e2. Recommended: (a), recorded in decision 0219.
- **O8. The basis-dependent operations.** Options:
  - (a) Document and pin them (criterion 6, 1.8).
  - (b) Also offer variants computed in another basis, such as a bulk relative to a change.

  (b) has no present user (C12). Recommended: (a).
- **O9. Seeds for WU12's randomized programs.** Decision 0216 item 6 asks for the 50-seed campaign at WU14, at each gate, and at the release, and WU12 is not a gate. Running it here costs about 5 minutes and caught an overflow in WU14. The alternative is to wait for the M5 gate's campaign. Recommended: run it in WU12 (criterion 14), and again at the gate.

## 16. Agent cost estimate

| Part | Agent | Model, effort | Estimate |
| --- | --- | --- | --- |
| D | cpp-expert with hci-expert judgement | Opus, high | 100k to 180k tokens |
| A | math-expert (derivation, readings, golden, conventions check, predictions, Sage) | Opus, high | 250k to 400k |
| N | numerical-expert | Opus, high | 60k to 100k |
| B | test author (doctest-expert) | Opus, high | 350k to 500k |
| C | implementer (cpp-expert judgement) | Opus, high | 250k to 400k |
| E | implementer (evidence) | Opus, high | 150k to 250k |
| Review | blind reviewer | Opus, high | 150k to 250k |
| Delta review, if needed | blind reviewer | Opus, high | 50k to 120k |
| Coordination | coordinator | Opus, high | 60k to 100k |

Size M to L: about 1.4M to 2.3M tokens with O1 (a). Machine time, inferred from section 12: about 2 to 3.5 hours of pool time, most of it mutants. This planner's own cost is recorded by the coordinator from the harness report.

## 17. Out of scope, and notes for later units

- A machine-checked proof of criterion 13's statements through math-tools' proof harness is not planned (C12). The paper proof in note 0026 is the kind 1 evidence; a later unit may add one if the owner wants it.
- Composition of changes (φ_BC ∘ φ_AB = φ_AC, by Cauchy–Binet) has no present user and is not offered.
- Diagonalization in the core: the LDLᵀ stays test support (R5). A user who needs it is a reason to reconsider.
- The cga3 preset (M6, after v1) is the first expected user of `transform` agreeing across bases. That agreement is tested here only on the core's terms.
- Plan 0031's O2 (the generator rule at the structure layer) is not affected.

## 18. Outcome

Planned. Nothing is implemented.
