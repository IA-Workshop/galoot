# 0001 Core v1 roadmap

- Status: accepted by the owner on 2026-10-31, all of D1 to D17, with amendments to D4, D5, D6, D11, D13, and D16. Each is recorded in `docs/agent/decisions/` under the number of the same order (D1 is 0001, and so on). Where a decision record and section 7 differ, the record governs.
- Written by: Claude (agent), 2026-10-31, merging two independent plans: an architecture and critical-path plan (Fable) and a definition-of-done and gates plan (Opus). Neither plan was checked against the repository line by line by the author of this merge; claims marked "computed" come from the architecture planner's read-only spikes, which are not yet committed as `test/reference/` scripts and so are not V1 evidence (V10).
- Agent cost: the Opus planner used about 141k tokens over 14 tool calls. The Fable planner's cost was not reported. Estimates for later units belong in each unit's plan (W3).

## 1. Departures and ambiguities (C20)

1. Roadmap source. W11 names the design study's phased roadmap as current. This plan replaces its core content (phases 1 to 4) with the milestones below. That is a roadmap change and needs the owner (D1).
2. "Core v1" is read as `galoot::core` 1.0.0 (W13: each component is versioned). Models ship beside it at 0.x. C2 makes models extensions, so they are not part of the 1.0.0 claim, but the core cannot be shown to meet C5 and V8 without at least one.
3. C2 says basis elements multiply to "a single basis element times an exact coefficient". In a non-diagonal stored basis this is false: in `conformal(3)`, e4 e5 = −1 + e45 (computed). Both planners read C2 as covering such forms through its clause on exact changes of basis. That reading needs a decision record (D3).
4. Governance status. CONSTITUTION.md and PERFORMANCE.md still say "draft for adoption". A definition of done that cites drafts can move when they are edited (D2).

## 2. Where the two plans disagree, and the recommendation

| Topic | Architecture plan | Gates plan | Recommendation |
| --- | --- | --- | --- |
| Coefficient type | Integers in product plans; rationals only in the change of basis. The design study's "dyadic" claim is wrong in general (computed: random integer Gram matrices give integer tables, while diagonalization gives non-dyadic squares such as 3/2) | Form entries exact rationals, change of basis supplied | Integer plan coefficients, rational change of basis and form entries. Spike B settles it before WU6 |
| Collected plans for repeated inputs (design study phase 4) | Not in 1.0; waits for measurement (C3) | In 1.0, because C6 and P4 require them | In 1.0 only if Spike E and the P2 run show composed plans violate C6 (cancelling components kept). Otherwise defer by decision. Decide at gate G3 on evidence (D8) |
| Models in the release train | `rga3` only; `conformal(3)` tested as a signature in core | rga2, rga3, cga2, cga3 (types and 6.1 to 6.3) | `rga3` required. cga3 types are an optional milestone M6 (D7) |
| Largest n | `metric<N>` up to 32, tested to 8, practical limit measured | 1 ≤ n ≤ 16 | Accept n ≤ 16 for v1 and test sparse use at 8, 12, 16. Raise later (D4) |
| `unitize` and norms | In v1 with a `sqrt_scalar` concept | Out of v1 | Squared norms in v1. Square-root norms and `unitize` wait for the scalar concept (note 0006, item 15). Smaller core (C12) |
| Plan generator at run time | Two-pass `consteval` | `constexpr`, callable at run time, so exhaustive proofs need not be `static_assert`s | `constexpr`. Spike A confirms the constant-evaluation limits before WU6 |
| Error messages | C++26 `static_assert` text is unavailable under the S4 flags (computed on Clang 23); use constrained catch-all overloads with literal messages | Compile-fail tests check the `static_assert` text | Both: catch-all overloads whose bodies carry literal messages, checked by compile-fail tests on both compilers |

Design-study content both planners flag as wrong or ungoverned: "dyadic coefficients"; `static_assert` messages that print blade names; `basis_names` inside `metric` and a fixed 16×16 matrix in every type; the `algebra<M, T>` layer (no present user); the fourteen-repository split and graded signatures in the first phases (C12, CONVENTIONS 9); the "GCC 15+ / Clang 19+" floor (not installed; the machine has GCC 16.2 and Clang 23.1); "within 10% of Terathon" and "fail on compile-time regressions" (only the owner sets bounds, C14; P7 compares and reports); "masks up to 32" against a 16×16 matrix.

## 3. Core v1 scope

In (headers under `include/galoot/core/`): assertions and checking mode; bits (grade, reorder sign, complement and involution signs); metric (symmetric form, factories `R`, `rigid`, `conformal`); oriented blade sets with the 1.6 name parser and 1.7 order; the one term-list mechanism (`blade_product`, plans, unary maps, complement conjugation, memoization); `mv` and the scalar concepts; gp, antigp, wedge, antiwedge, dot, antidot and the operators of 7.1; reverse, antireverse, grade involution, left and right complements; bulk, weight and their duals; exact change of basis (conformal(d) to R(d+1,1,0) among others); projection, antiprojection, `transform`, `compose`; a lossy conversion under its own name (`truncate<Blades>`, S8); `format.hpp` outside the umbrella header.

Out (notes, not code, C12): fused symbolic plans unless D8 admits them; graded signatures and everything in design-study Tier 1 and later; exp, log, inverse, square-root norms, unitize; contractions and expansions; reflection, `mdspan`, SIMD lane packs; a general outermorphism API; models other than `rga3` (and optionally cga3 types); the multi-repository split; a precompiled or cached table.

## 4. Definition of done for core 1.0.0

Numbered criteria from the gates plan, merged. Each is checkable. "Ref script" is `test/reference/`, which shares no code with the library (V1 kind 3).

**Scope and interface**
- DoD-1 (C2). Accepts diagonal R(p,q,r) for 1 ≤ n ≤ 16 and symmetric rational forms given with an exact change of basis. Rejects n out of range and graded signatures at compile time. The release record states the class covered.
- DoD-2 (C12, C5). The public inventory is exactly the list in section 3. Each entry names its CONVENTIONS section, tests, documentation page, and present user. An entry with no user is removed.

**Generation**
- DoD-3 (C3, C4). A decision record maps every operation to "the term-list generator composed with per-basis data". No hand-written or cached table, or each has a C3 decision with measurement.
- DoD-4 (C6, P5). Each generated result's blade set equals the blades with a surviving term. For repeated-input operations, structurally cancelling components are absent (if D8 admits collected plans) or the declared-output check holds. Every named type has `sizeof == components × sizeof(scalar)` as a `static_assert`.
- DoD-5 (C5). The model, a test-only exact rational scalar, and one test-only model (for example STA) are added with no diff under `include/galoot/core/`, shown in the review record.
- DoD-6 (C13, S6). An include scan finds only standard and core headers and no cycle. Every header compiles alone. A user file compiles with only `-std=c++23 -pedantic-errors -I include`.

**Exactness and scalars**
- DoD-7 (C7). Plan coefficients have an exact type with no floating-point type or literal in plan data. One plan passes with int64 where integral, the test rational, float, double, and `std::complex<double>`. An inadequate scalar is rejected with a message in the terms of the algebra.
- DoD-8 (V11). Exact-scalar tests use `==` only.
- DoD-9 (V12). Each rounding-scalar tolerance is a named constant derived before the result is seen, from the plan's forward error bound or an exact reference. A tolerance change is a decision record. Presets declare floating-point options, including `-ffp-contract`.

**Verification**
- DoD-10 (V3, V1). The identity suite runs over the signature set in section 5 in two layers. Structure is checked exhaustively on blade pairs and triples for n ≤ 6 (kind 1). Kernels are checked with seeded randomized exact-rational multivectors against the ref script (kind 2), with sparse use at n = 8, 12, 16. Families: associativity of gp, antigp, wedge, antiwedge; the four De Morgan laws; left∘right = id; u ∧ ū = 𝟙; reverse and antireverse involutions and their product rule; e_i e_j + e_j e_i = 2g_ij; a ∧ b★ = (a•b)𝟙; G𝔾 = det(g)·I; u★★ = 0 exactly when degenerate; change of basis is an algebra isomorphism and round-trips; diagonal and null-basis results agree.
- DoD-11 (V6, C10, C11). Every implemented CONVENTIONS item has a C++ test with a golden value from a header generated by the ref script. Every section 8 row the core touches has a test that tells the alternatives apart. A script checks traceability from convention ID to test.
- DoD-12 (V4). Symmetry and range of the form, exactness of the change of basis (PᵀGP = D), unique valid blade names, ±1 oriented signs, in-range unduplicated nonzero plan terms, u ∧ ū = 𝟙 for each complement sign, and the 1.7 result order are checked at generation, each with a rejection test.
- DoD-13 (V5). One test file per component, in V5 order.
- DoD-14 (V2). Every `TEST_CASE` carries a reference marker, enforced by script. A recorded fault-injection check shows the suite catches a flipped reorder sign, a flipped complement sign, a dropped term, a wrong coefficient, and a swapped 1.7 order.
- DoD-15 (V7). One seed source, printed on failure, varying signatures as well as values; each failure kept as a fixed test.

**Validation**
- DoD-16 (V8, core). Independent matrix representations preserve the product table for Cl(0,1)≅ℂ, Cl(0,2)≅ℍ, Cl(1,1)≅M₂(ℝ), Cl(3,0) (Pauli), Cl(1,3) (Dirac); R(0,0,n) gp equals wedge; Witt-basis CAR relations; grade counts C(n,k); the pseudoscalar square from a derivation in a research note; conformal(d) ≅ R(d+1,1,0).
- DoD-17 (V8, models). `rga3`: every Checked value of CONVENTIONS section 5; point groups closed under compose with orders from the International Tables (families realizable over ℚ only; hexagonal and trigonal need √3 and are deferred or run under V12); Denavit–Hartenberg chains against independent 4×4 matrices; incidence. Each source is cited only after a research note records it as read and recomputed (V9).
- DoD-18 (V10). The release record has a status table for every convention family, identity family, V8 case, and performance claim, with revision, command, and seed.

**Explicitness and errors**
- DoD-19 (C9, S7). A ratified misuse catalogue (mixed algebras, unknown blade, duplicate blade, n out of range, graded signature, asymmetric form, implicit lossy conversion, non-scalar type, odd+even versor in `transform`) has a compile-fail test on both compilers, matching the literal message.
- DoD-20 (C8). Conversions between blade sets are not implicit. Checking mode is selected explicitly by `GALOOT_CHECKING` in every preset. In checking mode a discard of a nonzero part aborts (a `WILL_FAIL` test).
- DoD-21 (S9). The suite builds with `-fno-exceptions`; results that are a function's only effect are `[[nodiscard]]`.

**Cost (track-only, C14)**
- DoD-22 (P1). Per-translation-unit wall time and peak memory on both compilers for the release revision, and before and after for each core unit (W10).
- DoD-23 (P2). A growth benchmark: fixed use against n ∈ {4, 8, 16}, and fixed n against 1, 4, 16 distinct operations, with compiler instruction counts beside wall time. Growth at fixed use is interpreted under D5.
- DoD-24 (P4, P6, V15). For rga3 point∧point, point∧point∧point, line∨plane, transform(point, motor), transform(line, motor): a counting scalar shows operation counts equal the ref script's collected-term counts; a recorded read of the `-O2` assembly shows no blade branches, calls, or allocation; a pinned benchmark against an in-repo straight-line form derived from the mathematics.
- DoD-25 (P7, P13). The performance history baseline is committed. `runs.tsv` keeps, through every compaction, the rows P13 says a compaction keeps, including every run named `gate:<selection>`, and every other gate run is counted in the history and retrievable from version control.

**Style, documentation, records, release**
- DoD-26 (S3 to S5). clang-format and clang-tidy clean; the S4 warnings pass on GCC 16 and Clang 23; the suite passes under ASan+UBSan; TSan recorded as not applicable.
- DoD-27 (W12). README quickstart, a core guide, and a model guide. Every fenced C++ example is extracted, compiled, and run as a test on both compilers. User documentation contains no process records.
- DoD-28 (C18, W3, W5, W6). Decision records for every item in section 7; plans closed with outcomes; a W5 review record for every merge into `develop`; the auditor's release audit has no blocking finding; lessons recorded.
- DoD-29 (W13, C20). The release record states core 1.0.0, the CONVENTIONS version implemented, the model versions, the V10 table, and that every check ran on the released revision. No open blocking defect. The owner merges to `master` and tags.

## 5. Signature set for the identity suite

- Diagonal, n ≤ 4: R(1,0,0), R(0,1,0), R(0,0,1); R(2,0,0), R(0,2,0), R(1,1,0), R(1,0,1), R(0,0,2); R(3,0,0), R(2,1,0), R(2,0,1), R(1,1,1), R(0,0,3); R(4,0,0), R(3,0,1)=rigid(3), R(1,3,0), R(3,1,0), R(2,2,0), R(2,0,2), R(0,0,4).
- Diagonal, n = 5 and 6: R(4,1,0), R(4,0,1), R(4,2,0), R(3,3,0). Sparse only: R(8,0,0), R(6,6,0), R(15,0,1), R(8,8,0).
- Non-diagonal: conformal(1) to conformal(4); Witt bases with one and two fermion modes; a degenerate non-diagonal form such as [[0,1,0],[1,0,0],[0,0,0]]; a diagonal form entered as a general matrix with its null generator first; seeded random small-integer symmetric forms with n ≤ 5, with a test-side rational LDLᵀ supplying the diagonalizing basis.
- Seeded random R(p,q,r) with n ≤ 6.

## 6. Milestones and gates

Every gate requires: a report that opens with departures (C20) and carries a V10 table; an auditor run on the gated `develop` revision; a W5 review record for every merge in the milestone; `runs.tsv` entries; a snippets coalescing review in `docs/agent/snippets/reviews/` (decision 0021); owner acceptance recorded as the outcome here (W11). A gate that fails twice with no new understanding halts under W8. Work units (WU) come from the architecture plan and are listed in section 8.

| # | Milestone | Work units | Exit evidence | Owner decides |
| --- | --- | --- | --- | --- |
| M0 | Harness and phase 1 closure | WU0, WU1 | Governance ratified; decision records for the toolchain matrix, doctest (with C13 continuity record), CMake 4.4, presets, and golden-value format; build, presets, lint, format check; per-TU time and memory launcher; ref script emits a committed golden header and a CTest regenerates and diffs it; traceability, reference-marker, and include-scan scripts; a doctest run of zero test cases fails; Spikes A to D and F recorded as research notes | D1 to D6, D9 |
| M1 | Bits, metric, blade sets | WU2, WU3, WU4, WU5 | Exhaustive basis-rule proofs (n ≤ 8); 1.1, 1.2, 1.3, 1.5, 2.1, 3.1 pinned; V4 checks with rejection tests; first P1 baseline | Core interface for signatures and blade sets |
| M2 | Plan generator and `mv` | WU6, WU7, WU8 | DoD-10 structure proofs over the diagonal set and the non-diagonal tables checked against the ref script; 1.7, 4.1, 4.2 pinned; DoD-7 across five scalars; first P2 run; first assembly read | Signature set (section 5) |
| M3 | Operation layer | WU9, WU10, WU11 | 3.2 to 3.4, 4.3, 7.1 pinned; dual-sign distinguishing test; checking mode; V12 double tests with forward bounds | Operation inventory, API names |
| M4 | `rga3` model as data | WU13, WU15, WU16 | Zero core diff; 1.6 and 5.1 to 5.7 pinned; flector distinguishing test; rigid V8 cases | Sufficiency of V8 cases |
| M5 | Non-diagonal forms and change of basis | WU12 (runs beside M4 on different files, W7) | Non-diagonal and Witt signatures in DoD-10; 1.4 pinned; isomorphism and round trip; conformal-duals distinguishing test; core V8 matrix | Conformal-duals confirmation; wording of 1.4 |
| M6 (optional) | cga3 named types | after M4 and M5 | Zero core diff; 6.1 to 6.3 pinned; conformal V8 cases | D7 |
| M7 | Cost evidence and collected plans | WU14 (identity suite grows from M3), WU17 | DoD-22 to DoD-25; Spike E decides whether collected plans enter 1.0 | D8 |
| M8 | Release candidate and v1 | WU18 | Full suite on every configuration; DoD-27 docs; V10 table; auditor release audit; owner walkthrough | Owner merges to `master` and tags (W13) |

Phase mapping from the design study: phase 1 is M0; phase 2 is M1 to M4; phase 3 is M5 and M6; phase 4 is M7 (collected plans only; SIMD lane packs are extension scalars). Replacements for design-study gates: the "golden-value test file" becomes the generated golden header plus regeneration check; "GCC 15+/Clang 19+" becomes the installed matrix, with no untested minimum claimed; "within 10% of Terathon" becomes DoD-24 and is recorded, not gated; "compile-time budgets that fail CI" becomes the P7 history with a comparison rule stated in advance; black-box cross-checks against other libraries are optional and late (V1 kind 4).

## 7. Decisions for the owner

Recommendations are given for each. Each is recorded as a C18 decision when made.

| # | Decision | Recommendation |
| --- | --- | --- |
| D1 | Adopt this plan as the roadmap in W11 and mark design-study phases 1 to 4 as superseded | Yes |
| D2 | Ratify CONSTITUTION.md and PERFORMANCE.md and update their status lines | Yes, before M0 closes |
| D3 | Read C2 as covering non-diagonal forms via exact change of basis; blades in a non-orthogonal metric are wedge monomials (CONVENTIONS 1.1 wording) | Yes; leave C2's text as is and record the reading |
| D4 | Size limit | n ≤ 16 in v1; mask type `std::uint32_t`; no bound asserted beyond what Spike A measures; n = 0 rejected |
| D5 | Interpret P2 so growth at fixed use of at most O(n²) (the signature data itself) is admitted, measured in instruction counts over at least three sizes; the P7 report criterion is set in advance at twice the historical spread | Yes (amends P2's strict wording) |
| D6 | Toolchain matrix: Linux x86-64, GCC 16.2 and Clang 23.1, libstdc++; MSVC unsupported; no untested minimum claimed | Yes |
| D7 | Whether cga3 named types ship with 1.0 (M6) and whether the 6.4 operations wait | Types optional in M6; 6.4 and the partner question wait until after v1 |
| D8 | Collected plans for repeated inputs under C4 as an exact post-pass | Decide at M7 on Spike E and P2 evidence |
| D9 | CONVENTIONS 1.3 with component markers and the parity rule for `transform` moved into section 4; the name `truncate` for the lossy conversion | Yes |
| D10 | Checking-mode meaning for rounding scalars: discarded magnitude within the plan's forward error bound | Yes; exact zero for exact scalars |
| D11 | Test-only exact rational scalar, owned, overflow-trapping int64; no GMP or Boost | Yes |
| D12 | Compile-time failure helper in `consteval` code that avoids `throw` (S9), one STYLE sentence | Yes |
| D13 | No run-cost or build-cost bounds in v1 | Yes (C14) |
| D14 | Owned benchmark harness, about 100 lines, no dependency (C13) | Yes |
| D15 | Plan term lists stay in `detail` in v1 | Yes |
| D16 | Single repository with module directories; defer the split | Yes |
| D17 | Release as 1.0.0 with no intermediate tags | Yes |

## 8. Work units and critical path

| # | Unit | Depends on | Size |
| --- | --- | --- | --- |
| WU0 | Decision records: toolchain, `mask_t` and N, wedge-monomial blades, integer coefficients, layout, this roadmap, build choices, doctest | Owner answers | S |
| WU1 | Build skeleton: CMake 4.4, presets, doctest runner, `galoot_add_test` with labels and zero-tests failure, compile-fail test function, per-TU time and memory launcher, `assert.hpp` | WU0 | M |
| WU2 | `bits.hpp` with exhaustive tests against definitions | WU1 | S |
| WU3 | `metric.hpp` and factories, generation-time validity checks | WU1 | S |
| WU4 | `blade_set.hpp`, name parser, 1.7 order | WU2 | M |
| WU5 | `detail/rational.hpp`, overflow fails compilation | WU1 | S |
| WU6 | `blade_product`: the one mechanism, from the defining relations, integer arithmetic; ref script `clifford_reference.py` (Gram–Schmidt over `Fraction`) | WU3, WU4, Spikes A and B | M |
| WU7 | `plan.hpp`: result typing, unary maps, complement conjugation, memoization | WU6 | M |
| WU8 | `mv.hpp`: concepts, aggregate, `apply`, `truncate`, `get<"e41">`, exact equality; test exact scalar | WU7, Spike F | M |
| WU9 | gp, wedge, `*`, `^` | WU8 | S |
| WU10 | Involutions, complements, antiproducts; De Morgan laws | WU9 | M |
| WU11 | Bulk, weight, duals, dot, antidot | WU10 | M |
| WU12 | `basis_change.hpp` and its V4 check | WU5, WU11 | M |
| WU13 | `transform.hpp`: compose, transform, project, antiproject (exact rotor 3/5, 4/5 for V11) | WU11 | M |
| WU14 | Identity and conventions suites, compile-fail catalogue | WU10 to WU13 | M |
| WU15 | `format.hpp`; catch-all overloads with literal messages | WU8, Spike C | S |
| WU16 | `rga3` model and its V8 cases; README example compiled as a test | WU13, WU15 | M |
| WU17 | Cost evidence: P2 grid, P4 assembly reads, benchmark harness, first performance records, Spike E | WU13, WU1 | M |
| WU18 | Release prep: close plans, record versions, auditor run | all | S |

Critical path, one lane, each unit merged to `develop` before the next branches: WU0 → WU2 → WU4 → WU6 → WU7 → WU8 → WU9 → WU10 → WU11 → WU13 → WU16 → WU18.

Parallel lanes in separate worktrees (W7): the build lane (WU1, later WU17) with `build-expert`; the spike lane in the workbench (W11 permits running ahead); the metric lane (WU3, WU5); the verification lane (WU12 and WU15 beside WU13; WU14 accumulating). A shared job-slot server (P11) must run before builds can follow the governance.

## 9. Spikes

| # | Question | Retires before |
| --- | --- | --- |
| A | Compile time, memory, and constant-evaluation limits for gp plans at n = 4, 5, 6, 8 and the rga3 typed pairs, on both compilers; value versus tag-type template arguments | WU6 |
| B | Commit `clifford_reference.py`; confirm integer tables for conformal(3), R(1,3), random integer Gram matrices | WU6 |
| C | Five misuse cases on two compilers; confirm the catch-all overload pattern | WU15 |
| D | Memoization form: variable template, class static, or function-local; count evaluations for 1 and 16 uses | WU7 |
| E | Composed `transform` against a hand-written straight-line form (P6 method) | M7 |
| F | Fixed-string template argument for `get<"e41">` on both compilers, including the missing-name diagnostic | WU8 |

## 10. Risks to this plan

1. C1's "smallest, maximally performant, user-friendly" is not measurable. DoD-2, DoD-24, and DoD-27 are proxies, and the final judgment is the owner's walkthrough.
2. P2 as written forbids any growth with dimension and may be unmeetable without D5.
3. C9's "not in terms of internal machinery" is only partly measurable; compilers still print instantiation stacks. The owner samples diagnostics at the gates.
4. If the generator ends up `consteval`-only, the exhaustive proofs must run at compile time and build cost may be prohibitive. Spike A decides.
5. Common-mode error: the spec author and the ref-script author are the same kind of agent, so V1 kind 3 independence is weaker than it looks. Kind 2 identities and the owner's review of the ref script mitigate it.
6. Exact-rational overflow in randomized n = 6 products must trap as a test failure, not wrap.
7. Checking mode on rounding scalars (D10) puts error-bound logic in the core, against C12; a wrong bound gives false aborts.
8. The point-group referent needs a source read under V9 and covers only ℚ-realizable families.
9. The Terathon side of the dual-sign and partner disagreements stays "not tested" under C15 and C16. The record keeps saying so.
10. No older compilers are installed, so the toolchain floor cannot be tested.

## 11. Next step

The decision records of WU0 are written (0001 to 0017). Remaining in M0: ratify the status lines of CONSTITUTION.md and PERFORMANCE.md (decision 0002), edit CONVENTIONS to 1.3 (0009), claim WU1 with its own plan, and start the spike lane (section 9) in the workbench. WU1 needs the shared job-slot server of P11 running.

## Addendum, 2026-11-01: after the M0 and M1 gate

Decision 0094 closed M0 and M1. WU5 (exact rational) moved from M1 to M5, ahead of WU12 (the change of basis). Section 11 above is stale: the next unit is the build-lane change, then M2 (WU6 `blade_product` and the plan, WU7 plans, WU8 `mv`).

## Addendum, 2026-11-07: after the M2 and M3 gates

M2 was accepted by the owner on 2026-11-02 (decision 0163). M3 (operation layer: WU9, WU10, WU11) was accepted by the owner on 2026-11-07 with the auditor's recommendations, with B1, MA1, and MA2 of the gate report stated (`records/gates/2026-11-04-M3.md`, decision 0182). The owner decisions of M3 are recorded in decisions 0182 to 0192: the operation inventory and the sixteen `*_layout` names are public interface (0187), and `dot` and `antidot` return multivectors (0191). Next: the fix of B1 (0184) and the split of `identities_test.cpp` (0189), then WU13 (plan 0021) in M4.

## Addendum, 2026-11-08: after the M4 gate

M4 (the `rga3` algebra as data: WU13, WU15, WU16, with the rotation-normalize and presets units) was accepted by the owner on 2026-11-08 with the auditor's findings and the rulings of decision 0215 (`records/gates/2026-11-08-M4-audit.md`, `records/gates/2026-11-08-wu6-wu8-audit.md`). The sufficiency of the V8 cases was accepted with its limits stated (decision 0215 item 3). The rga3 algebra now lives under `include/galoot/presets/` in namespace `galoot::presets::rga3` (decision 0213), and named algebras are layers on the core, with the cga3 preset (M6) shipping after v1 (decision 0211). Next: the evidence tools (plan 0026), the pool and runlog repairs, WU14, WU17 and M5 (WU12).
