# 0215: M4 accepted, with the owner's rulings on the gate findings

Date: 2026-11-08. Status: recorded. Source: the owner's reply "M4 accepted, please proceed. Accept all others and proceed" to the gate package (the M4 audit `records/gates/2026-11-08-M4-audit.md`, the audit of 9d82f30 to a75ce5e `records/gates/2026-11-08-wu6-wu8-audit.md`, the review of the three unreviewed merges, and the snippets review `snippets/reviews/M4.md`).

## Rulings

1. M4 (the `rga3` algebra as data: WU13, WU15, WU16, with the rotation-normalize and presets units) is accepted. The auditor's major findings MA2 and MA3 are closed; MA1 is ruled in item 2.
2. MA1: the wording edit to CONVENTIONS 7.2 ("by its canonical name (1.1; the scalar is 1, 1.5)", commit `5928dfa`) is ratified as worded.
3. The sufficiency of the V8 cases is accepted for M4, with these limits stated: the 32 point-group orders are checked against Hestenes' Table 4 (a primary source in the geometric algebra literature) and the groups were rebuilt from his Table 2 symbols and matched in order and in the (det, trace) multisets; the group elements agree with his only up to a choice of axes; the abstract-group and point-symmetry columns rest on Wikipedia plus a computation; the joint chains have no primary source; the International Tables and the Denavit-Hartenberg paper were not read.
4. E1 (the early audit): the merge messages of the WU6 to WU8 window that lack the P13 line are a past departure, with no change to P13.
5. E2: the merges `68e8ba6` (WU6a), `c972952` (WU6b) and `fc964a8` (plan 0012) without the delta review of decision 0094 item 3 are past departures, with the inferred mitigation that later reviews read those headers again. No delta review of `4e5ea90` is made.
6. m4: the brief tells a reviewer to read the author's outcome only after its own checks; no amendment.
7. Records-only and research-note merges into `develop` need the P9 checks their files can affect but no independent review. WORKFLOW W5 carries the sentence.
8. W2: no amendment. Commits that touch tests run `tests.*` and lint before they are committed (P8's reading).
9. Agent cost: the coordinator records each subagent's cost in `agent-cost.tsv` when it hands back, writing "not visible" where tokens are unknown.
10. The evidence-tools unit (plan 0026) is accepted as planned: its six questions are answered as the plan recommends (TOML spec; `tools/mutate` and `tools/compiles`; port the `0023-rga3` and `0011` sets only; a library `static_assert` counts as a kill and a warning or invalid failure does not; flags from the build trees; `catalogue/scan.py` stays until WU14). The evaluation-counter snippet stays for WU17. The pool and runlog repairs the three-merge review recommended (an automatic top-up of lost slots as decision 0130 item 7 accepted, `prune` refusing to delete unless the history matches exactly, and documenting the `runlog` test label) are a separate small unit.
11. The remaining uses of "model" in CONVENTIONS and of "preset" for a CMake configuration in `tools/`, the agent files and core `assert.hpp` wait for the next governance pass and the next core unit.

## Next

The evidence-tools unit (plan 0026), the pool and runlog repairs, then WU14, WU17 and M5 (WU12). M6 ships after v1 (decision 0211).
