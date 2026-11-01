# 0082 The trace gate checks every item and every row, and the golden checks read test headers

- Status: accepted
- Date: 2026-10-31
- Decided by: Claude (agent, Opus 5.5), in cleanup unit 1 (plan 0007, item 4), carrying out findings D2, D6, and D7 of the M0 scripts delta review (`docs/agent/records/reviews/2026-10-31-m0-scripts-delta.md`). The owner may reverse it (C18). Amends item 3 of decision 0072 and resolves the gap its amendment names (D6).

## Context

D2: the trace gate passed vacuously for the 20 CONVENTIONS items that had no table row, so none of them could ever be marked implemented; a table line with an extra `|` was skipped without a finding; and Implemented was set per row but evaluated per item, so a `yes` row passed on a test that stated another row's fact. D6: `golden` and `trace` read only `*_test.cpp`, so a helper header under `test/` that used golden values was never scanned. D7: ruff B023 on the alias closure.

## Options weighed

For items without rows: add rows for all of them now, all `no`, or exempt items that are not implemented. Exempting needs a second notion of "implemented" outside the table, which is the column's job. Chosen: every item has a row, and the 20 rows were written from the specification's text, each with one fact a later test can state.

For malformed rows: a line is a row when its first cell is an item number; one with other than three cells, or no closing `|`, is a finding. A fact may not contain `|` (no escape is supported; no fact needs one).

For which files the golden checks read: `*_test.cpp` and headers (`*.hpp`, `*.h`) under the directories given. The marker check still reads only `*_test.cpp`, the files compiled into test programs. Fixture headers under `test/tools/test_references/` are read too; their namespaces are not the real golden header's, so they add nothing to the real-tree check.

## Decision

`trace` fails when an item of the specification has no row, when a line that starts a row is not three cells, and when a row marked `yes` has no test stating that row's fact. It still prints, per item, every test that states any of the item's facts. `golden` and `trace` read headers. The alias closure is a function of its arguments.

## Evidence

Fixtures: `trace_conventions.md` gains item 4.1, which `trace_bad_table.md` leaves without a row and `trace_good_table.md` gives one (so its accepting output becomes "items with no test: 3.1, 4.1", the only changed expected value, changed for this reason); `trace_bad_table.md` gains a second, implemented fact of 1.1 that no test states and a row with an extra cell; `helper_headers/helper.hpp` reads golden values outside any test case. Against the script before the change, `tools.test_references_trace_rejects` lacks all three new findings and `tools.test_references_golden_helper_header` lacks its one. The gate command on the real tree exits 0 with the 30 rows.

## Consequences

The trace table must gain a row in the same change as any new CONVENTIONS item. Facts written now for unimplemented items are a first wording; the unit that implements an item may change its fact together with the markers that state it.
