# 0223: The owner's rulings on the WU12 reviews

Date: 2026-11-09. Status: recorded. Source: the owner's reply "Accept all recommendations" to the open items after the WU12 blind review, the hci reading and the Sage cross-check.

## Rulings

1. The hci reading's should-fix items S1 to S7 are applied in this unit, with the test commits for pinned literals first (W9): S1 one error for a wrong-D tag; S2 and S3 reworded messages (S2's advice was unsafe); S4 the header example; S5 the diagonal's symbol; S6 a catch-all for a value given in place of the tag's template argument, after a cpp-expert confirms the overload set; S7 a sentence naming `change_basis` in the `basis_change` comment. N6 of the hci reading waits for the M5 message pass.
2. The names stay (decision 0222 item 1), with S7's sentence. A README section on a user's own algebra and a change of basis is written at the M5 gate. No change-finding factory in v1; `witt_change` is considered after v1.
3. The blind review's N1: the tag checks compute a flag and reject it with one `static_assert`, so an overflow gives one error with the message first (about 20 lines and a changed catalogue row). N7's comment wording is applied with that header change.
4. N3, the meaning of `==` on `basis_change`: it stays as documented for now (it compares the stored rows), the comment says so, and the owner decides before release whether to rename it or compare matrices. N2 and N5 wait.
5. N6 (`tools/mutate` classing a Clang constant-evaluation guard as invalid, and a spec that expects a survival) goes to the backlog as a tools unit.
6. The GCC 16.2.1 wrong-code reproducer is characterized by a cpp-expert who also scans the library and test sources for the pattern; a canary test follows if the scan finds exposure.
7. The stale math-tools note on `lift_isometry` is updated by a math-tools agent.
8. Note 0026 gets a pointer to the Sage record; the dump program stays in `docs/agent/records/`.
9. CONVENTIONS 1.17 is committed as drafted in note 0026 section E at the merge, after the Sage record.
