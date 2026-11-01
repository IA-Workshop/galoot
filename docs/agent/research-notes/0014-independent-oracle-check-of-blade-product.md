# 0014 Independent check of the geometric product tables with the math tools

- Date: 2026-11-01
- Checked by: Claude (agent, Opus 5.5, math-expert role) at `develop` `b2deca7`; coordinator summary by Claude (agent, Sonnet 5.5). The scripts, dumps, and the use log are outside the repository, in `/home/brianm/projects/utils/math-core/math-tools/trial-wu6a/` and `logs/use-log.md`; they are the evidence and are not cited from code.

## Result

No oracle disagrees with galoot on any value or on term order. 100 forms with n up to 6 were dumped from the compiled `blade_product` (GCC 16.2.1 and Clang 23.1.4 give byte-identical output): the 42 test signatures and R(0,0,5) and R(0,0,6) through the library's own factories, 44 seeded extra forms (dense, all-null, low-rank, conformal(3) with the null pair first, and others), and 20 forms with entries up to 32767 for the overflow guard. 117,184 products in all.

| Check | Result |
| --- | --- |
| Exact Python route W (own wedge-monomial reduction, python-flint) against galoot | 0 mismatches; term lists have no zero coefficient or repeat and are in canonical order |
| Pseudoscalar square against (−1)^{n(n−1)/2} det G | 0 failures on 100 forms |
| `golden.hpp` `gp_tables` (1,408 rows) and `wedge_basis` (9 rows) against route W | 0 mismatches |
| SageMath `CliffordAlgebra` (passagemath-modules 10.8.13), route A, 57 non-diagonal and degenerate forms | 0 mismatches over 56,292 pairs; e4⟑e5 = −1 + e45 for the conformal null pair |
| galgebra 0.6.0, 55 non-diagonal and degenerate forms | 0 mismatches over 48,160 pairs |
| galgebra symbolic tables, n = 3 to 6 | The pseudoscalar square equals (−1)^{n(n−1)/2} det G as a polynomial; every coefficient on e_C is homogeneous of degree (|A|+|B|−|C|)/2 |
| Z3 5.3.3 | `add_checked` proved exact at 16, 32, and 64 bits; the reorder-sign cocycle at 6 and 8 bits directly; complement signs for every n up to 16; wider widths timed out |

## Findings that touch the repository

1. The A7 marker in `test/core/blade_product_test.cpp` calls the listed numbers values of det g. They are pseudoscalar squares (det R(3,0,0) is +1; det of random form 1 is −31). The code and note 0013 are right; the marker wording is not.
2. The generic maximum number of terms of a product, by |A ∪ B| = 0 to 6, is 1, 1, 2, 4, 7, 14, 29 (galgebra), against the bound `product_capacity` of 1, 1, 2, 4, 8, 16, 32. The bound is safe and tight to |A ∪ B| = 3.
3. 16 of 300 probe forms were rejected by the overflow guard although their exact tables fit in 64 bits (largest coefficient between 2^59 and 2^63), because an intermediate overflowed. This is within the documented contract ("every coefficient formed") but accepts fewer forms than fit; one sentence in the `coefficient_type` comment or decision 0101 should say so.

## Candidate committed values, not yet ported

The pseudoscalar squares of conformal(1) to conformal(4): +1, −1, −1, +1. The term-count sequence above. Two guard-boundary forms with exact values written out (one accepted at about 2^61, one rejected although its table fits). A second route inside `golden.py` that cross-checks the reference tables before they are written (addresses plan 0011 risk 1). Porting each is a unit with its own plan and review.

## Tools that earn a permanent role

The exact harness (route W with python-flint) and Sage route A for bulk tables; galgebra for symbolic identities only (45 to 113 s per dense n = 6 form); Z3 narrowly (small widths and the add guard).

## Unverified

Every dump is a run-time evaluation in a `GALOOT_CHECKING=1`, -O2 build; compile-time agreement rests on the existing test that compares the two for rigid(3). Z3 did not reach 32-bit proofs for the reorder-sign claims. Hypothesis, kingdon, ganja, Oscar, Grassmann, and Shewchuk were not used.
