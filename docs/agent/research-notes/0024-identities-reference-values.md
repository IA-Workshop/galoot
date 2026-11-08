# 0024 Reference values for the identity suite: the involution signs over the antiproducts, a seeded R(p, q, r) draw, golden duals and mirrors, and the dual-sign tests

Written by: Claude (agent, Opus 5.5, math-expert role), 2026-11-08, for WU14 part A (plan 0029, section 9 step 1, items (a) to (e)), on `feature/core-identities--math`. Not yet reviewed by the owner or by the unit's blind reviewer.

## Sources and tools

| Source | Revision | Read? |
| --- | --- | --- |
| Conformal wiki, page Duals, raw wikitext, https://conformalgeometricalgebra.org/wiki/index.php?title=Duals&action=raw | revid 151 (parent 150), saved 2023-10-23T07:16:11Z by "Eric Lengyel", 4112 bytes, MediaWiki sha1 2ef8cd59edd9ccdc9d1432ade4305965fed94b52; SHA-256 of the bytes read 24b2b0cbf8884e8a2249ab88ea2217b73961d1f26f91469f2b61361c4cd066fe | yes, in full |
| Conformal wiki, page Sphere, raw wikitext, same form of URL | revid 208 (parent 203), saved 2024-07-31T04:31:32Z by "Eric Lengyel", 3019 bytes, sha1 a8fae73836dae9d6de183465b9c857d03f181787; SHA-256 078c4549758f581d89c5021149e51f3fe0f5592f4ee350e79c30e6c023d310b1 | yes, in full |

How the sources were read. Each page was fetched once with curl on 2026-11-08 at about 19:30 UTC with a generic browser User-Agent and no other identifying header (no e-mail address, no From header, no cookie), into a scratch directory outside the repository; the revision metadata came from the wiki's `api.php`, and the SHA-1 of the bytes downloaded equals each revision's sha1. I read the wikitext line by line, with no summarizing model in between. The pages' images were not read. No passage of either page addresses its reader with instructions. These are the revisions note 0005 read for its C3 and C5 (Duals is unchanged since 2023; the Sphere revision is the one current then).

Tools, outside the repository (math-tools, branch `tools/ga-involutions-duals` at `a7dee72`, folder `exercises/ga/ga-E12-involutions-duals-mirrors/`; galoot does not depend on them): galgebra 0.6.0 and SymPy 1.14.0 on CPython 3.14.8 for ga-E12 and ga-E13, and CPython 3.14.9 with the standard library for the draw and ga-E14. galgebra is an implementation of a different lineage that shares no code with Galoot or its reference scripts.

Evidence labels:

- Derived: my derivation by hand, written out below.
- Checked: verified exactly by a script; the script and its scope are named.
- Read: stated in a source at the cited place.
- Inferred or assumed: my conclusion, not checked; said where it applies.

## Summary

1. The grade involution over the antiproduct carries a sign: grade_involution(a ⟇ b) = (−1)^n grade_involution(a) ⟇ grade_involution(b), and the same for the antiwedge, in every metric. The antigrade involution is an automorphism of both antiproducts. For odd n, the rule without the sign fails on every pair whose antiproduct is nonzero, so criterion 3's test tells the sign apart at n = 1, 3, 5, and only there. (A1, A2)
2. The antigrade-involution clauses of CONVENTIONS 2.1 hold: x̌ = (−1)^n x̂, and right(x̌) and left(x̌) are the grade involutions of right(x) and left(x). (A3)
3. The seeded draw gives R(0,3,2) and R(0,2,3) at n = 5 and R(1,3,2) and R(1,1,4) at n = 6. The cost model predicts that every identity of criterion 5 builds at both compilers' default limits on full layouts at n = 5 and on vectors and bivectors at n = 6; the largest is 704,143 of Clang's 1,048,576 steps (the antiproduct's `fill` phase of full<5> × full<5> in R(0,3,2)). full<6> does not fit Clang's limit. (B1 to B3)
4. `golden.py` now writes the namespaces `duals` (CONVENTIONS 3.3) and `mirrors` (4.4), each from two routes that agree, and ga-E13 recomputes all their values with galgebra and agrees. (C1 to C4)
5. Both section 8 cases tell their alternatives apart: the point case of `dual_test.cpp:961` fails under left duals, and the sphere case of `dual_test.cpp:1122` fails under the conformal wiki's Duals table, which I read directly at revid 151. The conformal row's condition of decision 0216 item 5 is met. (D1, D2)
6. No new disagreement between sources was found, so nothing stops for the owner under C11.

## A. The involutions over the antiproducts (part A (a))

Convention: Galoot's (CONVENTIONS 2.1, 3.1, 4.2). n generators; x̂ is the grade involution, (−1)^k on a blade of grade k; x̌ is the antigrade involution, (−1)^m on a blade of antigrade m = n − k; a ⟇ b = left(right(a) ⟑ right(b)) and a ∨ b = left(right(a) ∧ right(b)).

A1. Derived. Three facts, each for any symmetric metric:

- (i) right(x̌) = (right x)^. A blade u of grade k has antigrade n − k, and right(u) is ± a blade of grade n − k, so both sides multiply right(u) by (−1)^(n−k). The same holds with left.
- (ii) x̌ is an automorphism of ⟇ and ∨. Applying right to the antiproduct and using right(left(y)) = y, right((a ⟇ b)ˇ) = (right(a) ⟑ right(b))^ by (i), which is (right a)^ ⟑ (right b)^ because x̂ is an automorphism of ⟑ (in every metric: the geometric product preserves the parity of grades, which is the Z/2 grading of the Clifford algebra). By (i) again this is right(ǎ) ⟑ right(b̌), and applying left gives (a ⟇ b)ˇ = ǎ ⟇ b̌. The wedge preserves grade parity too, so the same argument gives (a ∨ b)ˇ = ǎ ∨ b̌.
- (iii) Since x̂ = (−1)^n x̌, (a ⟇ b)^ = (−1)^n (a ⟇ b)ˇ = (−1)^n ǎ ⟇ b̌ = (−1)^n ((−1)^n â) ⟇ ((−1)^n b̂) = (−1)^n â ⟇ b̂. Likewise for ∨.

A second route for (iii), by degrees: the antiproduct adds antigrades modulo 2 (it is ⟑ conjugated by the complements, which exchange grade and antigrade), so a term of a ⟇ b, for blades of grades g₁ and g₂, has antigrade ≡ (n − g₁) + (n − g₂), hence grade ≡ g₁ + g₂ − n (mod 2), and its sign under x̂ is (−1)^n times (−1)^(g₁+g₂). Worked example, n = 3: 𝟙 ⟇ 𝟙 = 𝟙, and (𝟙)^ = −𝟙, while (−1)³ (−𝟙) ⟇ (−𝟙) = −𝟙. For n = 2 the sign is +1: (e12)^ = e12.

A2. Checked (ga-E12, `antiproduct_involutions.py`, output `out/antiproduct_involutions.txt`). On every pair of basis blades: (iii) for ⟇ and ∨, (ii) for both, x̂ over ⟑ and ∧, and antireverse(a ∨ b) = antireverse(b) ∨ antireverse(a), the other criterion 3 rule. Forms: R(1,1,0), the null pair, R(3,0,0), R(1,1,1), conformal(1), the degenerate form [[0,1,0],[1,0,0],[0,0,0]], rigid(3), R(2,0,2), conformal(2), R(2,1,2), R(1,1,3), conformal(3), seeded dense random forms of 2 to 5 generators (seed 20261108), and a Gram matrix of independent symbols for n = 2, 3, 4, and 5. The symbolic runs make each claim a polynomial identity in the entries, so it holds for every symmetric form of up to five generators (V1 kind 1 for those n). 6,848 pairs, no failure. Control: the rule without (−1)^n fails, for every odd-n form, on every pair whose antiproduct is nonzero (64 of 64 in R(3,0,0), 1,024 of 1,024 symbolically at n = 5), and holds for every even-n form. n = 1 is not in the run (galgebra builds no algebra of one generator from a name string); there the rule is the derivation alone.

Consequence for criterion 3's test: the expected rule is grade_involution(a ⟇ b) = (−1)^n grade_involution(a) ⟇ grade_involution(b). Its marker can state "the grade involution of an antiproduct is (-1)^n times the antiproduct of the grade involutions". Only the groups with odd n (the forms of one and three generators, the diagonal and dense forms of five, conformal(3), and the new n = 5 forms) can show a wrong sign; the even-n groups, the sparse forms of 8, 12, and 16 among them, check only that the involution keeps the order.

A3. Derived and checked (ga-E12 C1, C2): x̌ = (−1)^n x̂ and right(x̌) = (right x)^, left(x̌) = (left x)^, on every blade of every form above. `conventions_check.py` already checks the first two in the six diagonal forms of its odd-sign check. These are the clauses CONVENTIONS 2.1 states; nothing in them needs a change.

For section 6's 2.1 row of the plan: no public function computes x̌, so I recommend pinning these facts through the transform. The golden `mirrors` values (C3) were computed with x̌ in the odd form −V ⟇ x̌ ⟇ V̰, and the two parities of n give opposite relations to x̂ (rigid(2), R(3,0,0), rigid(4), R(5,0,0) are odd; rigid(3) and R(4,0,0) even), so a test that compares `transform` with them tells x̌ from x̂ wherever n is odd. Its marker can state the 2.1 facts as written in the fact list of section F.

## B. The seeded R(p, q, r) group (part A (b))

B1. The draw. Python's `random.Random(20261108)`; for n = 5 and then n = 6, list every (p, q, r) with p + q + r = n, ordered by p descending and then q descending; choose one uniformly from those with q ≥ 2 and r ≥ 2, then one uniformly from those with r ≥ 3 other than the first. Result: R(0,3,2), R(0,2,3), R(1,3,2), R(1,1,4). The four meet criterion 5 at each n separately, and none is a form the suite already has at n = 5 or 6. `random.choice` draws through `getrandbits`, so the result is the same on every CPython 3 from 3.2 on (background; checked only on 3.14.9). Comment for the group's source, with no record citation:

    // Diagonal forms R(p, q, r) drawn with Python's random.Random(20261108):
    // for n = 5 and then n = 6, of the (p, q, r) with p + q + r = n, ordered
    // by p and then q, both descending, one chosen with q >= 2 and r >= 2,
    // then one with r >= 3 other than the first: R(0,3,2), R(0,2,3),
    // R(1,3,2), R(1,1,4).

B2. Prediction (ga-E14, `galoot_rpqr_cost.py`, output `out/galoot_rpqr_cost.txt`), from `docs/agent/snippets/plan_cost_model` with the weights of `records/performance/plan-cost-model/out/weights-{clang,gcc}-galoot-O0.tsv`. Each identity is traced as the plans and maps it instantiates, with each result set from the model's mirror of the generator, so that nested products (such as (a ⟑ b) ⟑ c on vectors and bivectors) are predicted on their real operands. The forms are written as `R<p, q, r>()` writes them, with only the nonzero diagonal entries.

| Group | Layouts | Clang, largest phase (of 1,048,576 steps) | GCC, largest phase (of 2³³ operations) |
| --- | --- | --- | --- |
| R(0,3,2), criteria 1 to 3, De Morgan, antiproduct laws | full<5> × full<5> | 704,143, antiproduct `fill` | 3,803,234 |
| R(0,2,3), the same | full<5> × full<5> | 672,303, antiproduct `fill` | 3,638,966 |
| R(0,3,2) and R(0,2,3), the dual identities | full<5> | 15,650 | 109,177 |
| R(0,3,2) and R(0,2,3), the dot identities | full<5> × full<5> | 498,927 and 479,135 | 2,841,826 and 2,737,750 |
| R(1,3,2), the kernel identities | vectors × bivectors | 84,771 | 480,240 |
| R(1,3,2), the kernel identities | bivectors × bivectors | 282,369 | 1,601,935 |
| R(1,1,4), the kernel identities | vectors × bivectors, bivectors × bivectors | 83,809 and 259,352 | 476,334 and 1,479,104 |
| R(1,3,2) and R(1,1,4), the dual identities | full<6> | 55,842 | 391,145 |
| R(1,3,2) and R(1,1,4), the dot identities | vectors, bivectors | 154,687 and 141,952 | 880,897 and 814,345 |
| Not proposed: R(1,3,2) and R(1,1,4) | full<6> × full<6> | 3,394,023 and 3,091,327, over | about 1.8 × 10⁷, under |

No loop reaches GCC's 2¹⁸ iterations (the longest is 2,304, at full<6>). Recommendation: the n = 5 forms on `full_mv` as `diagonal_forms_of_five` has them, and the n = 6 forms on vectors and bivectors as `forms_of_six` has them; at the defaults, no raised limit and no source of its own are needed for limits' sake. The two n = 5 forms add about as much compile work as `identities_five_diagonal_test.cpp`, so a source of their own keeps the parallel build balanced (the implementer measures, criterion 14).

B3. What the prediction rests on (inferred, not measured here):

- The weights were fitted to the headers of `4895a40`. Since then `plan.hpp` renamed the operations and gave the two operation enums the underlying type `int` in place of `std::uint8_t`, and `blade_set.hpp` changed `blades`; neither touches the loops the model counts, and the canary `core.plan_cost_model.*`, which pins exact thresholds, is in the suite that passed on all eight presets at `8ff2042` (the run log's `gate:all` rows of 2026-11-08), whose `include/` equals this branch point's, so the model's counts still held for its units. Part C's build is the confirmation.
- With two or more null generators, a row of the Gram matrix has no written entry, and the generator's `abp.entry_row_implied` event occurs, which the fit never saw; it is up to 768 of the full<5> antiproduct's events. The script counts each such event at the largest weight of its function's events (25 steps on Clang), a generous margin: the implied reads the fit did see weigh 0 steps on Clang and at most 1 operation on GCC. The table includes that margin.
- The model predicts the constant evaluations of plans and maps only. Template instantiation and code generation are not in it, so it says nothing about wall time or memory.

## C. Golden values (part A (c), O2)

The rule of `golden.py` (its docstring): a value is written only where two routes agree, and the script stops otherwise. Both namespaces were added at `e24f9c7`.

C1. `duals`, for CONVENTIONS 3.3: the four duals of every blade of rigid(3) and conformal(3), as `blade_image` arrays `<form>_bulk_dual`, `_weight_dual`, `_left_bulk_dual`, `_left_weight_dual`, indexed by mask. Route 1, the definitions: bulk_dual(u) = right(G u), weight_dual(u) = right(𝔾 u), left_bulk_dual(u) = left(G u), left_weight_dual(u) = left(𝔾 u), with G e_A = Σ det g[A, B] e_B and 𝔾 = left G right, from the script's own complements; it also checks right(𝔾 u) = G right(u), the form `dual.hpp` uses. Route 2, the products of `clifford_reference.py`: u★ = reverse(u) ⟑ 𝟙 and u☆ = antireverse(u) ⟇ 1, the identities note 0005 B4 read on the rigid Duals page, and each left dual as the right one times (−1)^(gr ag). In rigid(3) a third route, `conventions_check.py`'s `right(bulk(x))` and its left and weight forms, also agrees. That route 2 holds in conformal(3), where G is not diagonal, is checked here by the agreement, not derived; I derived it only in outline (ũ ⟑ 𝟙 = ũ ⌋ 𝟙, and a ∧ (ũ ⌋ 𝟙) = (a • u)𝟙 by the contraction duality), which is background, not a proof.

C2. What `duals` tells apart: in rigid(3) (n = 4) the left and right duals differ in sign at odd grades (the alternative of section 8's "Dual sign" row); in conformal(3) (n = 5) they are equal, and the weight dual is the negated bulk dual, which is the conformal row's alternative (D2). A test comparing `bulk_dual` and `weight_dual` of every blade with these values fails under either alternative.

C3. `mirrors`, for CONVENTIONS 4.4: for one mirror V in each of rigid(2), R(3,0,0), rigid(3), R(4,0,0), rigid(4), and R(5,0,0) (n = 3, 4, 5; the forms of `conventions_check.py`'s odd-sign check), the squares, a vector f, V on its blades, λ, and `transform(e_A, V)` for every blade as `transform_image` arrays. V is the hyperplane with x ∨ V = f · x, V = (−1)^(n−1) left(f); f has every entry nonzero and λ ≠ 1 (25, 9, 9, 25, 25, 16), so the factor λ shows in every image. Route 1, geometric algebra: −V ⟇ x̌ ⟇ V̰ in `conventions_check.py`, which also checks x ∨ V = f · x and V ⟇ V̰ = λ𝟙. Route 2, matrices with no geometric algebra: λ = Σ g_ii f_i², N_i = g_ii f_i, the reflection R x = x − 2 (f · x) N / λ, which fixes the mirror and negates N, and the image of e_A is λ Σ_B det R[B, A] e_B, the λ-scaled outermorphism, λ for the scalar. Every image is an integer: a k-minor of λI − 2N fᵀ is divisible by λ^(k−1) (derived). Example (rigid(2), f = (3, 4, 5), the line 3x + 4y + 5w = 0): the origin e3 goes to −30 e1 − 40 e2 + 25 e3, the point (−6/5, −8/5) of weight 25, which is the reflection of the origin in that line.

C4. What `mirrors` tells apart: dropping the leading minus negates every image, and every image is nonzero; the plain sandwich V ⟇ x ⟇ V̰ differs from it by (−1)^(m+1) on a blade of antigrade m, so it is wrong on every blade of even antigrade; and x̂ in place of x̌ is wrong on every blade when n is odd (ga-E13 checks the second relation on every image).

C5. Checked independently (ga-E13, `duals_mirrors_crosscheck.py`): the 192 duals recomputed with galgebra's products, G built as the outermorphism of g through galgebra's wedge (not from Gram minors) and the complements solved from u ∧ ū = 𝟙, and the 112 mirror images as −V ⟇ x̌ ⟇ V̰ with galgebra's product, all equal what `golden.py` computes. The mirror V was taken from `golden.py` as input, and ga-E13 checks its incidence and λ on its own.

C6. The facts. Each namespace's `// Fact:` line is a row the trace table already holds, so the trace gate passes now, before any test uses the values: `duals` states "a ^ bulk_dual(b) = (a . b) antiscalar for a and b of equal grade" (3.3), which fixes every bulk dual because the wedge pairing of grades k and n − k is nondegenerate; `mirrors` states "a single mirror maps each vector of the mirror to itself times lambda" (4.4), which is route 2's defining property and fixes the leading sign. A fact closer to all four columns of `duals` would be new, such as "bulk_dual(u) = right(G u) and weight_dual(u) = G right(u)", and would need its row in the trace table in the same change, with no test stating it until part B; that is part C2's choice (the plan gives it the trace rows), and the fact lines change together with every marker that states them.

## D. The section 8 tests (part A (d), O5)

D1. "Dual sign". `dual_test.cpp:961`, "the duals of a point are planes with the dual-sign convention", checks `Plane{bulk_dual(p)} == Plane{2, 3, 5, 0}` and `Plane{weight_dual(p)} == Plane{0, 0, 0, 7}` for p = (2, 3, 5, 7). The alternative the row records is that Terathon's `BulkDual` and `WeightDual` of points and planes return left duals. Derived: for a vector in n = 4, gr ag = 3 is odd, so left(u) = −right(u), and a library whose `bulk_dual` were the left dual would give (−2, −3, −5, 0) and (0, 0, 0, −7); the test's own checks of `left_bulk_dual` and `left_weight_dual` give exactly these. Checked: the golden `duals` rows agree (right(e1) = +e234, left(e1) = −e234). So the case fails under the alternative. The next case, "the duals of a line and a plane by hand" (`dual_test.cpp:986`), does the same for planes (bulk_dual(g) = −7 e4; the left dual is +7 e4) and could be named beside it; a line cannot tell them apart (gr ag = 4).

D2. "Conformal duals". `dual_test.cpp:1122`, "the bulk dual of a conformal sphere is a round point", takes the sphere of the Sphere page (Read, revid 208: s = −e1234 + p_x e4235 + p_y e4315 + p_z e4125 − ½(p² − r²) e3215) with p = (1, 2, 3) and r² = 4, so s_u = −1, s_x, s_y, s_z = 1, 2, 3, s_w = −5, and expects `bulk_dual(s)` = (−1, −2, −3, −1, −5) on e1 to e5. Read (Duals page, revid 151, the sphere row): s★ = s_x e1 + s_y e2 + s_z e3 − s_u e4 − s_w e5, which here is (1, 2, 3, 1, 5). Checked (ga-E13): right(G s) computed with galgebra is (−1, −2, −3, −1, −5), the test's value, and the table's value is its negation, which equals right(𝔾 s), the weight dual. The page itself says (Read) that "In conformal geometric algebra, it is always the case that" the antidual is the negated dual. So the test fails under the table, and under a library that confused G with 𝔾; because n = 5, it cannot and need not tell left from right (they are equal). The condition of decision 0216 item 5 is met.

D3. Proposed text for CONVENTIONS 1.16, section 8's Test column (the owner ratifies the edit, C10; not edited here):

- Dual sign: "`check_identities`; "the duals of a point are planes with the dual-sign convention" in `test/core/dual_test.cpp`, which also checks that the left duals give the negation".
- Conformal duals: ""the bulk dual of a conformal sphere is a round point" in `test/core/dual_test.cpp`, which the Duals table's sphere row fails".

Reading the table's other rows directly confirmed note 0005 C5 on its sphere and round point rows only; the flat point and plane rows were read but not recomputed here.

## E. Prediction for DoD-14's faults (part A (e))

Made before `faults.toml` exists, from the five faults as plan 0029 section 8 describes them; to be compared with the run when the exact edits are written (criterion 11). "The identity suites" means `core.identities`, `core.dual_identities`, `core.transform_identities`, and the new sources of part B, judged without the golden-value tests.

| Fault | Predicted | Why |
| --- | --- | --- |
| `reorder_sign` negated for one class of pairs (`bits.hpp`) | killed, by associativity of ⟑ (criterion 1) and the defining relation (criterion 2) | The complements also use `reorder_sign`, so the change reaches products and complements alike, but a class of pairs that is not a coboundary (a change of every e_A to ±e_A) breaks associativity; if the class holds a pair of generators e_j, e_i with j > i, e_i e_j + e_j e_i is no longer 2 g_ij. A class that is a coboundary would be an equivalent mutant, which the minimal edit should avoid. |
| one complement sign flipped (`bits.hpp`) | killed, by the De Morgan laws (existing) | The antiproducts are conjugated by the complements; one wrong sign breaks left ∘ right = id or right(a ⟑ b) = right(a) ⟇ right(b) on the pairs through that blade, which every full-layout group reaches. If the flipped sign is used consistently by both complements, so that left ∘ right stays the identity, the antiproducts stay associative, and only the De Morgan right law and the antiscalar identity catch it (the latter only if the blade is 1 or 𝟙). |
| the last term of a blade product dropped (`blade_product.hpp`) | killed on the non-diagonal groups, by associativity of ⟑ and ⟇ and the defining relation | A diagonal product has one term per pair; the extra terms come from off-diagonal entries, which the dense, Witt, and conformal forms have. |
| one coefficient doubled (`blade_product.hpp`) | killed if the doubling is in one pair or one path; survives every identity except criterion 2 if it doubles every square g_ii | Doubling every square consistently is the algebra of another metric, in which every identity holds; criterion 2 compares with `Alg::value.gram` computed in the test, so it alone catches that. |
| two blades of one grade swapped in the 1.7 order (`canonical_less`) | survives the identity suites | Their comparisons are blade by blade on masks (`value_on`), whatever the layout order; the order is pinned by the golden `result_order` tests, outside these suites. |

## F. Facts for the markers and the trace table

Phrases a test can state, matched as the trace table matches facts (each is short, and none contains `|`):

- 2.1: "the antigrade involution of a blade of antigrade m is (-1)^m times it"; "the antigrade involution is (-1)^n times the grade involution"; "right(x') is the grade involution of right(x)".
- Criterion 3 (4.2's product rules have no CONVENTIONS row of their own): "the grade involution of an antiproduct is (-1)^n times the antiproduct of the grade involutions".
- 3.3 and 4.4: the existing rows of C6.

## Not verified

- The Terathon library itself (section 8's "Dual sign" row): not read, as before; D1 shows only that the test separates the two sign choices.
- Route 2 of C1 in non-diagonal forms is checked by agreement on rigid(3) and conformal(3) only, not proved.
- B2's figures are predictions; nothing was compiled for them. E's table is a prediction.
- n = 1 for A2 rests on the derivation alone.
