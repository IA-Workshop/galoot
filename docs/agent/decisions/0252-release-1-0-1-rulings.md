# 0252 Rulings for the 1.0.1 work (plans 0043, 0044 and 0045)

Date: 2026-11-21. Source: the owner's message of 2026-11-21, "Accept all recommendation and proceed".

Reading followed. The coordinator's reports of 2026-11-21 put the 1.0.1 items to the owner as one item ("the 1.0.1 combined rulings (plans 0043 to 0045); I recommend accepting all"), and the report the owner answered recommends the order "1.0.1 (plans 0043 to 0045), then plan 0047's safety net". The owner's acceptance is read as covering each item below at the option its plan recommends. An item the owner meant otherwise is superseded by a later decision (lesson 0073). The options are quoted in short; the full text is in each plan's owner section (0043 section 11, 0044 section 13, 0045 section 10).

## Plan 0043 (test, build and tool cost)

1. **O1, the rounding unit.** Options: (a) one function per law, in about six sources; (b) split by section only; (c) turn the forced inlining off for this unit on the Clang optimized presets. Ruled: (a).
2. **O2, the gcc-debug peak.** Options: (a) split `transform_identities_test.cpp` only; (b) also split `identities_four_test.cpp` into two group sources, as an addendum to U2. Ruled: (b).
3. **O3, users' cost.** Options: a default of `GALOOT_FORCE_INLINE` that follows the optimization level, or no change. Ruled: no change in 1.0.1; considered for 1.1.0 with a measurement (plan 0047 V7b supplies it).
4. **O4, the dirty-mark gap.** Options: (a) fix the read-at-the-end gap and pin the staged case with a test; (b) rescope from the owner's account of the observation. Ruled: (a).
5. **O5, records-only closures.** Options: accept or reopen each of (a) m6 recorded as unexplained, (b) the CTest label anomaly closed as "no record and no reproduction", (c) the known-limits table. Ruled: all three accepted.

## Plan 0044 (header and interface hygiene)

6. **O1, the cache option's spellings.** Options: (a) `ON` or `OFF` only; (b) also `0` and `1`; (c) any CMake boolean constant. Ruled: (a).
7. **O2, the README's copy route.** Options: (a) wording: copy `include/galoot`; (b) move doctest out of `include/`, a governance and build change. Ruled: (a) in 1.0.1.
8. **O3, the deleted `==`'s hint.** Options: (a) the guide note and its rejected block; (b) also `= delete("...")` behind a feature test, a departure from STYLE S2; (c) nothing. Ruled: (a).
9. **O4, "model" and "preset" in the governance and agent files.** Options: (a) leave to the next governance pass; (b) prepare the owner text now; (c) keep "model" as the ratified term. Ruled: (a); the agent files and tools are left alone.
10. **O5, the reading of plan 0044 section 1 item 5** (one feature branch, P8 in one configuration per unit, one P9 matrix at the merge). Ruled: as read.

## Plan 0045 (release procedure)

11. **O1, the branch name in the governance.** Options: (a) replace `develop` by `main` in the seventeen governance occurrences, ratified as one text, with the twelve agent-file occurrences in the same commit; (b) one ratified sentence in W7; (c) rename `main` back to `develop`. Ruled: (a). The text is the word-for-word replacement of plan 0045 section 6 and nothing else; this item is its ratification. History under `docs/agent/` keeps `develop`.
12. **O2, `master` and the remote.** Options: (a) `master` stays the release branch and the owner pushes `master` and the tag `1.0.0` now, then `master` and `1.0.1` at the close-out; (b) retire `master`; (c) keep `master` local only. Ruled: (a). The push is the owner's action.
13. **O3, the repository URL in the README.** Ruled: `https://github.com/IA-Workshop/galoot.git` is filled in the release commit. This closes decision 0249 item 1.
14. **O4, `tools/runlog compact`.** Options: (a) a compaction at the 1.0.1 matrix's records tip, in R7; (b) one at `a4cd99d` before the matrix; (c) decline. Ruled: (a). This is the owner's approval P13 asks for, for that one compaction.
15. **O5, release notes.** Options: (a) the annotated tag message only; (b) a root `CHANGELOG.md` kept by each release commit; (c) as (a) or (b), with `GIT_SHALLOW TRUE` in the README's `FetchContent` block. Ruled: (b), with `GIT_SHALLOW TRUE`.
16. **O6, the matrix's shape.** Options: (a) all eleven presets, one at a time; (b) all eleven with the clang-measure builds started first; (c) the nine gate presets only. Ruled: (a).
17. **O7, audits.** Options: (a) one release audit after the matrix, with gate G2 before it; (b) two audits as at 1.0.0. Ruled: (a).
18. **O8, the walkthrough.** Options: (a) light, about 15 minutes; (b) full, as plan 0042 section 8. Ruled: (a).
19. **O9, DoD-22's compile rows at R.** Options: (a) copy the debug trees' rows into `compile.tsv`; (b) skip. Ruled: (a).
20. **O10, the version number.** Options: (a) hold any added public name or user-set option to 1.1.0; (b) release 1.1.0 instead. Ruled: (a).

## Consequences

- R0 and R1 of plan 0045 are done by the merges of 2026-11-21 and this decision. R2 applies item 11.
- The owner said on 2026-11-16 that no hosted CI is used; nothing here proposes one.
- Still the owner's alone: the push of item 12, the walkthrough, the merge into `master` and the tag (R11).

Status: in force.
