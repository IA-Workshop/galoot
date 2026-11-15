# 0247 The bulk dual's growth at fixed use is a recorded exception to C6, beside decision 0141

- Status: written at the M7 gate as decision 0241 item 7 directs, from the owner's ruling in decision 0240 item 10 (recorded as an exception beside 0141, not fixed before 1.0). It carries 0141's end conditions and its review date unchanged.
- Date: 2026-11-11
- Decided by: the owner (decisions 0240 item 10 and 0241 item 7); recorded by Claude (agent, Sonnet 5.5)
- Bears on: decision 0141 (the antiproduct's exception), decision 0237 item 5 (the finding), decision 0172 (the dual operations as compositions), decision 0005 (the reading of P2), CONSTITUTION C6

## Context

Plan 0038's growth grid (part E3) measured the compile-time cost of fixed uses of the public operations in R(n−1, 0, 1) for n = 4, 8 and 16, with the rigid(3) layouts embedded as the grid's records describe (`records/performance/wu17-growth/README.md`, measured at `18849b5`, GCC 16.2.1 and Clang 23.1.4, pinned to CPU 4, three repeats, a second campaign agreeing within 0.8%). Each figure is retired instructions of the use, that is, its unit minus the baseline unit of the same n.

For `bulk_dual` of a point, a line and a plane the use costs:

| | n = 4 | n = 8 | n = 16 | Change, n = 4 to 16 | Larger range |
| --- | --- | --- | --- | --- | --- |
| GCC 16 | 451.0 M | 453.8 M | 459.2 M | +8.2 M (+1.8%) | 0.5 M |
| Clang 23 | 468.6 M | 479.8 M | 503.0 M | +34.4 M (+7.3%) | 3.0 M |

The growth is beyond the ranges and small. The baseline unit grows by 14.2 M (GCC) and 15.3 M (Clang) over the same sizes, and that growth belongs to the algebra's own signature checks, not to a use. The grid's other fixed uses of the same unit shape (wedge, geometric product, reverse) show none: their changes are within twice the larger range, at most 0.6%.

Decision 0237 item 5 recorded the finding as a defect found beside decision 0141 that 0141 does not cover, since 0141 names the antiproduct and the antiwedge. Constitution C6 says cost follows use, so growth with n at fixed use departs from it.

The mechanism follows the antiproduct's. `bulk_dual(x)` is `geometric_product(reverse(x), 𝟙)` (decision 0172), and 𝟙 is the antiscalar, a blade of grade n. The cause the grid's record gives, that the generation's work grows with the grade of that operand, is inferred from the pattern and not from a time report, as the record says. The grid did not measure the other duals, the weight duals, the antidot or the projections. `dual.hpp`'s comment already says the weight, the weight duals, the antidot and the projection pass through complements whose grades grow with n.

## Decision

1. `bulk_dual` keeps its composition of decision 0172. Its growth with n at fixed use is accepted as an exception to C6 for twelve months, to 2027-11-01, on the same terms as decision 0141.
2. The exception ends, and a remedy is scheduled as its own unit, at the first of these (the end conditions of decision 0141 item 2, read for `bulk_dual`):
   - a production report of a build-time problem from `bulk_dual`;
   - a model, named type, or use that needs an algebra with more than six generators while using few of them;
   - 2027-11-01, when the owner reviews this decision together with decision 0141.
3. No remedy is proposed or prepared. When the exception ends, the remedy is considered with decision 0140's, since both concern a product taken against a blade of grade n or its complement.
4. Any unit that touches `bulk_dual` or the generation it composes records the cost of the same fixed use at n = 4 and at the largest n it supports, so that the review has current figures. Figures for the weight duals, the antidot and the projections are not part of this exception; a unit that finds growth in one of them records it as a defect beside this decision.
5. Release records (W13) list this exception beside the antiproduct's.

## Consequences

- Decision 0237 item 5's defect is closed as an accepted, time-limited exception, not as fixed.
- The two exceptions share one review date, 2027-11-01, and one set of triggers.
- The growth grid was measured at `18849b5`, before the forced inlining of decision 0244. The kernels' inlining does not change the plan generation that the growth comes from (inferred; the grid was not rerun, since decision 0246 item 2 defers the reruns past 1.0). The release record states the date of the grid it cites.
