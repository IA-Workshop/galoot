# 0070 The seed campaign finds the limits of a test's own arithmetic, which seed 0 does not reach

- Date: 2026-11-10
- Unit: WU14 (plan 0029), found by the 50-seed campaign; recorded from the M5 gate audit (finding m14)

## What happened

Every preset runs the randomized identity programs at seed 0 only. A campaign over seeds 1 to 50 of the same programs found that `core_transform_identities_test` aborted on seeds 8, 23, 27, 37, 38, 39, and 43 (7 of 50) with "rational: a result does not fit std::int64_t". The cause was the test's exact rational type, not the library. With hyperplane entries drawn from -2 to 2, the outermorphism check at grade 4 compares polynomials of degree 24 in those entries, and a value can need 76 bits. Seed 0 needs 62 bits, so the ordinary suite never saw it. Over seeds 0 to 2000, 400 of 2001 needed more than 63 bits. The test author bounded the hyperplane draws to -1 to 1 (at most 56 bits over the same 2001 seeds), added the seven seeds as a case, and all 50 seeds then passed. A mutant that had looked killed by this suite, `squares_doubled`, had in fact been "killed" by the overflow, so its verdict changed when the cause was found (decision 0217's correction).

## What to do

- Run the seed campaign (seeds 1 to 50, or a wider range) for every randomized program before its unit merges and at each gate. It is the only run that varies the draws, so it is the detector of limits that a fixed seed does not reach.
- When a campaign run aborts, read the message before blaming the library. Check whether a test helper's integer or rational type overflowed, and measure the bits each seed needs (the probe in `records/mutations/0029-identities/seeds/bounded/probe/` does this with a wider type).
- Bound a random draw so the whole range of seeds fits the helper's arithmetic with margin, and keep the failing seeds as a named case so the bound has a test.
- Do not count a mutant as killed by a suite that fails on a seed for an unrelated reason. Judge it again once the failure is fixed.
