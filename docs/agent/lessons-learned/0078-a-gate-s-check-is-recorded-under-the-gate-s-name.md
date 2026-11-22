# 0078 A check that a gate runs is recorded under the gate's name, or the history does not keep its row

- Date: 2026-11-21
- Unit: plan 0045 (release 1.0.1), units R6 to R8

## What happened

The release matrix recorded eleven of its twelve side checks as `gate:<check>@<revision>` and one as `conventions_check@578aa15`. The compaction that followed kept the eleven and removed the twelfth from the working log, with six older rows named the same way, among them the 1.0.0 matrix's. PERFORMANCE P13 says such rows stay. The rows are in git and counted in the history's groups, so nothing was lost, but a reader checking the matrix against the working log finds one check missing.

## Cause

`tools/runlog` decides what stays from a selection's family, the text before the first `:`. A selection with no `:` is its own family and matches no gate. P13 describes the rule by the checks' names, not by the tool's rule, and no brief said how a gate's checks are named.

## What now detects it

A brief for a gate or a matrix names every check `gate:<check>@<revision>`. Before a compaction, the unit lists the rows of step `check` and `trace` at committed revisions whose selection has no `:`, and reports them before the tool runs. Whether P13's text or the tool changes is put to the owner (decision 0264 section 1 item 2).
