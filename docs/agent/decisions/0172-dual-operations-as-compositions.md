# 0172 The bulk, the weight, the duals, the dot, the antidot, and the projection are compositions of the public products, maps, and complements; the dot keeps only the plan's terms on the scalar

- Status: accepted (plan 0020 readings R4, R5, R10; recorded by the WU11a implementer)
- Date: 2026-11-04
- Recorded by: Claude (agent, Opus 5.5), implementer of WU11a, cpp-expert role. The proofs below were written by a math-expert agent (Claude, Opus 5.5) from the definitions, and checked by its script, committed as `records/spikes/dual-identities/check.py`.
- Bears on: constitution C3, C4, and C12; CONVENTIONS 3.2, 3.3, 3.4, 4.1, and 4.3; decisions 0141 (the antiproduct's growth with n) and 0165; PERFORMANCE P4; the post-WU10 audit B1

## Context

CONVENTIONS defines the bulk as G x, the metric exomorphism, which sends e_A to the sum of det g[A, B] e_B over the blades of its grade; the weight as the antiexomorphism left(G right(x)); the duals as the complements of those; the dot as aᵀ G b on the scalar and the antidot as aᵀ 𝔾 b on the antiscalar; and the projection as b ∨ (a ∧ b☆). C4 admits new generative machinery only where composition of the one mechanism with per-basis data cannot express a capability.

## Options weighed

1. A generator of the metric exomorphism from Gram minors, one map per layout. It is the definition read literally and would cost one generation per layout in place of three (reverse, product, complement). It is new machinery, which C4 admits only when composition cannot express the capability, and composition can. C3 allows it later as a measured replacement.
2. The antiproduct routes: weight_dual(x) = x̰ ⟇ 1 and antidot(a, b) = the antiscalar part of a ⟇ b̰. Both are exact, but they inherit the antiproduct's limits and its growth with n (decision 0141): the antidot of two `full<5>` values of conformal(3) through ⟇ fails at `-O2` on Clang 23 (the configuration of audit B1), while the route through the complements builds.
3. Compositions of the public functions (chosen):

| Operation | Definition | Composition |
| --- | --- | --- |
| `bulk(x)` | G x | `left_complement(geometric_product(reverse(x), 𝟙))` |
| `weight(x)` | left(G right(x)) | `left_complement(bulk(right_complement(x)))` |
| `bulk_dual(x)` | right(G x) | `geometric_product(reverse(x), 𝟙)` |
| `weight_dual(x)` | right(weight(x)) = G right(x) | `bulk(right_complement(x))` |
| `left_bulk_dual(x)` | left(G x) | `left_complement(bulk(x))` |
| `left_weight_dual(x)` | left(weight(x)) | `left_complement(weight(x))` |
| `dot(a, b)` | aᵀ G b | the terms of the plan of a ⟑ b on the scalar, applied to a and `reverse(b)` |
| `antidot(a, b)` | aᵀ 𝔾 b 𝟙 | `left_complement(dot(right_complement(a), right_complement(b)))` |
| `project(a, b)` | b ∨ (a ∧ b☆) | `antiwedge(b, wedge(a, weight_dual(b)))` |

𝟙 is the unit antiscalar, a one-blade multivector with coefficient 1, the only new value. The plan of reading R4 wrote bulk as left(bulk_dual(x)); the implementation writes the same expression in place, so that the bulk and the weight do not depend on the duals, which came in a later commit.

The dot. Written as `part<scalar>(geometric_product(a, reverse(b)))`, it generates and runs the whole product and then drops all but one coefficient. GCC 16 removes the dead terms; Clang 23 at `-O2` keeps the product kernel out of line and computes all of it (the planner's probe: 48 multiplications for the dot of two rigid(3) motors, whose value needs 4). A careful person would not write that (P4). So `detail::scalar_part_kernel<Alg, A, B>` takes the terms of the existing product plan whose output is the scalar: a mask on the plan's outputs, per-basis data of the same kind as `identity_kernel` and `dropped_positions`, and no new generator. It is pinned by static_asserts in `dual_test.cpp` (three terms of 1 for the vectors of R(3,0,0), one of −1 for e4 with e5 in conformal(3), none for e12 with e13 or e1 with e2). Its first term is assigned, not added to zero, so a lone −0.0 survives. The reverse of b is applied by the public `reverse`, as in the definition.

## Decision

1. The compositions of the table above. No new plan or map generator.
2. The dot's kernel of option 3, in `detail` of `dual.hpp`, its only user, so `mv.hpp` is not touched. Its narrow-scalar check reads only the kept terms.
3. Measured on the committed header (records in `records/performance/duals-cost/`):
   - Run code (P4), `-O2 -ffp-contract=off`, GCC 16.2.1 and Clang 23.1.4: the dot and the antidot of two rigid(3) motors compile to 4 multiplications and 3 additions on both; the bulk of a line and the weight dual of a point to no arithmetic but moves and sign flips; the projection of a point onto a plane to 21 multiplications, the hand count; no call on either compiler.
   - Limits, the largest `full<n>` of R(n,0,0) that builds at the default limits, the same at `-O0` (with libstdc++'s assertions, which it defines by itself there) and at `-O2` (without them): the six unary operations up to `full<9>` on GCC (the largest tried) and `full<7>` on Clang; the dot, the antidot, and the projection up to `full<6>` on GCC and `full<5>` on Clang. Every operation builds for `full<5>` of conformal(3) on both compilers in both configurations. In the dense form of five generators with 2 on the diagonal and 1 elsewhere, the unary operations and the projection build on both, and the dot and the antidot build on GCC and not on Clang. Each failure is the limit of a part: the reverse map's at `full<8>` on Clang, the geometric product's plan at `full<6>` on Clang and `full<7>` on GCC, the wedge's plan at `full<7>` on GCC for the projection. At `-O0` GCC reports that last one as a libstdc++ assertion that "is not a constant expression" inside the wedge plan's typing, not as its operation count; with `-fconstexpr-ops-limit=400000000` the same unit builds, so it is the count.
4. Folding the reverse sign into the operand's signs (per-basis data) would remove the reverse map from the bulk and the dot, and with it the `full<8>` limit on Clang, but it reads less plainly; it waits for a measured need (C12).

## Consequences

- Each operation's compile cost is the sum of its parts': three generations for the bulk (reverse, product with 𝟙, complement), more for the weight, whose complements grow with n as the antiproduct's do. The growth at fixed use is measured beside decision 0141's in `records/performance/duals-cost/`.
- `bulk(x) + weight(x) = x` holds only in rigid(d)-like metrics; the header and the tests state the other cases.
- The proof of claim (a) closes a gap in the plan's sketch: "every generator lies in 𝟙" gives x 𝟙 = x ⌋ 𝟙 only in a diagonal metric. In a non-diagonal one, two terms of x ∧ (…) are each nonzero and cancel (lemma B below; in the Witt form, e1 ∧ (e2 ⌋ 𝟙) = e12 = (ι₁ e2) ⌋ 𝟙). No formula of the sketch was wrong.

## Proofs

These proofs show five identities that let the bulk, the weight, their duals, the dot, and the antidot be built from the geometric product, the reverse, and the complements. They hold for every symmetric bilinear form over a field of characteristic 0, including degenerate, indefinite, and non-diagonal forms. Each proof uses only the definitions in section 0 and the facts proved in section 1 and the appendix.

### 0. Setting and conventions

**Field and form.** F is a field of characteristic 0 (for example the rationals). V = Fⁿ has basis e_1, …, e_n, and g is a symmetric n × n matrix over F, with g(e_i, e_j) = g_ij. Nothing is assumed about the rank or signature of g.

**Index sets.** A, B, C, D denote subsets of {1, …, n}, always listed in increasing order: A = {a_1 < a_2 < ⋯ < a_k}. The complement is Aᶜ = {1, …, n} ∖ A. For a blade of index set A, the *grade* is gr = |A| = k and the *antigrade* is ag = n − k. In the code a blade is a bit mask in which bit i − 1 stands for e_i; nothing below depends on that encoding.

**Exterior algebra.** Λ is the exterior algebra of V, with the wedge ∧. Its basis is the *wedge monomials* e_A = e_{a_1} ∧ ⋯ ∧ e_{a_k}, with e_∅ = 1. Λᵏ is the span of the e_A with |A| = k; an element of Λᵏ is *homogeneous of grade k*. For disjoint A and B, e_A ∧ e_B = (−1)^{inv(A,B)} e_{A∪B}, where inv(A, B) counts the pairs a ∈ A, b ∈ B with a > b: each such pair costs one transposition when the list is sorted. If A and B meet, e_A ∧ e_B = 0. The wedge does not depend on g. The antiscalar is 𝟙 = e_1 ∧ ⋯ ∧ e_n, the blade of all n generators.

**Coordinates.** A multivector x is written x = Σ_A x_A e_A. For two multivectors, aᵀb means Σ_A a_A b_A, the coordinate dot product, which uses no metric. For a linear map M on Λ, aᵀ M b means aᵀ (M b). The matrix of M has entry M_{AB} = the coefficient of e_A in M e_B.

**Complements.** The right complement right(e_A) and the left complement left(e_A) of a basis blade are fixed by

    e_A ∧ right(e_A) = 𝟙,    left(e_A) ∧ e_A = 𝟙,

and both extend linearly. Since e_A ∧ e_{Aᶜ} = ε_A 𝟙 with ε_A = (−1)^{inv(A, Aᶜ)} = ±1, and 1/ε_A = ε_A,

    right(e_A) = ε_A e_{Aᶜ},    left(e_A) = ε′_A e_{Aᶜ},  where e_{Aᶜ} ∧ e_A = ε′_A 𝟙.

**Reverse.** The reverse x̃ multiplies a blade of grade k by (−1)^{k(k−1)/2} and extends linearly.

**Metric exomorphism.** det g[A, B] is the determinant of the submatrix of g on rows A and columns B (in increasing order); the empty minor is 1. The metric exomorphism is the linear map

    G e_A = Σ_{|B| = |A|} det g[A, B] e_B.

So G e_i = Σ_j g_ij e_j on vectors, G 1 = 1, and G keeps each grade. Its matrix entry is G_{BA} = det g[A, B]. The antiexomorphism is 𝔾 = left ∘ G ∘ right.

**Geometric product.** ⟑ is the associative product with unit 1 fixed by e_i ⟑ e_j + e_j ⟑ e_i = 2 g_ij, in which the blade e_A is the wedge of its generators, that is, their antisymmetrized product (1/k!) Σ_σ sgn(σ) e_{a_σ(1)} ⟑ ⋯ ⟑ e_{a_σ(k)}. The appendix shows that this algebra is Λ with the product described by Fact F of section 1.2. ⟨x⟩₀ denotes the scalar part (the coefficient of 1).

**The four operations.** The dot is a • b = aᵀ G b, a number. The antidot is a ∘ b = (aᵀ 𝔾 b) 𝟙. The bulk dual is u★ = right(G u), and the bulk is G u.

### 1. Tools

#### 1.1 Contraction by a vector

For a vector v, the *contraction* ι_v is the linear map on Λ given on a wedge of vectors by

    ι_v(w_1 ∧ ⋯ ∧ w_m) = Σ_{p=1}^{m} (−1)^{p−1} g(v, w_p) w_1 ∧ ⋯ ∧ ŵ_p ∧ ⋯ ∧ w_m,

where the hat means that w_p is left out, and ι_v(1) = 0. The right side is linear in each w_p, and it changes sign when two neighbors w_p, w_{p+1} are exchanged (the two terms that drop one of them trade places with opposite signs, and every other term is a wedge with two factors exchanged). So it depends only on w_1 ∧ ⋯ ∧ w_m, and ι_v is well defined. On blades it reads

    ι_{e_i}(e_B) = Σ_{p=1}^{m} (−1)^{p−1} g_{i b_p} e_{B ∖ b_p}.

Three properties follow directly from the formula.

**(I1) Derivation rule.** For X homogeneous of grade r and any Y, ι_v(X ∧ Y) = ι_v(X) ∧ Y + (−1)^r X ∧ ι_v(Y). *Why:* split the sum over p into the factors of X and those of Y; a factor of Y sits behind the r factors of X, which gives the extra (−1)^r. In particular, for a vector w,

    ι_v(w ∧ Y) = g(v, w) Y − w ∧ ι_v(Y).    (I1′)

**(I2) Anticommutation.** ι_v ι_w = −ι_w ι_v, and so ι_v ι_v = 0. *Why:* applying ι_v ι_w to w_1 ∧ ⋯ ∧ w_m removes two factors w_p, w_q. The term that removes w_p first and w_q second has the opposite sign to the term that removes them in the other order, because removing the earlier factor shifts the position of the later one by one.

**(I3) Contraction is the adjoint of the wedge.** For X of grade k + 1 and Y of grade k,

    (ι_v X)ᵀ Y = Xᵀ ((G v) ∧ Y).

*Proof.* Both sides are linear in v, so take v = e_i. Then G v = Σ_j g_ij e_j, and from the blade formula, ι_{e_i} = Σ_j g_ij κ_j, where κ_j is the contraction for the identity matrix: κ_j(e_B) = (−1)^{p−1} e_{B ∖ j} if j = b_p is in B, and 0 otherwise. So it is enough to show (κ_j e_B)ᵀ e_C = e_Bᵀ (e_j ∧ e_C) for |B| = k + 1, |C| = k. Both sides are 0 unless B = C ∪ {j} with j ∉ C. In that case j = b_p with p − 1 = the number of elements of C below j. The left side is (−1)^{p−1}. On the right, e_j ∧ e_C = (−1)^{p−1} e_B, since e_j has to move past the p − 1 smaller indices of C. The two agree. ∎

#### 1.2 The geometric product of a vector with anything

**Fact F.** For every vector v and every multivector Y,

    v ⟑ Y = ι_v(Y) + v ∧ Y.

This is Chevalley's description of the Clifford algebra inside the exterior algebra. The appendix proves it from the definition in section 0. Two consequences are used repeatedly.

- **(F1)** Let a be the smallest index of A and A′ = A ∖ {a}. Then e_A = e_a ∧ e_{A′} with sign +1, because e_a is already in first place, and F gives

      e_A = e_a ⟑ e_{A′} − ι_{e_a}(e_{A′}).

  The correction term ι_{e_a}(e_{A′}) has grade |A| − 2. It is zero when g is diagonal, because then g_{a b} = 0 for every b in A′.
- **(F2)** For a vector v, v ∧ 𝟙 = 0 (there is no grade n + 1), so v ⟑ 𝟙 = ι_v(𝟙).

#### 1.3 Complements

**(C1) Pairing.** For X and Y of the same grade k,

    X ∧ right(Y) = (Xᵀ Y) 𝟙    and    left(Y) ∧ X = (Xᵀ Y) 𝟙.

*Proof.* Take basis blades X = e_A and Y = e_C with |A| = |C| = k. Then e_A ∧ right(e_C) = ε_C e_A ∧ e_{Cᶜ}. This is zero unless A is disjoint from Cᶜ, that is, A ⊆ C, and with equal sizes that means A = C. For A = C it is 𝟙 by the definition of the right complement. So e_A ∧ right(e_C) is 𝟙 when A = C and 0 otherwise, which is (e_Aᵀ e_C) 𝟙. Extend bilinearly. The left complement is the same argument with the factors in the other order. ∎

**(C2) Nondegeneracy.** If Z, Z′ have grade n − k and X ∧ Z = X ∧ Z′ for every X of grade k, then Z = Z′. *Why:* take X = e_A. Then e_A ∧ (Z − Z′) is ±(coefficient of e_{Aᶜ} in Z − Z′) times 𝟙, and as A runs over the k-sets, Aᶜ runs over all (n − k)-sets.

**(C3) Inverses.** right ∘ left = left ∘ right = identity. *Proof.* left(e_A) = ε′_A e_{Aᶜ} and right(e_{Aᶜ}) = ε_{Aᶜ} e_A. By the definitions, e_{Aᶜ} ∧ e_A = ε′_A 𝟙 and also e_{Aᶜ} ∧ e_A = ε_{Aᶜ} 𝟙, so ε_{Aᶜ} = ε′_A and right(left(e_A)) = (ε′_A)² e_A = e_A. In the same way e_A ∧ e_{Aᶜ} is both ε_A 𝟙 and ε′_{Aᶜ} 𝟙, so left(right(e_A)) = e_A. ∎

**(C4) The two complements differ by (−1)^{gr·ag}.** For Y homogeneous of grade k, with gr = k and ag = n − k,

    left(Y) = (−1)^{k(n−k)} right(Y).

*Why:* e_{Aᶜ} ∧ e_A = (−1)^{k(n−k)} e_A ∧ e_{Aᶜ}, since each of the k factors of e_A moves past the n − k factors of e_{Aᶜ}. The exponent k(n − k) is symmetric in k and n − k, so it is the same for Y and for its complement.

**(C5) Signed permutation.** The matrix R of right has one nonzero entry ±1 in each row and each column, so it is orthogonal: Rᵀ R = I. By (C3), the matrix of left is R⁻¹ = Rᵀ.

#### 1.4 The exomorphism on a wedge of vectors

**(G1)** For vectors v_1, …, v_k, G(v_1 ∧ ⋯ ∧ v_k) = (G v_1) ∧ ⋯ ∧ (G v_k).

*Proof.* Both sides are linear in each v_t and change sign when two of them are exchanged, so it is enough to check basis vectors e_{a_1}, …, e_{a_k} with a_1 < ⋯ < a_k (a repeated vector makes both sides 0). Expanding the right side,

    ∧_t (Σ_j g_{a_t j} e_j) = Σ_{j_1, …, j_k} g_{a_1 j_1} ⋯ g_{a_k j_k} e_{j_1} ∧ ⋯ ∧ e_{j_k}.

Terms with a repeated j vanish. Group the rest by the set B = {j_1, …, j_k}: if j_t = b_{σ(t)} for a permutation σ, the wedge is sgn(σ) e_B. So the coefficient of e_B is Σ_σ sgn(σ) Π_t g_{a_t b_σ(t)} = det g[A, B], the Leibniz formula for the minor. That is G e_A. ∎

**(G2) Symmetry.** G_{BA} = det g[A, B] = det(g[A, B])ᵀ = det g[B, A] = G_{AB}, since g[A, B]ᵀ = g[B, A] for symmetric g. So the matrix of G is symmetric.

**(G3) Reverse of a wedge.** e_{a_k} ∧ ⋯ ∧ e_{a_1} = (−1)^{k(k−1)/2} e_A = ẽ_A, because reversing a list of k distinct factors takes k(k − 1)/2 transpositions.

### 2. Claim (a): x̃ ⟑ 𝟙 = right(G x)

**Statement.** For every multivector x and every symmetric g, x̃ ⟑ 𝟙 = right(G x). Since left ∘ right is the identity (C3), the bulk is G x = left(x̃ ⟑ 𝟙).

**Sign conventions.** The only sign in (a) is the reverse, (−1)^{k(k−1)/2} with k = gr(x). No gr·ag sign appears because the complement is the right one and 𝟙 is the right factor. Without the reverse, x ⟑ 𝟙 = (−1)^{k(k−1)/2} right(G x).

**Example.** In R(3, 0, 0), x = e12: x̃ = −e12, and −e12 ⟑ e123 = −e1e2e1e2e3 = e3. On the other side, G e12 = e12 and right(e12) = e3, since e12 ∧ e3 = e123. In the Witt form g = [[0, 1], [1, 0]], x = e12: x̃ = −e12, and by the computation in the remark below e12 ⟑ 𝟙 = 1, so x̃ ⟑ 𝟙 = −1. On the other side, G e12 = det g e12 = −e12 and right(e12) = 1, so right(G x) = −1.

We define a left contraction by multivectors and prove the statement in three lemmas.

**Definition (left contraction).** For vectors u_1, …, u_k and any Y,

    (u_1 ∧ ⋯ ∧ u_k) ⌋ Y = ι_{u_1}(ι_{u_2}(⋯ ι_{u_k}(Y))),

with ι_{u_k} applied first, and 1 ⌋ Y = Y. By (I2) the right side is alternating in the u's, so this depends only on the wedge and extends linearly to every left argument. Directly from the definition, (u ∧ X) ⌋ Y = ι_u(X ⌋ Y) for a vector u and any X.

**Lemma A (contraction of a complement).** For a vector v and Y homogeneous of grade k,

    ι_v(right(Y)) = right(Y ∧ G v).

*Proof.* Both sides have grade n − k − 1. By (C2) it is enough to show that they give the same wedge with every X of grade k + 1.

- Right side: by (C1), X ∧ right(Y ∧ G v) = (Xᵀ (Y ∧ G v)) 𝟙. Moving the vector G v past the k factors of Y gives Y ∧ G v = (−1)^k (G v) ∧ Y, and by (I3) Xᵀ((G v) ∧ Y) = (ι_v X)ᵀ Y. So the wedge is (−1)^k ((ι_v X)ᵀ Y) 𝟙.
- Left side: X ∧ right(Y) has grade n + 1, so it is 0, and so is its contraction. By the derivation rule (I1), 0 = ι_v(X) ∧ right(Y) + (−1)^{k+1} X ∧ ι_v(right(Y)). So X ∧ ι_v(right(Y)) = (−1)^k ι_v(X) ∧ right(Y), which by (C1) is (−1)^k ((ι_v X)ᵀ Y) 𝟙.

The two agree. ∎

With Y = 1 and right(1) = 𝟙 this gives ι_{e_i}(𝟙) = right(G e_i) = Σ_p g_ip right(e_p), with right(e_p) = (−1)^{p−1} e_{{1..n} ∖ p}.

**Lemma B (wedge through a contraction).** For a vector u, B homogeneous of grade m, and any Y,

    u ∧ (B ⌋ Y) = (ι_u B) ⌋ Y + (−1)^m B ⌋ (u ∧ Y).

In particular u ∧ (B ⌋ 𝟙) = (ι_u B) ⌋ 𝟙, since u ∧ 𝟙 = 0.

*Proof by induction on m.* By linearity, take B a wedge of vectors.

- *Base, m = 0.* B = c is a scalar, ι_u c = 0, and both sides are c (u ∧ Y).
- *Hypothesis.* The statement holds for every B′ of grade m − 1 and every Y.
- *Step.* Write B = w ∧ B′ with w a vector and B′ of grade m − 1, so B ⌋ Y = ι_w(B′ ⌋ Y). By (I1′) with the roles of the vectors exchanged, u ∧ ι_w(Z) = g(u, w) Z − ι_w(u ∧ Z). With Z = B′ ⌋ Y and the hypothesis for u ∧ (B′ ⌋ Y):

      u ∧ (B ⌋ Y) = g(u, w) (B′ ⌋ Y) − ι_w((ι_u B′) ⌋ Y) − (−1)^{m−1} ι_w(B′ ⌋ (u ∧ Y))
                  = g(u, w) (B′ ⌋ Y) − (w ∧ ι_u B′) ⌋ Y + (−1)^m B ⌋ (u ∧ Y).

  By (I1′), ι_u B = g(u, w) B′ − w ∧ ι_u B′, so the first two terms are (ι_u B) ⌋ Y. ∎

**Lemma C (the product with 𝟙 is the contraction onto 𝟙).** For every multivector X, X ⟑ 𝟙 = X ⌋ 𝟙.

*Proof by strong induction on the grade, for blades X = e_A* (then extend linearly).

- *Base, grade 0.* 1 ⟑ 𝟙 = 𝟙 = 1 ⌋ 𝟙.
- *Hypothesis.* The statement holds for every blade of grade less than k.
- *Step.* Let |A| = k ≥ 1, a its smallest index, A′ = A ∖ {a}. By (F1) and associativity,

      e_A ⟑ 𝟙 = e_a ⟑ (e_{A′} ⟑ 𝟙) − (ι_{e_a} e_{A′}) ⟑ 𝟙.

  The hypothesis applies to e_{A′} (grade k − 1) and to ι_{e_a} e_{A′} (grade k − 2), and Fact F expands the product with e_a:

      e_A ⟑ 𝟙 = ι_{e_a}(e_{A′} ⌋ 𝟙) + [ e_a ∧ (e_{A′} ⌋ 𝟙) − (ι_{e_a} e_{A′}) ⌋ 𝟙 ].

  By Lemma B the bracket is 0, and the first term is (e_a ∧ e_{A′}) ⌋ 𝟙 = e_A ⌋ 𝟙. ∎

**Remark (where the cancellation is needed).** In a diagonal metric both terms of the bracket are 0 separately: ι_{e_a} e_{A′} = 0, and e_{A′} ⌋ 𝟙 is a multiple of the blade on the complement of A′, which contains e_a. In a non-diagonal metric neither is 0. In the Witt form g = [[0, 1], [1, 0]] with A = {1, 2}, e_2 ⌋ 𝟙 = ι_{e_2}(e1 ∧ e2) = g_21 e2 − g_22 e1 = e2, so e1 ∧ (e2 ⌋ 𝟙) = e12, while ι_{e_1} e2 = g_12 = 1 and 1 ⌋ 𝟙 = e12. They cancel, and e12 ⟑ 𝟙 = ι_{e_1}(e2) = 1, which matches the pseudoscalar square (−1)^{n(n−1)/2} det g = (−1)(−1) = 1.

**Proof of (a).** Let A = {a_1 < ⋯ < a_k}. By the definition of ⌋, and right(1) = 𝟙,

    e_A ⌋ 𝟙 = ι_{e_{a_1}}(ι_{e_{a_2}}(⋯ ι_{e_{a_k}}(right(1)))).

Apply Lemma A from the inside out. The innermost step gives right(1 ∧ G e_{a_k}) = right(G e_{a_k}). The next gives right(G e_{a_k} ∧ G e_{a_{k−1}}). Each step appends the next image on the right, and after k steps (an induction on the number of steps, with the homogeneous Y = G e_{a_k} ∧ ⋯ ∧ G e_{a_{j+1}} at step j)

    e_A ⌋ 𝟙 = right(G e_{a_k} ∧ G e_{a_{k−1}} ∧ ⋯ ∧ G e_{a_1}) = right(G(e_{a_k} ∧ ⋯ ∧ e_{a_1})) = right(G ẽ_A),

by (G1) and then (G3). With Lemma C, e_A ⟑ 𝟙 = right(G ẽ_A). Both sides are linear in e_A, and the reverse is linear and its own inverse, so applying this to ẽ_A = ±e_A gives ẽ_A ⟑ 𝟙 = right(G e_A). Extend linearly to every x. ∎

**On the sketch this replaces.** The sketch said: the product of any multivector with 𝟙 is its left contraction onto 𝟙 "since every generator lies in 𝟙"; for a vector, e_i ⟑ 𝟙 = Σ_p g_ip right(e_p); for a blade, (a ∧ A′) ⌋ 𝟙 = a ⌋ (A′ ⌋ 𝟙); v ⌋ right(Y) = right(Y ∧ G v); and induction on the grade gives right(G Ã). Every formula in it is correct, including its signs; the computation above follows it. It has one gap. "Every generator lies in 𝟙" proves X ⟑ 𝟙 = X ⌋ 𝟙 only in a diagonal metric, where a blade is the product of orthogonal generators. In a non-diagonal metric a blade is not a product of its generators, and the step needs Lemma B's cancellation, as the remark shows. (If ⌋ were instead defined by grade selection, ⟨X Y⟩ at grade gr Y − gr X, then X ⟑ 𝟙 = X ⌋ 𝟙 would need X ⟑ 𝟙 to be homogeneous, and (a ∧ A′) ⌋ 𝟙 = a ⌋ (A′ ⌋ 𝟙) would need a proof. Either way there is one step to prove.) The sketch also needs the contraction of a multivector to be defined, as above, and v ⌋ right(Y) = right(Y ∧ G v) to be proved, which is Lemma A.

### 3. Claim (b): right(𝔾 x) = G right(x)

**Proof.** 𝔾 x = left(G(right(x))). Apply right to both sides: by (C3), right ∘ left is the identity, so right(𝔾 x) = G(right(x)). ∎

No sign appears. In the same way, 𝔾 = right ∘ G ∘ left as well. *Why:* for x of grade k, left(x) = (−1)^{k(n−k)} right(x) (C4), so right(G left(x)) = (−1)^{k(n−k)} right(G right(x)). The element G right(x) has grade n − k, because G keeps grades, and (C4) for it gives right(G right(x)) = (−1)^{(n−k)k} left(G right(x)). The two signs cancel, and right(G left(x)) = left(G right(x)) = 𝔾 x.

### 4. Claim (c): the scalar part of e_A ⟑ ẽ_B is det g[A, B]

**Statement.** ⟨e_A ⟑ ẽ_B⟩₀ = det g[A, B] if |A| = |B|, and 0 otherwise. Hence ⟨a ⟑ b̃⟩₀ = aᵀ G b for all multivectors a and b.

**Sign conventions.** The reverse is (−1)^{k(k−1)/2} with k = gr(b), applied to the right factor. No complement is involved.

**The idea.** Both sides are bilinear forms that do not depend on the basis, so we can compute them in a basis where g is diagonal.

**Proof.** Define, for X = u_1 ∧ ⋯ ∧ u_k and Y = w_1 ∧ ⋯ ∧ w_k,

    β(X, Y) = det [g(u_i, w_j)]_{i, j = 1..k},

and extend bilinearly. This is well defined, because the determinant is linear in each u_i and each w_j and changes sign when two u's or two w's are exchanged. On basis blades, β(e_A, e_B) = det g[A, B].

Choose a basis f_1, …, f_n of V that is orthogonal for g: g(f_i, f_j) = d_i if i = j and 0 otherwise. Such a basis exists for every symmetric form when 2 is invertible (symmetric elimination, recalled and not reproved here). Write f_S = f_{s_1} ∧ ⋯ ∧ f_{s_k} for an increasing list S.

1. *The f_S of grade k span the same space Λᵏ as the e_A.* Each e_a is a combination of the f's, and expanding e_{a_1} ∧ ⋯ ∧ e_{a_k} gives a combination of the f_S with |S| = k.
2. *For distinct orthogonal f's, wedge and product agree.* By Fact F, f_i ⟑ f_T = ι_{f_i}(f_T) + f_i ∧ f_T, and ι_{f_i}(f_T) = 0 when i ∉ T, since every g(f_i, f_t) is then 0. By induction f_S = f_{s_1} ⟑ ⋯ ⟑ f_{s_k}. Also f_i ⟑ f_j = −f_j ⟑ f_i for i ≠ j, and f_i ⟑ f_i = d_i.
3. *The scalar part in the f basis.* By (G3), f̃_T = f_{t_m} ⟑ ⋯ ⟑ f_{t_1}, so f_S ⟑ f̃_T = f_{s_1} ⋯ f_{s_k} f_{t_m} ⋯ f_{t_1}. If S = T, the middle factors pair off, f_{s_k} f_{s_k} = d_{s_k} and so on outward, and the product is d_{s_1} ⋯ d_{s_k}. If S ≠ T, then anticommuting the factors into order and squaring the repeated ones gives ± (Π_{i ∈ S ∩ T} d_i) times the product of the distinct f_i with i in the symmetric difference S △ T, which is nonempty. By step 2 that product is a wedge monomial of grade |S △ T| > 0, so its scalar part is 0.
4. *β in the f basis.* β(f_S, f_T) = det [d_{s_i} δ_{s_i t_j}]. If S = T the matrix is diagonal with determinant d_{s_1} ⋯ d_{s_k}. If S ≠ T with |S| = |T|, some s ∈ S is not in T, and its row is zero, so the determinant is 0.

So for X, Y in Λᵏ, ⟨X ⟑ Ỹ⟩₀ and β(X, Y) are bilinear and agree on the basis pairs (f_S, f_T). Hence they agree everywhere, and in particular ⟨e_A ⟑ ẽ_B⟩₀ = β(e_A, e_B) = det g[A, B]. For |A| ≠ |B|, step 1 writes e_A and e_B in f_S and f_T of different sizes; then S ≠ T, and step 3 gives scalar part 0. ∎

**The dot.** By bilinearity and (G2),

    ⟨a ⟑ b̃⟩₀ = Σ_{A,B} a_A b_B ⟨e_A ⟑ ẽ_B⟩₀ = Σ_{A,B} a_A det g[A, B] b_B = Σ_A a_A (G b)_A = aᵀ G b,

where the sum runs over pairs of equal grade, since the others contribute 0.

**Worked example: e12 • e12 = +1 in R(3, 0, 0).** Here g is the identity, so e12 = e1 ∧ e2 = e1 ⟑ e2 (since g_12 = 0). The square is e12 ⟑ e12 = e1 e2 e1 e2 = −e1 e1 e2 e2 = −1, so ⟨e12 ⟑ e12⟩₀ = −1. The reverse is ẽ12 = −e12, so ⟨e12 ⟑ ẽ12⟩₀ = +1, which is det g[{1,2}, {1,2}] = det I₂ = 1 = e12 • e12. The reverse is what makes the scalar part of the product agree with the dot on bivectors (and on every grade k with k(k − 1)/2 odd, that is, k ≡ 2, 3 mod 4). In the Witt form [[0, 1], [1, 0]], e12 • e12 = det g = −1, and indeed e12 ⟑ e12 = +1 (section 2, remark), so ⟨e12 ⟑ ẽ12⟩₀ = −1.

### 5. Claim (d): aᵀ 𝔾 b = right(a)ᵀ G right(b), and left(c) = c 𝟙

**Proof.** Let R be the matrix of right. By (C5) the matrix of left is Rᵀ, so 𝔾 = left ∘ G ∘ right has matrix Rᵀ G R, and

    aᵀ 𝔾 b = aᵀ Rᵀ G R b = (R a)ᵀ G (R b) = right(a)ᵀ G right(b).

For a scalar c, left(c) = c left(1), and left(1) ∧ 1 = 𝟙 gives left(1) = 𝟙. So left(c) = c 𝟙. ∎

**The antidot.** Write dot(x, y) for the scalar multivector ⟨x ⟑ ỹ⟩₀ · 1, which is (xᵀ G y) · 1 by (c). Then

    a ∘ b = (aᵀ 𝔾 b) 𝟙 = left((right(a)ᵀ G right(b)) · 1) = left(dot(right(a), right(b))).

**Sign conventions.** None beyond those inside right and left. The left complement on both arguments gives the same number: left(a)ᵀ G left(b) = aᵀ R G Rᵀ b = aᵀ (right ∘ G ∘ left) b = aᵀ 𝔾 b, by section 3.

### 6. Claim (e): G𝔾 = 𝔾G = det(g) I, the double dual, and the duality with the wedge

#### 6.1 G𝔾 = 𝔾G = det(g) I

**A formula for 𝔾.** For |A| = k,

    𝔾 e_A = left(G(ε_A e_{Aᶜ})) = ε_A Σ_{|D| = n−k} det g[Aᶜ, D] left(e_D) = Σ_{|C| = k} ε_A ε_C det g[Aᶜ, Cᶜ] e_C,

substituting C = Dᶜ and using left(e_D) = ε′_D e_{Dᶜ} with ε′_D = ε_{Dᶜ} (from (C3)'s proof). So the entry of 𝔾 is the signed complementary minor: 𝔾_{CA} = ε_A ε_C det g[Aᶜ, Cᶜ].

**Laplace expansion by complementary minors.** For any n × n matrix M with rows r_1, …, r_n and any k-set A,

    det M = Σ_{|C| = k} ε_A ε_C det M[A, C] det M[Aᶜ, Cᶜ].

*Proof.* In Λ, r_1 ∧ ⋯ ∧ r_n = (det M) 𝟙, by the computation in (G1) with k = n. Moving the rows of A to the front, in order, costs the sign of sorting the list (A, Aᶜ), which is ε_A: r_1 ∧ ⋯ ∧ r_n = ε_A (∧_{a ∈ A} r_a) ∧ (∧_{b ∈ Aᶜ} r_b). By the same expansion as in (G1), ∧_{a ∈ A} r_a = Σ_C det M[A, C] e_C and ∧_{b ∈ Aᶜ} r_b = Σ_D det M[Aᶜ, D] e_D. Finally e_C ∧ e_D is ε_C 𝟙 when D = Cᶜ and 0 otherwise. ∎ Also, if two rows of M are equal, det M = 0, since the wedge of the rows contains a repeated vector.

**Proof of G𝔾 = det(g) I.** Using the formula for 𝔾 and (G2),

    G 𝔾 e_A = Σ_C ε_A ε_C det g[Aᶜ, Cᶜ] Σ_B det g[C, B] e_B = Σ_B ( Σ_C ε_A ε_C det g[B, C] det g[Aᶜ, Cᶜ] ) e_B.

Fix B with |B| = k. Let M_B be the matrix whose rows at the positions of A are the rows b_1 < ⋯ < b_k of g, in that order, and whose rows at the positions of Aᶜ are the rows of g at Aᶜ. Then M_B[A, C] = g[B, C] and M_B[Aᶜ, Cᶜ] = g[Aᶜ, Cᶜ], so the inner sum is the Laplace expansion of det M_B. If B = A, then M_B = g and the sum is det g. If B ≠ A, some b ∈ B lies outside A, that is, in Aᶜ. Row b of g then appears twice in M_B, and the sum is 0. So G 𝔾 e_A = det(g) e_A. ∎

**Proof of 𝔾G = det(g) I.** The matrix of G is symmetric (G2), and so is the matrix of 𝔾 = Rᵀ G R. Transposing G 𝔾 = det(g) I gives 𝔾ᵀ Gᵀ = 𝔾 G = det(g) I. ∎

This needs no assumption on det g. In a degenerate metric both products are 0.

#### 6.2 The double bulk dual

**Statement.** With u★ = right(G u), for u homogeneous of grade k (gr = gr(u) = k, ag = ag(u) = n − k),

    u★★ = (−1)^{k(n−k)} det(g) u.

**Proof.** u★ = right(G u) has grade n − k, so G(u★) does too. By (C4) applied to an element of grade n − k, right(G u★) = (−1)^{(n−k)k} left(G u★). So

    u★★ = (−1)^{k(n−k)} left(G right(G u)) = (−1)^{k(n−k)} 𝔾(G u) = (−1)^{k(n−k)} det(g) u,

by 6.1. ∎ The exponent is the same whether it is read with the grade and antigrade of u or of u★. In a degenerate metric u★★ = 0 for every u, so only the complements are invertible.

#### 6.3 a ∧ b★ = (a • b) 𝟙 for a and b of equal grade

**Proof.** Let a and b be homogeneous of grade k. Then G b also has grade k, and by (C1),

    a ∧ b★ = a ∧ right(G b) = (aᵀ G b) 𝟙 = (a • b) 𝟙,

with a • b = aᵀ G b = ⟨a ⟑ b̃⟩₀ by (c). ∎

The equal grades matter. If gr(a) < gr(b), the wedge has grade gr(a) + n − gr(b), strictly between 0 and n, and it need not vanish, while a • b = 0. For example, in R(2, 0, 0), 1 ∧ e1★ = right(e1) = e2. If gr(a) > gr(b), the grade exceeds n and the wedge is 0.

#### 6.4 a ∧ left(G b) = (−1)^{gr·ag} (a • b) 𝟙

**Proof.** For b of grade k, G b has grade k, and by (C4), left(G b) = (−1)^{k(n−k)} right(G b). Then 6.3 gives a ∧ left(G b) = (−1)^{k(n−k)} (a • b) 𝟙, where gr = k is the common grade of a and b and ag = n − k. ∎

So the left bulk dual satisfies the same duality as the right one, with the sign (−1)^{gr·ag}. It is +1 when n is odd and −1 for odd grades when n is even (for example, vectors and trivectors in n = 4).

### 7. The exact check

The script `check.py` checks every claim above exactly, over Python's `Fraction`, on every blade (for the linear claims) or every pair of blades (for the bilinear ones). Since each claim is linear or bilinear, a check on every blade or blade pair is a proof for that form. It covers seven forms: rigid(3) = R(3, 0, 1); conformal(3), with e4 and e5 null and e4 · e5 = −1; R(1, 1, 0); R(2, 0, 2); the degenerate non-diagonal form [[0, 1, 0], [1, 0, 0], [0, 0, 0]]; the Witt form [[0, 1], [1, 0]]; and R(3, 0, 0) for the worked example. Run it with

    python3 -I -B check.py [REPO]

where REPO is a Galoot checkout (the default is the checkout that holds the script, `records/spikes/dual-identities/check.py`). It reads `test/reference/clifford_reference.py` for one of its two geometric products and `test/reference/golden.hpp` for the values it compares, and writes nothing.

**What is independent of what.**

- The script implements the wedge, the complements (from u ∧ right(u) = 𝟙 and left(u) ∧ u = 𝟙), the reverse (as the reversed wedge), the contraction, and the determinant (Leibniz expansion over permutations) itself. None of these share code with `clifford_reference.py`, `golden.py`, or the library.
- G is computed twice: from Gram minors with the Leibniz determinant, and as the wedge of the images of the generators (G1). The two agree on every blade.
- The geometric product is computed twice: by `clifford_reference.py`, which diagonalizes the form and multiplies in the orthogonal basis, and by the script's own recursion from (F1), e_A ⟑ Y = e_a ⟑ (e_{A′} ⟑ Y) − (ι_{e_a} e_{A′}) ⟑ Y, which never diagonalizes. They agree on every blade pair of every form, and every claim that involves a product is checked with both. Fact F itself is checked against `clifford_reference.py`'s product on every generator and blade.
- The forms are written in the script and compared with `clifford_reference.py`'s constructors.
- The lemmas of section 2 (A, B, C, and e_A ⌋ 𝟙 = right(G ẽ_A)) are checked separately, so a failure would point to the step that broke.

Six deliberately wrong variants of the script were each rejected: dropping the reverse in (a), using the left complement in (a), giving the left complement the right complement's sign, taking the scalar part without the reverse in (c), dropping (−1)^{gr·ag} from the double dual, and changing the sign of one coefficient in golden.hpp. The double-dual variant is caught only by the nondegenerate forms, since in a degenerate one both sides are 0.

### 8. What these proofs do not show

- They assume characteristic 0, which the appendix uses to divide by k! and to diagonalize g. Chevalley's construction works more generally, but that is not proved here.
- Claim (c) and 𝔾G = det(g) I use the symmetry of g. For a non-symmetric bilinear form, G is not symmetric and these steps fail as written.
- They concern the mathematics, not any implementation. A program is covered only by tests that compare it with these identities.
- The existence of a g-orthogonal basis when 2 is invertible is a standard fact, recalled and not reproved.

### Appendix. Fact F from the definition of the product

The geometric product of section 0 is the product of the Clifford algebra Cl(g): the associative algebra with unit generated by e_1, …, e_n subject only to e_i e_j + e_j e_i = 2 g_ij. The blade e_A of Cl(g) is the antisymmetrized product α(e_{a_1}, …, e_{a_k}), where α(v_1, …, v_k) = (1/k!) Σ_σ sgn(σ) v_{σ(1)} ⋯ v_{σ(k)}. We show that Cl(g), with these blades, is Λ with v ⟑ Y = ι_v(Y) + v ∧ Y.

1. *Operators on Λ.* For a vector v, let L_v(Y) = ι_v(Y) + v ∧ Y. Then L_v L_w + L_w L_v = 2 g(v, w) on Λ. *Check:* L_v L_w Y = ι_v ι_w Y + ι_v(w ∧ Y) + v ∧ ι_w Y + v ∧ w ∧ Y. By (I1′), ι_v(w ∧ Y) = g(v, w) Y − w ∧ ι_v Y. Add the same expression with v and w exchanged: the ι ι terms cancel by (I2), the v ∧ w and w ∧ v terms cancel, and −w ∧ ι_v Y + v ∧ ι_w Y cancels against −v ∧ ι_w Y + w ∧ ι_v Y. What remains is 2 g(v, w) Y.
2. *A map from Cl(g) to Λ.* By step 1 the operators L_{e_i} satisfy the defining relations, so e_i ↦ L_{e_i} extends to an algebra homomorphism ρ from Cl(g) to the linear maps of Λ. Let φ(x) = ρ(x)(1). Then φ(v x) = L_v(φ(x)) for every vector v and every x in Cl(g).
3. *φ sends each blade to its wedge monomial.* (v_1, …, v_k) ↦ φ(α(v_1, …, v_k)) is linear in each v_t and alternating, and so is (v_1, …, v_k) ↦ v_1 ∧ ⋯ ∧ v_k. Take a g-orthogonal basis f_1, …, f_n. For distinct indices, L_{f_i} acts on f_{j_1} ∧ ⋯ ∧ f_{j_m} with i not among the j's as f_i ∧ (·), since every g(f_i, f_j) in the contraction is 0. So L_{f_{i_σ(1)}} ⋯ L_{f_{i_σ(k)}} 1 = f_{i_σ(1)} ∧ ⋯ ∧ f_{i_σ(k)} = sgn(σ) f_{i_1} ∧ ⋯ ∧ f_{i_k}, and averaging over σ with the sign sgn(σ) gives f_{i_1} ∧ ⋯ ∧ f_{i_k}. With a repeated index both maps give 0. Two alternating multilinear maps that agree on all tuples of basis vectors agree everywhere, so φ(α(v_1, …, v_k)) = v_1 ∧ ⋯ ∧ v_k. In particular φ(e_A) = e_A.
4. *φ is a bijection.* By step 3 it is onto. Cl(g) is spanned by the 2ⁿ ordered products e_{i_1} ⋯ e_{i_k} with i_1 < ⋯ < i_k, since the relations sort any product and replace e_i e_i by g_ii. So the dimension of Cl(g) is at most 2ⁿ, the dimension of Λ, and φ is one to one.
5. *Conclusion.* Identify Cl(g) with Λ through φ. Blades go to wedge monomials (step 3), and the product of a vector v with any Y goes to φ(v φ⁻¹(Y)) = L_v(Y) = ι_v(Y) + v ∧ Y (step 2). That is Fact F. In the same way the wedge of the algebra, the antisymmetrized product, is the exterior product of Λ. ∎
