# 0028 The conventions derived again, independently, and what the derivations say

Written by: Claude (agent, Opus 5.5, math-expert role), 2026-11-21 UTC, for plan 0047 unit V3 (sections 6, 6.1, 7 and 9.3 item 1), on `research/conventions-rederived` from `main` at `01dce7f`. The record of rows, tools and rerun commands is [`records/cross-checks/0047-conventions/`](../records/cross-checks/0047-conventions/README.md); this note holds the derivations and what they mean.

## Departures and open points first (C20)

1. **No disagreement was found.** Every derivable claim of CONVENTIONS 1.18 sections 1 to 5 and 7, and the specified parts of section 6, came out as written, and every value of `golden.hpp` and every hand value transcribed from the tests' reference comments was reproduced. An unfavorable finding would have been reported as found; there is none to report.
2. **One reading carries many rows**: the antiexomorphism 𝔾 of 3.2, which CONVENTIONS describes only for a diagonal metric, is read as left ∘ G ∘ right (section 2 below). The record's open item 1 asks the owner whether to state it in CONVENTIONS.
3. **Independence is of code and order, not of construction.** This route and the library's general-form reference are both the Chevalley construction (section 1). What is independent is said in the record's section 2.
4. **Sources.** Shirokov (arXiv:1109.2322v1, section 2) was read for the grade involution. Hestenes' Table 2 was read for the point-group code (in the copy at `knowledge-base/references/crystallography/crystalsymmetry.pdf`). Lengyel's wikis were not re-read; the International Tables were not read.
5. Departures from the brief and the plan are listed in the record's section 1; the open items for the owner are in its section 5.

## 1. The route

**The engine.** For a symmetric bilinear form B on V = span(e1, …, en), each generator acts on the exterior algebra ΛV by L(e_i) = ε(e_i) + ι(e_i): ε is wedging on the left, and ι is the contraction with ι(e_i) e_j = B_ij, extended so that it passes a vector with a sign, ι(v)(a ∧ x) = (ι(v) a) x − a ∧ ι(v) x for a vector a. Then L(v)² = ε² + (ει + ιε) + ι² = 0 + B(v, v) + 0, which is the Clifford relation, so the L(e_i) generate a copy of the Clifford algebra acting on ΛV (Chevalley). The Clifford element that acts on 1 to give the wedge monomial e_a1 ∧ … ∧ e_ak is the antisymmetrized product (1/k!) Σ_σ sgn σ e_σ(a1) ⋯ e_σ(ak), and the product x ⟑ y is the action of x on y. The engine checks this identification on every blade of every form before it is used, and checks that the plain ordered product fails it on conformal(3) (where e4 ⟑ e5 acting on 1 is −1 + e45).

**What else is computed from definitions.** The complements by solving u ∧ s e_C = 𝟙 and s e_C ∧ u = 𝟙 for the sign s; the reverse by reversing the factors of a monomial and counting inversions; the grade involution as the outermorphism of −id; the metric exomorphism as the outermorphism of G, by wedging the images G e_j of the generators (so the minor formula of 3.2 and 1.8 is a second, separate route, through python-flint); the antiproduct and antiwedge by their definitions in 4.2; the transform by 4.4. Blades are tuples of indices, not masks.

**The forms.** Sixteen Gram matrices (listed in the rows file): diagonal ones with zero, one and two null generators, indefinite ones, conformal(1) to (3), a Witt form, two dense nondegenerate forms, and two degenerate non-diagonal forms whose radical lies along no generator. And symbolic Gram matrices with n(n + 1)/2 independent entries for n = 3 and 4.

## 2. Derivations

Each statement holds for every symmetric form unless it says otherwise. The engine confirmed each on the forms above; the record's rows say on which inputs.

**D1, the reverse and the grade involution (2.1).** Reversing k factors of a wedge is a permutation with k(k − 1)/2 inversions, so a grade-k blade reverses to (−1)^(k(k−1)/2) times itself. The grade involution is the automorphism α with α(v) = −v; Shirokov defines it so, basis-free, as "a unique canonical automorphism α … satisfying α ∘ α = id, α(j(v)) = −j(v) ∀v ∈ V" (section 2, quoted). Since a grade-k blade is a wedge of k vectors and α preserves the wedge (the wedge is the antisymmetrized product), α multiplies it by (−1)^k. This is 2.1's grade involution, now with a source read (gap G3).

**D2, complements twice.** If right(e_A) = s e_Ā, then left(right(e_A)) = e_A and left = (−1)^(gr·ag) right on every blade, so right(right(u)) = (−1)^(k(n−k)) u for grade k. Since k(n − k) = kn − k² ≡ k(n − 1) (mod 2), this is the identity for odd n and the grade involution for even n.

**D3, De Morgan for every form (4.2, gap G2).** With a ⟇ b = left(right(a) ⟑ right(b)), the law right(a ⟑ b) = right(a) ⟇ right(b) reduces to right(right(a)) ⟑ right(right(b)) = right(right(a ⟑ b)). By D2, right ∘ right is the identity or the grade involution, and both are automorphisms of ⟑ for every form. The same argument works for the left complement and for ∧ and ∨. Associativity of ⟇ and ∨ follows because they are ⟑ and ∧ conjugated by the bijection right. The engine confirmed the four laws on every blade pair for a general symbolic Gram matrix with n = 3 and 4.

**D4, the antireverse and the antigrade involution.** They are the complement conjugates left ∘ op ∘ right of the reverse and the grade involution. right(e_A) has grade m = n − k, so the signs are (−1)^(m(m−1)/2) and (−1)^m, as 2.1 states. A conjugate of an (anti-)automorphism of ⟑ is an (anti-)automorphism of ⟇. And (−1)^m = (−1)^n (−1)^k gives x̌ = (−1)^n x̂; right(x̌) = (−1)^m right(x), which is the grade involution of right(x) since right(x) has grade m. No published source for the antigrade involution was found (gap G3 stays open on that half; its label is Proposed).

**D5, the dual identity (3.3).** For blades of one grade, e_A ∧ right(e_C) = 𝟙 if A = C and 0 otherwise, because right(e_C) is ± the blade on the generators C lacks. So for a and b of grade k, a ∧ right(G b) = Σ_C (G b)_C a ∧ right(e_C) = (aᵀ G b) 𝟙: a ∧ b★ = (a · b) 𝟙, for every form, degenerate or not. With the left complement the same computation gives (−1)^(k(n−k)) (a · b) 𝟙, so the left dual flips the sign of the identity whenever k(n − k) is odd, as for a point in four dimensions.

**D6, the antiexomorphism and the antidot (3.2, 4.1).** Read 𝔾 = left ∘ G ∘ right. In a diagonal metric right(e_A) = s e_Ā, G multiplies it by the product of the squares of the generators in Ā, and left undoes the sign, which is 3.2's description of the weight. For a nondegenerate form, left ∘ G ∘ right = det(G) · Λ(G⁻¹), the adjugate compound (confirmed on ten nondegenerate forms). The coordinate pairing ⟨x, y⟩ = Σ x_A y_A satisfies ⟨x, left(y)⟩ = ⟨right(x), y⟩, so aᵀ 𝔾 b = right(a) · right(b): the antidot is the dot of the complements.

**D7, the double dual (3.4).** im G is the coordinate-orthogonal complement of the radical, since G is symmetric. G(x) lies in Λ(im G) for every x. The coordinate complement of an element of Λ(im G) is divisible by every radical vector (it is the Euclidean Hodge star of a multivector in a subspace, which carries the volume of the orthogonal complement). G kills anything divisible by a radical vector. So u★★ = 0 for every u when det G = 0, for diagonal and non-diagonal forms alike; for nondegenerate forms u★★ = (−1)^(gr·ag) det(G) u. This is wider than 3.4's "Checked for rigid(3)", and agrees with it.

**D8, a change of basis and the bulk dual (1.8).** If φ preserves ∧ and · and sends 𝟙 to d 𝟙′, then for every a of b's grade, φ(a) ∧ φ(b★) = φ(a ∧ b★) = (a · b) d 𝟙′ = d (φa · φb) 𝟙′ = φ(a) ∧ d φ(b)★′. The wedge pairing between grades k and n − k is nondegenerate and φ is bijective on grade k, so φ(b★) = d φ(b)★′.

**D9, the transform (4.4).** For a hyperplane h (an antivector), h ⟇ antireverse(h) = λ𝟙 with λ = B(right(h), right(h)), so λ ≠ 0 exactly when the hyperplane's normal vector right(h) is not null. With V an antiproduct of such hyperplanes and T as in 4.4, the engine found λ^(k−1) T(a1 ∧ … ∧ ak) = T(a1) ∧ … ∧ T(ak) and T(1) = λ on ten forms, and T(x) = λx for every vector x with x ∨ h = 0. Of the two sign alternatives, +V ⟇ x̌ ⟇ Ṽ fails the mirror property for every vector tried, and the single-sign −V ⟇ x ⟇ Ṽ fails only for even n: for odd n a vector's antigrade is even, x̌ = x on vectors, and only higher grades tell the two apart. The composition rule transform(x, Q2 ⟇ Q1) = transform(transform(x, Q1), Q2) needs only that the antireverse is an anti-automorphism and the antigrade involution an automorphism of ⟇ (D4), and holds for any elements of one parity each.

**D10, the meets (gap G5).** In rigid(3) with the oriented coordinates of 1.6 and 5.3, the symbolic antiwedge gives: g ∨ h has direction n_h × n_g and moment h_w n_g − g_w n_h; (g ∨ h) ∨ k has weight det[n_g; n_h; n_k] and, unitized, is the point Cramer's rule gives for the three plane equations; l ∨ g has weight −(v · n_g); p ∧ q ∧ r ∧ s = det[p; q; r; s] 𝟙 with the rows (x, y, z, w); l1 ∨ l2 = −(v1 · m2 + v2 · m1). The direction of g ∨ h is the opposite of the naive n_g × n_h. The library's `golden::rigid_meets` states the same formulas.

**D11, point-group settings (gap G17).** Hestenes gives each point group by angles between generating unit vectors (Table 2: for example "p q̄" has generators a and bc, with π/p between a and b, π/q between b and c, and a ⊥ c), not by coordinates. For 2/m, −42m and 32 the library's groups and the earlier rebuild's realization of those angles are different sets of matrices; solving X g = h X for the intertwiners of two generators and imposing XᵀX = I, det X = 1, gives in each case a rotation that carries one group onto the other, and from it a unitized motor. All three are half-turns (axes in the record's section 4). So "differs only by setting" is now shown, not inferred, for that realization.

## 3. What this means for the labels

Nothing here changes a label; only the owner edits CONVENTIONS. For the owner's next revision, the evidence now available is:

| Item | Label now | New evidence |
| --- | --- | --- |
| 2.1, grade involution | Proposed | derived (D1) and read in a primary source (Shirokov, section 2) |
| 2.1, antigrade involution | Proposed | derived (D4); no source |
| 3.4 | Checked for rigid(3) | derived for every degenerate form (D7) |
| 4.2 | Checked | derived for every form (D3), and checked symbolically for n = 3 and 4 |
| 4.4 | Proposed; odd sign Checked for n = 3, 4, 5 | derived as stated (D9) by a second route |
| meets (5.3) | Checked | signs derived (D10) |

## 4. Not verified

- Symbolic checks stop at n = 4 (and antiproduct associativity at 768 of the 4,096 blade triples for n = 4). Statements for larger n rest on the derivations D2 to D9 and on numerical forms with n ≤ 5.
- The "only if" halves of 1.8's "exactly when" were shown on twelve changes, not proved; research note 0026 proves the necessary direction for ⟇, the weight dual and the antidot.
- Lengyel's pages were not re-read; Confirmed labels are supported here by derivation only.
- The International Tables' standard settings, and whether the library's settings are standard, were not examined.
