# 0158 WU8 runs as two parts, and writes the messages of its own functions

- Status: accepted by the owner, 2026-11-01 ("proceed as recommended")
- Date: 2026-11-01
- Decided by: the owner, on plan 0016 questions 1 and 2; recorded by Claude (agent, Opus 5.5)

## Decision

1. WU8 runs as WU8a on `feature/core-mv-a` (the type and its construction, the scalar concepts, the set checks with the repeated-mask rejection, exact equality, named access, the test-only rational, and the blade-set changes of decision 0157), then WU8b on `feature/core-mv-b` (the kernel that applies plans and maps, the coefficient check, the conversions and `truncate` with checking mode, DoD-7 across five scalars, and the first assembly read), each with its own W5 review and merge.
2. WU8 writes the catch-alls of its own public functions and constructors (`get`, the count and conversion constructors, `truncate`). WU15 keeps `format.hpp` and the review of the DoD-19 catalogue as a whole; the products' catch-alls belong to the units that add the products.
3. The proposed reviewer checklist of research note 0019 section 7 is not ruled on; it stays a proposal.
