# 0192 Plan 0021's questions O1 to O8 and departure 7; CONVENTIONS 1.10 approved with three changes

- Status: accepted (owner answer 2026-11-07, relayed by coordinator: "proceed with recommendations", M3 gate report owner decision 10). CONVENTIONS version 1.10 is ratified in the form below; it takes effect when WU13a commits it (plan 0021 section 4 step 2), and its 4.3 antiprojection sentences only when the condition of item 2(c) is met.
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), records clerk role
- Bears on: plan 0021 (departures 1, 2, 4, 7; R2, R9; section 8); CONVENTIONS 2.1, 4.3, 4.4, 4.5, 7.1, 5.4 to 5.6; decisions 0009, 0010, 0175, 0182, 0188; V9

## Context

Plan 0021 (WU13: `compose`, `transform`, `antiproject`, and the rounding check of `transform`) left eight questions to the owner, among them the text of CONVENTIONS 1.10, and declared in departure 7 that WU13a pins 5.4 to 5.6 in the core's tests. The M3 gate report recommended yes to each, with three changes to the proposed 1.10 text:

- (a) The proposed 4.4 uses x̌, the antigrade involution, which 2.1 does not define.
- (b) O5 leaves `std::complex<double>` and other inexact scalars without IEEE limits unchecked, but ratified 4.5 says an operation that drops part of its own result "verifies on a rounding scalar" that the dropped part is within its bound. The two disagree unless 4.5 says which scalars are checked.
- (c) The proposed 4.3 antiprojection sentences rest on a summarized fetch of the rigid wiki's Projections page, a substitute source under V9.

## Decision

1. **O1.** M3 is accepted (decision 0182); WU13a may branch.
2. **O2.** CONVENTIONS 1.10 is the text proposed in plan 0021 section 8, with these changes:
   - (a) 2.1 defines the antigrade involution: "the antigrade involution multiplies a blade of antigrade m by (−1)^m".
   - (b) 4.5 says which rounding scalars the check of an operation that drops part of its own result covers, the scalars of O5 (IEEE binary types rounding to nearest, with float and double tested and `long double` neither tested nor claimed), and that other inexact scalars, such as `std::complex<double>`, are not checked.
   - (c) 4.3's antiprojection sentences are ratified only after the Projections page has been read directly (its source text, not a summary) under V9, and `conventions_check.py`'s new antiprojection checks pass. If the reading differs from b ∧ (a ∨ b☆), section 8 gains a row and the owner decides (C11, plan 0021 risk 3).

   The 4.4 changes (the antireverse in place of Ṽ, the parity of the antigrade, the odd sign −V ⟇ x̌ ⟇ V̰, the one-parity condition on `compose`) and the 7.1 wording are ratified now. The evidence labels in the 1.10 text take effect only if WU13a's step 1 checks pass (plan 0021 section 4).
3. **O3.** `antiproject` goes in `dual.hpp`, beside `project`.
4. **O4.** `compose` rejects an operand of mixed parity.
5. **O5.** The rounding check covers IEEE binary types rounding to nearest; float and double are tested; `long double` is neither tested nor claimed; other inexact scalars are not checked, and the comment and 4.5 say so (item 2(b)).
6. **O6.** Decision 0010's bound is kept for version 1, documented in the comment, characterized by WU13b (plan 0021 WU13b criterion 5), and revisited with the cga3 model (M6).
7. **O7.** A dropped part that is not finite fails the check.
8. **O8.** One merge into `develop`, after both WU13a and WU13b are reviewed.
9. **Departure 7.** WU13a pins 5.4, 5.5, and 5.6 with core tests on layouts written in the test and sets their trace rows to `yes`.

## Consequences

- The 2.1 sentence is given above. The 4.5 sentence is written by WU13a's implementer in step 2 with the content of item 5, and the commit and the review request list it.
- Decision 0176, which plan 0021 reserved for these rulings, records instead the 1.10 text as committed and the state of item 2(c)'s condition.
- Plan 0021's status, branches, and section 8 record these rulings.
