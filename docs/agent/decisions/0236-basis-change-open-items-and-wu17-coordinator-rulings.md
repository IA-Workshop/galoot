# 0236 Basis-change open items and WU17 coordinator rulings

Date: 2026-11-10. The owner replied "Accept the recommendations" to the explanation of O1 to O3 from the `basis_change` unit (plan 0039, decision 0235).

## Owner rulings

1. **O1, `detail::inverse_change`.** Options: (a) leave it; (b) give it the reduction; (c) have `basis_change_identities_test.cpp` read the checked inverse rows and delete the function. Accepted: (c), as a small cleanup unit (C12: no test-only code in a header). Backlog until scheduled; it needs its own review.
2. **O2, remaining spurious overflows after the reduction.** Options: (a) skip the elimination's discarded left-block entries; (b) check inner products in reduced fractions; (c) leave. Accepted: (c) for 1.0, a backlog item after v1 unless a user hits it.
3. **O3, a hint on the deleted `==`.** Options: add `bool operator==(const basis_change&) const = delete;` with a comment naming `same_rows`, or leave the generic error. Accepted: add it in the pre-release diagnostics pass, with its own core review.

## Coordinator rulings within plan 0038 (reversible by the owner)

4. **Harness size** (about 150 lines against decision 0014's "about 100"): accepted as recorded.
5. **Part D's single author (D1, D2) and part C's (M3, L1):** the blind and delta reviews' mutant probes stand in for independent test authoring on those parts. New code parts get a separate test author.
6. **Part C's criterion 11 gap mutants:** two of the four named mutants do not close a gap (the existing suite kills them), and `first_stage_all` is killable by a degenerate-form counting case. The gap is stated against the whole existing suite; the unit adds that case and a true gap mutant (`double_negation`).
7. **Part C's two programs per preset (checking on and off) and the trace row under CONVENTIONS 4.4:** accepted.
