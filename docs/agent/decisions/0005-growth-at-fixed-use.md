# 0005 Reading of P2: growth must follow the algebra's structure

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner, accepting plan 0001 section 7 (D5). Recorded by Claude (agent).
- Owner amendment: agree, but don't block progress; any growth should be consistent with the properties of the algebra and its sparsity/symmetry pattern.

## Context

P2 as written forbids any growth with dimension at fixed use, which may be unmeetable.

## Decision

Growth at fixed use is acceptable when it is consistent with the properties of the algebra and its sparsity and symmetry pattern, for example the O(n^2) of the signature data itself. It is measured in compiler instruction counts over at least three sizes. The P7 report criterion is stated in advance as twice the historical spread. Growth is reported, and it does not block progress.

## Consequences

P2 needs an amendment sentence when it is next edited. Excess growth is filed as a defect, not a gate failure.
