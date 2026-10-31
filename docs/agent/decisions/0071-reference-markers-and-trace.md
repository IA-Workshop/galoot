# 0071 Reference markers on test cases, and convention traceability through an agent-side table

- Status: accepted
- Date: 2026-10-31
- Decided by: Claude (agent, Opus 5.5), in unit M0 closure (plan 0006), within the constraint the owner set on 2026-10-31 (STYLE S11 as tightened, decision 0034; relayed by the coordinator: "mark tests by the mathematical fact or definition they pin, in words a human reader understands ... design the traceability script to map from the facts' IDs kept in an agent-side table under docs/agent/"). The owner may reverse it (C18).

## Context

Plan 0006 criterion 3 asks for one fixed form of reference marker on every `TEST_CASE`, with regression guards labeled (V1, V2, S14, DoD-14), and criterion 2 for a script that traces convention IDs to tests and reports the IDs with no test (DoD-11). S11 now forbids code, including test names, comments, and generated files, to cite CONVENTIONS by number or section, a governance provision, or any agent record. So convention IDs cannot appear in test code, and the trace must come from a table outside it.

## Options weighed

Where the marker goes:

1. In the test case name, as a suffix. The names grow long and the reference is repeated in every failure line. Filters with `--test-case` would have to match it.
2. A doctest decorator such as `* doctest::description(...)`. Structured, but a reader sees it only in the source anyway, and the parse is no simpler.
3. A comment block directly above the `TEST_CASE`, beginning `// Reference:`. Plain to read, survives clang-format's comment reflow because continuation lines are allowed, and needs no doctest feature. Chosen.

What the marker says:

1. A convention ID. Forbidden by S11.
2. A kind of evidence from a closed list, then the fact in words: `proof`, `identity`, `independent value`, `comparison` (V1 kinds 1 to 4), `contract` (a behavior documented at its declaration, for tests of the build and tools, which have no mathematical referent), and `regression guard`. Chosen. The kind makes V1's ranking visible in the test, and the words give the reader the reason in place.

How traceability works:

1. Parse IDs from the marker. Forbidden.
2. Map each test by name in a table. The table breaks whenever a name changes, and nothing in the test tells the reader what it pins.
3. A table of facts: each row is a CONVENTIONS item, a fact of it in the words a marker states it, and the golden namespaces that hold its values. A test pins an item when its marker contains one of the item's facts, as written (whitespace collapsed). Chosen. The golden namespaces give a mechanical signal, independent of the wording, that a test case relies on an item's values.

## Decision

The marker is a comment block directly above each `TEST_CASE` (no blank line between) whose first line begins `// Reference:`; later comment lines continue it. Its text is a `;`-separated list of sources `<kind>: <text>` with a kind from the list above. A regression guard is the only source of its marker, and a test case whose name contains "regression" must carry one. The marker text must not match a pattern of record pointers: `decision|plan|note|review|lesson` followed by four digits, `research note`, `spike A` to `F`, `workbench`, `docs/agent`, `CONVENTIONS`, an agent model name, a bare provision token `[CWSPV]` plus one or two digits, or `DoD-N`.

`tools/test_references.py markers <paths>` checks the form over every `*_test.cpp` (the only files the build compiles into test programs), and fails if it finds no test case. `tools/test_references.py trace <table> <CONVENTIONS.md> <golden.hpp> <paths>` checks that every table item exists in CONVENTIONS.md, that the table's golden namespaces are exactly those of the header, and that each test case that uses a golden namespace states its fact; it prints each item with the test cases that pin it and the items with none. The table is `docs/agent/records/convention-trace.md`.

CTests: `tests.reference_markers` and `tests.convention_trace` on the real tree (labels `tests;tests.references`); `tools.test_references_rejects_markers` and `tools.test_references_rejects_trace` on fixtures in `test/tools/test_references/`, each requiring every expected finding (labels `tools;tools.test_references`).

## Consequences

- The two smoke test cases gained markers of kind `contract`. Their names still cite records; they are left for the cleanup unit of decision 0034.
- WU2 (bits) and WU3 (metric) are running in parallel and do not know this form. When either merges to `develop` after this unit, `tests.reference_markers` fails on its test cases until they carry markers, and `tests.convention_trace` fails if they use golden namespaces without stating the fact. Whichever unit merges second adds the markers. This is reported to the owner (W7: a fix to another active unit's files is reported, not made).
- The pointer pattern is deliberately coarse. `\b[CWSPV]\d{1,2}\b` would reject a point-group name such as C2 or C3 in a marker; the cleanup unit that extends the check to whole files (decision 0034) should refine it, for instance by requiring a governance document name before the token, when a point-group test arrives.
- `tests.convention_trace` passes paths under `docs/agent/` to the script. That is the build reading agent-side data, not citing it; the owner may rule otherwise, in which case the trace becomes a gate-time command outside CTest.
- A golden value used in a helper outside any test case is not seen by the trace check; the docstring says so.
- Not checked: that a marker's words are true of its test. Review checks that.
