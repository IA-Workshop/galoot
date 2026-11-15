# 0009 Spike B: an exact Clifford reference and integer product tables

Date: 2026-10-31. Checked by: a spike subagent (Claude Opus 5.5) working for the owner. Scope: plan 0001, section 9, Spike B. It adds `test/reference/clifford_reference.py` and answers whether geometric-product tables in the wedge-monomial basis (decision 0003) have integer coefficients for integer Gram matrices. No library header exists yet, so nothing here tests Galoot itself.

Revision: work started at `68eb9813` and the owner committed governance changes while it ran. Every check below last ran at `870343ca45d0e81f07ddd037dd59697644751d2a` (`develop`), plus uncommitted files: `test/reference/clifford_reference.py` and the workbench scripts named below. None of the intervening commits touch `test/reference/` or the workbench. The scripts and results were afterward copied unchanged to `docs/agent/records/spikes/` and committed, so the figures are reproducible from that commit.

How each claim was checked:

- **Exhaustive**: every blade pair, or every blade triple, of the named signature was checked by the script. For that signature this is a proof (V1, kind 1).
- **Sampled**: seeded random signatures. The results support a hypothesis for other signatures, not a theorem (V9).
- **Derivation**: my own argument from the definitions, written out below. No one else has checked it.
- **Inferred**: my reasoning, not checked.

## Summary

1. `clifford_reference.py` computes the full geometric-product table for any symmetric integer Gram matrix by a route that the library will not use: symmetric LDLᵀ diagonalization over `fractions.Fraction`, the single-term sign rule in the orthogonal basis, and a change back to wedge monomials through minors of the change-of-basis matrix. It validates each table by the defining relations, associativity on every basis triple, and a third check that pins the basis to wedge monomials.
2. conformal(3): every coefficient is an integer, in fact −1, 0, or 1. 128 of the 1024 blade pairs give more than one term, never more than two (Exhaustive).
3. R(1,3,0) and rigid(3) are diagonal, so every pair gives at most one term, with coefficients ±1 or 0. In ten seeded random forms with n ≤ 5, and in one degenerate non-diagonal form, every coefficient is an integer (Exhaustive per signature; Sampled over forms).
4. Wedge-basis coefficients are integers for every symmetric integer Gram matrix of every n (Derivation). So they are never non-integer and never non-dyadic. The orthogonal squares that LDLᵀ produces are often non-dyadic, for example −10/3 and 118/15. That is where the design study's "dyadic" claim fails, and it is why plan coefficients can be integers while the change of basis needs rationals (decision 0003).
5. A cross-check against Spike A's integer generator, which computes the same tables by the contraction recursion, agrees on every blade pair of all 14 signatures on both compilers (note 0008).

## The script

`test/reference/clifford_reference.py` uses only the Python standard library and no external geometric-algebra code (C15, C16). Its route:

1. `diagonalize(g)` returns T and d with T g Tᵀ = diag(d). Row i of T gives the orthogonal vector f_i in the stored basis. A zero pivot is replaced by a later nonzero diagonal entry (a swap), or else by f_k + f_j for some j with g(f_k, f_j) ≠ 0, whose square 2 g(f_k, f_j) is nonzero. A row with nothing to pair with lies in the radical and keeps square 0. The function asserts that T g Tᵀ is diagonal.
2. `change_of_basis(m, n)` maps each blade A to Σ_S det(m[A, S]) S, the outermorphism of a change of vector basis. Wedges need no metric, so this is exact in both directions: e_A in f blades uses T⁻¹, and f_S in e blades uses T.
3. `Algebra.mul` expands both factors in f blades, multiplies by f_S f_T = reorder_sign(S, T) · Π_{i ∈ S∩T} d_i · f_{S△T}, and maps the result back to e blades.

`validate` makes three checks on each table:

1. Defining relations: e_i e_j + e_j e_i = 2 g_ij for all i and j.
2. Associativity on every triple of basis blades, (2ⁿ)³ triples.
3. Wedge-monomial basis: each e_A equals (1/k!) Σ_σ sgn(σ) e_{a_σ(1)} ⋯ e_{a_σ(k)}. Checks 1 and 2 hold in any basis of the algebra, including the basis of Clifford-product monomials, so only this check tells the convention of decision 0003 apart from that alternative.

Commands, from the repository root:

```bash
tools/timed test - clifford_reference.py -- \
  python3 -I docs/agent/records/spikes/spike-a/measure.py -- \
  python3 -I test/reference/clifford_reference.py
# 3.6 s wall, 13.6 MiB peak at 870343ca; prints the survey table and "all reference checks passed"

python3 -I test/reference/clifford_reference.py table conformal 3   # full table, 1024 lines
python3 -I test/reference/clifford_reference.py table random 3      # seeded form 3 (n = 5)
python3 -I test/reference/clifford_reference.py table gram 0,-1;-1,0
```

Seed: `SEED = 20261031` in the script. Random form k has n = 2 + (k mod 4), so n runs through 2, 3, 4, 5, and upper-triangle entries are drawn uniformly from [−3, 3].

### The checks can fail

`docs/agent/records/spikes/spike-b/fault_injection.py` (run as `python3 -I docs/agent/records/spikes/spike-b/fault_injection.py`) injects three faults into the conformal(3) table. Each was caught by the check it targets:

| Fault | Check that caught it | Failures reported |
| --- | --- | --- |
| e4 e5 with scalar term +1 instead of −1 | 1, `anticommutator e4 e5` | 128 |
| e12 e23 doubled (no vector pair changed) | 2, `associativity e1 e2 e23` | 122 |
| Clifford-product monomials stored instead of wedge monomials | 3 only, `wedge monomial e45` | 8, one per blade that holds both e4 and e5 |

The third row is the evidence that checks 1 and 2 do not pin the basis and check 3 does.

## Findings

### B1 conformal(3)

- **Claim.** With the CONVENTIONS 1.4 form (e1 to e3 Euclidean, e4 and e5 null, g(4,5) = −1), every coefficient of the 1024-pair table is −1, 0, or 1. 128 pairs give two terms, 768 give one, 128 give zero, and none gives more than two. Checked: Exhaustive, with checks 1 to 3 passing.
- **Claim.** The two-term pairs are exactly those in which one factor contains e4 but not e5 and the other contains e5 but not e4: 8 × 8 pairs each way, 128 in all. Checked: Exhaustive, by comparing the set of two-term pairs with that predicate (one-off command, given under "Reproduction").
- **Claim.** Hand checks: e4 e5 = −1 + e45, e5 e4 = −1 − e45, e4 e45 = e4, e45 e4 = −e4, e45 e45 = 1. Checked: by hand from e45 = e4 e5 + 1 and the anticommutator, and matched against the table.
- **Claim.** The LDLᵀ route gives orthogonal squares (1, 1, 1, −2, 1/2) for conformal(3). The diagonal counterpart of CONVENTIONS 1.4, R(4,1,0), has squares (1, 1, 1, 1, −1). The two differ because the script's elimination does not normalize its vectors, which is allowed: any diagonalizing basis serves the reference. Checked: Exhaustive (script output). Inferred: the library's change of basis to R(4,1,0) is a different T and has to be checked separately (WU12).

### B2 R(1,3,0), rigid(3), the degenerate form, and ten random forms

The survey output, verbatim except for the column order. "Multi" is the number of blade pairs with more than one term, "max" the largest number of terms in one product, and "max |c|" the largest coefficient magnitude.

| Signature | n | Gram (rows split by ;) | det | Orthogonal squares | Multi / pairs | Max | Integer | max \|c\| |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| conformal(3) | 5 | 1,0,0,0,0;0,1,0,0,0;0,0,1,0,0;0,0,0,0,−1;0,0,0,−1,0 | −1 | 1,1,1,−2,1/2 | 128 / 1024 | 2 | yes | 1 |
| R(1,3,0) | 4 | diag(1,−1,−1,−1) | −1 | 1,−1,−1,−1 | 0 / 256 | 1 | yes | 1 |
| rigid(3) | 4 | diag(1,1,1,0) | 0 | 1,1,1,0 | 0 / 256 | 1 | yes | 1 |
| degenerate | 3 | 0,1,0;1,0,0;0,0,0 | 0 | 2,−1/2,0 | 6 / 64 | 2 | yes | 1 |
| random[0] | 2 | −1,−2;−2,3 | −7 | −1,7 | 6 / 16 | 2 | yes | 7 |
| random[1] | 3 | 2,0,−3;0,1,−3;−3,−3,−2 | −31 | 2,1,−31/2 | 36 / 64 | 4 | yes | 31 |
| random[2] | 4 | −2,0,−2,2;0,−2,−3,3;−2,−3,1,3;2,3,3,3 | 236 | −2,−2,15/2,118/15 | 204 / 256 | 7 | yes | 236 |
| random[3] | 5 | 3,1,−3,−1,−2;1,−3,−2,1,0;−3,−2,−2,3,0;−1,1,3,3,2;−2,0,0,2,−1 | −240 | 3,−10/3,−47/10,176/47,−15/11 | 912 / 1024 | 14 | yes | 240 |
| random[4] | 2 | 3,1;1,−2 | −7 | 3,−7/3 | 6 / 16 | 2 | yes | 7 |
| random[5] | 3 | −3,−3,0;−3,−2,2;0,2,0 | 12 | −3,1,−4 | 28 / 64 | 4 | yes | 12 |
| random[6] | 4 | 0,1,−1,0;1,−1,0,−1;−1,0,2,0;0,−1,0,−1 | 2 | −1,1,1,−2 | 164 / 256 | 6 | yes | 2 |
| random[7] | 5 | −3,1,2,−1,−2;1,−3,0,−3,−1;2,0,0,3,−3;−1,−3,3,1,−2;−2,−1,−3,−2,2 | −875 | −3,−8/3,3/2,4,−875/48 | 900 / 1024 | 14 | yes | 875 |
| random[8] | 2 | 3,−2;−2,−2 | −10 | 3,−10/3 | 6 / 16 | 2 | yes | 10 |
| random[9] | 3 | −3,1,−2;1,−3,−2;−2,−2,1 | 40 | −3,−8/3,5 | 42 / 64 | 4 | yes | 40 |

- **Claim.** In each of these 14 tables every coefficient is an integer, and checks 1 to 3 pass. Checked: Exhaustive for each listed signature (every pair and every triple).
- **Claim.** For integer forms in general this is Sampled evidence only: ten forms, seed 20261031, entries in [−3, 3], two or three of each n from 2 to 5. It supports the derivation in B3; it does not prove it.
- **Withdrawn after review (see B5).** In every random form the largest coefficient magnitude equals |det g|; this is false in general, and |det g| is only a lower bound. The text that follows is the original claim. Checked: Exhaustive for the 14 tables. Inferred: the pseudoscalar square is e_{1…n} e_{1…n} = (−1)^{n(n−1)/2} det g, which would explain it. For n = 16 with entries of moderate size, det g can exceed the range of a 32-bit integer, so the plan coefficient type matters (bears on DoD-7 and decision 0004; not measured here).
- **Claim.** In dense random forms most pairs give several terms, up to 14 at n = 5. So the "single term per pair" picture holds only for diagonal forms, and a plan's term count depends on the form, not only on n. Checked: Exhaustive for the listed forms.

### B3 Integer coefficients for every integer form (question 3)

- **Claim.** If g is a symmetric integer matrix, every coefficient of e_A e_B in the wedge-monomial basis is an integer. Checked: Derivation, below; independently reviewed and confirmed (B5). It is consistent with the 14 exhaustive tables of B2.

Derivation. Let Λ be the set of integer combinations of the wedge monomials e_A. For a generator e_i and any x, the Clifford product satisfies e_i x = e_i ⌋ x + e_i ∧ x (the Chevalley identity). For x = e_B, the wedge e_i ∧ e_B is ±e_{B∪{i}} or 0, and the left contraction e_i ⌋ e_B = Σ_p (−1)^(p−1) g(i, b_p) e_{B∖b_p} has integer coefficients, so e_i Λ ⊆ Λ. Now let a be the lowest index of A, with A' = A ∖ {a}. Then e_A = e_a ∧ e_{A'} = e_a e_{A'} − e_a ⌋ e_{A'}, and the contraction is an integer combination of wedge monomials of grade |A| − 2. So e_A x = e_a (e_{A'} x) − (e_a ⌋ e_{A'}) x. By induction on the grade of A, with e_∅ = 1 as the base, e_A Λ ⊆ Λ for every A. ∎

The same recursion is what Spike A's C++ generator computes, so the derivation also explains why that generator needs only integer arithmetic.

- **Consequence.** Wedge-basis coefficients are never non-integer, and so never non-dyadic, for any n (Derivation, confirmed in B5; Exhaustive only for the 14 listed signatures). Non-dyadic rationals appear only in the orthogonal squares and in T: for example −10/3 in random[3] and random[8], 118/15 in random[2], and −875/48 in random[7]. Checked: Exhaustive (script output above).
- **Proven by exhaustion**: integrality and checks 1 to 3 for the 14 listed signatures. **Proven by derivation**: integrality for every symmetric integer form, pending review. **Sampled**: the other observations of B2 (term counts, max |c| = |det g|) for forms outside the list.

### B4 Independence and cross-check

- **Claim.** The script shares no code with Galoot (there is no library code yet) or with Spike A's generator. It takes a different route: rational diagonalization and minors, against integer contraction recursion in the stored basis. Checked: by construction.
- **Claim.** `docs/agent/records/spikes/spike-a/diff_reference.py` compares the table printed by Spike A's generator, run at run time, with `clifford_reference.py table` for all 14 signatures. All agree line for line on both compilers. Checked: Local (`tools/timed test - spike-a:diff_reference -- python3 -I docs/agent/records/spikes/spike-a/diff_reference.py gcc clang`, 3.1 s).
- **Caveat.** Plan 0001 risk 5 applies: the script and the generator were written by the same agent in one session, so the routes are independent but the author is not. The owner's review of the script, and checks 1 to 3, which come from definitions rather than either implementation, are the mitigation.

## Reproduction

```bash
cd /home/brianm/projects/utils/math-core/galoot
python3 -I test/reference/clifford_reference.py                  # survey + checks
python3 -I docs/agent/records/spikes/spike-b/fault_injection.py       # the checks can fail
python3 -I -c "
import sys; sys.path.insert(0, 'test/reference')
import clifford_reference as r
t = r.Algebra(r.conformal(3)).table()
multi = sorted(k for k, v in t.items() if len(v) > 1)
pred = sorted((a, b) for a in range(32) for b in range(32)
              if (a >> 3 & 1 and not a >> 4 & 1 and b >> 4 & 1 and not b >> 3 & 1)
              or (a >> 4 & 1 and not a >> 3 & 1 and b >> 3 & 1 and not b >> 4 & 1))
print(len(multi), multi == pred)"                                # 128 True
```

Toolchain: Python 3.14.8. Machine: Intel Core i9-14900K, Linux 7.3.0-4-cachyos.

## Open questions

1. Should the reference also emit a golden C++ header (plan 0001, M0 exit evidence), and in what format? Not done; the table format here is plain text.
2. The derivation in B3 should be reviewed by someone other than its author before it is cited as kind 1 evidence for forms outside the list.
3. Exhaustive associativity costs (2ⁿ)³ table lookups, about 3.7 s for this set in Python. At n = 6 that is 262,144 triples per form, which is still practical; n = 8 (16.7 million triples) would need sampling or the C++ side.
4. The plan coefficient type: is `int` enough? That max |c| = |det g| was observed, not proven, so it is no bound. The pseudoscalar square alone reaches |det g|, which for n = 16 forms with entries of a few units can exceed 2³¹ (Inferred). Decide in WU6 with DoD-7.

### B5 Independent review (2026-10-31)

A blind reviewer (Claude Fable 5.1, no access to this note's argument until after writing its own) derived the result independently, then ran the script and its own Chevalley-route check. Full record: `docs/agent/records/reviews/2026-10-31-integer-coefficients.md`; its scripts are in `docs/agent/records/spikes/spike-b-review/`.

- **Confirmed (kind 1, derivation).** For every symmetric integer g, every coefficient of e_A e_B in the wedge-monomial basis is an integer, and this holds for all pairs if and only if g is integral. The proof uses the operators e_i ∧ and the contraction by g, which satisfy the defining relation with g, so the exterior algebra is a Clifford module; the factor 2 sits in the relation, not in the lattice. The recursive blade and the antisymmetrized blade of CONVENTIONS 1.1 are the same element.
- **Half-integer forms.** For g = ½ × an integer matrix the coefficients are dyadic and not integral in general; the denominator divides 2^((|A|+|B|−|C|)/2). Example: g = [[0, ½],[½, 0]] gives e_1 e_2 = ½ + e_12.
- **Corrected.** The largest coefficient is not |det g|: it is at least max |g_ij| (e_i e_ij = g_ii e_j − g_ij e_i), and 61 of about 300 seeded forms violate the earlier claim. A plan's coefficient type cannot be sized from |det g|.
- **Corroborated (kind 3).** The reviewer's tables agree with the script's on all 14 signatures and 9 half-integer forms, with no mismatches.
- **Referred to the conventions owner.** CONVENTIONS 4.1 defines the wedge as the terms of a ⟑ b whose blades share no generator, which is the diagonal-metric rule. In conformal(3) it would make e4 ∧ e5 equal −1 + e45, against 1.1. A reworded definition is proposed in decision 0023.
