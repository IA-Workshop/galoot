# 0069 A new test marker is checked only by tests.reference_markers, which a unit's own selection leaves out

- Date: 2026-11-04
- Unit: WU11a (plan 0020)

## What happened

The WU11a implementer added a test with the marker `// Reference: definition: ...`. "definition" is not one of the kinds a marker may name (proof, identity, independent value, comparison, contract, regression guard), so `tests.reference_markers` failed from that commit on. The runs before each commit used the selection `core.dual` and the header checks, which do not include the marker check, so the failure stayed hidden through eight commits until the full suite ran on every preset. In the review fixes the same week, three new markers each held a semicolon, which the check reads as the start of a second source with no kind; running the check before the commit caught them.

## What to do

- Whenever a commit adds or changes a `// Reference:` marker, run `tests.reference_markers` (with `tests.citations` and `tests.golden_facts`, which are as cheap) before committing: `ctest --preset gcc-debug -R 'tests\.(reference_markers|citations|golden_facts)'`.
- Write a marker as one source: a kind from the list, a colon, and text without semicolons; join clauses with commas or "while".
