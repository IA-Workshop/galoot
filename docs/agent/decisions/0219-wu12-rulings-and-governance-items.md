# 0219: The owner's rulings on plan 0030 (WU12) and the open items after the WU14 follow-ups

Date: 2026-11-09. Status: recorded. Source: the owner's reply "Accept all recommendations" to the questions of plan 0030 section 15 and the open items reported after the merge at ba05a36.

## Rulings on plan 0030

1. O1: the DoD-16 matrix representations (Pauli, Dirac, quaternion, complex, M2(R)) stay in WU12, so M5 closes with its full evidence.
2. O2: a change of basis is integer rows over per-row denominators; WU5 (a core rational type) is closed.
3. O3: the interface is a tag carrying source, target and value, with `change_basis`, `change_basis_back`, `conformal_change<D>()` and `from_rows`, the core computing the inverse; names are confirmed after part D (decision 0220).
4. O4: CONVENTIONS 1.17 (distinct primed names in 1.4, the blade map, 1 -> -1', and a new 1.8 on what a change preserves) is ratified after part A's checks earn its labels.
5. O5: the conformal duals are confirmed (3.3's definition over the wiki's Duals table).
6. O6: Sage Route A and `lift_isometry` are a kind-4 black-box cross-check recorded in note 0026; expected values come only from `test/reference/` (C15).
7. O7: Gram matrices are integer; rational forms are reached by rescaling.
8. O8: the basis-dependent operations are documented and pinned; no variants in another basis.
9. O9: the 50-seed campaign runs inside WU12 and again at the M5 gate.

## Rulings on the other open items

10. WORKFLOW W3 gains the criterion 12 sentence (the owner accepted the wording proposed by the plan 0031 author and reviewer, and made the edit mine to write).
11. Theorem R at the structure layer is a small follow-up unit after WU12's plan is settled (reaches n = 6, Witt and random forms).
12. The math-tools manifest's ga-P7 and ga-P11 replay commands are fixed (opam root set before the `cd`) by a math-tools agent.
13. `rga3.headers` joins `tests.messages_pinned`'s labels in the next unit that touches `test/CMakeLists.txt`.
14. The two form-list readings of plan 0031 are confirmed.
