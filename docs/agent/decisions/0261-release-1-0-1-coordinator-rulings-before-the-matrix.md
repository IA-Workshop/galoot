# 0261 Release 1.0.1: the coordinator's rulings before the matrix

Date: 2026-11-21. Source: the hand-backs of plan 0045's unit R5, its blind review, and gate G2. Each is a reading within the plan and the owner's rulings (decision 0252); the owner can reverse any of them.

1. **The diagnostics record is regenerated in full.** Unit R5 stopped when regenerating `records/diagnostics/catalogue/scan-output.md` changed more than line places: the table gains the message "GALOOT_FORCE_INLINE must be 0 or 1", which three tests pin. The message has been in the headers since 2026-11-11 and is in 1.0.0; the record, written at `ebe7336`, was never regenerated. Ruled: regenerate it as a records commit before the release commit, with its header and the result line of `misuse-catalogue.md` brought in step (183 messages, 161 pinned, 22 internal). The 1.0.1 release record notes that 1.0.0's catalogue lacked one message.

2. **No limits text is added in 1.0.1.** Item 2 of decision 0259 section 3 was put to the owner with a stated default and was not answered before the release commit. The default holds: the six-generator Clang failure with its `-fbracket-depth` remedy, and the higher step counts of optimized Clang builds, go to plan 0049's limits page. The item stays open for the owner to rule otherwise, at the cost of a new release commit and matrix.

3. **The documentation-tooling branch stays open during the release (gate G2, finding F1).** Plan 0045 asks that no open branch hold run-log rows that `main` lacks when the log is compacted. `feature/docs--tooling` (plan 0049 unit U2) holds such rows, because decision 0253 lets U2 start once plans 0043 and 0044 are in `main`. Ruled: the branch is not merged into `main`, or into its feature branch, until 1.0.1 is tagged and `main` is at T. Its rows are new rows written after the release branch was cut; the compaction removes none of them, and the branch does not change the rows the compaction removes, so its later merge brings no removed row back. At that merge `main` is merged into the branch first, and `tools/runlog check` must report no removed row present. The check of G2 is otherwise passed; it is run again in R7 before the compaction, with this branch the one allowed exception.

4. **The changelog's date.** The 1.0.1 entry carries the date of the release commit, 2026-11-21, whatever day the tag is made. The release record says so.

5. **The blind review's minor findings on the release commit are fixed before the matrix**, since the matrix has not run and a fix now costs no rerun: the statement about which Clang builds became faster is narrowed to the two measurement builds, a count that cannot be reproduced is dropped, "a program that compiled with 1.0.0 compiles with 1.0.1" is limited to programs that use only public names, the README's copy-route sentence is reworded and points to the changelog, and the regenerated record's header says the message predates 1.0.1. The fix gets a delta review; the last of these commits that changes anything outside `docs/agent/` becomes R.

6. **Inputs the plan's unit rows do not name** (gate G2, advisory A1). R6 samples the two cost questions earlier gates left to it, and says which tests the sanitizer and measurement presets leave out. R7 takes its friction lines from section 14 of plan 0043 and section 15 of plan 0044.

Status: in force.
