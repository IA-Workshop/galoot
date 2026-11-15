# 0094 Owner acceptance of the M0 and M1 gate, with the gate's thirteen decisions

- Status: accepted
- Date: 2026-11-01
- Recorded by: Claude (agent, Sonnet 5.5), on the owner's instruction "Accept all as recommended, proceed", given on 2026-11-01 in answer to the gate report `records/gates/2026-10-31-M0-M1.md`.

## Decision

The owner accepted every recommendation in the gate report's list of decisions. In effect:

1. M0 is accepted. Departure 1 stands as stated: M1 work began before the M0 gate closed.
2. M1 is closed without WU5. WU5 (exact rational) moves to M5, beside the change of basis, its first user. This amends plan 0001 sections 6 and 8; plan 0001 itself is not rewritten (see the dated section appended to it).
3. A delta review is required after any fix pass on core code; it is not required for records and tooling. The delta review of the existing fix passes is done (gate report addendum, plan 0009).
4. W2 and W9 are amended as recommended and ratified. The commits listed in the gate report's departure 3 are ratified as exceptions.
5. The snippets review is applied: `runs_tsv_merge.py` deleted, the `golden.py` and `test_references.py` index entries removed, `spike-a/measure.py` marked superseded, `count_evals.py` indexed, the workbench spike copies cleared. The scratchpad convention checks are dropped; `test/reference/conventions_check.py` covers them.
6. The M1 core interface decisions (0060 as amended by 0035, 0036, 0061, 0062; 0090 to 0093) are accepted, including the nod on 0035 item 1. At default limits Clang fails `full<14>` and GCC fails `full<16>`; later units do not rely on `full<N>` above n = 13.
7. The P1 baseline for the M1 code is the instruction count in shared mode, taken by decision 0080's copy step on fresh trees at the gate revision.
8. Governance text: P11 says 23 slots, W11 points at plan 0001, WORKFLOW, VVUQ, and STYLE carry a status line, and the design study notes that plan 0001 supersedes its phases 1 to 4.
9. Past in-place edits to decision records are accepted. From now on a change to a decision is a new record or a dated appended section.
10. The vendored doctest is trimmed to `doctest.h`, `doctest.cpp`, and `LICENSE.txt`; their SHA-256 sums are in decision 0026's appended section.
11. A root `LICENSE` (MIT, copyright holder Brian J. McDermott) is added, and the design study's three process citations are removed.
12. Lessons for the lost slot and the other items of the gate report's departure 8 are recorded. The build-lane change that compiles compile-fail sources directly and in parallel is approved as a unit of its own.
13. `master` at the first governance commit is intended.

## Consequences

The governance edits in item 8 and item 4 take effect with this record (C18). Work on M2 may begin once the build-lane unit is planned.
