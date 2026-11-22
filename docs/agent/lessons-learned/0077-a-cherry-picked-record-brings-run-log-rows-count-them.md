# 0077 A cherry-picked review record can bring more run-log rows than its own: count them

- Date: 2026-11-21
- Unit: plans 0043 and 0044 (merge gates), plan 0045 (unit R7)

## What happened

At plan 0043's merge, cherry-picking a review record onto `main` added 419 run-log rows, not the record's 12: the review branch's log also held the 407 rows of the feature branch it had reviewed, and the union merge of the log kept every line. At plan 0044's merge the same step would have duplicated rows and was resolved by hand. At the release, a review branch whose records had not been cherry-picked stopped the run-log compaction, because it held rows the release branch lacked.

## Cause

`runs.tsv` merges by union, so a cherry-pick whose parent's log differs from the target's brings the difference along. Review branches are cut from a feature branch and picked onto another branch, and no step counted what arrived.

## What now detects it

Whoever cherry-picks a record that changes the run log counts the log's lines before and after, compares the difference with the rows the commit itself adds (`git show <commit> -- <log> | grep -c '^+[^+]'`), and runs `tools/runlog check`. A review record is picked onto the branch it reviewed before that branch merges, so the rows the two share arrive once. Before a compaction, the check of open branches lists every branch that holds rows the boundary lacks; review branches are picked or excepted by a recorded reading first.
