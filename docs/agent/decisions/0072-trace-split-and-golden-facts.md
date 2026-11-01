# 0072 Trace split: build checks read only test/, the item mapping is a gate command

- Status: accepted; ratified by the owner on 2026-10-31 ("proceed as recommended"), together with the W2 departures at `7025d83` and `42f0c6e` and the edit lifting the complement table in `conventions_check.py`; supersedes the traceability part of decision 0071 (its CTest `tests.convention_trace`, the Golden values column, and the pointer pattern). The marker form of 0071 stands.
- Date: 2026-10-31
- Decided by: Claude (agent, Opus 5.5), in unit M0 closure (plan 0006), carrying out the W5 review `docs/agent/records/reviews/2026-10-31-m0-scripts.md` (findings B1, N1, N2, N4, N6) as relayed by the coordinator. The owner may reverse it (C18).

## Context

Review finding B1: `tests.convention_trace` named `docs/agent/` files in a build file, so clearing or moving agent records would fail `ctest` for a human contributor, against STYLE S11. N1: the trace could not fail on an implemented item with no test. N2: the pointer pattern missed several pointer forms and rejected mathematics (C3, S2, V4). N4: aliases and `using` escaped the use check, generic namespace names could match library namespaces, and namespace-scope `static_assert`s were not credited.

## Decision

1. Each namespace of `golden.hpp` ends its comment with `// Fact: <fact>`, written by `golden.py`. The build checks read only `test/`: `tests.reference_markers` (the marker form) and `tests.golden_facts` (`tools/test_references.py golden`: a test case, or a `static_assert` outside test cases with a marker above it, that uses golden namespace X states X's fact).
2. A use is `golden::X` or a namespace alias of `golden` or `golden::X`, found in code with comments and literals blanked. `using` declarations and directives of golden names are rejected, and so is a use outside a test case or a `static_assert` (a helper takes the values as arguments). Facts match ignoring case and whitespace runs, with ∧ read as ^ and − as -.
3. The mapping from facts to CONVENTIONS items is the gate command `tools/test_references.py trace`, documented with its exact command in `docs/agent/records/convention-trace.md` and run by an agent at each merge into `develop` and each gate. The table's columns are Item, Fact, Implemented. It fails when an item is not in CONVENTIONS.md, a golden fact is missing from the table, Implemented is not yes or no, a fact is empty, or an implemented item has no test stating one of its facts.
4. The pointer pattern rejects: a governance document name (any case) followed by a provision token or "kind N"; the document names in capitals; "conventions specification"; "section N.N" and "item N.N"; decision, plan, note, review, or lesson followed by four digits; "lessons-learned"; "spike A" to "F"; "workbench"; `docs/agent`; "WU" plus digits; "DoD-N"; "M<digit> gate", "gate M<digit>", and the milestone forms; agent model names. A bare token such as C3, S2, V4, or M2 is mathematics and passes.
5. Each command has fixtures under `test/tools/test_references/` that must fail with every listed finding (`*_rejects`, `*_no_rows`) and fixtures that must pass (`*_accepts`), all in CTest under `tools;tools.test_references`.

## Consequences

- No build file or test names a path under `docs/agent/`. The traceability report and the implemented-item failure are only as current as the last gate run; a merge message quotes the gate command's output.
- A test file that writes `using namespace golden::...` fails `tests.golden_facts`; WU2 and WU3 tests that adopt golden values must use qualified names or aliases.
- The rejecting fixture `markers_bad.cpp` contains pointer forms on purpose ("decision 9999", "WU1", "STYLE S9", ...), as input the check must reject.
- Not handled: a golden value reached through a reference or pointer variable declared outside the test case, and macros that expand to golden names.

## Amendment, 2026-10-31

Review delta D6: the checks read only `*_test.cpp` files, so a helper header under `test/` that uses `golden::` values is not scanned. That, and not variables or macros declared at namespace scope (which are caught as uses outside a test case), is the real gap. The gate command is part of the merge checks of the workflow document (W5) and of the reviewer and auditor definitions. Review delta D2 (a convention item with no table row passes vacuously) is fixed before the first such item is implemented.
