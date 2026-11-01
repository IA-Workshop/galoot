# 0070 Golden-value header: format, generation, and regeneration check

- Status: accepted
- Date: 2026-10-31
- Decided by: Claude (agent, Opus 5.5), in unit M0 closure (plan 0006). The owner may reverse it (C18).

## Context

Plan 0001 (M0 exit evidence; DoD-11) asks for a committed C++ header of golden values emitted by the reference scripts in `test/reference/`, and a CTest that regenerates it and fails on any difference. Note 0009 left the format open (its open question 1). The values must be derived from the definitions by a route that shares no code with the library (V1 kind 3). STYLE S11 as tightened on 2026-10-31 (decision 0034) forbids the header and its generator to cite CONVENTIONS by number or section, or any agent record.

## Options weighed

1. Format. (a) A plain-text table, as `clifford_reference.py table` prints, parsed by each C++ test at run time. (b) A JSON file. (c) A C++ header of `inline constexpr` aggregates. (a) and (b) need a parser in the tests, and their values could not be used in `static_assert`. (c) needs nothing beyond the standard library and works at compile time and at run time. Chosen: (c).
2. Organization. (a) One namespace per CONVENTIONS item (`item_3_1`). (b) One namespace per fact, named in words (`complements`, `grade_signs`). (a) writes item numbers into code, which S11 now forbids. Chosen: (b). The mapping from items to namespaces lives in `docs/agent/records/convention-trace.md` (decision 0071).
3. Layout and the format check. (a) Emit text that clang-format leaves unchanged, which ties the generator to one clang-format version and bin-packs the tables. (b) Run clang-format inside the generator, which makes the regeneration check depend on the installed formatter. (c) Emit one entry per line between `// clang-format off` and `// clang-format on`, with every line within 80 columns. Chosen: (c); the comment that opens the header says why the formatter is off.
4. Regeneration check. (a) Write the header in the build tree and compare with `cmake -E compare_files`, which gives no diff on failure. (b) `golden.py --check FILE`, which regenerates in memory and prints a unified diff. Chosen: (b).
5. Scope of the values, now. The items plan 0006 names: 1.1 to 1.7, 2.1, and 3.1. Per-blade tables for n = 4 and n = 5 (the sizes of rigid(3) and conformal(3), and an even and an odd n, which the left complement tells apart); signature tables for n up to 6, matching the range WU2 and WU3 test exhaustively; for 1.1's non-diagonal clause, the nine products of e4, e5, and e45 in conformal(3), not the full 1024-pair table, which has no present user (C12).

## Decision

`test/reference/golden.py` writes `test/reference/golden.hpp`: namespace `golden`, shared aggregate types at its top, and one namespace per fact holding `inline constexpr` arrays. It imports `clifford_reference.py` and `conventions_check.py` from its own directory and computes each value from the definition, compares it with the closed formula or table where the specification gives one (for example the reverse sign from the product of reversed generators against (−1)^(k(k−1)/2), and the n = 4 complements against `conventions_check.py`'s table), validates the conformal(3) product table with `clifford_reference.validate`, and stops with exit 1 if two routes disagree. `reference.golden` runs `golden.py --check`. `reference.golden_rejects` (`test/tools/golden_check_test.py`) shows the check failing on a copy with one sign flipped and on a copy with one blank line added. An INTERFACE target `galoot_golden` with a verified header set compiles the header on its own under the presets' warning flags; a test that uses the values links it and includes `<reference/golden.hpp>`. It does not link `galoot::core`.

## Consequences

- A change to either reference script that changes a value fails `reference.golden` until the header is regenerated and committed, so a value cannot drift silently. A hand edit fails the same way.
- Tests of WU2 and WU3 can switch from values written in the test to the golden arrays; when they do, the trace check requires their markers to state the fact (decision 0071).
- New golden values are added to `golden.py` with a row in the trace table in the same change.
- Not verified here: that the library agrees with the values. No library header implements these conventions on `develop` yet.
