# 0020 WU11: bulk, weight, duals, dot, antidot, and M3's rounding tests

- Status: planned 2026-11-02 (planner: Claude, agent, Opus 5.5, planner role with math-expert, cpp-expert, and hci-expert judgement; implementers and reviewers to be named by the coordinator at branching). Not started. WU11a implemented on `feature/core-duals` 2026-11-04, ready for the blind review (section 10).
- Branches, from `develop` at `344a0ff` or later (section 1, departure 1):
  - WU11a: `feature/core-duals`, worktree `.claude/worktrees/core-duals`. The test author works on `feature/core-duals-tests`, worktree `.claude/worktrees/core-duals-tests`, branched from `feature/core-duals` at its first commit (section 4, "Commit order and W2"; departure 5 explains the name).
  - WU11b: `feature/core-rounding`, worktree `.claude/worktrees/core-rounding`. It changes only test files and can run beside WU11a. Its last group waits for WU11a's merge.
- Decision numbers 0171 to 0175: 0171 to 0174 in WU11a, 0175 in WU11b (section 2).
- Source of scope: plan 0001 (WU11, "Bulk, weight, duals, dot, antidot", size M, after WU10; M3, "3.2 to 3.4, 4.3, 7.1 pinned; dual-sign distinguishing test; checking mode; V12 double tests with forward bounds"; WU13, "compose, transform, project, antiproject"); CONVENTIONS 3.2 to 3.4, 4.1, 4.3, 4.5, 7.1 and section 8 ("Dual sign"); plan 0019 (structure, reading R3, R5, the Outcome); decisions 0159 to 0170; the reviews `records/reviews/2026-11-02-*.md`; the post-WU10 audit `records/gates/2026-11-02-post-wu10-audit.md` (B1, Info 3, Info 6, owner items 4, 5, 7, 9); lesson 0067; the third handoff.
- Goal: the public bulk and weight, their right and left duals, the dot and the antidot of CONVENTIONS 3.2, 3.3, and 4.1, built as compositions of the public products, maps, and complements; the projection of 4.3 if the owner agrees (question O1); the dual-sign distinguishing test of section 8; and, in a second part, the V12 rounding tests with forward error bounds that M3 needs, with a statement of what M3's "checking mode" covers.

## 1. Departures, ambiguities, and owner questions (C20)

### Departures

1. **Two parts.** Plan 0001 lists WU11 as one unit of size M. The roadmap row holds eight public functions with their result layouts and messages. M3 also needs V12 rounding tests that no unit owns (plan 0019 R5, audit Info 6). Those tests cover every M3 operation, not only WU11's, and change only test files. So:
   - WU11a, the roadmap row: `bulk`, `weight`, `bulk_dual`, `weight_dual`, `left_bulk_dual`, `left_weight_dual`, `dot`, `antidot`, their layouts and messages, the golden values they need, and `project` if O1 is answered yes.
   - WU11b: the V12 double tests with forward bounds for every M3 operation, a test-only tracking scalar that gives each result its bound, and the checking-mode inventory for the M3 gate.

   WU11b touches no file of WU11a except `test/CMakeLists.txt` (separate blocks; whichever merges second merges `develop` first, W7). Its tests of the WU11 operations are its last group and wait for WU11a's merge. If the coordinator prefers one branch, the commits of section 4 are the same, in the same order.
2. **`project` moves from WU13 to WU11a, if O1 is answered yes.** Plan 0001 puts `project` and `antiproject` in WU13 (M4) but asks for 4.3 to be pinned at M3, which holds only WU9 to WU11. The two rows disagree. `project` is a one-line composition of WU11's `weight_dual` with WU9's and WU10's products, and it needs no transform machinery. `antiproject` has no definition in CONVENTIONS, so it stays in WU13, whose plan proposes the 4.3 amendment that defines it.
3. **`golden.py` gains one namespace.** WU10 composed its expected values from existing golden values. The bulk and the dot need the metric exomorphism G, which the golden header does not hold. G could be composed from the golden product tables, as ⟨e_A ẽ_B⟩₀, but that is the same identity WU11a uses for `dot`, so it would not be an independent route. The new namespace holds G computed from Gram minors and checked inside `golden.py` against a second route (section 5).
4. **Planning probes, not reproducible as committed (V10).** These statements rest on probes in the planner's scratch area, against a copy of `include/` taken at `7729c91` (the headers are the same at `344a0ff`, which changed only records), on g++ 16.2.1 and clang++ 23.1.4, each run through `tools/timed --log` into a scratch log:
   - `identities.py`: the composition identities of reading R4, exact over `Fraction`, on every blade and every blade pair of nine forms (rigid(3), conformal(2), conformal(3), the degenerate form [[0, 1, 0], [1, 0, 0], [0, 0, 0]], seeded random form 1, R(1,1,0), R(2,1,1), R(1,3,0), and the Witt form [[0, 1], [1, 0]]). It reads `test/reference/clifford_reference.py` for the geometric product and computes G from Gram minors. No failure.
   - `compose.hpp` with `limits.cpp` and `limits2.cpp`: the compositions of R4 written with today's public functions, compiled at `-O0` and `-O2` (section 1, reading R10).
   - `asm.cpp` and `restricted.cpp`: `-O2 -ffp-contract=off` assembly of the compositions (reading R5).
   - Instruction counts of `limits2.cpp` at n = 4 and n = 16, one run each, pinned to CPU 2 without checking that it is a performance core (P14). Indicative only.

   The implementer repeats each as a test or a committed probe before relying on it.
5. **W7's subfeature form cannot be used.** W7 names subfeature branches `feature/<name>/<part>`. Git refuses that name while `feature/<name>` exists ("cannot lock ref 'refs/heads/feature/x/part': 'refs/heads/feature/x' exists"; probed in a scratch repository). No unit has used the form, so nothing broke. This plan names the test author's branch `feature/core-duals-tests`. The defect in W7 goes to the owner (W9), noted in the outcome; it is not fixed here, because W7 is ratified text.

### Readings

R1. **Scope and names.** The roadmap row and CONVENTIONS fix the names:
   - `bulk(x)` and `weight(x)` (3.2);
   - `bulk_dual(x)`, `weight_dual(x)`, `left_bulk_dual(x)`, `left_weight_dual(x)` (3.3 names the left ones);
   - `dot(a, b)` and `antidot(a, b)` (4.1; 7.1 says they have no operator);
   - `project(a, b)`, the projection of a onto b, b ∨ (a ∧ b☆) (4.3), with the object first as in `transform(x, V)` (4.4), only after O1.

   No operator is added (7.1). No norm functions are added: square-root norms are out of v1 (plan 0001 section 3), and squared norms need no name of their own while `dot(x, x)` and `antidot(x, x)` give them (C12). `antiproject` waits for WU13 (departure 2).

R2. **Where the dot and antidot belong.** The brief asked whether `project`, `dot`, and `antidot` "of 4.3" belong here or to WU13. The dot and the antidot are defined in 4.1, not 4.3, and the WU11 row names them, so they are WU11's without question. Only `project` is in doubt (departure 2, O1).

R3. **Result layouts follow rule R3 of plan 0019 (decision 0165).** None of the new operations sends every blade to itself times a sign: the bulk and the weight drop blades in a degenerate metric and move them in a non-diagonal one (in conformal(3), G e4 = −e5). So every one returns the canonical set of the blades it reaches, with orientation +1, in order of grade and then of index list:
   - `bulk(line)` in rigid(3) holds e12, e13, e23, and the moment on e31 appears negated on e13; `Line{bulk(l)}` reads it back under the line's orientations.
   - `bulk_dual(point)` holds e124, e134, e234, and `Plane{bulk_dual(p)}` is the plane (x, y, z, 0).
   - `dot(a, b)` holds the scalar 1 when a and b share a grade on which G is not zero, and no blade otherwise. So `dot(point, line)` has an empty layout: its type says it is zero. `antidot` holds 𝟙 likewise.

   Decision 0165 weighed the alternative of keeping the input's orientations for the part of the input that the bulk keeps. It would keep `get<"e31">(bulk(l))` working in rigid(3), but it needs a second rule that holds only in diagonal metrics. Not taken.

   Each function declares its result through a public layout variable template, as decision 0166 did for the products, so that Clang names the operation and not `detail::` or `product_plan` on the first line: `bulk_layout<Alg, S>`, `weight_layout`, `bulk_dual_layout`, `weight_dual_layout`, `left_bulk_dual_layout`, `left_weight_dual_layout`, `dot_layout<Alg, A, B>`, `antidot_layout`, and `project_layout` after O1. That is nine new public names. The audit's owner item 7 asks whether the seven existing `*_layout` names stay public. WU11a follows the ruling: if the owner moves them to `detail`, WU11a's names go with them, in the same commit as the move or after it; if the ruling is still open when WU11a starts, WU11a adds its names in the existing form, and a later move covers all sixteen at the same cost.

R4. **Every operation is a composition of existing public functions (C4).** The composition test: each operation below is a sum of products of its inputs with exact coefficients that the one mechanism already generates. None needs a new plan or map generator.

   | Operation | Definition (CONVENTIONS) | Composition | New code beyond the call chain |
   | --- | --- | --- | --- |
   | `bulk_dual(x)` | right(G x) (3.3) | `geometric_product(reverse(x), 𝟙)` | the unit antiscalar of the algebra, a one-blade mv |
   | `bulk(x)` | G x (3.2) | `left_complement(bulk_dual(x))` | none |
   | `weight_dual(x)` | right(𝔾 x) (3.3) | `bulk(right_complement(x))`, since right(𝔾 x) = G right(x) | none |
   | `weight(x)` | 𝔾 x = left(G right(x)) (3.2) | `left_complement(weight_dual(x))` | none |
   | `left_bulk_dual(x)` | left(G x) | `left_complement(bulk(x))` | none |
   | `left_weight_dual(x)` | left(𝔾 x) | `left_complement(weight(x))` | none |
   | `dot(a, b)` | aᵀ G b (4.1) | the scalar part of `geometric_product(a, reverse(b))` | a kernel that keeps only the plan's terms on the scalar (R5) |
   | `antidot(a, b)` | aᵀ 𝔾 b 𝟙 (4.1) | `left_complement(dot(right_complement(a), right_complement(b)))` | none beyond `dot` |
   | `project(a, b)` | b ∨ (a ∧ b☆) (4.3) | `antiwedge(b, wedge(a, weight_dual(b)))` | none |

   The identities behind the table, each checked by `identities.py` on every blade or blade pair of the nine forms of departure 4, with no failure:
   - x̃ ⟑ 𝟙 = right(G x). Sketch: the product of any multivector with 𝟙 is its left contraction onto 𝟙, since every generator lies in 𝟙. For a vector, e_i ⟑ 𝟙 = e_i ⌋ 𝟙 = Σ_p g_ip right(e_p) = right(G e_i), since right(e_p) is (−1)^(p−1) times the blade of the other generators. For a blade, (a ∧ A') ⌋ 𝟙 = a ⌋ (A' ⌋ 𝟙), and v ⌋ right(Y) = right(Y ∧ G v), so induction on the grade gives right(G Ã). Decision 0172 writes the proof out in full (math-expert role), and the exhaustive tests of section 5 check it on every golden form.
   - right(𝔾 x) = G right(x), from 𝔾 = left G right and right ∘ left = identity.
   - ⟨e_A ẽ_B⟩₀ = det g[A, B], the Gram determinant of the two blades, zero across grades. So the scalar part of a ⟑ b̃ is aᵀ G b, with e12 • e12 = +1 in a Euclidean metric, where the scalar part of e12 ⟑ e12 is −1.
   - aᵀ 𝔾 b = right(a)ᵀ G right(b), because the right complement is a signed permutation of the blades, whose inverse, the left complement, is its transpose. And left(c) = c 𝟙 for a scalar c.
   - Also checked, for the tests: G 𝔾 = det(g) times the identity; 𝔾 = right G left as well as left G right; u★★ = (−1)^(gr·ag) det(g) u; a ∧ b★ = (a • b) 𝟙 for a and b of equal grade; and a ∧ left(G b) = (−1)^(gr·ag) (a • b) 𝟙. These agree with the knowledge base note `geometric-algebra/duals-and-metric-exomorphism.md`.

   Weighed and rejected, in decision 0172:
   - A generator of the metric exomorphism from Gram minors, one map per layout. It is the definition read literally, and it would cost one generation per layout instead of three. But it is new machinery, which C4 admits only when composition cannot express the capability, and composition can. C3 allows a replacement later, on measurement.
   - The antiproduct routes: weight_dual(x) = x̰ ⟇ 1 and antidot(a, b) = the antiscalar part of a ⟇ b̰. Both are exact (checked by `identities.py`), but they inherit the antiproduct's limit on Clang: the antidot through ⟇ of two `full<5>` values of conformal(3) fails at `-O2` (probe `limits.cpp`, case 4), the failure of review F1 and audit B1, while the route through the complements builds (case 3). They also inherit decision 0141's growth with n.

R5. **The dot keeps only the terms it needs (P4).** Written as `part<scalar>(geometric_product(a, reverse(b)))`, the dot generates and runs the whole product and then drops all but one coefficient. Probe `asm.cpp` (`-O2 -ffp-contract=off`), for the dot of two rigid(3) motors, whose value needs 4 multiplications and 3 additions:
   - GCC 16 inlines the kernel and removes the dead terms: 4 `mulsd` and 3 `addsd`.
   - Clang 23 keeps the 48-term product kernel out of line, calls it, and computes all 48 products (48 `mulsd`) before it reads one.

   A careful person would not write the second form (P4). Probe `restricted.cpp` builds the dot from the same product plan, with a kernel that keeps only the terms whose output is the scalar: 4 multiplications and 3 additions on both compilers, no call. That kernel is per-basis data (a mask on the plan's outputs) composed with the existing plan, the same kind of data `identity_kernel` and `dropped_positions` already are, so it is no new machinery under C4. It lives in `detail` of the new header, its only user, so `mv.hpp` is not touched. Decision 0172 records the probe and the choice. The other compositions need no such kernel: `bulk(line)` and `weight_dual(point)` compile to moves and sign flips, and `project(point, plane)` to 21 multiplications, the hand count, on both compilers with no call (GCC's only calls are to `__stack_chk_fail`).

R6. **The dot and the antidot return multivectors.** CONVENTIONS 4.1 says "the dot a • b = aᵀGb is a scalar, and the antidot a ∘ b = aᵀ𝔾b multiplies 𝟙". "A scalar" can mean a number of type T or the grade-0 element of the algebra. Readings:
   1. Both return multivectors, on {1} and {𝟙} or empty (R3). Recommended. It is the existing idiom: `p & g` of a point and a plane already returns an mv on {1}, read with `get<"1">`. The antidot's 𝟙 stays visible (C8: nothing hidden). The De Morgan relation right(antidot(a, b)) = dot(right(a), right(b)) is one equation of multivectors. And an empty layout tells the user at compile time that the dot is identically zero.
   2. `dot` returns T and `antidot` an mv on {𝟙}: the most literal reading of 4.1, but the two are no longer each other's mirror.
   3. Both return T: the antidot's 𝟙 is then hidden, which C8 asks the name to state.

   WU11a builds reading 1. The owner decides API names at the M3 gate (plan 0001 section 6), and question O4 asks for the confirmation there. Changing it before release is a listed test change (W9).

R7. **The diagonal sentence of 3.2 omits a sign.** "In a diagonal metric, the bulk keeps the blades whose square is nonzero." The first sentence of 3.2 defines the bulk as G applied, and G multiplies e_A by the product of its generators' squares. In R(1,1,0), bulk(e2) = −e2: the blade is kept, with its coefficient negated. The two sentences agree in rigid(d), where every nonzero square is +1. WU11a follows the definition, as the ratified first sentence. Question O2 proposes wording for version 1.9.

R8. **The bulk and the weight are not always complementary parts.** In rigid(d) every blade is in exactly one of the two, with its coefficient unchanged, so bulk(x) + weight(x) = x. Elsewhere they are not:
   - in R(n, 0, 0) both are x;
   - in conformal(3), 𝔾 = −G (research note 0005, C1), so the weight is the negated bulk;
   - with two or more null generators, as in R(2,0,2), a blade holding some but not all of the null generators, such as e3, is in neither.

   The comments say so, and the tests pin each case. The cga3 model's four parts (6.3) are the model's own masks, not the core's bulk and weight, and `dual.hpp`'s comment says so for conformal(3).

R9. **Degenerate metrics.** In R(0,0,n), G keeps only the scalar and 𝔾 only the antiscalar. So `bulk` of a value without a scalar part has an empty layout, `weight_dual(x)` is x's antiscalar coefficient on the scalar, the dot is a₀b₀, and every double dual is zero by type. In the degenerate non-diagonal form, the bulk moves blades (G e1 = e2). Both cases are in the golden values or the hand values of section 5.

R10. **Clang's constant-evaluation limits for the compositions.** Probes `limits.cpp` and `limits2.cpp`, at `-O0` (where libstdc++ defines `_GLIBCXX_ASSERTIONS` by itself) and `-O2` (where it does not, the configuration of audit B1):

   | Case | g++ -O0 | g++ -O2 | clang++ -O0 | clang++ -O2 |
   | --- | --- | --- | --- | --- |
   | `bulk`, `weight`, `dot`, `antidot` (complements), u★★ of `full<5>`, conformal(3) | builds | builds | builds | builds |
   | `antidot` through ⟇, the same | builds | builds | builds | fails |
   | `bulk`, `weight` of `full<5>`, the form with 2 on the diagonal and 1 elsewhere | builds | builds | builds | builds |
   | `bulk` of `full<6>` and `full<7>`, R(n,0,0) | builds | builds | builds | builds |
   | `bulk` of `full<8>`, R(8,0,0) | builds | builds | fails | fails |
   | `dot` of `full<6>`, R(6,0,0) | builds | builds | fails | fails |

   The `full<8>` failure is the reverse map's own limit, which `plan.hpp` already states (1,251,192 steps at n = 8). The `full<6>` dot fails where the geometric product of `full<6>` fails. Both are the limits of the parts, not new ones. Folding the reverse sign into the operand's layout signs would remove the reverse map from `bulk_dual` (a sign on each blade, per-basis data), but it reads less plainly, so it waits for a measured need (C12). The implementer measures the largest `full<n>` for each new operation on both compilers, with and without `_GLIBCXX_ASSERTIONS`, and the dense five-generator forms of the identity suite, and states the limits in `dual.hpp`.

   Growth at fixed use, indicative (departure 4): the weight of a rigid(3) point embedded in R(15,0,1) costs the whole translation unit 2.8% more instructions on GCC and 7.6% more on Clang than in rigid(3), and the bulk 0.6% and 1.0%. The n = 16 figure includes the per-algebra check of the larger metric. The weight's growth comes from the complements, whose grades grow with n, as the antiproduct's does (decision 0141). The implementer measures it pinned (P14), for the four rigid(3) types at n = 4 and n = 16, and records it beside 0141's figures.

R11. **Messages (plan 0016 reading 21; decision 0166).** One error, the operation as written, the mistake in the algebra's terms, the fix after a semicolon, no final period or quotes. Each public spelling checks for itself, before it calls anything, that the scalar holds every exact coefficient of its own composed plan, so a narrow scalar never reaches the inner `geometric_product`'s message (review F5) or `apply:`:
   - narrow scalar, one each for the nine functions: "bulk: the scalar type cannot hold one of the bulk's exact coefficients; use a scalar type that holds larger integers exactly", and likewise for `weight`, the four duals, `dot`, `antidot`, and `project`;
   - mixed operands, for the three binary functions: "dot: the operands are in different algebras; give both types the same algebra tag" and "dot: the operands have different scalar types; give both types the same scalar type", and likewise for `antidot` and `project`.

   That is 15 messages, 12 without `project`. They are pinned by `expect_output` compile-fail tests on both compilers, each also in the form `auto r = ...;`, with `apply:`, `geometric_product:`, `left_complement`, `product_plan`, and `detail::` forbidden on the first error line. Each binary catch-all returns the first operand's type, and `dual.hpp` has its own rejecting function for its three spellings, so `product.hpp`'s spelling list stays as it is. The narrow-scalar checks of `dot` and `antidot` read only the terms they keep (R5), so a scalar that holds the dot's coefficients but not the whole product's is accepted. Each narrow-scalar test uses a layout on which the checked plan differs from the alternatives that a mutant could check instead (review F2). For example, in diag(100, 100) over `std::int8_t`, bulk(1) = 1 fits and weight(1) = 10,000 does not, and on {e12} the reverse holds.

R12. **Name collisions and first-time traps.**
   - `weight`, `bulk`, and `dot` are common words. A variable of that name in scope stops an unqualified call, as for `part` (WU10b F7c). The comments say so.
   - C++26's `std::execution::bulk` is a function object (libstdc++ 16 lists it in its `std` module). Under `using namespace std::execution`, it would hide `bulk` from argument-dependent lookup, as `std::ranges::reverse` hides `reverse` (plan 0019 R6). The comment says so; no workaround.
   - `dot` is Lengyel's dot, aᵀ G b. In a Euclidean metric it gives e12 • e12 = +1, where the scalar part of the geometric product, which many libraries call the inner or scalar product, gives −1. The comment states this, and a test pins it.
   - `weight(p)` of a point is a multivector on {e4}, not the number w; `get<"e4">(weight(p))` reads it.

R13. **Headers (decision 0171).** Recommended: one new header, `include/galoot/core/dual.hpp`, for the nine functions, their layouts, the restricted kernel, and the rejecting function. `core.hpp` includes it. It includes `product.hpp` and `unary.hpp`, because the compositions call both, so neither existing header changes. Alternatives weighed in 0171:
   - Unary operations in `unary.hpp` and binary ones in `product.hpp`: `unary.hpp` would include `product.hpp` for `bulk_dual`, and `product.hpp` would include `unary.hpp` for `dot`'s reverse, which is a cycle.
   - Calls of `detail::apply` instead of the public functions, so that the existing headers could hold them: then the operations are no longer plain compositions a reader can check against the table of R4.

   Cost: a program that includes only `product.hpp` or `unary.hpp` pays nothing. Every includer of `core.hpp` pays one more header's parse, about +0.7% of the smoke unit if it is like `linear.hpp` (plan 0019 Outcome), measured as P2.

R14. **The checking mode at M3 (C8, decision 0010, decision 0168).** None of WU11's operations assumes that something it drops is zero:
   - the bulk and the weight drop blades by definition, which decision 0168's reading of C8 (ratified) covers: an operation whose purpose is to discard assumes nothing;
   - the dot and the antidot compute one coefficient, the definition's, and the restricted kernel is a way of computing it, not a discard from the user's value;
   - the duals and `project` drop nothing.

   So no WU11 operation checks anything, and each comment says "never checks". The only M3 operation with a check is `truncate`, for exact scalars (decision 0161), and its checking-mode tests exist (`core.mv_truncate_nonzero_<scalar>`, built in both modes; decision 0030 sets the mode in every preset). Reading proposed for M3's "checking mode": the M3 gate shows that existing evidence and WU11b's per-operation inventory (section 2). The rounding-scalar check of decision 0010, a discarded part within the operation's forward bound, belongs to the first operation that drops part of its own result, `transform` (CONVENTIONS 4.4 and 4.5, WU13). WU13 builds it in the core, and WU11b's test-side bound is the independent oracle WU13 checks it against. Question O3.

R15. **The V12 tests (WU11b) and their oracle.** VVUQ V12 asks for a tolerance fixed before the result is seen, derived from an error bound or an exact reference, and the tightest that admits every known source of difference. The design:
   - **Inputs.** Seeded random coefficients p · 2^−27 with |p| < 2^27, taken from the suite's one seed source (V7). Each is exact in double and in the test rational, and a product of two of them needs up to 54 bits, so it rounds in double. The exact results are meant to fit the test rational's 63 bits; the test author sizes p per form by measurement (random form 1 has coefficients up to 31), and the rational stops rather than wraps if a case does not fit.
   - **Exact value e.** The golden composition over the test rational, as in WU10's tests: independent of the library (V1 kind 3).
   - **Bound.** A test-only tracking scalar, `test_support::tracked`, carries an exact value, an exact magnitude, and a count of roundings. Its operations follow the double's rounding events in the kernel: a product of two values adds one rounding to the sum of their counts; a sum adds one to the larger count; negation, a copy, and a conversion from an integer coefficient add none; the magnitude of a sum or product is the sum or product of the magnitudes. Run through the same library call as the double, it gives each output coefficient its depth K and its magnitude M, the result of the same computation with every coefficient and input replaced by its absolute value. Then |d − e| ≤ γ_K M, with γ_K = K u / (1 − K u) and u = 2^−53 (Higham, *Accuracy and Stability of Numerical Algorithms*, 2nd ed., Lemma 3.1; to be confirmed by reading under V9 before it is cited in a test). Because the tracked run goes through the library's own kernels, the bound has exactly the roundings that were computed, including those of the composed operations (`project` has three stages). Its value must also equal e exactly, which is a free check.
   - **The check in double.** e and M have power-of-two denominators, so each rounds to double once: ê and M̂. The test passes when fl(|d − ê|) ≤ fl(γ_(K+4) M̂). The four extra roundings cover ê, M̂, the subtraction, and the product that forms the tolerance. The numerical-expert writes this margin's derivation beside the constant (S15) and in decision 0175. An exact operation (K = 0: a sign map, a copy, negation) must give d == e exactly.
   - **The test can fail (V2).** A self-test shows that a value outside the bound by more than the margin is rejected, and a mutant that rounds each product through `float` in `run_product` is killed.
   - **Report.** Per family, the largest observed ratio of |d − e| to γ_K M (knowledge base, `numerical-analysis/testing-and-practice/tolerances-and-checking.md`). A ratio above 1 is a defect. A ratio far below 1 is information, never a reason to tighten (V12).

   Rejected: an x87 `long double` reference (x87 is out of scope); `__float128` or `__int128` (compiler extensions, which S2 disables); double-double arithmetic in the test (more machinery than an exact rational with sized inputs, C12). The scalar operations that are one IEEE operation per coefficient (`s * a`, `a * s`, `a / s`) are compared for equality with the same IEEE operation written in the test, since IEEE 754 already bounds that operation by u. Float is not covered: M3 asks for double, and a float run would need its own input scale. It is noted for WU17.

### The W2 question (lesson 0067; audit owner item 4)

The owner has an open question: whether W2's exception may cover a run of test commits written before their code. The audit recommends amending W2 to allow it. This plan does not depend on the answer:
- If W2 is amended before WU11a starts, the test author commits on `feature/core-duals` directly, and the run is declared in the review request.
- Otherwise, the test author commits each group of tests on `feature/core-duals-tests`, then hands off. The implementer takes the groups in order with `git cherry-pick -x`. Each cherry-picked test commit is the single failing commit that W2 allows, its message says so, and the implementer's code commit follows it at once. History stays linear, the test author's authorship and the original commit are recorded, and no shared history is rewritten. The tests branch is not merged, since its commits are already in.

Either way, the test author first compiles every test against a scratch stand-in of the new header, like the planner's `compose.hpp`, to show the tests are well formed and to kill scratch mutants (lesson 0067). The implementer runs the mutants again on the real code.

### Owner questions (section 8)

- O1. `project` in WU11a (departure 2).
- O2. CONVENTIONS version 1.9: the wording of 3.2 (R7, R8), bundled with the audit's proposed 4.5 wording if the owner takes it.
- O3. M3's "checking mode" and "V12 double tests" read as R14 and R15.
- O4. The dot and the antidot as multivectors (R6), confirmed at the M3 gate.

WU11a does not wait for any of them: commits 8 and 9 of section 4 (`project`) wait for O1, and the version row of CONVENTIONS waits for O2.

## 2. Definition of done

WU11a:
- `include/galoot/core/dual.hpp`, new: the eight functions of R1 (nine after O1), each a short composition as in R4, with its own coefficient check (R11). Its `///` comments state:
  - the definition (3.2, 3.3, 4.1, 4.3, restated) and the identity the composition uses;
  - the result layout (R3), with a rigid(3) example and the conversion that reads it;
  - the cases of R8, R9, and R12;
  - "never checks" (R14);
  - the measured limits on both compilers (R10);
  - for the binary functions, that a requires-expression asks a catch-all and is told yes;
  - `[[nodiscard]]` and `constexpr` on all.

  It also holds the nine layout variable templates (one `///` each), the restricted kernel in `detail`, and the rejecting function. `core.hpp` includes `dual.hpp`, and `galoot_core`'s header set lists it.
- `test/reference/golden.py` and `golden.hpp`: the namespace `exomorphisms`, with fact "the metric exomorphism sends e_A to the sum of det g[A, B] e_B", and G of every blade of the five golden forms (section 5).
- Tests (doctest-expert role) in a new `test/core/dual_test.cpp`, whose identities over the signature set may go in `test/core/dual_identities_test.cpp` if the measured cost says so (audit owner item 9: nothing is added to `identities_test.cpp`). Compile-fail tests for each message of R11. CTest labels `core;core.dual`.
- Trace table (`records/convention-trace.md`):
  - 3.2: the rigid(3) fact set to `yes`, and the new golden fact as a row set to `yes`;
  - 3.3: the existing fact set to `yes`, and a new row "the left duals are the right duals times (-1)^(gr ag)";
  - 3.4: set to `yes`;
  - 4.1: a new row "the dot is a^T G b and the antidot a^T antiG b times the antiscalar";
  - 4.3: set to `yes` after O1.
- Decisions 0171 (split, header, layouts and names), 0172 (the compositions, the identities with their proofs, the restricted kernel, the alternatives), 0173 (messages), and 0174 (`project` in WU11 and the 1.9 proposal, with the owner's rulings on O1 and O2).
- Records:
  - mutants, in `records/mutations/0020-duals/`;
  - the diagnostics table (`records/diagnostics/wu11a-messages.md`: each message, its first-line length, its line count, both compilers);
  - W10 before and after for every suite that includes `core.hpp`, and P2 for the smoke unit, pinned;
  - the limits table of R10, measured;
  - the growth figures of R10, pinned;
  - the assembly counts of R5;
  - decision 0080's copy step at the merge, so that `compile.tsv` holds the new units (audit owner item 8);
  - the plan's outcome for WU11a.

WU11b:
- `test/support/tracked.hpp` (the tracking scalar of R15) with its self-tests, and `test/core/rounding_test.cpp` covering every M3 operation over double: the four products and their operators; the five maps and `~`, `!`; sums, differences, negation, scaling, division; `part` and `truncate`; and, after WU11a merges, WU11a's functions. Labels `core;core.rounding`.
- A research note recording the reading of the source of the γ bound (V9), by the numerical-expert role.
- Decision 0175: the tolerance policy of R15, the margin's derivation, and the M3 checking-mode inventory of R14, with the owner's ruling on O3.
- Records: mutants (`records/mutations/0020-rounding/`), the observed ratios, the cost of the new unit, and the plan's outcome closed.

Both parts: W5 blind review, with the usability checklist for WU11a (it changes a public interface), requested by the coordinator; a delta review after any header change that follows the review (decision 0094 item 3); merge by the coordinator after P9's checks.

## 3. Acceptance criteria

WU11a:
1. **Values against independent references.** On every blade of the five golden forms and the oriented rigid(3) blades, `bulk`, `weight`, and the four duals equal the composition from golden G and golden complements: G e_A from `exomorphisms`, 𝔾 = left G right, the duals as right or left of those. On every blade pair of the five forms, `dot` and `antidot` equal Σ a_A G_AB b_B and Σ a_A 𝔾_AB b_B 𝟙 from the same golden values. In R(1,1,0), R(2,1,1), and R(1,3,0), the bulk and the weight equal the products of the golden diagonal squares. Exact equality throughout.
2. **Layouts.** Each result layout equals a layout written in the test from the definition, never one taken from `detail`. Each `*_layout` name has an accepting `static_assert` (rigid(3) point and line) and a rejecting one (a non-algebra), following review F4.
3. **Conventions and the distinguishing test.**
   - 3.2: in rigid(3), bulk(x) + weight(x) = x for random x, with the bulk on the blades without e4.
   - 3.3: a ∧ bulk_dual(b) = (a • b) 𝟙 for a and b of equal grade, at every grade of rigid(3), with (a • b) composed from golden G, not from the library's `dot`. The left duals give (−1)^(gr·ag) times that. Hand values: bulk_dual(Point{x, y, z, w}) read as a Plane is (x, y, z, 0), left_bulk_dual gives (−x, −y, −z, 0), and weight_dual gives (0, 0, 0, w). This is the dual-sign test of section 8 (right duals against left duals at the odd grades of n = 4), and it also tells the bulk dual from the weight dual and right(G x) from G(right x).
   - 3.4: bulk_dual(bulk_dual(x)) is zero, by value and not only by an empty layout, in rigid(3), R(0,0,3), R(2,0,2), and the degenerate non-diagonal form. In the non-degenerate forms it is (−1)^(gr·ag) det(g) x, so a mutant that empties every layout dies.
   - 4.1: dot(e12, e12) = +1 in R(3,0,0).
   - 4.3, after O1: the point (1, 1, 7) projected onto the plane z = 2 is (1, 1, 2, 1). For unitized points and unitized planes with rational unit normals ((3/5, 4/5, 0), (2/3, 2/3, 1/3)), the result has weight 1, lies on the plane, and does not change when projected again.
4. **Identities over the signature set** (plan 0001 section 5), seeded and exact: G𝔾 = 𝔾G = det(g) times the identity, as bulk(weight(x)) and weight(bulk(x)); 𝔾 = right G left; the general double dual of criterion 3; symmetry of `dot` and `antidot`; right(antidot(a, b)) = dot(right(a), right(b)); and the R8 cases. Dense five-generator forms use grade subsets where Clang's product limit applies (R10). Sparse sets at n = 8, 12, and 16.
5. **Validation (V8).** Each referent was read and recomputed in research note 0005:
   - the rigid(3) split (B6);
   - 𝔾 = left G right = right G left (B6, the poster);
   - weight = −bulk in conformal(3) (C1);
   - P • P = −r² for a round point of conformal(3) (C2), as `dot` on conformal(3) vectors;
   - the bulk dual of the real sphere, −(p + e4 + ½(p² − r²) e5) (C3).

   The comparison with the conformal wiki's Duals table (section 8, "Conformal duals") stays with WU12.
6. **Diagnostics.** Each of the 15 rejections (12 before O1) gives one error, also in the form `auto r = ...;`, with the message on the first error line on GCC and Clang, pinned by `expect_output` compile-fail tests that forbid the internal names of R11. Clang's first line for a mismatched comparison names `bulk_layout<...>` (and likewise for the other operations). Sizes are recorded beside the WU10 tables.
7. **Detection.** Each catch-all has a requires-expression probe showing the documented "yes" for a rejected call, and a probe showing that it captures no valid call and no other type's function in a scope where `galoot::core`'s names are visible. Each narrow-scalar test distinguishes its checked plan (R11).
8. **Limits.** The table of R10 is measured for every new operation and stated in `dual.hpp`. `dual_test.cpp` builds in the `clang-measure` configuration (optimized, without libstdc++'s assertions), so WU11a adds nothing to audit B1.
9. **Run code (P4).** Read in the `-O2` assembly on both compilers: dot and antidot of two motors use 4 multiplications and 3 additions each; bulk of a line and weight_dual of a point use none; project of a point onto a plane uses 21; none calls anything but the stack protector.
10. **Mutants.** Each mutant of section 6 is killed on gcc-debug and clang-debug by a test, never by a warning. An equivalent survivor is recorded with its reason.
11. **Checks.** P8 and P9. Every build and test is timed with `tools/timed`, and builds draw from the shared job pool, whose status is read before and after each heavy run. All seven presets pass from new, empty trees at the final revision of the build inputs, on a clean tree, with the revision checked in the run rows (WU10a departure 10). The trace gate, `tools/runlog check`, and the format check pass.
12. **No citation** of project records in code, comments, tests, or build files (STYLE S11).
13. **W2.** No run of non-building commits beyond the single test commits that W2 allows, unless the owner has amended W2, in which case the run is declared.

WU11b:
1. The tracking scalar's rules hold on hand cases: depth and magnitude of a product, a sum, a negation, and a coefficient. A value outside the bound by more than the margin is rejected.
2. Every M3 operation over double, on rigid(3), conformal(2), conformal(3), and random form 1, is within γ_(K+4) M̂ of the exact golden value, coefficient by coefficient, and exact where K = 0. The tracked value equals the golden value exactly. The largest observed ratio per family is recorded.
3. Each tolerance is a named constant with its derivation beside it, and the source of γ_K is read and recorded (V9).
4. Mutants killed: products rounded through `float`; a scalar multiplication by the reciprocal; and the tracking scalar's sum rule undercounting by one, caught by its self-test.
5. Checks as for WU11a, criteria 11 and 12.

## 4. Approach

Tests come before the code they judge, with the mechanics of section 1, "The W2 question". Expected-value changes to existing tests go in their own listed commits (W9). Small commits.

WU11a, on `feature/core-duals`:

0. Claim: this plan committed to `develop`. Branch and worktrees as in the header.
1. `golden.py`: the `exomorphisms` namespace (section 5), `golden.hpp` regenerated, the trace row for its fact (set to `no` until commit 3). The golden check and the facts tests run. This changes `golden.hpp`, so every test that includes it rebuilds once (P8).
2. Test group: `bulk` and `weight` (golden values, layouts, R8 and R9 cases, conformal(3) validation, two narrow-scalar compile-fail tests).
3. `dual.hpp` with `bulk`, `weight`, their layouts and checks; `core.hpp`; the header set; CTest registration.
4. Test group: the four duals, the dual-sign test, the double duals, four narrow-scalar tests.
5. The four duals and their layouts.
6. Test group: `dot` and `antidot` (golden values, hand values, symmetry, the De Morgan relation, e12 • e12, a −0.0 coefficient kept by the restricted kernel, empty layouts), four mixed-operand tests, two narrow-scalar tests, and the catch-all probes.
7. `dot`, `antidot`, the restricted kernel with its S7 accept and reject assertions, the rejecting function, and the layouts.
8. After O1 only, test group: `project` (4.3 hand value, unitized planes, idempotence, a point onto a line, two mixed-operand tests, one narrow-scalar test).
9. After O1 only: `project` and its layout.
10. Test commit: the identities over the signature set (criterion 4), passing when committed. The `slow` label only if measured.
11. Trace rows of section 2, with the trace gate run.
12. Records of section 2; decisions 0171 to 0174; the WU11a outcome. If O1 is still open here, WU11a closes without commits 8 and 9, which are recorded as waiting, and a later small unit adds them (W8).

WU11b, on `feature/core-rounding`:

0. Claim line for WU11b in this plan, on `develop`.
1. Research note: the source of the γ bound, read and confirmed (numerical-expert role).
2. `test/support/tracked.hpp` and its self-tests.
3. `rounding_test.cpp`: the WU9 and WU10 operations.
4. After WU11a merges: `develop` merged in; the WU11 operations added.
5. Decision 0175 and the checking-mode inventory; records; the outcome closed.

## 5. Evidence

Independent oracles (V1 kind 3) come from the golden header, generated by `test/reference/golden.py` by routes that share no code with the library. Identities (kind 2) are checked exactly, on seeded random multivectors over the integers or the test rational. The exhaustive checks on every blade and every blade pair of the golden forms are proofs for those forms (kind 1).

| Operation | Golden values that exist | What WU11 adds, and how the test uses it |
| --- | --- | --- |
| `bulk`, `weight` | `gp_tables` (Gram matrices and det g of five forms), `complements`, `grade_signs`, `diagonal_signatures` | `exomorphisms`: G e_A for every blade of the five `gp_tables` forms. In `golden.py`, route 1 is Gram minors det g[A, B], with `clifford_reference.det`. Route 2 is the scalar part of e_A ẽ_B, from `clifford_reference`'s product table. The script stops if they differ, and it also checks G𝔾 = det(g) times the identity with its own complements. The test composes 𝔾 = left G right from golden G and golden complements. Diagonal forms use `diagonal_signatures`. |
| duals | as above | right or left of golden G and 𝔾, composed in the test |
| `dot`, `antidot` | as above | Σ a_A G_AB b_B and Σ a_A 𝔾_AB b_B, composed in the test. Route 1 of `golden.py` is not the library's route (⟨a b̃⟩₀), so the test is independent of it. |
| dual-sign test | `complements`, `oriented_names::rigid_3`, `units` | (a • b) from golden G; hand values from the rigid(3) complement table (Lengyel's, Checked) |
| `project` | `oriented_names::rigid_3` | hand values, as `conventions_check.py`'s `check_objects` |
| validation | research note 0005 (B6, C1, C2, C3) | the values recomputed in the note |

`conventions_check.py` is unchanged. It already checks 3.3, the left-dual sign, 3.4, and 4.3 for rigid(3), which is where those items' Checked labels come from. Its `bulk` and `weight` are mask-based and valid only for its 0 and 1 squares.

V10, to fill in at each outcome. Verification by the above. Validation as in criterion 5. Not validated: the non-diagonal bulk and weight beyond conformal(3) (no published table for other forms), and the Terathon side of the dual-sign disagreement, which stays untested under C15 and C16 (plan 0001, risk 9).

WU11b's evidence is the bound of R15 against the exact golden value. It verifies the claim that rounding enters only through the scalar (C7), as far as these operations and inputs go. A sampled claim states its seed and sample count (V9).

## 6. Mutants

WU11a, each on gcc-debug and clang-debug:
- `bulk_dual` uses `left_complement`, so it is `left_bulk_dual` (killed at the odd grades of n = 4);
- `bulk_dual` omits the reverse;
- `bulk_dual` multiplies 𝟙 ⟑ x̃;
- `weight_dual` is right(bulk(x)), the bulk dual;
- `weight` calls the bulk;
- `weight` takes `right_complement` on the outside;
- `left_weight_dual` uses the right complement;
- `dot` omits the reverse (killed by e12 • e12);
- the restricted kernel keeps the antiscalar's terms, or keeps every term;
- the restricted kernel adds its first term to zero (the −0.0 test);
- `antidot` returns `dot` without the complements;
- `antidot` takes the right complement of the scalar on the outside (equivalent: left(1) = right(1) = 𝟙; recorded);
- `project`'s operands are swapped;
- `project` uses `bulk_dual`;
- each narrow-scalar check is dropped, and each checks another operation's plan;
- the algebra and scalar constraints are dropped from the three catch-alls;
- a layout name returns the input's layout.

WU11b:
- `run_product` rounds each product through `float`;
- `a / s` multiplies by the reciprocal;
- the tracking scalar's sum adds no rounding;
- the margin is removed (the self-test with a value at the edge).

## 7. Risks

1. **Stacked limits.** A composition is bounded by the smallest limit of its parts: the reverse map's at `full<8>` on Clang, and the product's at `full<6>` on Clang and on dense five-generator forms (R10). The models planned use n ≤ 6 and typed layouts far below these. The comments state the limits.
2. **The restricted kernel is new `detail` code.** It is about 20 lines over the existing plan. It gets S7 accept and reject assertions, its own mutants, and the assembly check of criterion 9.
3. **Nine public names, pending audit owner item 7** (R3). If the ruling comes after the merge, a move to `detail` changes `dual.hpp` with the others.
4. **Clang's optimized build without assertions (audit B1).** WU11a measures that configuration by hand (criterion 8) until the owner rules on B1.
5. **Users' expectations.** Someone who knows the inner product expects −1 from e12 • e12. Someone who knows the cga3 model's parts expects the core's bulk in conformal(3) to be a subset of blades. Both are stated in comments and pinned by tests (R8, R12).
6. **Growth with n of the weight, the weight dual, the antidot, and `project`,** through the complements (R10). Measured; recorded beside decision 0141; not a blocker under P2 as decision 0005 reads it.
7. **The golden header rebuilds every test that includes it** (commit 1). Once.
8. **Rational overflow in WU11b.** The tracking scalar holds two rationals. Inputs are sized by measurement, and the rational traps rather than wraps.
9. **Shared files.** `test/CMakeLists.txt` with WU11b, and `runs.tsv` rows with every lane: whichever merges second merges `develop` first (W7).
10. **Owner-dependent commits.** `project` waits for O1, and the 1.9 version row waits for O2.

## 8. Open questions for the owner

- **O1. `project` in WU11a.** Plan 0001 puts it in WU13 but asks for 4.3 to be pinned at M3. Recommended: build `project(a, b)` = b ∨ (a ∧ b☆) in WU11a. `antiproject` stays in WU13, whose plan proposes the 4.3 text that defines it. If the answer is no, the M3 gate reports 4.3 as pinned at M4.
- **O2. CONVENTIONS version 1.9, 3.2.** Proposed text in place of the second sentence: "In a diagonal metric, the bulk keeps the blades whose square is nonzero, each times the product of the squares of its generators, and the weight keeps the blades whose complement's square is nonzero, each times the product of the squares of the generators it lacks. In rigid(d) these factors are 1, the bulk is the part without e(d+1), and the weight is the part with it; with two or more null generators some blades are in neither." Recommended: yes, in the same version as the audit's proposed 4.5 wording, if the owner takes that too. WU11a follows 3.2's first sentence either way (R7).
- **O3. M3's "checking mode" and "V12 double tests".** Recommended: read as R14 and R15. The M3 gate shows the existing exact-scalar checks and WU11b's inventory and bounds. The rounding-scalar check of decision 0010 is built with `transform` in WU13, verified against WU11b's bound.
- **O4. Return types of `dot` and `antidot`.** Recommended: multivectors on {1} and {𝟙}, or empty (R6), confirmed when the M3 gate reviews the operation inventory and API names.

Also in force, and open with the owner from the audit: W2 (handled by section 1 either way), the public `*_layout` names (R3), and B1 (criterion 8).

## 9. Agent cost estimate

Recorded in `docs/agent/records/agent-cost.tsv` as each agent finishes. For comparison: plan 0019 estimated WU10, both parts, at 2.2M to 3.2M tokens, and blind reviews have used 280k to 380k.

| Part | Role | Model | Estimate |
| --- | --- | --- | --- |
| WU11a | test author, with `golden.py` (doctest-expert) | Opus, high | 300k to 400k tokens |
| WU11a | proof of decision 0172 and review of the `golden.py` routes (math-expert) | Opus, high | 80k to 150k |
| WU11a | implementer (cpp-expert role) | Opus, high | 450k to 600k |
| WU11a | blind reviewer (W5, with the usability checklist) | Opus, high | 300k to 380k |
| WU11a | delta reviewer, if the review asks for fixes | Opus, high | 200k to 270k |
| WU11b | implementer and test author (numerical-expert role) | Opus, high | 350k to 500k |
| WU11b | blind reviewer (W5) | Opus, high | 220k to 300k |
| WU11b | delta reviewer, if needed | Opus, high | 150k to 220k |

Size M overall, as plan 0001 has it: about 2.1M to 2.9M tokens in all. This planner's own cost is recorded by the coordinator from the harness report.

## 10. Outcome

### WU11a

Implemented on `feature/core-duals` (implementer: Claude, agent, Opus 5.5, cpp-expert role; test author: Claude, agent, Opus 5.5, doctest-expert role, on `feature/core-duals-tests`; proofs for decision 0172 and a review of the golden routes: Claude, agent, Opus 5.5, math-expert role). The owner answered O1 to O4 on 2026-11-04, "take all recommendations" (decision 0174), so `project` (commits 8 and 9) and CONVENTIONS version 1.9 are in. Not reviewed yet; not merged.

Commits, in the order of section 4. W2 was not amended, so the test author committed each group on `feature/core-duals-tests` and each was cherry-picked with `-x` immediately before the code it judges; each such commit is the single failing commit that W2 allows, and its message says so.

| Step | Commit | What |
| --- | --- | --- |
| 1 | `3f60402` | golden `exomorphisms` (test author; `feature/core-duals` fast-forwarded to it) |
| 2 | `c766fc4` (from `87074e5`) | tests of the bulk and the weight, and `FORBID_FIRST` / `FORBID_ON_FIRST_ERROR` |
| 3 | `b3dd471` | `dual.hpp` with the bulk and the weight; `core.hpp`; the header set |
| 4 | `111a59c` (from `07a95a2`) | tests of the four duals |
| 5 | `82cbec1` | the four duals |
| 6 | `ae239b9` (from `ee2301d`) | tests of the dot and the antidot |
| 7 | `b6ea20f` | the dot, the antidot, the scalar-part kernel, the rejecting function |
| 8 | `9f7b25f` (from `e6d40aa`) | tests of `project` |
| 9 | `75c8c04` | `project` |
| 10 | `863edc8` (from `fba38db`) | the identities over the signature set |
| | `6837785` | the kernel's static_asserts (the implementer's own test, listed for review) |
| | `cab40c0` | CONVENTIONS 1.9 and decision 0174 |
| 11 | `63995c5` | trace rows |
| | `4720980` | golden comments (math-expert review, minor) |
| | `69d6dcc` | the measured limits in `dual.hpp`'s comment |
| | `505737e`, `48172c4` | clang-tidy fixes: the antiscalar's layout; the dual tests (listed for review) |
| 12 | the records commit | decisions 0171 to 0173, records, this outcome |

The test author's `75596d4` (run-log rows) was empty when cherry-picked: the union merge of `runs.tsv` had already brought its rows with `e6d40aa`. It was skipped; all 324 of the test author's run rows are in the branch's log.

Departures and readings (C20):

1. The test author changed shared test infrastructure outside the `core.dual` block (`c766fc4`): `FORBID_FIRST` in `test/expect_output.cmake` and `FORBID_ON_FIRST_ERROR` in `cmake/galoot_testing.cmake`, with three tests `tools.expect_output_forbid_first_*`, because no existing check could forbid a name on the first error line only, which R11 asks. This is build-expert ground; existing tests do not use it.
2. Eight `core.dual_compare_<function>` compile-fail tests (and one for `project`) pin criterion 6's "Clang's first line names `bulk_layout<...>`"; their required literal is matched anywhere in the output, not only on the first line.
3. `bulk(x)` is written `left_complement(geometric_product(reverse(x), 𝟙))` in place, not `left_complement(bulk_dual(x))` as R4's table has it, so that commit 3 does not depend on commit 5; the composition is the same.
4. The message nouns of R11's "likewise" are the test author's: "the bulk's", "the weight's", "the bulk dual's", "the weight dual's", "the left bulk dual's", "the left weight dual's", "the dot's", "the antidot's", "the projection's".
5. The S7 assertions of the dot's kernel are in `dual_test.cpp` (`6837785`), written by the implementer, not in the header, so that no includer of `core.hpp` evaluates a plan for them.
6. The limits search, the assembly probe, and the diagnostics sizes ran against headers of `6837785` (limits and assembly, from a scratch copy) or `69d6dcc` (diagnostics); the later header commits change comments and the way the antiscalar's layout is built, nothing a probe measures. Their run rows read `-dirty`, since the worktree held uncommitted records (and, during the limit search, a mutant run's edit of `dual.hpp`, which the scratch copy did not see).
7. The mutants ran before `505737e` and `48172c4` and were not rerun (records README).
8. The plan file's status line and this outcome are edited on this branch; WU11b may edit the same file, so whichever merges second merges `develop` first (W7).

Evidence:

- Verification: the bulk, the weight, and the four duals of every blade of the five golden forms and the oriented rigid(3) blades against golden G and golden complements; the dot and the antidot of every blade pair of those forms against Σ a_A G_AB b_B and Σ a_A 𝔾_AB b_B; the diagonal products of squares in R(1,1,0), R(2,1,1), and R(1,3,0); layouts against layouts written from the definition, with accepting and rejecting static_asserts for each layout name; the dual-sign test of section 8 at the odd grades of n = 4 (bulk_dual of a point is the plane (x, y, z, 0), left_bulk_dual (−x, −y, −z, 0)); double duals zero by value in rigid(3), R(0,0,3), R(2,0,2), and the degenerate non-diagonal form, and (−1)^(gr·ag) det(g) x elsewhere; e12 • e12 = +1 in R(3,0,0); the projection of (1, 1, 7) onto z = 2 and onto rational unit planes; the identities of criterion 4 over the signature set, seeded and exact, with sparse sets at n = 8, 12, and 16. All pass on every preset (below).
- Proofs (decision 0172): the identities behind the compositions, from the definitions, for every symmetric form over a field of characteristic 0, by the math-expert, checked by `records/spikes/dual-identities/check.py` (17,878 exact checks on seven forms and golden.hpp, all passing). They close one gap in R4's sketch (the non-diagonal case of x 𝟙 = x ⌋ 𝟙).
- Golden routes: reviewed by the math-expert; no blocking or major finding, two minor comment findings fixed in `4720980`.
- Validation (V8): the rigid(3) split, 𝔾 = left G right = right G left, weight = −bulk in conformal(3), P • P = −r² for a round point, and the bulk dual of the real sphere, against research note 0005; all pass. Not validated: the non-diagonal bulk and weight beyond conformal(3), and the Terathon side of the dual-sign disagreement.
- Mutants (`records/mutations/0020-duals/README.md`): 45 on gcc-debug and clang-debug; 44 killed on both, each by a test; one equivalent survivor, `antidot_right_complement_outside` (right(1) = left(1) = 𝟙), as section 6 expected.
- Diagnostics (`records/diagnostics/wu11a-messages.md`): each of the 15 rejections gives one error with its message on the first error line on both compilers, with no `apply:`, `geometric_product:`, `left_complement`, `product_plan`, or `detail::` there; Clang's first line for a mismatched comparison names the operation's layout.
- Run code (P4, `records/performance/duals-cost/README.md`): the dot and the antidot of two motors 4 multiplications and 3 additions on both compilers; the bulk of a line and the weight dual of a point no arithmetic; the projection of a point onto a plane 21 multiplications; no calls.
- Limits (same record, criterion 8): the six unary operations up to `full<7>` on Clang and `full<9>` on GCC, the three binary ones up to `full<5>` and `full<6>`, the same with and without libstdc++'s assertions; everything builds for `full<5>` of conformal(3); the dot and the antidot of `full<5>` in the dense five-generator form fail on Clang only. Stated in `dual.hpp`. `dual_test.cpp` and `dual_identities_test.cpp` build and pass under `clang-measure` (audit B1's configuration) and `gcc-measure`.
- W10 and P2 (pinned, three repeats): a unit that includes only `core.hpp` +7.6 M (+0.8%) on GCC and +6.9 M (+0.7%) on Clang; `smoke_test.cpp` +0.7% on both. `dual_test.cpp` 38.1 G and 35.7 G instructions; `dual_identities_test.cpp` 137.3 G and 153.3 G (about half of `identities_test.cpp`; not labeled `slow`).
- Growth at fixed use, four rigid(3) objects from n = 4 to 16: the bulk +1.5% (GCC) and +8.4% (Clang); the weight +12.4% and +72.0%; the weight dual +13.8% and +76.9%; the antidot +77.9% and +386.0%; the projection +10.1% and +45.6%. The operations through the complements grow as the antiproduct does (decision 0141); the antidot most. Reported under P2; blocks nothing.

Open, for the owner or later units: plan departure 5 (W7's subfeature branch form cannot exist beside its feature branch); audit owner item 7 (the 16 `*_layout` names); a direct generator of the antiexomorphism, if the antidot's growth with n is ever a measured problem; decision 0080's copy step of `compile.tsv` at the merge; the math-expert's knowledge-base note draft (`bulk-dual-by-reverse-and-antiscalar.md`), not added, since the brief gave no permission to write there.
