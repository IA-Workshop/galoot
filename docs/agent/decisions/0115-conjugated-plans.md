# 0115 The antiproducts as conjugated plans, generated pair by pair

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), implementing WU7b of plan 0013 in the role of cpp-expert.

## Context

CONVENTIONS 4.2 defines a ⟇ b = left(right(a) ⟑ right(b)) and a ∨ b = left(right(a) ∧ right(b)) for every metric. Constitution C4 admits one generative mechanism, composed with exact per-basis data. Plan 0013 (section 3, WU7b) describes the conjugation at the level of sets: make_product_plan complements both input sets, folding each complement's sign into the oriented blade, generates the base plan over them, complements each output on the left, and reverses each pair's run, because the complement reverses canonical order (plan section 2). Criterion 17 asks that the run reversal be what the generation-time order check verifies.

## Options weighed

1. As the plan describes it, at the level of sets inside make_product_plan: two complemented sets and a pass over the generated plan as further `static constexpr` locals. A run-time test of an antiproduct plan would then have to repeat the composition, so a defect in make_product_plan's composition would be visible only to the compile-time tests.
2. In generate_plan, pair by pair: each factor enters the base product's pair rule as its right complement, with its sign, and each base term is replaced by its left complement, the pair's run read from the last. Chosen. The arithmetic is the plan's, term for term; generate_plan<antigp> and <antiwedge> are callable at run time, so the exhaustive tests run the code the compile-time plan runs, and make_product_plan is unchanged apart from passing n to the capacity.
3. A separate generator for the antiproducts. A second mechanism (C4).
4. Building each conjugated run with `term_list::add`, which inserts in canonical order. The reversal would happen by sorting, so the reversal mutant would be equivalent and every insertion would scan the run.

## Decision

1. `BinaryOperation` gains `antigp` and `antiwedge`. `base_operation(op)` names the product each conjugates.
2. `factor_of<Op, N>(a)` gives the blade a factor e_A enters the base rule as: for an antiproduct its right complement, `complement_mask(a, N)` with coefficient `right_complement_sign(a, N)`; otherwise e_A with 1. `run_term<Op, Capacity, N>(terms, k)` gives the term at place k of a pair's run: for an antiproduct the left complement of the base term at place size − 1 − k, coefficient times `left_complement_sign(D, N)` on `complement_mask(D, N)`; otherwise the term at k. generate_plan multiplies s_a s_b by both factors' coefficients and writes the run as before, so a and b keep the positions of A and B.
3. `pair_terms` is constrained to the geometric product and the wedge (`requires`), since the antiproducts are built on it rather than in it; a test shows it rejects both antiproducts.
4. `pair_capacity(op, a, b, n)` and `plan_capacity(op, as, bs, n)` take n (commit `54b17ae`, with the migration of their test calls): the geometric antiproduct's pair needs `product_capacity` of the two complements, and a complement depends on n; the antiwedge needs 1. The plan did not foresee this signature change.
5. The checks of a generated plan are unchanged. The order check, which requires each pair's terms in strictly increasing canonical order, now also verifies the reversal: a run left in the base product's order is in decreasing order (a test shows the check flags e1235 ⟇ e1234 of conformal(3) unreversed). The closed-form check of the complement signs lives in the maps (decision 0114); a conjugated plan's signs are checked against the definition, the De Morgan laws, and the golden composition by the tests.
6. A stray generator in a direct call of generate_plan for an antiproduct is rejected by `complement_mask`'s own range check in `bits.hpp` (`core.plan_antiproduct_pair_beyond_n` and `_second`); through product_plan the set check comes first (`core.plan_antigp_beyond_n`).

## Proofs at the level of plans

The tests check each of these exhaustively over their finite ranges; here is why each holds. Write σ(u) = (−1)^(gr(u)·ag(u)) for a blade u, so that right(right(u)) = left(left(u)) = σ(u) u and left(u) = σ(u) right(u) (CONVENTIONS 3.1).

- The complement reverses canonical order. Of two blades of different grades, the complements have the grades exchanged. Of two of one grade, u and v differ in the same generators as their complements, and canonical order within a grade is decided by the lowest generator of u xor v: the blade holding it comes first, and that generator belongs to u exactly when it does not belong to the complement of u. So a base run in increasing order becomes decreasing, and reading it from the last restores increasing order with no sort. (Also checked for every pair of masks, n = 1 to 8, while planning.)
- De Morgan, left: left(a) ⟇ left(b) = left(right(left(a)) ⟑ right(left(b))) = left(a ⟑ b), since right∘left is the identity. The same for ∧ and ∨.
- De Morgan, right: right(a) ⟇ right(b) = left(right(right(a)) ⟑ right(right(b))) = σ(a)σ(b) left(a ⟑ b) = Σ_D σ(a)σ(b)σ(D) c_D right(e_D) over the terms c_D e_D of a ⟑ b. Every such D has gr(D) ≡ gr(a) + gr(b) (mod 2), because the product preserves the parity of grades. If n is odd, k(n − k) is even for every k, so every σ is 1. If n is even, k(n − k) ≡ k (mod 2), so σ(a)σ(b)σ(D) = (−1)^(gr a + gr b + gr D) = 1. So right(a) ⟇ right(b) = right(a ⟑ b), and the same for ∧ and ∨.
- Associativity: (a ⟇ b) ⟇ c = left(right(left(right(a) right(b))) right(c)) = left(right(a) right(b) right(c)), and a ⟇ (b ⟇ c) gives the same, by associativity of ⟑ and of ∧.
- Identities: right(𝟙) = 1, so 𝟙 ⟇ x = left(right(x)) = x, and x ⟇ 𝟙 = x likewise.
- u ∨ right(u) = left(right(u) ∧ σ(u) u) = σ(u) left(σ(u) u ∧ right(u)) = left(𝟙) = 1, using right(u) ∧ u = σ(u) u ∧ right(u); and left(u) ∨ u = left(right(left(u)) ∧ right(u)) = left(u ∧ right(u)) = left(𝟙) = 1.
- Map relations: the reverse and the grade involution square their signs to 1. right(right(e_A)) carries the inversions of (A, Ā) and of (Ā, A), k(n − k) in all, so σ. left(right(e_A)) carries the inversions of (A, Ā) twice, so 1. The antireverse: left(reverse(right(e_A))) = s · rev(Ā) · s e_A, with s the right sign of A and the left sign of Ā, and rev(Ā) = (−1)^(m(m−1)/2) for m = n − k, which is the antireverse's sign.
- Reversal rules: the reverse is an anti-automorphism of ⟑ (it reverses each product of generators), so reverse(a ⟑ b) = reverse(b) ⟑ reverse(a); conjugating by the complements gives antireverse(a ⟇ b) = antireverse(b) ⟇ antireverse(a).

## Evidence

- `test/core/plan_test.cpp` from `a913760`: both antiproduct plans of every pair of the four golden forms (n = 3, 4, 5) equal left(right(a) × right(b)) composed from the golden product tables and complements alone, the antiwedge with the test's own wedge; the De Morgan laws with either complement for every pair of the 42 forms (n ≤ 6), compared as term lists; ⟇ and ∨ associative on every triple of the 42 forms (n ≤ 6, 0.61 s on gcc-debug), 𝟙 and 1 as identities, u ∨ right(u) = left(u) ∨ u = 1, and the reversal rules for every pair; the hand values of rigid(3) (e4 ⟇ e4 = −𝟙 and the others of plan section 6, D3) and the two-term run e1235 ⟇ e1234 = −e123 + e12345 in conformal(3); the counts and result sets of D7 with every term of plane ∨ plane; compile time equal to run time for the antiproduct plans of `full<4>` in rigid(3) and conformal(2).
- `core.plan_evaluated_once` includes motor ⟇ point (commit `c6b7a4c`).
- Mutants: `records/mutations/0013-plan-maps/`, including the run reversal dropped and the left and right complements swapped.

## Consequences

- WU10 builds the public `antigp` and `antiwedge` and the operators `%` and `&` on these plans.
- For non-diagonal forms the antiproducts rest on the definition, the De Morgan proofs, and the golden composition (verification); no published worked value of a non-diagonal antiproduct has been read (plan departure 8), so they are not validated.
