# 0074 A record that one unit hands to another needs a named owner

- Date: 2026-11-15
- Unit: plan 0042 (release), found by the release audit (MA1)

## What happened

DoD-3 asks for a record that maps every operation to the generator. Plan 0011 handed that record to WU14 and WU17. Neither made it, and the release plan reported its "row for every requirement" criterion as met. The release audit found DoD-3 with no record, DoD-8 and DoD-13 with no evidence statement, and DoD-13 departed from by rulings. U11 wrote the DoD table and the operation map at the end, from decisions.

## Cause

The record belonged to no unit's definition of done. A plan said who would write it "later", and no later plan listed it. The V10 table was built from the evidence that existed, so an absent record showed up as no row, not as a gap.

## What now detects it

`records/release/dod-1-to-29.md` lists every DoD criterion with its evidence and status, including "gap". A plan that hands a record to another unit adds it to that unit's deliverables in the same commit, and the milestone audit reads the roadmap's DoD list against the V10 table criterion by criterion, so a missing row is a finding.
