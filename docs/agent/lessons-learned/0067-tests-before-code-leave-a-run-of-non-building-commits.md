# 0067 Tests written before the code by another agent leave a long run of non-building commits

- Date: 2026-11-02
- Unit: WU10a and WU10b (plan 0019)

## What happened

A test author wrote the tests from the plan and the references, then a separate implementer made them pass. This kept the tests independent of the code (the test author's own mutants and the later reviews found no oracle taken from the implementation), but the test commits cannot build before the code exists, so WU10a had five and WU10b fifteen commits in a row that do not build. W2 allows a commit that fails by design only when its message says so, and both reviews flagged the run.

## What to do

- Keep the separation: it is the reason the tests can be trusted.
- Declare the run in the plan's departures before it happens, and list the range in the review request.
- A stand-in header in the scratchpad lets the test author prove the tests are well formed and kill scratch mutants, but that evidence is not reproducible as committed, so the implementer re-runs the mutants on the real code.
- The owner has not yet ruled whether W2 may cover a run of test commits; until then each such run is a declared departure.
