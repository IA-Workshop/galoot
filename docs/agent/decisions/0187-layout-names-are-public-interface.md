# 0187 The `*_layout` names are public interface

- Status: accepted (owner answer 2026-11-07, relayed by coordinator: "proceed with recommendations", M3 gate report owner decision 6)
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), records clerk role
- Bears on: decisions 0165 and 0171; the post-WU10 audit's owner item 7; plan 0020 R3; plan 0021 R4 and risk 5; C12

## Context

Each operation has a public alias for its result layout. Whether these stay public or move to `detail` was open since the post-WU10 audit. The M3 inventory lists sixteen: `geometric_product_layout`, `wedge_layout`, `geometric_antiproduct_layout`, `antiwedge_layout`, `right_complement_layout`, `left_complement_layout`, `sum_layout`, `bulk_layout`, `weight_layout`, `bulk_dual_layout`, `weight_dual_layout`, `left_bulk_dual_layout`, `left_weight_dual_layout`, `dot_layout`, `antidot_layout`, `project_layout`. Plan 0021 adds `compose_layout` and `antiproject_layout`.

## Options weighed

1. Confirm them as public interface (recommended). They spell a result type, Clang names the operation through them on the first line of a mismatch (`core.dual_compare_*`), and WU16's model aliases will spell types through them.
2. Move them to `detail`. The cost of the move grows with each unit.

## Decision

Option 1. The sixteen names above and plan 0021's `compose_layout` and `antiproject_layout` are public interface, covered by C12's rule on removal and incompatible change.

## Consequences

- Plan 0021 R4 and risk 5 lose their open question.
- A release record lists any later removal or incompatible change of these names (W13).
