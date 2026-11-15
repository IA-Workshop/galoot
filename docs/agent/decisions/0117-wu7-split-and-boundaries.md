# 0117 WU7 runs as two parts, and its boundaries with WU9 and WU10

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), implementing WU7a of plan 0013. The owner said to proceed with the plan's defaults for its owner questions; this record holds questions 1 and 2 (departures 1 to 3). Plan 0013 reserves decisions 0110 to 0119 and assigns 0110 to 0116; this is the first of the unassigned numbers.

## Context

Plan 0001 lists WU7 as one unit of size M: result typing, unary maps, complement conjugation, memoization. Plan 0001 places "gp, wedge, `*`, `^`" in WU9 and "involutions, complements, antiproducts; De Morgan laws" in WU10. STYLE S16 admits the operation parameter only with a second operation present, and the texts of WU7 and WU10 overlap on complements and conjugation.

## Decision

1. Two parts, run one after the other, each with its own W5 review and merge (owner question 1, plan default): WU7a on `feature/core-plan-results` (typed binary plans for the geometric product and the wedge, the evaluation check), then WU7b on `feature/core-plan-maps` (unary maps, the antiproducts as complement-conjugated plans, the plan-level De Morgan and involution proofs), branched from `develop` after WU7a merges.
2. WU9 boundary (plan departure 2): WU7a adds the wedge as a plan in `detail`; WU9 keeps the public `gp` and `wedge` on multivectors and the operators `*` and `^`. The 4.1 trace row of the wedge moves to `yes` in WU7a, which implements CONVENTIONS 4.1 in `include/galoot/core/`.
3. WU10 boundary (owner question 2, plan default, departure 3 as written): WU7b builds the plan-level maps and conjugated plans in `detail` and proves the De Morgan laws and the involution relations on that data exhaustively over finite ranges; the 4.2 trace row moves to `yes` in WU7b. WU10 provides the public operations on multivectors (`reverse`, `right_complement`, `antigp`, `antiwedge`, and the rest), the operators `~`, `!`, `%`, `&`, and the randomized multivector identities of DoD-10.
4. Memoization (departure 4): no new mechanism. Every new variable template follows the form of decisions 0102 item 7 and 0103, and the CTest is decision 0112.

## Consequences

- WU7b is claimed by a dated line in plan 0013 when WU7a has merged.
- Plan 0001's WU9 and WU10 lines are read with these boundaries; the roadmap text itself is not edited here.
