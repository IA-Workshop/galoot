# 0026 The exact change of basis: what it preserves, the readings behind 1.4 and DoD-16, and part A's predictions

Written by: Claude (agent, Opus 5.5, math-expert role), 2026-11-09 UTC, for WU12 part A (plan 0030, section 9 step 2, items (a) to (g)), on `feature/core-basis-change--math` from `develop` at `a6307ba`. Item (h), the Sage cross-check, comes later and will be added here. Item (h) is recorded in `records/cross-checks/0030-sage-change-of-basis/README.md` (0 mismatches over 63 changes and 71,864 image entries, both directions), not in this note. Not yet reviewed by the owner or by the unit's blind reviewer.

## Departures and open points first (C20)

1. **The probe's hypotheses are corrected in three places** (section A9). The antiproduct family (⟇, the weight dual, the antidot, compose) carries the factor 1/det P, not det P; the bulk and the weight are preserved with the factor 1, never det P; and "preserved only by an orthogonal P", in plan 0030's draft of 1.8, is wrong for degenerate forms: rigid(3) with e4 ↦ 2e′4 keeps the bulk and the weight though P is not orthogonal. The condition the probe suggested for the antiproduct family, P⁻ᵀ G′ P⁻¹ = G, is proved necessary and sufficient.
2. **For a nondegenerate form, the antiproduct family cannot tell det P from 1/det P.** Its condition forces det P = ±1 (A6). Plan 0030's risk 1 asks part B to test a change with |det P| ≠ 1; for the antiwedge and the bulk dual any change with |det P| ≠ 1 does, but for ⟇, the weight dual, the antidot, and `transform` only a change of a degenerate form can. I recommend rigid(3) with e4 ↦ 2e′4 (det P = 2) for those (section H).
3. **Two published equations are wrong as printed**, found by recomputation (V9): Hestenes's "Old Wine in New Bottles" eq. 7 writes E = e ∧ e* = e₊ ∧ e₋, but e₊ ∧ e₋ = −e ∧ e* for every λ; Doran, Hestenes, Sommen, and Van Acker's eq. 3.34 writes w_i w*_j + w_j w*_i = δ_ij where their eqs. 2.5 and 3.7 give w_i w*_j + w*_j w_i = δ_ij (B3, B4). Neither affects a Galoot convention; both are recorded here and in the knowledge base.
4. **The job pool was not running**, and my brief and role forbid starting it, so no CTest ran. I ran the commands those CTests run (`golden.py --check`, `test_references.py golden`, `markers`, `citations`, and the trace gate) directly through `tools/timed`, and compiled `golden.hpp` alone with `g++` and `clang++ -fsyntax-only`, one small compile each, outside the pool. Plan 0030 section 10 lists `reference.golden` and `tests.golden_facts` on gcc-debug for part A; the commands are the same, the CTest wrapper did not run.
5. **The trace table gains a row** (`1.4 | the change of basis sends e_A to the sum of det P[A, B] e'_B | no`), because a new golden namespace needs its fact in the same change (the table's rules). It sits under 1.4 because 1.8 does not exist until 1.17 is ratified; at ratification the row moves to 1.8, and part C sets it to `yes` when `basis_change.hpp` lands.
6. **The cost and overflow predictions rest on choices part B and part C have not made**: which LDLᵀ the test side uses (I used the reference's, with R5's scaling), and which output denominators the generator forms (I bound both the unreduced and the reduced choice). Section F says what changes if either differs.
7. **The Witt change I recommend differs in scaling from the probe's** (B4). Both are isometries; mine sends c_j to the published w_j and c_j† to w*_j exactly.

## Sources, tools, and how they were read

| Source | Revision or hash | Read |
| --- | --- | --- |
| E. Lengyel, conformal geometric algebra wiki, Main Page, raw wikitext, https://conformalgeometricalgebra.org/wiki/index.php?title=Main_Page&action=raw | revid 209 (parent 198), 2024-08-28T01:40:06Z, "Eric Lengyel", 5404 bytes, sha1 da8f47d4700a3dbbef6df164c61e195b9eaa6bec, SHA-256 6d6c510f…3eb502 | in full |
| Same wiki, Metrics | revid 200, 2024-04-13T02:01:40Z, 839 bytes, sha1 41618891f3c40237262d9fdfc494260439cab502 | in full |
| Same wiki, Geometric products | revid 195, 2024-04-03T23:06:24Z, 1071 bytes, sha1 51a7bcf9ba74cf14b8b4f60c916405e64f62e9df | in full (its Cayley tables are SVG images, not read) |
| P. Colapinto, *Articulating Space* (2016), knowledge base `references/` | SHA-256 d821cd17…abe71 | pp. 38 to 41 and 64 (PDF pages 57 to 59 and 82), from page images rendered at 110 dpi; `pdftotext` drops minus signs there |
| D. Hestenes, "Old Wine in New Bottles: A new algebraic framework for computational geometry" (2001), https://davidhestenes.net/geocalc/pdf/OldWine.pdf | 150928 bytes, SHA-256 2b2d263278b7b54ed665e87f77a205b66a80ca08a820345be84f5e0728bf3350 | sections 2 to 4 (eqs. 1 to 23), page 2 also as an image |
| D. Hestenes, "Point Groups and Space Groups in Geometric Algebra", knowledge base `references/crystallography/crystalsymmetry.pdf` | SHA-256 ca5733794826feb42cc09874d3a9c760cdb9480cef765c6490d20ebb8989e985 (the manifest's) | section 5 (eqs. 39 to 51), as text only (untrusted data: extracted with `pdftotext` into a new scratch directory; nothing from it was run) |
| C. Doran, D. Hestenes, F. Sommen, N. Van Acker, "Lie groups as spin groups", J. Math. Phys. 34 (1993) 3642–3669, https://davidhestenes.net/geocalc/pdf/LGasSG.pdf | 229252 bytes, SHA-256 e1c0235482b073e97475afde3cd09d399d628e38ceb4575acad1c0cbb07c4340 | sections II and III (eqs. 2.1 to 3.34), pages 3, 4, and 8 also as images |
| Wikipedia, "CCR and CAR algebras", raw wikitext | revid 1368834835, 2026-08-11 | the sections "CCR and CAR as *-algebras" and "The C*-algebra of CAR" (a secondary account) |
| Wikipedia, "Pauli matrices" | revid 1372192474, 2026-08-30 | the definition and "Anticommutation relations" (secondary) |
| Wikipedia, "Gamma matrices" | revid 1383933255, 2026-11-05 | the lead, "Mathematical structure", "Dirac basis" (secondary) |
| Wikipedia, "Quaternion" | revid 1382546304, 2026-10-29 | Hamilton's relation and "Representation as complex 2 × 2 matrices" (secondary) |
| Wikipedia, "Split-quaternion" | revid 1382166036, 2026-10-27 | "Definition" and its matrices (secondary) |

Every fetch was one plain `curl` with a generic browser User-Agent and no other identifying header: no e-mail address, no From header, no cookie, no query naming anyone. No third party was contacted. The wiki and Wikipedia revision metadata came from each site's `api.php`, and the SHA-1 of the bytes read equals each revision's sha1. I read the texts myself, with no summarizing model between. No passage of any source addresses its reader with instructions; I scanned the two PDFs' extracted text for such passages and found none.

Tools: CPython 3.14.9 (standard library) for the committed reference scripts and the spike scripts that use only them; galgebra 0.6.0 with SymPy 1.14.0 in math-tools' `toolset/env/py/.venv` (CPython 3.14.9) for `galgebra_check.py` and `readings_check.py`. galgebra is of a different lineage and shares no code with Galoot or its reference scripts. The scripts and their outputs are committed in `records/spikes/wu12-part-a/`; each output names its command, and every run went through `tools/timed`.

Evidence labels used below: **Proved** (my derivation, written out), **Checked** (an exact computation, with its script and scope), **Read** (stated in a source at the cited place), **Inferred** (my conclusion, not checked).

## A. What a change of basis preserves (part A (a), criterion 13)

### A0. Setting and notation

Galoot's conventions throughout (CONVENTIONS 1.1, 2.1, 3.1 to 3.3, 4.1, 4.2, 4.4, and `dual.hpp`'s comment). The source algebra Cl(G) has generators e1 to en with Gram matrix G, e_i · e_j = G_ij; the target Cl(G′) has e′1 to e′n with G′. Both are symmetric rational matrices; neither need be invertible.

A change of basis is an invertible rational n × n matrix P whose row i writes e_i in the target's generators, e_i ↦ Σ_j P_ij e′_j, with

    P G′ Pᵀ = G                                                        (isometry)

which says that the images of the generators have the source's inner products: (Σ_k P_ik e′_k) · (Σ_l P_jl e′_l) = (P G′ Pᵀ)_ij.

For a set of generators A = {a1 < … < ak}, P[A, B] is the k × k submatrix of P with rows A and columns B. The map φ is

    φ(e_A) = Σ_{|B| = k} det P[A, B] e′_B,

extended linearly. Q = P⁻ᵀ is the contragredient of P, and ψ is defined from Q as φ is from P. Write d = det P.

The coordinate pairing ⟨x, y⟩ = Σ_A x_A y_A takes the basis blades as orthonormal. It depends on the basis; it is not the dot. C(M) is the compound matrix of M: block diagonal over grades, with C(M)_{AB} = det M[A, B]. Then the coordinate column of φ(x) is C(P)ᵀ[x], and that of ψ(x) is C(Q)ᵀ[x] = C(P⁻¹)[x]. The metric exomorphism of 3.2 is C(G), so bulk(x) has coordinates C(G)[x], and x • y = [x]ᵀ C(G) [y] (4.1).

One standard fact is used throughout, the **Cauchy–Binet formula**: C(MN) = C(M) C(N), C(Mᵀ) = C(M)ᵀ, and C(I) = I. (Background, not re-read in a source.)

### A1. φ is an algebra isomorphism of the wedge-basis tables

**Lemma 1 (outermorphism; any P, no metric).** φ(e_A) = f(e_a1) ∧ … ∧ f(e_ak), where f(e_i) = Σ_j P_ij e′_j; and φ(x ∧ y) = φ(x) ∧ φ(y) for all x, y, with φ(1) = 1.

*Proof.* Expand the wedge of the k vectors f(e_ai) by multilinearity. A term picks one column j_i for each row a_i; it vanishes when two columns coincide, and otherwise equals the sign of the permutation that sorts (j_1, …, j_k) times e′_B, B = {j_1, …, j_k}. Grouping by B gives Σ_σ sgn σ Π_i P_{a_i, b_σ(i)} = det P[A, B], Leibniz's formula. For blades: if A and B share a generator, e_A ∧ e_B = 0, and φ(e_A) ∧ φ(e_B) wedges two equal vectors f(e_c), so it is 0 too; otherwise e_A ∧ e_B = s e_{A∪B}, with s the sign of sorting A followed by B, and the same reordering of the f-vectors gives φ(e_A) ∧ φ(e_B) = s φ(e_{A∪B}). Linearity gives the rest. ∎

**Theorem 1 (algebra map).** If P G′ Pᵀ = G, then φ(x ⟑ y) = φ(x) ⟑′ φ(y) for all x and y.

*Proof.* The linear map f from the source's vectors into Cl(G′) satisfies f(v) ⟑′ f(v) = (vᵀ P) G′ (Pᵀ v) = vᵀ G v, the square of v in Cl(G). The universal property of the Clifford algebra (the knowledge base's `structures/universal-property-and-functoriality.md`, after Gallier, Def. 1.3) then gives exactly one algebra map Φ: Cl(G) → Cl(G′) with Φ(e_i) = f(e_i). It remains to show Φ = φ on every basis blade. Galoot's blade e_A is the wedge e_a1 ∧ … ∧ e_ak, not the geometric product (CONVENTIONS 1.1), and in a Clifford algebra over the rationals the wedge of k vectors is their antisymmetrized product, v_1 ∧ … ∧ v_k = (1/k!) Σ_σ sgn σ v_σ(1) ⋯ v_σ(k) (the identification of the wedge basis that `clifford_reference.py` checks on every blade of its tables, check 3 of `validate`). An algebra map takes an antisymmetrized product of vectors to the antisymmetrized product of their images, so Φ(e_A) = f(e_a1) ∧′ … ∧′ f(e_ak), which is φ(e_A) by Lemma 1. Two linear maps that agree on a basis are equal. ∎

The proof needs the wedge basis: in the ordered-product basis e_a1 ⟑ ⋯ ⟑ e_ak the same Φ mixes grades (the knowledge base's functoriality note, checked there against Sage).

**Corollary 1.** φ keeps grades (a minor pairs sets of one size), φ(1) = 1′, and for invertible P its inverse is the change by P⁻¹: P⁻¹ G P⁻ᵀ = G′, so P⁻¹ is an isometry the other way, and since φ_R ∘ φ_P = φ_{PR} on generators (row convention), φ_{P⁻¹} ∘ φ_P = φ_I = identity on generators, hence on everything by Lemma 1.

**Theorem 2.** φ(𝟙) = d 𝟙′. *Proof.* The top minor of P is det P. ∎

### A2. Two lemmas about coordinates

**Lemma 2 (adjoint pair; any invertible P).** ⟨φ(x), ψ(y)⟩′ = ⟨x, y⟩.

*Proof.* [φx]ᵀ[ψy] = [x]ᵀ C(P) C(P⁻¹) [y] = [x]ᵀ[y] by Cauchy–Binet. ∎

**Lemma 3 (complements; any invertible P, no metric).** φ(right(x)) = d · right′(ψ(x)), and the same with left. With P and Q exchanged: ψ(right(x)) = d⁻¹ · right′(φ(x)).

*Proof.* For x and y of one grade k, x ∧ right(y) = ⟨x, y⟩ 𝟙: for basis blades, u ∧ right(u) = 𝟙 is the definition (3.1), and for v ≠ u of grade k, right(u) is ± the blade of the generators u lacks, which shares a generator with v, so v ∧ right(u) = 0. Apply φ, using Lemma 1 and Theorem 2: φ(x) ∧′ φ(right y) = d ⟨x, y⟩ 𝟙′. By the same identity in the target and Lemma 2, φ(x) ∧′ right′(ψ y) = ⟨φx, ψy⟩′ 𝟙′ = ⟨x, y⟩ 𝟙′. Subtracting, φ(x) ∧′ [φ(right y) − d right′(ψ y)] = 0 for every x of grade k. φ maps grade k onto grade k (it is invertible and keeps grades), and the wedge pairing of grade k with grade n − k into grade n is nondegenerate, so the bracket is 0. For left: left(u) = (−1)^(k(n−k)) right(u) on grade k (3.1), and φ and ψ keep grades. Exchanging the roles of P and Q uses Q⁻ᵀ = P and det Q = 1/d. ∎

**Lemma 4 (dot and bulk; an isometry).** If P G′ Pᵀ = G: (i) φ(x) •′ φ(y) = x • y; (ii) bulk′(φ(x)) = ψ(bulk(x)).

*Proof.* (i) [φx]ᵀ C(G′) [φy] = [x]ᵀ C(P) C(G′) C(Pᵀ) [y] = [x]ᵀ C(P G′ Pᵀ) [y] = [x]ᵀ C(G) [y]. (ii) The coordinates of ψ(bulk x) are C(P⁻¹) C(G) [x] = C(P⁻¹) C(P) C(G′) C(Pᵀ) [x] = C(G′) [φx], those of bulk′(φx). ∎

In words: a complement or a bulk turns something that changes by φ into something that changes by ψ. Everything below combines Lemmas 3 and 4 with Theorem 1.

### A3. The factors for every isometry

**Theorem 3.** For every change of basis (P invertible, P G′ Pᵀ = G):

| Operation | Rule | Factor |
| --- | --- | --- |
| ⟑, ∧ | φ(a op b) = φ(a) op′ φ(b) | 1 (Theorem 1, Lemma 1) |
| dot | φ(a • b) = φ(a) •′ φ(b), a scalar | 1 (Lemma 4 (i)) |
| reverse, antireverse, grade and antigrade involutions | φ(op x) = op′ φ(x) | 1: each multiplies a blade by a sign that depends on its grade and on n, both kept |
| antiwedge ∨ | φ(a ∨ b) = d⁻¹ φ(a) ∨′ φ(b) | 1/d, for every invertible P, with no metric |
| bulk dual ★, left bulk dual | φ(x★) = d φ(x)★′ | d |
| weight | φ(weight x) = d² weight′(ψ(x)) | not a preservation; used in A5 |

*Proof of the antiwedge.* a ∨ b = left(right a ∧ right b) (4.2). By Lemma 3, then Lemma 1 for ψ, then Lemma 3 with roles exchanged: φ(a ∨ b) = d left′(ψ(right a) ∧′ ψ(right b)) = d left′(d⁻¹ right′φ(a) ∧′ d⁻¹ right′φ(b)) = d⁻¹ φ(a) ∨′ φ(b).

*Bulk dual.* x★ = right(bulk x) (3.3). φ(right(bulk x)) = d right′(ψ(bulk x)) = d right′(bulk′(φx)) by Lemmas 3 and 4. The left bulk dual is the same with left.

*Weight.* weight = left ∘ bulk ∘ right (3.2). φ(left(bulk(right x))) = d left′(ψ(bulk(right x))) = d left′(bulk′(φ(right x))) = d left′(bulk′(d right′(ψx))) = d² weight′(ψx). ∎

### A4. The antiproduct family, and its condition

Call (C) the condition that the contragredient is an isometry too:

    Q G′ Qᵀ = G with Q = P⁻ᵀ, that is, P⁻ᵀ G′ P⁻¹ = G.                  (C)

**Theorem 4.** Each of the following holds with some factor f for all inputs exactly when (C) holds, and then f is the one given:

| Operation | Factor under (C) |
| --- | --- |
| geometric antiproduct ⟇ | 1/d |
| weight dual ☆ and left weight dual | 1/d |
| antidot | 1/d |
| compose(Q2, Q1) = Q2 ⟇ Q1 | 1/d |
| transform(x, V), before its result is cut to x's blades | 1/d² |
| project(a, b) and antiproject(a, b) | 1/d² |

*Proof, ⟇.* If (C) holds, ψ is an algebra map of ⟑ by Theorem 1 applied to Q. Then φ(a ⟇ b) = φ(left(right a ⟑ right b)) = d left′(ψ(right a) ⟑′ ψ(right b)) = d · d⁻² left′(right′φa ⟑′ right′φb) = d⁻¹ φ(a) ⟇′ φ(b). Conversely, suppose φ(a ⟇ b) = f φ(a) ⟇′ φ(b) for all a, b. The same steps turn this into d ψ(u ⟑ v) = f d² ψ(u) ⟑′ ψ(v) for all u and v (u = right a ranges over everything). With u = v = 1 this gives f = 1/d, so ψ is an algebra map; on generators, ψ(e_i) ⟑′ ψ(e_j) + ψ(e_j) ⟑′ ψ(e_i) is 2(Q G′ Qᵀ)_ij and must be ψ(2 G_ij) = 2 G_ij: that is (C).

*Weight dual.* ☆ = right ∘ weight = bulk ∘ right (`dual.hpp`; it follows from right ∘ left = identity). Under (C), Lemma 4 (ii) applied to Q reads bulk′(ψx) = φ(bulk x). So φ(bulk(right x)) = bulk′(ψ(right x)) = bulk′(d⁻¹ right′φx) = d⁻¹ (φx)☆′. Conversely, φ ∘ bulk ∘ right = f bulk′ ∘ right′ ∘ φ = f d bulk′ ∘ ψ ∘ right (Lemma 3), and right is invertible, so in coordinates C(Pᵀ) C(G) = f d C(G′) C(P⁻¹). On grade 0 this is 1 = f d; on grade 1, Pᵀ G = G′ P⁻¹, which is (C). The left weight dual differs by a sign per grade.

*Antidot.* The antidot is Σ a_A (left G right)_AB b_B times 𝟙 (`dual.hpp`). The complements are signed permutations of the blades with left = right⁻¹, so the matrix of left is the transpose of that of right, and ⟨a, left z⟩ = ⟨right a, z⟩. Hence a ∘ b = (right a • right b) 𝟙. Then φ(a ∘ b) = d (right a • right b) 𝟙′, while φa ∘′ φb = (right′φa •′ right′φb) 𝟙′ = d² (ψ(right a) •′ ψ(right b)) 𝟙′, which is d² (right a • right b) 𝟙′ under (C) by Lemma 4 (i) for Q. So f = 1/d. Conversely, d (u • v) = f d² (ψu •′ ψv) for all u, v; u = v = 1 gives f = 1/d, and vectors then give (C).

*compose, transform, project, antiproject.* compose is one antiproduct. transform (4.4) is V ⟇ x ⟇ V̰, or −V ⟇ x̌ ⟇ V̰, two antiproducts with the antireverse and the antigrade involution, each kept by φ, so the factor is d⁻². project(a, b) = b ∨ (a ∧ b☆) and antiproject(a, b) = b ∧ (a ∨ b☆): one antiwedge and one weight dual, d⁻¹ · d⁻¹. These directions need only (C) as a sufficient condition; I did not prove that (C) is necessary for them. ∎

**Remarks.**

- **For a nondegenerate form, (C) forces d = ±1.** Taking determinants of P G′ Pᵀ = G and of (C): det G = d² det G′ = d⁻² det G′, so d⁴ = 1 when det G ≠ 0, and d = ±1 over the rationals. Then 1/d = d, which is why the probe, whose changes all had d = ±1, could not see the difference. A factor 1/d ≠ d in the antiproduct family can show only for a degenerate form, such as rigid(3) with e4 ↦ 2e′4 (d = 2), where (C) holds: the null generator's row satisfies (C) for any scale.
- **(C) holds for every isometry between nondegenerate forms whose Gram matrices are their own inverses** (G² = I and G′² = I). Inverting P G′ Pᵀ = G gives P⁻ᵀ G′⁻¹ P⁻¹ = G⁻¹, which is (C) when G⁻¹ = G and G′⁻¹ = G′. This covers 1.4's change (the null pair's block [[0, −1], [−1, 0]] and diag(1, …, 1, −1) are their own inverses), the Witt change to R(m, m, 0), and every change among diagonal forms with squares ±1 (rotations, boosts).
- **Unit versors stay unit only up to d².** If V ⟇ V̰ = λ𝟙, then φ(V) ⟇′ φ(V)̰ = λ d² 𝟙′ under (C).
- **transform cuts its result to the blades of x** (4.4). The rule of Theorem 4 is for the uncut values. For a versor and an x whose transform has no part outside x's blades (the case checking mode verifies), the cut drops zero on both sides, and the rule holds for `transform` as called, with the image of x's layout as the layout on the target side.

### A5. The operations a change of basis does not preserve

**Theorem 5.** For every change of basis:

| Operation | Preserved, with some factor f, exactly when | f |
| --- | --- | --- |
| right and left complements | P is orthogonal: P Pᵀ = I | d |
| bulk | P Pᵀ G = G | 1 |
| weight | G′ Pᵀ P = G′ | 1 |

For a nondegenerate G (or G′) the last two conditions also say P is orthogonal. For a degenerate form they are weaker: rigid(3) with e4 ↦ 2e′4 keeps the bulk and the weight, and R(0, 0, 1) with e1 ↦ 3e′1 does too.

*Proof, complements.* By Lemma 3, φ ∘ right = f right′ ∘ φ exactly when d ψ = f φ on every grade (right′ is invertible). On grade 0, ψ(1) = φ(1) = 1, so f = d; on grade 1, d Q = f P, so Q = P, that is P Pᵀ = I. Conversely Q = P makes ψ = φ.

*Bulk.* By Lemma 4 (ii), bulk′ ∘ φ = ψ ∘ bulk, so φ ∘ bulk = f bulk′ ∘ φ exactly when (φ − f ψ) ∘ bulk = 0, in coordinates C(Pᵀ) C(G) = f C(P⁻¹) C(G). Grade 0 gives f = 1 (bulk(1) = 1); grade 1 gives Pᵀ G = P⁻¹ G, that is P Pᵀ G = G. Conversely, that equation makes C(PᵀG) = C(P⁻¹G) on every grade by Cauchy–Binet.

*Weight.* By Theorem 3, φ ∘ weight = d² weight′ ∘ ψ, so preservation means weight′ ∘ (d² ψ − f φ) = 0. weight′ = left′ ∘ bulk′ ∘ right′ and left′ is invertible; by Lemma 3 (both forms), right′ ∘ ψ = d⁻¹ φ ∘ right and right′ ∘ φ = d ψ ∘ right; so the condition becomes bulk′ ∘ (φ − f ψ) ∘ right = 0, that is C(G′) (C(Pᵀ) − f C(P⁻¹)) = 0. Grade 0 gives f = 1, grade 1 gives G′ Pᵀ = G′ P⁻¹, that is G′ Pᵀ P = G′, and Cauchy–Binet gives the other grades. ∎

### A6. Hand values for criterion 6

Checked (`hand_values.py`, with `conventions_check.py`'s Chevalley product and, for the products, `clifford_reference.py`'s; output `hand-values-out.txt`). "By way of the target" is φ⁻¹(op′(φ(x))).

| Change | Value in the source | By way of the target |
| --- | --- | --- |
| conformal(3), R(4,1,0) | bulk(e4) = −e5 | −½ e5 |
| conformal(3), R(4,1,0) | weight(e4) = e5 | ½ e5 |
| conformal(3), R(4,1,0) | right(e4) = −e1235, and left(e4) = −e1235 (n = 5) | ½ e1235 |
| random 1, diag(2, 1, −62) (the change of section F) | e12 ⟇ e12 = −2 e123 | −124 e123 |
| random 1, diag(2, 1, −62) | antidot(e12, e12) = −2 e123 | −124 e123 |
| random 1, diag(2, 1, −62) | weight_dual(e3) = 2 e12 − 6 e13 + 3 e23 | −2510 e12 − 744 e13 + 186 e23 |

### A7. Checks of the statements

- Checked, exhaustively (`preservation_check.py`, output `preservation-out.txt`): every row of Theorems 1 to 5, the iff of Theorems 4 and 5 included, on every blade and every pair of blades of 29 changes: conformal(1) to (3), both Witt changes, a rotation and a boost, the null shear of R(1,0,1), the permutation from rigid(3) with its null generator first, rigid(3) with e4 ↦ 2e′4, rigid(3) with e1 ↦ e′1 + e′4 and e4 ↦ ⅓e′4, R(0,0,1) with e1 ↦ 3e′1, R(2,0,1) with a rotation and e3 ↦ ½e′3, diag(4, 1) to R(2,0,0), the degenerate form to R(1,1,1), a dense M R(2,1,0) Mᵀ with det M = 3, and the scaled LDLᵀ changes of all ten random forms, the degenerate form, and the two forms whose radical lies along no generator. transform is checked on three random versors of each parity for each change that satisfies (C). Where |d| ≠ 1 the wrong power of d is shown to fail. A control (a matrix that is not an isometry) is caught. Products from `clifford_reference.py`; complements, exomorphism, duals written from their definitions. This sample is exhaustive per change, so for those 29 changes it is a proof (V1 kind 1); for changes in general the proof above is the evidence.
- Checked by a second implementation (`galgebra_check.py`, output `galgebra-out.txt`): φ(a ⟑ b) = φa ⟑′ φb, φ(a ∨ b) = φa ∨′ φb / d, and φ(a ⟇ b) = φa ⟇′ φb / d exactly when (C), on every pair of blades of conformal(2), the one-mode Witt change, rigid(3) with e4 ↦ 2e′4 (d = 2), and M R(2,1,0) Mᵀ (d = 3), with galgebra's products and φ formed as galgebra wedges of the generators' images (no minors). 0 failures, and 49 of 64 pairs fail ⟇/d for the last change, which violates (C), as the theorem requires.
- Checked in `conventions_check.py` (section D) on eleven changes, by a third product (Chevalley's).

### A8. What remains unverified in A

- (C) is proved sufficient for compose, transform, project, and antiproject, not necessary (Theorem 4's last paragraph).
- The antisymmetrization identity used in Theorem 1's proof is standard and checked by `clifford_reference.py` on its tables; I did not re-derive it from the Chevalley construction here (background).
- No machine-checked proof (plan 0030 section 17 leaves it out).

### A9. The probe's hypotheses against the proofs

| Plan 0030 section 5.2 | Proved |
| --- | --- |
| ⟑, ∧, dot, reverse, antireverse, grade involution with f = 1 | Proved (Theorems 1, 3) |
| ∨ with f = det P, "predicted 1/det P" | 1/d for every invertible P (Theorem 3) |
| bulk dual with f = det P | d (Theorem 3) |
| ⟇, weight dual, antidot with f = det P, where P⁻ᵀ G′ P⁻¹ = G | **Corrected:** 1/d, exactly where (C) holds (Theorem 4); equal to d whenever the form is nondegenerate |
| complements, bulk, weight: f = 1 (rotation) or det P (permutation), orthogonal P only | **Corrected:** complements with d exactly for orthogonal P; bulk and weight with 1, under P Pᵀ G = G and G′ Pᵀ P = G′, which for degenerate forms admit non-orthogonal P (Theorem 5) |
| transform, "with the factors part A states" | 1/d² under (C) (Theorem 4) |

## B. Readings for criterion 9 (part A (b))

### B1. Lengyel, the conformal wiki: CONVENTIONS 1.4

Read (Main Page, revid 209, section "Introduction"): the algebra is built by adding "two projective basis vectors called **e**₋ and **e**₊" with "**e**₋ · **e**₋ = −1 and **e**₊ · **e**₊ = +1", and "It is convenient to define two new vectors **e**₄ and **e**₅ as **e**₄ = ½(**e**₋ − **e**₊) and **e**₅ = **e**₋ + **e**₊". The page identifies e4 as the origin and e5 as the point at infinity, writes the point x e1 + y e2 + z e3 + e4 + ½(x² + y² + z²) e5, and adds: "any definitions in which ab = 1/2 make the bivectors **e**₋ ∧ **e**₊ and **e**₄ ∧ **e**₅ equal to each other" (quoted). Read (Metrics, revid 200): the 5 × 5 metric with −1 at (4, 5) and (5, 4).

Translation: e₊ is Galoot's e′4 (squares to +1) and e₋ is e′5 (squares to −1) in R(4, 1, 0). The wiki's relation is then exactly 1.4's: origin = ½(e′5 − e′4), infinity = e′5 + e′4. Recomputed (`readings_check.py`, B1): both null, e4 · e5 = −1, the point formula equals the wiki's e₋, e₊ form, (a(e₋ − e₊)) ∧ (b(e₋ + e₊)) = 2ab e₋ ∧ e₊, and so e45 = e₋ ∧ e₊ = −e′45: det P = −1, as 1.17's 1.4 states. Research note 0005 (C1) read only the Metrics page and found no relation to e₊, e₋; the Main Page states it, so 1.4 can carry Confirmed beside Checked. The wiki treats the three-dimensional case; the relation is the same for every d.

### B2. Colapinto, *Articulating Space*

Read, from the page images: eq. 2.39, n∞ = e₋ − e₊ and n_o = (e₋ + e₊)/2; eq. 2.40, e₊ = n_o − n∞/2 and e₋ = n_o + n∞/2; the text "The weighting is such that their inner product is −1"; Table 2.4 with −1 between n_o and n∞; eq. 2.41, n_o n∞ = n_o · n∞ + n_o ∧ n∞ = −1 + E; eq. 2.42, E = n_o ∧ n∞ = ½(e₋ + e₊) ∧ (e₋ − e₊) = e₊e₋; eq. 2.43, E² = 1 (pp. 39 to 41). The caption of Table 2.6 (p. 64): "With e₋² = −1 and e₊² = 1, then n_o = (e₋ − e₊)/2 and n∞ = e₋ + e₊ represent the point at the origin and the point at infinity, respectively" (quoted).

Recomputed (B2): every equation holds as printed. Eq. 2.39 is 1.4 with e₊ ↦ −e₊: det +1, and its E = e₊e₋ is the negation of the bivector that 1.4's pair gives. The caption of Table 2.6 is 1.4 exactly (det −1, n_o ∧ n∞ = −e₊e₋). This confirms the knowledge base's record of the thesis using both choices; Galoot matches the caption and Lengyel. Eq. 2.41's −1 + E is CONVENTIONS 1.1's e4 ⟑ e5 = −1 + e45, which holds in either choice.

### B3. Hestenes, "Old Wine in New Bottles"

Read: eq. 1, a null basis {e, e*} of the Minkowski plane with e² = e*² = 0 and e · e* = 1; eq. 2, E = e ∧ e*, E² = 1; eqs. 3 to 5, ee* = 1 + E, e*e = 1 − E, Ee = −eE = e and e*E = −Ee* = e*; eq. 6, e± = (λe ± λ⁻¹e*)/√2 with λ ≠ 0; eq. 7, "e±² = ±1, e₊ · e₋ = 0, E = e ∧ e* = e₊ ∧ e₋ = e₊e₋" (quoted); eqs. 16 and 20, points x with e · x = 1, x = xE − ½x²e + e*, so that e* is the origin and e a point at infinity (p. 4).

Recomputed (B3, symbolic in λ): eqs. 1 to 6 and the first two clauses of eq. 7 hold; but e₊ ∧ e₋ = −e ∧ e* for every λ, so **eq. 7's E = e₊ ∧ e₋ is wrong in sign**: E = e₋ ∧ e₊. This is an erratum of the source (V9), not a Galoot disagreement: Galoot takes no convention from eq. 7.

Translation: Hestenes pairs his null vectors with +1, Galoot with −1. Put o = e* (the origin) and ∞ = −e; then o · ∞ = −1 and x + o + ½x² ∞ = x − ½x² e + e*, eq. 20's point in additive form. With λ = 1/√2, e₊ = ½e + e* and e₋ = ½e − e* have no square roots, and with e′(d+1) = −e₊, e′(d+2) = −e₋ (squares +1 and −1), o = ½(e′(d+2) − e′(d+1)) and ∞ = e′(d+2) + e′(d+1): 1.4 again, with o ∧ ∞ = −e′(d+1) ∧ e′(d+2). Checked in B3.

The crystallography paper (section 5, text only) uses the same pairing: points satisfy x · e = 1 (eq. 39), the conformal split x = xE − ½x²e + e0 (eq. 47) with E = e0 ∧ e and E² = 1 (eq. 48), and R4,1 = R3 ⊗ R1,1 generated by e0 and e (eq. 50). It states no orthonormal basis and no change to one, so it gives 1.4 nothing beyond the null pair's pairing, which agrees with Old Wine.

### B4. A published statement of the CAR, and the Witt change

Read (Doran, Hestenes, Sommen, Van Acker): eq. 2.5, w*_i · w_j = ½δ_ij; eqs. 2.7a and 2.7b, w_i · w_j = 0 and w*_i · w*_j = 0, so w_i w_j = −w_j w_i and w*_i w*_j = −w*_j w*_i; eq. 3.7, a · b = ½(ab + ba); eq. 3.16, e_i = w_i + w*_i and ē_i = w_i − w*_i, with e_i · e_j = δ_ij and ē_i · ē_j = −δ_ij (eq. 3.17); the text "The basis {w_i, w*_i} is called a Witt basis"; and eq. 3.34, printed "w_i w*_j + w_j w*_i = δ_ij", "the fundamental equation for fermion operators" (quoted, p. 8).

Recomputed (B4): eqs. 2.5, 2.7, 3.16 hold, and with eq. 3.7 they give w_i w*_j + w*_j w_i = δ_ij. **Eq. 3.34 as printed fails** (for i = j it gives 1 + 2 w_i ∧ w*_i); the second term should read w*_j w_i. Read (Wikipedia, "The C*-algebra of CAR", a secondary account): b(f) b*(g) + b*(g) b(f) = ⟨f, g⟩ and b(f) b(g) + b(g) b(f) = 0; on an orthonormal basis, {b_j, b_k*} = δ_jk and {b_j, b_k} = 0, and {b_j*, b_k*} = 0 follows by taking adjoints. Both sources agree once eq. 3.34 is corrected.

In the scaled Witt form of the test list (e(2j−1) · e(2j) = 1, the rest 0), c_j = e(2j−1) and c_j† = ½ e(2j) satisfy c_j · c_j† = ½, eq. 2.5's normalization, and {c_j, c_k†} = δ_jk, {c_j, c_k} = {c_j†, c_k†} = 0. Checked (B4) for one and two modes.

The Witt change I recommend for parts D and B sends e(2j−1) ↦ ½(e′_j + e′(m+j)) and e(2j) ↦ e′_j − e′(m+j) into R(m, m, 0), where e′1 to e′m square to +1 and e′(m+1) to e′(2m) to −1. It is an isometry (checked) with det P = −1 for m = 1 and m = 2, and it sends c_j to the published w_j and c_j† to w*_j, with DHSV's e_j = e′_j and ē_j = e′(m+j). The probe's change (e1 ↦ e′1 + e′2, e2 ↦ ½(e′1 − e′2)) is also an isometry; it sends c to 2w and c† to ½w*, a rescaling the CAR does not see.

### B5. The matrix representations (O1 kept them)

Read (Wikipedia, raw wikitext, revisions above):

- Pauli: σ1 = [[0, 1], [1, 0]], σ2 = [[0, −i], [i, 0]], σ3 = [[1, 0], [0, −1]], with {σ_j, σ_k} = 2δ_jk I, "the generators of a representation of the Clifford algebra for ℝ³".
- Gamma matrices, Dirac basis: γ⁰ = diag(1, 1, −1, −1), γᵏ = [[0, σᵏ], [−σᵏ, 0]] (equivalently iσ² ⊗ σᵏ), with {γ^μ, γ^ν} = 2η^(μν) I₄ and η of signature (+ − − −); the four matrices are written out.
- Quaternions: i² = j² = k² = ijk = −1, and a + bi + cj + dk ↦ [[a + bi, c + di], [−c + di, a − bi]], so i = diag(i, −i), j = [[0, 1], [−1, 0]], k = [[0, i], [i, 0]]; "Constraining any two of b, c, and d to zero produces a representation of complex numbers".
- Split-quaternions: i² = −1, j² = k² = 1, ij = k = −ji, with i = [[0, 1], [−1, 0]], j = [[0, 1], [1, 0]], k = [[1, 0], [0, −1]], an isomorphism onto the real 2 × 2 matrices.

Index convention, fixed first as the knowledge base's classification note asks: Galoot's R(p, q, r) puts the p generators of square +1 first (CONVENTIONS 1.2), which is the Cl(p, q) of Gallier and of the knowledge base, and the opposite of nLab's Cl_{s,t}. So R(3, 0, 0) is the Pauli algebra, R(1, 3, 0) is the Dirac algebra with e1 = γ⁰ and e2, e3, e4 = γ¹, γ², γ³ (η = (+ − − −) puts the one positive square first), R(0, 1, 0) ≅ ℂ with e1 = diag(i, −i), R(0, 2, 0) ≅ ℍ with e1 = i and e2 = j (so e12 = k), and R(1, 1, 0) ≅ M₂(ℝ) with e1 = split j (square +1) and e2 = split i (square −1), so e12 = ji = −k.

Recomputed in `golden.py` (section C2) for each: the anticommutators e_i e_j + e_j e_i = 2 g_ij I from the matrices, Hamilton's relations and the split-quaternion relations where they apply, faithfulness (the 2ⁿ blade matrices are linearly independent over the reals, as real vectors of real and imaginary parts: rank 2, 4, 4, 8, 16), and each product table equal to `clifford_reference.py`'s. Every published relation held as printed.

## C. Golden values (part A (c))

The rule of `golden.py` (its docstring): a value is written only where two routes agree. Added at `415f07c`.

C1. `basis_changes`, for 1.4: per d = 1 to 4, `d<d>_to_diagonal`, the image of every blade of conformal(d) under the change to R(d+1, 1, 0), and `d<d>_from_diagonal`, the image of every blade of R(d+1, 1, 0) under the inverse, as `change_image` rows of rational terms, indexed by mask. Route 1, `clifford_reference.py`: minors by elimination, the inverse by Gauss–Jordan. Route 2, `conventions_check.py`: each image as the wedge of its generators' images (no minor formed by name), the inverse as the adjugate over det P with Leibniz cofactors, and its own copy of the rows of P. The routes agree on all 2 · (8 + 16 + 32 + 64) = 240 images, each map undoes the other on every blade, and the antiscalar goes to its negation. Fact: "the change of basis sends e_A to the sum of det P[A, B] e'_B" (new trace row, item 1.4, not implemented). Criterion 2's hand values are there: in d = 3, e4 ↦ (−½ e′4 + ½ e′5) at mask 0b01000, e5 ↦ e′4 + e′5, e45 ↦ −e′45, 𝟙 ↦ −𝟙′.

C2. `representations`, for DoD-16 and criterion 9 (c): the product tables `r010`, `r020`, `r110`, `r300`, `r130` as `product` rows (4, 16, 16, 64, and 256 rows), computed by multiplying the published matrices of B5, checked as B5 says, and equal to `clifford_reference.py`'s tables (route 2, no matrices). Fact: "e_i e_j + e_j e_i = 2 g_ij", the existing 4.1 row, which states what the published matrices satisfy.

C3. Checks run (see the run list at the end): `golden.py --check` matches; `test_references.py golden` ("every use of golden values states its fact"); the trace gate passes with only 6.1 to 6.4 untested; `golden.hpp` compiles alone under GCC and Clang with `-Wall -Wextra -Werror`, and two `static_assert`s on the new arrays hold.

## D. `conventions_check.py` (part A (d))

Added at `dc36961` (comment wording fixed at `eb5b6ad`):

- `Form`: the Clifford algebra of any symmetric Gram matrix by Chevalley's construction (e_i x = e_i ∧ x + e_i ⌋ x, and e_a ∧ R = e_a R − e_a ⌋ R for a blade), with complements, grade signs, exomorphism by Gram minors, duals, products, antiproducts, dot, antidot, and the uncut transform. `check_form_product_against_diagonal` checks it against the script's single-term rule on rigid(3) and R(1,3,0); I also compared it, outside the script, with `clifford_reference.py` on every pair of blades of conformal(3), random form 3, and the degenerate form (0 mismatches).
- `check_conformal_change`, for 1.4: for d = 1 to 4, P D Pᵀ = G, det P = −1, each Euclidean e_i ↦ e′_i, the origin ↦ (e′(d+2) − e′(d+1))/2, infinity ↦ e′(d+2) + e′(d+1), e(d+1)(d+2) ↦ −e′(d+1)(d+2), 𝟙 ↦ −𝟙′, 1 ↦ 1, grades kept, the adjugate inverse undoing it on every blade, and the homomorphism of the two forms' products on every pair of blades (d ≤ 3) or of generators (d = 4); and the basis-dependence of the bulk, bulk(e4) = −e5 against −½ e5.
- `check_change_preserves`, for 1.8: on eleven changes (conformal(1) and (2), both Witt changes, a rotation, the null shear, rigid(3) null first, rigid(3) with e4 doubled, diag(4, 1) to R(2,0,0), the degenerate form to R(1,1,1), and random form 1 to diag(2, 1, −62)), every statement of Theorems 1 to 5 except compose, project, and antiproject, on every blade and pair of blades, with the iff conditions and the told-apart factors where |d| ≠ 1.

The new checks draw from their own random source, so the earlier checks' draws are unchanged; the script passes (2.8 s). A sanity mutation outside the script (the e(d+1) column of P negated) produced 20 failures.

## E. CONVENTIONS 1.17, draft text (part A (e))

Labels as the checks of section D earn them. The draft is for the owner's ratification at the merge (O4); CONVENTIONS itself is not edited here.

> 1.4 `conformal(d)` has e1 to ed Euclidean, e(d+1) the origin, and e(d+2) the point at infinity, with both of the last two null and e(d+1) · e(d+2) = −1. Its diagonal counterpart is R(d+1, 1, 0), whose generators are written e′1 to e′(d+2): e′(d+1) squares to +1 and e′(d+2) to −1. The change of basis from conformal(d) to R(d+1, 1, 0) (1.8) keeps e_i = e′_i for i ≤ d and writes the origin as ½(e′(d+2) − e′(d+1)) and the point at infinity as e′(d+2) + e′(d+1). Its determinant is −1, so e(d+1)(d+2) maps to −e′(d+1)(d+2) and 𝟙 to −𝟙′. (Checked; Confirmed for d = 3, research note 0026)
>
> 1.8 A change of basis from an algebra with Gram matrix G to one with Gram matrix G′ on the same number n of generators is an invertible rational matrix P whose row i writes e_i in the generators e′1 to e′n, with P G′ Pᵀ = G. It sends e_A to Σ_B det P[A, B] e′_B over the blades B of A's grade, so it keeps grades and sends 1 to 1′ and 𝟙 to det(P) 𝟙′, and its inverse is the change by P⁻¹. Write φ for it and d = det P. It preserves the geometric product, the wedge, the dot, the reverse, the antireverse, and the grade and antigrade involutions; φ(a ∨ b) = φ(a) ∨ φ(b) / d, and φ(u★) = d φ(u)★, as for the left bulk dual. The geometric antiproduct, the antidot, the weight duals, and compose are preserved exactly when P⁻ᵀ G′ P⁻¹ = G, each with the factor 1/d, and transform, project, and antiproject then with 1/d²; for a nondegenerate form this condition makes d = ±1, and it holds for every change between forms whose Gram matrices are their own inverses, such as the change of 1.4 and that of a Witt form to R(m, m, 0). The complements are preserved, with the factor d, exactly when P is orthogonal; the bulk exactly when P Pᵀ G = G, and the weight exactly when G′ Pᵀ P = G′, each with the factor 1, which for a nondegenerate form also mean that P is orthogonal. So the complements, the bulk, and the weight depend on the basis: in conformal(3) the bulk of e4 is −e5, while by way of R(4, 1, 0) it is −½ e5. (Checked on eleven changes, the general statements proved in research note 0026)

> Section 8, "Conformal duals", Choice: "3.3, the definition; confirmed by the owner on 2026-11-09 ("Accept all recommendations", decision 0219 item 5)".
>
> Revision history, 1.17: "1.4 names the generators of R(d+1, 1, 0) e′1 to e′(d+2), states the blade map and that 𝟙 maps to −𝟙′, and is confirmed against the conformal wiki's Main Page (research note 0026); 1.8 states what a change of basis preserves and with which factor; section 8's conformal-duals row records the owner's confirmation (decision 0219 items 4 and 5)".

With 1.17, the trace table's new row moves from 1.4 to 1.8, and part C adds the facts of 1.8 that its tests state.

## F. Cost and overflow predictions (part A (f))

`cost_and_overflow.py`, output `cost-and-overflow-out.txt` (75 s), from `docs/agent/snippets/plan_cost_model` with the weights of `records/performance/plan-cost-model/out/weights-{clang,gcc}-galoot-O0.tsv`. The traced identities are listed in the script's docstring: criterion 4's two products and criterion 5's operations on both sides, the antiproduct family only for the changes that satisfy (C), and the transform of a vector by an even versor for conformal(1) to (3).

F1. Constant-evaluation cost (largest phase; Clang's default is 1,048,576 steps, GCC's 2³³ operations):

| Group | Layouts | Clang | GCC | Verdict |
| --- | --- | --- | --- | --- |
| conformal(1), (2) and R(2,1,0), R(3,1,0) | full × full | 40,367; 196,735 | 213,538; 1,049,394 | under |
| conformal(3) and R(4,1,0) | full × full | 947,839 (antiproduct of full<5>, `fill`) | 5,096,386 | under the default by 10%; its own source under decision 0193's raised limit, as plan 0030 says, keeps a margin |
| conformal(4) and R(5,1,0) | vectors × bivectors; bivectors × bivectors | 154,563; 316,116 | 843,480; 1,719,245 | under |
| conformal(4) | full × full (not proposed) | 5,110,503 | 25,853,002 | over on Clang |
| Witt one and two modes, degenerate, rigid(3) null first | full × full | at most 226,064 | at most 1,203,118 | under |
| random forms of n ≤ 4 and their diagonals | full × full | at most 560,320 (random 2) | at most 3,005,649 | under |
| random 3 and 7 and their diagonals (n = 5) | vectors × vectors, × bivectors, bivectors × bivectors | at most 150,579 | at most 818,606 | under |
| random 3 and 7 (n = 5) | full × full (not proposed) | 6,482,384 and 7,263,222 | 34,022,671 and 38,106,148 | over on Clang |

No loop reaches GCC's 2¹⁸ iterations (the longest is 5,404, at a dense full<5>). The n = 6 and dense n = 5 groups have four phases each outside the fit, given the margin the script describes. These are predictions of the plans and maps only (template instantiation and code generation are not in the model), from weights fitted to the headers of `4895a40`, as note 0024 B3 says.

The change of basis itself is new code the model does not cover. Its size per layout: at most 48 nonzero minors for conformal(3) full (two per output blade), 96 for conformal(4) full, 36 for the two-mode Witt change (four per output blade), and 132 for a dense full<5> LDLᵀ change (ten per output blade). That is small beside a full<5> product plan (1,024 pairs); part C measures it (criterion 15).

F2. Overflow of the test rational. Under the bound of the script's docstring (every intermediate of every operation, both sides, the round trip, and the factor), with integer draws in [−K, K] in the source:

| Group | Largest safe K, unreduced output denominators | With reduced output denominators | Largest denominator of φ(x) |
| --- | --- | --- | --- |
| conformal(1) to (3), with the transform (degree 3) | 197,349 to 356,248 | the same | 2 |
| conformal(4), vectors and bivectors | 303,700,049 | the same | 2 |
| Witt, degenerate, rigid(3) null first | over 10⁸ | the same | at most 4 |
| random 0, 1, 4, 5, 6, 8, 9 | over 2.5 × 10⁷ | the same or more | at most 9 |
| random 7, vectors and bivectors (n = 5) | 767,622 (bivectors × bivectors) | the same | 72 |
| random 2 | 1,238,983 | 2,477,966 | 60 |
| random 3, vectors × vectors | 23,790 | 23,790 | 15,510 |
| random 3, vectors × bivectors | 815 | 21,661 | 10,934,550 |
| random 3, bivectors × bivectors | **9** | 6,802 | 10,934,550 |

So draws in [−9, 9] are provably safe in every group whichever denominators the generator forms, and draws in [−3, 3], the range of the probe and of the reference's random forms, leave a margin. The binding case is random form 3's LDLᵀ change, whose bivector images carry denominators up to 10,934,550 (its diagonal is (3, −30, −470, 517, −165), det P = 2/7755). The bound is a worst case over all draws, so the 50-seed campaign (criterion 14) cannot overflow on the traced computations with K ≤ 9; a computation the script does not trace (a nested product, rational inputs, or another LDLᵀ) needs the bound recomputed with the script.

## G. Gap-mutant predictions (part A (g), criterion 12)

Written before any `gaps.toml` run; the record is `records/mutations/0030-basis-change/gap-predictions.md`, which holds the edits, the argument, and the conditions. In short:

| Gap (plan 0029 coverage table) | Mutant | Branch point (identity suites) | Tip (with `core.basis_change_identities`) |
| --- | --- | --- | --- |
| isomorphism and round trip; agreement across bases | `conformal_pairing_positive` | survives | killed, by the core's isometry check of `conformal_change<D>` (a compile-time rejection, classified `assert`) |
| the same | `diagonal_negative_first` | survives | killed, by the isometry checks of the conformal and Witt changes against `R()` |
| the same | `witt_pairing_doubled`, as `from_gram` doubling off-diagonal entries | survives, unless the doubled dense forms overflow the test rational in the identity suites (coefficients grow to 21,836 in random 7); uncertain | killed, by the Witt change's isometry check |
| test-side LDLᵀ | none can count | — | — |

The LDLᵀ row has no counting mutant by R3's characterization: a mutant survives the branch-point identity suites only if every algebra stays self-consistent (each product the Clifford product of its stored Gram matrix, pinned by the generator rule since plan 0031), and an LDLᵀ test derives both its diagonal counterpart and its change from that same stored matrix, so by Theorem 1 every one of its assertions still holds. The exception, if part B builds a counterpart with `R()` (possible only for the forms whose scaled diagonal is in R's order with squares 1, −1, 0: "radical off the axes" with (1, −1, 0) and "all ones" with (1, 0, 0)), is described in the record.

## H. Recommendations for parts B, C, and D

1. Test a change with |d| ≠ 1 for the antiwedge and the bulk dual (random form 1's LDLᵀ change, d = ½, or any LDLᵀ change with d ≠ ±1), and rigid(3) with e4 ↦ 2e′4 (rows {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,2}, d = 2) for ⟇, the weight dual, the antidot, compose, and `transform`; it is the only kind of change on which 1/d and d differ there (A4).
2. Use the hand values of A6 for criterion 6, and Theorem 5's conditions, not "orthogonal only", in `basis_change.hpp`'s header comment.
3. Compare `transform` across bases on the image of x's layout, or check that the cut dropped nothing (A4, last remark).
4. Take the Witt change of B4 for the named Witt changes, so that criterion 9 (b) can state that c_j and c_j† map to the published Witt basis.
5. Keep criterion 4's conformal and Witt groups in `core.basis_change_identities`; the gap predictions rely on it.
6. Draw integers in [−3, 3] (at most [−9, 9]) for the kernel groups, unless the overflow script is rerun for other draws (F2).
7. Give conformal(3)'s full group its own source under the raised Clang limit; build the dense five on vectors and bivectors and conformal(4) on vectors × bivectors and bivectors × bivectors (F1).

## Run list (V10)

All on `feature/core-basis-change--math`, through `tools/timed`, rows in `records/performance/runs.tsv`:

| Command | Revision | Result |
| --- | --- | --- |
| `.venv/bin/python -I docs/agent/records/spikes/wu12-part-a/preservation_check.py test/reference` | tree of `eb5b6ad` plus the spike files | every statement holds |
| `.venv/bin/python -I docs/agent/records/spikes/wu12-part-a/galgebra_check.py` | same | all agree |
| `.venv/bin/python -I docs/agent/records/spikes/wu12-part-a/readings_check.py` | same | all reading checks passed |
| `python3 -I docs/agent/records/spikes/wu12-part-a/hand_values.py test/reference` | same | every value differs |
| `python3 -I docs/agent/records/spikes/wu12-part-a/cost_and_overflow.py .` | same | the tables of F |
| `python3 -I test/reference/conventions_check.py` | `eb5b6ad` | all convention checks passed (seed 20261031) |
| `python3 -I test/reference/golden.py --check test/reference/golden.hpp` | `eb5b6ad` | matches |
| `python3 -I tools/test_references.py golden test/reference/golden.hpp test`, `markers test`, `citations …` (as `tests.citations` passes its arguments), and the trace gate | `eb5b6ad` | pass; trace: items with no test 6.1 to 6.4 |
| `g++` and `clang++ -std=c++23 -fsyntax-only -Wall -Wextra -Werror -I test` on a file that includes `golden.hpp` | `eb5b6ad` | compile |

`.venv` is math-tools' `toolset/env/py/.venv` (galgebra 0.6.0, SymPy 1.14.0, CPython 3.14.9).

Verification status (V10): the statements of A are verified by proof and by three exact implementations on the changes listed; the readings of B are validated against the sources as read, with two errata of the sources recorded; the golden values are verified by two agreeing routes. Validation of `basis_change.hpp` itself waits for parts B and C.
