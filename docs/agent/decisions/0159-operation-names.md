# 0159 The geometric product and antiproduct are named in full

- Status: accepted by the owner, 2026-11-01 ("proceed as recommended"); the CONVENTIONS 7.1 amendment (version 1.6) is ratified by the same ruling
- Date: 2026-11-01
- Decided by: the owner, on research note 0019 question 3; recorded by Claude (agent, Opus 5.5)

## Context

CONVENTIONS 7.1 named the geometric product `gp` and the antiproduct `antigp`: the only abbreviations among the named operations (`wedge`, `antiwedge`, `reverse`, `right_complement`, `bulk_dual`, ...). Full-word identifiers are read faster (note 0019, section 3.5), and a rename costs more after release. No public function of either name exists yet; the names occur as the enumerators `BinaryOperation::gp` and `BinaryOperation::antigp` in `plan.hpp`'s `detail` namespace, in about 170 places in the tests, and in test-only helper and golden-data names (`gp_terms`, `gp_tables`).

## Decision

1. The public names are `geometric_product` and `geometric_antiproduct`; `*` and `%` stay their operators. `wedge` and `antiwedge` are unchanged. CONVENTIONS 7.1 is amended (version 1.6), and the trace table's facts for 5.4 and 7.1, both unimplemented, are reworded with the new names.
2. The enumerators become `BinaryOperation::geometric_product` and `BinaryOperation::geometric_antiproduct` in WU9, the unit that creates the public functions, so that the kernel call and the function read alike, in one commit that migrates every use with no change of expected value. WU8 renames nothing in `plan.hpp`: it applies plans generically, and plan 0015, in flight, edits that header.
3. Test-only helper names and the golden header's namespaces keep their names; they are not part of the interface, and renaming the golden namespaces would regenerate `golden.hpp` for no reader's benefit.

## Consequences

The design study keeps `gp` in its text; it is a reference, not the interface.
