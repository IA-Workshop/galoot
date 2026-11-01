# 0013 The square of the pseudoscalar in the wedge-monomial basis

Date: 2026-11-01. Written by: Claude (agent, Opus 5.5), the implementer of WU6a (plan 0011), as the referent of test A7. Checked by: nobody else yet; the W5 reviewer of WU6a is asked to check it (plan 0011, section 9).

How each claim was checked:

- **Derivation**: the argument below, from the definitions. No published source was read for it.
- **Exhaustive**: compared with every signature that `test/core/blade_product_test.cpp` lists, and with the four golden product tables, on both compilers.

## Claim

For a symmetric bilinear form g on a real vector space with basis e_1, …, e_n, and its Clifford algebra with e_i e_j + e_j e_i = 2 g(e_i, e_j), the wedge-monomial pseudoscalar I = e_1 ∧ e_2 ∧ … ∧ e_n satisfies

I I = (−1)^(n(n−1)/2) det G,

where G is the Gram matrix, G_ij = g(e_i, e_j). This holds for every symmetric g, degenerate or not, diagonal or not. Checked: Derivation, and Exhaustive for the listed signatures.

## Derivation

1. Diagonalize. Over the rationals (any field of characteristic not 2), a symmetric form has an orthogonal basis: there is an invertible matrix T with f_i = Σ_j T_ij e_j and g(f_i, f_j) = d_i δ_ij, that is, T G Tᵀ = D = diag(d_1, …, d_n). Some d_i may be zero when g is degenerate. Then det D = (det T)² det G.

2. The wedge changes by the determinant. The exterior product is multilinear and alternating and does not depend on g, so f_1 ∧ … ∧ f_n = det T · e_1 ∧ … ∧ e_n = det T · I.

3. For mutually orthogonal vectors, the wedge is the geometric product. For a vector v and any multivector x, v x = v ⌋ x + v ∧ x, the identity the header uses for a generator, extended linearly in v. By induction on k, f_1 f_2 ⋯ f_k = f_1 ∧ f_2 ∧ ⋯ ∧ f_k: for k = 1 there is nothing to show; for k > 1, the contraction f_1 ⌋ (f_2 ∧ ⋯ ∧ f_k) = Σ_p ± g(f_1, f_p) (f_2 ∧ ⋯ omitting f_p ⋯ ∧ f_k) vanishes term by term, because g(f_1, f_p) = 0 for p ≠ 1. So f_1 (f_2 ⋯ f_k) = f_1 (f_2 ∧ ⋯ ∧ f_k) = f_1 ∧ f_2 ∧ ⋯ ∧ f_k.

4. Square the orthogonal product. Orthogonal vectors anticommute (f_i f_j + f_j f_i = 2 g(f_i, f_j) = 0 for i ≠ j), so reversing the order of the n factors of f_1 ⋯ f_n takes n(n−1)/2 transpositions: f_n ⋯ f_1 = (−1)^(n(n−1)/2) f_1 ⋯ f_n. Hence

(f_1 ⋯ f_n)(f_1 ⋯ f_n) = (−1)^(n(n−1)/2) (f_1 ⋯ f_n)(f_n ⋯ f_1) = (−1)^(n(n−1)/2) d_1 d_2 ⋯ d_n,

collapsing f_n f_n = d_n, then f_(n−1) f_(n−1) = d_(n−1), and so on from the middle out.

5. Combine. By 2 and 3, (det T)² I I = (f_1 ⋯ f_n)², so by 4 and 1,

I I = (−1)^(n(n−1)/2) det D / (det T)² = (−1)^(n(n−1)/2) det G. ∎

Two remarks. Steps 1 to 5 use rational T and rational d_i, but both sides of the result are integer polynomials in the entries of G (the left side because every coefficient of a product of wedge monomials is one, research note 0009, B3), so the identity holds for integer forms as written. And for a degenerate g, det G = 0 and the square is zero, so I I has no term: rigid(3) and the degenerate form [[0, 1, 0], [1, 0, 0], [0, 0, 0]] give 0.

## Values the test writes out

| Form | n | det G | (−1)^(n(n−1)/2) | I I |
| --- | --- | --- | --- | --- |
| conformal(3) | 5 | −1 | +1 | −1 |
| rigid(3) | 4 | 0 | +1 | 0 |
| degenerate form | 3 | 0 | −1 | 0 |
| R(3,0,0) | 3 | 1 | −1 | −1 |
| R(1,3,0) | 4 | −1 | +1 | −1 |
| seeded random form 1 | 3 | −31 | −1 | 31 |

## How the test uses it

Test A7 (`The pseudoscalar squares to (-1)^(n(n-1)/2) det g`) computes det G by the Leibniz formula in the test, for every signature it lists, and compares e_(1…n) e_(1…n) from `blade_product` with the single scalar term (−1)^(n(n−1)/2) det G, or with no term when det G = 0. It also compares the test's det G of the four golden tables with the golden `*_det` values, which `golden.py` takes by elimination in `clifford_reference.det` and checks against the table's own pseudoscalar square before writing.

## Open

The reviewer of WU6a checks this derivation (plan 0011, section 9). Until then it stands as the author's derivation, consistent with the exhaustive comparison.
