# Blind review: WU11a of plan 0020, the bulk, the weight, the duals, the dot, the antidot, and the projection

- Reviewer: the blind-reviewer role, general-purpose agent, Claude Opus 5.5, briefed by the coordinator (W5), with the usability checklist since the change adds a public header.
- Range: `c86cb58..d831225` on `feature/core-duals`.
- Outcome: **approve with non-blocking findings.**
- Source of this text: written by the implementer from the coordinator's summary of the review; the reviewer's own report is held by the coordinator and was not passed on. Where the summary gave a finding's wording or values, they are used as given.

## What the reviewer checked

- An independent exact Clifford model of its own on 14 forms, among them rigid(3), conformal(2), conformal(3), the degenerate form that is not diagonal, and random dense forms.
- The library against that model: 8,064 values, built with GCC at `-O1` and Clang at `-O2`, with no mismatch.
- The tests' hand values, recomputed.
- Nine mutants of its own: six killed, three survived (findings F1 to F3).
- The assembly counts, recounted from the committed `.s` files (F5).
- The diagnostics, read.
- A trial merge with `develop`: the only conflict is in plan 0020.

## Findings

| Finding | As relayed | Resolution |
| --- | --- | --- |
| F1 | `project`'s antiwedge order is not pinned: the mutant `antiwedge(wedge(a, weight_dual(b)), b)` survives. Add a hand value for a line onto a plane, or check `project` over every blade pair of rigid(3). | `9bd09dd`: the line through (1, 0, 0) with direction (1, 2, 3) projected onto z = 2 is the line through (1, 0, 2) and (2, 2, 2), direction (1, 2, 0) and moment (−4, 2, 2), checked against an exterior-algebra model written apart from the library; the mutant gives the negation. The mutant is killed on gcc-debug and clang-debug. |
| F2 | A `kernel_held_exactly` that accepts when any kept coefficient fits survives: add compile-fail tests for the dot and the antidot whose kernels mix a held and a non-held coefficient (values on {e1, e2} in diag(200, 1) over `std::int8_t`), and an accept case. | `9bd09dd`: `core.dual_narrow_scalar_{dot,antidot}_mixed_kernel` and an accept test on e2 and e1; the mutant is killed by both compile-fail tests on both compilers. |
| F3 | Which operand `project`'s narrow-scalar check reads is not pinned: add a test with different layouts (a on {e12}, b on {1} in diag(100, 100) over `std::int8_t` must give project's message; a on {1}, b on {e12} is accepted). | `9bd09dd`: `core.dual_narrow_scalar_project_first_operand` and the accept test (a b² = 12 on the scalar); the mutant that checks the first operand is killed on both compilers. |
| F4, for the owner | CONVENTIONS 3.2's label "(Confirmed, with the diagonal case Checked)" overstates: `conventions_check.py` checks only rigid(3)'s mask filters. Relabel, or extend the script. | Not changed: the label is ratified text. Recorded in plan 0020's outcome as a question for the owner. |
| F5 | The instruction names in `records/performance/duals-cost/README.md` are wrong: Clang's dot and antidot use 2 `mulpd` each; GCC's projection uses 11 `mulsd` and 5 `mulpd`, 2 `subsd` and 3 `subpd`, and 3 `addsd`. The multiplication counts of 4 and 21 and "no call" stand. | Recounted from the committed `.s` files and corrected. |
| F6 | `dual.hpp` lines 63 and 64: the sentence on when the dot holds the scalar is wrong; reuse `dot_layout`'s wording. | `f4d85f8`. |
| F7 | `dual_test.cpp` lines 1291 and 1292: the sign of e123 reverse(e124) is +10000 e34. | `f4561f5`. |
| F8 (information) | `double d = dot(p, q)` and `get<"1">(dot(point, line))` give generic diagnostics. | None. |
| F9 | Add a lesson: run `tests.reference_markers` whenever a test marker is added. | Lesson 0069. |
| F10 | Merge notes. | Followed at the merge of `develop` into the branch (plan 0020, outcome). |
