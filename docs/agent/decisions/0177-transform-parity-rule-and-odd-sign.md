# 0177 The versor transform takes its form from the parity of the antigrades: V ⟇ x ⟇ V̰ for even, −V ⟇ x̌ ⟇ V̰ for odd; in rigid(3) the five object layouts never drop a nonzero part

- Status: accepted for the mathematics (the proofs below, checked by `records/spikes/transform-parity/check.py`). The CONVENTIONS 1.10 text of 2.1 and 4.4 that rests on it was approved by the owner on 2026-11-07 (plan 0021 O2, with three changes) and takes effect when committed; consequence 2 suggests a hypothesis for it.
- Date: 2026-11-07
- Recorded 2026-11-07 by Claude, agent, Opus 5.5, math-expert role, for WU13a.
- Bears on: CONVENTIONS 2.1, 3.1, 4.2, 4.4, 4.5, 5.4, 5.6, and the "Flector transform" row of section 8; plan 0021 departures 2 and 6, readings R2, R3, R8, R10, and O2; decisions 0009 (the parity rule) and 0172 (whose sections 0, 1, and appendix this record uses); VVUQ V1 and V9

## Context

CONVENTIONS 1.9 item 4.4 says that `transform(x, V)` applies V "by the parity of V's blades": an even V gives V ⟇ x ⟇ Ṽ, and an odd V gives −V ⟇ x̂ ⟇ Ṽ. Plan 0021 departure 2 found three problems with this, using sampling probes. Ṽ must be the antireverse V̰. The parity must be that of the antigrades. The odd sign must be −V ⟇ x̌ ⟇ V̰, with x̌ the antigrade involution, which is −V ⟇ x̂ ⟇ V̰ when n is even and +V ⟇ x̂ ⟇ V̰ when n is odd. Plan 0021 R8 also claims that in rigid(3) the transform never drops a nonzero coefficient for the point, line, plane, motor, and flector layouts. The probes were samples, which support hypotheses, not theorems (VVUQ V9). This record proves the four claims the brief names from the definitions, or says where they fail:

1. the parity rule;
2. the sign of the odd form, for every n and every symmetric form, under a hypothesis made explicit here;
3. the dropped-part claim for rigid(3) and its five layouts;
4. why the hand case in R(4,0,0) drops a nonzero part.

The proofs are written for a reader with no background in formal methods: every step names the fact it uses, and each fact is either proved here or in decision 0172, or is a hand computation the reader can redo.

## Options weighed

For an odd V, plan 0021 departure 6 tried twelve forms: sign ±; involution of x none, grade, or antigrade; and V̰ or Ṽ. Theorem 4 below shows that, up to the overall sign, each of them equals one of two maps, λ M_∧ or λ (−M)_∧. Here M is the vector map that V induces and λ is the coefficient of V ⟇ V̰ on 𝟙. The options are therefore these four.

1. **Parity by grade.** This is 4.4 read literally. For n even, grade and antigrade parity agree. For n odd they are opposite, and the form chosen by grade is λ(−M)_∧ for every V. The identity versor 𝟙 (antigrade 0, grade n) then acts as x ↦ −x on vectors. In rigid(2), Lengyel's motor r_x e1 + r_y e2 + r_z e3 + r_w e123 (odd grades, even antigrades) would be rejected as odd or given the odd formula. Rejected.
2. **The odd sign of 1.9 for every n: −V ⟇ x̂ ⟇ V̰.** For n odd this is −λ M_∧. It fails the outermorphism property already at grade 2, and a mirror sends each point of the mirror to −λ times itself: in rigid(2), a reflected unitized point gets weight −1. Rejected for odd n. For even n it is the chosen form.
3. **The single sign −V ⟇ x ⟇ V̰ for odd V,** the form of Lengyel's Flector page (note 0005, A1). For n even it is λ(−M)_∧. That is an outermorphism, and it composes consistently with the even rule. But a mirror then negates the points of its own mirror. CONVENTIONS section 8 already chose 5.6 over it for rigid(3). Rejected, as before.
4. **The rule (chosen).** V ⟇ x ⟇ V̰ when every blade of V has an even antigrade, and −V ⟇ x̌ ⟇ V̰ when every blade has an odd one. It is λ M_∧ for every n. It maps the identity to the identity, fixes the mirror of a reflection, and composes.

The reverse Ṽ in place of V̰ is not a separate option. For V of one parity, Ṽ = ±V̰ with a sign fixed by n and the parity, so each reverse form is one of the forms above. For example, in n = 3, Ṽ = −V̰ on every blade, so +V ⟇ x ⟇ Ṽ is −(the even rule).

## Decision

1. `transform(x, V)` is, for V whose blades all have antigrade parity p,

       T_V(x) = (−1)^p V ⟇ x^(p) ⟇ V̰,   with x^(0) = x and x^(1) = x̌,

   where x̌ multiplies each blade of antigrade m by (−1)^m. Since x̌ = (−1)^n x̂, the odd form is +V ⟇ x̂ ⟇ V̰ for odd n and −V ⟇ x̂ ⟇ V̰ for even n.
2. Theorem 4 holds under hypothesis (H) of section 3. (H) holds automatically when n ≤ 4, and when n = 5 and g has rank at least 2, for every V of one parity with V ⟇ V̰ = λ𝟙 and λ ≠ 0. It holds for every antiproduct of hyperplanes with λ ≠ 0, in any n. It can fail for n ≥ 6 even when V ⟇ V̰ = λ𝟙 (example 3.4).
3. In rigid(3), for any V of one antigrade parity, versor or not, the point, line, plane, motor, and flector layouts drop nothing nonzero (theorem 5.2). So `transform`'s exact check cannot fire there. It can fire only for a V that is not a versor, or for a layout that is not a union of whole antigrades.

## Consequences

1. The 1.10 text of plan 0021 O2 is supported: the antireverse, the antigrade parity, the odd sign −V ⟇ x̌ ⟇ V̰, and the composition order. The odd sign is now Checked for n = 3, 4, and 5 by `conventions_check.py` (rigid(2), R(3,0,0), rigid(3), R(4,0,0), rigid(4), R(5,0,0)), not only for n = 3 and 4, and proved here for every n under (H).
2. Two corrections to the O2 wording of 4.4, for the owner or the implementer:
   - "Either way the transform is the outermorphism of V's action on vectors" is exact only when λ = 1. In general T_V = λ M_∧. On vectors T_V = λM, and the outermorphism of λM is λ^k M_∧ at grade k, which is not T_V unless λ = 1. A precise wording is "λ times the outermorphism of a vector map, so that λ^(k−1) T(a1 ∧ … ∧ ak) = T(a1) ∧ … ∧ T(ak), where λ is the coefficient of V ⟇ V̰ on 𝟙".
   - The statement needs (H), or "for a versor V", meaning an antiproduct of hyperplanes, or any V of one parity with V ⟇ V̰ = λ𝟙, λ ≠ 0, that maps hyperplanes to hyperplanes. For n ≤ 4 this is no restriction beyond λ ≠ 0 (proposition 3.3).
3. 2.1's antigrade involution can carry the label Checked. `conventions_check.py` checks right(x̌) = (right x)^ and x̌ = (−1)^n x̂ for n = 3, 4, and 5, and the spike checks both on every blade of eight forms. Lemma 1.2 proves them.
4. R8's mechanism is confirmed with one step made explicit. The antireverse sign alone would allow antigrades m ≡ 1 − k (mod 4) as well. It is the parity of the antigrades that leaves only m ≡ k (mod 4).
5. The outermorphism property alone does not fix the odd form among all twelve candidates. It leaves two maps per parity, λ M_∧ and λ(−M)_∧, and composition does not separate them (the spike: "odd twisted True"). The mirror property does: a reflection fixes its mirror, equivalently a reflected point keeps a positive weight. Among the forms s V ⟇ x̂ ⟇ V̰ alone, the outermorphism property does fix s: s = +1 for n = 3 and 5, s = −1 for n = 4 (the spike and `conventions_check.py`).
6. Not proved here: whether (H) follows from V ⟇ V̰ = λ𝟙 for n = 5 when g has rank 0 or 1. No such form is planned.

## Proofs

### 0. Setting

The setting is section 0 of decision 0172. F is a field of characteristic 0 (the rationals). V = Fⁿ has basis e_1, …, e_n, and g is any symmetric n × n matrix: degenerate, indefinite, or not diagonal. Λ is the exterior algebra with the wedge ∧, and its basis blades e_A are wedge monomials. gr(e_A) = |A| is the grade, ag(e_A) = n − |A| is the antigrade, and 𝟙 = e_1 ∧ ⋯ ∧ e_n. An element is *homogeneous* of a grade or an antigrade if all its blades have it, and *of parity p* if all its blades have antigrade ≡ p (mod 2). For multivectors, xᵀy is the coordinate dot product, which uses no metric.

The geometric product ⟑ is that of the Clifford algebra Cl(g), realized on Λ by Fact F of decision 0172: v ⟑ Y = ι_v(Y) + v ∧ Y for a vector v. The appendix of 0172 shows that each blade e_A is the antisymmetrized product of its generators, and more generally that for any vectors v_1, …, v_k,

    v_1 ∧ ⋯ ∧ v_k = (1/k!) Σ_σ sgn(σ) v_σ(1) ⟑ ⋯ ⟑ v_σ(k)        (W)

(0172, appendix, step 3). R = right and L = left are the complements of CONVENTIONS 3.1, with L ∘ R = R ∘ L = identity (0172, C3). The antiproduct is a ⟇ b = L(R a ⟑ R b) and the antiwedge is a ∨ b = L(R a ∧ R b) (CONVENTIONS 4.2).

The four sign maps multiply a blade e_A by a sign:

| Map | Sign on a blade of grade k, antigrade m = n − k |
| --- | --- |
| reverse x̃ | (−1)^(k(k−1)/2) |
| antireverse x̰ | (−1)^(m(m−1)/2) |
| grade involution x̂ | (−1)^k |
| antigrade involution x̌ | (−1)^m |

For a linear map M of V, its *outermorphism* M_∧ is the linear map of Λ with M_∧(1) = 1 and M_∧(a_1 ∧ ⋯ ∧ a_k) = M a_1 ∧ ⋯ ∧ M a_k. By (G1) of decision 0172 applied to M, its matrix on grade k has entries M_∧[B, A] = det M[B, A], the k × k minors.

The candidate transform of a V of parity p (Decision 1) is T_V(x) = (−1)^p V ⟇ x^(p) ⟇ V̰. λ is defined by V ⟇ V̰ = λ𝟙 when V ⟇ V̰ is a multiple of 𝟙.

### 1. Tools

**1.1 The right complement turns the antiproduct into the geometric product.** For all a and b, R(a ⟇ b) = R(a) ⟑ R(b), R(a ∨ b) = R(a) ∧ R(b), and R(𝟙) = 1. *Why:* apply R to the definitions and use R ∘ L = identity. Then 𝟙 ∧ 1 = 𝟙 gives R(𝟙) = 1. So R is an isomorphism of algebras from (Λ, ⟇) onto (Λ, ⟑), and 𝟙 is the unit of ⟇. Every fact about ⟑ therefore has a twin for ⟇, read through R.

**1.2 The complement maps antigrade to grade, and the anti-maps to the plain maps.** R(e_A) = ±e_(Aᶜ), and |Aᶜ| = n − |A|, so R sends a blade of antigrade m to a blade of grade m. Each sign map in the table depends only on the grade or only on the antigrade, so

    R(x̰) = (R x)~,    R(x̌) = (R x)^,    and    x̌ = (−1)^n x̂.

*Why:* on e_A both sides of the first two multiply by the same sign. For the first, it is (−1)^(m(m−1)/2) with m = ag(e_A) = gr(R e_A). For the second, it is (−1)^m. For the third, (−1)^(n−k) = (−1)^n (−1)^k. These give 2.1's antigrade involution its label (consequence 3).

**1.3 The reverse is an anti-automorphism of ⟑, and the grade involution an automorphism.** (ab)~ = b̃ ã and (ab)^ = â b̂, and both maps fix scalars. Vectors are fixed by the reverse and negated by the grade involution.

*Why:* the defining relations e_i ⟑ e_j + e_j ⟑ e_i = 2 g_ij read the same with the order of every product reversed, and the same with every e_i replaced by −e_i. So there is an anti-automorphism α of Cl(g) with α(e_i) = e_i, and an automorphism β with β(e_i) = −e_i. Apply α to (W): it reverses the order of each product, which is the permutation of k factors with sign (−1)^(k(k−1)/2). So α(e_A) = (−1)^(k(k−1)/2) e_A, and α is the reverse. Applying β multiplies each product of k vectors by (−1)^k, so β is the grade involution.

By 1.1 and 1.2, the twins for ⟇ hold: (a ⟇ b)̰ = b̰ ⟇ a̰ and (a ⟇ b)ˇ = ǎ ⟇ b̌.

**1.4 Parities add.** If X has grade parity p and Y has grade parity q, then X ⟑ Y has grade parity p + q. *Why:* by (W), X is a sum of products of vectors, each with a number of factors ≡ p (mod 2). By Fact F, each vector factor changes the grade by ±1. Through 1.1 and 1.2: if V has antigrade parity p and x has antigrade parity q, then V ⟇ x has antigrade parity p + q.

**1.5 The pseudoscalar against a vector.** Let I = e_1 ∧ ⋯ ∧ e_n, which is 𝟙 regarded as an element of Cl(g). For every vector a, I ⟑ a = (−1)^(n−1) a ⟑ I. So if v has grade parity p, then v ⟑ I = (−1)^(p(n−1)) I ⟑ v.

*Why:* a ∧ I = 0, so by Fact F, a ⟑ I = ι_a(I), of grade n − 1. Apply the reverse (1.3): (I ⟑ a)~ = a ⟑ Ĩ = (−1)^(n(n−1)/2) ι_a(I). Reversing the grade-(n − 1) element ι_a(I) multiplies it by (−1)^((n−1)(n−2)/2). So I ⟑ a = (−1)^(n(n−1)/2 + (n−1)(n−2)/2) ι_a(I) = (−1)^((n−1)²) a ⟑ I, and (n − 1)² ≡ n − 1 (mod 2). For v, use (W) as in 1.4: each vector factor contributes (−1)^(n−1). This holds for every g, degenerate included.

**1.6 An automorphism that keeps the vectors is an outermorphism.** If ψ is an automorphism of the algebra (Λ, ⟑) with ψ(V) ⊆ V, then ψ = O_∧ with O = ψ restricted to V. *Why:* apply ψ to (W) for e_A. ψ is multiplicative and linear, so ψ(e_A) is the antisymmetrized product of the vectors ψ(e_(a_t)), which by (W) again is their wedge.

**1.7 The pairing (0172, C1).** For x and y of the same grade, x ∧ R(y) = (xᵀ y) 𝟙.

**1.8 The complement turns an outermorphism into its inverse transpose.** For an invertible linear map F of V,

    L ∘ F_∧ ∘ R = det(F) · (F^(−T))_∧,

where F^(−T) is the inverse of the transpose of F's matrix.

*Proof.* Three facts about outermorphisms come first, each from the minors formula: (FG)_∧ = F_∧ G_∧, so (F_∧)^(−1) = (F^(−1))_∧; F_∧(𝟙) = det(F) 𝟙, the n × n minor; and the transposed matrix of F_∧ has entries det F[A, B] = det Fᵀ[B, A], so (F_∧)ᵀ = (Fᵀ)_∧.

Let x and y have the same grade k, and let w = L(F_∧(R y)), so that F_∧(R y) = R(w), with w of grade k. Since F_∧ is multiplicative for ∧ and by 1.7,

    F_∧(x) ∧ F_∧(R y) = F_∧(x ∧ R y) = F_∧((xᵀ y) 𝟙) = det(F) (xᵀ y) 𝟙.

By 1.7 again, the left side is F_∧(x) ∧ R(w) = ((F_∧ x)ᵀ w) 𝟙 = (xᵀ (Fᵀ)_∧ w) 𝟙. So xᵀ (Fᵀ)_∧ w = det(F) xᵀ y for every x of grade k. That gives (Fᵀ)_∧ w = det(F) y, and so w = det(F) (F^(−T))_∧ y. ∎

### 2. The transform seen through the complement

Write v = R(V) and X = R(x). By 1.1 and 1.2,

    V ⟇ x ⟇ V̰ = L(v ⟑ X ⟑ ṽ),    V ⟇ x̌ ⟇ V̰ = L(v ⟑ X̂ ⟑ ṽ),    V ⟇ V̰ = L(v ⟑ ṽ),

and the antigrade parity of V is the grade parity of v. So V ⟇ V̰ = λ𝟙 means v ⟑ ṽ = λ (L(1) = 𝟙). The vectors of the ⟑ side, the grade-1 elements w, are the right complements of the hyperplanes h = L(w) of the x side (antigrade 1). The points and other vectors of the x side become elements of grade n − 1.

In words: the transform of x is the classical sandwich v X ṽ of Clifford algebra, applied to the complement of x. The classical theory is about vectors w, and here those vectors are the hyperplanes. The outermorphism the transform must be is an outermorphism for the wedge of the x side, which the complement turns into the antiwedge. Lemma 1.8 bridges the two.

### 3. The hypothesis and when it holds

**(H)** V has one antigrade parity p, V ⟇ V̰ = λ𝟙 with λ ≠ 0, and V ⟇ h ⟇ V̰ has antigrade 1 for every h of antigrade 1. Equivalently, on the ⟑ side: v has grade parity p, v ⟑ ṽ = λ ≠ 0, and v ⟑ w ⟑ ṽ is a vector for every vector w.

**3.1 Inverse.** If v ⟑ ṽ = λ ≠ 0, then ṽ ⟑ v = λ too, and v⁻¹ = ṽ/λ. *Why:* u = ṽ/λ satisfies v ⟑ u = 1. So Y ↦ v ⟑ Y is onto (Y = v ⟑ (u ⟑ Y)). Λ has finite dimension, so the map is also one to one. From v ⟑ (u ⟑ v) = (v ⟑ u) ⟑ v = v = v ⟑ 1 it follows that u ⟑ v = 1.

**3.2 Antiproducts of hyperplanes satisfy (H).** Let V = h_1 ⟇ ⋯ ⟇ h_r with each w_i = R(h_i) a vector of nonzero square g(w_i, w_i). Then v = w_1 ⟑ ⋯ ⟑ w_r has parity r mod 2 and v ⟑ ṽ = Π g(w_i, w_i) ≠ 0. For a vector u, the defining relation gives w ⟑ u ⟑ w = 2 g(w, u) w − g(w, w) u, a vector. Applying this r times shows that v ⟑ u ⟑ ṽ is a vector.

**3.3 For n ≤ 4, and for n = 5 with rank g ≥ 2, the last condition of (H) is automatic.** Suppose V has one antigrade parity and V ⟇ V̰ = λ𝟙 with λ ≠ 0.

- n ≤ 4. By lemma 5.1 below, V ⟇ h ⟇ V̰ for h of antigrade 1 has only antigrades m ≡ 1 (mod 4) with 0 ≤ m ≤ n. That leaves m = 1 alone. This needs no condition on λ.
- n = 5. For a vector w let y = v ⟑ w ⟑ ṽ. Then ỹ = y by 1.3, so y has only grades k with (−1)^(k(k−1)/2) = +1, that is k ≡ 0 or 1 (mod 4). By 1.4, y has grade parity p + 1 + p ≡ 1. So y = y_1 + c(w) I, with y_1 a vector and c(w) a number that depends linearly on w. Now y ⟑ y = λ v ⟑ (w ⟑ w) ⟑ ṽ = λ² g(w, w) by 3.1, a scalar. On the other side, y ⟑ y = y_1 ⟑ y_1 + 2c(w) y_1 ⟑ I + c(w)² I ⟑ I, using that I commutes with y_1 for odd n (1.5). Here y_1 ⟑ y_1 = g(y_1, y_1) is a scalar. So is I ⟑ I, since I is a multiple of the product of a g-orthogonal basis, which exists in characteristic 0 (0172, appendix, step 3). And y_1 ⟑ I = ι_(y_1)(I) has grade 4. Comparing grade-4 parts, c(w) ι_(y_1)(I) = 0. By the contraction formula, ι_u(I) = Σ_p (−1)^(p−1) g(u, e_p) e_(…p̂…) is zero exactly when g(u, e_p) = 0 for every p, that is, when u lies in the radical rad g. So for every w, either c(w) = 0 or y_1(w) ∈ rad g.

  Suppose c is not identically zero. Its zero set U is a hyperplane of V. Every vector is a difference of two vectors outside U (if u ∈ U and w_0 ∉ U, then u = (u + w_0) − w_0), so y_1(V) ⊆ rad g. The map w ↦ y is one to one, since w = ṽ ⟑ y ⟑ v / λ² by 3.1. Its image lies in rad g ⊕ F I, so 5 ≤ dim rad g + 1, which means rank g ≤ 1. So when rank g ≥ 2, c = 0 and y is a vector.

**3.4 For n = 6 the condition is not automatic.** In R(6,0,0) take V = 1 + 𝟙, of antigrades 6 and 0, both even. Then v = I + 1, and ṽ = 1 − I, because I has grade 6 and (−1)^15 = −1. With I ⟑ I = (−1)^15 = −1, v ⟑ ṽ = 1 − I ⟑ I = 2, so V ⟇ V̰ = 2𝟙. But I anticommutes with vectors (n even, 1.5), so for a vector w, v ⟑ w ⟑ ṽ = w + 2 I ⟑ w − I ⟑ w ⟑ I = 2 I ⟑ w, which has grade 5. On the x side, V ⟇ e1 ⟇ V̰ = −2 e23456: the "transform" moves a point entirely out of the vectors. The spike computes this ("R(6,0,0), V = 1 + I"). So for n ≥ 6, V ⟇ V̰ = λ𝟙 with λ ≠ 0 does not imply (H), and theorem 4 needs (H) as stated.

### 4. Theorem: the rule is λ times an outermorphism

**Statement.** Assume (H) for V, with parity p and factor λ. Let O be the linear map of V given by O(w) = (−1)^p v ⟑ w ⟑ v⁻¹. Then O is invertible, det O = (−1)^p, and with M = O^(−T),

    T_V = (−1)^p V ⟇ x^(p) ⟇ V̰ = λ M_∧    on all of Λ.

Consequently:

- (a) **Outermorphism.** λ^(k−1) T_V(a_1 ∧ ⋯ ∧ a_k) = T_V(a_1) ∧ ⋯ ∧ T_V(a_k) for all vectors a_1, …, a_k, and T_V(1) = λ.
- (b) **Mirror.** If V = h is one hyperplane (p = 1), then T_h(a) = λ a for every vector a in the mirror (a ∨ h = 0, equivalently a ∧ h = 0), and T_h(b) ≠ λ b for every b outside it.
- (c) **Composition.** T_W ∘ T_V = T_(W⟇V) for any W and V of one parity each. This needs no other part of (H).
- (d) **The other candidates.** −T_V and the maps x ↦ ±T_V(x̂) are −λ M_∧ and ±λ(−M)_∧. The first and the last with the minus sign fail (a) at k = 2, since (−λ)² ≠ −λ². The map λ(−M)_∧ passes (a) and (c) but fails (b): it sends each vector of the mirror to −λ times itself.

**Proof.**

*Step 1: an automorphism.* Define ψ(X) = v ⟑ X^(p) ⟑ v⁻¹, with X^(1) = X̂. It is a composition of the grade involution, an automorphism by 1.3 (used only when p = 1), and conjugation by the invertible v (3.1), so ψ is an automorphism of (Λ, ⟑). For a vector w, ψ(w) = (−1)^p v ⟑ w ⟑ v⁻¹ = (−1)^p λ⁻¹ v ⟑ w ⟑ ṽ = O(w), which is a vector by (H). By 1.6, ψ = O_∧. ψ is one to one, so O is one to one on V, hence invertible.

*Step 2: its determinant.* By the minors formula, O_∧(I) = det(O) I. By 1.5 with v of parity p, and I^(p) = (−1)^(pn) I,

    ψ(I) = (−1)^(pn) v ⟑ I ⟑ v⁻¹ = (−1)^(pn) (−1)^(p(n−1)) I = (−1)^p I.

So det O = (−1)^p. This holds for any v under (H), not only for a product of reflections. For a product of r hyperplanes, O is a product of r reflections, each of determinant −1, and r ≡ p (mod 2), in agreement.

*Step 3: back through the complement.* By section 2 and 1.2, (−1)^p V ⟇ x^(p) ⟇ V̰ = (−1)^p L(v ⟑ X^(p) ⟑ ṽ), with X = R(x). Since ṽ = λ v⁻¹ (3.1), v ⟑ X^(p) ⟑ ṽ = λ ψ(X) = λ O_∧(R x). So, with 1.8 and step 2,

    T_V = (−1)^p λ · L ∘ O_∧ ∘ R = (−1)^p λ · det(O) · (O^(−T))_∧ = λ M_∧.

The sign (−1)^p of the odd form is exactly what cancels det O. This is the sign question of the plan: without it, the transform of an odd versor would be −λ M_∧. ∎

*Proof of (a).* T_V(a_1 ∧ ⋯ ∧ a_k) = λ M a_1 ∧ ⋯ ∧ M a_k. Multiplying by λ^(k−1) gives (λ M a_1) ∧ ⋯ ∧ (λ M a_k) = T_V(a_1) ∧ ⋯ ∧ T_V(a_k). And M_∧(1) = 1.

*Proof of (b).* Let u = R(h), a vector. Then h ⟇ h̰ = L(u ⟑ u) = g(u, u) 𝟙, so λ = g(u, u) ≠ 0. From u ⟑ w + w ⟑ u = 2 g(u, w),

    O(w) = −u ⟑ w ⟑ u⁻¹ = w − (2/λ) g(u, w) u,

which is the reflection along u. With G the Gram matrix, g(u, w) = (G u)ᵀ w, so O = Id − (2/λ) u (G u)ᵀ, with Id the identity matrix. A reflection is its own inverse (O O = Id, as the formula shows), so M = O^(−T) = Oᵀ = Id − (2/λ) (G u) uᵀ. So M a = a exactly when uᵀ a = 0, because G u ≠ 0 (its pairing with u is λ ≠ 0). On the other side, a ∧ h = a ∧ L(u) = (−1)^(n−1) a ∧ R(u) = (−1)^(n−1) (aᵀ u) 𝟙, by CONVENTIONS 3.1 (L = (−1)^(gr·ag) R on a vector) and 1.7. Likewise a ∨ h = L(R(a) ∧ u) = ±(uᵀ a). So a lies in the mirror exactly when uᵀ a = 0, and then T_h(a) = λ a. For b outside the mirror, T_h(b) = λ b − 2 (uᵀ b) G u ≠ λ b.

*Proof of (c).* Write σ_V = (−1)^(p_V) and ι_V for the map x ↦ x^(p_V). Then

    T_W(T_V(x)) = σ_W σ_V W ⟇ (V ⟇ ι_V x ⟇ V̰)^(p_W) ⟇ W̰.

The antigrade involution is an automorphism of ⟇ (1.3, twin). V̌ = (−1)^(p_V) V because V has parity p_V, and the antireverse keeps antigrades, so the same holds for V̰. So (V ⟇ ι_V x ⟇ V̰)^(p_W) = V ⟇ ι_W ι_V x ⟇ V̰, because the two signs (−1)^(p_V p_W) cancel. Then V̰ ⟇ W̰ = (W ⟇ V)̰ (1.3, twin). By 1.4, W ⟇ V has parity p_V + p_W, so σ_W σ_V = σ_(W⟇V) and ι_W ι_V = ι_(W⟇V), since the involution squares to the identity. So T_W ∘ T_V = T_(W⟇V). ∎

*Proof of (d).* x̂ multiplies grade k by (−1)^k, and M_∧ keeps grades, so T_V(x̂) = λ (−M)_∧(x). For (a) at k = 2 with T = −λ M_∧: λ T(a ∧ b) = −λ² M a ∧ M b, while T(a) ∧ T(b) = +λ² M a ∧ M b. These differ, because M a ∧ M b ≠ 0 for independent a and b, M being invertible. For (b), λ(−M) a = −λ a on the mirror. Every candidate of the plan's twelve is one of ±λ M_∧ and ±λ(−M)_∧. This uses x̌ = (−1)^n x̂ and Ṽ = ±V̰ for V of one parity. The spike finds exactly the forms equal to λ M_∧ or λ(−M)_∧ passing (a) on its samples.

**Which parity.** The rule takes p from the antigrades. In odd n the grade parity of V is 1 − p, so choosing the form by grade gives (−1)^(1−p) V ⟇ x^(1−p) ⟇ V̰. Using x^(1−p) = (x^(p))ˇ and x̌ = −x̂ for odd n, this equals the map x ↦ T_V(x̂) = λ(−M)_∧. In particular the identity versor 𝟙 (grade n, odd) would act on vectors as −1, and every reflection would negate its mirror. In even n the two parities agree. This is the parity rule: the sandwich is λ times an automorphism of Cl(g), seen through the complement, whose induced vector map fixes the mirror of a reflection, exactly when the involution and the sign follow the antigrade parity of V, which is the grade parity of v = R(V).

**The sign for every n.** −V ⟇ x̌ ⟇ V̰ = (−1)^(n+1) V ⟇ x̂ ⟇ V̰. For a mirror in rigid(2) (n = 3), the form is +V ⟇ x̂ ⟇ V̰. The mirror e23 has u = R(e23) = e1 and λ = 1, and sends (1, 2, 1) to (−1, 2, 1). In rigid(3) (n = 4) it is −V ⟇ x̂ ⟇ V̰, CONVENTIONS 5.6, and the mirror e412 sends (1, 2, 3, 1) to (1, 2, −3, 1). Both values are checked by the spike and by `conventions_check.py`.

### 5. The dropped parts

**5.1 Lemma (antigrades modulo 4).** Let V have one antigrade parity, with no other condition: V need not be a versor. Let x be homogeneous of antigrade k. Then V ⟇ x ⟇ V̰, and so also ±V ⟇ x̌ ⟇ V̰, has only antigrades m ≡ k (mod 4).

*Proof.* Let y = V ⟇ x ⟇ V̰ and let p be V's parity.

- Parity. By 1.4, y has antigrade parity p + k + p ≡ k, so every antigrade m in y has m ≡ k (mod 2).
- Antireverse. By 1.3 (twin), y̰ = (V̰)̰ ⟇ x̰ ⟇ V̰ = V ⟇ x̰ ⟇ V̰ = ρ_k y, where ρ_m = (−1)^(m(m−1)/2) and x̰ = ρ_k x. The antireverse multiplies the antigrade-m part of y by ρ_m, so every antigrade m present in y has ρ_m = ρ_k.
- Together. ρ_(m+2) = −ρ_m, because (m+2)(m+1)/2 − m(m−1)/2 = 2m + 1 is odd. So if m ≡ k + 2 (mod 4), then ρ_m = −ρ_k, which is excluded. With m ≡ k (mod 2), only m ≡ k (mod 4) remains.

The odd form multiplies x by the sign (−1)^k first, which changes nothing. ∎

The antireverse step alone would also allow m ≡ 1 − k (mod 4), for example m = 1 when k = 0, since ρ_0 = ρ_1 = +1. The parity step excludes those.

**5.2 Theorem (rigid(3)).** In rigid(3) (n = 4, antigrades 0 to 4), let V have one antigrade parity, versor or not, and let x lie in one of the layouts

- point: e1, e2, e3, e4, all of antigrade 3;
- line: e41, e42, e43, e23, e31, e12, all of antigrade 2;
- plane: e423, e431, e412, e321, all of antigrade 1;
- motor: the eight blades of antigrades 0, 2, and 4;
- flector: the eight blades of antigrades 1 and 3.

Then T_V(x) lies in the same layout. Its coefficients on every other blade are zero, as polynomials in the coefficients of V and x.

*Proof.* Each layout is the set of all blades of some antigrades: {3}, {2}, {1}, {0, 2, 4}, {1, 3}. Write x as a sum of its homogeneous parts and apply lemma 5.1 to each. A part of antigrade 1, 2, or 3 goes to antigrades ≡ k (mod 4) within 0 to 4, which is k alone. A part of antigrade 0 or 4 goes to {0, 4}. Each layout's set of antigrades is closed under this: the motor holds both 0 and 4, and no other layout holds either. ∎

The product plans do reach other blades. For a point, the plan of V ⟇ x and then ⟇ V̰ reaches all eight odd blades and drops four. For a line it reaches the eight even blades and drops two; for a plane it drops four; for a motor or a flector, none. The spike lists these counts. Over a field, the coefficients on the dropped blades cancel identically. The spike also checks this as a polynomial identity, for generic V of the motor and of the flector layout and generic x of each layout: "nonzero dropped 0" in all ten cases.

Two limits of the theorem:

- It holds for layouts that are unions of whole antigrades. A layout that holds only part of an antigrade can drop nonzero parts even for a versor. A point stored on e1 and e4 alone, turned about the z axis, gets a nonzero e2 coefficient.
- For a versor (H), any n, and any layout that is a union of whole grades, nothing nonzero is dropped, because T_V = λ M_∧ keeps grades (theorem 4).

**5.3 The R(4,0,0) hand case.** In R(4,0,0), V = 1 + 𝟙 has antigrades 4 and 0, both even. Take x = 1, of antigrade 4. Lemma 5.1 allows antigrades 0 and 4 in the result. The scalar layout of x has only antigrade 4, so a part on 𝟙 (antigrade 0) would be dropped. The computation, on the ⟑ side: v = R(1) + R(𝟙) = I + 1, and X = R(1) = I. In R(4,0,0), I ⟑ I = (−1)^(4·3/2) = +1, and Ĩ = I. So

    v ⟑ X ⟑ ṽ = (1 + I) ⟑ I ⟑ (1 + I) = (I + 1) ⟑ (1 + I) = 2 + 2I,

and V ⟇ 1 ⟇ V̰ = L(2 + 2I) = 2𝟙 + 2. The scalar 2 is kept and 2𝟙 is dropped, which is nonzero.

Why: V is not a versor. V ⟇ V̰ = L((1 + I) ⟑ (1 + I)) = L(2 + 2I) = 2 + 2𝟙, which is not a multiple of 𝟙, so theorem 4 does not apply. For any V that satisfies (H), T_V(1) = λ M_∧(1) = λ is a scalar, and nothing is dropped from a scalar. The lemma's freedom m ≡ k (mod 4), with 0 ≡ 4, is what a non-versor can use when n = 4 and x holds antigrade 0 or 4 without the other. For n ≥ 5 it can also use antigrades 1 and 5. The spike recomputes both values ("R(4,0,0), V = 1 + I").

### 6. The mechanical check

`records/spikes/transform-parity/check.py`, run as `python3 -I -B docs/agent/records/spikes/transform-parity/check.py .` (seed 20261107, 1720 checks, about 4 s), checks each step:

- 1.2 and 1.3: on every blade of eight forms (rigid(2), R(3,0,0), rigid(3), R(2,2,0), conformal(2), rigid(4), conformal(3), R(4,1,0)), right(x̌) = (right x)^, right(x̰) = (right x)~, and x̌ = (−1)^n x̂. The script's diagonal product table equals `clifford_reference.py`'s on every blade pair.
- Theorem 4 (a) to (d), on 30 antiproducts of one to three random hyperplanes per form, with the twelve candidate forms: the rule passes (a) and (b); only forms equal to λ M_∧ or λ(−M)_∧ pass (a); only λ M_∧ passes (b); the rule composes, and so does the family that follows odd versors by x̂, while the family that follows even versors by x̂ does not. These are samples (V9). They test the theorem's conclusions on versors that satisfy (H) by 3.2. They do not replace the proof.
- Lemma 5.1: symbolically in rigid(3) for generic V of each parity and x of each antigrade, which proves it there; sampled on all eight forms.
- Theorem 5.2: symbolically, all ten pairs of a V layout and an x layout.
- 3.4 and 5.3: the hand values in R(6,0,0) and R(4,0,0).

`test/reference/conventions_check.py` checks the conclusions for single mirrors in n = 3, 4, and 5, and finds the sign s of s V ⟇ x̂ ⟇ V̰ from the outermorphism property alone: +1, −1, +1.

### 7. What these proofs do not show

- They assume characteristic 0, which the appendix of 0172 and 3.3 (the orthogonal basis for I ⟑ I) use.
- They concern the mathematics, not the library. `transform`'s tests and the golden compositions check the code.
- (H) for n = 5 with rank g ≤ 1, and for n ≥ 6 beyond antiproducts of hyperplanes, is assumed, not derived.
- Validation (V8): the rigid(3) values agree with Lengyel's published motor and reflection rows (note 0005, A3, A4, F2). For odd n and for the non-diagonal forms, no published table was read.
