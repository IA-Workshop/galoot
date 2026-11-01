# 0141 The antiproduct's growth at fixed use is a recorded exception to C6 for twelve months

- Status: accepted by the owner, 2026-11-01
- Date: 2026-11-01
- Decided by: the owner; recorded by Claude (agent, Sonnet 5.5)
- Bears on: decision 0116 (the growth, recorded as a defect), decision 0140 (the proposed remedy), decision 0005 (the reading of P2), CONSTITUTION C6

## Context

The geometric antiproduct and the antiwedge are built by conjugating the base product with the complements (decision 0115). For a fixed use embedded in a larger algebra, four rigid(3) pairs in R(n−1,0,1) with n from 4 to 16, the compile-time cost of the antiproduct grows by 265% on GCC and 464% on Clang, against 6% and 4% for the geometric product (research note 0018). The growth is an artifact of the construction, not of the algebra. Constitution C6 says cost follows use, so the growth is a departure from C6.

Decision 0140 proposes a remedy that keeps the one mechanism of C4: conjugate inside the generators the use is coupled to, plus one scalar and per-blade signs. It matches the full construction exactly over 113 forms and 168,764 pairs. It makes the generator harder to read than the direct transcription of CONVENTIONS 4.2, and for the models planned (n from 3 to 6) its gain is tens of milliseconds of compile time per translation unit. Runtime speed, compiler memory, and (as expected, not measured) release binaries are unchanged either way.

## Decision

1. The antiproduct keeps the conjugation of decision 0115. The growth is accepted as an exception to C6 for twelve months, to 2027-11-01.
2. The exception ends, and the fix of decision 0140 or another remedy is scheduled as its own unit, at the first of these:
   - a production report of a build-time problem from the antiproduct or antiwedge;
   - a model, named type, or use that needs an algebra with more than six generators while using few of them;
   - 2027-11-01, when the owner reviews this decision.
3. Decision 0140 stays proposed, not rejected. Its research note, check script, and prototype under `records/spikes/antiproduct-direct/` stay in the repository as the starting point.
4. Any unit that touches antiproduct generation records the cost at n = 4 and at the largest n it supports, so that the review of this exception has current figures.
5. Release records (W13) list this exception.

## Consequences

- Decision 0116's defect is closed as an accepted, time-limited exception, not as fixed.
- WU8 and later units build on the plan interface and do not depend on this choice.
- The certified-limits unit (decision 0125) keeps its model parametrized by weights so that a later change of antiproduct generation needs only a refit of those weights.
