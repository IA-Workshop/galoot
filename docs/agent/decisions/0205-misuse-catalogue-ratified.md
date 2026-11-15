# 0205 The misuse catalogue of DoD-19, ratified, and its scan

- Status: recorded. Ratified by the owner: decision 0196 item 5 accepted plan 0022 O3 ("ratify the misuse catalogue ... once part 1's record exists"), and the coordinator confirmed on 2026-11-08 that the record now exists and that this counts as ratification. The scan reading the CTest listing (item 3) was accepted by the coordinator the same day.
- Date: 2026-11-08
- Recorded by: Claude (agent, Opus 5.5), implementer of WU15
- Bears on: `records/diagnostics/misuse-catalogue.md`, `records/diagnostics/catalogue/scan.py`; plan 0001 DoD-19 and DoD-1; plan 0022 R1, departures 2 and 3, O3, O9

## Context

DoD-19 asks for a ratified misuse catalogue covering nine items, each with a compile-fail test on both compilers that matches the literal message. Part 1 of plan 0022 wrote the catalogue at `90ff083`.

## Decision

1. **The catalogue is ratified as DoD-19's**, including:
   - three items met by the compiler's own text, by earlier design: the implicit conversion between layouts, a tag that is not an algebra, and narrowing in braces;
   - graded signatures, which have no spelling and are met by `metric_like`'s rejection.
2. **Rows added later follow the same rule** and are listed in the merge report, starting with part 3's catch-alls.
3. **The scan reads CTest's JSON listing of a configured tree, one per compiler, not `test/CMakeLists.txt`.**
   - Options weighed: parsing the CMake file, with `set()` variables and `${semicolon}` resolved, as R1 described; or reading the listing, where every variable, loop, and compiler branch is already resolved.
   - The listing was chosen. A parser would have to reimplement `foreach`, `if(CMAKE_CXX_COMPILER_ID ...)`, and function calls to be right.
   - The cost is that the scan needs configured trees.
4. **The scan is a record tool, not a CTest or a merge gate (O9).** It runs at this unit, at the M4 gate, and at the release audit.

## Consequences

- A message that no test pins must be given a test or listed with its reason in the catalogue.
- The owner may promote the scan to a gate if the catalogue drifts.
